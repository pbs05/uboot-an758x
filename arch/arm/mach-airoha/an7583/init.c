// SPDX-License-Identifier: GPL-2.0

#include <fdtdec.h>
#include <init.h>
#include <linux/sizes.h>
#include <sysreset.h>
#include <asm/armv8/mmu.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/system.h>

DECLARE_GLOBAL_DATA_PTR;

#define AN7583_SCREG_WR1		0x1fb00284
#define AN7583_DRAM_SIZE_SHIFT		20
#define AN7583_DRAM_SIZE_MASK		0x3ff

int print_cpuinfo(void)
{
	printf("CPU:   Airoha AN7583\n");
	return 0;
}

int dram_init(void)
{
	u32 units;

	/* BL2 stores the dramc_main() result in SCREG_WR1[29:20] in 16 MiB units. */
	units = (readl(AN7583_SCREG_WR1) >> AN7583_DRAM_SIZE_SHIFT) &
		AN7583_DRAM_SIZE_MASK;
	if (!units)
		return fdtdec_setup_mem_size_base();

	gd->ram_base = 0x80000000;
	gd->ram_size = (phys_size_t)units * SZ_16M;

	return 0;
}

int dram_init_banksize(void)
{
	gd->bd->bi_dram[0].start = gd->ram_base;
	gd->bd->bi_dram[0].size = gd->ram_size > SZ_2G ?
		SZ_2G : get_effective_memsize();

#if CONFIG_NR_DRAM_BANKS > 1
	if (gd->ram_size > SZ_2G) {
		gd->bd->bi_dram[1].start = gd->ram_base + SZ_2G;
		gd->bd->bi_dram[1].size = gd->ram_size - SZ_2G;
	}
#endif

	return 0;
}

void reset_cpu(void)
{
	psci_system_reset();
}

static struct mm_region an7583_mem_map[] = {
	{
		/* DDR, 32-bit area */
		.virt = 0x80000000UL,
		.phys = 0x80000000UL,
		.size = SZ_2G,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_OUTER_SHARE,
	}, {
		/* DDR, 64-bit area */
		.virt = 0x100000000UL,
		.phys = 0x100000000UL,
		.size = SZ_4G + SZ_2G,
		.attrs = PTE_BLOCK_MEMTYPE(MT_NORMAL) | PTE_BLOCK_OUTER_SHARE,
	}, {
		.virt = 0x00000000UL,
		.phys = 0x00000000UL,
		.size = 0x40000000UL,
		.attrs = PTE_BLOCK_MEMTYPE(MT_DEVICE_NGNRNE) |
			 PTE_BLOCK_NON_SHARE |
			 PTE_BLOCK_PXN | PTE_BLOCK_UXN
	}, {
		/* List terminator */
	}
};
struct mm_region *mem_map = an7583_mem_map;
