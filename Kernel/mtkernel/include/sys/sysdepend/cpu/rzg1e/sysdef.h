/* RZ/G1E CPU and peripheral constants used by the kernel port. */
#ifndef __SYS_SYSDEPEND_CPU_RZG1E_SYSDEF_H__
#define __SYS_SYSDEPEND_CPU_RZG1E_SYSDEF_H__

#include "../core/armv7a/sysdef.h"

#define INTERNAL_RAM_START  0x40100000UL
#define INTERNAL_RAM_END    0x40B00000UL
#define INTERNAL_RAM_SIZE   (INTERNAL_RAM_END - INTERNAL_RAM_START)
#define INITIAL_SP          INTERNAL_RAM_END

#define GICD_BASE           0xF1001000UL
#define GICC_BASE           0xF1002000UL
#define N_INTVEC            415

#undef GICD_IGROUPR_N
#undef GICD_ICFGR_N
#undef GICD_IPRIORITYR_N
#undef GICD_ITARGETR_N
#undef GICD_ISENABLER_N
#undef GICD_ICENABLER_N
#define GICD_IGROUPR_N      ((N_INTVEC + 31) / 32)
#define GICD_ICFGR_N        ((N_INTVEC + 15) / 16)
#define GICD_IPRIORITYR_N   ((N_INTVEC + 3) / 4)
#define GICD_ITARGETR_N     ((N_INTVEC + 3) / 4)
#define GICD_ISENABLER_N    ((N_INTVEC + 31) / 32)
#define GICD_ICENABLER_N    ((N_INTVEC + 31) / 32)

#define INTPRI_BITWIDTH     5
#define INTPRI_SHIFT        (8 - INTPRI_BITWIDTH)
#define INTPRI_HIGHEST      0
#define INTPRI_LOWEST       31
#define INTPRI_SYSTICK      1

#define INTNO_SW0           0
#define INTNO_SW15          15
#define INTNO_SYSTICK       29
#define TIMER_INTLEVEL      0

#define COUNT_PER_SEC       32500000LL
#define NSEC_PER_COUNT      31
#define MIN_TIMER_PERIOD    1
#define MAX_TIMER_PERIOD    50

#define RZG1E_SYSCNT_BASE   0xE6080000UL
#define RZG1E_SYSCNT_CNTCR  (RZG1E_SYSCNT_BASE + 0x0000UL)
#define RZG1E_SYSCNT_CNTFID0 (RZG1E_SYSCNT_BASE + 0x0020UL)

#define RZG1E_SCIF0_BASE    0xE6E60000UL
#define RZG1E_PFC_BASE      0xE6060000UL
#define RZG1E_GPIO6_BASE    0xE6055400UL

#endif /* __SYS_SYSDEPEND_CPU_RZG1E_SYSDEF_H__ */
