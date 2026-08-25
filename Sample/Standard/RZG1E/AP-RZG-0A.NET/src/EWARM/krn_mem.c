/**
 * @file    krn_mem.c
 * @brief   Memory definitions to use in kernel inside.
 * @date    2016.05.27
 * @author  Copyright (c) 2016, eForce Co., Ltd. All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2016.05.27) y-kim
 *            Initial version.
 ****************************************************************************
 */
#include "kernel.h"
#include "krn_cfg.h"

/* System memory */
#pragma section = "SYSMEM"
UB SYSMEM[CFG_KRN_SYSMEM_SZ] @ "SYSMEM";

/* Stack memory */
#pragma section = "STKMEM"
UB STKMEM[CFG_KRN_STKMEM_SZ] @ "STKMEM";

/* Memory pool */
#pragma section = "MPLMEM"
UB MPLMEM[CFG_KRN_MPLMEM_SZ] @ "MPLMEM";
