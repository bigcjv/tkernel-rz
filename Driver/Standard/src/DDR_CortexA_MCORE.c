/**
 * @file    DDR_CortexA_MCORE.c
 * @brief   Micro C Cube Standard, Multi Core extension
 *          Synchronization and Asynchronization System Call
 * @date    2018.03.26
 * @author  Copyright (c) 2017-2018, eForce Co., Ltd. All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2017.10.11) i-cho
 *            Initial version.
 *          - rev 1.1 (2017.10.12) i-cho
 *            _ddr_mcore_syscall_cisr is moved to local variable.
 *            Fixed the IPA warnings.
 *          - rev 1.2 (2018.03.26) yokota
 *            Fixed synchronization.
 ****************************************************************************
 */

#include <string.h>
#include "uC3sys.h"
#include "DDR_CortexA_GIC.h"
#include "DDR_CortexA_MCORE_cfg.h"
#include "uC3mcext.h"

/* External functions --------------------------------------------------------*/

extern void _ddr_mcore_syscall_isr(void);


/* External variables --------------------------------------------------------*/

extern T_ASYNCTBL _kernel_asynctbl[];
extern T_SYNCTBL _kernel_synctbl[];
extern T_MCORE_EXT _kernel_mcore_ext;
extern volatile UW _kernel_sync_core;
extern UW ddr_mcore_flag;

/* Private macro -------------------------------------------------------------*/

#if (CORE_NUM   == 1U)
    #define MCORE_SYNC_WORD  0x00000002U
#elif (CORE_NUM == 2U)
    #define MCORE_SYNC_WORD  0x00000202U
#elif (CORE_NUM == 3U)
    #define MCORE_SYNC_WORD  0x00020202U
#elif (CORE_NUM == 4U)
    #define MCORE_SYNC_WORD  0x02020202U
#else
    #error  "Unsupported CORE_NUM"
#endif

/*
 * set event for inter-core system call
 */
void _kernel_async_sync_event_set(ID coreid)
{
    _ddr_gic_send_sgi((UINT)coreid, CFG_MCORE_SYSCALL_INTNO);
    _kernel_synch_cache();
}


/*
 * clear event for inter-core system call
 */
void _kernel_async_sync_event_clear(void)
{
    ;
}


/*
 * Initialize for inter-core system call
 */

ER _ddr_mcore_syscall_init(void)
{
    ER ercd;
    UINT my_coreid = get_cid();
    UINT idx = my_coreid - ID_CORE0;

    if (idx >= CORE_NUM) {
        ercd = E_PAR;
    } else {
        T_CISR ddr_mcore_syscall_cisr;
        ddr_mcore_syscall_cisr.isratr = TA_NULL;
        ddr_mcore_syscall_cisr.exinf  = (VP_INT)0;
        ddr_mcore_syscall_cisr.intno  = CFG_MCORE_SYSCALL_INTNO;
        ddr_mcore_syscall_cisr.isr    = (FP)_ddr_mcore_syscall_isr;
        ddr_mcore_syscall_cisr.imask  = (IMASK)CFG_MCORE_SYSCALL_IPL;

        if (my_coreid == ID_CORE0) {
            _kernel_mcore_ext.core_max = CORE_NUM;
            _kernel_mcore_ext.async_max = ASYNCBUF_NUM;
        }

        memset(&_kernel_synctbl[idx], 0x00, sizeof(T_SYNCTBL));
        memset(&_kernel_asynctbl[idx], 0x00, sizeof(T_ASYNCTBL));
        _kernel_mcore_ext.asynctbl[idx] = &_kernel_asynctbl[idx];
        _kernel_mcore_ext.synctbl[idx] = &_kernel_synctbl[idx];
        ercd = acre_isr((T_CISR *)&ddr_mcore_syscall_cisr);
        if (ercd >= E_OK) {
            _kernel_mcore_ext.ready[idx] = 1U;
            _kernel_synch_cache();
            (void)ena_int(CFG_MCORE_SYSCALL_INTNO);
            ercd = E_OK;
        }
    }

    return ercd;
}


/*
 * Wait until the other cores boot up
 */
void _kernel_start_multi_task(void)
{
    UINT my_coreid;
    UINT idx;


    my_coreid = get_cid();
    idx = my_coreid - ID_CORE0;

    _kernel_spin_lock(&ddr_mcore_flag);
    _kernel_sync_core |= (2U << (idx * 8U));
    _kernel_synch_cache();
    _kernel_spin_unlock(&ddr_mcore_flag);

    for(; _kernel_sync_core != MCORE_SYNC_WORD; ){
        ;
    }
    _kernel_mcore_ext.ready[idx] = 2U;
    _kernel_synch_cache();
}
