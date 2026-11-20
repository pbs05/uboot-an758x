
#ifndef _ECNT_CPUFREQ_H
#define _ECNT_CPUFREQ_H

enum e_cpu_freq {
    cpu_freq_500M=0,
    cpu_freq_550M,
    cpu_freq_600M,
    cpu_freq_650M,
    cpu_freq_700M,
    cpu_freq_750M,
    cpu_freq_800M,
    cpu_freq_850M,
    cpu_freq_900M,
    cpu_freq_950M,
	cpu_freq_1000M,
#if defined(TCSUPPORT_CPU_EN7581) || defined(TCSUPPORT_CPU_AN7583)
    cpu_freq_1050M,
    cpu_freq_1100M,
    cpu_freq_1150M,
    cpu_freq_1200M,
    cpu_freq_1250M,	/* experimental, outside public spec */
    cpu_freq_1300M,	/* experimental, outside public spec */
    cpu_freq_1350M,	/* experimental, outside public spec */
    cpu_freq_1400M,	/* experimental, outside public spec */
#endif
    cpu_freq_last
};

enum e_clk_src {
    clk_src_xtal=0,
    clk_src_armpll,
    clk_src_pll1,
    clk_src_pll2,
    clk_src_last
};

enum cpu_domain_clk_gating {
    cpu_clk_armpll,
    cpu_clk_pll1,
    cpu_clk_pll2,
    cpu_clk_armpll_div2
};

enum e_div_sel {
    div_sel_1=0,
    div_sel_2,
    div_sel_4,
    div_sel_6,
    div_sel_last
};

extern int en7523_armpll_set(enum e_cpu_freq cpuFreq);
extern void set_cpu_domain_clk_gating(enum cpu_domain_clk_gating pll, int isEnable);
unsigned int curr_armpll_clk_get (void);
int an7552_bootup_clk_src_switch(enum e_cpu_freq cpuFreq);

enum e_cpu_freq cpu_freq_enum_get(unsigned int armpll_clk);

#endif /* _ECNT_CPUFREQ_H */
