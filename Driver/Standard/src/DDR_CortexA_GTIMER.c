/**
 * @file    DDR_CortexA_GETIMER.c
 * @brief   Micro C Cube Standard, DEVICE DRIVER
 *          Cortex-A7, Cortex-A15, ARMv8 and later Generic Timer.
 * @date    2019.05.24
 * @author  Copyright (c) 2016-2019, eForce Co., Ltd. All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2016.05.13) y-kim
 *            Created based on DDR_RZG1_GETIMER.c
 *          - rev 1.1 (2016.12.21) yokota
 *            change for imx7ulp, add TA_FPU attribute for isr
 *          - rev 1.1 (2017.07.28) y-kim
 *            Added support for CPU standby mode.
 *          - rev 1.2 (2017.10.12) i-cho
 *            Fixed the IPA warnings.
 *          - rev 1.3 (2018.06.12) i-cho
 *            implement _kernel_micro_systim
 *          - rev 1.3a (2018.12.21) yokota
 *            change for  initialize from configuable master_core.
 *          - rev 1.3b (2019.05.24)
 *            New macro: CFG_NO_HALT_DBG
 ****************************************************************************
 */
#include "kernel.h"
#include "DDR_CortexA_GTIMER.h"
#include "DDR_CortexA_GTIMER_cfg.h"
#include "cpu_cfg.h"

/* External function prototypes ----------------------------------------------*/

extern UW cp15_get_cpuid_pfr1(void);
extern void cp15_set_cntp_ctl(UW val);
extern UW cp15_get_cntp_ctl(void);
extern void cp15_set_cntp_tval(UW val);
extern W cp15_get_cntp_tval(void);
extern void cp15_set_cntfrq(UW val);

/* Private function prototypes -----------------------------------------------*/

static BOOL is_supported(void);
static void tmr_start(void);
static void tmr_stop(void);
static void tmr_isr(VP_INT exinf);

/* Private typedef -----------------------------------------------------------*/

/* ARM generic counter */
struct t_gcnt {
    UW  CNTCR;          /* counter control register */
    UW  CNTSR;          /* counter status register  */
    UW  CNTCV0;         /* counter count value register (lower 32bit) */
    UW  CNTCV1;         /* counter count value register (upper 32bit) */
    UW  reserved[4];
    UW  CNTFID0;        /* frequency modes table   */
                        /* 0 is the base frequency */
};

/* Private macro -------------------------------------------------------------*/

/* ARM Generic Counter CNTCR (Counter Control Register) */
#define CNTCR_ENABLE        (1U << 0)    /* Enable*/
#define CNTCR_HDBG          (1U << 1)    /* Halt on debug */

/* CP15 CNTP_CTL (PL1 Physical Timer Control register) */
#define CNTP_CTRL_ENABLE    (1U << 0)
#define CNTP_CTRL_INT_MASK  (1U << 1)
#define CNTP_CTRL_INT_STAT  (1U << 2)

/* CP15 ID_PFR1 (Processor Feature Register 1) */
#define PFR1_GTIMER_MASK    0x000f0000U
#define PFR1_GTIMER_IMPL    0x00010000U
#define PFR1_GTIMER_NO_IMPL 0x00000000U

#define PFR1_GTIMER_MASK    0x000f0000U
#define PFR1_GTIMER_IMPL    0x00010000U
#define PFR1_GTIMER_NO_IMPL 0x00000000U

/* Private variables ---------------------------------------------------------*/

static UW g_tmr_systim   = 0U;
static UW g_tmr_rate     = 0U;
static UW g_tmr_tick     = 0U;
static W  g_tmr_suspend  = 0;

/**
 * Check generic timer support
 * @return TRUE or FALSE
 */
static BOOL is_supported(void)
{
    return ((cp15_get_cpuid_pfr1() & PFR1_GTIMER_MASK) == PFR1_GTIMER_IMPL)
            ? (TRUE)
            : (FALSE);
}

/**
 * Enable generic timer
 */
static void tmr_start(void)
{
    UW ctrl;

    ctrl = cp15_get_cntp_ctl();
    ctrl &= ~CNTP_CTRL_INT_MASK;
    ctrl |= CNTP_CTRL_ENABLE;
    cp15_set_cntp_ctl(ctrl);
}

/**
 * Disable generic timer
 */
static void tmr_stop(void)
{
    UW ctrl;

    ctrl = cp15_get_cntp_ctl();
    ctrl |= CNTP_CTRL_INT_MASK;
    ctrl &= ~CNTP_CTRL_ENABLE;
    cp15_set_cntp_ctl(ctrl);
}

/**
 * Timer event has triggered
 */
static void tmr_isr(VP_INT exinf)
{
    UW reload = 0U;
    W cur_tval = 0;

    (void)loc_cpu();

    /* After an event has triggered, a read of a TimerValue register
     * indicates the time since the event triggered.
     * So, subtract current CNTP_TVAL.
     */
    cur_tval = cp15_get_cntp_tval();
    if (cur_tval < 0) {
        reload = (UW)((W)g_tmr_rate + cur_tval);
    } else {
        reload = g_tmr_rate - (UW)cur_tval;
    }
    cp15_set_cntp_tval(reload - 1U);
    _kernel_synch_cache();

    g_tmr_systim += g_tmr_tick;
    _kernel_synch_cache();

    (void)unl_cpu();

    (void)isig_tim();
}


/**
 * Initialize ARM generic timer
 * @param tick      timer event cycle (ms)
 * @param base_clk  base frequency (hz)
 * @return E_OK or error code
 */
ER _ddr_cortexa_gtimer_init(UINT tick, UW base_clk)
{
    ER ercd;

    if (is_supported() == FALSE)
    {
        ercd =  E_NOSPT;
    }
    else
    {
        T_CISR tmr_cisr;
        tmr_cisr.isratr = (TA_HLNG | TA_FPU);
        tmr_cisr.exinf  = (VP_INT)0;
        tmr_cisr.intno  = CFG_GTIMER_INTNO;
        tmr_cisr.isr    = (FP)tmr_isr;
        tmr_cisr.imask  = (IMASK)CFG_GTIMER_IPL;

        ercd = acre_isr((T_CISR *)&tmr_cisr);
        if (ercd >= E_OK) {
            BOOL iflag;
            UW cntp_ctl;
            UINT cid;

            cntp_ctl = cp15_get_cntp_ctl();
            cntp_ctl &= ~CNTP_CTRL_ENABLE;
            cntp_ctl |= CNTP_CTRL_INT_MASK;
            cp15_set_cntp_ctl(cntp_ctl);

            iflag = sns_loc();
            if (iflag == FALSE) {
                (void)loc_cpu();
            }

            _kernel_synch_cache();
            g_tmr_tick = (tick * 1000U);
            g_tmr_rate = (tick * base_clk / 1000U);
            g_tmr_systim = 0U;
            _kernel_synch_cache();

            cntp_ctl &= ~CNTP_CTRL_INT_MASK;
            cntp_ctl |= CNTP_CTRL_ENABLE;
            cp15_set_cntp_tval(g_tmr_rate - 1U);
            cp15_set_cntp_ctl(cntp_ctl); /* enable down timer */

            /*
             * arch timer is started.
             * only master core
             */
            cid = get_cid();
            if (cid == MASTER_CORE_ID) {
#ifndef CFG_NO_SYSTEM_COUNTER
                volatile struct t_gcnt * reg
                    = (volatile struct t_gcnt *)CFG_GTIMER_REG_BASE;

                /*
                 * Update the timer if it is not at the right frequency.
                 */
                if (reg->CNTFID0 != base_clk) {
                    reg->CNTFID0 = base_clk;
                    cp15_set_cntfrq(base_clk);
                }
#ifdef CFG_NO_HALT_DBG
                reg->CNTCR |= CNTCR_ENABLE;
#else
                reg->CNTCR |= (CNTCR_ENABLE | CNTCR_HDBG);
#endif
#endif /* CFG_NO_SYSTEM_COUNTER */
            }

            if (iflag == FALSE) {
                (void)unl_cpu();
            }

            (void)ena_int(CFG_GTIMER_INTNO);
            ercd = E_OK;
        }
    }
    return ercd;
}

/*=====================================================================*/

/**
 * Kernel internal functions
 * should not call following functions in user application.
 */

/**
 * Read the system time by a microsecond unit
 * @return system time (us)
 */
UW _kernel_micro_systim(void)
{
    UW clk = 0U;
    UW tcn;
    if (g_tmr_tick != 0U) {
        if ((cp15_get_cntp_ctl() & CNTP_CTRL_INT_STAT) != 0U) {
            clk = g_tmr_systim + g_tmr_tick;
            tcn = (UW)(-cp15_get_cntp_tval());
        } else {
            clk = g_tmr_systim;
            tcn = g_tmr_rate - cp15_get_cntp_tval();
            if ((cp15_get_cntp_ctl() & CNTP_CTRL_INT_STAT) != 0U) {
                clk += g_tmr_tick;
                tcn = (UW)(-cp15_get_cntp_tval());
            }
        }
        clk += ((tcn * g_tmr_tick) / g_tmr_rate);
    }
    return clk;
}

/**
 * Stop the system timer
 */
void _kernel_micro_systim_stop(void)
{
    g_tmr_suspend = cp15_get_cntp_tval();
    tmr_stop();
}

/**
 * Restart the system timer
 */
void _kernel_micro_systim_start(void)
{
    cp15_set_cntp_tval((UW)g_tmr_suspend);
    tmr_start();
}

/*=====================================================================*/

/**
 * For CPU standby mode
 */

#if (USE_STANDBY_MODE == 1U)

/**
 * Backup ARM generic timer registers
 */
void _ddr_cortexa_gtimer_backup(void)
{
    _kernel_micro_systim_stop();
}

/**
 * Restore ARM generic timer registers
 */
void _ddr_cortexa_gtimer_restore(void)
{
#ifndef CFG_NO_SYSTEM_COUNTER
    /* TODO */
#endif /* #ifndef CFG_NO_SYSTEM_COUNTER */

    _kernel_micro_systim_start();
}

#endif /* #if (USE_STANDBY_MODE == 1U) */
