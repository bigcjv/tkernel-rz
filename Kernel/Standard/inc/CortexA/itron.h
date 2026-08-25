/***************************************************************************
    Micro C Cube Standard, KERNEL
    ITRON macro definitions for ARMv7-A dependent definitions

    Copyright (c)  2009-2019, eForce Co., Ltd. All rights reserved.

    Version Information
            2009.08.15: Created.
            2011.04.07: Added the exception ID.
            2011.10.27: Added the cache control function.
            2012.11.05: Supported for MPcore.
            2013.02.25: Added Endian macros for uNet3.
            2013.05.09: Added _kernel_disable_irq and _kernel_restore_irq.
            2015.05.08: Changed TKERNEL_PRVER to v1.11.0
            2015.08.28: Changed TKERNEL_PRVER to v1.12.0
            2016.01.06: Added external cache function declaration.
            2017.01.26: Fixed the IPA warnings.
            2017.07.19: Changed TKERNEL_PRVER to v1.30.0
            2017.09.01: Changed TKERNEL_PRVER to v1.31.0
            2018.02.20: Changed TKERNEL_PRVER to v1.32.0
            2018.03.08: Added flush cache function declaration.
                        delete obsolete _memory_barrier function.
            2018.03.13: Changed TKERNEL_PRVER to v1.32.2
            2018.03.27: Changed TKERNEL_PRVER to v1.33.0
            2018.03.30: Added ARM Compiler v6 byte order check.
                        Changed TKERNEL_PRVER to v1.33.2
            2018.04.04: Added "C" linkage macro.
                        Changed TKERNEL_PRVER to v1.33.4
            2018.07.12: Changed TKERNEL_PRVER to v1.40.0
            2019.04.08: Changed TKERNEL_PRVER to v1.41.0
  ***************************************************************************/

#ifndef _ITRON_H_
#define _ITRON_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL    (0)
#else
#define NULL    ((void *)0)
#endif
#endif

#define TRUE    1
#define FALSE   0

#define E_OK    0

#define ID_CORE0            1U
#define ID_CORE1            2U
#define ID_CORE2            3U
#define ID_CORE3            4U

#define TA_USR      0x10U

#define _kernel_INT_SIZE    4U
#define _kernel_SIZE_SIZE   4U
#define _kernel_ALIGN_SIZE  8U
#define _kernel_INT_BIT     (_kernel_INT_SIZE*8U)

#define TKERNEL_PRID    0x024AU
#define TKERNEL_PRVER   0x1410U

#define EXC_UDF     1U
#define EXC_SVC     2U
#define EXC_PRA     3U
#define EXC_DTA     4U


/************************************
    Byte Order Type
 ************************************/
#if defined (__ARMCOMPILER_VERSION)     /* for ARM Compiler v6 */
#if defined (__ARM_BIG_ENDIAN)
#define _UC3_ENDIAN_BIG
#undef _UC3_ENDIAN_LITTLE
#else
#define _UC3_ENDIAN_LITTLE
#undef _UC3_ENDIAN_BIG
#endif
#elif defined (__CC_ARM)    /* for ARM Compiler v5 */
#if defined (__BIG_ENDIAN)
#define _UC3_ENDIAN_BIG
#undef _UC3_ENDIAN_LITTLE
#else
#define _UC3_ENDIAN_LITTLE
#undef _UC3_ENDIAN_BIG
#endif
#elif defined (__ICCARM__)  /* for IAR Compiler */
#if (__LITTLE_ENDIAN__ == 1)
#define _UC3_ENDIAN_LITTLE
#undef _UC3_ENDIAN_BIG
#else
#define _UC3_ENDIAN_BIG
#undef _UC3_ENDIAN_LITTLE
#endif
#elif defined (__TMS470__)  /* for CCS Compiler */
#if defined (__little_endian__)
#define _UC3_ENDIAN_LITTLE
#undef _UC3_ENDIAN_BIG
#else
#define _UC3_ENDIAN_BIG
#undef _UC3_ENDIAN_LITTLE
#endif
#elif defined(__GNUC__)     /* for GNU C */
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define _UC3_ENDIAN_LITTLE
#undef _UC3_ENDIAN_BIG
#else
#define _UC3_ENDIAN_BIG
#undef _UC3_ENDIAN_LITTLE
#endif
#endif


/************************************
    Data Types
 ************************************/

typedef signed char B;
typedef signed short H;
typedef signed long W;
typedef unsigned char UB;
typedef unsigned short UH;
typedef unsigned long UW;
typedef char VB;
typedef short VH;
typedef long VW;
typedef void *VP;
typedef void (*FP)(void);

typedef signed long long D;
typedef unsigned long long UD;
typedef long long VD;

typedef unsigned long SIZE;
typedef unsigned long ADDR;

typedef int INT;
typedef unsigned int UINT;

typedef VP VP_INT;

typedef INT BOOL;
typedef INT FN;
typedef INT ER;
typedef INT ID;
typedef INT PRI;
typedef INT BOOL_ID;
typedef INT RDVNO;
typedef INT ER_ID;
typedef INT ER_UINT;
typedef UINT TEXPTN;
typedef UINT FLGPTN;
typedef UINT RDVPTN;
typedef UINT INHNO;
typedef UINT INTNO;
typedef UINT IMASK;
typedef UINT EXCNO;

typedef UINT ATR;
typedef UINT STAT;
typedef UINT MODE;

typedef struct t_systim {
    UW utime;
    UW ltime;
} SYSTIM;

typedef W TMO;

typedef UW RELTIM;
typedef UH OVRTIM;

/************************************
    ARMv7-A dependent definitions
 ************************************/

extern UW vget_fpscr(void);
extern void vset_fpscr(UW fpscr);
extern void vena_vfp(void);
extern void vdis_vfp(void);
extern ER _kernel_enavfp(UW *reg, UW psr);
extern void _kernel_invalid_cache(void);
extern void _kernel_clean_data_cache(void *ptr, SIZE sz);
extern void _kernel_flush_data_cache(void *ptr, SIZE sz);
extern void _kernel_invalid_data_cache(void *ptr, SIZE sz);
extern void _kernel_invalid_inst_cache(void *ptr, SIZE sz);
extern void _kernel_synch_cache(void);
extern void _kernel_memory_barrier(void);
extern void _kernel_invalid_tlb(void);
extern void _kernel_wait_for_interrupt(void);
extern void _kernel_clean_l2_cache(void *ptr, SIZE sz);     /* optional function */
extern void _kernel_flush_l2_cache(void *ptr, SIZE sz);     /* optional function */
extern void _kernel_invalid_l2_cache(void *ptr, SIZE sz);   /* optional function */
extern UINT get_cid(void);
extern UW _kernel_disable_irq(void);
extern void _kernel_restore_irq(UW cpsr);
extern void _kernel_start_systim(void);
extern void _kernel_stop_systim(void);

#ifdef __cplusplus
}
#endif

#endif /* _ITRON_H_ */
