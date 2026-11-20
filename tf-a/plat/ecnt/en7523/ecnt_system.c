
#include <plat_private.h>
#include <assert.h>
#include <asm/tc3162.h>
#include <lib/mmio.h>
#include <en7523_def.h>
#include <drivers/delay_timer.h>
#if defined(TCSUPPORT_CPU_EN7523)
#include "ecnt_cpufreq.h"
#endif

#define FPGA_SYS_HCLK 40
#if defined(TCSUPPORT_CPU_AN7583)
#define ASIC_SYS_HCLK 100
#elif defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
#define ASIC_SYS_HCLK 300
#else
#define ASIC_SYS_HCLK 250
#endif

#define RBUS_REG_BASE	    (0x1fa00000)
#define TIMEOUT_STS0		(RBUS_REG_BASE + 0xd0)
#define TIMEOUT_STS1		(RBUS_REG_BASE + 0xd4)
#define DMA_BLOCK_EN		(RBUS_REG_BASE + 0xec)
#define DMA_BLOCK_MASK		(RBUS_REG_BASE + 0xf0)
#if defined(TCSUPPORT_CPU_AN7552) || defined(TCSUPPORT_CPU_AN7583)
#if defined(TCSUPPORT_CPU_AN7552)
#define CPU_WRR_CFG0		(RBUS_REG_BASE + 0x1c4)
#define CPU_WRR_CFG1		(RBUS_REG_BASE + 0x1c8)

#define DRAM_HW_CONF		(0x1fb0008c)
#endif
#define DRAM_CONF_REG       (0x1fb00074)
void rbus2dram_out_of_order_enable(int enable);
#endif
#define DRAMC_CONF1_REG     (0x1fb00974)

/*
There are only two options for low_ddr_frequency_settings.
1:    low ddr frequency
0: normal ddr frequency
*/
#if defined(TCSUPPORT_LOW_DDR_FREQ)
unsigned int low_ddr_frequency_settings = 1;
#else
unsigned int low_ddr_frequency_settings = 0;
#endif

typedef union {
	struct {
		unsigned int intr		: 1;
		unsigned int intr_mask  : 1;
		unsigned int wCmd		: 1;
        unsigned int rCmd		: 1;
        unsigned int wData		: 1;
        unsigned int rData		: 1;
		unsigned int isWrite	: 1;
		unsigned int wrap		: 1;
		unsigned int len		: 8;
        unsigned int resv		: 15;
		unsigned int tout_on	: 1;
	} bits;
	unsigned int word;
} rbusToutInfo_t;

#define CHIP_SCU_BASE       (0x1fa20000)
#define RG_BUS_CLKDIV_SEL   (CHIP_SCU_BASE+0x01bc)


#pragma weak dramc_main

extern void en7523_packageID_init(void);
extern uint32_t uartDisable;
/* function mainly for FPGA use
 * ASIC dramc_main() will be defined in dramc driver
 */
unsigned long long dramc_main(void)
{
	#if defined(TCSUPPORT_CPU_AN7552) || defined(TCSUPPORT_CPU_AN7583)
	rbus2dram_out_of_order_enable(1); /* must be set before any dram access */
	#endif

	mmio_write_32(0x1fb00040, 1);
	mmio_write_32(0x1fb00040, 0);
    #if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7552)
    if ((is_asic() == 0)) { /* fpga */
        __asm__ volatile ("dsb sy");
        udelay(100000);
    }
    #endif
	return EN7523_MEM_SIZE;
}

static void check_fpga(void)
{
	unsigned int _isFPGA = 0;

	if ((is_asic() == 0))
		_isFPGA = 1;

	SET_IS_FPGA(_isFPGA);
}

#if defined(TCSUPPORT_CPU_EN7523)

static void check_pkg_id(void)
{
	if (is_asic()) {
		if (isAN7552) {
			if(!(isAN7552CT || isAN7552ST || isAN7563CT || isAN7563PT ||
				isAN7552FT)){
				ERROR("Unknow AN7552 Package ID.\n");
				panic();
			}
		} else if (isEN7581) {
			if(!(isAN7581GT || isAN7581PT || isAN7581CT || isAN7581DT ||
				isAN7581ST || isAN7551PT || isAN7566PT || isAN7581IT ||
				isAN7581FG || isAN7566GT || isAN7581FP || isAN7581FD ||
				isAN7551GT )) {
				ERROR("Unknow AN7581 Package ID.\n");
				panic();
			}
		} else if(isEN7523) {
			if(!(isEN7529DU || isEN7529DT || isEN7529CU || isEN7562DU ||
				isEN7562DT || isEN7562CU || isEN7523GU || isEN7523DU ||
				isEN7529GTH || isEN7562GTH || isEN7523SU || isEN7529GTS ||
				isEN7562GTS || isEN7529IT || isEN7529CT || isEN7562CT ||
				isEN7523DT || isEN7529DTM || isEN7562DTM || isEN7529ITM ||
				isEN7529CTM || isEN7562CTM || isEN7523DTM)) {
				ERROR("Unknow EN7523 Package ID.\n");
				panic();
			}
		} else if(isAN7583){
			if(!(isAN7583GT || isAN7583GIT || isAN7583CT || isAN7583DT ||
				isAN7583ST  || isAN9510GT  || isAN7553GT || isAN7583FG ||
				isAN7553CT  || isAN7567GT  || isAN7567CT || isAN7583ET ||
				isAN7583EIT || isAN7583FP  || isAN7583FD || isAN7583FS ||isAN7583FF)){
				ERROR("Unknow AN7583 Package ID.\n");
				panic();
			}
		}
	}
}

#ifdef TCSUPPORT_EMMC
uint8_t support_emmc_feature (void)
{
	uint8_t support = 1;

	if (is_asic()) {
		if(isEN7581) {
			if(isAN7581CT || isAN7581DT) {
				support = 0;
			}
		}
	}

	return support;
}
#endif

void disable_EFUSE(void)
{
	mmio_clrbits_32(0x1fa201e4, ((uint32_t)1 << 19));
	mmio_clrbits_32(0x1fa201ec, ((uint32_t)1 << 3));
}

void enable_EFUSE(void)
{
	mmio_setbits_32(0x1fa201e4, ((uint32_t)1 << 19));
	mmio_setbits_32(0x1fa201ec, ((uint32_t)1 << 3));
}

static void power_saving_config(void)
{
	/* set io driving to 4mA */
	mmio_write_32(0x1fa2001c, 0x3FFF);
	mmio_write_32(0x1fa20020, 0);
	mmio_write_32(0x1fa20024, 0xFFFFFFFF);
	mmio_write_32(0x1fa20028, 0);

	/* close TRNG */
	mmio_clrbits_32(0x1fa201e4, ((uint32_t)1 << 3));
	mmio_clrbits_32(0x1fa201e8, ((uint32_t)1 << 21));
	mmio_clrbits_32(0x1fa201ec, ((uint32_t)1 << 0));

	/* close I2C_Slave */
	mmio_clrbits_32(0x1fa201e4, ((uint32_t)1 << 18));

	/* close MDIO_Slave */
	mmio_clrbits_32(0x1fa201e4, ((uint32_t)1 << 26));

	/* close GDMP */
	//mmio_clrbits_32(0x1fa201e4, ((uint32_t)1 << 21));
	//mmio_clrbits_32(0x1fa201e8, ((uint32_t)1 << 7));

	/* disable rom clock */
	//mmio_clrbits_32(0x1fa201e4, ((uint32_t)1 << 31));

	/* disable EFUSE */
	//disable_EFUSE();

	/* disable ToD */
	mmio_clrbits_32(0x1fa201e8, ((uint32_t)1 << 27));

	/* disable PCM2 ZSI Clock */
	mmio_clrbits_32(0x1fa201e8, ((uint32_t)1 << 1));
	/* disable PCM2 ISI Clock */
	mmio_clrbits_32(0x1fa201e8, ((uint32_t)1 << 3));
}

#if (defined(TCSUPPORT_CPU_EN7581) && !defined(TCSUPPORT_CPU_AN7583)) || defined(TCSUPPORT_CPU_AN7552)
static unsigned int get_bus_clk_per_us(void)
{
    unsigned int val = mmio_read_32(RG_BUS_CLKDIV_SEL);
    int clk_src = ((val>>8)&0x1);
    int clk_div = (val&0x7);
    unsigned int clk_per_us;

    if (clk_src==0)
        clk_per_us = 600;
    else
        clk_per_us = 540;

    if (clk_div==0) {
        printf("Error(%s) clk_div:%d is wrong (RG_BUS_CLKDIV_SEL:0x%x)\n", __func__, clk_div, mmio_read_32(RG_BUS_CLKDIV_SEL));
        return 0;
    }

    return (clk_per_us/(clk_div+1));
}

#if defined(TCSUPPORT_CPU_EN7581)
static void driving_strength_config(void)
{
	mmio_write_32(0x1fa2001c,0);
	mmio_write_32(0x1fa20020,0);
	mmio_write_32(0x1fa20024,0);
	mmio_write_32(0x1fa20028,0);
	mmio_write_32(0x1fa2002c,0);
	mmio_write_32(0x1fa20030,0);
	mmio_write_32(0x1fa20034,0);
	mmio_write_32(0x1fa20038,0);
	mmio_write_32(0x1fa2003c,0);
	mmio_write_32(0x1fa20040,0);
}
#endif
#endif

static void en7523_init(void)
{
    unsigned int val = ASIC_SYS_HCLK;

	if ((is_asic() == 0))
	{
		SET_SYS_CLK(FPGA_SYS_HCLK);
	}
	else
	{
		#if (defined(TCSUPPORT_CPU_EN7581) && !defined(TCSUPPORT_CPU_AN7583)) || defined(TCSUPPORT_CPU_AN7552)
        val = get_bus_clk_per_us();
        if (val==0)
            val = ASIC_SYS_HCLK;
        #endif
        SET_SYS_CLK(val);
	}

	//en7580_setup_xtal_driving()
	en7523_packageID_init();

	/* only allow existing package ID */
	check_pkg_id();

	/*TODO: add EN7581 power saving support*/
	if(isEN7523)
	{
		power_saving_config();
	}
	
#if 0//defined(TCSUPPORT_CPU_EN7581)
	/*EN7581 SDK selects DDR4 16bit as default package*/
	SET_R2C_MODE(R2C_NEW128);
#endif

}
#endif

void ecnt_cpu_speedup(void)
{
#if defined(TCSUPPORT_CPU_EN7523)
	if ((is_asic() == 0))
			return;

	if(isEN7523) {
		if(isEN7529CU || isEN7529DU || isEN7529CT || isEN7529DT ||
		   isEN7529IT || isEN7529GTH || isEN7529GTS || isEN7562CU ||
		   isEN7562DU || isEN7562CT || isEN7562DT || isEN7562GTH ||
		   isEN7562GTS || isEN7529DTM || isEN7562DTM || isEN7529ITM ||
		   isEN7529CTM || isEN7562CTM)
			en7523_armpll_set(cpu_freq_950M);
		else if (isEN7523SU)
			en7523_armpll_set(cpu_freq_550M);
		else if (isEN7523GU || isEN7523DU || isEN7523DT || isEN7523DTM)
			en7523_armpll_set(cpu_freq_750M);
		else {
			ERROR("(%s)Unknow EN7523 Package ID.\n", __func__);
			return;
		}
	}
#if defined(TCSUPPORT_CPU_EN7581)
	else if(isEN7581)
	{
		/* Boot at 1200 MHz; 1250-1400 are left to the cpufreq OPP */
   	     en7523_armpll_set(cpu_freq_1200M);
	}
#endif
	else if(isAN7552)
	{
		an7552_bootup_clk_src_switch(cpu_freq_1000M);
	}
#if defined(TCSUPPORT_CPU_AN7583)
	else if(isAN7583)
	{
		an7552_bootup_clk_src_switch(cpu_freq_1200M);
	}
#endif
	else {
		ERROR("(%s)Unknow Chip ID.\n", __func__);
		return;
	}

  	  /* only enable ARMPLL */
	set_cpu_domain_clk_gating(cpu_clk_pll1, 0);
	set_cpu_domain_clk_gating(cpu_clk_pll2, 0);
	set_cpu_domain_clk_gating(cpu_clk_armpll_div2, 0);
#endif

  	return;
}

/* If rbus timeout happended, print the messages for debug*/
void rbus_timeout_check(void)
{
    rbusToutInfo_t toutInfo;

	if ((mmio_read_32(TIMEOUT_STS0) & 0x1) == 1)
	{
		printf("\r\nERROR !!! Rbus timeout happended before !!!\r\n");
		printf("\r\nThe Rbus timeout status is as below\r\n");
		printf("\r\nerrAddr = %x \r\n", mmio_read_32(TIMEOUT_STS1));
        toutInfo.word = mmio_read_32(TIMEOUT_STS0);
        printf("\r\nerrInfo = %x\r\n", toutInfo.word);
        printf("\t->rbus_timeout_on: %d\r\n", toutInfo.bits.tout_on);
        printf("\t->rbus_length: 0x%x\r\n", toutInfo.bits.len);
        printf("\t->rbus_wrap: %d\r\n", toutInfo.bits.wrap);
        printf("\t->is_write: %d\r\n", toutInfo.bits.isWrite);
        printf("\t->rd_data_timeout: %d\r\n", toutInfo.bits.rData);
        printf("\t->wt_data_timeout: %d\r\n", toutInfo.bits.wData);
        printf("\t->rd_cmd_timeout: %d\r\n", toutInfo.bits.rCmd);
        printf("\t->wt_cmd_timeout: %d\r\n", toutInfo.bits.wCmd);
        printf("\t->intr_mask_on: %d\r\n", toutInfo.bits.intr_mask);
        printf("\t->intr_issued: %d\r\n", toutInfo.bits.intr);
		/* clear the status*/
		mmio_write_32(TIMEOUT_STS0, 0);
		mmio_write_32(TIMEOUT_STS1, 0);
	}
}

#if defined(TCSUPPORT_CPU_AN7552) || defined(TCSUPPORT_CPU_AN7583)
void rbus2dram_out_of_order_enable(int enable)
{
    unsigned int val;

    /* Note: Wdog_reset can but SW_reset can't reset the bit31 */
    val = mmio_read_32(DRAM_CONF_REG);
    if (enable) {
        printf("bus2dram out_of_order mode\r\n");
        val |= (0x1<<31);
    }
    else {
        printf("bus2dram in_order mode\r\n");
        val &= (~(0x1<<31));
    }
    mmio_write_32(DRAM_CONF_REG, val);

    return;
}
#endif

#if defined(TCSUPPORT_CPU_AN7583)
void r2c_rd_bypass_wt_enable(int enable)
{
	unsigned int val;

	val = mmio_read_32(DRAMC_CONF1_REG);
	if (enable) {
		INFO("enable r2c_rd_bypass_wt\r\n");
		val |= (0x1<<5);
	}
	else {
		INFO("disable r2c_rd_bypass_wt\r\n");
		val &= (~(0x1<<5));
	}
	mmio_write_32(DRAMC_CONF1_REG, val);

	return;
}
#endif

/* bus related setting/configs */
void bus_config(void)
{
	/* Note: bus2dram inorder/out_of_order config is set in dramc_main() */
	#if defined(TCSUPPORT_CPU_AN7583)
	r2c_rd_bypass_wt_enable(1);

	/* crypto 256B boundary cross cut enable (CRYPTO_RAM_PWRDOWN 0x1FB70108[13]) */
	mmio_setbits_32(0x1FB70108, (uint32_t)0x1 << 13);
	#endif /* TCSUPPORT_CPU_AN7583 */
}

int ecnt_system_init(unsigned long long *p_dram_size)
{
#if LOG_LEVEL >= LOG_LEVEL_INFO
	unsigned int hwtrap = mmio_read_32(0x1fb000b4);

	INFO("HWCONF is %x\n", hwtrap);
#endif
	check_fpga();

	bus_config();
#if defined(TCSUPPORT_CPU_EN7523) || defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_EN7581)
	if(isEN7523 || isEN7581 || isAN7552 || isAN7583)
	{
		en7523_init();
#if defined(TCSUPPORT_CPU_EN7581) && !defined(TCSUPPORT_CPU_AN7583)
		driving_strength_config();
		/*set EMI clock*/
		mmio_write_32(0x1fa201b8, 0x0);
#endif

		if(isEN7581 || isAN7552)
		{
			/* set MDIO clk to 1.5M*/
			mmio_setbits_32(0x1fa2020c, ((uint32_t)0xf << 8));
		}

		if(isAN7552)
		{
			/* set IOMUX for I2C/MDIO 
				AN7563CT/AN7563PT -> MDIO
				AN7552CT/AN7552ST/AN7552FT -> I2C
			*/
			if(isAN7563CT || isAN7563PT)
			{
				mmio_setbits_32(0x1fa2021c, (uint32_t)0x1 << 13);
			}
			else
			{
				mmio_clrbits_32(0x1fa2021c, (uint32_t)0x1 << 13);
			}
		}

		ecnt_cpu_speedup();
		/* when boot from flash, ejtag will default on, and LAN LED is wrong. */
		set_boot_from_spi_ejtag_enable(0);
	}
#endif
	else
	{
		ERROR("Unknow Chip ID.\n");
		panic();
	}

	*p_dram_size = dramc_main();
	SET_DRAM_SIZE((*p_dram_size >> 20));

	/* DMA block mechanism should be able to block max burst size of all DMAs which is 128 bytes */
	mmio_write_32(DMA_BLOCK_EN, 0x0);
	mmio_write_32(DMA_BLOCK_MASK, 0xffffff80);
	mmio_write_32(DMA_BLOCK_EN, 0x1);

	rbus_timeout_check();

#if defined(TCSUPPORT_CPU_AN7552)
	/* Check DDR status. Only enable 7552 CPU WRR in DDR4 */
	if (mmio_read_32(DRAM_HW_CONF) & (1<<5))
	{
		/* Enable CPU WRR. Set QDMA be 0x6 and others be 0x1. */
		mmio_write_32(CPU_WRR_CFG0, 0x06010101);
		mmio_write_32(CPU_WRR_CFG1, 0x01010601);
	}
#endif

	return 0;
}
