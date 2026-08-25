/**
 * @file    RZG1_UC3.h
 * @brief   RZ/G1 CPU (ARM Cortex-A7 MPCore) processor register definitions
 * @date    2015.12.11
 * @author  Copyright (c) 2015, eForce Co.,Ltd.  All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2015.12.11) i-cho
 *            Reference manual RZ/G Series User's Manual: Hardware Rev.0.50
 *          - rev 1.1 (2016.03.08) i-cho
 *            Added, EtherMAC register.
 *          - rev 1.2 (2016.05.12) i-cho
 *            Added, SCIF register.
 *          - rev 1.3 (2016.08.17) i-cho
 *            Added, RZ/G1H interrupt number.
 *          - rev 1.4 (2017.02.19) j-hirata
 *            Added, Ethernet AVB registers (struct t_ether_avb)
 *          - rev 1.5 (2019.05.08)
 *            Added, RZ/G1N interrupt number, PFC register
 ****************************************************************************
 */
#ifndef _RZG1_UC3_H_
#define _RZG1_UC3_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "itron.h"

/********************************************************
 * Address Space (Legacy 4-Gbyte Space)
 ********************************************************
 */
#define LBSC_AREA0_BASE     (0x00000000UL)
#define LBSC_AREA1_BASE     (0x04000000UL)
#define DBSC3_BASE          (0x40000000UL)  /* SDRAM Area */
#define SYSAPB1_BASE        (0xE6000000UL)  /* System-domain peripheral bus (CP)    */
#define SYSAPB2_BASE        (0xE6200000UL)  /* System-domain peripheral bus (HP)    */
#define SYSAPB3_BASE        (0xE6C00000UL)  /* System-domain peripheral bus (MP)    */
#define PERI_65MHZ_BASE     (SYSAPB3_BASE+0x00200000UL) /* 65MHz clock module domain */
#define S3CTRL_BASE         (0xE8000000UL)  /* IPMMU, ICB control register          */
#define ON_CHIP_RAM_BASE    (0xE8080000UL)  /* On-chip RAM                          */
#define ETHERAVB_DESC_BASE  (0xEE0E8000UL)  /* SRAM area (16KB) for the EtherAVB descriptor */
#define ETHERMAC_BASE       (0xEE700000UL)  /* EtherMAC                             */
#define GIC_BASE            (0xF1000000UL)  /* GIC                                  */
#define LBSC_CFG_BASE       (0xFEC00000UL)  /* Asynchronous memory controller control register */
#define RTAPB1_BASE         (0xFF000000UL)  /* RT-APB1 (bck)    */
#define RTAPB2_BASE         (0xFFE80000UL)  /* RT-APB2 (hpck)   */
#define RTAPB3_BASE         (0xFFF60000UL)  /* RT-APB3 (pclk)   */


/********************************************************
 * Peripheral IO definitions
 ********************************************************
 */

#ifndef uint32_t
#define uint32_t    UW
#endif
#ifndef uint16_t
#define uint16_t    UH
#endif
#ifndef uint8_t
#define uint8_t     UB
#endif
#ifndef int32_t
#define int32_t     W
#endif
#ifndef int16_t
#define int16_t     H
#endif
#ifndef int8_t
#define int8_t      B
#endif

union iodefine_reg32_t
{
    volatile uint32_t  UINT32;          /*  32-bit Access   */
    volatile uint16_t  UINT16[2];       /*  16-bit Access   */
    volatile uint8_t   UINT8[4];        /*  8-bit Access    */
};

union iodefine_reg32_16_t
{
    volatile uint32_t  UINT32;          /*  32-bit Access   */
    volatile uint16_t  UINT16[2];       /*  16-bit Access   */
};

union iodefine_reg16_8_t
{
    volatile uint16_t  UINT16;          /*  16-bit Access   */
    volatile uint8_t   UINT8[2];        /*  8-bit Access    */
};


/**
 * RCLK watchdog timer (RWDT)
 */
struct t_rwdt {
    UW  RWTCNT;     /* 0x0000 RCLK watchdog timer counter */
    UW  RWTCSRA;    /* 0x0004 RCLK watchdog timer control/status register A */
    UW  RWTCSRB;    /* 0x0008 RCLK watchdog timer control/status register B */
};

#define REG_RWDT    (*(volatile struct t_rwdt *)(SYSAPB1_BASE+0x020000U))


/**
 * General-Purpose Input/Output Ports (GPIO)
 */
struct t_gpio {
    UW  IOINTSEL;   /* 0x0000 General IO/interrupt switching register   */
    UW  INOUTSEL;   /* 0x0004 General input/output switching register   */
    UW  OUTDT;      /* 0x0008 General output register                   */
    UW  INDT;       /* 0x000C General input register                    */
    UW  INTDT;      /* 0x0010 Interrupt display register                */
    UW  INTCLR;     /* 0x0014 Interrupt clear register                  */
    UW  INTMSK;     /* 0x0018 Interrupt mask register                   */
    UW  MSKCLR;     /* 0x001C Interrupt mask clear register 0           */
    UW  POSNEG;     /* 0x0020 Positive/negative logic select register   */
    UW  EDGLEVEL;   /* 0x0024 Edge/level select register                */
    UW  FILONOFF;   /* 0x0028 Chattering prevention on/off register     */
    UW  reserved_00[3];
    UW  INTMSKS;    /* 0x0038 Interrupt sub mask register               */
    UW  MSKCLRS;    /* 0x003C Interrupt sub mask clear register         */
    UW  OUTDTSEL;   /* 0x0040 Output data select register               */
    UW  OUTDTH;     /* 0x0044 Output data high register                 */
    UW  OUTDTL;     /* 0x0048 Output data low register                  */
    UW  BOTHEDGE;   /* 0x004C One edge/both edge select registe         */
};

#define REG_GPIO0   (*(volatile struct t_gpio *)(SYSAPB1_BASE+0x050000U))
#define REG_GPIO1   (*(volatile struct t_gpio *)(SYSAPB1_BASE+0x051000U))
#define REG_GPIO2   (*(volatile struct t_gpio *)(SYSAPB1_BASE+0x052000U))
#define REG_GPIO3   (*(volatile struct t_gpio *)(SYSAPB1_BASE+0x053000U))
#define REG_GPIO4   (*(volatile struct t_gpio *)(SYSAPB1_BASE+0x054000U))
#define REG_GPIO5   (*(volatile struct t_gpio *)(SYSAPB1_BASE+0x055000U))
#define REG_GPIO6   (*(volatile struct t_gpio *)(SYSAPB1_BASE+0x055400U))
#define REG_GPIO7   (*(volatile struct t_gpio *)(SYSAPB1_BASE+0x055800U))


/**
 * Pin Function Controller (PFC)
 */
struct t_pfc {
    UW  PMMR;       /* 0x0000 LSI Multiplexed Pin Setting Mask Register    */
    UW  GPSR0;      /* 0x0004 GPIO/peripheral function select register 0   */
    UW  GPSR1;      /* 0x0008 GPIO/peripheral function select register 1   */
    UW  GPSR2;      /* 0x000C GPIO/peripheral function select register 2   */
    UW  GPSR3;      /* 0x0010 GPIO/peripheral function select register 3   */
    UW  GPSR4;      /* 0x0014 GPIO/peripheral function select register 4   */
    UW  GPSR5;      /* 0x0018 GPIO/peripheral function select register 5   */
    UW  GPSR6;      /* 0x001C GPIO/peripheral function select register 6   */
    UW  IPSR0;      /* 0x0020 Peripheral function select register 0        */
    UW  IPSR1;      /* 0x0024 Peripheral function select register 1        */
    UW  IPSR2;      /* 0x0028 Peripheral function select register 2        */
    UW  IPSR3;      /* 0x002C Peripheral function select register 3        */
    UW  IPSR4;      /* 0x0030 Peripheral function select register 4        */
    UW  IPSR5;      /* 0x0034 Peripheral function select register 5        */
    UW  IPSR6;      /* 0x0038 Peripheral function select register 6        */
    UW  IPSR7;      /* 0x003C Peripheral function select register 7        */
    UW  IPSR8;      /* 0x0040 Peripheral function select register 8        */
    UW  IPSR9;      /* 0x0044 Peripheral function select register 9        */
    UW  IPSR10;     /* 0x0048 Peripheral function select register 10       */
    UW  IPSR11;     /* 0x004C Peripheral function select register 11       */
    UW  IPSR12;     /* 0x0050 Peripheral function select register 12       */
    UW  IPSR13;     /* 0x0054 Peripheral function select register 13       */
    UW  IPSR14;     /* 0x0058 Peripheral function select register 14       */
    UW  IPSR15;     /* 0x005C Peripheral function select register 15       */
    UW  IOCTRL0;    /* 0x0060 SD control register 0                        */
    UW  IOCTRL1;    /* 0x0064 SD control register 1                        */
    UW  reserved_01[2];
    UW  IOCTRL;     /* 0x0070 IIC, IICDVFS, MMC IO cell control register   */
    UW  GPSR7;      /* 0x0074 GPIO/peripheral function select register 7   */
    UW  reserved_02[3];
    UW  IOCTRL4;    /* 0x0084 VI1 Driving ability control register         */
    UW  IOCTRL5;    /* 0x0088 TDSEL control register                       */
    UW  IOCTRL6;    /* 0x008C POC control register                         */
    UW  MOD_SEL;    /* 0x0090 Module select register                       */
    UW  MOD_SEL2;   /* 0x0094 Module select register 2                     */
    UW  MOD_SEL3;   /* 0x0098 Module select register 3                     */
    UW  MOD_SEL4;   /* 0x009C Module select register 4                     */
    UW  reserved_03[24];
    UW  PUPR0;      /* 0x0100 LSI pin pull-up control register 0           */
    UW  PUPR1;      /* 0x0104 LSI pin pull-up control register 1           */
    UW  PUPR2;      /* 0x0108 LSI pin pull-up control register 2           */
    UW  PUPR3;      /* 0x010C LSI pin pull-up control register 3           */
    UW  PUPR4;      /* 0x0110 LSI pin pull-up control register 4           */
    UW  PUPR5;      /* 0x0114 LSI pin pull-up control register 5           */
    UW  PUPR6;      /* 0x0118 LSI pin pull-up control register 6           */
    UW  PUPR7;      /* 0x011C LSI pin pull-up control register 7           */
    UW  reserved_04[16];
    UW  IPSR16;     /* 0x0160 Peripheral function select register 16       */
    UW  reserved_05[55];
    UW  DDR3GPEN;   /* 0x0240 DDR3 General Port IO Enable register         */
    UW  DDR3GPOE;   /* 0x0244 DDR3 General Port Output Enable register     */
    UW  DDR3GPOD;   /* 0x0248 DDR3 General Port Output Data register       */
    UW  DDR3GPID;   /* 0x024C DDR3 General Port InputData register         */
};

#define REG_PFC     (*(volatile struct t_pfc *)(SYSAPB1_BASE+0x060000U))


/**
 * IIC Bus Interface(IIC)
 */
struct t_iic {
    UB  ICDR;       /* 0x000 IIC bus data register              */
    UB  reserved_00[3];
    UB  ICCR;       /* 0x004 IIC bus control register           */
    UB  reserved_01[3];
    UB  ICSR;       /* 0x008 IIC bus status register            */
    UB  reserved_02[3];
    UB  ICIC;       /* 0x00C IIC interruption control register  */
    UB  reserved_03[3];
    UB  ICCL;       /* 0x010 IIC clock control register low     */
    UB  reserved_04[3];
    UB  ICCH;       /* 0x014 IIC clock control register high    */
    UB  reserved_05[3];
    UB  ICTR;       /* 0x018 IIC transmit register              */
    UB  reserved_06[3];
    UB  ICRR;       /* 0x01C IIC receive register               */
    UB  reserved_07[3];
    UB  ICTA;       /* 0x020 IIC transmit monitor register      */
    UB  reserved_08[3];
    UB  ICTB;       /* 0x024 IIC transmit buffer monitor register    */
    UB  reserved_09[3];
    UB  ICTC;       /* 0x028 IIC transmit control register      */
    UB  reserved_10[3];
    UB  ICTD;       /* 0x02C IIC transmit control status monitor register    */
    UB  reserved_11[3];
    UB  ICSF;       /* 0x030 IIC shift register                 */
    UB  reserved_12[31];
    UB  ICIMSK;     /* 0x050 IIC automatic transmission interrupt mask register */
    UB  reserved_13[3];
    UB  ICINT;      /* 0x054 IIC automatic transmission interrupt flag register  */
    UB  reserved_14[3];
    UB  ICACE;      /* 0x058 IIC automatic transmission processing enabling register */
    UB  reserved_15[7];
    UB  ICTMC1;     /* 0x060 IIC automatic transmission timer control register  */
    UB  reserved_16[3];
    UB  ICTMC2;     /* 0x064 IIC automatic transmission timer control register  */
    UB  reserved_17[3];
    UB  ICTMCW;     /* 0x068 IIC automatic transmission wait control register   */
    UB  reserved_18[3];
    UB  ICVCON;     /* 0x06C IIC option enabling register       */
    UB  ICSTART;    /* 0x070 IIC automatic transmission transmit start register */
    UB  reserved_19[15];
    UB  ICATFR;     /* 0x080 IIC automatic transmission transmit control register   */
    UB  reserved_20[3];
    UB  ICATSET1;   /* 0x084 IIC automatic transmission transmit time register  */
    UB  reserved_21[3];
    UB  ICATSET2;   /* 0x088 IIC automatic transmission transmit time register  */
    UB  reserved_22[3];
    UB  ICARSET1;   /* 0x08C IIC automatic transmission reception time register */
    UB  reserved_23[3];
    UB  ICARSET2;   /* 0x090 IIC automatic transmission reception time register */
    UB  reserved_24[111];
    UB  ICATD00;    /* 0x100 IIC automatic transmission transmit data 00    */
    UB  reserved_25[3];
    UB  ICATD01;    /* 0x104 IIC automatic transmission transmit data 01    */
    UB  reserved_26[3];
    UB  ICATD02;    /* 0x108 IIC automatic transmission transmit data 02    */
    UB  reserved_27[3];
    UB  ICATD03;    /* 0x10C IIC automatic transmission transmit data 03    */
    UB  reserved_28[3];
    UB  ICATD04;    /* 0x110 IIC automatic transmission transmit data 04    */
    UB  reserved_29[3];
    UB  ICATD05;    /* 0x114 IIC automatic transmission transmit data 05    */
    UB  reserved_30[3];
    UB  ICATD06;    /* 0x118 IIC automatic transmission transmit data 06    */
    UB  reserved_31[3];
    UB  ICATD07;    /* 0x11C IIC automatic transmission transmit data 07    */
    UB  reserved_32[3];
    UB  ICATD08;    /* 0x120 IIC automatic transmission transmit data 08    */
    UB  reserved_33[3];
    UB  ICATD09;    /* 0x124 IIC automatic transmission transmit data 09    */
    UB  reserved_34[219];
    UB  ICATD10;    /* 0x200 IIC automatic transmission transmit data 10    */
    UB  reserved_35[3];
    UB  ICATD11;    /* 0x204 IIC automatic transmission transmit data 11    */
    UB  reserved_36[3];
    UB  ICATD12;    /* 0x208 IIC automatic transmission transmit data 12    */
    UB  reserved_37[3];
    UB  ICATD13;    /* 0x20C IIC automatic transmission transmit data 13    */
    UB  reserved_38[3];
    UB  ICATD14;    /* 0x210 IIC automatic transmission transmit data 14    */
    UB  reserved_39[3];
    UB  ICATD15;    /* 0x214 IIC automatic transmission transmit data 15    */
    UB  reserved_40[3];
    UB  ICATD16;    /* 0x218 IIC automatic transmission transmit data 16    */
    UB  reserved_41[3];
    UB  ICATD17;    /* 0x21C IIC automatic transmission transmit data 17    */
    UB  reserved_42[3];
    UB  ICATD18;    /* 0x220 IIC automatic transmission transmit data 18    */
    UB  reserved_43[3];
    UB  ICATD19;    /* 0x224 IIC automatic transmission transmit data 19    */
    UB  reserved_44[219];
    UB  ICARD00;    /* 0x300 IIC automatic transmission receipt data 00     */
    UB  reserved_45[3];
    UB  ICARD01;    /* 0x304 IIC automatic transmission receipt data 01     */
    UB  reserved_46[3];
    UB  ICARD02;    /* 0x308 IIC automatic transmission receipt data 02     */
    UB  reserved_47[3];
    UB  ICARD03;    /* 0x30C IIC automatic transmission receipt data 03     */
    UB  reserved_48[3];
    UB  ICARD04;    /* 0x310 IIC automatic transmission receipt data 04     */
    UB  reserved_49[3];
    UB  ICARD05;    /* 0x314 IIC automatic transmission receipt data 05     */
    UB  reserved_50[3];
    UB  ICARD06;    /* 0x318 IIC automatic transmission receipt data 06     */
    UB  reserved_51[3];
    UB  ICARD07;    /* 0x31C IIC automatic transmission receipt data 07     */
    UB  reserved_52[3];
    UB  ICARD08;    /* 0x320 IIC automatic transmission receipt data 08     */
    UB  reserved_53[3];
    UB  ICARD09;    /* 0x324 IIC automatic transmission receipt data 09     */
    UB  reserved_54[219];
    UB  ICARD10;    /* 0x400 IIC automatic transmission receipt data 10     */
    UB  reserved_55[3];
    UB  ICARD11;    /* 0x404 IIC automatic transmission receipt data 11     */
    UB  reserved_56[3];
    UB  ICARD12;    /* 0x408 IIC automatic transmission receipt data 12     */
    UB  reserved_57[3];
    UB  ICARD13;    /* 0x40C IIC automatic transmission receipt data 13     */
    UB  reserved_58[3];
    UB  ICARD14;    /* 0x410 IIC automatic transmission receipt data 14     */
    UB  reserved_59[3];
    UB  ICARD15;    /* 0x414 IIC automatic transmission receipt data 15     */
    UB  reserved_60[3];
    UB  ICARD16;    /* 0x418 IIC automatic transmission receipt data 16     */
    UB  reserved_61[3];
    UB  ICARD17;    /* 0x41C IIC automatic transmission receipt data 17     */
    UB  reserved_62[3];
    UB  ICARD18;    /* 0x420 IIC automatic transmission receipt data 18     */
    UB  reserved_63[3];
    UB  ICARD19;    /* 0x424 IIC automatic transmission receipt data 19     */
};

#define REG_IIC0    (*(volatile struct t_iic *)(SYSAPB2_BASE+0x300000U))
#define REG_IIC1    (*(volatile struct t_iic *)(SYSAPB2_BASE+0x310000U))
#define REG_IIC2    (*(volatile struct t_iic *)(SYSAPB2_BASE+0x320000U))
#define REG_IIC3    (*(volatile struct t_iic *)(SYSAPB1_BASE+0x0B0000U))


/**
 * 16-Bit Timer Pulse Unit (TPU)
 */
struct t_tpu {
    UW  TSTR;   /* 0x0000 Timer start register      */
    UW  reserved_00[3];
    UW  TCR0;   /* 0x0010 Timer control register 0  */
    UW  TMDR0;  /* 0x0014 Timer mode register 0     */
    UW  TIOR0;  /* 0x0018 Timer I/O control register 0      */
    UW  TIER0;  /* 0x001C Timer interrupt enable register 0 */
    UW  TSR0;   /* 0x0020 Timer status register 0   */
    UW  TCNT0;  /* 0x0024 Timer counter 0           */
    UW  TGRA0;  /* 0x0028 Timer general register A0 */
    UW  TGRB0;  /* 0x002C Timer general register B0 */
    UW  TGRC0;  /* 0x0030 Timer general register C0 */
    UW  TGRD0;  /* 0x0034 Timer general register D0 */
    UW  reserved_01[6];
    UW  TCR1;   /* 0x0050 Timer control register 1  */
    UW  TMDR1;  /* 0x0054 Timer mode register 1     */
    UW  TIOR1;  /* 0x0058 Timer I/O control register 1      */
    UW  TIER1;  /* 0x005C Timer interrupt enable register 1 */
    UW  TSR1;   /* 0x0060 Timer status register 1   */
    UW  TCNT1;  /* 0x0064 Timer counter 1           */
    UW  TGRA1;  /* 0x0068 Timer general register A1 */
    UW  TGRB1;  /* 0x006C Timer general register B1 */
    UW  TGRC1;  /* 0x0070 Timer general register C1 */
    UW  TGRD1;  /* 0x0074 Timer general register D1 */
    UW  reserved_02[6];
    UW  TCR2;   /* 0x0090 Timer control register 2  */
    UW  TMDR2;  /* 0x0094 Timer mode register 2     */
    UW  TIOR2;  /* 0x0098 Timer I/O control register 2      */
    UW  TIER2;  /* 0x009C Timer interrupt enable register 2 */
    UW  TSR2;   /* 0x00A0 Timer status register 2   */
    UW  TCNT2;  /* 0x00A4 Timer counter 2           */
    UW  TGRA2;  /* 0x00A8 Timer general register A2 */
    UW  TGRB2;  /* 0x00AC Timer general register B2 */
    UW  TGRC2;  /* 0x00B0 Timer general register C2 */
    UW  TGRD2;  /* 0x00B4 Timer general register D2 */
    UW  reserved_03[6];
    UW  TCR3;   /* 0x00D0 Timer control register 3  */
    UW  TMDR3;  /* 0x00D4 Timer mode register 3     */
    UW  TIOR3;  /* 0x00D8 Timer I/O control register 3      */
    UW  TIER3;  /* 0x00DC Timer interrupt enable register 3 */
    UW  TSR3;   /* 0x00E0 Timer status register 3   */
    UW  TCNT3;  /* 0x00E4 Timer counter 3           */
    UW  TGRA3;  /* 0x00E8 Timer general register A3 */
    UW  TGRB3;  /* 0x00EC Timer general register B3 */
    UW  TGRC3;  /* 0x00F0 Timer general register C3 */
    UW  TGRD3;  /* 0x00F4 Timer general register D3 */
    UW  reserved_04[2];
    UW  TMIR;   /* 0x0100 Motor control setting register    */
    UW  TMRR;   /* 0x0104 Motor deceleration (stop) transition register */
    UW  TMSR;   /* 0x0108 Motor control status register     */
    UW  reserved_05[1];
    UW  TMMPR0; /* 0x0110 Motor operation pattern storing register 0    */
    UW  TMMPR1; /* 0x0114 Motor operation pattern storing register 1    */
    UW  TMSPR0; /* 0x0118 Motor stop pattern storing register 0         */
    UW  TMSPR1; /* 0x011C Motor stop pattern storing register 1         */
    UW  TMOPR;  /* 0x0120 Motor output pattern storing register         */
    UW  reserved_06[3];
    UW  TMASR;  /* 0x0130 Motor acceleration the number of steps register   */
    UW  TMTSR;  /* 0x0134 Motor normal the number of steps register     */
    UW  TMRSR;  /* 0x0138 Motor deceleration the number of steps register   */
    UW  reserved_07[1];
    UW  TMSCR;  /* 0x0140 Motor control sequence counter register       */
    UW  TMTCR;  /* 0x0144 Motor control normal counter register         */
};

#define REG_TPU0    (*(volatile struct t_tpu *)(SYSAPB1_BASE+0x0F0000U))


/**
 *  Compare Match Timer 1 (CMT1)
 */
struct t_cmt1 {
    UW  STR0;       /* 0x0000 Compare match timer start register 0          */
    UW  reserved_00[3];
    UW  CSR0;       /* 0x0010 Compare match timer control/status register 0 */
    UW  CNT0;       /* 0x0014 Compare match timer counter 0                 */
    UW  COR0;       /* 0x0018 Compare match timer constant register 0       */
    UW  reserved_01[1];
    UW  CSRH0;      /* 0x0020 Compare match timer control/status register H0    */
    UW  CNTH0;      /* 0x0024 Compare match timer counter H0                */
    UW  CORH0;      /* 0x0028 Compare match timer constant register H0      */
    UW  reserved_02[5];
    UW  CSRM0;      /* 0x0040 Compare match timer match control/status register 0   */
    UW  CNTM0;      /* 0x0044 Compare match timer match counter 0           */
    UW  reserved_03[46];
    UW  STR1;       /* 0x0100 Compare match timer start register 1          */
    UW  reserved_04[3];
    UW  CSR1;       /* 0x0110 Compare match timer control/status register 1 */
    UW  CNT1;       /* 0x0114 Compare match timer counter 1                 */
    UW  COR1;       /* 0x0118 Compare match timer constant register 1       */
    UW  reserved_05[1];
    UW  CSRH1;      /* 0x0120 Compare match timer control/status register H1    */
    UW  CNTH1;      /* 0x0124 Compare match timer counter H1                */
    UW  CORH1;      /* 0x0128 Compare match timer constant register H1      */
    UW  reserved_06[53];
    UW  STR2;       /* 0x0200 Compare match timer start register 2          */
    UW  reserved_07[3];
    UW  CSR2;       /* 0x0210 Compare match timer control/status register 2 */
    UW  CNT2;       /* 0x0214 Compare match timer counter 2                 */
    UW  COR2;       /* 0x0218 Compare match timer constant register 2       */
    UW  reserved_08[1];
    UW  CSRH2;      /* 0x0220 Compare match timer control/status register H2    */
    UW  CNTH2;      /* 0x0224 Compare match timer counter H2                */
    UW  CORH2;      /* 0x0228 Compare match timer constant register H2      */
    UW  reserved_09[53];
    UW  STR3;       /* 0x0300 Compare match timer start register 3          */
    UW  reserved_10[3];
    UW  CSR3;       /* 0x0310 Compare match timer control/status register 3 */
    UW  CNT3;       /* 0x0314 Compare match timer counter 3                 */
    UW  COR3;       /* 0x0318 Compare match timer constant register 3       */
    UW  reserved_11[1];
    UW  CSRH3;      /* 0x0320 Compare match timer control/status register H3    */
    UW  CNTH3;      /* 0x0324 Compare match timer counter H3                */
    UW  CORH3;      /* 0x0328 Compare match timer constant register H3      */
    UW  reserved_12[1];
    UW  CNT3BK0;    /* 0x0330 Compare match timer counter 3 backup 0        */
    UW  CNT3BK1;    /* 0x0334 Compare match timer counter 3 backup 1        */
    UW  reserved_13[2];
    UW  CSRM3;      /* 0x0340 Compare match timer match control/status register 3   */
    UW  CNTM3;      /* 0x0344 Compare match timer match counter 3           */
    UW  reserved_14[46];
    UW  STR4;       /* 0x0400 Compare match timer start register 4          */
    UW  reserved_15[3];
    UW  CSR4;       /* 0x0410 Compare match timer control/status register 4 */
    UW  CNT4;       /* 0x0414 Compare match timer counter 4                 */
    UW  COR4;       /* 0x0418 Compare match timer constant register 4       */
    UW  reserved_16[1];
    UW  CSRH4;      /* 0x0420 Compare match timer control/status register H4    */
    UW  CNTH4;      /* 0x0424 Compare match timer counter H4                */
    UW  CORH4;      /* 0x0428 Compare match timer constant register H4      */
    UW  reserved_17[53];
    UW  STR5;       /* 0x0500 Compare match timer start register 5          */
    UW  reserved_18[3];
    UW  CSR5;       /* 0x0510 Compare match timer control/status register 5 */
    UW  CNT5;       /* 0x0514 Compare match timer counter 5                 */
    UW  COR5;       /* 0x0518 Compare match timer constant register 5       */
    UW  reserved_19[57];
    UW  STR6;       /* 0x0600 Compare match timer start register 6          */
    UW  reserved_20[3];
    UW  CSR6;       /* 0x0610 Compare match timer control/status register 6 */
    UW  CNT6;       /* 0x0614 Compare match timer counter 6                 */
    UW  COR6;       /* 0x0618 Compare match timer constant register 6       */
    UW  reserved_21[57];
    UW  STR7;       /* 0x0700 Compare match timer start register 7          */
    UW  reserved_22[3];
    UW  CSR7;       /* 0x0710 Compare match timer control/status register 7 */
    UW  CNT7;       /* 0x0714 Compare match timer counter 7                 */
    UW  COR7;       /* 0x0718 Compare match timer constant register 7       */
    UW  reserved_23[569];
    UW  CLKE;       /* 0x1000 CLK enable register                           */
};

#define REG_CMT1    (*(volatile struct t_cmt1 *)(SYSAPB1_BASE+0x130000U))


/**
 * Clock Pulse Generator (CPG)
 */
struct t_cpg {
    UW  reserved_00[1];
    UW  FRQCRB;         /* 0x0004 Frequency control register B          */
    UW  reserved_01[20];
    UW  DVFSCR0;        /* 0x0058 DVFS control register 0               */
    UW  DVFSCR1;        /* 0x005C DVFS control register 1               */
    UW  reserved_02[5];
    UW  SDCKCR;         /* 0x0074 SDHI clock frequency control register     */
    UW  SD2CKCR;        /* 0x0078 SDHI2 clock frequency control register    */
    UW  reserved_03[14];
    UW  RGXCR;          /* 0x00B4 RGX control register                  */
    UW  reserved_04[6];
    UW  PLLECR;         /* 0x00D0 PLL Enable Control Register           */
    UW  reserved_05[1];
    UW  PLL0CR;         /* 0x00D8 PLL0 control register                 */
    UW  reserved_06[1];
    UW  FRQCRC;         /* 0x00E0 Frequency control register C          */
    UW  reserved_07[84];
    UW  GPUCKCR;        /* 0x0234 GPU clock frequency control register  */
    UW  reserved_08[2];
    UW  MMC0CKCR;       /* 0x0240 MMC0 clock frequency control register */
    UW  MMC1CKCR;       /* 0x0244 MMC1 clock frequency control register */
    UW  reserved_09[9];
    UW  SD3CKCR;        /* 0x026C SDHI3 clock frequency control register    */
    UW  RCANCKCR;       /* 0x0270 RCAN clock frequency control register     */
};

#define REG_CPG     (*(volatile struct t_cpg *)(SYSAPB1_BASE+0x150000U))


/**
 * Module Standby, Software Reset (MSTPRST)
 */
struct t_cpg_msr {
    UW  reserved_00[12];
    UW  MSTPSR0;        /* 0x0030 Module stop status register 0 */
    UW  reserved_01[1];
    UW  MSTPSR1;        /* 0x0038 Module stop status register 1 */
    UW  MSTPSR5;        /* 0x003C Module stop status register 5 */
    UW  MSTPSR2;        /* 0x0040 Module stop status register 2 */
    UW  reserved_02[1];
    UW  MSTPSR3;        /* 0x0048 Module stop status register 3 */
    UW  MSTPSR4;        /* 0x004C Module stop status register 4 */
    UW  reserved_03[20];
    UW  SRCR0;          /* 0x00A0 Software reset register 0     */
    UW  reserved_04[1];
    UW  SRCR1;          /* 0x00A8 Software reset register 1     */
    UW  reserved_05[1];
    UW  SRCR2;          /* 0x00B0 Software reset register 2     */
    UW  reserved_06[1];
    UW  SRCR3;          /* 0x00B8 Software reset register 3     */
    UW  SRCR4;          /* 0x00BC Software reset register 4     */
    UW  reserved_07[1];
    UW  SRCR5;          /* 0x00C4 Software reset register 5     */
    UW  reserved_08[26];
    UW  SMSTPCR0;       /* 0x0130 System module stop control register 0     */
    UW  SMSTPCR1;       /* 0x0134 System module stop control register 1     */
    UW  SMSTPCR2;       /* 0x0138 System module stop control register 2     */
    UW  SMSTPCR3;       /* 0x013C System module stop control register 3     */
    UW  SMSTPCR4;       /* 0x0140 System module stop control register 4     */
    UW  SMSTPCR5;       /* 0x0144 System module stop control register 5     */
    UW  SMSTPCR6;       /* 0x0148 System module stop control register 6     */
    UW  SMSTPCR7;       /* 0x014C System module stop control register 7     */
    UW  reserved_09[28];
    UW  MSTPSR6;        /* 0x01C0 Module stop status register 6 */
    UW  MSTPSR7;        /* 0x01C4 Module stop status register 7 */
    UW  SRCR6;          /* 0x01C8 Software reset register 6     */
    UW  SRCR7;          /* 0x01CC Software reset register 7     */
    UW  reserved_10[468];
    UW  SRCR8;          /* 0x0920 Software reset register 8     */
    UW  SRCR9;          /* 0x0924 Software reset register 9     */
    UW  SRCR10;         /* 0x0928 Software reset register 10    */
    UW  SRCR11;         /* 0x092C Software reset register 11    */
    UW  reserved_11[4];
    UW  SRSTCLR0;       /* 0x0940 Software reset clear register 0   */
    UW  SRSTCLR1;       /* 0x0944 Software reset clear register 1   */
    UW  SRSTCLR2;       /* 0x0948 Software reset clear register 2   */
    UW  SRSTCLR3;       /* 0x094C Software reset clear register 3   */
    UW  SRSTCLR4;       /* 0x0950 Software reset clear register 4   */
    UW  SRSTCLR5;       /* 0x0954 Software reset clear register 5   */
    UW  SRSTCLR6;       /* 0x0958 Software reset clear register 6   */
    UW  SRSTCLR7;       /* 0x095C Software reset clear register 7   */
    UW  SRSTCLR8;       /* 0x0960 Software reset clear register 8   */
    UW  SRSTCLR9;       /* 0x0964 Software reset clear register 9   */
    UW  SRSTCLR10;      /* 0x0968 Software reset clear register 10  */
    UW  SRSTCLR11;      /* 0x096C Software reset clear register 11  */
    UW  reserved_12[8];
    UW  SMSTPCR8;       /* 0x0990 System module stop control register 8     */
    UW  SMSTPCR9;       /* 0x0994 System module stop control register 9     */
    UW  SMSTPCR10;      /* 0x0998 System module stop control register 10    */
    UW  SMSTPCR11;      /* 0x099C System module stop control register 11    */
    UW  MSTPSR8;        /* 0x09A0 Module stop status register 8     */
    UW  MSTPSR9;        /* 0x09A4 Module stop status register 9     */
    UW  MSTPSR10;       /* 0x09A8 Module stop status register 10    */
    UW  MSTPSR11;       /* 0x09AC Module stop status register 11    */
};

#define REG_CPG_MSR     (*(volatile struct t_cpg_msr *)(SYSAPB1_BASE+0x150000U))


/**
 * Advanced Power Management Unit for AP-System Core (APMU)
 */
#if (defined(CFG_G1M) || defined(CFG_G1N) || defined(CFG_G1E))
/* Cortex-15 access to this address */
#define REG_APMU_CA7_BASE   (SYSAPB1_BASE+0x151010U)
#define REG_APMU_CA15_BASE  (SYSAPB1_BASE+0x152180U)

struct t_apmu_ca7 {
    UW  WUPCR;      /* 0x1010 CA7 CPU wake up control register         */
    UW  reserved_00[11];
    UW  PSTR;       /* 0x1040 CA7 power status register                */
    UW  reserved_01[47];
    UW  CPU0CR;     /* 0x1100 CA7 CPU0 power status control register   */
    UW  reserved_02[3];
    UW  CPU1CR;     /* 0x1110 CA7 CPU1 power status control register   */
    UW  reserved_03[3];
    UW  CPU2CR;     /* 0x1120 CA7 CPU2 power status control register   */
    UW  reserved_04[3];
    UW  CPU3CR;     /* 0x1130 CA7 CPU3 power status control register   */
    UW  reserved_05[20];
    UW  DBGRCR;     /* 0x1180 CA7 debug resource reset control register*/
    UW  CPUCMCR;    /* 0x1184 CA7 common power control register        */
};

struct t_apmu_ca15 {
    UW  DBGRCR;      /* 0x2180 CA15 debug resource reset control register */
    UW  reserved_00[1955];
    UW  WUPCR;      /* 0x4010 CA15 CPU wake up control register          */
    UW  reserved_01[11];
    UW  PSTR;       /* 0x4040 CA15 power status register                 */
    UW  reserved_02[47];
    UW  CPU0CR;     /* 0x4100 CA15 CPU0 power status control register    */
    UW  reserved_03[3];
    UW  CPU1CR;     /* 0x4110 CA15 CPU1 power status control register    */
    UW  reserved_04[3];
    UW  CPU2CR;     /* 0x4120 CA15 CPU2 power status control register    */
    UW  reserved_05[3];
    UW  CPU3CR;     /* 0x4130 CA15 CPU3 power status control register    */
    UW  reserved_06[20];
    UW  CPUCMCR;    /* 0x4184 CA15 common power control register         */
};

#define REG_APMU_CA7    (*(volatile struct t_apmu_ca7 *)(REG_APMU_CA7_BASE))
#define REG_APMU_CA15   (*(volatile struct t_apmu_ca15 *)(REG_APMU_CA15_BASE))

#else
/* Cortex-A7 access to this address */
#define REG_APMU_CA7_BASE   (SYSAPB1_BASE+0x151180U)
#define REG_APMU_CA15_BASE  (SYSAPB1_BASE+0x152010U)

struct t_apmu_ca7 {
    UW  DBGRCR;     /* 0x1180 CA7 debug resource reset control register */
    UW  reserved_00[2979];
    UW  WUPCR;      /* 0x4010 CA7 CPU wake up control register          */
    UW  reserved_01[11];
    UW  PSTR;       /* 0x4040 CA7 power status register                 */
    UW  reserved_02[47];
    UW  CPU0CR;     /* 0x4100 CA7 CPU0 power status control register    */
    UW  reserved_03[3];
    UW  CPU1CR;     /* 0x4110 CA7 CPU1 power status control register    */
    UW  reserved_04[3];
    UW  CPU2CR;     /* 0x4120 CA7 CPU2 power status control register    */
    UW  reserved_05[3];
    UW  CPU3CR;     /* 0x4130 CA7 CPU3 power status control register    */
    UW  reserved_06[20];
    UW  CPUCMCR;    /* 0x4184 CA7 common power control register         */
};

struct t_apmu_ca15 {
    UW  WUPCR;      /* 0x2010 CA15 CPU wake up control register         */
    UW  reserved_00[11];
    UW  PSTR;       /* 0x2040 CA15 power status register                */
    UW  reserved_01[47];
    UW  CPU0CR;     /* 0x2100 CA15 CPU0 power status control register   */
    UW  reserved_02[3];
    UW  CPU1CR;     /* 0x2110 CA15 CPU1 power status control register   */
    UW  reserved_03[3];
    UW  CPU2CR;     /* 0x2120 CA15 CPU2 power status control register   */
    UW  reserved_04[3];
    UW  CPU3CR;     /* 0x2130 CA15 CPU3 power status control register   */
    UW  reserved_05[20];
    UW  DBGRCR;     /* 0x2180 CA15 debug resource reset control register*/
    UW  CPUCMCR;    /* 0x2184 CA15 common power control register        */
};

#define REG_APMU_CA7    (*(volatile struct t_apmu_ca7 *)(REG_APMU_CA7_BASE))
#define REG_APMU_CA15   (*(volatile struct t_apmu_ca15 *)(REG_APMU_CA15_BASE))

#endif


/**
 * Reset (RST)
 */
struct t_rst {
    UW  reserved_00[4];
    UW  SBAR;       /* 0x0010 SYS boot address register     */
    UW  SBAR2;      /* 0x0014 SYS boot address register 2   */
    UW  reserved_01[2];
    UW  CA15BAR;    /* 0x0020 CA15 boot address register    */
    UW  CA15BAR2;   /* 0x0024 CA15 boot address register 2  */
    UW  reserved_02[2];
    UW  CA7BAR;     /* 0x0030 CA7 boot address register     */
    UW  CA7BAR2;    /* 0x0034 CA7 boot address register 2   */
    UW  reserved_03[2];
    UW  CA15RESCNT; /* 0x0040 CA15 reset control register   */
    UW  CA7RESCNT;  /* 0x0044 CA7 reset control register    */
    UW  reserved_04[3];
    UW  WDTRSTCR;   /* 0x0054 Watchdog timer reset control register */
    UW  RSTOUTCR;   /* 0x0058 PRESETOUT control register    */
    UW  reserved_05[1];
    UW  MODEMR;     /* 0x0060 Mode monitoring register    */
};

#define REG_RST     (*(volatile struct t_rst *)(SYSAPB1_BASE+0x160000U))


/**
 * System Controller (SYSC)
 */
struct t_sysc {
    UW  SYSCSR;         /* 0x0000 SYSC Status Register              */
    UW  SYSCISR;        /* 0x0004 Interrupt Status Register         */
    UW  SYSCISCR;       /* 0x0008 Interrupt Status Clear Register   */
    UW  SYSCIER;        /* 0x000C Interrupt Enable Register         */
    UW  SYSCIMR;        /* 0x0010 Interrupt Mask Register           */
    UW  WUPMSKCA15;     /* 0x0014 CA15 Wake Up Mask Register        */
    UW  WUPMSKCA7;      /* 0x0018 CA7 Wake Up Mask Register         */
    UW  reserved_00[1];
    UW  SYSCEERSR;      /* 0x0020 External Event Request Status Register            */
    UW  SYSCEERSCR;     /* 0x0024 External Event Request Status Clear Register      */
    UW  SYSCEERSER;     /* 0x0028 External Event Request Status Enable Register     */
    UW  SYSCEERSR2;     /* 0x002C External Event Request Status Register 2          */
    UW  SYSCEERSCR2;    /* 0x0030 External Event Request Status Clear register 2    */
    UW  SYSCEERSER2;    /* 0x0034 External Event Request Status Enable Register 2   */
    UW  reserved_01[2];
    UW  PWRSR0;         /* 0x0040 Power status register 0           */
    UW  reserved_02[1];
    UW  PWROFFSR0;      /* 0x0048 Power shutoff status register 0   */
    UW  reserved_03[1];
    UW  PWRONSR0;       /* 0x0050 Power resume status register 0    */
    UW  PWRER0;         /* 0x0054 Power shutoff/resume error register 0 */
    UW  reserved_04[26];
    UW  PWRSR2;         /* 0x00C0 Power status register 2           */
    UW  PWROFFCR2;      /* 0x00C4 Power shutoff control register 2  */
    UW  PWROFFSR2;      /* 0x00C8 Power shutoff status register 2   */
    UW  PWRONCR2;       /* 0x00CC Power resume control register 2   */
    UW  PWRONSR2;       /* 0x00D0 Power resume status register 2    */
    UW  PWRER2;         /* 0x00D4 Power shutoff/resume error register 2 */
    UW  reserved_05[10];
    UW  PWRSR3;         /* 0x0100 Power status register 3           */
    UW  PWROFFCR3;      /* 0x0104 Power shutoff control register 3  */
    UW  PWROFFSR3;      /* 0x0108 Power shutoff status register 3   */
    UW  PWRONCR3;       /* 0x010C Power resume control register 3   */
    UW  PWRONSR3;       /* 0x0110 Power resume status register 3    */
    UW  PWRER3;         /* 0x0114 Power shutoff/resume error register 3 */
    UW  reserved_06[26];
    UW  PWRSR5;         /* 0x0180 Power status register 5           */
    UW  PWROFFCR5;      /* 0x0184 Power shutoff control register 5  */
    UW  PWROFFSR5;      /* 0x0188 Power shutoff status register 5   */
    UW  PWRONCR5;       /* 0x018C Power resume control register 5   */
    UW  PWRONSR5;       /* 0x0190 Power resume status register 5    */
    UW  PWRER5;         /* 0x0194 Power shutoff/resume error register 5 */
    UW  reserved_07[10];
    UW  PWRSR6;         /* 0x01C0 Power status register 6           */
    UW  reserved_08[1];
    UW  PWROFFSR6;      /* 0x01C8 Power shutoff status register 6   */
    UW  reserved_09[1];
    UW  PWRONSR6;       /* 0x01D0 Power resume status register 6    */
    UW  PWRER6;         /* 0x01D4 Power shutoff/resume error register 6 */
};

#define REG_SYSC    (*(volatile struct t_sysc *)(SYSAPB1_BASE+0x180000U))


/**
 * External Interrupt Controller (IRQC)
 */
struct t_irqc {
    /* 0x0000 IRQC / IRQC event detector block */
    UW  INTREQ_STS0;    /* 0x000 Interrupt request status register 0    */
    UW  INTEN_STS0;     /* 0x004 Interrupt enable status register 0     */
    UW  INTEN_SET0;     /* 0x008 Interrupt enable set register 0        */
    UW  reserved_00[61];
    UW  DETECT_STATUS;  /* 0x100 IRQn detect status register            */
    UW  MONITOR;        /* 0x104 IRQn signal level monitor register     */
    UW  HLVL_STS;       /* 0x108 IRQn high level detect status register */
    UW  LLVL_STS;       /* 0x10C IRQn low level detect status register  */
    UW  S_R_EDGE_STS;   /* 0x110 IRQn sync rising edge detect status register   */
    UW  S_F_EDGE_STS;   /* 0x114 IRQn sync falling edge detect status register  */
    UW  A_R_EDGE_STS;   /* 0x118 IRQn async rising edge detect status register  */
    UW  A_F_EDGE_STS;   /* 0x11C IRQn async falling edge detect status register */
    UW  CHTEN_STS;      /* 0x120 Chattering reduction status register   */
    UW  reserved_01[23];
    UW  CONFIG_00;      /* 0x180 IRQ0 configuration register            */
    UW  CONFIG_01;      /* 0x184 IRQ1 configuration register            */
    UW  CONFIG_02;      /* 0x188 IRQ2 configuration register            */
    UW  CONFIG_03;      /* 0x18C IRQ3 configuration register            */
    UW  CONFIG_04;      /* 0x190 IRQ4 configuration register            */
    UW  CONFIG_05;      /* 0x194 IRQ5 configuration register            */
    UW  CONFIG_06;      /* 0x198 IRQ6 configuration register            */
    UW  CONFIG_07;      /* 0x19C IRQ7 configuration register            */
    UW  CONFIG_08;      /* 0x1A0 IRQ8 configuration register            */
    UW  CONFIG_09;      /* 0x1A4 IRQ9 configuration register            */

    /* 0x0400 NMI event detector block */
    UW  reserved_02[150];
    UW  NMIREQ_STS0;    /* 0x400 NMI request status register 0          */
    UW  NMIEN_STS0;     /* 0x404 NMI enable status register 0           */
    UW  NMIEN_SET0;     /* 0x408 NMI enable set register 0              */
    UW  reserved_03[63];
    UW  HLVL_STS_NMI;   /* 0x508 NMI high level detect status register  */
    UW  LLVL_STS_NMI;   /* 0x50C NMI low level detect status register   */
    UW  S_R_EDGE_STS_NMI;   /* 0x510 NMI sync rising edge detect status register    */
    UW  S_F_EDGE_STS_NMI;   /* 0x514 NMI sync falling edge detect status register   */
    UW  A_R_EDGE_STS_NMI;   /* 0x518 NMI async rising edge detect status register   */
    UW  A_F_EDGE_STS_NMI;   /* 0x51C NMI async falling edge detect status register  */
    UW  CHTEN_STS_NMI;  /* 0x520 Chattering reduction status register   */
    UW  reserved_04[7];
    UW  DEB_SET_NMI;    /* 0x540 NMI debounce setting register          */
    UW  reserved_05[15];
    UW  CONFIG0_NMI;    /* 0x580 NMI configuration 0 register           */
    UW  CONFIG1_NMI;    /* 0x584 NMI configuration 1 register           */
    UW  CONFIG2_NMI;    /* 0x588 NMI configuration 2 register           */
    UW  CONFIG3_NMI;    /* 0x58C NMI configuration 3 register           */
    UW  CONFIG4_NMI;    /* 0x590 NMI configuration 4 register           */
    UW  CONFIG5_NMI;    /* 0x594 NMI configuration 5 register           */
    UW  CONFIG6_NMI;    /* 0x598 NMI configuration 6 register           */
    UW  CONFIG7_NMI;    /* 0x59C NMI configuration 7 register           */

    /* 0x0A00 NMI mask lock block */
    UW  reserved_06[280];
    UW  NMI_LCK;        /* 0xA00 NMI mask lock set register             */
    UW  NMI_LCKCODE;    /* 0xA04 NMI lock code register                 */
    UW  NMI_DBG;        /* 0xA08 NMI debug control enable register      */
    UW  reserved_07[63];
    UW  NMI_DBGCODE;    /* 0xA08 NMI debug code register                */
};

#define REG_IRQC        (*(volatile struct t_irqc *)(SYSAPB1_BASE+0x1C0000U))


/**
 * Timer Unit (TMU)
 */
struct t_tmu0 {
    UW  reserved_00[1];
    UB  TSTR0;  /* 0x0004 Timer start register 0    */
    UB  reserved_01[3];
    UW  TCOR0;  /* 0x0008 Timer constant register 0 */
    UW  TCNT0;  /* 0x000C Timer counter 0           */
    UH  TCR0;   /* 0x0010 Timer control register 0  */
    UH  reserved_02[1];
    UW  TCOR1;  /* 0x0014 Timer constant register 1 */
    UW  TCNT1;  /* 0x0018 Timer counter 1           */
    UH  TCR1;   /* 0x001C Timer control register 1  */
    UH  reserved_03[1];
    UW  TCOR2;  /* 0x0020 Timer constant register 2 */
    UW  TCNT2;  /* 0x0024 Timer counter 2           */
    UH  TCR2;   /* 0x0028 Timer control register 2  */
};

struct t_tmu1 {
    UW  reserved_00[1];
    UB  TSTR1;  /* 0x0004 Timer start register 0    */
    UB  reserved_01[3];
    UW  TCOR3;  /* 0x0008 Timer constant register 0 */
    UW  TCNT3;  /* 0x000C Timer counter 0           */
    UH  TCR3;   /* 0x0010 Timer control register 0  */
    UH  reserved_02[1];
    UW  TCOR4;  /* 0x0014 Timer constant register 1 */
    UW  TCNT4;  /* 0x0018 Timer counter 1           */
    UH  TCR4;   /* 0x001C Timer control register 1  */
    UH  reserved_03[1];
    UW  TCOR5;  /* 0x0020 Timer constant register 2 */
    UW  TCNT5;  /* 0x0024 Timer counter 2           */
    UH  TCR5;   /* 0x0028 Timer control register 2  */
};

struct t_tmu2 {
    UW  reserved_00[1];
    UB  TSTR2;  /* 0x0004 Timer start register 0    */
    UB  reserved_01[3];
    UW  TCOR6;  /* 0x0008 Timer constant register 0 */
    UW  TCNT6;  /* 0x000C Timer counter 0           */
    UH  TCR6;   /* 0x0010 Timer control register 0  */
    UH  reserved_02[1];
    UW  TCOR7;  /* 0x0014 Timer constant register 1 */
    UW  TCNT7;  /* 0x0018 Timer counter 1           */
    UH  TCR7;   /* 0x001C Timer control register 1  */
    UH  reserved_03[1];
    UW  TCOR8;  /* 0x0020 Timer constant register 2 */
    UW  TCNT8;  /* 0x0024 Timer counter 2           */
    UH  TCR8;   /* 0x0028 Timer control register 2  */
};

struct t_tmu3 {
    UW  reserved_00[1];
    UB  TSTR3;  /* 0x0004 Timer start register 0    */
    UB  reserved_01[3];
    UW  TCOR9; /* 0x0008 Timer constant register 0 */
    UW  TCNT9; /* 0x000C Timer counter 0           */
    UH  TCR9;  /* 0x0010 Timer control register 0  */
    UH  reserved_02[1];
    UW  TCOR10; /* 0x0014 Timer constant register 1 */
    UW  TCNT10; /* 0x0018 Timer counter 1           */
    UH  TCR10;  /* 0x001C Timer control register 1  */
    UH  reserved_03[1];
    UW  TCOR11; /* 0x0020 Timer constant register 2 */
    UW  TCNT11; /* 0x0024 Timer counter 2           */
    UH  TCR11;  /* 0x0028 Timer control register 2  */
};

#define REG_TMU0    (*(volatile struct t_tmu0 *)(SYSAPB1_BASE+0x1E0000U))
#define REG_TMU1    (*(volatile struct t_tmu1 *)(RTAPB3_BASE+0x00000U))
#define REG_TMU2    (*(volatile struct t_tmu2 *)(RTAPB3_BASE+0x10000U))
#define REG_TMU3    (*(volatile struct t_tmu3 *)(RTAPB3_BASE+0x20000U))


/**
 * Serial Communications Interface with FIFO A (SCIFA)
 */
struct t_scifa {
    UH  SMR;    /* 0x0000 Serial mode register      16  */
    UB  reserved_00[2];
    UB  BRR;    /* 0x0004 Bit rate register         8   */
    UB  reserved_01[3];
    UH  SCR;    /* 0x0008 Serial control register   16  */
    UB  reserved_02[2];
    UB  TDSR;   /* 0x000C Transmit data stop register 8 */
    UB  reserved_03[3];
    UH  FER;    /* 0x0010 FIFO error count register 16  */
    UB  reserved_04[2];
    UH  SSR;    /* 0x0014 Serial status register 16     */
    UB  reserved_05[2];
    UH  FCR;    /* 0x0018 FIFO control register 16      */
    UB  reserved_06[2];
    UH  FDR;    /* 0x001C FIFO data count register 16   */
    UB  reserved_07[2];
    UB  FTDR;   /* 0x0020 Transmit FIFO data register 8 */
    UB  reserved_08[3];
    UB  FRDR;   /* 0x0024 Receive FIFO data register 8  */
    UB  reserved_09[3];
    UB  reserved_10[4]; /* 0x28 */
    UB  reserved_11[4]; /* 0x2C */
    UH  PCR;    /* 0x0030 Serial port control register 16 */
    UB  reserved_12[2];
    UH  PDR;    /* 0x0034 Serial port data register 16  */
};

#define REG_SCIFA0  (*(volatile struct t_scifa *)(SYSAPB3_BASE+0x040000U))
#define REG_SCIFA1  (*(volatile struct t_scifa *)(SYSAPB3_BASE+0x050000U))
#define REG_SCIFA2  (*(volatile struct t_scifa *)(SYSAPB3_BASE+0x060000U))
/* RZ/G1H not supported */
#define REG_SCIFA3  (*(volatile struct t_scifa *)(SYSAPB3_BASE+0x070000U))
#define REG_SCIFA4  (*(volatile struct t_scifa *)(SYSAPB3_BASE+0x078000U))
#define REG_SCIFA5  (*(volatile struct t_scifa *)(SYSAPB3_BASE+0x080000U))


/**
 * Serial Communications Interface with FIFO (SCIF)
 */
struct t_scif {
    UH  SMR;    /* 0x00 Serial mode register 16         */
    UB  reserved_00[2];
    UB  BRR;    /* 0x04 Bit rate register 8             */
    UB  reserved_01[3];
    UH  SCR;    /* 0x08 Serial control register 16      */
    UB  reserved_02[2];
    UB  FTDR;   /* 0x0C Transmit FIFO data register 8   */
    UB  reserved_03[3];
    UH  FSR;    /* 0x10 Serial status register 16       */
    UB  reserved_04[2];
    UB  FRDR;   /* 0x14 Receive FIFO data register 8    */
    UB  reserved_05[3];
    UH  FCR;    /* 0x18 FIFO control register 16        */
    UB  reserved_06[2];
    UH  FDR;    /* 0x1C FIFO data count register 16     */
    UB  reserved_07[2];
    UH  SPTR;   /* 0x20 Serial port register 16         */
    UB  reserved_08[2];
    UH  LSR;    /* 0x24 Line status register 16         */
    UB  reserved_09[10];
    UH  DL;     /* 0x30 Frequency division register 16  */
    UB  reserved_10[2];
    UH  CKS;    /* 0x34 Clock Select register 16        */
};

#define REG_SCIF0  (*(volatile struct t_scif *)(PERI_65MHZ_BASE+0x060000U))
#define REG_SCIF1  (*(volatile struct t_scif *)(PERI_65MHZ_BASE+0x068000U))
/* REG_SCIF2 register is defined in SCIF driver. */
#define REG_SCIF3  (*(volatile struct t_scif *)(PERI_65MHZ_BASE+0x0A8000U))
#define REG_SCIF4  (*(volatile struct t_scif *)(PERI_65MHZ_BASE+0x0E0000U))
#define REG_SCIF5  (*(volatile struct t_scif *)(PERI_65MHZ_BASE+0x0E8000U))

/**
 * Ethernet MAC Controller (Ether)
 */
struct t_ether {
    UW  reserved_00[128];
    UW  CXR0;   /* 0x0200 HDMAC operating mode setting register         */
    UW  reserved_01[1];
    UW  CXR1;   /* 0x0208 Transmit activation register                  */
    UW  reserved_02[1];
    UW  CXR2;   /* 0x0210 Receive activation register                   */
    UW  reserved_03[1];
    UW  CXR3;   /* 0x0218 Transmit descriptor start address setting register    */
    UW  reserved_04[1];
    UW  CXR4;   /* 0x0220 Receive descriptor start address setting register     */
    UW  reserved_05[1];
    UW  CXR5;   /* 0x0228 Status register                               */
    UW  reserved_06[1];
    UW  CXR6;   /* 0x0230 Interrupt mask setting register               */
    UW  reserved_07[1];
    UW  CXR7;   /* 0x0238 Error mask setting register                   */
    UW  reserved_08[1];
    UW  CXR8;   /* 0x0240 Discarded frame counter register              */
    UW  reserved_09[1];
    UW  CXR9;   /* 0x0248 Transmit FIFO threshold setting register      */
    UW  reserved_10[1];
    UW  CXR10;  /* 0x0250 External FIFO depth setting register          */
    UW  reserved_11[1];
    UW  CXR11;  /* 0x0258 Receive activation reset method setting register      */
    UW  reserved_12[2];
    UW  CXR13;  /* 0x0264 Transmit FIFO underflow counter register      */
    UW  CXR14;  /* 0x0268 Receive FIFO overflow counter register        */
    UW  CXR15;  /* 0x026C RMII mode register                            */
    UW  CXR16;  /* 0x0270 Receive FIFO busy transmit threshold setting register */
    UW  reserved_13[2];
    UW  CXR18;  /* 0x027C Transmit interrupt mode setting register      */
    UW  reserved_14[32];
    UW  CXR20;  /* 0x0300 feLic operating mode setting register         */
    UW  reserved_15[1];
    UW  CXR2A;  /* 0x0308 Long frame length check value setting register*/
    UW  reserved_16[1];
    UW  CXR21;  /* 0x0310 Status register                               */
    UW  reserved_17[1];
    UW  CXR22;  /* 0x0318 interrupt mask setting register               */
    UW  reserved_18[1];
    UW  CXR23;  /* 0x0320 MII control register                          */
    UW  reserved_19[1];
    UW  CXR2B;  /* 0x0328 PHY status register                           */
    UW  reserved_20[5];
    UW  CXR30;  /* 0x0340 Random number generating counter upper limit setting register */
    UW  reserved_21[3];
    UW  CXR70;  /* 0x0350 IPG counter setting register                  */
    UW  CXR71;  /* 0x0354 Automatic PAUSE parameter setting register    */
    UW  CXR72;  /* 0x0358 Manual PAUSE parameter setting register       */
    UW  reserved_22[1];
    UW  CXR80;  /* 0x0360 Receive PAUSE frame counter register          */
    UW  CXR81;  /* 0x0364 PAUSE frame retransmit count setting register */
    UW  CXR82;  /* 0x0368 PAUSE frame retransmit counter register       */
    UW  reserved_23[21];
    UW  CXR24;  /* 0x03C0 MAC address high register                     */
    UW  reserved_24[1];
    UW  CXR25;  /* 0x03C8 MAC address low register                      */
    UW  reserved_25[1];
    UW  CXR40;  /* 0x03D0 TINT1 count register                          */
    UW  CXR41;  /* 0x03D4 TINT2 count register                          */
    UW  CXR42;  /* 0x03D8 TINT3 count register                          */
    UW  CXR43;  /* 0x03DC TINT4 count register                          */
    UW  reserved_26[1];
    UW  CXR50;  /* 0x03E4 RINT1 count register                          */
    UW  CXR51;  /* 0x03E8 RINT2 count register                          */
    UW  CXR52;  /* 0x03EC RINT3 count register                          */
    UW  CXR53;  /* 0x03F0 RINT4 count register                          */
    UW  CXR54;  /* 0x03F4 RINT5 count register                          */
    UW  CXR55;  /* 0x03F8 RINT8 count register                          */
};

#define REG_ETHER   (*(volatile struct t_ether *)(ETHERMAC_BASE))

/**
 * EthernetAVB
 */
struct t_ether_avb {
    UW  CCC;              /* 0x0000 AVB-DMAC mode register                        */
    UW  DBAT;             /* 0x0004 Descriptor base address table register        */
    UW  DLR;              /* 0x0008 Descriptor base address load request register */
    UW  CSR;              /* 0x000C AVB-DMAC status register                      */
    UW  CDAR[22];         /* 0x0010 Current descriptor address register           */
    UW  reserved_0068[8]; /* 0x0068 - 0x0087                                      */
    UW  ESR;              /* 0x0088 Error status register                         */
    UW  reserved_008C[1]; /* 0x008C */
    UW  RCR;              /* 0x0090 Receive configuration register                */
    UW  RQC[5];           /* 0x0094 Receive queue configuration register          */
    UW  reserved_00A8[2]; /* 0x00A8 - 0x00AF */
    UW  RPC;              /* 0x00B0 Receive padding configuration Register        */
    UW  reserved_00B4[2]; /* 0x00B4 - 0x00BB */
    UW  UFCW;             /* 0x00BC Unread frame counter warning level register   */
    UW  UFCS;             /* 0x00C0 Unread frame counter stop level register      */
    UW  UFCV[5];          /* 0x00C4 Unread frame counter register                 */
    UW  reserved_00D8[2]; /* 0x00D8 - 0x00DF */
    UW  UFCD[5];          /* 0x00E0 Unread frame counter decrement register       */
    UW  reserved_00F4[2]; /* 0x00F4 - 0x00FB */
    UW  SFO;              /* 0x00FC Separation filter offset register             */
    UW  SFP[32];          /* 0x0100 Separation filter pattern register            */
    UW  reserved_0180[16];/* 0x0180 - 01BF */
    UW  SFM[2];           /* 0x01C0 Separation filter mask register               */
    UW  reserved_01C8[78];/* 0x01C8 - 0x02FF */
    UW  TGC;              /* 0x0300 Transmit configuration register               */
    UW  TCCR;             /* 0x0304 Transmit configuration control register       */
    UW  TSR;              /* 0x0308 Transmit status register                      */
    UW  reserved_030C[1]; /* 0x030C */
    UW  TFA0;             /* 0x0310 Time stamp FIFO access register 0             */
    UW  TFA1;             /* 0x0314 Time stamp FIFO access register 1             */
    UW  TFA2;             /* 0x0318 Time stamp FIFO access register 2             */
    UW  reserved_031C[1]; /* 0x031C */
    UW  CIVR[2];          /* 0x0320 CBS increment value register                  */
    UW  CDVR[2];          /* 0x0328 CBS decrement value register                  */
    UW  CUL[2];           /* 0x0330 CBS upper limit register                      */
    UW  CLL[2];           /* 0x0338 CBS lower limit register                      */
    UW  reserved_0340[4]; /* 0x0340 - 0x34F */
    UW  DIC;              /* 0x0350 Descriptor interrupt control register         */
    UW  DIS;              /* 0x0354 Descriptor interrupt status register          */
    UW  EIC;              /* 0x0358 Error interrupt control register              */
    UW  EIS;              /* 0x035C Error interrupt status register               */
    UW  RIC0;             /* 0x0360 Receive interrupt control register 0          */
    UW  RIS0;             /* 0x0364 Receive interrupt status register 0           */
    UW  RIC1;             /* 0x0368 Receive interrupt control register 1          */
    UW  RIS1;             /* 0x036C Receive interrupt status register 1           */
    UW  RIC2;             /* 0x0370 Receive interrupt control register 2          */
    UW  RIS2;             /* 0x0374 Receive interrupt status register 2           */
    UW  TIC;              /* 0x0378 Transmit interrupt control register           */
    UW  TIS;              /* 0x037C Transmit interrupt status register            */
    UW  ISS;              /* 0x0380 interrupt summary status register             */
    UW  reserved_0384[3]; /* 0x0384 - 0x038F */
    UW  GCCR;             /* 0x0390 gPTP configuration control register           */
    UW  GMTT;             /* 0x0394 gPTP maximum transit time register            */
    UW  GPTC;             /* 0x0398 gPTP presentation time comparison             */
    UW  GTI;              /* 0x039C gPTP timer increment register                 */
    UW  GTO[3];           /* 0x03A0 gPTP timer offset register                    */
    UW  GIC;              /* 0x03AC gPTP interrupt control register               */
    UW  GIS;              /* 0x03B0 gPTP interrupt status register                */
    UW  reserved_03B4[1]; /* 0x03B4 */
    UW  GCT[3];           /* 0x03B8 gPTP timer capture register                   */
    UW  reserved_03C4[79];/* 0x03C4 - 0x04FF */
    UW  ECMR;             /* 0x0500 E-MAC mode register                           */
    UW  reserved_0504[1]; /* 0x0504 */
    UW  RFLR;             /* 0x0508 Receive frame length register                 */
    UW  reserved_050C[1]; /* 0x050C */
    UW  ECSR;             /* 0x0510 E-MAC status register                         */
    UW  reserved_0514[1]; /* 0x0514 */
    UW  ECSIPR;           /* 0x0518 E-MAC interrupt permission register           */
    UW  reserved_051C[1]; /* 0x051C */
    UW  PIR;              /* 0x0520 PHY interface register                        */
    UW  reserved_0524[1]; /* 0x0524 */
    UW  PSR;              /* 0x0528 PHY Status Register                           */
    UW  PIPR;             /* 0x052C PHY_INT Polarity Register                     */
    UW  reserved_0530[10];/* 0x0530 - 0x0557 */
    UW  MPR;              /* 0x0558 Manual PAUSE frame register                   */
    UW  PFTCR;            /* 0x055C PAUSE frame transmit counter                  */
    UW  PFRCR;            /* 0x0560 PAUSE frame receive counter                   */
    UW  reserved_0564[19];/* 0x0564 - 0x05AF */
    UW  GECMR;            /* 0x05B0 EthernetAVB Mode Register                     */
    UW  reserved_05B4[3]; /* 0x05B4 - 0x05BF */
    UW  MAHR;             /* 0x05C0 E-MAC address high register                   */
    UW  reserved_05C4[1]; /* 0x05C4 - 0x05C7 */
    UW  MALR;             /* 0x05C8 E-MAC address low register                    */
    UW  reserved_05CC[93];/* 0x05CC - 0x073F */
    UW  CEFCR;            /* 0x0740 CRC error frame receive counter register      */
    UW  reserved_0744[1]; /* 0x0744 */
    UW  FRECR;            /* 0x0748 Frame receive error counter register          */
    UW  reserved_074C[1]; /* 0x074C */
    UW  TSFRCR;           /* 0x0750 Too-short frame receive counter register      */
    UW  reserved_0754[1]; /* 0x0754 */
    UW  TLFRCR;           /* 0x0758 Too-long frame receive counter register       */
    UW  reserved_075C[1]; /* 0x075C */
    UW  RFCR;             /* 0x0760 Residual-bit frame receive counter register   */
    UW  reserved_0764[5]; /* 0x0764 - 0x777 */
    UW  MAFCR;            /* 0x0778 Multicast address frame receive counter register */
};

#define REG_ETHER_AVB (*(volatile struct t_ether_avb *)(0xE6800000UL))

/**
 *  Compare Match Timer 0 (CMT0)
 */
struct t_cmt0 {
    UW  reserved_00[320];
    UW  STR0;     /* 0x0500 Compare match timer start register 0      */
    UW  reserved_01[3];
    UW  CSR0;     /* 0x0510 Compare match timer control/status register 0 */
    UW  CNT0;     /* 0x0514 Compare match timer counter 0             */
    UW  COR0;     /* 0x0518 Compare match timer constant register 0   */
    UW  reserved_02[57];
    UW  STR1;     /* 0x0600 Compare match timer start register 1      */
    UW  CSR1;     /* 0x0610 Compare match timer control/status register 1 */
    UW  CNT1;     /* 0x0614 Compare match timer counter 1             */
    UW  COR1;     /* 0x0618 Compare match timer constant register 1   */
    UW  reserved_03[633];
    UW  CLKE;     /* 0x1000 CLK enable register                       */
};

#define REG_CMT0    (*(volatile struct t_cmt0 *)(RTAPB1_BASE+0xCA0000U))



/**
 * Interrupt Number (Private Interrupts)
 */

/* Software Generated Interrupt (ID:0-15) */
#define INT_SOFT_0              0   /* SGI 0 */
#define INT_SOFT_1              1   /* SGI 1 */
#define INT_SOFT_2              2   /* SGI 2 */
#define INT_SOFT_3              3   /* SGI 3 */
#define INT_SOFT_4              4   /* SGI 4 */
#define INT_SOFT_5              5   /* SGI 5 */
#define INT_SOFT_6              6   /* SGI 6 */
#define INT_SOFT_7              7   /* SGI 7 */
#define INT_SOFT_8              8   /* SGI 8 */
#define INT_SOFT_9              9   /* SGI 9 */
#define INT_SOFT_10             10  /* SGI 10 */
#define INT_SOFT_11             11  /* SGI 11 */
#define INT_SOFT_12             12  /* SGI 12 */
#define INT_SOFT_13             13  /* SGI 13 */
#define INT_SOFT_14             14  /* SGI 14 */
#define INT_SOFT_15             15  /* SGI 15 */
/* Private Peripheral Interrupt (ID:16-31) */
#define INT_RESERVED_16         16  /* PPI 0 */
#define INT_RESERVED_17         17  /* PPI 1 */
#define INT_RESERVED_18         18  /* PPI 2 */
#define INT_RESERVED_19         19  /* PPI 3 */
#define INT_RESERVED_20         20  /* PPI 4 */
#define INT_RESERVED_21         21  /* PPI 5 */
#define INT_RESERVED_22         22  /* PPI 6 */
#define INT_RESERVED_23         23  /* PPI 7 */
#define INT_RESERVED_24         24  /* PPI 8 */
#define INT_VIRT_MAINTENANCE    25  /* Virtual maintenance interrupt */
#define INT_HYP_TIMER           26  /* Hypervisor timer */
#define INT_VIRT_TIMER          27  /* Virtual timer */
#define INT_FIQ                 28  /* Legacy FIQ signal */
#define INT_S_PHYSICAL_TIMER    29  /* Secure physical timer */
#define INT_NS_PHYSICAL_TIMER   30  /* Non-secure physical time */
#define INT_IRQ                 31  /* IRQ signal from the PL */

/**
 * Interrupt Number (Shared Peripheral Interrupts)
 * 384 SPI interrupts are integrated into ID[415:32]
 */
#if defined(CFG_G1M) || defined(CFG_G1N)
#define INT_IRQ0                32
#define INT_IRQ1                33
#define INT_IRQ2                34
#define INT_IRQ3                35
#define INT_GPIO0               36
#define INT_GPIO1               37
#define INT_GPIO2               38
#define INT_GPIO3               39
#define INT_GPIO4               40
#define INT_GPIO5               41
#define INT_GPIO6               42
#define INT_GPIO7               43
#define INT_IRQ4                44
#define INT_IRQ5                45
#define INT_IRQ6                46
#define INT_IRQ7                47
#define INT_IRQ8                48
#define INT_IRQ9                49
#define INT_DCU                 50
#define INT_I2C4                51
#define INT_I2C5                52
#define INT_HSCIF2              53
#define INT_SCIF2               54
#define INT_SCIF3               55
#define INT_SCIF4               56
#define INT_SCIF5               57
#define INT_RESERVED_58         58
#define INT_RESERVED_59         59
#define INT_RESERVED_60         60
#define INT_SCIFA3              61
#define INT_SCIFA4              62
#define INT_SCIFA5              63
#define INT_RESERVED_64         64
#define INT_RESERVED_65         65
#define INT_RESERVED_66         66
#define INT_RESERVED_67         67
#define INT_RESERVED_68         68
#define INT_RESERVED_69         69
#define INT_RESERVED_70         70
#define INT_RESERVED_71         71
#define INT_RESERVED_72         72
#define INT_RESERVED_73         73
#define INT_RESERVED_74         74
#define INT_RESERVED_75         75
#define INT_RESERVED_76         76
#define INT_RESERVED_77         77
#define INT_RESERVED_78         78
#define INT_RESERVED_79         79
#define INT_RESERVED_80         80
#define INT_RESERVED_81         81
#define INT_RESERVED_82         82
#define INT_RESERVED_83         83
#define INT_RESERVED_84         84
#define INT_RESERVED_85         85
#define INT_RESERVED_86         86
#define INT_RESERVED_87         87
#define INT_RESERVED_88         88
#define INT_RESERVED_89         89
#define INT_RESERVED_90         90
#define INT_RESERVED_91         91
#define INT_RESERVED_92         92
#define INT_RESERVED_93         93
#define INT_RESERVED_94         94
#define INT_RESERVED_95         95
#define INT_RESERVED_96         96
#define INT_RESERVED_97         97
#define INT_RESERVED_98         98
#define INT_RESERVED_99         99
#define INT_RESERVED_100        100
#define INT_THS                 101
#define INT_RESERVED_102        102
#define INT_RESERVED_103        103
#define INT_CA15_IRQPNU_CORE0   104
#define INT_CA15_IRQPNU_CORE1   105
#define INT_RESERVED_106        106
#define INT_RESERVED_107        107
#define INT_CA15_CTIIRQ_CORE0   108
#define INT_CA15_CTIIRQ_CORE1   109
#define INT_RESERVED_110        110
#define INT_RESERVED_111        111
#define INT_CA15_nINTERRIRQ     112
#define INT_CA15_nAXIERRIRQ     113
#define INT_RESERVED_114        114
#define INT_RESERVED_115        115
#define INT_RESERVED_116        116
#define INT_RESERVED_117        117
#define INT_RESERVED_118        118
#define INT_RESERVED_119        119
#define INT_RESERVED_120        120
#define INT_RESERVED_121        121
#define INT_RESERVED_122        122
#define INT_RESERVED_123        123
#define INT_CCI400              124
#define INT_CCI400_OVF          125
#define INT_RESERVED_126        126
#define INT_RESERVED_127        127
#define INT_LBSC_WT0            128
#define INT_LBSC_ATA            129
#define INT_LBSC_DMAC0          130
#define INT_LBSC_DMAC1          131
#define INT_LBSC_DMAC2          132
#define INT_USB3H_HOST          133
#define INT_RESERVED_134        134
#define INT_RESERVED_135        135
#define INT_RESERVED_136        136
#define INT_SATA0               137
#define INT_SATA1               138
#define INT_USB2_597            139
#define INT_USB2_HOST0          140
#define INT_USB_DMAC_CH0        141
#define INT_USB_DMAC_CH1        142
#define INT_USB_DMAC_DDM        143
#define INT_RESERVED_144        144
#define INT_USB2_HOST1          145
#define INT_RESERVED_146        146
#define INT_RESERVED_147        147
#define INT_PCIEC               148
#define INT_PCIEC_DMA           149
#define INT_PCIEC_ERROR         150
#define INT_POWERVR_G6400       151
#define INT_CMT1_0              152
#define INT_CMT1_1              153
#define INT_CMT1_2              154
#define INT_CMT1_3              155
#define INT_CMT1_4              156
#define INT_CMT1_5              157
#define INT_CMT1_6              158
#define INT_CMT1_7              159
#define INT_TMU1_TUNI0          160
#define INT_TMU1_TUNI1          161
#define INT_TMU1_TUNI2          162
#define INT_TMU3_TUNI0          163
#define INT_TMU3_TUNI1          164
#define INT_TMU3_TUNI2          165
#define INT_RESERVED_166        166
#define INT_TPU                 167
#define INT_TMU0_TUNI0          168
#define INT_TMU0_TUNI1          169
#define INT_TMU0_TUNI2          170
#define INT_RESERVED_171        171
#define INT_RWDT                172
#define INT_RESERVED_173        173
#define INT_CMT0_0              174
#define INT_CMT0_1              175
#define INT_SCIFA0              176
#define INT_SCIFA1              177
#define INT_RESERVED_178        178
#define INT_TMU1_TUNI3          179
#define INT_SCIFB0              180
#define INT_SCIFB1              181
#define INT_SCIFB2              182
#define INT_SCIFA2              183
#define INT_SCIF0               184
#define INT_SCIF1               185
#define INT_HSCIF0              186
#define INT_HSCIF1              187
#define INT_MSIOF0              188
#define INT_MSIOF1              189
#define INT_MSIOF2              190
#define INT_RESERVED_191        191
#define INT_RESERVED_192        192
#define INT_RESERVED_193        193
#define INT_ETHER_MAC           194
#define INT_ETHER_AVB           195
#define INT_RESERVED_196        196
#define INT_SDHI0               197
#define INT_RESERVED_198        198
#define INT_SDHI2               199
#define INT_SDHI3               200
#define INT_MMC0                201
#define INT_RESERVED_202        202
#define INT_RESERVED_203        203
#define INT_RESERVED_204        204
#define INT_IIC3                205
#define INT_IIC0                206
#define INT_IIC1                207
#define INT_RESERVED_208        208
#define INT_RESERVED_209        209
#define INT_RESERVED_210        210
#define INT_RESERVED_211        211
#define INT_RESERVED_212        212
#define INT_RESERVED_213        213
#define INT_RESERVED_214        214
#define INT_RESERVED_215        215
#define INT_QSPI                216
#define INT_RESERVED_217        217
#define INT_CAN0                218
#define INT_CAN1                219
#define INT_VIN0                220
#define INT_VIN1                221
#define INT_VIN2                222
#define INT_RESERVED_223        223
#define INT_RESERVED_224        224
#define INT_RESERVED_225        225
#define INT_RESERVED_226        226
#define INT_RESERVED_227        227
#define INT_RESERVED_228        228
#define INT_SYS_DMAC0_ERR       229
#define INT_IPMMUDS0            230
#define INT_IPMMUDS0_SEC        231
#define INT_SYS_DMAC_DEI0       232
#define INT_SYS_DMAC_DEI1       233
#define INT_SYS_DMAC_DEI2       234
#define INT_SYS_DMAC_DEI3       235
#define INT_DMAC_DEI4           236
#define INT_SYS_DMAC_DEI5       237
#define INT_SYS_DMAC_DEI6       238
#define INT_SYS_DMAC_DEI7       239
#define INT_SYS_DMAC_DEI8       240
#define INT_SYS_DMAC_DEI9       241
#define INT_SYS_DMAC_DEI10      242
#define INT_SYS_DMAC_DEI11      243
#define INT_SYS_DMAC_DEI12      244
#define INT_SYS_DMAC_DEI13      245
#define INT_SYS_DMAC_DEI14      246
#define INT_RESERVED_247        247
#define INT_SYS_DMAC_DEI15      248
#define INT_SYS_DMAC_DEI16      249
#define INT_SYS_DMAC_DEI17      250
#define INT_SYS_DMAC_DEI18      251
#define INT_SYS_DMAC1_ERR       252
#define INT_IPMMU_M_SEC         253
#define INT_IPMMU_M             254
#define INT_IPMMU_SY0           255
#define INT_IPMMU_SY0_SEC       256
#define INT_IPMMU_SY1           257
#define INT_IPMMU_MP            258
#define INT_RESERVED_259        259
#define INT_RESERVED_260        260
#define INT_RESERVED_261        261
#define INT_RESERVED_262        262
#define INT_RESERVED_263        263
#define INT_RESERVED_264        264
#define INT_CORESIGHT           265
#define INT_APMU1               266
#define INT_RESERVED_267        267
#define INT_RESERVED_268        268
#define INT_RESERVED_269        269
#define INT_RESERVED_270        270
#define INT_RESERVED_271        271
#define INT_RESERVED_272        272
#define INT_RESERVED_273        273
#define INT_RESERVED_274        274
#define INT_RESERVED_275        275
#define INT_CPG                 276
#define INT_APMU0               277
#define INT_VSPD0               278
#define INT_VSPD1               279
#define INT_GPIO0_ALT           280
#define INT_GPIO1_ALT           281
#define INT_GPIO2_ALT           282
#define INT_GPIO3_ALT           283
#define INT_GPIO4_ALT           284
#define INT_GPIO5_ALT           285
#define INT_GPIO6_ALT           286
#define INT_GPIO7_ALT           287
#define INT_DU0                 288
#define INT_RESERVED_289        289
#define INT_VCP3_VLC0           290
#define INT_VCP3_CE0            291
#define INT_IPMMU_GP            292
#define INT_IPMMU_GP_SEC        293
#define INT_FDP1_0              294
#define INT_FDP1_1              295
#define INT_RESERVED_296        296
#define INT_RESERVED_297        297
#define INT_RESERVED_298        298
#define INT_VSPS                299
#define INT_DU1                 300
#define INT_RESERVED_301        301
#define INT_RESERVED_302        302
#define INT_RESERVED_303        303
#define INT_RESERVED_304        304
#define INT_RESERVED_305        305
#define INT_RESERVED_306        306
#define INT_RESERVED_307        307
#define INT_RESERVED_308        308
#define INT_RESERVED_309        309
#define INT_RESERVED_310        310
#define INT_RESERVED_311        311
#define INT_RESERVED_312        312
#define INT_RESERVED_313        313
#define INT_RESERVED_314        314
#if defined(CFG_G1M)
#define INT_TSIF0               315
#else
#define INT_RESERVED_315        315
#endif
#define INT_RESERVED_316        316
#define INT_2D_DMAC             317
#define INT_I2C2                318
#define INT_I2C0                319
#define INT_I2C1                320
#define INT_RESERVED_321        321
#define INT_I2C3                322
#define INT_RESERVED_323        323
#define INT_RESERVED_324        324
#define INT_RESERVED_325        325
#define INT_RESERVED_326        326
#define INT_RESERVED_327        327
#define INT_RESERVED_328        328
#define INT_SYSC                329
#define INT_RESERVED_330        330
#define INT_RESERVED_331        331
#define INT_RESERVED_332        332
#define INT_RESERVED_333        333
#define INT_RESERVED_334        334
#define INT_TMU2_TUNI0          335
#define INT_TMU2_TUNI1          336
#define INT_TMU2_TUNI2          337
#define INT_TMU2_TUNI3          338
#define INT_RESERVED_339        339
#define INT_SYS_DMAC_DEI19      340
#define INT_SYS_DMAC_DEI20      341
#define INT_SYS_DMAC_DEI21      342
#define INT_SYS_DMAC_DEI22      343
#define INT_SYS_DMAC_DEI23      344
#define INT_SYS_DMAC_DEI24      345
#define INT_SYS_DMAC_DEI25      346
#define INT_SYS_DMAC_DEI26      347
#define INT_SYS_DMAC_DEI27      348
#define INT_SYS_DMAC_DEI28      349
#define INT_SYS_DMAC_DEI29      350
#define INT_RESERVED_351        351
#define INT_AUDIO_DMAC_DEI0     352
#define INT_AUDIO_DMAC_DEI1     353
#define INT_AUDIO_DMAC_DEI2     354
#define INT_AUDIO_DMAC_DEI3     355
#define INT_AUDIO_DMAC_DEI4     356
#define INT_AUDIO_DMAC_DEI5     357
#define INT_AUDIO_DMAC_DEI6     358
#define INT_AUDIO_DMAC_DEI7     359
#define INT_AUDIO_DMAC_DEI8     360
#define INT_AUDIO_DMAC_DEI9     361
#define INT_AUDIO_DMAC_DEI10    362
#define INT_AUDIO_DMAC_DEI11    363
#define INT_AUDIO_DMAC_DEI12    364
#define INT_AUDIO_DMAC_DEI13    365
#define INT_AUDIO_DMAC_DEI14    366
#define INT_AUDIO_DMAC_DEI15    367
#define INT_AUDIO_DMAC_DEI16    368
#define INT_AUDIO_DMAC_DEI17    369
#define INT_AUDIO_DMAC_DEI18    370
#define INT_AUDIO_DMAC_DEI19    371
#define INT_AUDIO_DMAC_DEI20    372
#define INT_AUDIO_DMAC_DEI21    373
#define INT_AUDIO_DMAC_DEI22    374
#define INT_AUDIO_DMAC_DEI23    375
#define INT_AUDIO_DMAC_DEI24    376
#define INT_AUDIO_DMAC_DEI25    377
#define INT_AUDIO_DMAC0_ERR     378
#define INT_AUDIO_DMAC1_ERR     379
#define INT_RESERVED_380        380
#define INT_RESERVED_381        381
#define INT_RESERVED_382        382
#define INT_RESERVED_383        383
#define INT_SCU0                384
#define INT_SCU1                385
#define INT_SCU2                386
#define INT_SCU3                387
#define INT_SCU4                388
#define INT_SCU5                389
#define INT_SCU6                390
#define INT_SCU7                391
#define INT_SCU8                392
#define INT_SCU9                393
#define INT_RESERVED_394        394
#define INT_RESERVED_395        395
#define INT_RESERVED_396        396
#define INT_RESERVED_397        397
#define INT_RESERVED_398        398
#define INT_RESERVED_399        399
#define INT_RESERVED_400        400
#define INT_RESERVED_401        401
#define INT_SSI0                402
#define INT_SSI1                403
#define INT_SSI2                404
#define INT_SSI3                405
#define INT_SSI4                406
#define INT_SSI5                407
#define INT_SSI6                408
#define INT_SSI7                409
#define INT_SSI8                410
#define INT_SSI9                411
#define INT_S3_CTRL             412
#define INT_S3_CTRL_SECURE      413
#define INT_RESERVED_414        414
#define INT_RESERVED_415        415
#else

#ifdef  CFG_G1H
#define INT_IRQ0                32
#define INT_IRQ1                33
#define INT_IRQ2                34
#define INT_IRQ3                35
#define INT_GPIO0               36
#define INT_GPIO1               37
#define INT_GPIO2               38
#define INT_GPIO3               39
#define INT_GPIO4               40
#define INT_GPIO5               41
#define INT_RESERVED_42         42
#define INT_RESERVED_43         43
#define INT_RESERVED_44         44
#define INT_RESERVED_45         45
#define INT_RESERVED_46         46
#define INT_RESERVED_47         47
#define INT_RESERVED_48         48
#define INT_RESERVED_49         49
#define INT_RESERVED_50         50
#define INT_RESERVED_51         51
#define INT_RESERVED_52         52
#define INT_RESERVED_53         53
#define INT_RESERVED_54         54
#define INT_RESERVED_55         55
#define INT_RESERVED_56         56
#define INT_RESERVED_57         57
#define INT_RESERVED_58         58
#define INT_RESERVED_59         59
#define INT_RESERVED_60         60
#define INT_RESERVED_61         61
#define INT_RESERVED_62         62
#define INT_RESERVED_63         63
#define INT_RESERVED_64         64
#define INT_RESERVED_65         65
#define INT_RESERVED_66         66
#define INT_RESERVED_67         67
#define INT_RESERVED_68         68
#define INT_RESERVED_69         69
#define INT_RESERVED_70         70
#define INT_RESERVED_71         71
#define INT_RESERVED_72         72
#define INT_RESERVED_73         73
#define INT_RESERVED_74         74
#define INT_RESERVED_75         75
#define INT_RESERVED_76         76
#define INT_RESERVED_77         77
#define INT_RESERVED_78         78
#define INT_RESERVED_79         79
#define INT_RESERVED_80         80
#define INT_RESERVED_81         81
#define INT_RESERVED_82         82
#define INT_RESERVED_83         83
#define INT_RESERVED_84         84
#define INT_RESERVED_85         85
#define INT_RESERVED_86         86
#define INT_RESERVED_87         87
#define INT_RESERVED_88         88
#define INT_RESERVED_89         89
#define INT_RESERVED_90         90
#define INT_RESERVED_91         91
#define INT_RESERVED_92         92
#define INT_RESERVED_93         93
#define INT_RESERVED_94         94
#define INT_RESERVED_95         95
#define INT_RESERVED_96         96
#define INT_RESERVED_97         97
#define INT_RESERVED_98         98
#define INT_RESERVED_99         99
#define INT_RESERVED_100        100
#define INT_THS                 101
#define INT_RESERVED_102        102
#define INT_RESERVED_103        103
#define INT_CA15_IRQPNU_CORE0   104
#define INT_CA15_IRQPNU_CORE1   105
#define INT_CA15_IRQPNU_CORE2   106
#define INT_CA15_IRQPNU_CORE3   107
#define INT_CA15_CTIIRQ_CORE0   108
#define INT_CA15_CTIIRQ_CORE1   109
#define INT_CA15_CTIIRQ_CORE2   110
#define INT_CA15_CTIIRQ_CORE3   111
#define INT_CA15_nINTERRIRQ     112
#define INT_CA15_nAXIERRIRQ     113
#define INT_CA7_IRQPMU_CORE0    114
#define INT_CA7_IRQPMU_CORE1    115
#define INT_CA7_IRQPMU_CORE2    116
#define INT_CA7_IRQPMU_CORE3    117
#define INT_CA7_CTIIRQ_CORE0    118
#define INT_CA7_CTIIRQ_CORE1    119
#define INT_CA7_CTIIRQ_CORE2    120
#define INT_CA7_CTIIRQ_CORE3    121
#define INT_RESERVED_122        122
#define INT_CA7_nAXIERRIRQ      123
#define INT_CCI400              124
#define INT_CCI400_OVF          125
#define INT_RESERVED_126        126
#define INT_RESERVED_127        127
#define INT_LBSC_WT0            128
#define INT_LBSC_ATA            129
#define INT_LBSC_DMAC0          130
#define INT_LBSC_DMAC1          131
#define INT_LBSC_DMAC2          132
#define INT_USB3H_HOST          133
#define INT_RESERVED_134        134
#define INT_RESERVED_135        135
#define INT_RESERVED_136        136
#define INT_SATA0               137
#define INT_SATA1               138
#define INT_USB2_597            139
#define INT_USB2_HOST0          140
#define INT_USB_DMAC_CH0        141
#define INT_USB_DMAC_CH1        142
#define INT_USB_DMAC_DDM        143
#define INT_USB2_HOST1          144
#define INT_USB2_HOST2          145
#define INT_R_GP2D              146
#define INT_RESERVED_147        147
#define INT_PCIEC               148
#define INT_PCIEC_DMA           149
#define INT_PCIEC_ERROR         150
#define INT_POWERVR_G6400       151
#define INT_CMT1_0              152
#define INT_CMT1_1              153
#define INT_CMT1_2              154
#define INT_CMT1_3              155
#define INT_CMT1_4              156
#define INT_CMT1_5              157
#define INT_CMT1_6              158
#define INT_CMT1_7              159
#define INT_TMU1_TUNI0          160
#define INT_TMU1_TUNI1          161
#define INT_TMU1_TUNI2          162
#define INT_TMU3_TUNI0          163
#define INT_TMU3_TUNI1          164
#define INT_TMU3_TUNI2          165
#define INT_RESERVED_166        166
#define INT_TPU                 167
#define INT_TMU0_TUNI0          168
#define INT_TMU0_TUNI1          169
#define INT_TMU0_TUNI2          170
#define INT_RESERVED_171        171
#define INT_RWDT                172
#define INT_RESERVED_173        173
#define INT_CMT0_0              174
#define INT_CMT0_1              175
#define INT_SCIFA0              176
#define INT_SCIFA1              177
#define INT_RESERVED_178        178
#define INT_TMU1_TUNI3          179
#define INT_SCIFB0              180
#define INT_SCIFB1              181
#define INT_SCIFB2              182
#define INT_SCIFA2              183
#define INT_SCIF0               184
#define INT_SCIF1               185
#define INT_HSCIF0              186
#define INT_HSCIF1              187
#define INT_MSIOF0              188
#define INT_MSIOF1              189
#define INT_MSIOF2              190
#define INT_MSIOF3              191
#define INT_RESERVED_192        192
#define INT_RESERVED_193        193
#define INT_ETHER_MAC           194
#define INT_ETHER_AVB           195
#define INT_SCIF2               196
#define INT_SDHI0               197
#define INT_SDHI1               198
#define INT_SDHI2               199
#define INT_SDHI3               200
#define INT_MMC0                201
#define INT_MMC1                202
#define INT_RESERVED_203        203
#define INT_RESERVED_204        204
#define INT_IIC3                205
#define INT_IIC0                206
#define INT_IIC1                207
#define INT_IIC2                208
#define INT_RESERVED_209        209
#define INT_RESERVED_210        210
#define INT_RESERVED_211        211
#define INT_RESERVED_212        212
#define INT_RESERVED_213        213
#define INT_RESERVED_214        214
#define INT_RESERVED_215        215
#define INT_QSPI                216
#define INT_RESERVED_217        217
#define INT_CAN0                218
#define INT_CAN1                219
#define INT_VIN0                220
#define INT_VIN1                221
#define INT_VIN2                222
#define INT_VIN3                223
#define INT_IMR_X2_0            224
#define INT_IMR_X2_1            225
#define INT_IMR_LSX2_0          226
#define INT_IMR_LSX2_1          227
#define INT_RESERVED_228        228
#define INT_SYS_DMAC0_ERR       229
#define INT_IPMMUDS0            230
#define INT_IPMMUDS0_SEC        231
#define INT_SYS_DMAC_DEI0       232
#define INT_SYS_DMAC_DEI1       233
#define INT_SYS_DMAC_DEI2       234
#define INT_SYS_DMAC_DEI3       235
#define INT_DMAC_DEI4           236
#define INT_SYS_DMAC_DEI5       237
#define INT_SYS_DMAC_DEI6       238
#define INT_SYS_DMAC_DEI7       239
#define INT_SYS_DMAC_DEI8       240
#define INT_SYS_DMAC_DEI9       241
#define INT_SYS_DMAC_DEI10      242
#define INT_SYS_DMAC_DEI11      243
#define INT_SYS_DMAC_DEI12      244
#define INT_SYS_DMAC_DEI13      245
#define INT_SYS_DMAC_DEI14      246
#define INT_RESERVED_247        247
#define INT_SYS_DMAC_DEI15      248
#define INT_SYS_DMAC_DEI16      249
#define INT_SYS_DMAC_DEI17      250
#define INT_SYS_DMAC_DEI18      251
#define INT_SYS_DMAC1_ERR       252
#define INT_IPMMU_M_SEC         253
#define INT_IPMMU_M             254
#define INT_IPMMU_SY0           255
#define INT_IPMMU_SY0_SEC       256
#define INT_IPMMU_SY1           257
#define INT_IPMMU_MP            258
#define INT_RESERVED_259        259
#define INT_RESERVED_260        260
#define INT_RESERVED_261        261
#define INT_RESERVED_262        262
#define INT_RESERVED_263        263
#define INT_RESERVED_264        264
#define INT_CORESIGHT           265
#define INT_APMU1               266
#define INT_RESERVED_267        267
#define INT_RESERVED_268        268
#define INT_RESERVED_269        269
#define INT_RESERVED_270        270
#define INT_RESERVED_271        271
#define INT_RESERVED_272        272
#define INT_RESERVED_273        273
#define INT_RESERVED_274        274
#define INT_RESERVED_275        275
#define INT_CPG                 276
#define INT_APMU0               277
#define INT_VSPD0               278
#define INT_VSPD1               279
#define INT_GPIO0_ALT           280
#define INT_GPIO1_ALT           281
#define INT_GPIO2_ALT           282
#define INT_GPIO3_ALT           283
#define INT_GPIO4_ALT           284
#define INT_GPIO5_ALT           285
#define INT_RESERVED_286        286
#define INT_RESERVED_287        287
#define INT_DU0                 288
#define INT_DRC                 289
#define INT_VCP3_VLC0           290
#define INT_VCP3_CE0            291
#define INT_VCP3_VLC1           292
#define INT_VCP3_CE1            293
#define INT_FDP1_0              294
#define INT_FDP1_1              295
#define INT_FDP1_2              296
#define INT_RESERVED_297        297
#define INT_VSPR                298
#define INT_VSPS                299
#define INT_DU1                 300
#define INT_DU2                 301
#define INT_RESERVED_302        302
#define INT_RESERVED_303        303
#define INT_RESERVED_304        304
#define INT_RESERVED_305        305
#define INT_RESERVED_306        306
#define INT_RESERVED_307        307
#define INT_RESERVED_308        308
#define INT_RESERVED_309        309
#define INT_RESERVED_310        310
#define INT_RESERVED_311        311
#define INT_RESERVED_312        312
#define INT_RESERVED_313        313
#define INT_RESERVED_314        314
#define INT_RESERVED_315        315
#define INT_RESERVED_316        316
#define INT_2D_DMAC             317
#define INT_I2C2                318
#define INT_I2C0                319
#define INT_I2C1                320
#define INT_RESERVED_321        321
#define INT_I2C3                322
#define INT_RESERVED_323        323
#define INT_RESERVED_324        324
#define INT_RESERVED_325        325
#define INT_RESERVED_326        326
#define INT_RESERVED_327        327
#define INT_RESERVED_328        328
#define INT_SYSC                329
#define INT_RESERVED_330        330
#define INT_RESERVED_331        331
#define INT_RESERVED_332        332
#define INT_RESERVED_333        333
#define INT_RESERVED_334        334
#define INT_TMU2_TUNI0          335
#define INT_TMU2_TUNI1          336
#define INT_TMU2_TUNI2          337
#define INT_TMU2_TUNI3          338
#define INT_RESERVED_339        339
#define INT_SYS_DMAC_DEI19      340
#define INT_SYS_DMAC_DEI20      341
#define INT_SYS_DMAC_DEI21      342
#define INT_SYS_DMAC_DEI22      343
#define INT_SYS_DMAC_DEI23      344
#define INT_SYS_DMAC_DEI24      345
#define INT_SYS_DMAC_DEI25      346
#define INT_SYS_DMAC_DEI26      347
#define INT_SYS_DMAC_DEI27      348
#define INT_SYS_DMAC_DEI28      349
#define INT_SYS_DMAC_DEI29      350
#define INT_RESERVED_351        351
#define INT_AUDIO_DMAC_DEI0     352
#define INT_AUDIO_DMAC_DEI1     353
#define INT_AUDIO_DMAC_DEI2     354
#define INT_AUDIO_DMAC_DEI3     355
#define INT_AUDIO_DMAC_DEI4     356
#define INT_AUDIO_DMAC_DEI5     357
#define INT_AUDIO_DMAC_DEI6     358
#define INT_AUDIO_DMAC_DEI7     359
#define INT_AUDIO_DMAC_DEI8     360
#define INT_AUDIO_DMAC_DEI9     361
#define INT_AUDIO_DMAC_DEI10    362
#define INT_AUDIO_DMAC_DEI11    363
#define INT_AUDIO_DMAC_DEI12    364
#define INT_AUDIO_DMAC_DEI13    365
#define INT_AUDIO_DMAC_DEI14    366
#define INT_AUDIO_DMAC_DEI15    367
#define INT_AUDIO_DMAC_DEI16    368
#define INT_AUDIO_DMAC_DEI17    369
#define INT_AUDIO_DMAC_DEI18    370
#define INT_AUDIO_DMAC_DEI19    371
#define INT_AUDIO_DMAC_DEI20    372
#define INT_AUDIO_DMAC_DEI21    373
#define INT_AUDIO_DMAC_DEI22    374
#define INT_AUDIO_DMAC_DEI23    375
#define INT_AUDIO_DMAC_DEI24    376
#define INT_AUDIO_DMAC_DEI25    377
#define INT_AUDIO_DMAC0_ERR     378
#define INT_AUDIO_DMAC1_ERR     379
#define INT_RESERVED_380        380
#define INT_RESERVED_381        381
#define INT_RESERVED_382        382
#define INT_RESERVED_383        383
#define INT_SCU0                384
#define INT_SCU1                385
#define INT_SCU2                386
#define INT_SCU3                387
#define INT_SCU4                388
#define INT_SCU5                389
#define INT_SCU6                390
#define INT_SCU7                391
#define INT_SCU8                392
#define INT_SCU9                393
#define INT_RESERVED_394        394
#define INT_RESERVED_395        395
#define INT_RESERVED_396        396
#define INT_RESERVED_397        397
#define INT_RESERVED_398        398
#define INT_RESERVED_399        399
#define INT_RESERVED_400        400
#define INT_RESERVED_401        401
#define INT_SSI0                402
#define INT_SSI1                403
#define INT_SSI2                404
#define INT_SSI3                405
#define INT_SSI4                406
#define INT_SSI5                407
#define INT_SSI6                408
#define INT_SSI7                409
#define INT_SSI8                410
#define INT_SSI9                411
#define INT_S3_CTRL             412
#define INT_S3_CTRL_SECURE      413
#define INT_RESERVED_414        414
#define INT_RESERVED_415        415
#else

#define INT_IRQ0                32
#define INT_IRQ1                33
#define INT_IRQ2                34
#define INT_IRQ3                35
#define INT_GPIO0               36
#define INT_GPIO1               37
#define INT_GPIO2               38
#define INT_GPIO3               39
#define INT_GPIO4               40
#define INT_GPIO5               41
#define INT_GPIO6               42
#define INT_RESERVED_43         43
#define INT_IRQ4                44
#define INT_IRQ5                45
#define INT_IRQ6                46
#define INT_IRQ7                47
#define INT_IRQ8                48
#define INT_IRQ9                49
#define INT_DCU                 50
#define INT_I2C4                51
#define INT_I2C5                52
#define INT_HSCIF2              53
#define INT_SCIF2               54
#define INT_SCIF3               55
#define INT_SCIF4               56
#define INT_SCIF5               57
#define INT_RESERVED_58         58
#define INT_RESERVED_59         59
#define INT_RESERVED_60         60
#define INT_SCIFA3              61
#define INT_SCIFA4              62
#define INT_SCIFA5              63
#define INT_RESERVED_64         64
#define INT_RESERVED_65         65
#define INT_RESERVED_66         66
#define INT_RESERVED_67         67
#define INT_RESERVED_68         68
#define INT_RESERVED_69         69
#define INT_RESERVED_70         70
#define INT_RESERVED_71         71
#define INT_RESERVED_72         72
#define INT_RESERVED_73         73
#define INT_RESERVED_74         74
#define INT_RESERVED_75         75
#define INT_RESERVED_76         76
#define INT_RESERVED_77         77
#define INT_RESERVED_78         78
#define INT_RESERVED_79         79
#define INT_RESERVED_80         80
#define INT_RESERVED_81         81
#define INT_RESERVED_82         82
#define INT_RESERVED_83         83
#define INT_RESERVED_84         84
#define INT_RESERVED_85         85
#define INT_RESERVED_86         86
#define INT_RESERVED_87         87
#define INT_RESERVED_88         88
#define INT_RESERVED_89         89
#define INT_RESERVED_90         90
#define INT_RESERVED_91         91
#define INT_RESERVED_92         92
#define INT_RESERVED_93         93
#define INT_RESERVED_94         94
#define INT_RESERVED_95         95
#define INT_RESERVED_96         96
#define INT_RESERVED_97         97
#define INT_RESERVED_98         98
#define INT_RESERVED_99         99
#define INT_RESERVED_100        100
#define INT_RESERVED_101        101
#define INT_RESERVED_102        102
#define INT_RESERVED_103        103
#define INT_RESERVED_104        104
#define INT_RESERVED_105        105
#define INT_RESERVED_106        106
#define INT_RESERVED_107        107
#define INT_RESERVED_108        108
#define INT_RESERVED_109        109
#define INT_RESERVED_110        110
#define INT_RESERVED_111        111
#define INT_RESERVED_112        112
#define INT_RESERVED_113        113
#define INT_CA7_IRQPMU_CORE0    114
#define INT_CA7_IRQPMU_CORE1    115
#define INT_RESERVED_116        116
#define INT_RESERVED_117        117
#define INT_CA7_CTIIRQ_CORE0    118
#define INT_CA7_CTIIRQ_CORE1    119
#define INT_RESERVED_120        120
#define INT_RESERVED_121        121
#define INT_RESERVED_122        122
#define INT_CA7_nAXIERRIRQ      123
#define INT_CCI400              124
#define INT_CCI400_OVF          125
#define INT_RESERVED_126        126
#define INT_RESERVED_127        127
#define INT_LBSC_WT0            128
#define INT_LBSC_ATA            129
#define INT_LBSC_DMAC0          130
#define INT_LBSC_DMAC1          131
#define INT_LBSC_DMAC2          132
#define INT_RESERVED_133        133
#define INT_RESERVED_134        134
#define INT_RESERVED_135        135
#define INT_RESERVED_136        136
#define INT_RESERVED_137        137
#define INT_RESERVED_138        138
#define INT_USB2_597            139
#define INT_USB2_HOST0          140
#define INT_USB_DMAC_CH0        141
#define INT_USB_DMAC_CH1        142
#define INT_USB_DMAC_DDM        143
#define INT_RESERVED_144        144
#define INT_USB2_HOST1          145
#define INT_RESERVED_146        146
#define INT_RESERVED_147        147
#define INT_RESERVED_148        148
#define INT_RESERVED_149        149
#define INT_RESERVED_150        150
#define INT_POWERVR_G6400       151
#define INT_CMT1_0              152
#define INT_CMT1_1              153
#define INT_CMT1_2              154
#define INT_CMT1_3              155
#define INT_CMT1_4              156
#define INT_CMT1_5              157
#define INT_CMT1_6              158
#define INT_CMT1_7              159
#define INT_TMU1_TUNI0          160
#define INT_TMU1_TUNI1          161
#define INT_TMU1_TUNI2          162
#define INT_TMU3_TUNI0          163
#define INT_TMU3_TUNI1          164
#define INT_TMU3_TUNI2          165
#define INT_RESERVED_166        166
#define INT_TPU                 167
#define INT_TMU0_TUNI0          168
#define INT_TMU0_TUNI1          169
#define INT_TMU0_TUNI2          170
#define INT_RESERVED_171        171
#define INT_RWDT                172
#define INT_RESERVED_173        173
#define INT_CMT0_0              174
#define INT_CMT0_1              175
#define INT_SCIFA0              176
#define INT_SCIFA1              177
#define INT_RESERVED_178        178
#define INT_TMU1_TUNI3          179
#define INT_SCIFB0              180
#define INT_SCIFB1              181
#define INT_SCIFB2              182
#define INT_SCIFA2              183
#define INT_SCIF0               184
#define INT_SCIF1               185
#define INT_HSCIF0              186
#define INT_HSCIF1              187
#define INT_MSIOF0              188
#define INT_MSIOF1              189
#define INT_MSIOF2              190
#define INT_RESERVED_191        191
#define INT_RESERVED_192        192
#define INT_RESERVED_193        193
#define INT_ETHER_MAC           194
#define INT_ETHER_AVB           195
#define INT_RESERVED_196        196
#define INT_SDHI0               197
#define INT_RESERVED_198        198
#define INT_SDHI2               199
#define INT_SDHI3               200
#define INT_MMC0                201
#define INT_RESERVED_202        202
#define INT_RESERVED_203        203
#define INT_RESERVED_204        204
#define INT_RESERVED_205        205
#define INT_IIC0                206
#define INT_IIC1                207
#define INT_RESERVED_208        208
#define INT_RESERVED_209        209
#define INT_RESERVED_210        210
#define INT_RESERVED_211        211
#define INT_RESERVED_212        212
#define INT_RESERVED_213        213
#define INT_RESERVED_214        214
#define INT_RESERVED_215        215
#define INT_QSPI                216
#define INT_RESERVED_217        217
#define INT_CAN0                218
#define INT_CAN1                219
#define INT_VIN0                220
#define INT_VIN1                221
#define INT_VIN2                222
#define INT_RESERVED_223        223
#define INT_RESERVED_224        224
#define INT_RESERVED_225        225
#define INT_RESERVED_226        226
#define INT_RESERVED_227        227
#define INT_RESERVED_228        228
#define INT_SYS_DMAC0_ERR       229
#define INT_IPMMUDS0            230
#define INT_IPMMUDS0_SEC        231
#define INT_SYS_DMAC_DEI0       232
#define INT_SYS_DMAC_DEI1       233
#define INT_SYS_DMAC_DEI2       234
#define INT_SYS_DMAC_DEI3       235
#define INT_DMAC_DEI4           236
#define INT_SYS_DMAC_DEI5       237
#define INT_SYS_DMAC_DEI6       238
#define INT_SYS_DMAC_DEI7       239
#define INT_SYS_DMAC_DEI8       240
#define INT_SYS_DMAC_DEI9       241
#define INT_SYS_DMAC_DEI10      242
#define INT_SYS_DMAC_DEI11      243
#define INT_SYS_DMAC_DEI12      244
#define INT_SYS_DMAC_DEI13      245
#define INT_SYS_DMAC_DEI14      246
#define INT_RESERVED_247        247
#define INT_SYS_DMAC_DEI15      248
#define INT_SYS_DMAC_DEI16      249
#define INT_SYS_DMAC_DEI17      250
#define INT_SYS_DMAC_DEI18      251
#define INT_SYS_DMAC1_ERR       252
#define INT_IPMMU_M_SEC         253
#define INT_IPMMU_M             254
#define INT_IPMMU_SY0           255
#define INT_IPMMU_SY0_SEC       256
#define INT_IPMMU_SY1           257
#define INT_IPMMU_MP            258
#define INT_RESERVED_259        259
#define INT_RESERVED_260        260
#define INT_RESERVED_261        261
#define INT_RESERVED_262        262
#define INT_RESERVED_263        263
#define INT_RESERVED_264        264
#define INT_CORESIGHT           265
#define INT_APMU1               266
#define INT_RESERVED_267        267
#define INT_RESERVED_268        268
#define INT_RESERVED_269        269
#define INT_RESERVED_270        270
#define INT_RESERVED_271        271
#define INT_RESERVED_272        272
#define INT_RESERVED_273        273
#define INT_RESERVED_274        274
#define INT_RESERVED_275        275
#define INT_CPG                 276
#define INT_APMU0               277
#define INT_VSPD0               278
#define INT_VSPD1               279
#define INT_GPIO0_ALT           280
#define INT_GPIO1_ALT           281
#define INT_GPIO2_ALT           282
#define INT_GPIO3_ALT           283
#define INT_GPIO4_ALT           284
#define INT_GPIO5_ALT           285
#define INT_GPIO6_ALT           286
#define INT_RESERVED_287        287
#define INT_DU0                 288
#define INT_RESERVED_289        289
#define INT_VCP3_VLC0           290
#define INT_VCP3_CE0            291
#define INT_IPMMU_GP            292
#define INT_IPMMU_GP_SEC        293
#define INT_FDP1_0              294
#define INT_RESERVED_295        295
#define INT_RESERVED_296        296
#define INT_RESERVED_297        297
#define INT_VSPR                298
#define INT_VSPS                299
#define INT_DU1                 300
#define INT_DU2                 301
#define INT_RESERVED_302        302
#define INT_RESERVED_303        303
#define INT_RESERVED_304        304
#define INT_RESERVED_305        305
#define INT_RESERVED_306        306
#define INT_RESERVED_307        307
#define INT_RESERVED_308        308
#define INT_RESERVED_309        309
#define INT_RESERVED_310        310
#define INT_RESERVED_311        311
#define INT_RESERVED_312        312
#define INT_RESERVED_313        313
#define INT_RESERVED_314        314
#define INT_RESERVED_315        315
#define INT_RESERVED_316        316
#define INT_2D_DMAC             317
#define INT_I2C2                318
#define INT_I2C0                319
#define INT_I2C1                320
#define INT_RESERVED_321        321
#define INT_I2C3                322
#define INT_RESERVED_323        323
#define INT_RESERVED_324        324
#define INT_RESERVED_325        325
#define INT_RESERVED_326        326
#define INT_RESERVED_327        327
#define INT_RESERVED_328        328
#define INT_SYSC                329
#define INT_RESERVED_330        330
#define INT_RESERVED_331        331
#define INT_RESERVED_332        332
#define INT_RESERVED_333        333
#define INT_RESERVED_334        334
#define INT_TMU2_TUNI0          335
#define INT_TMU2_TUNI1          336
#define INT_TMU2_TUNI2          337
#define INT_TMU2_TUNI3          338
#define INT_RESERVED_339        339
#define INT_SYS_DMAC_DEI19      340
#define INT_SYS_DMAC_DEI20      341
#define INT_SYS_DMAC_DEI21      342
#define INT_SYS_DMAC_DEI22      343
#define INT_SYS_DMAC_DEI23      344
#define INT_SYS_DMAC_DEI24      345
#define INT_SYS_DMAC_DEI25      346
#define INT_SYS_DMAC_DEI26      347
#define INT_SYS_DMAC_DEI27      348
#define INT_SYS_DMAC_DEI28      349
#define INT_SYS_DMAC_DEI29      350
#define INT_RESERVED_351        351
#define INT_AUDIO_DMAC_DEI0     352
#define INT_AUDIO_DMAC_DEI1     353
#define INT_AUDIO_DMAC_DEI2     354
#define INT_AUDIO_DMAC_DEI3     355
#define INT_AUDIO_DMAC_DEI4     356
#define INT_AUDIO_DMAC_DEI5     357
#define INT_AUDIO_DMAC_DEI6     358
#define INT_AUDIO_DMAC_DEI7     359
#define INT_AUDIO_DMAC_DEI8     360
#define INT_AUDIO_DMAC_DEI9     361
#define INT_AUDIO_DMAC_DEI10    362
#define INT_AUDIO_DMAC_DEI11    363
#define INT_AUDIO_DMAC_DEI12    364
#define INT_RESERVED_365        365
#define INT_RESERVED_366        366
#define INT_RESERVED_367        367
#define INT_RESERVED_368        368
#define INT_RESERVED_369        369
#define INT_RESERVED_370        370
#define INT_RESERVED_371        371
#define INT_RESERVED_372        372
#define INT_RESERVED_373        373
#define INT_RESERVED_374        374
#define INT_RESERVED_375        375
#define INT_RESERVED_376        376
#define INT_RESERVED_377        377
#define INT_AUDIO_DMAC0_ERR     378
#define INT_RESERVED_379        379
#define INT_RESERVED_380        380
#define INT_RESERVED_381        381
#define INT_RESERVED_382        382
#define INT_RESERVED_383        383
#define INT_RESERVED_384        384
#define INT_SCU1                385
#define INT_SCU2                386
#define INT_SCU3                387
#define INT_SCU4                388
#define INT_SCU5                389
#define INT_SCU6                390
#define INT_RESERVED_391        391
#define INT_RESERVED_392        392
#define INT_RESERVED_393        393
#define INT_RESERVED_394        394
#define INT_RESERVED_395        395
#define INT_RESERVED_396        396
#define INT_RESERVED_397        397
#define INT_RESERVED_398        398
#define INT_RESERVED_399        399
#define INT_RESERVED_400        400
#define INT_RESERVED_401        401
#define INT_SSI0                402
#define INT_SSI1                403
#define INT_SSI2                404
#define INT_SSI3                405
#define INT_SSI4                406
#define INT_SSI5                407
#define INT_SSI6                408
#define INT_SSI7                409
#define INT_SSI8                410
#define INT_SSI9                411
#define INT_S3_CTRL             412
#define INT_S3_CTRL_SECURE      413
#define INT_RESERVED_414        414
#define INT_RESERVED_415        415
#endif
#endif

#ifdef __cplusplus
}
#endif
#endif  /* _RZG1_UC3_H_ */
