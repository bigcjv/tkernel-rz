/**
 * @file    exception.c
 * @brief   Micro C Cube Standard, exception handler (Cortex-A)
 * @date    2018.03.05
 * @author  Copyright (c) 2018, eForce Co., Ltd. All rights reserved.
 *
 ******************************************************************************
 * @par     History
 *          - rev 1.0 (2018.03.05)
 *            Initial version.
 ******************************************************************************
 */

#include "kernel.h"

/*
 * Error state
 */
void int_abort(void)
{
    for(;;);
}

/*
 * Data abort exception handler
 */
void data_abort_exception_handler(UW *reg, UW psr)
{
    for(;;);
}

const T_DEXC dexc_dta = {TA_NULL, (FP)data_abort_exception_handler};

/*
 * Prefetch abort exception handler
 */
void prefetch_abort_exception_handler(UW *reg, UW psr)
{
    for(;;);
}

const T_DEXC dexc_pra = {TA_NULL, (FP)prefetch_abort_exception_handler};

/*
 * Undefined instruction  handler
 */
void undefined_instruction_handler(UW *reg, UW psr)
{
    ER ercd;
    ercd = _kernel_enavfp(reg, psr);
    if (ercd != E_OK) {
        int_abort();
    }
}

const T_DEXC dexc_vfp = {TA_NULL, (FP)undefined_instruction_handler};

