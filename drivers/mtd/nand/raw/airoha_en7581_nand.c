// SPDX-License-Identifier: GPL-2.0
/*
 * Airoha EN7581 parallel NAND controller
 *
 * The register-level NAND/ECC datapath descends from the MediaTek NFI used by
 * MT7621.  EN7581 changes transaction setup, DMA completion accounting and
 * BCH input polarity; those differences are kept explicit in this driver.
 */

#include <clk.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <log.h>
#include <nand.h>
#include <malloc.h>
#include <memalign.h>
#include <linux/dma-direction.h>
#include <linux/dma-mapping.h>
#include <linux/io.h>
#include <linux/iopoll.h>
#include <linux/sizes.h>
#include <linux/bitops.h>
#include <linux/bitfield.h>
#include "airoha_en7581_nand.h"

/* NFI core registers */
#define NFI_CNFG			0x000
#define   CNFG_OP_MODE			GENMASK(14, 12)
#define     CNFG_OP_CUSTOM		6
#define   CNFG_AUTO_FMT_EN		BIT(9)
#define   CNFG_HW_ECC_EN		BIT(8)
#define   CNFG_BYTE_RW			BIT(6)
#define   CNFG_ECC_DATA_SOURCE_INV	BIT(5)
#define   CNFG_DMA_BURST_EN		BIT(2)
#define   CNFG_READ_MODE		BIT(1)
#define   CNFG_AHB			BIT(0)

#define     CNFG_OP_IDLE		0
#define     CNFG_OP_SINGLE_READ		2
#define     CNFG_OP_PROGRAM		3
#define     CNFG_OP_ERASE		4
#define     CNFG_OP_READ		6

#define NFI_PAGEFMT			0x004
#define   PAGEFMT_FDM_ECC		GENMASK(15, 12)
#define   PAGEFMT_FDM			GENMASK(11, 8)
#define   PAGEFMT_SPARE			GENMASK(5, 4)
#define   PAGEFMT_PAGE			GENMASK(1, 0)

#define NFI_CON				0x008
#define   CON_NFI_SEC			GENMASK(15, 12)
#define   CON_NFI_BWR			BIT(9)
#define   CON_NFI_BRD			BIT(8)
#define   CON_NOB			GENMASK(7, 5)
#define   CON_SRD			BIT(4)
#define   CON_NFI_RST			BIT(1)
#define   CON_FIFO_FLUSH		BIT(0)

#define NFI_ACCCON			0x00c
#define   ACCCON_POECS			GENMASK(31, 28)
#define   ACCCON_POECS_DEF		3
#define   ACCCON_PRECS			GENMASK(27, 22)
#define   ACCCON_PRECS_DEF		3
#define   ACCCON_C2R			GENMASK(21, 16)
#define   ACCCON_C2R_DEF		7
#define   ACCCON_W2R			GENMASK(15, 12)
#define   ACCCON_W2R_DEF		7
#define   ACCCON_WH			GENMASK(11, 8)
#define   ACCCON_WH_DEF			15
#define   ACCCON_WST			GENMASK(7, 4)
#define   ACCCON_WST_DEF		15
#define   ACCCON_WST_MIN		3
#define   ACCCON_RLT			GENMASK(3, 0)
#define   ACCCON_RLT_DEF		15
#define   ACCCON_RLT_MIN		3

#define NFI_CMD				0x020

#define NFI_ADDRNOB			0x030
#define   ADDR_ROW_NOB			GENMASK(6, 4)
#define   ADDR_COL_NOB			GENMASK(2, 0)

#define NFI_COLADDR			0x034
#define NFI_ROWADDR			0x038

#define NFI_STRDATA			0x040
#define   STR_DATA			BIT(0)

#define NFI_CNRNB			0x044
#define   CB2R_TIME			GENMASK(7, 4)
#define   STR_CNRNB			BIT(0)

#define NFI_DATAW			0x050
#define NFI_DATAR			0x054

#define NFI_PIO_DIRDY			0x058
#define   PIO_DIRDY			BIT(0)

#define NFI_STA				0x060
#define   STA_NFI_FSM			GENMASK(19, 16)
#define     STA_FSM_CUSTOM_DATA		14
#define   STA_BUSY			BIT(8)
#define   STA_ADDR			BIT(1)
#define   STA_CMD			BIT(0)

#define NFI_ADDRCNTR			0x070
#define   SEC_CNTR			GENMASK(15, 12)
#define   SEC_ADDR			GENMASK(9, 0)

#define NFI_CSEL			0x090
#define   CSEL				GENMASK(1, 0)

#define NFI_FDM0L			0x0a0
#define NFI_FDML(n)			(0x0a0 + ((n) << 3))

#define NFI_FDM0M			0x0a4
#define NFI_FDMM(n)			(0x0a4 + ((n) << 3))

#define NFI_MASTER_STA			0x210
#define   MAS_ADDR			GENMASK(11, 9)
#define   MAS_RD			GENMASK(8, 6)
#define   MAS_WR			GENMASK(5, 3)
#define   MAS_RDDLY			GENMASK(2, 0)

/* EN7581 extends the shared NFI bank with byte-count and mode registers. */
#define NFI_INTR_EN			0x010
#define NFI_INTR_STA			0x014
#define   INTR_AHB_DONE			BIT(6)
#define NFI_STRADDR			0x080
#define NFI_SECCUS_SIZE			0x22c
#define   SECCUS_SIZE			GENMASK(12, 0)
#define   SECCUS_SIZE_EN		BIT(16)
#define NFI_MISC_CTL2			0x53c
#define   MISC_RD_BYTE_NUM		GENMASK(12, 0)
#define   MISC_WR_BYTE_NUM		GENMASK(28, 16)
#define NFI_MODE			0x55c
#define   NFI_MODE_SERIAL		BIT(2)
#define NFI_RD_CNT			0x568
#define NFI_WR_CNT			0x56c
#define NFI_CNT_CLR			0x578
#define   CNT_CLR_WR			BIT(2)
#define   CNT_CLR_RD			BIT(3)

/* ECC engine registers */
#define ECC_ENCCON			0x000
#define   ENC_EN			BIT(0)

#define ECC_ENCCNFG			0x004
#define   ENC_CNFG_MSG			GENMASK(28, 16)
#define   ENC_MODE			GENMASK(5, 4)
#define     ENC_MODE_NFI		1
#define   ENC_TNUM			GENMASK(2, 0)

#define ECC_ENCIDLE			0x00c
#define   ENC_IDLE			BIT(0)

#define ECC_DECCON			0x100
#define   DEC_EN			BIT(0)

#define ECC_DECCNFG			0x104
#define   DEC_EMPTY_EN			BIT(31)
#define   DEC_CS			GENMASK(28, 16)
#define   DEC_CON			GENMASK(13, 12)
#define     DEC_CON_CORRECT		3
#define   DEC_MODE			GENMASK(5, 4)
#define     DEC_MODE_NFI		1
#define   DEC_TNUM			GENMASK(2, 0)

#define ECC_DECIDLE			0x10c
#define   DEC_IDLE			BIT(0)

#define ECC_DECENUM0			0x114
#define ECC_DECENUM1			0x118
#define   ERRNUM_S			5
#define   ERRNUM_M			GENMASK(4, 0)

#define ECC_DECDONE			0x11c
#define   DEC_DONE7			BIT(7)
#define   DEC_DONE6			BIT(6)
#define   DEC_DONE5			BIT(5)
#define   DEC_DONE4			BIT(4)
#define   DEC_DONE3			BIT(3)
#define   DEC_DONE2			BIT(2)
#define   DEC_DONE1			BIT(1)
#define   DEC_DONE0			BIT(0)

#define ECC_DECIRQEN			0x140
#define   DEC_IRQEN			BIT(0)

/* ENCIDLE and DECIDLE */
#define   ECC_IDLE			BIT(0)

#define ACCTIMING(tpoecs, tprecs, tc2r, tw2r, twh, twst, trlt) \
	(FIELD_PREP(ACCCON_POECS, tpoecs) | \
	 FIELD_PREP(ACCCON_PRECS, tprecs) | \
	 FIELD_PREP(ACCCON_C2R, tc2r) | \
	 FIELD_PREP(ACCCON_W2R, tw2r) | \
	 FIELD_PREP(ACCCON_WH, twh) | \
	 FIELD_PREP(ACCCON_WST, twst) | \
	 FIELD_PREP(ACCCON_RLT, trlt))

/*
 * DE-approved EN7581 NFI access timing for the 150 MHz APB clock. BootROM
 * leaves NFI_ACCCON at the reset default (0xf3ffffff, all fields maxed to the
 * slowest setting), so the driver programs it here. Matches the value tf-a
 * carries as PARALLEL_NAND_FLASH_TIMING (TCSUPPORT_CPU_ARMV8 branch).
 */
#define NFI_ACCCON_EN7581		0x40044326

#define MASTER_STA_MASK			(MAS_ADDR | MAS_RD | MAS_WR | \
					 MAS_RDDLY)
#define NFI_RESET_TIMEOUT		1000000
#define NFI_CORE_TIMEOUT		500000
#define ECC_ENGINE_TIMEOUT		500000

#define ECC_SECTOR_SIZE			512
#define ECC_PARITY_BITS			13

#define NFI_FDM_SIZE			8

/* EN7581 physical register banks; both are directly accessible at EL2. */
#define NFI_BASE			0x1fa11000
#define NFI_ECC_BASE			0x1fa12000

/*
 * A raw transaction can cover a 4 KiB page plus OOB. Cache-line alignment
 * confines DMA invalidation to this buffer.
 */
static u8 airoha_dma_buffer[SZ_8K] __aligned(ARCH_DMA_MINALIGN);

static const u16 airoha_nfi_page_size[] = { SZ_512, SZ_2K, SZ_4K };
static const u8 airoha_nfi_spare_size[] = { 16, 26, 27, 28 };
static const u8 airoha_ecc_strength[] = { 4, 6, 8, 10, 12, 14, 16 };

static inline u32 nfi_read32(struct airoha_nfc *nfc, u32 reg)
{
	return readl(nfc->nfi_regs + reg);
}

static inline void nfi_write32(struct airoha_nfc *nfc, u32 reg, u32 val)
{
	writel(val, nfc->nfi_regs + reg);
}

static inline u16 nfi_read16(struct airoha_nfc *nfc, u32 reg)
{
	return readw(nfc->nfi_regs + reg);
}

static inline void nfi_write16(struct airoha_nfc *nfc, u32 reg, u16 val)
{
	writew(val, nfc->nfi_regs + reg);
}

static inline void ecc_write16(struct airoha_nfc *nfc, u32 reg, u16 val)
{
	writew(val, nfc->ecc_regs + reg);
}

static inline u32 ecc_read32(struct airoha_nfc *nfc, u32 reg)
{
	return readl(nfc->ecc_regs + reg);
}

static inline void ecc_write32(struct airoha_nfc *nfc, u32 reg, u32 val)
{
	writel(val, nfc->ecc_regs + reg);
}

static inline u8 *oob_fdm_ptr(struct nand_chip *nand, int sect)
{
	return nand->oob_poi + sect * NFI_FDM_SIZE;
}

static inline u8 *oob_ecc_ptr(struct airoha_nfc *nfc, int sect)
{
	struct nand_chip *nand = &nfc->nand;

	return nand->oob_poi + nand->ecc.steps * NFI_FDM_SIZE +
		sect * (nfc->spare_per_sector - NFI_FDM_SIZE);
}

static inline u8 *page_data_ptr(struct nand_chip *nand, const u8 *buf,
				int sect)
{
	return (u8 *)buf + sect * nand->ecc.size;
}

static int airoha_ecc_wait_idle(struct airoha_nfc *nfc, u32 reg)
{
	u32 val;
	int ret;

	ret = readw_poll_timeout(nfc->ecc_regs + reg, val, val & ECC_IDLE,
				 ECC_ENGINE_TIMEOUT);
	if (ret) {
		pr_warn("ECC engine timed out entering idle mode\n");
		return -EIO;
	}

	return 0;
}

static int airoha_ecc_decoder_wait_done(struct airoha_nfc *nfc, u32 sect)
{
	u32 val;
	int ret;

	ret = readw_poll_timeout(nfc->ecc_regs + ECC_DECDONE, val,
				 val & (1 << sect), ECC_ENGINE_TIMEOUT);
	if (ret) {
		/* These registers identify the stalled decoder stage after a timeout. */
		pr_err("ECC decoder sector %u timeout: done=0x%04x idle=0x%04x enum0=0x%08x enum1=0x%08x cfg=0x%08x\n",
			sect, val, readw(nfc->ecc_regs + ECC_DECIDLE),
			ecc_read32(nfc, ECC_DECENUM0),
			ecc_read32(nfc, ECC_DECENUM1),
			ecc_read32(nfc, ECC_DECCNFG));
		return -ETIMEDOUT;
	}

	return 0;
}

static void airoha_ecc_encoder_op(struct airoha_nfc *nfc, bool enable)
{
	airoha_ecc_wait_idle(nfc, ECC_ENCIDLE);
	ecc_write16(nfc, ECC_ENCCON, enable ? ENC_EN : 0);
}

static void airoha_ecc_decoder_op(struct airoha_nfc *nfc, bool enable)
{
	airoha_ecc_wait_idle(nfc, ECC_DECIDLE);
	ecc_write16(nfc, ECC_DECCON, enable ? DEC_EN : 0);
}

static int airoha_ecc_correct_check(struct airoha_nfc *nfc, u32 sect)
{
	u32 decnum;
	u32 num_error_bits;

	/*
	 * Each DECENUM register holds four five-bit sector counters. A value of
	 * 0x1f marks an uncorrectable codeword. AHB correction has already updated
	 * the DMA buffer, so this path only accounts for MTD statistics.
	 */
	decnum = ecc_read32(nfc, sect < 4 ? ECC_DECENUM0 : ECC_DECENUM1);
	num_error_bits = (decnum >> ((sect & 3) * ERRNUM_S)) & ERRNUM_M;

	return num_error_bits == ERRNUM_M ? -1 : num_error_bits;
}

static void airoha_nfc_hw_reset(struct airoha_nfc *nfc)
{
	u32 val;
	int ret;

	/* EN7581 resets the FIFO and parallel state machine together through CON. */
	nfi_write16(nfc, NFI_CON, CON_FIFO_FLUSH | CON_NFI_RST);
	ret = readl_poll_timeout(nfc->nfi_regs + NFI_STA, val,
				 !(val & GENMASK(3, 0)), NFI_RESET_TIMEOUT);
	if (ret)
		pr_warn("EN7581 NFI reset timeout: sta=0x%08x\n", val);

	/* A second reset edge clears FIFO state produced by the first edge. */
	nfi_write16(nfc, NFI_CON, CON_FIFO_FLUSH | CON_NFI_RST);
}

static inline void airoha_nfc_hw_init(struct airoha_nfc *nfc)
{
	nfi_write16(nfc, NFI_CNRNB, CB2R_TIME | STR_CNRNB);
	airoha_nfc_hw_reset(nfc);
	/* Program the DE-approved EN7581 timing; BootROM only left the reset default. */
	nfi_write32(nfc, NFI_ACCCON, NFI_ACCCON_EN7581);
	nfi_write32(nfc, NFI_INTR_EN, INTR_AHB_DONE);
}

static int airoha_nfc_issue_command(struct airoha_nfc *nfc, u8 command)
{
	u32 val;
	int ret;

	nfc->command = command;
	nfi_write32(nfc, NFI_CMD, command);

	ret = readl_poll_timeout(nfc->nfi_regs + NFI_STA, val, !(val & STA_CMD),
				 NFI_CORE_TIMEOUT);
	if (ret) {
		pr_err("EN7581 command 0x%02x timeout: sta=0x%08x\n",
		       command, val);
		return -EIO;
	}

	return 0;
}

static int airoha_nfc_send_command(struct airoha_nfc *nfc, u8 command)
{
	u32 config, mode = CNFG_OP_IDLE;

	/*
	 * EN7581 selects the data direction and bus state machine from the opening
	 * NAND command. Closing commands retain the mode latched by that command.
	 */
	switch (command) {
	case NAND_CMD_READ0:
	case NAND_CMD_RNDOUT:
		mode = CNFG_OP_READ;
		break;
	case NAND_CMD_SEQIN:
	case NAND_CMD_SET_FEATURES:
		mode = CNFG_OP_PROGRAM;
		break;
	case NAND_CMD_ERASE1:
		mode = CNFG_OP_ERASE;
		break;
	case NAND_CMD_READID:
	case NAND_CMD_STATUS:
	case NAND_CMD_PARAM:
	case NAND_CMD_GET_FEATURES:
		mode = CNFG_OP_SINGLE_READ;
		break;
	case NAND_CMD_READSTART:
	case NAND_CMD_RNDOUTSTART:
	case NAND_CMD_RNDIN:
	case NAND_CMD_CACHEDPROG:
	case NAND_CMD_PAGEPROG:
	case NAND_CMD_ERASE2:
		goto issue;
	case NAND_CMD_RESET:
		/* RESET starts from idle so its direction is independent of prior I/O. */
		break;
	default:
		break;
	}

	config = nfi_read32(nfc, NFI_CNFG);
	config &= ~(CNFG_OP_MODE | CNFG_AHB | CNFG_DMA_BURST_EN |
		    CNFG_BYTE_RW | CNFG_READ_MODE | CNFG_HW_ECC_EN |
		    CNFG_AUTO_FMT_EN | CNFG_ECC_DATA_SOURCE_INV);
	config |= FIELD_PREP(CNFG_OP_MODE, mode);
	if (mode == CNFG_OP_READ || mode == CNFG_OP_SINGLE_READ)
		config |= CNFG_READ_MODE;
	if (mode == CNFG_OP_SINGLE_READ)
		config |= CNFG_BYTE_RW;
	nfi_write32(nfc, NFI_CNFG, config);

issue:
	return airoha_nfc_issue_command(nfc, command);
}

static int airoha_nfc_send_address(struct airoha_nfc *nfc, u32 column,
				   u32 row, u8 column_cycles,
				   u8 row_cycles)
{
	u32 val;
	int ret;

	if (column_cycles > 4 || row_cycles > 4) {
		pr_err("EN7581 address cycle overflow: command=0x%02x col=%u row=%u\n",
		       nfc->command, column_cycles, row_cycles);
		return -EINVAL;
	}

	/* ADDRNOB latches column, row, and both cycle counts as one phase. */
	nfi_write32(nfc, NFI_COLADDR, column);
	nfi_write32(nfc, NFI_ROWADDR, row);
	nfi_write32(nfc, NFI_ADDRNOB,
		    FIELD_PREP(ADDR_COL_NOB, column_cycles) |
		    FIELD_PREP(ADDR_ROW_NOB, row_cycles));

	ret = readl_poll_timeout(nfc->nfi_regs + NFI_STA, val,
				 !(val & (STA_CMD | STA_ADDR)),
				 NFI_CORE_TIMEOUT);
	if (ret) {
		pr_warn("EN7581 address timeout: command=0x%02x col=%u row=%u sta=0x%08x\n",
			nfc->command, column_cycles, row_cycles, val);
		return -EIO;
	}

	return 0;
}

static int airoha_nfc_dev_ready(struct mtd_info *mtd)
{
	struct airoha_nfc *nfc = nand_get_controller_data(mtd_to_nand(mtd));

	if (nfi_read32(nfc, NFI_STA) & STA_BUSY)
		return 0;

	return 1;
}

static void airoha_nfc_select_chip(struct mtd_info *mtd, int chipnr)
{
	struct airoha_nfc *nfc = nand_get_controller_data(mtd_to_nand(mtd));

	nfi_write16(nfc, NFI_CSEL, 0);
}

static int airoha_nfc_wait_pio_ready(struct airoha_nfc *nfc)
{
	int ret;
	u16 val;

	ret = readw_poll_timeout(nfc->nfi_regs + NFI_PIO_DIRDY, val,
				 val & PIO_DIRDY, NFI_CORE_TIMEOUT);
	if (ret < 0)
		pr_err("NFI core PIO mode not ready\n");

	return ret;
}

static int airoha_nfc_read_data(struct airoha_nfc *nfc, u8 *buf, u32 len)
{
	u32 chunk, i;
	int ret = 0;

	/*
	 * Short READID, STATUS, and parameter transfers use the SRD/PIO path.
	 * CON_NOB is three bits wide, so longer transfers keep the active command
	 * while consuming data in seven-byte chunks.
	 */
	while (len) {
		chunk = min_t(u32, len, 7);
		nfi_write16(nfc, NFI_CON,
			    FIELD_PREP(CON_NOB, chunk) | CON_SRD);
		for (i = 0; i < chunk; i++) {
			ret = airoha_nfc_wait_pio_ready(nfc);
			if (ret) {
				memset(buf, 0xff, len);
				goto out;
			}
			*buf++ = nfi_read32(nfc, NFI_DATAR) & 0xff;
			len--;
		}
	}

out:
	nfi_write16(nfc, NFI_CON, 0);
	return ret;
}

static int airoha_nfc_write_data(struct airoha_nfc *nfc, const u8 *buf,
				 u32 len)
{
	u32 chunk, config, i;
	int ret = 0;

	config = nfi_read32(nfc, NFI_CNFG);
	config &= ~(CNFG_READ_MODE | CNFG_AHB | CNFG_DMA_BURST_EN);
	config |= CNFG_BYTE_RW;
	nfi_write32(nfc, NFI_CNFG, config);

	while (len) {
		chunk = min_t(u32, len, 7);
		nfi_write16(nfc, NFI_CON, FIELD_PREP(CON_NOB, chunk));
		for (i = 0; i < chunk; i++) {
			ret = airoha_nfc_wait_pio_ready(nfc);
			if (ret)
				goto out;
			nfi_write32(nfc, NFI_DATAW, *buf++);
		}
		len -= chunk;
	}

out:
	nfi_write16(nfc, NFI_CON, 0);
	return ret;
}

static void airoha_nfc_write_byte(struct mtd_info *mtd, u8 byte)
{
	struct airoha_nfc *nfc = nand_get_controller_data(mtd_to_nand(mtd));

	(void)airoha_nfc_write_data(nfc, &byte, 1);
}

static void airoha_nfc_write_buf(struct mtd_info *mtd, const u8 *buf, int len)
{
	struct airoha_nfc *nfc = nand_get_controller_data(mtd_to_nand(mtd));

	(void)airoha_nfc_write_data(nfc, buf, len);
}

static u8 airoha_nfc_read_byte(struct mtd_info *mtd)
{
	struct airoha_nfc *nfc = nand_get_controller_data(mtd_to_nand(mtd));

	if (nfc->short_io_pos >= nfc->short_io_len)
		return 0xff;

	return airoha_dma_buffer[nfc->short_io_pos++];
}

static void airoha_nfc_read_buf(struct mtd_info *mtd, u8 *buf, int len)
{
	struct airoha_nfc *nfc = nand_get_controller_data(mtd_to_nand(mtd));
	size_t available;

	available = nfc->short_io_len - nfc->short_io_pos;
	available = min_t(size_t, available, len);
	memcpy(buf, airoha_dma_buffer + nfc->short_io_pos, available);
	if (available < len)
		memset(buf + available, 0xff, len - available);
	nfc->short_io_pos += available;
}

static int airoha_nfc_wait_ready(struct airoha_nfc *nfc)
{
	u32 status;

	return readl_poll_timeout(nfc->nfi_regs + NFI_STA, status,
				  !(status & STA_BUSY), NFI_CORE_TIMEOUT);
}

static int airoha_nfc_prepare_short_read(struct airoha_nfc *nfc,
					 u32 length, u32 exposed_length)
{
	int ret;

	if (exposed_length > sizeof(airoha_dma_buffer) || length > exposed_length)
		return -E2BIG;

	memset(airoha_dma_buffer, 0, exposed_length);
	ret = airoha_nfc_read_data(nfc, airoha_dma_buffer, length);
	if (ret)
		return ret;

	nfc->short_io_len = exposed_length;
	nfc->short_io_pos = 0;
	return 0;
}

static int airoha_nfc_begin_page_read(struct nand_chip *nand, int page,
				      u32 config, bool use_ecc)
{
	struct airoha_nfc *nfc = nand_get_controller_data(nand);
	struct mtd_info *mtd = nand_to_mtd(nand);
	u8 column_cycles = mtd->writesize > 512 ? 2 : 1;
	u8 row_cycles = nand->options & NAND_ROW_ADDR_3 ? 3 : 2;
	int ret;

	/*
	 * EN7581 latches PAGEFMT, auto-format, ECC, and DMA state when READ0 is
	 * issued. Keeping setup and data transfer in one function preserves that
	 * page transaction boundary.
	 */
	airoha_nfc_hw_reset(nfc);
	nfi_write32(nfc, NFI_CNFG, config);
	if (use_ecc)
		airoha_ecc_decoder_op(nfc, true);

	ret = airoha_nfc_issue_command(nfc, NAND_CMD_READ0);
	if (!ret)
		ret = airoha_nfc_send_address(nfc, 0, page,
					      column_cycles, row_cycles);
	if (!ret && mtd->writesize > 512)
		ret = airoha_nfc_issue_command(nfc, NAND_CMD_READSTART);
	if (!ret)
		ret = airoha_nfc_wait_ready(nfc);

	return ret;
}

static void airoha_nfc_command(struct mtd_info *mtd, unsigned int command,
			       int column, int page_addr)
{
	struct nand_chip *nand = mtd_to_nand(mtd);
	struct airoha_nfc *nfc = nand_get_controller_data(nand);
	u8 column_cycles = mtd->writesize > 512 ? 2 : 1;
	u8 row_cycles = nand->options & NAND_ROW_ADDR_3 ? 3 : 2;
	u32 read_length;
	int ret = 0;

	nfc->short_io_len = 0;
	nfc->short_io_pos = 0;

	if (command == NAND_CMD_READOOB) {
		column += mtd->writesize;
		command = NAND_CMD_READ0;
	}

	switch (command) {
	case NAND_CMD_RESET:
		airoha_nfc_hw_reset(nfc);
		ret = airoha_nfc_send_command(nfc, command);
		if (!ret)
			ret = airoha_nfc_wait_ready(nfc);
		break;
	case NAND_CMD_READID:
		airoha_nfc_hw_reset(nfc);
		ret = airoha_nfc_send_command(nfc, command);
		if (!ret)
			ret = airoha_nfc_send_address(nfc, column, 0, 1, 0);
		/* Full geometry is encoded in the five-byte device ID. */
		read_length = column ? 4 : 5;
		if (!ret)
			ret = airoha_nfc_prepare_short_read(nfc, read_length,
						   column ? 4 : 8);
		break;
	case NAND_CMD_STATUS:
		airoha_nfc_hw_reset(nfc);
		ret = airoha_nfc_send_command(nfc, command);
		if (!ret)
			ret = airoha_nfc_prepare_short_read(nfc, 1, 1);
		break;
	case NAND_CMD_PARAM:
		airoha_nfc_hw_reset(nfc);
		ret = airoha_nfc_send_command(nfc, command);
		if (!ret)
			ret = airoha_nfc_send_address(nfc, column, 0, 1, 0);
		if (!ret)
			ret = airoha_nfc_wait_ready(nfc);
		if (!ret)
			ret = airoha_nfc_prepare_short_read(nfc, 3 * 256,
						   3 * 256);
		break;
	case NAND_CMD_GET_FEATURES:
		airoha_nfc_hw_reset(nfc);
		ret = airoha_nfc_send_command(nfc, command);
		if (!ret)
			ret = airoha_nfc_send_address(nfc, column, 0, 1, 0);
		if (!ret)
			ret = airoha_nfc_prepare_short_read(nfc, 4, 4);
		break;
	case NAND_CMD_SET_FEATURES:
		airoha_nfc_hw_reset(nfc);
		ret = airoha_nfc_send_command(nfc, command);
		if (!ret)
			ret = airoha_nfc_send_address(nfc, column, 0, 1, 0);
		break;
	case NAND_CMD_READ0:
		airoha_nfc_hw_reset(nfc);
		ret = airoha_nfc_send_command(nfc, command);
		if (!ret)
			ret = airoha_nfc_send_address(nfc, column, page_addr,
						     column_cycles, row_cycles);
		if (!ret && mtd->writesize > 512)
			ret = airoha_nfc_send_command(nfc, NAND_CMD_READSTART);
		if (!ret)
			ret = airoha_nfc_wait_ready(nfc);
		break;
	case NAND_CMD_RNDOUT:
		ret = airoha_nfc_send_command(nfc, command);
		if (!ret)
			ret = airoha_nfc_send_address(nfc, column, 0,
						     column_cycles, 0);
		if (!ret)
			ret = airoha_nfc_send_command(nfc, NAND_CMD_RNDOUTSTART);
		break;
	case NAND_CMD_SEQIN:
		airoha_nfc_hw_reset(nfc);
		ret = airoha_nfc_send_command(nfc, command);
		if (!ret)
			ret = airoha_nfc_send_address(nfc, column, page_addr,
						     column_cycles, row_cycles);
		break;
	case NAND_CMD_RNDIN:
		ret = airoha_nfc_send_command(nfc, command);
		if (!ret)
			ret = airoha_nfc_send_address(nfc, column, 0,
						     column_cycles, 0);
		break;
	case NAND_CMD_ERASE1:
		airoha_nfc_hw_reset(nfc);
		ret = airoha_nfc_send_command(nfc, command);
		if (!ret)
			ret = airoha_nfc_send_address(nfc, 0, page_addr, 0,
						     row_cycles);
		break;
	case NAND_CMD_PAGEPROG:
	case NAND_CMD_CACHEDPROG:
	case NAND_CMD_ERASE2:
		ret = airoha_nfc_send_command(nfc, command);
		if (!ret)
			ret = airoha_nfc_wait_ready(nfc);
		break;
	default:
		pr_err("EN7581 unsupported NAND command 0x%02x\n", command);
		return;
	}

	if (ret)
		pr_err("EN7581 NAND command 0x%02x failed: %d\n", command, ret);
}

static int airoha_nfc_calc_ecc_strength(struct airoha_nfc *nfc,
					u32 avail_ecc_bytes)
{
	struct nand_chip *nand = &nfc->nand;
	struct mtd_info *mtd = nand_to_mtd(nand);
	u32 required, strength;
	int i;

	/* The declared strength selects the on-flash BCH codeword. */
	required = nand->ecc_step_ds == ECC_SECTOR_SIZE ?
		   nand->ecc_strength_ds : 0;
	strength = avail_ecc_bytes * 8 / ECC_PARITY_BITS;

	if (required) {
		for (i = 0; i < ARRAY_SIZE(airoha_ecc_strength); i++)
			if (airoha_ecc_strength[i] >= required &&
			    airoha_ecc_strength[i] <= strength)
				break;
	} else {
		for (i = ARRAY_SIZE(airoha_ecc_strength) - 1; i >= 0; i--)
			if (airoha_ecc_strength[i] <= strength)
				break;
	}

	if (unlikely(i < 0 || i >= ARRAY_SIZE(airoha_ecc_strength))) {
		pr_err("OOB size (%u) is not supported\n", mtd->oobsize);
		return -EINVAL;
	}

	nand->ecc.strength = airoha_ecc_strength[i];
	nand->ecc.bytes = DIV_ROUND_UP(nand->ecc.strength * ECC_PARITY_BITS, 8);

	pr_debug("ECC strength adjusted to %u bits\n", nand->ecc.strength);

	return i;
}

static int airoha_nfc_set_spare_per_sector(struct airoha_nfc *nfc)
{
	struct nand_chip *nand = &nfc->nand;
	struct mtd_info *mtd = nand_to_mtd(nand);
	u32 size;
	int i;

	size = mtd->oobsize / nand->ecc.steps;

	for (i = ARRAY_SIZE(airoha_nfi_spare_size) - 1; i >= 0; i--)
		if (airoha_nfi_spare_size[i] <= size)
			break;

	if (unlikely(i < 0 ||
	    airoha_nfi_spare_size[i] < nand->ecc.bytes + NFI_FDM_SIZE)) {
		pr_err("OOB size (%u) is not supported\n", mtd->oobsize);
		return -EINVAL;
	}

	nfc->spare_per_sector = airoha_nfi_spare_size[i];

	return i;
}

static int airoha_nfc_ecc_init(struct airoha_nfc *nfc)
{
	struct nand_chip *nand = &nfc->nand;
	struct mtd_info *mtd = nand_to_mtd(nand);
	u32 avail_ecc_bytes, encode_block_size, decode_block_size;
	u32 ecc_enccfg, ecc_deccfg;
	int ecc_cap;

	nand->ecc.options |= NAND_ECC_CUSTOM_PAGE_ACCESS;

	nand->ecc.size = ECC_SECTOR_SIZE;
	nand->ecc.steps = mtd->writesize / nand->ecc.size;

	avail_ecc_bytes = mtd->oobsize / nand->ecc.steps - NFI_FDM_SIZE;

	ecc_cap = airoha_nfc_calc_ecc_strength(nfc, avail_ecc_bytes);
	if (ecc_cap < 0)
		return ecc_cap;

	encode_block_size = (nand->ecc.size + NFI_FDM_SIZE) * 8;
	ecc_enccfg = ecc_cap | FIELD_PREP(ENC_MODE, ENC_MODE_NFI) |
		     FIELD_PREP(ENC_CNFG_MSG, encode_block_size);

	decode_block_size = ((nand->ecc.size + NFI_FDM_SIZE) * 8) +
			    nand->ecc.strength * ECC_PARITY_BITS;
	ecc_deccfg = ecc_cap | FIELD_PREP(DEC_MODE, DEC_MODE_NFI) |
		     FIELD_PREP(DEC_CS, decode_block_size) |
		     FIELD_PREP(DEC_CON, DEC_CON_CORRECT) | DEC_EMPTY_EN;

	airoha_ecc_encoder_op(nfc, false);
	ecc_write32(nfc, ECC_ENCCNFG, ecc_enccfg);

	airoha_ecc_decoder_op(nfc, false);
	/*
	 * Decoder IRQ enable latches one DECDONE bit for each completed 512-byte
	 * sector. Polling these latched bits provides deterministic completion for
	 * the internal NFI-to-ECC data path.
	 */
	ecc_write32(nfc, ECC_DECCNFG, ecc_deccfg);
	ecc_write16(nfc, ECC_DECIRQEN, DEC_IRQEN);

	return 0;
}

static int airoha_nfc_set_page_format(struct airoha_nfc *nfc)
{
	struct nand_chip *nand = &nfc->nand;
	struct mtd_info *mtd = nand_to_mtd(nand);
	int i, spare_size;
	u32 pagefmt;

	spare_size = airoha_nfc_set_spare_per_sector(nfc);
	if (spare_size < 0)
		return spare_size;

	for (i = 0; i < ARRAY_SIZE(airoha_nfi_page_size); i++) {
		if (airoha_nfi_page_size[i] == mtd->writesize)
			break;
	}

	if (unlikely(i >= ARRAY_SIZE(airoha_nfi_page_size))) {
		pr_err("Page size (%u) is not supported\n", mtd->writesize);
		return -EINVAL;
	}

	pagefmt = FIELD_PREP(PAGEFMT_PAGE, i) |
		  FIELD_PREP(PAGEFMT_SPARE, spare_size) |
		  FIELD_PREP(PAGEFMT_FDM, NFI_FDM_SIZE) |
		  FIELD_PREP(PAGEFMT_FDM_ECC, NFI_FDM_SIZE);

	nfi_write16(nfc, NFI_PAGEFMT, pagefmt);

	return 0;
}

static int airoha_nfc_attach_chip(struct nand_chip *nand)
{
	struct airoha_nfc *nfc = nand_get_controller_data(nand);
	int ret;

	if (nand->options & NAND_BUSWIDTH_16) {
		pr_err("16-bit buswidth is not supported");
		return -EINVAL;
	}

	ret = airoha_nfc_ecc_init(nfc);
	if (ret)
		return ret;

	return airoha_nfc_set_page_format(nfc);
}

static void airoha_nfc_write_fdm(struct airoha_nfc *nfc)
{
	struct nand_chip *nand = &nfc->nand;
	u32 vall, valm;
	u8 *oobptr;
	int i, j;

	for (i = 0; i < nand->ecc.steps; i++) {
		vall = 0;
		valm = 0;
		oobptr = oob_fdm_ptr(nand, i);

		for (j = 0; j < 4; j++)
			vall |= (u32)oobptr[j] << (j * 8);

		for (j = 0; j < 4; j++)
			valm |= (u32)oobptr[j + 4] << (j * 8);

		nfi_write32(nfc, NFI_FDML(i), vall);
		nfi_write32(nfc, NFI_FDMM(i), valm);
	}
}

static void airoha_nfc_read_sector_fdm(struct airoha_nfc *nfc, u32 sect)
{
	struct nand_chip *nand = &nfc->nand;
	u32 vall, valm;
	u8 *oobptr;
	int i;

	vall = nfi_read32(nfc, NFI_FDML(sect));
	valm = nfi_read32(nfc, NFI_FDMM(sect));
	oobptr = oob_fdm_ptr(nand, sect);

	for (i = 0; i < 4; i++)
		oobptr[i] = (vall >> (i * 8)) & 0xff;

	for (i = 0; i < 4; i++)
		oobptr[i + 4] = (valm >> (i * 8)) & 0xff;
}

static int airoha_nfc_dma_transfer(struct airoha_nfc *nfc, void *buf,
				   size_t dma_len, size_t controller_len,
				   unsigned int sectors, bool read)
{
	enum dma_data_direction direction = read ? DMA_FROM_DEVICE : DMA_TO_DEVICE;
	dma_addr_t dma_addr;
	u32 config, count, misc, status, trigger;
	int ret;

	if (!dma_len || controller_len > FIELD_MAX(MISC_RD_BYTE_NUM))
		return -EINVAL;

	dma_addr = dma_map_single(buf, dma_len, direction);
	if (upper_32_bits(dma_addr)) {
		pr_err("EN7581 NFI DMA address exceeds 32 bits: 0x%llx\n",
		       (unsigned long long)dma_addr);
		dma_unmap_single(dma_addr, dma_len, direction);
		return -ERANGE;
	}

	config = nfi_read32(nfc, NFI_CNFG);
	config &= ~CNFG_BYTE_RW;
	config |= CNFG_AHB | CNFG_DMA_BURST_EN;
	if (read)
		config |= CNFG_READ_MODE;
	else
		config &= ~CNFG_READ_MODE;
	nfi_write32(nfc, NFI_CNFG, config);

	/*
	 * MISC_CTL2 bounds the physical page span. RD_CNT and WR_CNT account for
	 * NAND-side wire bytes, so AHB completion plus controller_len confirms that
	 * both main data and spare data have completed.
	 */
	misc = nfi_read32(nfc, NFI_MISC_CTL2);
	if (read) {
		misc &= ~MISC_RD_BYTE_NUM;
		misc |= FIELD_PREP(MISC_RD_BYTE_NUM, controller_len);
	} else {
		misc &= ~MISC_WR_BYTE_NUM;
		misc |= FIELD_PREP(MISC_WR_BYTE_NUM, controller_len);
	}
	nfi_write32(nfc, NFI_MISC_CTL2, misc);

	/* INTR_STA is read-clear; consume the previous completion before start. */
	(void)nfi_read32(nfc, NFI_INTR_STA);
	/* RD_CNT and WR_CNT must start at zero for this transfer. */
	nfi_write32(nfc, NFI_CNT_CLR, read ? CNT_CLR_RD : CNT_CLR_WR);
	nfi_write32(nfc, NFI_STRADDR, lower_32_bits(dma_addr));
	nfi_write32(nfc, NFI_CON, FIELD_PREP(CON_NFI_SEC, sectors));
	trigger = read ? CON_NFI_BRD : CON_NFI_BWR;
	nfi_write32(nfc, NFI_CON,
		    FIELD_PREP(CON_NFI_SEC, sectors) | trigger);

	ret = readl_poll_timeout(nfc->nfi_regs + NFI_INTR_STA, status,
				 status & INTR_AHB_DONE, NFI_CORE_TIMEOUT);
	/*
	 * AHB completion covers the DRAM payload. Auto-format then consumes FDM,
	 * BCH parity, and padding for every sector. Waiting for controller_len
	 * keeps CON asserted through the fourth codeword.
	 */
	if (!ret)
		ret = readl_poll_timeout(nfc->nfi_regs +
					 (read ? NFI_RD_CNT : NFI_WR_CNT),
					 count, count == controller_len,
					 NFI_CORE_TIMEOUT);
	else
		count = nfi_read32(nfc, read ? NFI_RD_CNT : NFI_WR_CNT);
	if (ret)
		pr_err("EN7581 %s DMA incomplete: expected=%zu count=%u sectors=%u intr=0x%08x\n",
		       read ? "read" : "write", controller_len, count, sectors,
		       status);

	nfi_write32(nfc, NFI_CON, 0);
	dma_unmap_single(dma_addr, dma_len, direction);
	return ret;
}

static int airoha_nfc_read_page_hwecc(struct mtd_info *mtd,
				      struct nand_chip *nand, uint8_t *buf,
				      int oob_required, int page)
{
	struct airoha_nfc *nfc = nand_get_controller_data(nand);
	u8 *workbuf = buf ? buf : airoha_dma_buffer;
	size_t wire_len = nand->ecc.steps *
			  (nand->ecc.size + nfc->spare_per_sector);
	u32 config;
	int bitflips = 0, ret = 0;
	int rc, i;

	config = FIELD_PREP(CNFG_OP_MODE, CNFG_OP_READ) |
		 CNFG_AHB | CNFG_DMA_BURST_EN | CNFG_READ_MODE |
		 CNFG_AUTO_FMT_EN | CNFG_HW_ECC_EN |
		 CNFG_ECC_DATA_SOURCE_INV;
	rc = airoha_nfc_begin_page_read(nand, page, config, true);
	if (rc) {
		ret = rc;
		goto out;
	}

	rc = airoha_nfc_dma_transfer(nfc, workbuf, mtd->writesize,
				     wire_len,
				     nand->ecc.steps, true);
	if (rc) {
		ret = rc;
		goto out;
	}

	for (i = 0; i < nand->ecc.steps; i++) {
		rc = airoha_ecc_decoder_wait_done(nfc, i);
		airoha_nfc_read_sector_fdm(nfc, i);

		if (rc < 0) {
			ret = -EIO;
			continue;
		}

		rc = airoha_ecc_correct_check(nfc, i);

		if (rc < 0) {
			pr_warn("Uncorrectable ECC error at page %d step %d\n",
				page, i);
			bitflips = nand->ecc.strength + 1;
			mtd->ecc_stats.failed++;
		} else {
			if (rc > bitflips)
				bitflips = rc;
			mtd->ecc_stats.corrected += rc;
		}
	}

out:
	airoha_ecc_decoder_op(nfc, false);
	nfi_write32(nfc, NFI_CNFG,
		    FIELD_PREP(CNFG_OP_MODE, CNFG_OP_IDLE));

	if (ret < 0)
		return ret;

	return bitflips;
}

static int airoha_nfc_read_page_raw(struct mtd_info *mtd,
				    struct nand_chip *nand, uint8_t *buf,
				    int oob_required, int page)
{
	struct airoha_nfc *nfc = nand_get_controller_data(nand);
	u8 *raw = airoha_dma_buffer;
	size_t sector_wire = nand->ecc.size + nfc->spare_per_sector;
	size_t formatted_wire = sector_wire * nand->ecc.steps;
	size_t raw_len = mtd->writesize + mtd->oobsize;
	u32 config;
	int i, ret;

	if (raw_len > sizeof(airoha_dma_buffer))
		return -E2BIG;
	memset(raw, 0xff, raw_len);

	config = FIELD_PREP(CNFG_OP_MODE, CNFG_OP_READ) |
		 CNFG_AHB | CNFG_DMA_BURST_EN | CNFG_READ_MODE;
	ret = airoha_nfc_begin_page_read(nand, page, config, false);
	if (ret)
		return ret;

	ret = airoha_nfc_dma_transfer(nfc, raw, formatted_wire,
				      formatted_wire,
				      nand->ecc.steps, true);
	if (ret)
		return ret;

	for (i = 0; i < nand->ecc.steps; i++) {
		if (buf)
			memcpy(page_data_ptr(nand, buf, i),
			       raw + i * sector_wire, nand->ecc.size);
		memcpy(oob_fdm_ptr(nand, i),
		       raw + i * sector_wire + nand->ecc.size, NFI_FDM_SIZE);
		memcpy(oob_ecc_ptr(nfc, i),
		       raw + i * sector_wire + nand->ecc.size + NFI_FDM_SIZE,
		       nfc->spare_per_sector - NFI_FDM_SIZE);
	}

	/* Bytes beyond the formatted 28-byte sector spares remain raw page OOB. */
	if (raw_len > formatted_wire)
		memcpy(nand->oob_poi + formatted_wire - mtd->writesize,
		       raw + formatted_wire, raw_len - formatted_wire);

	return 0;
}

static int airoha_nfc_read_oob_hwecc(struct mtd_info *mtd,
				     struct nand_chip *nand, int page)
{
	return airoha_nfc_read_page_hwecc(mtd, nand, NULL, 1, page);
}

static int airoha_nfc_read_oob_raw(struct mtd_info *mtd,
				   struct nand_chip *nand, int page)
{
	return airoha_nfc_read_page_raw(mtd, nand, NULL, 1, page);
}

static int airoha_nfc_check_empty_page(struct nand_chip *nand, const u8 *buf)
{
	struct mtd_info *mtd = nand_to_mtd(nand);
	u8 *oobptr;
	u32 i, j;

	if (buf) {
		for (i = 0; i < mtd->writesize; i++)
			if (buf[i] != 0xff)
				return 0;
	}

	for (i = 0; i < nand->ecc.steps; i++) {
		oobptr = oob_fdm_ptr(nand, i);
		for (j = 0; j < NFI_FDM_SIZE; j++)
			if (oobptr[j] != 0xff)
				return 0;
	}

	return 1;
}

static int airoha_nfc_write_page_hwecc(struct mtd_info *mtd,
				       struct nand_chip *nand,
				       const u8 *buf, int oob_required,
				       int page)
{
	struct airoha_nfc *nfc = nand_get_controller_data(nand);
	u8 *workbuf = (u8 *)buf;
	size_t wire_len = nand->ecc.steps *
			  (nand->ecc.size + nfc->spare_per_sector);
	u8 column_cycles = mtd->writesize > 512 ? 2 : 1;
	u8 row_cycles = nand->options & NAND_ROW_ADDR_3 ? 3 : 2;
	u8 status;
	u32 config;
	int ret;

	if (airoha_nfc_check_empty_page(nand, buf)) {
		/*
		 * The ECC engine generates parity for all-0xff input. Leaving an erased
		 * page untouched preserves all-0xff main and OOB semantics.
		 */
		return 0;
	}

	if (!workbuf) {
		memset(airoha_dma_buffer, 0xff, mtd->writesize);
		workbuf = airoha_dma_buffer;
	}

	/*
	 * SEQIN latches encoder, FDM, and auto-format state. STATUS after PAGEPROG
	 * reports the media program result.
	 */
	airoha_nfc_hw_reset(nfc);
	config = FIELD_PREP(CNFG_OP_MODE, CNFG_OP_PROGRAM) |
		 CNFG_AHB | CNFG_DMA_BURST_EN | CNFG_AUTO_FMT_EN |
		 CNFG_HW_ECC_EN | CNFG_ECC_DATA_SOURCE_INV;
	nfi_write32(nfc, NFI_CNFG, config);
	airoha_ecc_encoder_op(nfc, true);
	airoha_nfc_write_fdm(nfc);
	ret = airoha_nfc_issue_command(nfc, NAND_CMD_SEQIN);
	if (!ret)
		ret = airoha_nfc_send_address(nfc, 0, page,
					      column_cycles, row_cycles);
	if (ret)
		goto out;

	ret = airoha_nfc_dma_transfer(nfc, workbuf, mtd->writesize,
				      wire_len,
				      nand->ecc.steps, false);
	if (!ret)
		ret = airoha_nfc_issue_command(nfc, NAND_CMD_PAGEPROG);
	if (!ret)
		ret = airoha_nfc_wait_ready(nfc);

out:
	airoha_ecc_encoder_op(nfc, false);
	nfi_write32(nfc, NFI_CNFG,
		    FIELD_PREP(CNFG_OP_MODE, CNFG_OP_IDLE));
	if (ret)
		return ret;

	ret = nand_status_op(nand, &status);
	if (ret)
		return ret;

	return status & NAND_STATUS_FAIL ? -EIO : 0;
}

static int airoha_nfc_write_page_raw(struct mtd_info *mtd,
				     struct nand_chip *nand,
				     const u8 *buf, int oob_required,
				     int page)
{
	struct airoha_nfc *nfc = nand_get_controller_data(nand);
	u8 *raw = airoha_dma_buffer;
	size_t sector_wire = nand->ecc.size + nfc->spare_per_sector;
	size_t formatted_wire = sector_wire * nand->ecc.steps;
	size_t raw_len = mtd->writesize + mtd->oobsize;
	u8 column_cycles = mtd->writesize > 512 ? 2 : 1;
	u8 row_cycles = nand->options & NAND_ROW_ADDR_3 ? 3 : 2;
	u8 status;
	u32 config;
	int i, ret;

	if (raw_len > sizeof(airoha_dma_buffer))
		return -E2BIG;
	memset(raw, 0xff, raw_len);
	for (i = 0; i < nand->ecc.steps; i++) {
		if (buf)
			memcpy(raw + i * sector_wire,
			       page_data_ptr(nand, buf, i), nand->ecc.size);
		memcpy(raw + i * sector_wire + nand->ecc.size,
		       oob_fdm_ptr(nand, i), NFI_FDM_SIZE);
		memcpy(raw + i * sector_wire + nand->ecc.size + NFI_FDM_SIZE,
		       oob_ecc_ptr(nfc, i),
		       nfc->spare_per_sector - NFI_FDM_SIZE);
	}
	if (raw_len > formatted_wire)
		memcpy(raw + formatted_wire,
		       nand->oob_poi + formatted_wire - mtd->writesize,
		       raw_len - formatted_wire);

	airoha_nfc_hw_reset(nfc);
	config = FIELD_PREP(CNFG_OP_MODE, CNFG_OP_PROGRAM) |
		 CNFG_AHB | CNFG_DMA_BURST_EN;
	nfi_write32(nfc, NFI_CNFG, config);
	ret = airoha_nfc_issue_command(nfc, NAND_CMD_SEQIN);
	if (!ret)
		ret = airoha_nfc_send_address(nfc, 0, page,
					      column_cycles, row_cycles);
	if (ret)
		goto out;

	/* The sector engine sends four 512+spare units; trailing OOB stays erased. */
	ret = airoha_nfc_dma_transfer(nfc, raw, formatted_wire,
				      formatted_wire,
				      nand->ecc.steps, false);
	if (!ret)
		ret = airoha_nfc_issue_command(nfc, NAND_CMD_PAGEPROG);
	if (!ret)
		ret = airoha_nfc_wait_ready(nfc);

out:
	nfi_write32(nfc, NFI_CNFG,
		    FIELD_PREP(CNFG_OP_MODE, CNFG_OP_IDLE));
	if (ret)
		return ret;

	ret = nand_status_op(nand, &status);
	if (ret)
		return ret;

	return status & NAND_STATUS_FAIL ? -EIO : 0;
}

static int airoha_nfc_write_oob_hwecc(struct mtd_info *mtd,
				      struct nand_chip *nand, int page)
{
	return airoha_nfc_write_page_hwecc(mtd, nand, NULL, 1, page);
}

static int airoha_nfc_write_oob_raw(struct mtd_info *mtd,
				    struct nand_chip *nand, int page)
{
	return airoha_nfc_write_page_raw(mtd, nand, NULL, 1, page);
}

static int airoha_nfc_ooblayout_free(struct mtd_info *mtd, int section,
				     struct mtd_oob_region *oob_region)
{
	struct nand_chip *nand = mtd_to_nand(mtd);

	if (section >= nand->ecc.steps)
		return -ERANGE;

	oob_region->length = NFI_FDM_SIZE - 1;
	oob_region->offset = section * NFI_FDM_SIZE + 1;

	return 0;
}

static int airoha_nfc_ooblayout_ecc(struct mtd_info *mtd, int section,
				    struct mtd_oob_region *oob_region)
{
	struct nand_chip *nand = mtd_to_nand(mtd);

	if (section)
		return -ERANGE;

	oob_region->offset = NFI_FDM_SIZE * nand->ecc.steps;
	oob_region->length = mtd->oobsize - oob_region->offset;

	return 0;
}

static const struct mtd_ooblayout_ops airoha_nfc_ooblayout_ops = {
	.rfree = airoha_nfc_ooblayout_free,
	.ecc = airoha_nfc_ooblayout_ecc,
};

/* Syndrome pages require a raw OOB read at the physical marker position. */
static int airoha_nfc_block_bad(struct mtd_info *mtd, loff_t ofs)
{
	struct nand_chip *nand = mtd_to_nand(mtd);
	struct mtd_oob_ops ops;
	int ret, i = 0;
	u16 bad = 0xffff;

	memset(&ops, 0, sizeof(ops));
	ops.oobbuf = (uint8_t *)&bad;
	ops.ooboffs = nand->badblockpos;
	if (nand->options & NAND_BUSWIDTH_16) {
		ops.ooboffs &= ~0x01;
		ops.ooblen = 2;
	} else {
		ops.ooblen = 1;
	}
	ops.mode = MTD_OPS_RAW;

	if (nand->bbt_options & NAND_BBT_SCANLASTPAGE)
		ofs += mtd->erasesize - mtd->writesize;

	do {
		ret = mtd_read_oob(mtd, ofs, &ops);
		if (ret)
			return ret;

		/* Raw OOB reads fill one marker byte on x8 NAND and a word on x16 NAND. */
		if (likely(nand->badblockbits == 8)) {
			if (nand->options & NAND_BUSWIDTH_16)
				ret = bad != 0xffff;
			else
				ret = (bad & 0xff) != 0xff;
		} else {
			ret = hweight8(bad & 0xff) < nand->badblockbits;
		}

		i++;
		ofs += mtd->writesize;
	} while (!ret && (nand->bbt_options & NAND_BBT_SCAN2NDPAGE) && i < 2);

	return ret;
}

static int airoha_nfc_init_chip(struct airoha_nfc *nfc)
{
	struct nand_chip *nand = &nfc->nand;
	struct mtd_info *mtd;
	int ret;

	nand_set_controller_data(nand, nfc);

	/*
	 * block_bad() reads the physical OOB marker when an eraseblock is accessed.
	 * This on-demand check bounds startup time on the 2048-block device while
	 * preserving bad-block decisions for NAND, MTD, and UBI operations.
	 */
	nand->options |= NAND_NO_SUBPAGE_WRITE | NAND_SKIP_BBTSCAN;

	nand->ecc.mode = NAND_ECC_HW_SYNDROME;
	nand->ecc.read_page = airoha_nfc_read_page_hwecc;
	nand->ecc.read_page_raw = airoha_nfc_read_page_raw;
	nand->ecc.write_page = airoha_nfc_write_page_hwecc;
	nand->ecc.write_page_raw = airoha_nfc_write_page_raw;
	nand->ecc.read_oob = airoha_nfc_read_oob_hwecc;
	nand->ecc.read_oob_raw = airoha_nfc_read_oob_raw;
	nand->ecc.write_oob = airoha_nfc_write_oob_hwecc;
	nand->ecc.write_oob_raw = airoha_nfc_write_oob_raw;

	nand->dev_ready = airoha_nfc_dev_ready;
	nand->select_chip = airoha_nfc_select_chip;
	nand->write_byte = airoha_nfc_write_byte;
	nand->write_buf = airoha_nfc_write_buf;
	nand->read_byte = airoha_nfc_read_byte;
	nand->read_buf = airoha_nfc_read_buf;
	nand->cmdfunc = airoha_nfc_command;
	nand->block_bad = airoha_nfc_block_bad;

	mtd = nand_to_mtd(nand);
	mtd_set_ooblayout(mtd, &airoha_nfc_ooblayout_ops);

	airoha_nfc_hw_init(nfc);

	ret = nand_scan_ident(mtd, 1, NULL);
	if (ret) {
		pr_err("EN7581 parallel NAND identification failed: %d\n", ret);
		return ret;
	}

	printf("EN7581 parallel NAND: ID %02x:%02x, page %u, OOB %u, erase %u\n",
	       nand->id.data[0], nand->id.data[1], mtd->writesize,
	       mtd->oobsize, mtd->erasesize);
	printf("EN7581 parallel NAND: onfi_ver=%d ecc_strength_ds=%u onfi_ecc_bits=%u\n",
	       nand->onfi_version, nand->ecc_strength_ds,
	       nand->onfi_params.ecc_bits);

	ret = airoha_nfc_attach_chip(nand);
	if (ret) {
		pr_err("EN7581 parallel NAND ECC setup failed: %d\n", ret);
		return ret;
	}

	ret = nand_scan_tail(mtd);
	if (ret) {
		pr_err("EN7581 parallel NAND scan tail failed: %d\n", ret);
		return ret;
	}

	nand_register(0, mtd);
	printf("EN7581 parallel NAND registered: ECC%u/%u, spare %u/sector\n",
	       nand->ecc.strength, nand->ecc.size, nfc->spare_per_sector);
	return 0;
}

static void airoha_nfc_set_regs(struct airoha_nfc *nfc)
{
	nfc->nfi_regs = (void __iomem *)NFI_BASE;
	nfc->ecc_regs = (void __iomem *)NFI_ECC_BASE;
}

static int airoha_nfc_probe(struct udevice *dev)
{
	struct airoha_nfc *nfc = dev_get_priv(dev);
	struct clk nfi_clk;
	ofnode flash_node;
	ulong clock_rate;
	int ret;

	airoha_nfc_set_regs(nfc);

	/*
	 * PNAND shares the SoC SPI clock with SNFI. The default pinctrl state routes
	 * GPIO4-7 and GPIO30-42 to the parallel bus before probe.
	 */
	ret = clk_get_by_index(dev, 0, &nfi_clk);
	if (ret) {
		dev_err(dev, "failed to get NFI clock: %d\n", ret);
		return ret;
	}
	ret = clk_enable(&nfi_clk);
	if (ret) {
		dev_err(dev, "failed to enable NFI clock: %d\n", ret);
		return ret;
	}
	clock_rate = clk_get_rate(&nfi_clk);
	flash_node = ofnode_first_subnode(dev_ofnode(dev));
	if (!ofnode_valid(flash_node)) {
		dev_err(dev, "missing NAND chip child node\n");
		return -ENODEV;
	}
	nand_set_flash_node(&nfc->nand, flash_node);

	{
		u32 acccon = nfi_read32(nfc, NFI_ACCCON);
		u32 wh = FIELD_GET(ACCCON_WH, acccon);
		u32 wst = FIELD_GET(ACCCON_WST, acccon);
		u32 rlt = FIELD_GET(ACCCON_RLT, acccon);
		u32 wr_mhz_x10, rd_mhz_x10;

		wr_mhz_x10 = (u32)((u64)clock_rate * 10 / (wh + wst) / 1000000);
		rd_mhz_x10 = (u32)((u64)clock_rate * 10 / rlt / 1000000);

		printf("EN7581 NFI: mode=0x%08x acccon=0x%08x clock=%lu Hz\n",
		       nfi_read32(nfc, NFI_MODE), acccon, clock_rate);
		printf("  NAND bus: write %u.%u MHz (WH+WST=%u), read %u.%u MHz (RLT=%u)\n",
		       wr_mhz_x10 / 10, wr_mhz_x10 % 10, wh + wst,
		       rd_mhz_x10 / 10, rd_mhz_x10 % 10, rlt);
	}
	if (nfi_read32(nfc, NFI_MODE) & NFI_MODE_SERIAL) {
		dev_err(dev, "NFI is configured for serial NAND\n");
		return -ENODEV;
	}

	return airoha_nfc_init_chip(nfc);
}

static const struct udevice_id airoha_nfc_ids[] = {
	{ .compatible = "airoha,en7581-nfc" },
	{ }
};

U_BOOT_DRIVER(airoha_en7581_nand) = {
	.name = "airoha_en7581_nand",
	.id = UCLASS_MTD,
	.of_match = airoha_nfc_ids,
	.probe = airoha_nfc_probe,
	.priv_auto = sizeof(struct airoha_nfc),
};

void airoha_nfc_spl_init(struct airoha_nfc *nfc)
{
	struct nand_chip *nand = &nfc->nand;

	airoha_nfc_set_regs(nfc);

	nand_set_controller_data(nand, nfc);

	nand->options |= NAND_NO_SUBPAGE_WRITE;

	nand->ecc.mode = NAND_ECC_HW_SYNDROME;
	nand->ecc.read_page = airoha_nfc_read_page_hwecc;

	nand->dev_ready = airoha_nfc_dev_ready;
	nand->select_chip = airoha_nfc_select_chip;
	nand->read_byte = airoha_nfc_read_byte;
	nand->read_buf = airoha_nfc_read_buf;
	nand->cmdfunc = airoha_nfc_command;

	airoha_nfc_hw_init(nfc);
}

int airoha_nfc_spl_post_init(struct airoha_nfc *nfc)
{
	struct nand_chip *nand = &nfc->nand;
	int nand_maf_id, nand_dev_id;
	int ret;

	ret = nand_detect(nand, &nand_maf_id, &nand_dev_id, NULL);

	if (ret)
		return ret;

	nand->numchips = 1;
	nand->mtd.size = nand->chipsize;

	return airoha_nfc_attach_chip(nand);
}

void board_nand_init(void)
{
	struct udevice *dev;
	int ret;

	ret = uclass_get_device_by_driver(UCLASS_MTD,
					  DM_DRIVER_GET(airoha_en7581_nand),
					  &dev);
	if (ret && ret != -ENODEV)
		pr_err("Failed to initialize EN7581 parallel NAND: %d\n", ret);
}
