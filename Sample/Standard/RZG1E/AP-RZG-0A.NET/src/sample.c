/**
 * @brief   Sample program
 * @date    2019.07.01
 * @author  Copyright (c) 2019, eForce Co.,Ltd.  All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2019.07.01) j.hirata
 *            Initial version.
 ****************************************************************************
 */

#include <string.h>
#include "kernel.h"
#include "kernel_id.h"
#include "DDR_CortexA_GIC.h"
#include "DDR_CortexA_GTIMER.h"
#include "DDR_COM.h"
#include "DDR_RZG1_SCIF.h"
#include "DDR_RZG1_SCIF_cfg.h"
#include "krn_cfg.h"
#define CFG_G1E
#include "RZG1_UC3.h"

#include "net_hdr.h"
#include "net_strlib.h"

/* External function prototypes ----------------------------------------------*/
extern UINT led_dly;
extern ER net_setup(void);
extern ER net_sta_console(void);
extern void board_init(void);

/* Private function prototypes -----------------------------------------------*/
void apl_main_tsk(VP_INT);

/* Private typedef -----------------------------------------------------------*/

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/

/* Debug print */
#define put_str(x)    puts_com_opt(x)


/* Private variables ---------------------------------------------------------*/

/* Serial communication */
static const T_COM_SMOD apl_com_mod = {115200, BLEN8, PAR_NONE, SBIT1, FLW_NONE};
static const VB apl_msg[] = "\n\r\tuNet3 Sample Program for eForce Operating System \n\r"
                            "\t\tSerial Port (SCIF 0)\r\n";
static const VB apl_msg_cbk_eth[] = "\r\n-- Callback of ethernet --\r\n\r\n";

/* Task */
static const T_CTSK apl_ctsk_main = {
    TA_HLNG|TA_ACT|TA_FPU, (VP_INT)0, (FP)apl_main_tsk, 8, 0x800, 0, "apl_main_tsk"};

/*******************************
        文字列出力
 *******************************/
void puts_com_opt(const VB* msg)
{
    UINT txcnt;

    txcnt = net_strlen(msg);
    puts_com(DID_UART0, (VB*)msg, &txcnt, TMO_FEVR);

    return;
}

/******************************************************************
        Callback function of ethernet driver
        イーサネットドライバのコールバック関数
        (gNET_DEV.cbk(net_cfg.c)で本関数を登録)
 ******************************************************************/
void apl_eth_cbk(UH dev_num, UH evt, VP sts)
{
    UW sts_lnk;

    /* 下記はデバッグ用のメッセージ表示 */
    /* リリース時はコールバック関数の中で時間のかかる処理を実装しない */
    if (evt == EV_CBK_DEV_LINK) {
        put_str((VB *)apl_msg_cbk_eth);
        sts_lnk = (UW)sts;
        if (sts_lnk == PHY_STS_LINK_DOWN) {
            /* Link down */
            put_str("  Link Down");
        } else {
            /* Link up */
            put_str("  Link Up (");
            switch (sts_lnk) {
                case PHY_STS_10HD:      /* 10M Half */
                    put_str("10M/Half-Duplex)");
                    break;
                case PHY_STS_10FD:      /* 10M Full */
                    put_str("10M/Full-Duplex)");
                    break;
                case PHY_STS_100HD:     /* 100M Half */
                    put_str("100M/Half-Duplex)");
                    break;
                case PHY_STS_100FD:     /* 100M Full */
                    put_str("100M/Full-Duplex)");
                    break;
                default:
                    break;
            }
        }
        put_str("\r\n");
    }
    put_str("\r\n");

    return;
}

/*******************************
      Main Task
 *******************************/
void apl_main_tsk(VP_INT exinf)
{
    ER ercd;

    /* UART */
    ini_com(DID_UART0, &apl_com_mod);
    dly_tsk(1);
    ctr_com(DID_UART0, STA_COM, 0);
    put_str(apl_msg);

    /* Initialize Network */
    ercd = net_setup();
    if (ercd == E_OK) {
        net_sta_console();    /* UART console */
    }

    /* Blinking LED1, LED2 */
    while (1) {
        REG_GPIO6.OUTDTL = ~(0x01U << 24);
        REG_GPIO6.OUTDTH = 0x01U << 25;
        (void)dly_tsk(led_dly);
        REG_GPIO6.OUTDTH = 0x01U << 24;
        REG_GPIO6.OUTDTL = ~(0x01U << 25);
        (void)dly_tsk(led_dly);
    }
}

/*******************************
      OSの初期化ルーチン
 *******************************/
void initpr(void)
{
    extern const T_DEXC dexc_vfp;
    extern const T_DEXC dexc_dta;
    extern const T_DEXC dexc_pra;
    (void)def_exc(EXC_UDF, (T_DEXC *)&dexc_vfp);
    (void)def_exc(EXC_PRA, (T_DEXC *)&dexc_dta);
    (void)def_exc(EXC_DTA, (T_DEXC *)&dexc_pra);

    _ddr_cortexa_gtimer_init(CFG_KRN_TICK, (260000000U / 8U));
    _ddr_rzg1_scif_init(DID_UART0, &REG_SCIF0);

    /* Main task */
    MainTaskID = acre_tsk((T_CTSK*)&apl_ctsk_main);
}


/*******************************
      main entry
 *******************************/

int main(void)
{
    extern UB SYSMEM[];
    extern UB STKMEM[];
    extern UB MPLMEM[];
    ER ercd;
    T_CSYS csys;

    /* ダミーアクセス */

    SYSMEM[0] = 0U;
    STKMEM[0] = 0U;
    MPLMEM[0] = 0U;

    /* ハードウェアの初期化の残り */

    board_init();
    _ddr_gic_init(CFG_GIC_BASE);

    csys.tskpri_max = CFG_KRN_TSKPRI_MAX;
    csys.tskid_max  = CFG_KRN_TSKID_MAX;
    csys.semid_max  = CFG_KRN_SEMID_MAX;
    csys.flgid_max  = CFG_KRN_FLGID_MAX;
    csys.dtqid_max  = CFG_KRN_DTQID_MAX;
    csys.mbxid_max  = CFG_KRN_MBXID_MAX;
    csys.mtxid_max  = CFG_KRN_MTXID_MAX;
    csys.mbfid_max  = CFG_KRN_MBFID_MAX;
    csys.porid_max  = CFG_KRN_PORID_MAX;
    csys.mpfid_max  = CFG_KRN_MPFID_MAX;
    csys.mplid_max  = CFG_KRN_MPLID_MAX;
    csys.almid_max  = CFG_KRN_ALMID_MAX;
    csys.cycid_max  = CFG_KRN_CYCID_MAX;
    csys.isrid_max  = CFG_KRN_ISRID_MAX;
    csys.devid_max  = CFG_KRN_DEVID_MAX;
    csys.tick       = CFG_KRN_TICK;
    csys.ssb_num    = CFG_KRN_SSB_NUM;
    csys.sysmem_top = (VP)&SYSMEM[0];
    csys.sysmem_end = (VP)&SYSMEM[CFG_KRN_SYSMEM_SZ];
    csys.stkmem_top = (VP)&STKMEM[0];
    csys.stkmem_end = (VP)&STKMEM[CFG_KRN_STKMEM_SZ];
    csys.mplmem_top = (VP)&MPLMEM[0];
    csys.mplmem_end = (VP)&MPLMEM[CFG_KRN_MPLMEM_SZ];
    csys.sysidl = SYSTEM_IDLE;
    csys.inistk = STACK_ID_INIT;
    csys.trace = TRACE_DISABLE;
    csys.agent = AGENT_DISABLE;

    ercd = start_uC3(&csys, initpr);

    /* It does not reach */
    return (int)ercd;
}
