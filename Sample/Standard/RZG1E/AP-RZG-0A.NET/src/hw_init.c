/**
 * @brief   hardware initialization for SK-RZG1E
 * @date    2020.02.10
 * @author  Copyright (c) 2020, eForce Co.,Ltd.  All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2020.02.10)
 *            Initial version.
 ****************************************************************************
 */
#include "kernel.h"
#define CFG_G1E
#include "RZG1_UC3.h"

static void led_pad_init(void)
{
    UW reg;

    /*** LED1-2 ***/
    /* GP6_24, GP6_25 */
    reg = REG_PFC.GPSR6;
    reg = reg & ~(0x3U << 24);
    REG_PFC.PMMR = ~reg;
    REG_PFC.GPSR6 = reg;

    _kernel_synch_cache();

    /* Output mode, High and low register is used */
    reg = REG_GPIO6.POSNEG;
    REG_GPIO6.POSNEG = reg & ~(0x3U << 24);
    reg = REG_GPIO6.IOINTSEL;
    REG_GPIO6.IOINTSEL = reg & ~(0x3U << 24);
    reg = REG_GPIO6.INOUTSEL;
    REG_GPIO6.INOUTSEL = reg | (0x3U << 24);
    reg = REG_GPIO6.OUTDTSEL;
    REG_GPIO6.OUTDTSEL = reg | (0x3U << 24);

    _kernel_synch_cache();

    return;
}

static void uart_pad_init(void)
{
    UW reg;

    /*** SCIF 0 ***/

    /* GP3_27/SCIF0_RXD_D,
     * GP3_28/SCIF0_TXD_D 
     * GP3_27 -> Peripheral function selected by IP8[11:9]
     * GP3_28 -> Peripheral function selected by IP8[14:12]
     */
    reg = REG_PFC.GPSR3;
    reg &= ~(0x3U << 27);
    REG_PFC.PMMR  = ~reg;
    REG_PFC.GPSR3 = reg;    /* to GPIO */

    reg = REG_PFC.MOD_SEL3; /* Mode select */
    reg &= ~(0x3U << 30);
    reg |= (0x3U << 30);
    REG_PFC.PMMR = ~reg;
    REG_PFC.MOD_SEL3 = reg;

    reg = REG_PFC.IPSR8;    /* function select */
    reg &= ~(0x3FU << 9);
    reg |= (0x12U << 9);
    REG_PFC.PMMR  = ~reg;
    REG_PFC.IPSR8 = reg;

    reg = REG_PFC.GPSR3;    /* to peripheral */
    reg |= (0x3U << 27);
    REG_PFC.PMMR  = ~reg;
    REG_PFC.GPSR3 = reg;

    _kernel_synch_cache();

    /* Pull-up disable */
    reg = REG_PFC.PUPR3;
    reg &= ~(0x02000000U);  /* HSCIF0_HCTS(GP3[27]) */
    REG_PFC.PUPR3 = reg;

    _kernel_synch_cache();

    return;
}

static void ether_pad_init(void)
{
    UW reg;

    /*
     * GP3[13] IP6[31:29]   ETH_MDIO
     * GP3[14] IP7[ 2: 0]   ETH_CRS_DV
     * GP3[15] IP7[ 5: 3]   ETH_RX_ER
     * GP3[16] IP7[ 8: 6]   ETH_RXD0
     * GP3[17] IP7[11: 9]   ETH_RXD1
     * GP3[18] IP7[14:12]   ETH_LINK
     * GP3[19] IP7[17:15]   ETH_REF_CLK
     * GP3[20] IP7[20:18]   ETH_TXD1
     * GP3[21] IP7[23:21]   ETH_TX_EN
     * GP3[22] IP7[26:24]   ETH_MAGIC (Not using)
     * GP3[23] IP7[29:27]   ETH_TXD0
     * GP3[24] IP8[ 2: 0]   ETH_MDC
     */

    /* to GPIO */
    reg = REG_PFC.GPSR3;
    reg &= ~0x01FFE000U;
    REG_PFC.PMMR = ~reg;
    REG_PFC.GPSR3 = reg;

    /* function select */
    reg = REG_PFC.IPSR6;
    reg &= ~0xE0000000U;
    REG_PFC.PMMR = ~reg;
    REG_PFC.IPSR6 = reg;

    reg = REG_PFC.IPSR7;
    reg &= ~0x38FFFFFFU;
    REG_PFC.PMMR = ~reg;
    REG_PFC.IPSR7 = reg;

    reg = REG_PFC.IPSR8;
    reg &= ~0x00000007U;
    REG_PFC.PMMR = ~reg;
    REG_PFC.IPSR8 = reg;

    /* to peripheral */
    reg = REG_PFC.GPSR3;
    reg |= 0x01BFE000U;
    REG_PFC.PMMR = ~reg;
    REG_PFC.GPSR3 = reg;

    _kernel_synch_cache();

    /* Pull-up disable */
    reg = REG_PFC.PUPR3;
    reg &= ~0x006FF800U;
    REG_PFC.PUPR3 = reg;

    _kernel_synch_cache();

    /* GPIO_GP_3_22 */
    reg = REG_PFC.GPSR3;
    reg = reg & ~(1U << 22);
    REG_PFC.PMMR = ~reg;
    REG_PFC.GPSR3 = reg;

    _kernel_synch_cache();

    REG_GPIO3.IOINTSEL &= ~(1U << 22);
    REG_GPIO3.INOUTSEL |= (1U << 22);
    REG_GPIO3.OUTDTSEL &= ~(1U << 22);
    /* PHY_RST */
    REG_GPIO3.OUTDT |= (1U << 22);

    _kernel_synch_cache();

    return;
}


/***********************************************************************
  Hardware dependent Device Initialization
 **********************************************************************/

void board_init(void)
{
    led_pad_init();
    uart_pad_init();
    ether_pad_init();

    return;
}

