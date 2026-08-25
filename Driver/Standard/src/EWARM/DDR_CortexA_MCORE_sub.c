/**
 * @file    DDR_CortexA_MCORE_sub.c
 * @brief   Micro C Cube Standard, Multi Core extension
 *          Synchronization and Asynchronization System Call
 * @date    2019.06.05
 * @author  Copyright (c) 2019, eForce Co., Ltd. All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2019.06.05) i-cho
 *            Initial version.
 ****************************************************************************
 */
#include "uC3sys.h"
#include "DDR_CortexA_MCORE_cfg.h"
#include "uC3mcext.h"

/* External variables --------------------------------------------------------*/

#pragma section = "SYNC"
__root T_ASYNCTBL  _kernel_asynctbl[CORE_NUM] @ "SYNC";
__root T_SYNCTBL   _kernel_synctbl[CORE_NUM] @ "SYNC";
__root T_MCORE_EXT _kernel_mcore_ext @ "SYNC";
__root volatile UW _kernel_sync_core @ "SYNC";
__root UW ddr_mcore_flag @ "SYNC";
