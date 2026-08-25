/**
 * @file    DDR_CortexA_GIC.h
 * @brief   Micro C Cube Standard, DEVICE DRIVER
 *          ARM Generic Interrupt Controller
 * @date    2018.03.28
 * @author  Copyright (c) 2016-2018, eForce Co., Ltd. All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2016.04.27) y-kim
 *            Created based on DDR_RZG1_GIC.h.
 *          - rev 1.1 (2016.12.15) yokota
 *            Add cp15_get_cbar function.
 *          - rev 1.1 (2017.07.28) y-kim
 *            Added support for CPU standby mode.
 *          - rev 1.2 (2018.03.06) i-cho
 *            Added CPU interface and Distributor register definition.
 *          - rev 1.3 (2018.03.28) yokota
 *            Added include header DDR_CortexA_GIC_cfg.h
 *            Change t_gic_backup's macro from SYSTEM_SINGLE_CORE to MPCORE.
 *            Add _ddr_gic_send_sgi function.
 ****************************************************************************
 */
#ifndef DDR_CORTEXA_GIC_H_
#define DDR_CORTEXA_GIC_H_

#ifdef __cplusplus
extern "C"
{
#endif

/*
 * Generic Interrupt Controller
 */

/* for CPU interface register */
struct t_gicc {
    UW  ICR;                /* 0x0000 CPU Interface control */
    UW  PMR;                /* 0x0004 Interrupt Priority mask */
    UW  BPR;                /* 0x0008 Binary Point */
    UW  IAR;                /* 0x000C Interrupt Acknowledge */
    UW  EOIR;               /* 0x0010 End of interrupt */
    UW  RPR;                /* 0x0014 Running Priority */
    UW  HPIR;               /* 0x0018 Highest Pending interrupt */
    UW  ABPR;               /* 0x001C Aliased Non-secure binary point */
    UW  AIAR;               /* 0x0020 Aliased Interrupt Acknowledge Register */
    UW  AEOIR;              /* 0x0024 Aliased End of Interrupt Register */
    UW  AHPPIR;             /* 0x0028 Aliased Highest Priority Pending Interrupt Register */
    UW  reserved_0[41];
    UW  APR0;               /* 0x00D0 Active Priorities Register 0 */
    UW  reserved_1[3];
    UW  NSAPR0;             /* 0x00E0 Non-secure Active Priorities Register 0 */
    UW  reserved_2[6];
    UW  IDR;                /* 0x00FC CPU Interface implementer identification */
    UW  reserved_3[960];
    UW  DIR;                /* 0x1000 Deactivate Interrupt Register */
};

/* for Distributor register */
struct t_gicd {
    UW  DCR;                /* 0x0000 Distributor Control */
    UW  ICTR;               /* 0x0004 Interrupt Controller Type */
    UW  IIDR;               /* 0x0008 Distributor Implementer Identification */
    UW  reserved_0[29];
    UW  ISR[32];            /* Interrupt Security */
    UW  ISER[32];           /* 0x100 Interrupt Set-enable */
    UW  ICER[32];           /* 0x180 Interrupt Clear-enable */
    UW  ISPR[32];           /* 0x200 Interrupt Set-pending */
    UW  ICPR[32];           /* 0x280 Interrupt Clear-pending */
    UW  ABR[32];            /* 0x300 Active Bit */
    UW  reserved_1[32];
    UW  IPR[255];           /* 0x400 Interrupt Priority */
    UW  reserved_2;
    UW  IPTR[255];          /* 0x800 Interrupt Processor targets */
    UW  reserved_3;
    UW  ICFR[64];           /* 0xC00 Interrupt Configuration */
    UW  PPI_STATUS;         /* 0xD00 PPI Status */
    UW  SPI_STATUS0;        /* 0xD04 SPI Status 0 */
    UW  SPI_STATUS1;        /* 0xD08 SPI Status 1 */
    UW  reserved_4[125];
    UW  SGIR;               /* 0xF00 Software Generated interrupt */
    UW  reserved_5[51];
    UW  PIDR4;              /* 0xFD0 Peripheral ID4 */
    UW  PIDR5;              /* Peripheral ID5 */
    UW  PIDR6;              /* Peripheral ID6 */
    UW  PIDR7;              /* Peripheral ID7 */
    UW  PIDR0;              /* Peripheral ID0 */
    UW  PIDR1;              /* Peripheral ID1 */
    UW  PIDR2;              /* Peripheral ID2 */
    UW  PIDR3;              /* Peripheral ID3 */
    UW  CIDR0;              /* Component ID0 */
    UW  CIDR1;              /* Component ID1 */
    UW  CIDR2;              /* Component ID2 */
    UW  CIDR3;              /* Component ID3 */
};


typedef enum {
    GIC_IRQ_LEVEL_HIGH = 0U,    /* interrupt is level-sensitive */
    GIC_IRQ_EDGE_RISING         /* interrupt is edge-triggered. */

} _ddr_gic_int_type_t;

extern VP cp15_get_cbar(void);
extern void _ddr_gic_init(UW gic_base);
extern ER _ddr_gic_int_type(INTNO intno, _ddr_gic_int_type_t type);
extern void _ddr_gic_send_sgi(UINT coreid, INTNO intno);

#include "cpu_cfg.h"
#include "DDR_CortexA_GIC_cfg.h"

/**
 * For CPU standby mode
 */

/* For GIC register backup */
typedef struct t_gic_backup {
    UW  ISER[32];           /* GICD: 0x100 Interrupt Set-enable */
    UW  IPR[255];           /* GICD: 0x400 Interrupt Priority */
#if (MPCORE == 1U)
    UW  IPTR[255];          /* GICD: 0x800 Interrupt Processor targets */
#endif /* #if (MPCORE == 1U) */
    UW  ICFR[64];           /* GICD: 0xC00 Interrupt Configuration */
} T_GIC_BACKUP;

extern void _ddr_cortexa_gic_backup(T_GIC_BACKUP * backup);
extern void _ddr_cortexa_gic_restore(const T_GIC_BACKUP * backup);

#ifdef __cplusplus
}
#endif

#endif  /* DDR_CORTEXA_GIC_H_ */
