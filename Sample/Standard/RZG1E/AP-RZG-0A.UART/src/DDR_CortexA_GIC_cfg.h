/**
 * @file    DDR_CortexA_GIC_cfg.h
 * @brief   User configuration for ARM Generic Interrupt Controller
 * @date    2019.06.05
 * @author  Copyright (c) 2019, eForce Co., Ltd. All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2019.06.05)
 *            Initial version.
 ****************************************************************************
 */
#ifndef DDR_CORTEXA_GIC_CFG_H_
#define DDR_CORTEXA_GIC_CFG_H_

#define CFG_G1E
#include "RZG1_UC3.h"

#ifdef __cplusplus
extern "C" {
#endif

/* priority mask for ICCICR register */

#define CFG_GIC_BASE          (0xF1000000UL)
#define CFG_GIC_INTNUM_MAX    (415U)
#define CFG_GIC_PRI_MASK      (0xF8U)
#define CFG_GIC_BIN_POINT     (2U)
#define CFG_GIC_IPTR_OFFSET   (0U)
  
#ifdef __cplusplus
}
#endif
#endif /* DDR_CORTEXA_GIC_CFG_H_ */
