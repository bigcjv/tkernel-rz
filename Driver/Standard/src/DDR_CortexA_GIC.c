/**
 * @file    DDR_CortexA_GIC.c
 * @brief   Micro C Cube Standard, DEVICE DRIVER
 *          ARM Generic Interrupt Controller
 * @date    2018.07.30
 * @author  Copyright (c) 2016-2018, eForce Co., Ltd. All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2016.04.27) y-kim
 *            Created based on DDR_RZG1_GIC.c.
 *          - rev 1.1 (2017.07.28) y-kim
 *            Added support for CPU standby mode.
 *          - rev 1.2 (2018.03.09) i-cho
 *            Corrected that check of the USE_SYSTEM macro.
 *            Added send SGI function.
 *            GICD_ICFGR initialization was made customizable.
 *          - rev 1.3 (2018.03.28) yokota
 *            Remove include string.h, DDR_CortexA_GIC_cfg.h
 *          - rev 1.4 (2018.07.30) i-cho
 *            Split initialization into master part and slave part.
 *          - rev 1.5 (2018.10.17) i-cho
 *            Change t_gic_backup's macro from SINGLE_CORE to MPCORE.
 *          - rev 1.5.a (2018.12.21) yokota
 *            Change for customize uC3+Linux.
 *          - rev 1.6 (2019.05.08)
 *            Remove warning for unused-variable
 ****************************************************************************
 */
#include <string.h>
#include "uC3sys.h"
#include "DDR_CortexA_GIC.h"
#if (USE_SYSTEM != SYSTEM_SINGLE_CORE)
#include "uC3mcext.h"
#endif /* #if (USE_SYSTEM != SYSTEM_SINGLE_CORE) */

/* External variables --------------------------------------------------------*/

extern T_VINFTBL vinftbl[];
extern int _kernel_dummy_2;
#if (USE_SYSTEM != SYSTEM_SINGLE_CORE)
extern UW _ddr_gic_flag;
#endif /* #if (USE_SYSTEM != SYSTEM_SINGLE_CORE) */

/* Private function prototypes -----------------------------------------------*/

static ER _ena_int(INTNO intno);
static ER _dis_int(INTNO intno);
static void spurious_isr(void);

/* Private typedef -----------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

#define GICv1   1U
#define GICv2   2U
#define GIC400  3U

#define GIC_DIST_OFFSET         0x1000U
#if (USE_CPU == CPU_CORTEX_A5)
#ifndef CFG_GIC_CPU_OFFSET
    #define GIC_CPU_OFFSET          0x0100U
#else
    #define GIC_CPU_OFFSET          CFG_GIC_CPU_OFFSET
#endif
    #define GIC_VER                 GICv1
#elif (USE_CPU == CPU_CORTEX_A7)
#ifndef CFG_GIC_CPU_OFFSET
    #define GIC_CPU_OFFSET          0x2000U
#else
    #define GIC_CPU_OFFSET          CFG_GIC_CPU_OFFSET
#endif
    #define GIC_VER                 GICv2
#elif (USE_CPU == CPU_CORTEX_A9)
#ifndef CFG_GIC_CPU_OFFSET
    #define GIC_CPU_OFFSET          0x0100U
#else
    #define GIC_CPU_OFFSET          CFG_GIC_CPU_OFFSET
#endif
    #define GIC_VER                 GICv1
#elif (USE_CPU == CPU_CORTEX_A15)
#ifndef CFG_GIC_CPU_OFFSET
    #define GIC_CPU_OFFSET          0x2000U
#else
    #define GIC_CPU_OFFSET          CFG_GIC_CPU_OFFSET
#endif
    #define GIC_VER                 GICv2
#else
    #error  "Unsupported CPU"
#endif

#if (USE_SYSTEM == SYSTEM_SINGLE_CORE)
    #define SPIN_LOCK()
    #define SPIN_UNLOCK()
#else /* #if (USE_SYSTEM == SYSTEM_SINGLE_CORE) */
    #define SPIN_LOCK()     ((void)_kernel_spin_lock(&_ddr_gic_flag))
    #define SPIN_UNLOCK()   ((void)_kernel_spin_unlock(&_ddr_gic_flag))
#endif /* #if (USE_SYSTEM == SYSTEM_SINGLE_CORE) */

/* Private variables ---------------------------------------------------------*/

static volatile struct t_gicc * g_gicc = 0;
static volatile struct t_gicd * g_gicd = 0;
#define REG_ICD                 (*g_gicd)
#define REG_ICC                 (*g_gicc)

#ifndef USEABLE_CORE_MASK
#define USEABLE_CORE_MASK 0xFFU
#endif

static void init_master_core(void)
{
    UW i;
    volatile UW *reg = 0;
#if (MPCORE == 1U)
    UW val;
#endif
    REG_ICD.DCR = 0U;

    /* Set all Group 0 */
    reg = &REG_ICD.ISR[0];
    for (i = 0U; i < CFG_GIC_INTNUM_MAX; i += 32U) {
        *reg++ = 0x00000000U;
    }

    /* Set all disable */
    reg = &REG_ICD.ICER[0];
    for (i = 0U; i < CFG_GIC_INTNUM_MAX; i += 32U) {
        *reg++ = 0xFFFFFFFFU;
    }

    /* Clear pending all */
    reg = &REG_ICD.ICPR[0];
    for (i = 0U; i < CFG_GIC_INTNUM_MAX; i += 32U) {
        *reg++ = 0xFFFFFFFFU;
    }

    /*
     * Set edge-triggered or level-sensitive
     * It's depends on CPU
     */
#ifdef CFG_GIC_FUNC_INIT_ICFGR
    CFG_GIC_FUNC_INIT_ICFGR(g_gicc, g_gicd);
#else
    /* Set all default
     * ICFR[0]: RO 0xAAAAAAAA
     * ICFR[1]: RO 0x55540000
     */
    reg = &REG_ICD.ICFR[2];
    for (i = 32U; i < CFG_GIC_INTNUM_MAX; i += 16U) {
        *reg++ = 0x55555555U;
    }
#endif

#if (MPCORE == 1U)
#ifdef  REMOTE_CORE_ID
     /* Clear Target CPU all*/
     if (REMOTE_CORE_ID == ID_CORE0) {
         val = 0x01010101U;
     } else if (REMOTE_CORE_ID == ID_CORE1) {
         val = 0x02020202U;
     }
#else
     val = 0x00000000U;
#endif
    /*
     * Clear Target CPU all
     * IPTR[0-7]: RO 0x00000000
     */
    reg = &REG_ICD.IPTR[8];
    for (i = 32U; i < CFG_GIC_INTNUM_MAX; i += 4U) {
        *reg++ = val;
    }
#endif /* #if (MPCORE == 1U) */

    /* Set priority */
    reg = &REG_ICD.IPR[0];
    for (i = 0U; i < CFG_GIC_INTNUM_MAX; i += 4U) {
#ifdef  REMOTE_CORE_ID
        *reg++ = 0xA0A0A0A0U;
#else
        *reg++ = 0xFFFFFFFFU;
#endif
    }

    REG_ICD.DCR = 1U;   /* enable interrupt */
}

#if (USE_SYSTEM != SYSTEM_SINGLE_CORE)
static void init_slave_core(void)
{
    UW i;
    volatile UW *reg = 0;

    /**
     * for SGI interrupts of slave cores
     */

    /** Group 0 */
    REG_ICD.ISR[0] = 0x00000000U;

    /** Disable */
    REG_ICD.ICER[0] = 0xFFFFFFFFU;

    /** Clear pending */
    REG_ICD.ICPR[0] = 0xFFFFFFFFU;

    /** Set priority */
    reg = &REG_ICD.IPR[0];
    for (i = 0U; i < 32U; i += 4U) {
        *reg++ = 0xFFFFFFFFU;
    }
}
#endif


/**
 * Initialize GIC
 * @param gic_base  ARM peripheral base address
 */
void _ddr_gic_init(UW gic_base)
{
    g_gicd = (volatile struct t_gicd *)(gic_base + GIC_DIST_OFFSET);
    g_gicc = (volatile struct t_gicc *)(gic_base + GIC_CPU_OFFSET);

    /*
     * distributor init
     */
#if (USE_SYSTEM == SYSTEM_SINGLE_CORE)
    init_master_core();
#else /* #if (USE_SYSTEM == SYSTEM_SINGLE_CORE) */

    if (get_cid() == ID_CORE0) {
        init_master_core();
    } else {
        init_slave_core();
    }
#endif /* #if (USE_SYSTEM == SYSTEM_SINGLE_CORE) */

    /*
     * CPU interface init
     */
    REG_ICC.PMR = CFG_GIC_PRI_MASK;
    REG_ICC.BPR = CFG_GIC_BIN_POINT;

    /* clear pending if there is any interrupts */
    do {
        UW intno;
        intno = REG_ICC.IAR & 0x1FFFU;
        if (intno == 1023U) {
            break;
        }
        REG_ICC.EOIR = intno;
    } while (1);

    /* enable interrupt */
#if (GIC_VER == GICv2)
    REG_ICC.ICR = 3U;
#elif (GIC_VER == GICv1)
    REG_ICC.ICR = 1U;
#endif
}

/**
 * Set interrupt detection type
 * @param intno     interrupt number
 * @param type      Interrupt types
 *                  GIC_IRQ_LEVEL_HIGH  - interrupt is level-sensitive.
 *                  GIC_IRQ_EDGE_RISING - interrupt is edge-triggered.
 * @return E_OK     trigger mode set
 *         E_PAR    invalid interrupt number
 */
ER _ddr_gic_int_type(INTNO intno, _ddr_gic_int_type_t type)
{
    ER ercd;

    if ((intno < 32U) || (CFG_GIC_INTNUM_MAX <= intno)) {
        ercd = E_PAR;
    } else {
        UW val;
        volatile UW *reg = 0;

        _kernel_lock();

        SPIN_LOCK();
        reg = &REG_ICD.ICFR[(intno >> 4)];
        val = *reg;
        switch (type) {
            case GIC_IRQ_LEVEL_HIGH:
                val &= ~(0x2U << ((intno & 0x0fU) * 2U));
                ercd = E_OK;
                break;
            case GIC_IRQ_EDGE_RISING:
                val |= (0x2U << ((intno & 0x0fU) * 2U));
                ercd = E_OK;
                break;
            default:
                ercd = E_PAR;
                break;

        }
        *reg = val;
        SPIN_UNLOCK();
        _kernel_unlock();
    }

    return ercd;
}

static ER _ena_int(INTNO intno)
{
    UINT pri;
    volatile UW *reg = 0;
    UW val, field_mask;

#if (MPCORE == 1U)
    if (intno >= 32U) {
        /*
         * For SPIs interrupt,
         * select the core that handles interrupt.
         */
        UW target_mask = 1U << (((ID)get_cid() - ID_CORE0) + (ID)CFG_GIC_IPTR_OFFSET);
        reg = &REG_ICD.IPTR[(intno >> 2)];
        target_mask = (target_mask       << ((intno & 3U) * 8U));
        field_mask  = (USEABLE_CORE_MASK << ((intno & 3U) * 8U));
        val = *reg;
        if (((target_mask & field_mask) != 0U) &&
            ((val & field_mask) == 0U)) {
            val &= ~(0xFFU << ((intno & 3U) * 8U));
            *reg = val | target_mask;
        } else {
            return E_PAR;
        }
    }
#endif /* #if (MPCORE == 1U) */

    pri = vinftbl[intno].intinfo & 0xffU;
    reg = &REG_ICD.IPR[(intno >> 2)];
    field_mask = (0xff << ((intno & 3U) * 8U));
    pri = (pri << ((intno & 3U) * 8U));
    val = *reg;
    if ((val & field_mask) != pri) {
        val &= ~field_mask;
        *reg = val | pri;
    }
    REG_ICD.ISER[(intno >> 5)] = (1U << (intno & 0x1fU));
    _kernel_synch_cache();

    return E_OK;
}

/**
 * Enable interrupt
 * @param intno     interrupt number
 * @return E_OK or error code
 */
ER ena_int(INTNO intno)
{
    ER ercd;

    if (intno < CFG_GIC_INTNUM_MAX) {
        _kernel_lock();
        if (intno >= 32U) {
            SPIN_LOCK();
            ercd = _ena_int(intno);
            SPIN_UNLOCK();
        } else {
            ercd = _ena_int(intno);
        }
        _kernel_unlock();
    } else {
        ercd = E_PAR;
    }
    return ercd;
}

static ER _dis_int(INTNO intno)
{
#if (MPCORE == 1U)
    if (intno >= 32U) {
        /* Clear own CPU targets field. */
        volatile UW *reg = 0;
        UW val;
        UW target_mask, field_mask;

        reg = &REG_ICD.IPTR[(intno >> 2)];
        target_mask = 1U << (((ID)get_cid() - ID_CORE0) + (ID)CFG_GIC_IPTR_OFFSET);
        target_mask = (target_mask << ((intno & 3U) * 8U));
        field_mask  = (USEABLE_CORE_MASK << ((intno & 3U) * 8U));
        val = *reg;
        if (((target_mask & field_mask) != 0U) &&
            ((val & target_mask) != 0U)) {
            val &= ~field_mask;
#ifdef REMOTE_CORE_ID
            target_mask = 1U << ((REMOTE_CORE_ID-ID_CORE0) + (ID)CFG_GIC_IPTR_OFFSET);
            val |= (target_mask << ((intno & 3U) * 8U));
#endif
            *reg = val;
        } else {
            return E_PAR;
        }
    }
#endif /* #if (MPCORE == 1U) */

    REG_ICD.ICER[(intno >> 5)] = (1U << (intno & 0x1fU));
    _kernel_synch_cache();

    return E_OK;
}

/**
 * Disable interrupt
 * @param intno     interrupt number
 * @return E_OK or error code
 */
ER dis_int(INTNO intno)
{
    ER ercd;

    if (intno < CFG_GIC_INTNUM_MAX) {
        _kernel_lock();
        if (intno >= 32U) {
            SPIN_LOCK();
            ercd = _dis_int(intno);
            SPIN_UNLOCK();
        } else {
            ercd = _dis_int(intno);
        }
        _kernel_unlock();
    } else {
        ercd = E_PAR;
    }
    return ercd;
}

/**
 * Send SGI to the specific core
 * @param coreid    destination core id
 * @param intno     SGI number
 */
void _ddr_gic_send_sgi(UINT coreid, INTNO intno)
{
    UW target = (1U << (coreid + 15U + CFG_GIC_IPTR_OFFSET));
    REG_ICD.SGIR = (target | (intno & 0xFU));
}


/* Kernel internal functions ------------------------- */

static void spurious_isr(void)
{
}

/**
 * Read the occurred interrupt information
 * @param intpara   interrupt information
 * @return TRUE or FALSE
 */
BOOL _kernel_pre_inthdr(T_INTPARA *intpara)
{
    BOOL retcd;
    UINT intno;
    UW pri_now;

    pri_now = REG_ICC.RPR & 0xFFU;   /* for reference of software if it will use at future */
    *intpara->savedt = (REG_ICC.IAR & 0x1FFFU) | (pri_now << 16);
    intno = *intpara->savedt & 0x3FFU;

    if (intno < CFG_GIC_INTNUM_MAX) {
        intpara->intinfo = vinftbl[intno].intinfo;
        intpara->intfunc = vinftbl[intno].intfunc;
        intpara->next = vinftbl[intno].next;
        retcd = TRUE;
    } else {
        intpara->intfunc = &spurious_isr;
        retcd = FALSE;
    }

    return retcd;
}

/**
 * End the interrupt handler
 * @param savedt    interrupt information
 */
void _kernel_post_inthdr(UW savedt)
{
    REG_ICC.EOIR = savedt & 0x1FFFU;
}

/*=====================================================================*/

/**
 * For CPU standby mode
 */

#if (USE_STANDBY_MODE == 1U)

/**
 * Backup GIC distributor registers
 * @param backup    GIC backup information
 */
void _ddr_cortexa_gic_backup(T_GIC_BACKUP * backup)
{
    REG_ICC.ICR = 0U;
    REG_ICD.DCR = 0U;

    memcpy(backup->ISER, (VP)g_gicd->ISER, sizeof(backup->ISER));
    memcpy(backup->IPR , (VP)g_gicd->IPR , sizeof(backup->IPR ));
#if (MPCORE == 1U)
    memcpy(backup->IPTR, (VP)g_gicd->IPTR, sizeof(backup->IPTR));
#endif /* #if (MPCORE == 1U) */
    memcpy(backup->ICFR, (VP)g_gicd->ICFR, sizeof(backup->ICFR));
}

/**
 * Restore GIC distributor registers
 * @param backup    GIC backup information
 */
void _ddr_cortexa_gic_restore(const T_GIC_BACKUP * backup)
{
    UINT cid = get_cid();

    /*
     * restore distributor
     */

    if (cid == ID_CORE0) {         /* only master core */
        UW i;
        volatile UW *reg = 0;

        /* Disable all */
        reg = &REG_ICD.ICER[0];
        for (i = 0U; i < CFG_GIC_INTNUM_MAX; i += 32U)
            *reg++ = 0xffffffffU;

        /* Clear pending all */
        reg = &REG_ICD.ICPR[0];
        for (i = 0U; i < CFG_GIC_INTNUM_MAX; i += 32U)
            *reg++ = 0xffffffffU;


        memcpy((VP)g_gicd->ISER, backup->ISER, sizeof(backup->ISER));
        memcpy((VP)g_gicd->IPR , backup->IPR , sizeof(backup->IPR ));
#if (MPCORE == 1U)
        memcpy((VP)g_gicd->IPTR, backup->IPTR, sizeof(backup->IPTR));
#endif /* #if (MPCORE == 1U) */
        memcpy((VP)g_gicd->ICFR, backup->ICFR, sizeof(backup->ICFR));

        REG_ICD.DCR = 1U;        /* enable interrupt */
    }

    /*
     * restore CPU interface
     */

    REG_ICC.PMR = CFG_GIC_PRI_MASK;
    REG_ICC.BPR = CFG_GIC_BIN_POINT;

    /* clear pending if there is any interrupts */
    do {
        UW intno;
        intno = REG_ICC.IAR & 0x1fffU;
        if (intno == 1023U) {
            break;
        }
        REG_ICC.EOIR = intno;
    } while (1);

    /* enable interrupt */
#if (GIC_VER == GICv2)
    REG_ICC.ICR = 3U;
#elif (GIC_VER == GICv1)
    REG_ICC.ICR = 1U;
#endif
}

#endif /* #if (USE_STANDBY_MODE == 1U) */
