/**
 * @file    DDR_CortexA_GIC_sub.c
 * @brief   Micro C Cube Standard, DEVICE DRIVER
 *          ARM Generic Interrupt Controller
 * @date    2017.11.09
 * @author  Copyright (c) 2016-2017, eForce Co., Ltd. All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2016.05.27) y-kim
 *            Created based on compiler dependent code of DDR_RZG1_GIC.c.
 *          - rev 1.1 (2016.12.15) yokota
 *            Add cp15_get_cbar function.
 *          - rev 1.2 (2017.11.09) i-cho
 *            Corrected SINGLE_CORE conditional directive.
 ****************************************************************************
 */
#include "uC3sys.h"
#include "DDR_CortexA_GIC_cfg.h"
#include "cpu_cfg.h"

/* External variables --------------------------------------------------------*/

#pragma section = "VINFTBL"
T_VINFTBL vinftbl[CFG_GIC_INTNUM_MAX] @ "VINFTBL";
#pragma section = "VECTTBL"
__root int _kernel_dummy_2 @ "VECTTBL";

#if (USE_SYSTEM != SYSTEM_SINGLE_CORE)
#pragma section = "SYNC"
__root UW _ddr_gic_flag @ "SYNC";
#endif /* #if (USE_SYSTEM != SYSTEM_SINGLE_CORE) */

/**
 * Read CP15 CBAR (Processor Feature Register 1)
 */
VP cp15_get_cbar(void)
{
    VP reg;
    asm( "MRC p15, 4,  %0, c15, c0, 0"  : "=r"(reg) );
    return reg;
}
