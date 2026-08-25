 /**
 * @file    cpu_cfg.h
 * @brief   Configuration for CPU (Do not modify this file)
 * @date    2019.06.05
 * @author  Copyright (c) 2019, eForce Co., Ltd. All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2019.06.05)
 *            Initial version.
 ****************************************************************************
 */

/******************* !!!!! Do not modify !!!!!! *****************************/

#ifndef CPU_CFG_H_
#define CPU_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ARMv7-A */
#define CPU_CORTEX_A5       (1U)
#define CPU_CORTEX_A7       (2U)
#define CPU_CORTEX_A8       (3U)
#define CPU_CORTEX_A9       (4U)
#define CPU_CORTEX_A15      (5U)

/* ARMv8-A */
#define CPU_CORTEX_A53      (11U)
#define CPU_CORTEX_A57      (12U)

/* ARMv7-R */
#define CPU_CORTEX_R4       (21U)
#define CPU_CORTEX_R5       (22U)
#define CPU_CORTEX_R7       (24U)
#define CPU_CORTEX_R8       (25U)

/* ARMv7-M */
#define CPU_CORTEX_M3       (31U)
#define CPU_CORTEX_M4       (32U)
#define CPU_CORTEX_M7       (33U)

/* ARMv8-R */
#define CPU_CORTEX_R52      (41U)

/* uC3 platform */
#define SYSTEM_SINGLE_CORE  (1U)
#define SYSTEM_AMP          (2U)
#define SYSTEM_SMP          (3U)    /**< not supported */

/* Target CPU configuration */
#define USE_CPU             (CPU_CORTEX_A7)
#define USE_SYSTEM          (SYSTEM_SINGLE_CORE)

/* multicore processor? */
#define MPCORE              (1U)    /**< Cortex-A7 MP */

/* for hetero-core */
#define CLUSTER_NUMBER      (0U)

#define CFG_G1E

#ifdef __cplusplus
}
#endif
#endif /* CPU_CFG_H_ */
