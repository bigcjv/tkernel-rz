/**
 * @file    mmutbl_cfg.c
 * @brief   MMU configuration table
 * @date    2018.11.30
 * @author  Copyright (c) 2017-2018, eForce Co., Ltd. All rights reserved.
 * 
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2016.02.03) i-cho
 *            Initial version.
 *          - rev 1.0 (2018.11.30) yokota
 *            refactoring C code.
 ****************************************************************************
 */
#include "kernel.h"
#include "DDR_CortexA_MMU.h"
/******************************************************************************
 *      Target dependent MMU configuration
 *
 *     Memory region attributes
 *
 *         ATR_STRG -----------------------------------------+
 *         ATR_SDEV -----------------------------------------+
 *         ATR_WTNW -----------------------------------------+
 *         ATR_WBNW -----------------------------------------+
 *         ATR_NONC -----------------------------------------+-- [ATR_SHRD] -- [ATR_NEXC] -- [ATR_NGLB]
 *         ATR_WBAW -----------------------------------------+
 *         ATR_NDEV -----------------------------------------+
 *         ATR_SELA --+-- ATR_INONC --+---+-- ATR_ONONC --+--+
 *                    +-- ATR_IWBAW --+   +-- ATR_OWBAW --+
 *                    +-- ATR_IWTNW --+   +-- ATR_OWTNW --+
 *                    +-- ATR_IWBNW --+   +-- ATR_OWBNW --+
 ******************************************************************************
 */

const T_MEM_CFG mmu_cfgtbl[] = {
        /*          SIZE,      PADDR,      VADDR,    AP,     ATTR */
        {     0x04000000, 0x00000000, 0x00000000, AP_RO, ATR_SDEV  },   /*  64Mb Boot SPI-Flash */
        {     0x00100000, 0x40000000, 0x40000000, AP_RW, ATR_WBAW  },   /*  1Mb Program code(SDRAM) */
        {     0x00A00000, 0x40100000, 0x40100000, AP_RW, ATR_WBAW  },   /*  10Mb Program data(SDARM) */
        {     0x00500000, 0x40B00000, 0x40B00000, AP_RW, ATR_NONC  },   /*  5Mb Non-cacheable data area */
        {     0x01000000, 0x41000000, 0x41000000, AP_RW, ATR_WBAW|ATR_SHRD },  /* 16Mb Shareable data area */
        {     0x00100000, 0x50000000, 0x50000000, AP_RO, ATR_WBAW  },   /*  1Mb TTB */
        {     0x00200000, 0xE6000000, 0xE6000000, AP_RW, ATR_STRG  },   /*  2Mb SYS-APB1 */
        {     0x00100000, 0xE6200000, 0xE6200000, AP_RW, ATR_STRG  },   /*  1Mb SYS-APB2_1 */
        {     0x00040000, 0xE6300000, 0xE6300000, AP_RW, ATR_STRG  },   /*  256Kb SecureRAM2 */
        {     0x0083F000, 0xE63C1000, 0xE63C1000, AP_RW, ATR_STRG  },   /* SYS-APB2_2 */
        {     0x00400000, 0xE6C00000, 0xE6C00000, AP_RW, ATR_STRG  },   /*  3Mb SYS-APB3 */
        {     0x00080000, 0xE8000000, 0xE8000000, AP_RW, ATR_STRG  },   /* 512Kb IPMMU, ICB control register */
        {     0x00A00000, 0xEC000000, 0xEC000000, AP_RW, ATR_STRG  },   /*  1Mb MP-APB1 ~ 7 */
        {     0x000E0000, 0xEE000000, 0xEE000000, AP_RW, ATR_STRG  },   /* 896Kb USB */
        {     0x00004000, 0xEE0E8000, 0xEE0E8000, AP_RW, ATR_NONC  },   /*  16Kb SRAM area for the EtherAVB descriptor */
        {     0x00800000, 0xEE100000, 0xEE100000, AP_RW, ATR_STRG  },   /*  SDHI, eMMC, SATA0, SATA1, EtherMAC */
        {     0x00100000, 0xF1000000, 0xF1000000, AP_RW, ATR_STRG  },   /*  1Mb GIC */
        {     0x01400000, 0xFD000000, 0xFD000000, AP_RW, ATR_STRG  },   /*  3DG ~  */
        {     0x00A00000, 0xFE600000, 0xFE600000, AP_RW, ATR_STRG  },   /* MTX, SYS-APB7, LBSC */
        {     0x00800000, 0xFF800000, 0xFF800000, AP_RW, ATR_STRG  },   /* SYS-APB8, RT-APB, RT-AHB */
        {     0x00000000, 0x00000000, 0x00000000, 0    , 0         },   /* Terminator */
};

