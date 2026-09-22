/*
 * Copyright (c) 2015-2018, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <string.h>

#include <plat_private.h>
#include <platform_def.h>

#include <common/bl_common.h>
#include <common/debug.h>
#include <drivers/io/io_driver.h>
#include <drivers/io/io_fip.h>
#include <drivers/io/io_memmap.h>
#include <drivers/io/io_encrypted.h>
#include <tools_share/firmware_image_package.h>

#if TRUSTED_BOARD_BOOT
#define TRUSTED_BOOT_FW_CERT_NAME	"tb_fw.crt"
#define TRUSTED_KEY_CERT_NAME		"trusted_key.crt"
#define SOC_FW_KEY_CERT_NAME		"soc_fw_key.crt"
#define TOS_FW_KEY_CERT_NAME		"tos_fw_key.crt"
#define NT_FW_KEY_CERT_NAME			"nt_fw_key.crt"
#define SOC_FW_CONTENT_CERT_NAME	"soc_fw_content.crt"
#define TOS_FW_CONTENT_CERT_NAME	"tos_fw_content.crt"
#define NT_FW_CONTENT_CERT_NAME		"nt_fw_content.crt"
#endif /* TRUSTED_BOARD_BOOT */

#if defined(IMAGE_BL23)
static uintptr_t ubi_dev_handle;
#endif
static const io_dev_connector_t *fip_dev_con;
static uintptr_t fip_dev_handle;
static const io_dev_connector_t *memmap_dev_con;
static uintptr_t memmap_dev_handle;
static const io_dev_connector_t *enc_dev_con;
static uintptr_t enc_dev_handle;

#if defined(IMAGE_BL31)
io_block_spec_t fip_block_spec = {
	.offset = 0,
	.length = 0
};

void set_fip_block_spec(size_t offset, size_t length)
{
	fip_block_spec.offset = offset;
	fip_block_spec.length = length;
}
#else
static const io_block_spec_t fip_block_spec = {
	.offset = PLAT_ECNT_FIP_BASE,
	.length = PLAT_ECNT_FIP_MAX_SIZE
};
#endif

static const io_uuid_spec_t bl2_uuid_spec = {
	.uuid = UUID_TRUSTED_BOOT_FIRMWARE_BL2,
};
#if !defined(IMAGE_BL1)
static const io_uuid_spec_t bl31_uuid_spec = {
	.uuid = UUID_EL3_RUNTIME_FIRMWARE_BL31,
};
#ifdef TCSUPPORT_OPTEE
static const io_uuid_spec_t bl32_uuid_spec = {
		.uuid = UUID_SECURE_PAYLOAD_BL32,
};
#endif
static const io_uuid_spec_t bl33_uuid_spec = {
	.uuid = UUID_NON_TRUSTED_FIRMWARE_BL33,
};
#endif
#if TRUSTED_BOARD_BOOT
static const io_uuid_spec_t tb_fw_cert_uuid_spec = {
	.uuid = UUID_TRUSTED_BOOT_FW_CERT,
};
#if !defined(IMAGE_BL1)
static const io_uuid_spec_t trusted_key_cert_uuid_spec = {
	.uuid = UUID_TRUSTED_KEY_CERT,
};
static const io_uuid_spec_t soc_fw_key_cert_uuid_spec = {
	.uuid = UUID_SOC_FW_KEY_CERT,
};

static const io_uuid_spec_t nt_fw_key_cert_uuid_spec = {
	.uuid = UUID_NON_TRUSTED_FW_KEY_CERT,
};
static const io_uuid_spec_t soc_fw_cert_uuid_spec = {
	.uuid = UUID_SOC_FW_CONTENT_CERT,
};

static const io_uuid_spec_t nt_fw_cert_uuid_spec = {
	.uuid = UUID_NON_TRUSTED_FW_CONTENT_CERT,
};
#ifdef TCSUPPORT_OPTEE
static const io_uuid_spec_t tos_fw_key_cert_uuid_spec = {
	.uuid = UUID_TRUSTED_OS_FW_KEY_CERT,
};

static const io_uuid_spec_t tos_fw_cert_uuid_spec = {
	.uuid = UUID_TRUSTED_OS_FW_CONTENT_CERT,
};
#endif
#endif
#endif /* TRUSTED_BOARD_BOOT */

#if defined(IMAGE_BL23)
static int check_ubi(const uintptr_t spec);
#endif
static int open_fip(const uintptr_t spec);
static int open_memmap(const uintptr_t spec);
static int open_enc_fip(const uintptr_t spec);

struct plat_io_policy {
	uintptr_t *dev_handle;
	uintptr_t image_spec;
	int (*check)(const uintptr_t spec);
};

static const struct plat_io_policy fip_memmap_policy = {
	.dev_handle = &memmap_dev_handle,
	.image_spec = (uintptr_t)&fip_block_spec,
	.check = open_memmap,
};

#if defined(IMAGE_BL23)
static const struct plat_io_policy fip_ubi_policy = {
	.dev_handle = &ubi_dev_handle,
	.image_spec = (uintptr_t)NULL,
	.check = check_ubi,
};
#endif

static const struct plat_io_policy enc_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)NULL,
	.check = open_fip,
};

static const struct plat_io_policy bl2_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)&bl2_uuid_spec,
	.check = open_fip,
};

#if !defined(IMAGE_BL1)
static const struct plat_io_policy bl31_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)&bl31_uuid_spec,
	.check = open_fip,
};

#ifdef TCSUPPORT_OPTEE
static const struct plat_io_policy bl32_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)&bl32_uuid_spec,
	.check = open_fip,
};
#endif

static const struct plat_io_policy bl33_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)&bl33_uuid_spec,
	.check = open_fip,
};
#endif

#if TRUSTED_BOARD_BOOT
static const struct plat_io_policy tb_fw_cert_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)&tb_fw_cert_uuid_spec,
	.check = open_fip,
};

#if !defined(IMAGE_BL1)
static const struct plat_io_policy trusted_key_cert_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)&trusted_key_cert_uuid_spec,
	.check = open_fip,
};

static const struct plat_io_policy soc_fw_key_cert_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)&soc_fw_key_cert_uuid_spec,
	.check = open_fip,
};

static const struct plat_io_policy nt_fw_key_cert_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)&nt_fw_key_cert_uuid_spec,
	.check = open_fip,
};

static const struct plat_io_policy soc_fw_cert_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)&soc_fw_cert_uuid_spec,
	.check = open_fip,
};

static const struct plat_io_policy nt_fw_cert_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)&nt_fw_cert_uuid_spec,
	.check = open_fip,
};

#ifdef TCSUPPORT_OPTEE
static const struct plat_io_policy tos_fw_key_cert_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)&tos_fw_key_cert_uuid_spec,
	.check = open_fip,
};

static const struct plat_io_policy tos_fw_cert_policy = {
	.dev_handle = &fip_dev_handle,
	.image_spec = (uintptr_t)&tos_fw_cert_uuid_spec,
	.check = open_fip,
};
#endif /* TCSUPPORT_OPTEE */
#endif /* !IMAGE_BL1 */
#endif /* TRUSTED_BOARD_BOOT */

static const struct plat_io_policy *policies[] = {
	[ENC_IMAGE_ID] = &enc_policy,
	[BL2_IMAGE_ID] = &bl2_policy,

#if !defined(IMAGE_BL1)
	[BL31_IMAGE_ID] = &bl31_policy,
#ifdef TCSUPPORT_OPTEE
	[BL32_IMAGE_ID] = &bl32_policy,
#endif
	[BL33_IMAGE_ID] = &bl33_policy,
#endif

#if TRUSTED_BOARD_BOOT
	[TRUSTED_BOOT_FW_CERT_ID] = &tb_fw_cert_policy,
#if !defined(IMAGE_BL1)
	[TRUSTED_KEY_CERT_ID] = &trusted_key_cert_policy,
	[SOC_FW_KEY_CERT_ID] = &soc_fw_key_cert_policy,
	[NON_TRUSTED_FW_KEY_CERT_ID] = &nt_fw_key_cert_policy,
	[SOC_FW_CONTENT_CERT_ID] = &soc_fw_cert_policy,
	[NON_TRUSTED_FW_CONTENT_CERT_ID] = &nt_fw_cert_policy,
#ifdef TCSUPPORT_OPTEE
	[TRUSTED_OS_FW_KEY_CERT_ID] = &tos_fw_key_cert_policy,
	[TRUSTED_OS_FW_CONTENT_CERT_ID] = &tos_fw_cert_policy,
#endif
#endif
#endif
};

#if defined(IMAGE_BL23)
static int check_ubi(const uintptr_t spec)
{
	int result;

	result = io_dev_init(ubi_dev_handle, (uintptr_t)NULL);

	return result;
}
#endif

static int open_fip(const uintptr_t spec)
{
	int result;
	uintptr_t local_image_handle;

	result = io_dev_init(fip_dev_handle, (uintptr_t)FIP_IMAGE_ID);
	if ((result == 0) && (spec != (uintptr_t)NULL)) {
		result = io_open(fip_dev_handle, spec, &local_image_handle);
		if (result == 0) {
			VERBOSE("Using FIP\n");
			result = fip_file_enc(local_image_handle);
			io_close(local_image_handle);
		}
	}
	return result;
}

static int open_enc_fip(const uintptr_t spec)
{
	int result;
	uintptr_t local_image_handle;

	result = io_dev_init(enc_dev_handle, (uintptr_t)ENC_IMAGE_ID);
	if (result == 0) {
		result = io_open(enc_dev_handle, spec, &local_image_handle);
		if (result == 0) {
			VERBOSE("Using encrypted FIP\n");
			io_close(local_image_handle);
		}
	}
	return result;
}

static int open_memmap(const uintptr_t spec)
{
	int result;
	uintptr_t local_image_handle;

	result = io_dev_init(memmap_dev_handle, (uintptr_t)NULL);
	if (result == 0) {
		result = io_open(memmap_dev_handle, spec, &local_image_handle);
		if (result == 0) {
			VERBOSE("Using Memmap\n");
			io_close(local_image_handle);
		}
	}
	return result;
}

#if defined(IMAGE_BL23)
void plat_ecnt_io_switch_to_memmap(void)
{
	policies[FIP_IMAGE_ID] = &fip_memmap_policy;
}

/*
 * Report whether the FIP is loaded from the UBI 'fip' volume rather than from
 * the pre-loaded memmap window.  The memmap pre-load describes the boot
 * partition layout only; when UBI owns the FIP the image loader reads the
 * volume directly, so the window is neither needed nor meaningful.
 */
int plat_ecnt_fip_uses_ubi(void)
{
	return policies[FIP_IMAGE_ID] == &fip_ubi_policy;
}
#endif

void plat_ecnt_io_setup(const hw_trap_t *hw_trap)
{
	int io_result;

	policies[FIP_IMAGE_ID] = &fip_memmap_policy;

#if defined(IMAGE_BL23) && !defined(ECNT_NAND_FIP_IN_BOOT_PARTITION)
	if (!hw_trap->is_emmc &&
	    (!hw_trap->fw_upgrade_mode || hw_trap->skip_fw_upgrade || plat_get_hw_bypass())) {
		policies[FIP_IMAGE_ID] = &fip_ubi_policy;
		io_result = mtk_fip_image_setup(&ubi_dev_handle,
						&policies[FIP_IMAGE_ID]->image_spec);
		if (io_result != 0) {
			ERROR("Failed to initialize NAND UBI FIP source (%i)\n",
			      io_result);
			policies[FIP_IMAGE_ID] = &fip_memmap_policy;
		}
	}
#endif

	io_result = register_io_dev_fip(&fip_dev_con);
	assert(io_result == 0);

	io_result = register_io_dev_memmap(&memmap_dev_con);
	assert(io_result == 0);

	io_result = io_dev_open(fip_dev_con, (uintptr_t)NULL,
				&fip_dev_handle);
	assert(io_result == 0);

	io_result = io_dev_open(memmap_dev_con, (uintptr_t)NULL,
				&memmap_dev_handle);
	assert(io_result == 0);

	io_result = register_io_dev_enc(&enc_dev_con);
	assert(io_result == 0);

	io_result = io_dev_open(enc_dev_con, (uintptr_t)NULL,
				&enc_dev_handle);
	assert(io_result == 0);

	(void)io_result;
}

int plat_get_image_source(unsigned int image_id, uintptr_t *dev_handle,
			  uintptr_t *image_spec)
{
	int result;
	const struct plat_io_policy *policy;

	assert(image_id < ARRAY_SIZE(policies));

	policy = policies[image_id];
	result = policy->check(policy->image_spec);
	if (result == 0)
	{
		if ((image_id == BL2_IMAGE_ID) || (image_id == BL31_IMAGE_ID) || (image_id == BL33_IMAGE_ID)) {
			INFO("FW UN-ENCRYPTION\n");
		}

		*image_spec = policy->image_spec;
		*dev_handle = *(policy->dev_handle);
	}
	else if (result == FW_ENCRYPTION)
	{
		if ((image_id == BL2_IMAGE_ID) || (image_id == BL31_IMAGE_ID) || (image_id == BL33_IMAGE_ID)) {
			INFO("FW ENCRYPTION\n");
		}

		result = open_enc_fip(policy->image_spec);
		if (result == 0)
		{
			*image_spec = policy->image_spec;
			*dev_handle = enc_dev_handle;
		}
	}

	return result;
}

int plat_check_bypass(void)
{
	int result = 0;
	uint16_t plat_toc_flag = 0;

	result = io_dev_init(fip_dev_handle, (uintptr_t)FIP_IMAGE_ID);
	if ((result == 0))
	{
		fip_dev_get_plat_toc_flag((io_dev_info_t *)fip_dev_handle , &plat_toc_flag);
		result = plat_toc_flag & BYPASS_FWUPGRADE;
		if (result == BYPASS_FWUPGRADE) {
			INFO("3-3-4\n");
		} else {
			INFO("3-3-3\n");
		}
	} else {
		INFO("3-3-2\n");
	}

	return result;
}

#ifdef TCSUPPORT_ARM_SECURE_BOOT_FLASH_KEY
int plat_check_secure_boot_flash_key(void)
{
	int result = 0;

	result = io_dev_init(fip_dev_handle, (uintptr_t)FIP_IMAGE_ID);

	if (result == 0)
	{
		uint16_t plat_toc_flag = 0;

		fip_dev_get_plat_toc_flag((io_dev_info_t *)fip_dev_handle , &plat_toc_flag);
		result = plat_toc_flag & ARM_SECURE_BOOT_FLASH_KEY;

		if (result == ARM_SECURE_BOOT_FLASH_KEY)
		{
			NOTICE("Get secure key in flash\n");
		}
		else
		{
			NOTICE("Do not get secure key in flash\n");

			result = 0;
		}
	}
	else
	{
		NOTICE("Do not get secure key in flash\n");

		result = 0;
	}

	return result;
}
#endif

int plat_check_header(uint8_t *base)
{
	fip_toc_header_t *header = (fip_toc_header_t *) base;

	if ((header->name == TOC_HEADER_NAME) && (header->serial_number != 0))
	{
		return 1;
	} else
	{
		return 0;
	}
}
