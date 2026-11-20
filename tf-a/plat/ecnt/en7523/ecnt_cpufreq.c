
#if defined(IMAGE_BL2) || defined(IMAGE_BL31)
#include <stdio.h>
#include <lib/mmio.h>
#include <drivers/delay_timer.h>
#else /* UBoot */
#include <asm/io.h>
#include <common.h>
#endif
#include <ecnt_avs.h>
#include <asm/tc3162.h>
#include "ecnt_cpufreq.h"

/* cpu frequency adjustment registers */
#define IO_PHYS				(0x10000000)
#define MCUCFG_BASE		    (IO_PHYS + 0xEFBE000)
#define CR_ACLKEN_DIV       (MCUCFG_BASE + 0x640)
#define CR_MP0_PLL_DIV      (MCUCFG_BASE + 0x7A0)
#define CR_BUS_PLL_DIV      (MCUCFG_BASE + 0x7C0)
#define CR_CPU_CLK_GATING	(IO_PHYS + 0xFA201E0)
#define CR_CHIP_PROBE_MODE  (IO_PHYS + 0xFA20220)
#define CR_CLK_PROBE_MODE   (IO_PHYS + 0xFA20244)
#define CR_RGS_ECC_SEL      (IO_PHYS + 0xFA20254)
#if defined(TCSUPPORT_CPU_AN7583)
#define CR_PLL_WR_PROTECT   (IO_PHYS + 0xFA20268)
#define CR_CPUPLL_SDM_PCW   (IO_PHYS + 0xFA202AC)
#define CR_CPUPLL_SDM_PCW_CHG	(IO_PHYS + 0xFA202B0)
#define CR_CPUPLL_SDM_SSC_PRD	(IO_PHYS + 0xFA202B8)
#define PCW_MASK            (0xff)
#define PCW_SHIFT           (24)
#define POSDIV_MASK         (0x7)
#define POSDIV_SHIFT        (18)
#define WT_PROTECT_MAGIC    (0x80)
#elif defined(TCSUPPORT_CPU_EN7581)
#define CR_PLL_WR_PROTECT   (IO_PHYS + 0xFA20268)
#define CR_CPUPLL_SDM_PCW   (IO_PHYS + 0xFA202B4)
#define CR_CPUPLL_SDM_PCW_CHG   (IO_PHYS + 0xFA202B8)
#define CR_CPUPLL_SDM_SSC_PRD (0) // AN7552 only
#define PCW_MASK            (0xff)
#define PCW_SHIFT           (24)
#define POSDIV_MASK         (0x7)
#define POSDIV_SHIFT        (4)
#define WT_PROTECT_MAGIC    (0x12)
#elif defined(TCSUPPORT_CPU_AN7552)
#define CR_PLL_WR_PROTECT   (IO_PHYS + 0xFA20268)
#define CR_CPUPLL_SDM_PCW   (IO_PHYS + 0xFA202AC)
#define CR_CPUPLL_SDM_PCW_CHG   (IO_PHYS + 0xFA202B0)
#define CR_CPUPLL_SDM_SSC_PRD   (IO_PHYS + 0xFA202B8)
#define PCW_MASK            (0xff)
#define PCW_SHIFT           (24)
#define POSDIV_MASK         (0x7)
#define POSDIV_SHIFT        (18)
#define WT_PROTECT_MAGIC    (0x80)
#else /* 7523 */
#define CR_PLL_WR_PROTECT   (IO_PHYS + 0xFA20264)
#define CR_SYSPLL_PCW_25M   (IO_PHYS + 0xFA202A8)
#define CR_SYSPLL_PCW_20M   (IO_PHYS + 0xFA202AC)
#define CR_SYSPLL_DISABLE   (IO_PHYS + 0xFA202B0)
#define XTAL_MASK           (0x7f)
#define XTAL_SHIFT          (24)
#define WT_PROTECT_MAGIC    (0x80)
#endif

#if defined(IMAGE_BL2) || defined(IMAGE_BL31)
#define	writeReg(reg, data) mmio_write_32(reg, data)
#define readReg(reg)        mmio_read_32(reg)
#else /* UBoot */
#define VPint   *(volatile unsigned int *)
#define	writeReg(reg, data)	(VPint(reg) = data)
#define	readReg(reg)		(VPint(reg))
#endif

#define VAL_0               (0)
#define VAL_1               (1)

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7583)
static unsigned char cpu_freq_config_pcw[]=     { 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24, 0x26, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C};
static unsigned char cpu_freq_config_posdiv[]=  {VAL_1,VAL_1,VAL_1,VAL_1,VAL_1,VAL_1,VAL_1,VAL_1,VAL_1,VAL_1,VAL_0,VAL_0,VAL_0,VAL_0,VAL_0,VAL_0,VAL_0,VAL_0,VAL_0};
static char *clk_src_name[]={"xtal(50MHz)","armpll(500~1400MHz)","pll1(540MHz)","pll2(400MHz)"};
#elif defined(TCSUPPORT_CPU_AN7552)
static unsigned char cpu_freq_config_pcw[]=     { 0x14, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24, 0x26, 0x14};
static unsigned char cpu_freq_config_posdiv[]=  {VAL_1,VAL_1,VAL_1,VAL_1,VAL_1,VAL_1,VAL_1,VAL_1,VAL_1,VAL_1,VAL_0};
static char *clk_src_name[]={"xtal(50MHz)","armpll(500~1000MHz)","pll1(540MHz)","pll2(400MHz)"};
#else
static unsigned char cpu_freq_config_xtal25M[]= {0x28, 0x2c, 0x30, 0x34, 0x38, 0x3c, 0x40, 0x44, 0x48, 0x4c};
static unsigned char cpu_freq_config_xtal20M[]= {0x32, 0x37, 0x3C, 0x41, 0x46, 0x4B, 0x50, 0x55, 0x5A, 0x5F};
static char *clk_src_name[]={"xtal(20/25MHz)","armpll(500~950MHz)","pll1(540MHz)","pll2(500MHz)"};
#endif
//static unsigned int  voltage_config[]         ={ 115,  115,  115,  115,  115,  115,  125,  125,  125,  125};	//YMC mark for new AVS FW
static unsigned int  armpll_clk_MHz[]         = {  500,  550,  600,  650,  700,  750,  800,  850,  900,  950, 1000, 1050, 1100, 1150, 1200, 1250, 1300, 1350, 1400};

static unsigned int clk_divider_config[]={0x0, 0xa, 0xb, 0x1d};


void set_cpu_domain_clk_gating(enum cpu_domain_clk_gating pll, int isEnable)
{
    unsigned int val;

    val = readReg(CR_CPU_CLK_GATING);
	if(isEnable) {
        val |= ((uint32_t)1 << (int)pll);
	} else {
	    val &= (~((uint32_t)1 << (int)pll));
	}
    writeReg(CR_CPU_CLK_GATING, val);
}

enum e_cpu_freq cpu_freq_enum_get(unsigned int armpll_clk)
{
    int i;

    for (i=0; i<cpu_freq_last; i++) {
        if (armpll_clk_MHz[i]==armpll_clk)
            return i;
    }

    printf("ERROR: %s for armpll_clk:%d\n", __func__, armpll_clk);
    return 0;
}

enum e_div_sel divider_sel_enum_get(unsigned int divider)
{
    if (!((divider==1)||(divider==2)||(divider==4)||(divider==6))) {
        printf("ERROR: %s for divider:%d, should be 1,2,4,6\n", __func__, divider);
        return 0;
    }

    if (divider==1)
        return div_sel_1;
    else
        return (divider>>1);
}

int isXtalClk25M (void)
{
    unsigned int val;
    
    val = readReg(CR_RGS_ECC_SEL);

    if (val&(1<<19)) /* XTAL==25MHz */
        return 1;
    else    /* XTAL==20MHz */
        return 0;
}

enum e_clk_src curr_clk_src_enum_get (void)
{
    unsigned int val;
    enum e_clk_src clk_src;

    val = readReg(CR_BUS_PLL_DIV);
    clk_src = ((val>>9)&0x3);

    return clk_src;
}

int clk_src_switch (enum e_clk_src clk_src)
{
    unsigned int val, mux1sel;

    /* check if clk_src_switch is needed */
    val = readReg(CR_BUS_PLL_DIV);
    mux1sel = (val>>9)&0x3;
    if (mux1sel==clk_src)
        return 0;

    /* enable clk_src */
    if (clk_src!=clk_src_xtal) /* src_xtal is always on and can't be disabled, so no need to enable. */
	    set_cpu_domain_clk_gating((clk_src-1), 1);

    /* set "L2C SRAM interface and MCU_BIU clock divider" as "1/1*CPU Clock" for XTAL or "1/2*CPU Clock" for others */
    val = readReg(CR_ACLKEN_DIV);
    val &= (~0x1f);
#if !defined(TCSUPPORT_CPU_EN7581) && !defined(TCSUPPORT_CPU_AN7552) && !defined(TCSUPPORT_CPU_AN7583)
    if (clk_src==clk_src_xtal)
        val |= 0x01;
    else
#endif
        val |= 0x12;
    writeReg(CR_ACLKEN_DIV, val);

	udelay(1);
    /* switch to target clk_src */
    val = readReg(CR_BUS_PLL_DIV);
    val &= (~(0x3<<9));
    val |= (clk_src<<9);
    writeReg(CR_BUS_PLL_DIV, val);

    /* read back to make sure */
    val = readReg(CR_BUS_PLL_DIV);
    val = ((val>>9)&0x3);
    if (val!=clk_src) {
        printf("ERROR: val:%d != clk_src:%d for %s\n", val, clk_src, clk_src_name[clk_src]);
        return -1;
    }

    return 0;
}

unsigned int curr_clk_divider_get (void)
{
    int i;
    unsigned int val;

    val = readReg(CR_BUS_PLL_DIV);
    val = ((val>>17)&0x1f);

    for (i=0; i<div_sel_last; i++) {
        if (clk_divider_config[i]==val) {
            if (i==0)
                return 1;
            else
                return (i<<1);
        }
    }

    if (i==div_sel_last) {
        printf("ERROR: can't find val:%d for clk_divider_config\n", val);
    }
    return 0;
}

/* if need to adjust armpll, adjust armpll first, then set divider. */
int clk_divider_sel_set (enum e_div_sel divider_sel)
{
    unsigned int val;

    if (divider_sel>=div_sel_last) {
        printf("ERROR: divider_sel:%d>=div_sel_last\n", divider_sel);
        return -1;
    }

    val = readReg(CR_BUS_PLL_DIV);
    val &= (~(0x1f<<17));
    val |= (clk_divider_config[divider_sel]<<17);
    writeReg(CR_BUS_PLL_DIV, val);

    /* read back to make sure */
    val = readReg(CR_BUS_PLL_DIV);
    val = ((val>>17)&0x1f);
    if (val!=clk_divider_config[divider_sel]) {
        printf("ERROR: val:0x%x != clk_divider_config[%d]:0x%x\n", val, divider_sel, clk_divider_config[divider_sel]);
        return -1;
    }

    return 0;
}

unsigned int curr_armpll_clk_get (void)
{
    int i;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_AN7552)
    unsigned int pcw, posdiv, freq;

    pcw = (((readReg(CR_CPUPLL_SDM_PCW))>>PCW_SHIFT)&PCW_MASK);
	if(isEN7581)
		posdiv = (((readReg(CR_CPUPLL_SDM_PCW_CHG))>>POSDIV_SHIFT)&POSDIV_MASK);
	else
		posdiv = (((readReg(CR_CPUPLL_SDM_SSC_PRD))>>POSDIV_SHIFT)&POSDIV_MASK);

    if (posdiv==VAL_0)
        freq = (pcw*50);
    else if (posdiv==VAL_1)
        freq = (pcw*25);
    else {
		if(isEN7581){
	        printf("ERROR: can't get armpll due to wrong posdiv:%d (CPUPLL_SDM_PCW:0x%x, CPUPLL_SDM_PCW_CHG:0x%x)\n",
	                (int)posdiv, readReg(CR_CPUPLL_SDM_PCW), readReg(CR_CPUPLL_SDM_PCW_CHG));
		}else{
	        printf("ERROR: can't get armpll due to wrong posdiv:%d (CPUPLL_SDM_PCW:0x%x, CR_CPUPLL_SDM_SSC_PRD:0x%x)\n",
                (int)posdiv, readReg(CR_CPUPLL_SDM_PCW), readReg(CR_CPUPLL_SDM_SSC_PRD));
		}
	
        return 0;
    }
    
    for (i=0; i<cpu_freq_last; i++) {
        if (freq == armpll_clk_MHz[i])
            break;
    }

    if (i<cpu_freq_last) {
        return freq;
    }
    else {
		if(isEN7581){
	        printf("ERROR: can't get armpll due to wrong freq:%d (CPUPLL_SDM_PCW:0x%x, CPUPLL_SDM_PCW_CHG:0x%x)\n",
	                (int)freq, readReg(CR_CPUPLL_SDM_PCW), readReg(CR_CPUPLL_SDM_PCW_CHG));
		}else if((isAN7552) || (isAN7583)){
	        printf("ERROR: can't get armpll due to wrong freq:%d (CPUPLL_SDM_PCW:0x%x, CR_CPUPLL_SDM_SSC_PRD:0x%x)\n",
	                (int)freq, readReg(CR_CPUPLL_SDM_PCW), readReg(CR_CPUPLL_SDM_SSC_PRD));		
		}
		else{
			printf("Unknown Chip id!\n");
		}
        return 0;
    }

#else
    unsigned int val, val2;
    int isXtal25M=0;

    if (isXtalClk25M()) { /* XTAL==25MHz */
        isXtal25M=1;
        val = readReg(CR_SYSPLL_PCW_25M);
        val2 = ((val>>XTAL_SHIFT)&XTAL_MASK);
        for (i=0; i< cpu_freq_last; i++) {
            if (val2 == cpu_freq_config_xtal25M[i])
                break;
        }
    }
    else { /* XTAL==20MHz */
        val = readReg(CR_SYSPLL_PCW_20M);
        val2 = ((val>>XTAL_SHIFT)&XTAL_MASK);
        for (i=0; i< cpu_freq_last; i++) {
            if (val2 == cpu_freq_config_xtal20M[i])
                break;
        }
    }

    if (i< cpu_freq_last)
        return armpll_clk_MHz[i];
    else {
        printf("ERROR: can't get armpll (isXtal25M:%d, SYSPLL_PCW value:0x%x)\n", isXtal25M, val);
        return 0;
    }
#endif
}

static void configure_armpll(enum e_cpu_freq cpuFreq)
{
    unsigned int val;

#if defined(TCSUPPORT_CPU_AN7583)
	/* set CR_CPUPLL_SDM_PCW[31:24] */
	val = readReg(CR_CPUPLL_SDM_PCW);
	val &= (~(PCW_MASK<<PCW_SHIFT));
	val |= (cpu_freq_config_pcw[cpuFreq]<<PCW_SHIFT);
	writeReg(CR_CPUPLL_SDM_PCW, val);

	/* set CR_CPUPLL_SDM_SSC_PRD[20:18] (POSDIV) */
	val = readReg(CR_CPUPLL_SDM_SSC_PRD);
	val &= (~(POSDIV_MASK<<POSDIV_SHIFT));
	val |= (cpu_freq_config_posdiv[cpuFreq]<<POSDIV_SHIFT);
	writeReg(CR_CPUPLL_SDM_SSC_PRD, val);

	/* toggle CPUPLL_SDM_PCW_CHG[0] */
	val = readReg(CR_CPUPLL_SDM_PCW_CHG);
	if (val&0x1) val &= (~0x1);
	else val |= 0x1;
	writeReg(CR_CPUPLL_SDM_PCW_CHG, val);
#elif defined(TCSUPPORT_CPU_EN7581)
    /* set CR_CPUPLL_SDM_PCW[31:24] */
    val = readReg(CR_CPUPLL_SDM_PCW);
    val &= (~(PCW_MASK<<PCW_SHIFT));
    val |= (cpu_freq_config_pcw[cpuFreq]<<PCW_SHIFT);
    writeReg(CR_CPUPLL_SDM_PCW, val);
    /* set CPUPLL_SDM_PCW_CHG[6:4] (POSDIV) */
    val = readReg(CR_CPUPLL_SDM_PCW_CHG);
    val &= (~(POSDIV_MASK<<POSDIV_SHIFT));
    val |= (cpu_freq_config_posdiv[cpuFreq]<<POSDIV_SHIFT);
    /* toggle CPUPLL_SDM_PCW_CHG[0] */
    if (val&0x1) val &= (~0x1);
    else val |= 0x1;
    writeReg(CR_CPUPLL_SDM_PCW_CHG, val);
#elif defined(TCSUPPORT_CPU_AN7552)
	/* set CR_CPUPLL_SDM_PCW[31:24] */
	val = readReg(CR_CPUPLL_SDM_PCW);
	val &= (~(PCW_MASK<<PCW_SHIFT));
	val |= (cpu_freq_config_pcw[cpuFreq]<<PCW_SHIFT);
	writeReg(CR_CPUPLL_SDM_PCW, val);
	/* set CPUPLL_SDM_PCW_CHG[6:4] (POSDIV) */
	val = readReg(CR_CPUPLL_SDM_SSC_PRD);
	val &= (~(POSDIV_MASK<<POSDIV_SHIFT));
	val |= (cpu_freq_config_posdiv[cpuFreq]<<POSDIV_SHIFT);
	writeReg(CR_CPUPLL_SDM_SSC_PRD, val);
	/* toggle CPUPLL_SDM_PCW_CHG[0] */
	val = readReg(CR_CPUPLL_SDM_PCW_CHG);
	if (val&0x1) val &= (~0x1);
	else val |= 0x1;
	writeReg(CR_CPUPLL_SDM_PCW_CHG, val);

#else /* 7523 */
    if (isXtalClk25M()) { /* XTAL==25MHz */
        val = readReg(CR_SYSPLL_PCW_25M);
        val &= (~(XTAL_MASK<<XTAL_SHIFT));
        val |= (cpu_freq_config_xtal25M[cpuFreq]<<XTAL_SHIFT);
        writeReg(CR_SYSPLL_PCW_25M, val);
    }
    else { /* XTAL==20MHz */
        val = readReg(CR_SYSPLL_PCW_20M);
        val &= (~(XTAL_MASK<<XTAL_SHIFT));
        val |= (cpu_freq_config_xtal20M[cpuFreq]<<XTAL_SHIFT);
        writeReg(CR_SYSPLL_PCW_20M, val);
    }

    /* toggle SYSPLL_SDM_PCW_CHG */
    val = readReg(CR_SYSPLL_DISABLE);
    if (val&(1<<3))
        val &= (~(1<<3));
    else
        val |= (1<<3);
    writeReg(CR_SYSPLL_DISABLE, val);
#endif

    return;
}

static int confirm_armpll(enum e_cpu_freq cpuFreq)
{
    unsigned int val;

#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7583) || defined(TCSUPPORT_CPU_AN7552)
    val = (((readReg(CR_CPUPLL_SDM_PCW))>>PCW_SHIFT)&PCW_MASK);

	if (val!=cpu_freq_config_pcw[cpuFreq]) {
        printf("ERROR(%s): val:0x%x != config_pcw[%d]:0x%x (CR_CPUPLL_SDM_PCW:0x%x)\n", __func__, 
                val, cpuFreq, cpu_freq_config_pcw[cpuFreq], readReg(CR_CPUPLL_SDM_PCW));
        return -1;
    }
	if(isEN7581){
    	val = (((readReg(CR_CPUPLL_SDM_PCW_CHG))>>POSDIV_SHIFT)&POSDIV_MASK);
    	if (val!=cpu_freq_config_posdiv[cpuFreq]) {
    	    printf("ERROR(%s): val:0x%x != config_posdiv[%d]:0x%x (CR_CPUPLL_SDM_PCW_CHG:0x%x)\n", __func__, 
    	            val, cpuFreq, cpu_freq_config_posdiv[cpuFreq], readReg(CR_CPUPLL_SDM_PCW_CHG));
    	    return -1;
    	}
	}else if((isAN7552) || (isAN7583))
	{
    	val = (((readReg(CR_CPUPLL_SDM_SSC_PRD))>>POSDIV_SHIFT)&POSDIV_MASK);
    	if (val!=cpu_freq_config_posdiv[cpuFreq]) {
    	    printf("ERROR(%s): val:0x%x != config_posdiv[%d]:0x%x (CR_CPUPLL_SDM_PCW_CHG:0x%x)\n", __func__, 
    	            val, cpuFreq, cpu_freq_config_posdiv[cpuFreq], readReg(CR_CPUPLL_SDM_SSC_PRD));
    	    return -1;
    	}		
	}else
	{
		printf("Unknown Chip id!\n");
	}
#else
    if (isXtalClk25M()) { /* XTAL==25MHz */
        val = readReg(CR_SYSPLL_PCW_25M);
        val = ((val>>XTAL_SHIFT)&XTAL_MASK);
        if (val!=cpu_freq_config_xtal25M[cpuFreq]) {
            printf("ERROR: val:0x%x != config_xtal25M[%d]:0x%x\n", val, cpuFreq, cpu_freq_config_xtal25M[cpuFreq]);
            return -1;
        }
    }
    else { /* XTAL==20MHz */
        val = readReg(CR_SYSPLL_PCW_20M);
        val = ((val>>XTAL_SHIFT)&XTAL_MASK);
        if (val!=cpu_freq_config_xtal20M[cpuFreq]) {
            printf("ERROR: val:0x%x != config_xtal20M[%d]:0x%x\n", val, cpuFreq, cpu_freq_config_xtal20M[cpuFreq]);
            return -1;
        }
    }
#endif

    return 0;
}

int an7552_bootup_clk_src_switch(enum e_cpu_freq cpuFreq)
{
    unsigned int val;

    /* disable PLL Write Protect */
    val = readReg(CR_PLL_WR_PROTECT);
    val &= (~0xff);
    val |= WT_PROTECT_MAGIC;
    writeReg(CR_PLL_WR_PROTECT, val);

    /* wait for 0.5us at least */
    udelay(1);

    /* switch to target clk_src */
    val = readReg(CR_BUS_PLL_DIV);
    val &= (~(0x3<<9));
    val |= (clk_src_armpll<<9);
    writeReg(CR_BUS_PLL_DIV, val);

    /* read back to make sure */
    val = readReg(CR_BUS_PLL_DIV);
    val = ((val>>9)&0x3);
    if (val!=clk_src_armpll) {
        printf("ERROR: val:%d != clk_src:%d for %s\n", val, cpuFreq, clk_src_name[cpuFreq]);
        return -1;
    }

	/* disable PLL2_CLK */
	set_cpu_domain_clk_gating(cpu_clk_pll2, 0);

    /* enable PLL Write Protect */
    val = readReg(CR_PLL_WR_PROTECT);
    val &= (~0xff);
    writeReg(CR_PLL_WR_PROTECT, val);

    /* read armpll back to make sure it's expected */
    if (confirm_armpll(cpuFreq))
    {
		return -1;
    }

    return 0;
}


/* This function can only be used in ASIC */
int en7523_armpll_set(enum e_cpu_freq cpuFreq)
{
    unsigned int val;
#if 0	//YMC mark for new AVS FW
    AVS_STATUS_T ret;


    /* adjust voltage if needed */
    val = AVS_Get_Vcore();
    if (val != voltage_config[cpuFreq]) {
        ret = AVS_Set(voltage_config[cpuFreq]);
        if (ret!=AVS_OK) {
            printf("ERROR: AVS_Set failed (ErrCode:%d) for voltage_config[%d]:%d\n", ret, cpuFreq, voltage_config[cpuFreq]);
            /* default voltage is 1.15V which can support cpufreq up to 750 MHz*/
            if (cpuFreq>cpu_freq_750M) {
                cpuFreq = cpu_freq_750M;
                AVS_Set(voltage_config[cpuFreq]);
            }
        }
    }
#endif
	
	/* switch to PLL2_CLK */
	if (clk_src_switch(clk_src_pll2)) {
		printf("cpu clock switch to pll2 fail.\n");
		/* disable PLL2_CLK */
		set_cpu_domain_clk_gating(cpu_clk_pll2, 0);
		return -1;
	}

    /* disable PLL Write Protect */
    val = readReg(CR_PLL_WR_PROTECT);
    val &= (~0xff);
    val |= WT_PROTECT_MAGIC;
    writeReg(CR_PLL_WR_PROTECT, val);

    /* set armpll */
    configure_armpll(cpuFreq);

    /* wait for 0.5us at least */
    udelay(1);

	/* switch to ARMPLL_CLK */	  
	if (clk_src_switch(clk_src_armpll)) {
		printf("cpu clock switch to armpll fail.\n");
		/* enable PLL Write Protect */
		val = readReg(CR_PLL_WR_PROTECT);
		val &= (~0xff);
		writeReg(CR_PLL_WR_PROTECT, val);
		return -1;
	}

	/* disable PLL2_CLK */
	set_cpu_domain_clk_gating(cpu_clk_pll2, 0);

    /* enable PLL Write Protect */
    val = readReg(CR_PLL_WR_PROTECT);
    val &= (~0xff);
    writeReg(CR_PLL_WR_PROTECT, val);

    /* read armpll back to make sure it's expected */
    if (confirm_armpll(cpuFreq))
        return -1;

    return 0;
}
