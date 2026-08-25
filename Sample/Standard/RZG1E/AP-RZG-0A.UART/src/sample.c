/**
 * @brief   Sample program
 * @date    2016.02.03
 * @author  Copyright (c) 2016, eForce Co.,Ltd.  All rights reserved.
 *
 ****************************************************************************
 * @par     History
 *          - rev 1.0 (2016.02.03) i-cho
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

/**
 * Serial port configuration
 */
T_COM_SMOD const uart_ini = {115200, BLEN8, PAR_NONE, SBIT1, FLW_NONE};
VB const * const banner_str = "\n\r\teForce Operating System Sample Program V1.0\n\r\t\tSerial Port (SCIF 0)\r\n";


/**
 * Mail
 */
typedef struct t_msgblk {
    T_MSG   header;
    UINT    cnt;
    VB      buf[120];
} T_MSGBLK;

T_CMPF const cmpf = {TA_TFIFO, 20, sizeof(T_MSGBLK), 0, "Mpf"};
T_CMBX const cmbx = {TA_TFIFO|TA_MFIFO, 0, 0, "Mbx"};

/*******************************
        Snd Task
 *******************************/
void SndTask(VP_INT exinf)
{
    UINT txcnt;
    T_MSGBLK *blk;
    int i;
    VB chr;
    ER ercd;

    ini_com(DID_UART0, &uart_ini);
    ctr_com(DID_UART0, STA_COM, 0);

    txcnt = strlen(banner_str);
    puts_com(DID_UART0, (VB *)banner_str, &txcnt, TMO_FEVR);

    /* EnterKeyが入力されるまで，0~9を順に出力する */
    for(i = 0;;) {
        ercd = getc_com(DID_UART0, &chr, 0, 10);
        if (ercd == E_OK) {
            putc_com(DID_UART0, chr, TMO_FEVR);
            if (chr == (VB)'\r') {
                putc_com(DID_UART0, (VB)'\n', TMO_FEVR);
            }
            break;
        }
        dly_tsk(989);
        chr = (VB)(i + '0');
        putc_com(DID_UART0, chr, TMO_FEVR);
        if (++i >= 10)
            i = 0;
    }

    act_tsk(RcvTaskID);
    for(;;) {
        rcv_mbx(MbxID, (T_MSG **)&blk);
        puts_com(DID_UART0, blk->buf, &blk->cnt, TMO_FEVR);
        rel_mpf(MpfID, (VP)blk);
        ctr_com(DID_UART0, CLN_TXBUF, 100);
    }
}

const T_CTSK ctsk_snd = {TA_HLNG|TA_ACT|TA_FPU, (VP_INT)0, (FP)SndTask, 4, 0x400, 0, "SndTask"};


/*******************************
        Rcv Task
 *******************************/
void RcvTask(VP_INT exinf)
{
    T_MSGBLK *blk;

    for(;;) {
        get_mpf(MpfID, (VP *)&blk);
        blk->cnt = sizeof(blk->buf) - 1;
        gets_com(DID_UART0, blk->buf, 0, '\r', &blk->cnt, TMO_FEVR);
        *((VB *)blk->buf + blk->cnt) = '\n';
        blk->cnt++;
        snd_mbx(MbxID, (T_MSG *)blk);
    }
}

const T_CTSK ctsk_rcv = {TA_HLNG|TA_FPU, (VP_INT)0, (FP)RcvTask, 5, 0x400, 0, "RcvTask"};


/*******************************
        LED Task
 *******************************/
void LEDTask(VP_INT exinf)
{
    /* Blinking LED1, LED2 */
    while (1) {
        REG_GPIO6.OUTDTL = ~(0x01U << 24);
        REG_GPIO6.OUTDTH = 0x01U << 25;
        dly_tsk(500);
        REG_GPIO6.OUTDTH = 0x01U << 24;
        REG_GPIO6.OUTDTL = ~(0x01U << 25);
        dly_tsk(500);
    }
}

const T_CTSK ctsk_led = {TA_HLNG|TA_ACT|TA_FPU, (VP_INT)0, (FP)LEDTask, 8, 0x400, 0, "LEDTask"};


/*******************************
      OSの初期化ルーチン
 *******************************/
void initpr(void)
{
    extern const T_DEXC dexc_vfp;
    extern const T_DEXC dexc_dta;
    extern const T_DEXC dexc_pra;
    def_exc(EXC_UDF, (T_DEXC *)&dexc_vfp);
    def_exc(EXC_PRA, (T_DEXC *)&dexc_dta);
    def_exc(EXC_DTA, (T_DEXC *)&dexc_pra);

    _ddr_cortexa_gtimer_init(CFG_KRN_TICK, (260000000U / 8U));
    _ddr_rzg1_scif_init(DID_UART0, &REG_SCIF0);

    SndTaskID = acre_tsk((T_CTSK *)&ctsk_snd);
    RcvTaskID = acre_tsk((T_CTSK *)&ctsk_rcv);
    LEDTaskID = acre_tsk((T_CTSK *)&ctsk_led);
    MpfID = acre_mpf((T_CMPF *)&cmpf);
    MbxID = acre_mbx((T_CMBX *)&cmbx);
}


/*******************************
      main entry
 *******************************/
extern void board_init(void);

int main(void)
{
    extern UB SYSMEM[];
    extern UB STKMEM[];
    extern UB MPLMEM[];
    ER ercd;
    T_CSYS csys;

    /* ダミーアクセス */

    SYSMEM[0] = 0;
    STKMEM[0] = 0;
    MPLMEM[0] = 0;

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
