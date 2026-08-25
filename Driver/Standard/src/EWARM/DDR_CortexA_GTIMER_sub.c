/**
 * @file    DDR_CortexA_GETIMER_sub.c
 * @brief   Micro C Cube Standard, DEVICE DRIVER
 *          Cortex-A7, Cortex-A15, ARMv8 and later Generic Timer.
 * @date    2016.12.15
 * @author  Copyright (c) 2016, eForce Co., Ltd. All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2016.05.27) y-kim
 *            Created based on compiler dependent code of DDR_RZG1_GETIMER.c.
 *          - rev 1.1 (2016.12.15) yokota
 *            add cp15_get_cntfrq function. 
 ****************************************************************************
 */
#include "kernel.h"

/*
 * Write CP15 CNTFRQ (Counter Frequency register)
 */
void cp15_set_cntfrq(UW val)
{
    asm( "mcr p15, 0, %0, c14, c0, 0" : "+r"(val) );
}

/*
 * Read CP15 CNTFRQ (Counter Frequency register)
 */
UW cp15_get_cntfrq(void)
{
    UW reg;
    asm( "mrc p15, 0, %0, c14, c0, 0" : "=r"(reg) );
    return reg;
}

/**
 * Read CP15 ID_PFR1 (Processor Feature Register 1)
 */
UW cp15_get_cpuid_pfr1(void)
{
    UW reg;
    asm( "mrc p15, 0, %0, c0, c1, 1" : "=r"(reg) );
    return reg;
}

/*
 * Write CP15 CNTP_CTL (PL1 Physical Timer Control register)
 */
void cp15_set_cntp_ctl(UW val)
{
    asm( "mcr  p15, 0, %0, c14, c2, 1" : "+r"(val) );
    asm( "isb" );
}

/*
 * Read CP15 CNTP_CTL (PL1 Physical Timer Control register)
 */
UW cp15_get_cntp_ctl(void)
{
    UW val;
    asm( "mrc  p15, 0, %0, c14, c2, 1" : "=r"(val) );
    asm( "isb" );
    return val;
}

/*
 * Write CP15 CNTP_TVAL (PL1 Physical TimerValue register)
 */
void cp15_set_cntp_tval(UW val)
{
    asm( "mcr p15, 0, %0, c14, c2, 0" : "+r"(val) );
    asm( "isb" );
}

/*
 * Read CP15 CNTP_TVAL (PL1 Physical TimerValue register)
 * Timer value is signed.
 */
W cp15_get_cntp_tval(void)
{
    W val;
    asm( "isb" );
    asm( "mrc p15, 0, %0, c14, c2, 0" : "=r"(val) );
    return val;
}

