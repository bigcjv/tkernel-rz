/**
 * @file    DDR_CortexA_GTIMER_cfg.h
 * @brief   User configuration for ARM Generic Timer
 * @date    2019.06.05
 * @author  Copyright (c) 2019, eForce Co., Ltd. All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2019.06.05)
 *            Initial version.
 ****************************************************************************
 */
#ifndef DDR_CORTEXA_GTIMER_CFG_H_
#define DDR_CORTEXA_GTIMER_CFG_H_

#define CFG_G1E
#include "RZG1_UC3.h"

#ifdef __cplusplus
extern "C" {
#endif

#define CFG_GTIMER_INTNO    (INT_S_PHYSICAL_TIMER)
#define CFG_GTIMER_REG_BASE (0xE6080000UL)

/* タイマ割込み優先度 (8,16,...,240,248) */
#define CFG_GTIMER_IPL      (32U)

#define MASTER_CORE_ID      (ID_CORE0)

#ifdef __cplusplus
}
#endif
#endif /* DDR_CORTEXA_GTIMER_CFG_H_ */

