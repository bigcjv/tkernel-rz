/***************************************************************************
    Micro C Cube Standard, KERNEL
    ARMv7-A dependent internal edefinitions

    Copyright (c)  2009-2013, eForce Co.,Ltd.  All rights reserved.

    Version Information
            2009.08.16: Created.
            2011.10.27: Removed the cache control function.
            2013.02.19: Supported the AMP multi-core kernel.
            2016.03.01: Added "C" linkage macro.
 ***************************************************************************/

#ifndef _UC3CPU_H_
#define _UC3CPU_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct t_intpara {
    UW      *savedt;
    UW      intinfo;
    FP      intfunc;
    T_ISR   *next;
} T_INTPARA;

typedef struct t_vinftbl {
    UW      intinfo;
    FP      intfunc;
    T_ISR   *next;
    T_ISR   *prev;
} T_VINFTBL;

typedef struct t_cortexa_defint {
    UW          cmd[11];
    FP          entry;
    T_VINFTBL   vinftbl;
} T_CORTEXA_DEFINT;

extern IMASK _kernel_get_imask(void);
extern IMASK _kernel_set_imask(IMASK imask);
extern void _kernel_start_multi_task(void);

#ifdef __cplusplus
}
#endif


#endif
