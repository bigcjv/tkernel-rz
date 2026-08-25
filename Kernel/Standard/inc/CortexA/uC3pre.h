/***************************************************************************
    Micro C Cube Standard, KERNEL
    ARMv7-A dependent internal predefinitions

    Copyright (c)  2009-2018, eForce Co., Ltd. All rights reserved.

    Version Information
            2009.08.16: Created.
            2011.04.06: Removed the exception ID.
            2011.03.02: Added cache control functions.
            2013.02.19: Supported the AMP multi-core kernel.
            2017.01.26: Fixed the IPA warnings.
            2018.04.04: Added "C" linkage macro.
 ***************************************************************************/

#ifndef _UC3PRE_H_
#define _UC3PRE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef VW  T_REG;

typedef struct t_cpudep {
    FP      udfhdr;
    FP      svchdr;
    FP      prahdr;
    FP      dtahdr;
    T_REG   fpscr;
} T_CPUDEP;

#define CPU_DEPENDEND T_CPUDEP  cpudep;
#define _KERNEL_START_MULTI_TASK()  (_kernel_start_multi_task())

#ifdef __cplusplus
}
#endif

#endif
