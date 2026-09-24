// SPDX-License-Identifier: GPL-2.0+

#include <asm/global_data.h>
#include <command.h>
#include <malloc.h>
#include <memalign.h>
#include <net/httpd-json.h>
#include <time.h>
#include <console.h>
#include <dm/device.h>
#include <dm/ofnode.h>
#include <env.h>
#include <image.h>
#include <led.h>
#include <lmb.h>
#include <mapmem.h>
#include <mtd.h>
#include <u-boot/crc.h>
#include <ubi_uboot.h>
#include <net.h>
#include <net/httpd-page.h>
#include <net/httpd.h>
#include <net-lwip.h>
#include <part.h>
#include <stdio_dev.h>
#include <timer.h>
#include <version_string.h>
#include <linux/ctype.h>
#include <asm/unaligned.h>
#include <linux/err.h>
#include <linux/kernel.h>
#include <linux/libfdt.h>
#include <linux/mtd/mtd.h>
#include <lwip/apps/fs.h>
#include <lwip/apps/httpd.h>
#include <lwip/pbuf.h>

DECLARE_GLOBAL_DATA_PTR;

#define HTTPD_COMMAND_LEN	512
#define HTTPD_STORAGE_LEN	2048
#define HTTPD_STORAGE_NAME_LEN	128
#define HTTPD_OUTPUT_LEN	(64 * 1024)
#define HTTPD_ACTION_LEN	32
#define HTTPD_UBI_PART		"ubi"
#define HTTPD_FIRMWARE_VOLUME	"fit"
#define HTTPD_DATA_VOLUME	"rootfs_data"
#define HTTPD_STAGING_VOLUME	"fit.new"
#define HTTPD_RECOVERY_LAYOUT	"airoha,an758x-recovery-layout"
#define HTTPD_BOARD_DATA_MAX	4
#define HTTPD_DOWNLOAD_SEGMENT	(4 * 1024 * 1024)

enum http_post_kind {
	HTTP_POST_NONE,
	HTTP_POST_UPLOAD,
	HTTP_POST_FLASH_ALL_UPLOAD,
	HTTP_POST_TASK,
};

enum http_job_kind {
	HTTP_JOB_NONE,
	HTTP_JOB_COMMAND,
	HTTP_JOB_ACTION,
	HTTP_JOB_FIRMWARE,
	HTTP_JOB_STORAGE,
	HTTP_JOB_REBUILD,
	HTTP_JOB_ENV,
	HTTP_JOB_BL2,
	HTTP_JOB_FIP,
	HTTP_JOB_BOARD_DATA,
	HTTP_JOB_FLASH_ALL,
	HTTP_JOB_NAND_SCRUB,
};

enum http_storage_kind {
	HTTP_STORAGE_NONE,
	HTTP_STORAGE_UBI,
	HTTP_STORAGE_MTD,
};

enum http_storage_operation {
	HTTP_STORAGE_OP_NONE,
	HTTP_STORAGE_OP_READ,
	HTTP_STORAGE_OP_WRITE,
};

struct http_storage_request {
	enum http_storage_kind kind;
	enum http_storage_operation operation;
	char partition[HTTPD_STORAGE_NAME_LEN];
	char target[HTTPD_STORAGE_NAME_LEN];
	u64 offset;
	size_t size;
	bool dynamic;
};

enum http_job_state {
	HTTP_JOB_IDLE,
	HTTP_JOB_QUEUED,
	HTTP_JOB_RUNNING,
	HTTP_JOB_DONE,
};

struct http_post_state {
	void *connection;
	enum http_post_kind kind;
	ulong load_addr;
	size_t expected;
	size_t received;
	bool failed;
};

static struct http_post_state post_state;
static enum http_job_kind job_kind;
static enum http_job_state job_state;
static char command_buf[HTTPD_COMMAND_LEN];
static char action_buf[HTTPD_ACTION_LEN];
static char storage_buf[HTTPD_STORAGE_LEN];
static char output_buf[HTTPD_OUTPUT_LEN];
static char result_buf[HTTPD_OUTPUT_LEN * 6 + 1024];
static char info_buf[4096];
static char catalog_buf[16384];
static size_t output_len;
static int job_result;
static bool httpd_running;
static bool stop_requested;
static u32 upload_id, task_id;
static ulong buffer_addr;
static size_t buffer_size;
static phys_addr_t flash_all_alloc_addr;
static size_t flash_all_alloc_size;
static bool flash_all_upload_ready;
static unsigned int downloads, replies;
static bool backup_ready;
static ulong backup_at;
static char env_key[128], env_value[512];
static struct http_storage_request storage_request;
static struct http_storage_request backup_request;
static struct udevice *http_eth;
static struct netif *http_netif;
static ulong network_polled;
static const char *phase = "idle";

struct http_file {
	bool download;
	bool reply;
	struct mtd_info *mtd;
	char header[384];
	size_t header_len;
	char *allocation;
	void *cache;
	size_t cache_offset;
	size_t cache_size;
	size_t download_offset;
	size_t download_size;
};

static bool http_busy(void)
{
	return post_state.connection || job_state == HTTP_JOB_QUEUED ||
	       job_state == HTTP_JOB_RUNNING || backup_ready || downloads;
}

/* NAND operations yield between pages or eraseblocks to keep TCP alive. */
static void service_network(void)
{
	if (!http_netif || get_timer(network_polled) < 5)
		return;
	network_polled = get_timer(0);
	net_lwip_rx(http_eth, http_netif);
}

static int parse_task(void);
static int run_ubi_rebuild(void);
static int run_nand_scrub(void);
static int run_board_data_update(void);
static int open_download(struct fs_file *file, const char *name);
static bool upload_region_valid(ulong addr, size_t len);
static bool mtd_uses_pages(const struct mtd_info *mtd);
static struct mtd_info *whole_flash_mtd(void);
static void release_flash_all_upload(void);
static bool recovery_layout_present(void);
static int board_data_config(unsigned int index,
			     struct http_storage_request *request);
static int board_data_find(const char *target,
			   struct http_storage_request *request);
static void build_storage_catalog(void);

/* A dedicated sink keeps each task's output separate from the serial console. */
static void webconsole_putc(struct stdio_dev *dev, const char c)
{
	if (output_len + 1 < sizeof(output_buf)) {
		output_buf[output_len++] = c;
		output_buf[output_len] = '\0';
	}
}

static void webconsole_puts(struct stdio_dev *dev, const char *s)
{
	while (*s)
		webconsole_putc(dev, *s++);
}

static int webconsole_register(void)
{
	struct stdio_dev dev = {
		.flags = DEV_FLAGS_OUTPUT,
		.putc = webconsole_putc,
		.puts = webconsole_puts,
	};

	if (stdio_get_by_name("webconsole"))
		return 0;

	strlcpy(dev.name, "webconsole", sizeof(dev.name));
	return stdio_register(&dev);
}

static void fs_set_data(struct fs_file *file, const void *data, size_t len)
{
	file->data = data;
	file->len = min_t(size_t, len, INT_MAX);
	/* lwIP closes memory-backed responses when their index starts at EOF. */
	file->index = file->len;
	file->flags = 0;
	file->pextension = NULL;
}

static const char *job_state_name(void)
{
	switch (job_state) {
	case HTTP_JOB_QUEUED:
		return "queued";
	case HTTP_JOB_RUNNING:
		return "running";
	case HTTP_JOB_DONE:
		return "done";
	default:
		return "idle";
	}
}

static void build_result(void)
{
	struct http_json j = { result_buf, sizeof(result_buf), 0, false };

	json_printf(&j, "{\"id\":%u,\"state\":", task_id);
	json_string(&j, post_state.connection ? "receiving" :
			downloads ? "downloading" : job_state_name());
	json_printf(&j, ",\"busy\":%s,\"phase\":", http_busy() ? "true" : "false");
	json_string(&j, phase);
	json_printf(&j, ",\"code\":%d,\"upload_id\":%u,"
		    "\"download_id\":%u,\"download_size\":%zu,\"output\":",
		    job_result, upload_id, backup_ready ? task_id : 0,
		    backup_request.size);
	json_string(&j, output_buf);
	json_printf(&j, "}");
	if (j.overflow)
		strlcpy(result_buf, "{\"code\":-75}", sizeof(result_buf));
}

static void build_info(void)
{
	struct http_storage_request board_data;
	struct blk_desc *desc;
	struct mtd_info *mtd;
	const char *board = ofnode_read_string(ofnode_root(), "model");
	u64 flash_size = 0;
	unsigned int index;
	bool comma = false;

	if (!board)
		board = "unknown";
	if (CONFIG_IS_ENABLED(MTD)) {
		mtd_probe_devices();
		mtd_for_each_device(mtd) {
			if (!mtd_is_partition(mtd) && mtd->size > flash_size)
				flash_size = mtd->size;
		}
	}
	if (!flash_size && CONFIG_IS_ENABLED(MMC) &&
	    blk_get_device_by_str("mmc", "0", &desc) >= 0) {
		flash_size = (u64)desc->lba * desc->blksz;
	}

	{
		struct http_json j = { info_buf, sizeof(info_buf), 0, false };

		json_printf(&j, "{\"model\":"); json_string(&j, board);
		json_printf(&j, ",\"ram_bytes\":%llu,\"flash_bytes\":%llu",
			    (unsigned long long)gd->ram_size, (unsigned long long)flash_size);
		json_printf(&j, ",\"version\":"); json_string(&j, version_string);
		json_printf(&j, ",\"ubi\":%s,\"rebuild\":%s,\"board_data\":[",
			    CONFIG_IS_ENABLED(CMD_UBI) ? "true" : "false",
			    recovery_layout_present() ? "true" : "false");
		for (index = 0; index < HTTPD_BOARD_DATA_MAX; index++) {
			if (board_data_config(index, &board_data))
				break;
			if (comma)
				json_printf(&j, ",");
			comma = true;
			json_printf(&j, "{\"kind\":");
			json_string(&j, board_data.kind == HTTP_STORAGE_UBI ? "ubi" : "mtd");
			json_printf(&j, ",\"target\":");
			json_string(&j, board_data.target);
			json_printf(&j, ",\"size\":%zu}", board_data.size);
		}
		json_printf(&j, "]}");
	}

}

static int fs_json(struct fs_file *file, int code, const char *body, bool reply)
{
	struct http_file *owner = calloc(1, sizeof(*owner));
	size_t len = strlen(body);
	char *data;
	int head;

	if (!owner)
		return 0;
	data = malloc(len + 160);
	if (!data) { free(owner); return 0; }
	head = snprintf(data, 160, "HTTP/1.1 %d %s\r\nContent-Type: application/json\r\n"
			"Content-Length: %zu\r\nCache-Control: no-store\r\nConnection: close\r\n\r\n",
			code, code == 200 ? "OK" : code == 409 ? "Conflict" : "Error", len);
	memcpy(data + head, body, len);
	fs_set_data(file, data, head + len);
	file->flags = FS_FILE_FLAGS_HEADER_INCLUDED;
	owner->allocation = data;
	owner->reply = reply;
	file->pextension = owner;
	if (reply)
		replies++;
	return 1;
}

int fs_open_custom(struct fs_file *file, const char *name)
{
	if (!strcmp(name, "/") || !strcmp(name, "/index.html")) {
		fs_set_data(file, httpd_page, httpd_page_size);
		return 1;
	}
	if (!strcmp(name, "/api/device"))
		return fs_json(file, 200, info_buf, false);
	if (!strcmp(name, "/api/storage"))
		return fs_json(file, 200, catalog_buf, false);
	if (!strcmp(name, "/api/task")) {
		build_result();
		return fs_json(file, 200, result_buf, false);
	}
	if (!strcmp(name, "/api/env")) {
		char text[4096];
		struct http_json j = { text, sizeof(text), 0, false };
		const char *keys[] = { "httpd_ipaddr", "httpd_netmask", "httpd_dhcp_start", "httpd_dhcp_end", "ipaddr", "netmask", "serverip", "bootcmd" };
		int i;

		json_printf(&j, "{");
		for (i = 0; i < ARRAY_SIZE(keys); i++) {
			if (i) json_printf(&j, ",");
			json_string(&j, keys[i]); json_printf(&j, ":");
			json_string(&j, env_get(keys[i]));
		}
		json_printf(&j, "}");
		return fs_json(file, j.overflow ? 500 : 200, j.overflow ? "{}" : text, false);
	}
	if (!strcmp(name, "/api/reply")) {
		build_result();
		return fs_json(file, 200, result_buf, true);
	}
	if (!strcmp(name, "/api/busy"))
		return fs_json(file, 409, "{\"error\":\"busy\"}", false);
	if (!strcmp(name, "/api/error"))
		return fs_json(file, 400, "{\"error\":\"invalid request\"}", false);
	if (!strncmp(name, "/api/download/", 14))
		return open_download(file, name + 14);
	if (!strncmp(name, "/api/", 5))
		return fs_json(file, 404, "{\"error\":\"unknown API\"}", false);
	return 0;
}

void fs_close_custom(struct fs_file *file)
{
	struct http_file *owner = file->pextension;
	bool complete;

	if (!owner)
		return;
	if (owner->download) {
		complete = file->index >= file->len;
		if (owner->mtd)
			put_mtd_device(owner->mtd);
		downloads--;
		if (!complete || owner->download_offset + owner->download_size >=
		    backup_request.size) {
			backup_ready = false;
			phase = complete ? "done" : "error";
		} else {
			backup_at = get_timer(0);
		}
	}
	if (owner->reply)
		replies--;
	free(owner->allocation);
	free(owner->cache);
	free(owner);
}

int fs_read_custom(struct fs_file *file, char *buffer, int count)
{
	struct http_file *owner = file->pextension;
	size_t len, read = 0, offset, source_offset;
	int ret = 0;

	if (!owner || !owner->download || file->index >= file->len)
		return FS_READ_EOF;
	count = min(count, file->len - file->index);
	if (file->index < owner->header_len) {
		len = min_t(size_t, count, owner->header_len - file->index);
		memcpy(buffer, owner->header + file->index, len);
		file->index += len;
		return len;
	}
	offset = file->index - owner->header_len;
	source_offset = owner->download_offset + offset;
	len = count;
	if (owner->mtd) {
		u64 pos = backup_request.offset + source_offset;
		size_t block_left = owner->mtd->erasesize - pos % owner->mtd->erasesize;

		len = min(len, block_left);
		ret = mtd_block_isbad(owner->mtd, round_down(pos, owner->mtd->erasesize));
		if (ret > 0) {
			/* Physical main-area backups retain offsets across bad eraseblocks. */
			memset(buffer, 0xff, len);
			read = len;
			ret = 0;
		} else if (!ret) {
			ret = mtd_read(owner->mtd, pos, len, &read, buffer);
			if (ret == -EUCLEAN)
				ret = 0;
		}
	} else {
		if (!owner->cache_size || source_offset < owner->cache_offset ||
		    source_offset >= owner->cache_offset + owner->cache_size) {
			owner->cache_offset = source_offset;
			owner->cache_size = min_t(size_t, 32768,
						  owner->download_size - offset);
			ret = CONFIG_IS_ENABLED(CMD_UBI) ?
				ubi_volume_read_quiet(backup_request.target, owner->cache,
						      source_offset,
						      owner->cache_size) : -ENOSYS;
		}
		if (!ret) {
			len = min(len, owner->cache_offset + owner->cache_size -
				  source_offset);
			memcpy(buffer, owner->cache + source_offset -
			       owner->cache_offset, len);
			read = len;
		}
	}
	if (ret || read != len) {
		job_result = -EIO;
		strlcpy(output_buf, "Backup read failed", sizeof(output_buf));
		return FS_READ_EOF;
	}
	file->index += len;
	return len;
}

static int open_download(struct fs_file *file, const char *name)
{
	struct http_file *owner;
	char *end;
	u64 offset, length;
	ulong id;

	id = simple_strtoul(name, &end, 10);
	if (*end != '/')
		return fs_json(file, 400, "{\"error\":\"invalid range\"}", false);
	offset = simple_strtoull(end + 1, &end, 10);
	if (*end != '/')
		return fs_json(file, 400, "{\"error\":\"invalid range\"}", false);
	length = simple_strtoull(end + 1, &end, 10);
	if (*end || id != task_id || !backup_ready || downloads)
		return fs_json(file, 409, "{\"error\":\"backup expired\"}", false);
	if (!length || length > HTTPD_DOWNLOAD_SEGMENT || offset > SIZE_MAX ||
	    length > SIZE_MAX || offset > backup_request.size ||
	    length > backup_request.size - offset)
		return fs_json(file, 400, "{\"error\":\"invalid range\"}", false);
	owner = calloc(1, sizeof(*owner));
	if (!owner)
		return 0;
	if (backup_request.kind == HTTP_STORAGE_MTD) {
		owner->mtd = get_mtd_device_nm(backup_request.target);
		if (IS_ERR(owner->mtd)) { free(owner); return 0; }
	} else {
		owner->cache = malloc_cache_aligned(32768);
		if (!owner->cache) { free(owner); return 0; }
	}
	owner->header_len = snprintf(owner->header, sizeof(owner->header),
		"HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\n"
		"Content-Length: %zu\r\nContent-Disposition: attachment; filename=\"%s%s.bin\"\r\n"
		"Cache-Control: no-store\r\nConnection: close\r\n\r\n",
		(size_t)length, backup_request.target, owner->mtd ? "-main" : "");
	owner->download = true;
	owner->download_offset = offset;
	owner->download_size = length;
	fs_set_data(file, NULL, length + owner->header_len);
	file->index = 0;
	file->flags = FS_FILE_FLAGS_HEADER_INCLUDED;
	file->pextension = owner;
	downloads++;
	phase = "download";
	return 1;
}

static struct mtd_info *whole_flash_mtd(void)
{
	struct mtd_info *mtd, *whole = NULL;

	mtd_probe_devices();
	mtd_for_each_device(mtd) {
		if (!mtd_is_partition(mtd) && mtd_uses_pages(mtd) &&
		    (!whole || mtd->size > whole->size))
			whole = mtd;
	}

	return whole ? get_mtd_device_nm(whole->name) : ERR_PTR(-ENODEV);
}

static void release_flash_all_upload(void)
{
	if (flash_all_alloc_size && buffer_addr == flash_all_alloc_addr)
		buffer_size = 0;
	if (flash_all_alloc_size)
		lmb_free(flash_all_alloc_addr, flash_all_alloc_size, LMB_NONE);
	flash_all_alloc_addr = 0;
	flash_all_alloc_size = 0;
	flash_all_upload_ready = false;
}

static bool upload_region_valid(ulong addr, size_t len)
{
	phys_addr_t ram_end = gd->ram_base + gd->ram_size;
	/* The initial stack boundary keeps uploads below relocated U-Boot state. */
	phys_addr_t safe_end = min_t(phys_addr_t, ram_end, gd->start_addr_sp);
	phys_addr_t end = (phys_addr_t)addr + len;

	return len && end >= addr && addr >= gd->ram_base && end <= safe_end;
}

err_t httpd_post_begin(void *connection, const char *uri,
		       const char *http_request, u16_t http_request_len,
		       int content_len, char *response_uri,
		       u16_t response_uri_len, u8_t *post_auto_wnd)
{
	ulong addr = env_get_ulong("loadaddr", 16, CONFIG_SYS_LOAD_ADDR);
	struct mtd_info *mtd;
	phys_addr_t allocated, safe_end;

	if (http_busy()) {
		strlcpy(response_uri, "/api/busy", response_uri_len);
		return ERR_ARG;
	}
	if (content_len <= 0)
		goto reject;
	if (!strcmp(uri, "/api/upload") ||
	    !strcmp(uri, "/api/upload-all-flash")) {
		release_flash_all_upload();
		if (!strcmp(uri, "/api/upload-all-flash")) {
			mtd = whole_flash_mtd();
			if (IS_ERR(mtd))
				goto reject;
			if (content_len > mtd->size) {
				put_mtd_device(mtd);
				goto reject;
			}
			put_mtd_device(mtd);
			safe_end = min_t(phys_addr_t, gd->ram_base + gd->ram_size,
					 gd->start_addr_sp);
			allocated = safe_end;
			/* LMB excludes the running bootloader, stack and reserved DRAM. */
			if (lmb_alloc_mem(LMB_MEM_ALLOC_MAX, 4096, &allocated,
					  content_len, LMB_NONE))
				goto reject;
			if (!upload_region_valid(allocated, content_len)) {
				lmb_free(allocated, content_len, LMB_NONE);
				goto reject;
			}
			addr = allocated;
			flash_all_alloc_addr = allocated;
			flash_all_alloc_size = content_len;
			post_state.kind = HTTP_POST_FLASH_ALL_UPLOAD;
		} else {
			if (!upload_region_valid(addr, content_len))
				goto reject;
			post_state.kind = HTTP_POST_UPLOAD;
		}
		buffer_size = 0;
		env_set_hex("filesize", 0);
		phase = "upload";
	} else if (!strcmp(uri, "/api/task") && content_len < sizeof(storage_buf)) {
		post_state.kind = HTTP_POST_TASK;
	} else {
		goto reject;
	}
	post_state.connection = connection;
	post_state.expected = content_len;
	post_state.received = 0;
	post_state.failed = false;
	post_state.load_addr = addr;
	*post_auto_wnd = 1;
	return ERR_OK;
reject:
	strlcpy(response_uri, "/api/error", response_uri_len);
	return ERR_ARG;
}

err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
	size_t len = p->tot_len;

	if (connection != post_state.connection) {
		pbuf_free(p);
		return ERR_ARG;
	}
	if (post_state.failed || len > post_state.expected - post_state.received) {
		post_state.failed = true;
		pbuf_free(p);
		return ERR_ARG;
	}
	if (post_state.kind == HTTP_POST_UPLOAD ||
	    post_state.kind == HTTP_POST_FLASH_ALL_UPLOAD) {
		void *dst = map_sysmem(post_state.load_addr + post_state.received, len);

		pbuf_copy_partial(p, dst, len, 0);
		unmap_sysmem(dst);
	} else {
		pbuf_copy_partial(p, storage_buf + post_state.received, len, 0);
	}
	post_state.received += len;
	pbuf_free(p);
	return ERR_OK;
}

void httpd_post_finished(void *connection, char *response_uri, u16_t response_uri_len)
{
	int ret = -EINVAL;

	if (connection != post_state.connection) {
		strlcpy(response_uri, "/api/error", response_uri_len);
		return;
	}
	if (!post_state.failed && post_state.received == post_state.expected) {
		if (post_state.kind == HTTP_POST_UPLOAD ||
		    post_state.kind == HTTP_POST_FLASH_ALL_UPLOAD) {
			job_result = 0;
			job_state = HTTP_JOB_IDLE;
			output_len = 0;
			output_buf[0] = 0;
			buffer_addr = post_state.load_addr;
			buffer_size = post_state.received;
			flash_all_upload_ready =
				post_state.kind == HTTP_POST_FLASH_ALL_UPLOAD;
			upload_id++;
			if (!flash_all_upload_ready) {
				env_set_hex("fileaddr", buffer_addr);
				env_set_hex("filesize", buffer_size);
			}
			phase = "ready";
			ret = 0;
		} else {
			storage_buf[post_state.received] = 0;
			ret = parse_task();
			if (!ret) {
				task_id++;
				job_state = HTTP_JOB_QUEUED;
				output_len = 0;
				output_buf[0] = 0;
				job_result = 0;
				phase = "queued";
			}
		}
	}
	if (ret && post_state.kind == HTTP_POST_FLASH_ALL_UPLOAD)
		release_flash_all_upload();
	strlcpy(response_uri, ret ? "/api/error" : "/api/reply", response_uri_len);
	memset(&post_state, 0, sizeof(post_state));
}

static bool storage_name_valid(const char *name)
{
	const char *p;

	if (!name || !*name)
		return false;

	for (p = name; *p; p++) {
		if (!isalnum((unsigned char)*p) && *p != '_' && *p != '-' &&
		    *p != '.' && *p != ':' && *p != '@' && *p != ',' &&
		    *p != '+')
			return false;
	}

	return p - name < 128;
}

static ofnode recovery_layout_node(void)
{
	return ofnode_by_compatible(ofnode_null(), HTTPD_RECOVERY_LAYOUT);
}

static bool recovery_layout_present(void)
{
	return ofnode_valid(recovery_layout_node());
}

static int board_data_config(unsigned int index,
			     struct http_storage_request *request)
{
	ofnode layout = recovery_layout_node();
	ofnode volume;
	const char *name;
	const char *type;
	u32 size;
	unsigned int current = 0;

	if (!ofnode_valid(layout))
		return -ENOENT;
	ofnode_for_each_subnode(volume, layout) {
		if (current++ != index)
			continue;
		name = ofnode_read_string(volume, "volume-name");
		type = ofnode_read_string(volume, "volume-type");
		if (!name || !type || ofnode_read_u32(volume, "volume-size", &size) ||
		    !size || !storage_name_valid(name))
			return -EINVAL;

		memset(request, 0, sizeof(*request));
		request->kind = HTTP_STORAGE_UBI;
		request->operation = HTTP_STORAGE_OP_WRITE;
		request->size = size;
		if (!strcmp(type, "dynamic"))
			request->dynamic = true;
		else if (strcmp(type, "static"))
			return -EINVAL;
		strlcpy(request->target, name, sizeof(request->target));
		strlcpy(request->partition, HTTPD_UBI_PART,
			sizeof(request->partition));
		return 0;
	}

	return -ENOENT;
}

static int board_data_find(const char *target,
			   struct http_storage_request *request)
{
	unsigned int index;

	for (index = 0; index < HTTPD_BOARD_DATA_MAX; index++) {
		if (board_data_config(index, request))
			break;
		if (!strcmp(request->target, target))
			return 0;
	}

	return -ENOENT;
}

static void build_storage_catalog(void)
{
	struct http_json j = { catalog_buf, sizeof(catalog_buf), 0, false };
	struct mtd_info *mtd;
	struct ubi_device *ubi = CONFIG_IS_ENABLED(CMD_UBI) ? ubi_devices[0] : NULL;
	const char *part = HTTPD_UBI_PART;
	bool comma = false;
	int i;

	json_printf(&j, "{\"targets\":[");
	mtd_for_each_device(mtd) {
		if (!storage_name_valid(mtd->name)) continue;
		if (comma) json_printf(&j, ",");
		comma = true;
		json_printf(&j, "{\"kind\":\"mtd\",\"name\":"); json_string(&j, mtd->name);
		json_printf(&j, ",\"size\":%llu,\"whole\":%s}", (unsigned long long)mtd->size,
			    mtd_is_partition(mtd) ? "false" : "true");
	}
	if (ubi && !strcmp(ubi->mtd->name, part)) {
		for (i = 0; i < ubi->vtbl_slots; i++) {
			struct ubi_volume *v = ubi->volumes[i];

			if (!v || !storage_name_valid(v->name)) continue;
			if (comma) json_printf(&j, ",");
			comma = true;
			json_printf(&j, "{\"kind\":\"ubi\",\"name\":"); json_string(&j, v->name);
			json_printf(&j, ",\"size\":%llu}", (unsigned long long)v->used_bytes);
		}
	}
	json_printf(&j, "]}");
	if (j.overflow)
		strlcpy(catalog_buf, "{\"targets\":[],\"error\":\"catalog too large\"}", sizeof(catalog_buf));
}

static int parse_task(void)
{
	struct http_json_request r;
	char op[32], kind[12], target[HTTPD_STORAGE_NAME_LEN];
	u64 id, size, offset;

	if (json_request_init(&r, storage_buf) ||
	    json_text(&r, "operation", op, sizeof(op)) ||
	    json_uint(&r, "upload_id", &id))
		return -EINVAL;
	if (flash_all_upload_ready && strcmp(op, "flash-all"))
		return -EBUSY;
	memset(&storage_request, 0, sizeof(storage_request));
	if (!strcmp(op, "flash") || !strcmp(op, "flash-all") ||
	    !strcmp(op, "write") ||
	    !strcmp(op, "boot-upload") || !strcmp(op, "flash-bl2") ||
	    !strcmp(op, "flash-uboot") || !strcmp(op, "flash-board-data")) {
		if (!buffer_size || id != upload_id)
			return -EINVAL;
		if (strcmp(op, "flash-all")) {
			env_set_hex("loadaddr", buffer_addr);
			env_set_hex("filesize", buffer_size);
		}
	}
	if (!strcmp(op, "flash")) {
		job_kind = HTTP_JOB_FIRMWARE;
	} else if (!strcmp(op, "flash-all")) {
		if (!flash_all_upload_ready ||
		    buffer_addr != flash_all_alloc_addr ||
		    buffer_size != flash_all_alloc_size)
			return -EINVAL;
		job_kind = HTTP_JOB_FLASH_ALL;
	} else if (!strcmp(op, "flash-bl2")) {
		job_kind = HTTP_JOB_BL2;
	} else if (!strcmp(op, "flash-uboot")) {
		job_kind = HTTP_JOB_FIP;
	} else if (!strcmp(op, "flash-board-data")) {
		if (json_text(&r, "target", target, sizeof(target)) ||
		    board_data_find(target, &storage_request) ||
		    buffer_size != storage_request.size)
			return -EINVAL;
		job_kind = HTTP_JOB_BOARD_DATA;
	} else if (!strcmp(op, "ubi-rebuild")) {
		if (!recovery_layout_present()) return -EINVAL;
		job_kind = HTTP_JOB_REBUILD;
	} else if (!strcmp(op, "nand-scrub")) {
		job_kind = HTTP_JOB_NAND_SCRUB;
	} else if (!strcmp(op, "command")) {
		if (json_text(&r, "command", command_buf, sizeof(command_buf)) || !command_buf[0])
			return -EINVAL;
		job_kind = HTTP_JOB_COMMAND;
	} else if (!strcmp(op, "env-set")) {
		if (json_text(&r, "name", env_key, sizeof(env_key)) || !storage_name_valid(env_key) ||
		    json_text(&r, "value", env_value, sizeof(env_value)))
			return -EINVAL;
		job_kind = HTTP_JOB_ENV;
	} else if (!strcmp(op, "boot-production") || !strcmp(op, "boot-upload") ||
		   !strcmp(op, "reset") || !strcmp(op, "exit")) {
		strlcpy(action_buf, op, sizeof(action_buf));
		job_kind = HTTP_JOB_ACTION;
	} else if (!strcmp(op, "backup") || !strcmp(op, "write")) {
		if (json_text(&r, "kind", kind, sizeof(kind)) ||
		    json_text(&r, "target", storage_request.target, sizeof(storage_request.target)) ||
		    !storage_name_valid(storage_request.target) ||
		    json_uint(&r, "size", &size) || size > SIZE_MAX ||
		    json_uint(&r, "offset", &offset))
			return -EINVAL;
		storage_request.kind = !strcmp(kind, "ubi") ? HTTP_STORAGE_UBI :
				       !strcmp(kind, "mtd") ? HTTP_STORAGE_MTD : HTTP_STORAGE_NONE;
		if (!storage_request.kind)
			return -EINVAL;
		strlcpy(storage_request.partition, HTTPD_UBI_PART,
			sizeof(storage_request.partition));
		storage_request.operation = !strcmp(op, "backup") ?
					    HTTP_STORAGE_OP_READ : HTTP_STORAGE_OP_WRITE;
		storage_request.offset = offset;
		storage_request.size = !strcmp(op, "write") ? buffer_size : size;
		if (storage_request.kind == HTTP_STORAGE_UBI && offset)
			return -EINVAL;
		job_kind = HTTP_JOB_STORAGE;
	} else {
		return -EINVAL;
	}
	return 0;
}

static bool mtd_uses_pages(const struct mtd_info *mtd)
{
	return mtd->type == MTD_NANDFLASH || mtd->type == MTD_MLCNANDFLASH;
}

static int storage_mtd_transfer(struct mtd_info *mtd, u64 offset,
				void *buffer, size_t size, bool read)
{
	u8 *data = buffer;
	u64 physical = offset;
	size_t remaining = size;
	bool warned_bitflips = false;
	bool paged = mtd_uses_pages(mtd);

	while (remaining) {
		size_t transferred = 0;
		size_t chunk = paged ? min_t(size_t, remaining, mtd->writesize) :
			remaining;
		int ret;

		/* NAND offsets advance across factory bad blocks while RAM stays packed. */
		if (paged && (physical == offset ||
			      !(physical % mtd->erasesize))) {
			ret = mtd_block_isbad(mtd, physical);
			if (ret < 0)
				return ret;
			if (ret > 0) {
				printf("Skipping bad block at 0x%llx.\n", physical);
				physical = round_down(physical, mtd->erasesize) +
					mtd->erasesize;
				continue;
			}
		}
		if (physical > mtd->size || chunk > mtd->size - physical)
			return -ENOSPC;

		if (read)
			ret = mtd_read(mtd, physical, chunk, &transferred, data);
		else
			ret = mtd_write(mtd, physical, chunk, &transferred, data);
		if (ret == -EUCLEAN && transferred == chunk) {
			if (!warned_bitflips)
				puts("Corrected bitflips reached the MTD threshold.\n");
			warned_bitflips = true;
			ret = 0;
		}
		if (ret)
			return ret;
		if (transferred != chunk)
			return -EIO;

		physical += chunk;
		data += chunk;
		remaining -= chunk;
		service_network();
	}

	return 0;
}

static int storage_mtd_erase(struct mtd_info *mtd, u64 offset, size_t size)
{
	struct erase_info erase = {
		.mtd = mtd,
		.addr = offset,
		.len = mtd->erasesize,
	};
	u64 end = offset + size;
	int ret;

	while (erase.addr < end) {
		ret = mtd_block_isbad(mtd, erase.addr);
		if (ret < 0)
			return ret;
		if (ret > 0) {
			printf("Skipping bad block at 0x%llx.\n", erase.addr);
			erase.addr += mtd->erasesize;
			continue;
		}

		ret = mtd_erase(mtd, &erase);
		if (ret)
			return ret;
		erase.addr += mtd->erasesize;
		service_network();
	}

	return 0;
}

static int run_flash_all(void)
{
	struct mtd_info *mtd;
	const u8 *uploaded;
	u8 *block = NULL, *verify = NULL;
	size_t offset = 0, chunk, written;
	unsigned int skipped = 0;
	int ret = -EINVAL;

	if (!flash_all_upload_ready || !buffer_size ||
	    buffer_addr != flash_all_alloc_addr ||
	    buffer_size != flash_all_alloc_size ||
	    !upload_region_valid(buffer_addr, buffer_size))
		return CMD_RET_FAILURE;

	mtd = whole_flash_mtd();
	if (IS_ERR(mtd))
		return CMD_RET_FAILURE;
	if (!mtd->erasesize || !mtd->writesize ||
	    mtd->erasesize % mtd->writesize ||
	    mtd->size % mtd->erasesize || buffer_size > mtd->size) {
		puts("Whole Flash image exceeds the NAND geometry.\n");
		goto out_put_mtd;
	}

	block = malloc_cache_aligned(mtd->erasesize);
	verify = malloc_cache_aligned(mtd->erasesize);
	if (!block || !verify) {
		ret = -ENOMEM;
		goto out_put_mtd;
	}
	uploaded = map_sysmem(buffer_addr, buffer_size);
	if (CONFIG_IS_ENABLED(CMD_UBI) && ubi_devices[0]) {
		ret = ubi_detach();
		if (ret)
			goto out_unmap;
	}

	/* Physical offsets match whole-chip backups, including bad-block gaps. */
	for (offset = 0; offset < buffer_size; offset += mtd->erasesize) {
		struct erase_info erase = {
			.mtd = mtd,
			.addr = offset,
			.len = mtd->erasesize,
		};

		ret = mtd_block_isbad(mtd, offset);
		if (ret < 0)
			goto out_unmap;
		if (ret > 0) {
			skipped++;
			service_network();
			continue;
		}

		chunk = min_t(size_t, mtd->erasesize, buffer_size - offset);
		memset(block, 0xff, mtd->erasesize);
		memcpy(block, uploaded + offset, chunk);
		phase = "erase";
		ret = mtd_erase(mtd, &erase);
		if (ret)
			goto out_unmap;
		phase = "write";
		ret = mtd_write(mtd, offset, mtd->erasesize, &written, block);
		if (ret || written != mtd->erasesize) {
			ret = ret ? ret : -EIO;
			goto out_unmap;
		}
		phase = "verify";
		ret = mtd_read(mtd, offset, mtd->erasesize, &written, verify);
		if (ret == -EUCLEAN && written == mtd->erasesize)
			ret = 0;
		if (ret || written != mtd->erasesize ||
		    memcmp(block, verify, mtd->erasesize)) {
			ret = ret ? ret : -EIO;
			goto out_unmap;
		}
		service_network();
	}
	printf("Restored %zu bytes from offset 0; skipped %u bad blocks.\n",
	       buffer_size, skipped);
	ret = 0;
out_unmap:
	if (ret)
		printf("Whole Flash restore failed at 0x%zx (%d).\n", offset, ret);
	unmap_sysmem(uploaded);
out_put_mtd:
	free(verify);
	free(block);
	put_mtd_device(mtd);
	return ret ? CMD_RET_FAILURE : CMD_RET_SUCCESS;
}

static int run_storage_job(void)
{
	struct http_storage_request request = storage_request;
	ulong load_addr = env_get_ulong("loadaddr", 16, CONFIG_SYS_LOAD_ADDR);
	struct mtd_info *mtd;
	void *buffer;
	size_t used_bytes;
	size_t reserved_bytes;
	u32 expected_crc;
	u32 actual_crc;
	int ret;


	if (request.kind == HTTP_STORAGE_UBI) {
		if (!CONFIG_IS_ENABLED(CMD_UBI))
			return CMD_RET_FAILURE;
		ret = ubi_part(request.partition, NULL);
		if (ret)
			return CMD_RET_FAILURE;

		if (request.operation == HTTP_STORAGE_OP_READ) {
			if (!request.size) {
				ret = ubi_volume_get_size(request.target, &used_bytes,
							  NULL);
				if (ret)
					return CMD_RET_FAILURE;
				request.size = used_bytes;
			}
			backup_request = request;
			backup_ready = true;
			backup_at = get_timer(0);
			phase = "backup-ready";
			return CMD_RET_SUCCESS;
		}

		if (!upload_region_valid(load_addr, request.size))
			return CMD_RET_FAILURE;
		ret = ubi_volume_get_size(request.target, NULL, &reserved_bytes);
		if (ret || request.size > reserved_bytes) {
			printf("Volume '%s' has %zu bytes available; write needs %zu.\n",
			       request.target, ret ? 0 : reserved_bytes, request.size);
			return CMD_RET_FAILURE;
		}

		buffer = map_sysmem(load_addr, request.size);
		expected_crc = crc32_wd(0, buffer, request.size, CHUNKSZ_CRC32);
		ret = ubi_volume_write(request.target, buffer, 0, request.size);
		if (!ret)
			ret = ubi_volume_read(request.target, buffer, 0, request.size);
		if (!ret)
			actual_crc = crc32_wd(0, buffer, request.size, CHUNKSZ_CRC32);
		unmap_sysmem(buffer);
		if (ret)
			return CMD_RET_FAILURE;
		if (actual_crc != expected_crc) {
			printf("UBI readback CRC mismatch: %08x != %08x.\n",
			       actual_crc, expected_crc);
			return CMD_RET_FAILURE;
		}

		printf("Wrote and verified %zu bytes in UBI volume '%s' (CRC %08x).\n",
		       request.size, request.target, actual_crc);
		return CMD_RET_SUCCESS;
	}

	mtd_probe_devices();
	mtd = get_mtd_device_nm(request.target);
	if (IS_ERR(mtd)) {
		printf("MTD device '%s' was not found.\n", request.target);
		return CMD_RET_FAILURE;
	}
	if (request.operation == HTTP_STORAGE_OP_READ && !request.size)
		request.size = mtd->size - min_t(u64, request.offset, mtd->size);
	if (request.offset > mtd->size || request.size > mtd->size - request.offset) {
		puts("MTD range exceeds the selected device.\n");
		ret = CMD_RET_FAILURE;
		goto out_put_mtd;
	}
	if (mtd_uses_pages(mtd) &&
	    ((request.offset % mtd->writesize) ||
	     (request.size % mtd->writesize))) {
		printf("NAND ranges use 0x%x-byte page alignment.\n",
		       mtd->writesize);
		ret = CMD_RET_FAILURE;
		goto out_put_mtd;
	}
	if (request.operation == HTTP_STORAGE_OP_READ) {
		backup_request = request;
		backup_ready = true;
		backup_at = get_timer(0);
		phase = "backup-ready";
		ret = 0;
		goto out_put_mtd;
	}
	if (!upload_region_valid(load_addr, request.size)) {
		ret = CMD_RET_FAILURE;
		goto out_put_mtd;
	}

	buffer = map_sysmem(load_addr, request.size);
	if (request.offset % mtd->erasesize || request.size % mtd->erasesize) {
		puts("MTD writes require an eraseblock-aligned image and offset.\n");
		ret = -EINVAL;
		goto out_unmap;
	}
	phase = "erase";
	ret = storage_mtd_erase(mtd, request.offset, request.size);
	if (ret)
		goto out_unmap;
	phase = "write";
	expected_crc = crc32_wd(0, buffer, request.size, CHUNKSZ_CRC32);
	ret = storage_mtd_transfer(mtd, request.offset, buffer,
				   request.size, false);
	if (!ret)
		ret = storage_mtd_transfer(mtd, request.offset, buffer,
					   request.size, true);
	if (!ret) {
		actual_crc = crc32_wd(0, buffer, request.size, CHUNKSZ_CRC32);
		if (actual_crc != expected_crc) {
			printf("MTD readback CRC mismatch: %08x != %08x.\n",
			       actual_crc, expected_crc);
			ret = -EIO;
		}
	}
	if (!ret)
		printf("Wrote and verified %zu bytes in MTD '%s' at 0x%llx (CRC %08x).\n",
		       request.size, request.target, request.offset, actual_crc);
out_unmap:
	unmap_sysmem(buffer);

out_put_mtd:
	put_mtd_device(mtd);
	return ret ? CMD_RET_FAILURE : CMD_RET_SUCCESS;
}


/* FIP UUIDs identify BL2 and the BL31/BL33 pair before any erase operation. */
static bool boot_fip_valid(const u8 *data, size_t size, bool bl2)
{
	static const u8 uuids[][16] = {
		{0x5f,0xf9,0xec,0x0b,0x4d,0x22,0x3e,0x4d,0xa5,0x44,0xc3,0x9d,0x81,0xc7,0x3f,0x0a},
		{0x47,0xd4,0x08,0x6d,0x4c,0xfe,0x98,0x46,0x9b,0x95,0x29,0x50,0xcb,0xbd,0x5a,0x00},
		{0xd6,0xd0,0xee,0xa7,0xfc,0xea,0xd5,0x4b,0x97,0x82,0x99,0x34,0xf2,0x34,0xb6,0xe4},
	};
	static const u8 zero[16];
	size_t pos, first = size;
	u64 end = 0;
	unsigned int found = 0;
	int i;

	if (size < 56 || get_unaligned_le32(data) != 0xaa640001)
		return false;
	for (pos = 16; pos <= size - 40; pos += 40) {
		u64 offset = get_unaligned_le64(data + pos + 16);
		u64 len = get_unaligned_le64(data + pos + 24);

		if (!memcmp(data + pos, zero, 16))
			return first >= pos + 40 && found == (bl2 ? 1 : 6);
		if (!len || offset < end || offset > size || len > size - offset)
			return false;
		first = min_t(size_t, first, offset);
		end = offset + len;
		for (i = 0; i < ARRAY_SIZE(uuids); i++) {
			if (memcmp(data + pos, uuids[i], 16))
				continue;
			if (found & BIT(i))
				return false;
			found |= BIT(i);
		}
	}
	return false;
}

static bool prepare_bl2_image(u8 *image, size_t block_size,
			      const u8 *data, size_t size)
{
	if (block_size <= 0x800)
		return false;
	if (size == block_size && boot_fip_valid(data + 0x800, size - 0x800, true)) {
		memcpy(image, data, size);
		return true;
	}
	if (size <= block_size - 0x800 && boot_fip_valid(data, size, true)) {
		memset(image, 0xff, block_size);
		memcpy(image + 0x800, data, size);
		return true;
	}
	return false;
}

static int run_bootloader_update(bool bl2)
{
	const char *part = HTTPD_UBI_PART;
	struct mtd_info *mtd = NULL;
	void *uploaded, *image = NULL, *verify = NULL;
	size_t length = buffer_size, capacity;
	int ret = -EINVAL;

	if (!upload_region_valid(buffer_addr, length))
		return -EINVAL;
	uploaded = map_sysmem(buffer_addr, length);
	if (!bl2 && !boot_fip_valid(uploaded, length, false)) {
		puts("Expected a BL31 + U-Boot FIP.\n");
		goto out;
	}
	if (bl2) {
		mtd_probe_devices();
		mtd = get_mtd_device_nm("bl2");
		if (IS_ERR(mtd)) {
			ret = PTR_ERR(mtd);
			mtd = NULL;
			goto out;
		}
		/* The first eraseblock contains the preloader at physical offset 0x800. */
		if (!mtd_is_partition(mtd) || mtd->offset || mtd->parent->parent ||
		    !mtd_uses_pages(mtd) || mtd->size != mtd->erasesize ||
		    mtd->erasesize <= 0x800)
			goto out;
		ret = mtd_block_isbad(mtd, 0);
		if (ret) {
			if (ret > 0) ret = -EIO;
			goto out;
		}
		length = mtd->erasesize;
		image = malloc_cache_aligned(length);
		verify = malloc_cache_aligned(length);
		if (!image || !verify) { ret = -ENOMEM; goto out; }
		if (!prepare_bl2_image(image, length, uploaded, buffer_size)) {
			puts("Expected a BL2 preloader FIP or a full main-area firstblock with its FIP at 0x800.\n");
			ret = -EINVAL;
			goto out;
		}
		printf("BL2 input: %s (%zu bytes).\n",
		       buffer_size == length ? "firstblock" : "preloader FIP", buffer_size);
		phase = "erase";
		ret = storage_mtd_erase(mtd, 0, length);
		if (ret) goto out;
		phase = "write";
		ret = storage_mtd_transfer(mtd, 0, image, length, false);
		if (ret) goto out;
		phase = "verify";
		ret = storage_mtd_transfer(mtd, 0, verify, length, true);
	} else {
		if (!CONFIG_IS_ENABLED(CMD_UBI)) { ret = -ENOSYS; goto out; }
		ret = ubi_part((char *)part, NULL);
		if (ret) goto out;
		ret = ubi_volume_get_size("fip", NULL, &capacity);
		if (ret || length > capacity) { ret = ret ? ret : -EFBIG; goto out; }
		verify = malloc_cache_aligned(length);
		if (!verify) { ret = -ENOMEM; goto out; }
		phase = "write";
		ret = ubi_volume_write("fip", uploaded, 0, length);
		if (ret) goto out;
		phase = "verify";
		ret = ubi_volume_read("fip", verify, 0, length);
	}
	if (!ret && memcmp(bl2 ? image : uploaded, verify, length))
		ret = -EIO;
	if (!ret)
		printf("%s written and verified (%zu bytes).\n", bl2 ? "BL2" : "FIP", length);
out:
	if (ret)
		printf("%s update failed (%d).\n", bl2 ? "BL2" : "FIP", ret);
	if (mtd) put_mtd_device(mtd);
	free(verify);
	free(image);
	unmap_sysmem(uploaded);
	return ret;
}

static bool range_contains(const void *base, size_t total,
			   const void *data, size_t len)
{
	uintptr_t start = (uintptr_t)base;
	uintptr_t end = start + total;
	uintptr_t data_start = (uintptr_t)data;
	uintptr_t data_end = data_start + len;

	return end >= start && data_end >= data_start && data_start >= start &&
	       data_end <= end;
}

static int validate_firmware_fit(ulong load_addr, size_t image_size)
{
	const char *compatible;
	const void *image_data;
	const void *fit;
	size_t data_size;
	int compatible_len;
	int images_noffset;
	int image_noffset;
	int config_noffset;
	int fdt_noffset;
	int loadable_count;
	int loadable_index;
	u8 image_type;
	bool has_rootfs = false;
	int ret = CMD_RET_FAILURE;

	fit = map_sysmem(load_addr, image_size);
	if (fit_check_format(fit, image_size)) {
		puts("Firmware image has an invalid FIT structure.\n");
		goto out;
	}

	/* External FIT payload offsets must remain inside the uploaded buffer. */
	images_noffset = fdt_path_offset(fit, FIT_IMAGES_PATH);
	fdt_for_each_subnode(image_noffset, fit, images_noffset) {
		if (fit_image_get_data(fit, image_noffset, &image_data,
				       &data_size) ||
		    !range_contains(fit, image_size, image_data, data_size)) {
			printf("FIT payload '%s' exceeds the uploaded image.\n",
			       fit_get_name(fit, image_noffset, NULL));
			goto out;
		}
	}

	if (!fit_all_image_verify(fit)) {
		puts("Firmware image hash verification failed.\n");
		goto out;
	}

	config_noffset = fit_conf_get_node(fit, NULL);
	if (config_noffset < 0) {
		puts("Firmware image has no bootable configuration.\n");
		goto out;
	}

	loadable_count = fit_conf_get_prop_node_count(fit, config_noffset,
						      FIT_LOADABLE_PROP);
	for (loadable_index = 0; loadable_index < loadable_count;
	     loadable_index++) {
		image_noffset = fit_conf_get_prop_node_index(fit, config_noffset,
							     FIT_LOADABLE_PROP,
							   loadable_index);
		if (image_noffset >= 0 &&
		    !fit_image_get_type(fit, image_noffset, &image_type) &&
		    image_type == IH_TYPE_FILESYSTEM) {
			has_rootfs = true;
			break;
		}
	}
	if (!has_rootfs) {
		puts("Firmware configuration has no root filesystem.\n");
		goto out;
	}

	fdt_noffset = fit_conf_get_prop_node(fit, config_noffset,
					     FIT_FDT_PROP, IH_PHASE_NONE);
	if (fdt_noffset < 0 ||
	    fit_image_get_data(fit, fdt_noffset, &image_data, &data_size) ||
	    !range_contains(fit, image_size, image_data, data_size) ||
	    fdt_check_full(image_data, data_size)) {
		puts("Firmware device tree is invalid.\n");
		goto out;
	}

	compatible = fdt_getprop(image_data, 0, "compatible", &compatible_len);
	if (!compatible || compatible_len <= 1 ||
	    strnlen(compatible, compatible_len) == compatible_len ||
	    !of_machine_is_compatible(compatible)) {
		if (compatible && compatible_len > 1)
			printf("Firmware target '%.*s' does not match this board.\n",
			       compatible_len, compatible);
		else
			puts("Firmware target is missing.\n");
		goto out;
	}

	printf("Firmware FIT verified for %s (%zu bytes).\n", compatible,
	       image_size);
	ret = CMD_RET_SUCCESS;
out:
	unmap_sysmem((void *)fit);
	return ret;
}

static int run_firmware_update(void)
{
	const char *ubi_partition = HTTPD_UBI_PART;
	const char *fit_volume = HTTPD_FIRMWARE_VOLUME;
	const char *data_volume = HTTPD_DATA_VOLUME;
	ulong load_addr = env_get_ulong("loadaddr", 16, CONFIG_SYS_LOAD_ADDR);
	ulong file_size = env_get_ulong("filesize", 16, 0);
	void *image;
	u32 expected_crc;
	u32 actual_crc;
	int ret;

	if (!CONFIG_IS_ENABLED(CMD_UBI))
		return CMD_RET_FAILURE;

	if (!upload_region_valid(load_addr, file_size)) {
		puts("Firmware upload buffer is invalid.\n");
		return CMD_RET_FAILURE;
	}

	phase = "verify";
	ret = validate_firmware_fit(load_addr, file_size);
	if (ret)
		return ret;

	image = map_sysmem(load_addr, file_size);
	expected_crc = crc32_wd(0, image, file_size, CHUNKSZ_CRC32);
	unmap_sysmem(image);

	ret = ubi_part((char *)ubi_partition, NULL);
	if (ret) {
		printf("Unable to attach UBI partition '%s'.\n", ubi_partition);
		return CMD_RET_FAILURE;
	}

	if (ubi_volume_exists(HTTPD_STAGING_VOLUME) &&
	    ubi_volume_remove(HTTPD_STAGING_VOLUME))
		return CMD_RET_FAILURE;

	/* rootfs_data normally owns the free PEBs required by the new FIT volume. */
	if (ubi_volume_exists(data_volume) && ubi_volume_remove(data_volume))
		return CMD_RET_FAILURE;

	ret = ubi_volume_create(HTTPD_STAGING_VOLUME, file_size, true);
	if (ret)
		return CMD_RET_FAILURE;

	image = map_sysmem(load_addr, file_size);
	phase = "write";
	ret = ubi_volume_write(HTTPD_STAGING_VOLUME, image, 0, file_size);
	phase = "verify";
	if (!ret)
		ret = ubi_volume_read(HTTPD_STAGING_VOLUME, image, 0, file_size);
	unmap_sysmem(image);
	if (ret)
		return CMD_RET_FAILURE;

	image = map_sysmem(load_addr, file_size);
	actual_crc = crc32_wd(0, image, file_size, CHUNKSZ_CRC32);
	unmap_sysmem(image);
	if (actual_crc != expected_crc) {
		printf("Firmware readback CRC mismatch: %08x != %08x.\n",
		       actual_crc, expected_crc);
		return CMD_RET_FAILURE;
	}

	if (ubi_volume_exists(fit_volume) && ubi_volume_remove(fit_volume))
		return CMD_RET_FAILURE;

	ret = ubi_volume_rename(HTTPD_STAGING_VOLUME, fit_volume);
	if (ret) {
		printf("Verified firmware remains in UBI volume '%s'.\n",
		       HTTPD_STAGING_VOLUME);
		return CMD_RET_FAILURE;
	}

	printf("Firmware written to UBI volume '%s'; readback CRC %08x.\n",
	       fit_volume, actual_crc);
	return CMD_RET_SUCCESS;
}

static int create_recovery_layout(void)
{
	struct http_storage_request board_data;
	unsigned int index;
	int ret;

	ret = ubi_volume_create("fip", 0x100000, false);
	if (ret)
		return ret;
	ret = ubi_volume_create("ubootenv", 0x1f000, true);
	if (ret)
		return ret;
	ret = ubi_volume_create("ubootenv2", 0x1f000, true);
	if (ret)
		return ret;

	for (index = 0; index < HTTPD_BOARD_DATA_MAX; index++) {
		ret = board_data_config(index, &board_data);
		if (ret == -ENOENT)
			return 0;
		if (ret)
			return ret;
		ret = ubi_volume_create(board_data.target, board_data.size,
					board_data.dynamic);
		if (ret)
			return ret;
	}

	return 0;
}

static int run_ubi_rebuild(void)
{
	const char *part = HTTPD_UBI_PART;
	struct mtd_info *mtd;
	int ret;

	if (!CONFIG_IS_ENABLED(CMD_UBI))
		return -ENOSYS;
	if (!recovery_layout_present())
		return -EINVAL;
	mtd_probe_devices();
	/* Detach releases UBI's MTD reference before the physical erase. */
	ret = ubi_detach();
	if (ret)
		return ret;
	mtd = get_mtd_device_nm(part);
	if (IS_ERR(mtd))
		return PTR_ERR(mtd);
	/* The partition boundary keeps BL2 outside the rebuild range. */
	if (!mtd_is_partition(mtd) || !mtd->erasesize ||
	    mtd->size % mtd->erasesize) {
		put_mtd_device(mtd);
		return -EINVAL;
	}
	phase = "erase";
	ret = storage_mtd_erase(mtd, 0, mtd->size);
	put_mtd_device(mtd);
	if (ret)
		goto out;
	phase = "create";
	ret = ubi_part((char *)part, NULL);
	if (!ret)
		ret = create_recovery_layout();
out:
	if (ret)
		printf("UBI rebuild failed during %s (%d).\n", phase, ret);
	else
		puts("UBI rebuilt with empty base volumes. Write FIP, board data and system firmware before reboot.\n");
	return ret;
}

static int run_nand_scrub(void)
{
	struct mtd_info *mtd;
	struct erase_info erase;
	u64 addr;
	int ret;

	mtd = whole_flash_mtd();
	if (IS_ERR(mtd))
		return PTR_ERR(mtd);

	phase = "erase";
	printf("NAND scrub: clearing bad block markers on %s (%llu bytes)\n",
	       mtd->name, (unsigned long long)mtd->size);

	memset(&erase, 0, sizeof(erase));
	erase.mtd = mtd;
	erase.len = mtd->erasesize;
	erase.scrub = 1;

	for (addr = 0; addr < mtd->size; addr += mtd->erasesize) {
		erase.addr = addr;
		ret = mtd_block_isbad(mtd, addr);
		if (ret > 0)
			printf("Scrubbing bad block at 0x%llx.\n", addr);
		else if (ret < 0)
			goto out;

		ret = mtd_erase(mtd, &erase);
		if (ret)
			goto out;
		service_network();
	}

out:
	put_mtd_device(mtd);
	return ret;
}

static int run_board_data_update(void)
{
	int ret;

	if (storage_request.kind == HTTP_STORAGE_UBI) {
		ret = ubi_part(storage_request.partition, NULL);
		if (ret)
			return ret;
		if (!ubi_volume_exists(storage_request.target)) {
			ret = ubi_volume_create(storage_request.target,
						storage_request.size,
						storage_request.dynamic);
			if (ret)
				return ret;
		}
	}

	return run_storage_job();
}

static int run_action(void);

static int run_captured_job(void)
{
	char saved_stdout[STDIO_NAME_LEN];
	char saved_stderr[STDIO_NAME_LEN];
	int ret;

	strlcpy(saved_stdout, stdio_devices[stdout]->name, sizeof(saved_stdout));
	strlcpy(saved_stderr, stdio_devices[stderr]->name, sizeof(saved_stderr));
	if (console_assign(stdout, "webconsole") ||
	    console_assign(stderr, "webconsole")) {
		console_assign(stdout, saved_stdout);
		console_assign(stderr, saved_stderr);
		return CMD_RET_FAILURE;
	}

	switch (job_kind) {
	case HTTP_JOB_BL2:
		ret = run_bootloader_update(true);
		break;
	case HTTP_JOB_FIP:
		ret = run_bootloader_update(false);
		break;
	case HTTP_JOB_BOARD_DATA:
		ret = run_board_data_update();
		break;
	case HTTP_JOB_REBUILD:
		ret = run_ubi_rebuild();
		break;
	case HTTP_JOB_ENV:
		ret = env_set(env_key, env_value[0] ? env_value : NULL);
		if (!ret) ret = env_save();
		break;
	case HTTP_JOB_COMMAND:
		ret = run_command(command_buf, 0);
		break;
	case HTTP_JOB_ACTION:
		led_activity_on();
		ret = run_action();
		led_activity_blink();
		break;
	case HTTP_JOB_FIRMWARE:
		ret = run_firmware_update();
		break;
	case HTTP_JOB_FLASH_ALL:
		ret = run_flash_all();
		break;
	case HTTP_JOB_NAND_SCRUB:
		ret = run_nand_scrub();
		break;
	case HTTP_JOB_STORAGE:
		ret = run_storage_job();
		break;
	default:
		ret = CMD_RET_FAILURE;
		break;
	}
	console_assign(stdout, saved_stdout);
	console_assign(stderr, saved_stderr);
	return ret;
}

static int run_action(void)
{
	ulong load_addr;

	if (!strcmp(action_buf, "boot-production"))
		return run_command("run boot_production", 0);

	if (!strcmp(action_buf, "boot-upload")) {
		load_addr = env_get_ulong("loadaddr", 16, CONFIG_SYS_LOAD_ADDR);
		return run_commandf("bootm 0x%lx", load_addr);
	}

	if (!strcmp(action_buf, "reset"))
		return run_command("reset", 0);

	if (!strcmp(action_buf, "exit")) {
		stop_requested = true;
		return 0;
	}

	return CMD_RET_USAGE;
}

static void process_job(void)
{
	/* POST completion queues work so lwIP can finish its callback first. */
	if (job_state != HTTP_JOB_QUEUED || replies)
		return;

	job_state = HTTP_JOB_RUNNING;
	phase = "working";
	job_result = run_captured_job();
	if (job_kind == HTTP_JOB_FLASH_ALL)
		release_flash_all_upload();

	if (job_kind == HTTP_JOB_ACTION && job_result && !output_len) {
		snprintf(output_buf, sizeof(output_buf),
			 "Action '%s' returned %d\n", action_buf, job_result);
		output_len = strlen(output_buf);
	}
	command_buf[0] = '\0';
	action_buf[0] = '\0';
	storage_buf[0] = '\0';
	job_kind = HTTP_JOB_NONE;
	job_state = HTTP_JOB_DONE;
	if (!backup_ready) phase = job_result ? "error" : "done";
	build_info();
	build_storage_catalog();
}

bool uboot_httpd_is_running(void)
{
	return httpd_running;
}

int uboot_httpd_start(bool with_dhcp)
{
	struct udevice *udev;
	struct netif *netif;
	const char *recovery_addr;
	const char *recovery_mask;
	const char *ipaddr;
	int ret;

	if (httpd_running)
		return 0;

	recovery_addr = env_get("httpd_ipaddr");
	if (!recovery_addr || !*recovery_addr)
		recovery_addr = CONFIG_HTTPD_SERVER_IP;
	env_set("ipaddr", recovery_addr);

	recovery_mask = env_get("httpd_netmask");
	if (!recovery_mask || !*recovery_mask)
		recovery_mask = CONFIG_HTTPD_SERVER_NETMASK;
	env_set("netmask", recovery_mask);

	ret = webconsole_register();
	if (ret)
		return ret;

	ret = net_lwip_eth_start();
	if (ret)
		return ret;

	udev = eth_get_dev();
	netif = net_lwip_new_netif(udev);
	if (!netif) {
		net_lwip_eth_stop();
		return -ENODEV;
	}

	httpd_running = true;
	/* The cyclic LED timer shares the recovery network polling loop. */
	led_activity_blink();
	stop_requested = false;
	memset(&post_state, 0, sizeof(post_state));
	job_state = HTTP_JOB_IDLE;
	job_kind = HTTP_JOB_NONE;
	command_buf[0] = '\0';
	action_buf[0] = '\0';
	output_buf[0] = '\0';
	output_len = 0;
	job_result = 0;
	buffer_addr = env_get_ulong("loadaddr", 16, CONFIG_SYS_LOAD_ADDR);
	buffer_size = 0;
	backup_ready = false;
	upload_id = task_id = get_timer(0);
	downloads = replies = 0;
	http_eth = udev;
	http_netif = netif;
	build_info();
	build_storage_catalog();
	httpd_init();

	if (with_dhcp) {
		ret = uboot_httpd_dhcp_start(netif);
		if (ret)
			printf("DHCP server start failed: %d\n", ret);
	}

	ipaddr = env_get("ipaddr");
	printf("HTTP management: http://%s/  (%s)\n",
	       ipaddr, with_dhcp ? "DHCP enabled" : "static address");
	puts("Press Ctrl-C to return to the serial console.\n");
	clear_ctrlc();

	while (!stop_requested && !ctrlc()) {
		ret = net_lwip_rx(udev, netif);
		if (ret == -ENETDOWN || ret == -ENOLINK)
			ret = 0;
		if (ret < 0)
			break;
		if (backup_ready && !downloads && get_timer(backup_at) > 30000) {
			backup_ready = false;
			phase = "done";
		}
		process_job();
	}

	http_netif = NULL;
	uboot_httpd_dhcp_stop();
	httpd_stop();
	net_lwip_remove_netif(netif);
	net_lwip_eth_stop();
	httpd_running = false;
	led_activity_on();
	clear_ctrlc();
	return ret < 0 ? ret : 0;
}
