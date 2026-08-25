/**
 * @file    DDR_CortexA_MMU.h
 * @brief   Micro C Cube Standard, MMU and Cache management (Cortex-A)
 * @date    2018.03.01
 * @author  Copyright (c) 2013-2018, eForce Co., Ltd. All rights reserved.
 *
 ******************************************************************************
 * @par     History
 *          - rev 1.0 (2013.04.17)
 *            Initial version.
 *          - rev 1.1 (2016.04.27) y-kim
 *            Changed to C style code from assembly
 *          - rev 1.2 (2017.09.27) yokota
 *            add mem_cfg_t structure difinition. 
 *          - rev 1.3 (2018.03.01) yokota
 *            add T_MEM_CFG structure, add U suffix.
 ******************************************************************************
 */
#ifndef DDR_CORTEXA_MMU_H_
#define DDR_CORTEXA_MMU_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*
 *      Access permissions
 *
 *                          Privileged permissions      User permissions
 */
#define AP_NA       0x00000000U  /* No access                 No access  */
#define AP_RW       0x00000C30U  /* Read/write                Read/write */
#define AP_RO       0x00008E30U  /* Read-only                 Read-only  */
#define AP_RWNA     0x00000410U  /* Read/write                No access  */
#define AP_RWRO     0x00000820U  /* Read/write                Read-only  */
#define AP_RONA     0x00008610U  /* Read-only                 No access  */

/*
 *      Memory region attributes
 *
 *          ATR_STRG -----------------------------------------+
 *          ATR_SDEV -----------------------------------------+
 *          ATR_WTNW -----------------------------------------+
 *          ATR_WBNW -----------------------------------------+
 *          ATR_NONC -----------------------------------------+-- [ATR_SHRD} -- [ATR_NEXC} -- [ATR_NGLB}
 *          ATR_WBAW -----------------------------------------+
 *          ATR_NDEV -----------------------------------------+
 *          ATR_SELA --+-- ATR_INONC --+---+-- ATR_ONONC --+--+
 *                     +-- ATR_IWBAW --+   +-- ATR_OWBAW --+
 *                     +-- ATR_IWTNW --+   +-- ATR_OWTNW --+
 *                     +-- ATR_IWBNW --+   +-- ATR_OWBNW --+
 *                                                                  Memory type         Page shareable
 */

#define ATR_STRG   0x00000000U  /* Strongly Ordered                  Strongly Ordered    Shared       */
#define ATR_SDEV   0x00000004U  /* Shared Device                     Device              Shared       */
#define ATR_WTNW   0x00000008U  /* Outer and Inner Write-Through,                                     */
                                /* No Allocate on Write              Normal              S-bit        */
#define ATR_WBNW   0x0000000CU  /* Outer and Inner Write-Back,                                        */
                                /* No Allocate on Write              Normal              S-bit        */
#define ATR_NONC   0x00001040U  /* Outer and Inner Noncachable       Normal              S-bit        */
#define ATR_WBAW   0x0000104CU  /* Outer and Inner Write-Back,                                        */
                                /* Allocate on Write                 Normal              S-bit        */
#define ATR_NDEV   0x00002080U  /* Non-Shared Device                 Device              Non-shared   */
#define ATR_SELA   0x00004100U  /* Outer and Inner attributes                                         */
                                /* selectable                        Normal              S-bit        */

#define ATR_INONC  0x00000000U  /* Inner Noncachable                                                  */
#define ATR_IWBAW  0x00000004U  /* Inner Write-Back cached, Write Allocate                            */
#define ATR_IWTNW  0x00000008U  /* Inner Write-Through cached, No Allocate on Write                   */
#define ATR_IWBNW  0x0000000CU  /* Inner Write-Back cached, No Allocate on Write                      */

#define ATR_ONONC  0x00000000U  /* Outer Noncachable                                                  */
#define ATR_OWBAW  0x00001040U  /* Outer Write-Back cached, Write Allocate                            */
#define ATR_OWTNW  0x00002080U  /* Outer Write-Through cached, No Allocate on Write                   */
#define ATR_OWBNW  0x000030C0U  /* Outer Write-Back cached, No Allocate on Write                      */

#define ATR_NEXC   0x00008011U  /* Execute-Never attribute                                            */
#define ATR_SHRD   0x00010400U  /* Shared attribute                                                   */
#define ATR_NGLB   0x00020800U  /* Not-Global attribute                                               */

/*
 * Memory region configuration table
 */
typedef struct {
    UW    sz;       /* size */
    UW    p_addr;   /* physical address */
    UW    v_addr;   /* virtual address */
    UW    ap;       /* access privilege */
    UW    attr;     /* attribute */
} T_MEM_CFG;

#ifdef __cplusplus
}
#endif

#endif  /* DDR_CORTEXA_MMU_H_ */
