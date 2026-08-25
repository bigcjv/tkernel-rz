/**
 * @file    DDR_CortexA_GETIMER.h
 * @brief   Micro C Cube Standard, DEVICE DRIVER
 *          Cortex-A7, Cortex-A15, ARMv8 and later Generic Timer.
 * @date    2017.07.28
 * @author  Copyright (c) 2016-2017, eForce Co., Ltd. All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2016.05.13) y-kim
 *            Created based on DDR_RZG1_GETIMER.h
 *          - rev 1.1 (2017.07.28) y-kim
 *            Added support for CPU standby mode.
 ****************************************************************************
 */
#ifndef _DDR_CORTEXA_GTIMER_H_
#define _DDR_CORTEXA_GTIMER_H_

#ifdef __cplusplus
extern "C"
{
#endif

extern ER _ddr_cortexa_gtimer_init(UINT tick, UW base_clk);

/**
 * For CPU standby mode
 */
extern void _ddr_cortexa_gtimer_backup(void);
extern void _ddr_cortexa_gtimer_restore(void);

#ifdef __cplusplus
}
#endif

#endif  /* _DDR_CORTEXA_GTIMER_H_ */
