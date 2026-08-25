/**
 * @file    DDR_RZG1_SCIFA.c
 * @brief   RZ/G1 Serial communication interface with FIFO A
 * @date    2016.01.20
 * @author  Copyright (c) 2016, eForce Co.,Ltd.  All rights reserved.
 *
 ********************************************************************
 * @par     History
 *          - rev 1.0 (2016.01.20) i-cho
 *            Initial version.
 *          - rev 1.1 (2016.05.11) i-cho
 *            Corrected SSR check bit in _ddr_rzg1_scifa_snd_brk.
 ********************************************************************
 */
#include "kernel.h"

#include "DDR_COM.h"
#include "DDR_RZG1_SCIFA.h"
#include "commondef.h"
/* User config header */
#include "DDR_RZG1_SCIFA_cfg.h"
#include "RZG1_UC3.h"

#include <string.h>

/* External function prototypes -----------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void _ddr_rzg1_scifa_intr(T_RZG1_SCIFA_MNG *);
void _ddr_rzg1_scifa_rxi(T_RZG1_SCIFA_MNG *);
void _ddr_rzg1_scifa_bri(T_RZG1_SCIFA_MNG *);
void _ddr_rzg1_scifa_txi(T_RZG1_SCIFA_MNG *);
ER _ddr_rzg1_scifadrv(ID, VP, T_RZG1_SCIFA_MNG *);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MSTP2_SCIFA0   (1U << 4)
#define MSTP2_SCIFA1   (1U << 3)
#define MSTP2_SCIFA2   (1U << 2)

/* Serial Status Register (SCASSRn) */
#define SSR_ORER    ((UH)BIT9)  /* Overrun Error */
#define SSR_TSF     ((UH)BIT8)  /* Transmit Data Stop  */
#define SSR_ER      ((UH)BIT7)  /* Receive Error */
#define SSR_TEND    ((UH)BIT6)  /* Transmission End */
#define SSR_TDFE    ((UH)BIT5)  /* Transmit FIFO Data Empty */
#define SSR_BRK     ((UH)BIT4)  /* Break Detect */
#define SSR_FER     ((UH)BIT3)  /* Framing Error */
#define SSR_PER     ((UH)BIT2)  /* Parity Error */
#define SSR_RDF     ((UH)BIT1)  /* Receive FIFO Data Full */
#define SSR_DR      ((UH)BIT0)  /* Receive Data Ready */

/* Serial Control Register (SCASCRn) */
#define SCR_TDRQE   ((UH)BIT15) /* Tx Data Transfer Request Enable */
#define SCR_RDRQE   ((UH)BIT14) /* Rx Data Transfer Request Enable */
#define SCR_TENDE   ((UH)BIT12) /* Transmit End Interrupt Enable */
#define SCR_TSIE    ((UH)BIT11) /* Transmit Data Stop Interrupt Enable */
#define SCR_ERIE    ((UH)BIT10) /* Receive Error Interrupt Enable */
#define SCR_BRIE    ((UH)BIT9)  /* Break Interrupt Enable */
#define SCR_DRIE    ((UH)BIT8)  /* Receive Data Ready Interrupt Enable */
#define SCR_TIE     ((UH)BIT7)  /* Transmit Interrupt Enable */
#define SCR_RIE     ((UH)BIT6)  /* Receive Interrupt Enable */
#define SCR_TE      ((UH)BIT5)  /* Transmit Enable */
#define SCR_RE      ((UH)BIT4)  /* Receive Enable */

/* FIFO Data Count Register (SCAFDRn) */
#define FTRDR_MASK  (0x007F)

/* FIFO Control Register (SCAFCRn) */
#define FCR_TFRST   ((UH)BIT2)  /* Transmit FIFO Data Register Reset  */
#define FCR_RFRST   ((UH)BIT1)  /* Receive FIFO Data Register Reset  */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

#ifdef CFG_SCIFA_0

#if (CFG_RSTRG_0 == 60)
#define TRSTRG_0    (7<<8)
#elif (CFG_RSTRG_0 == 54)
#define TRSTRG_0    (6<<8)
#elif (CFG_RSTRG_0 == 48)
#define TRSTRG_0    (5<<8)
#elif (CFG_RSTRG_0 == 32)
#define TRSTRG_0    (4<<8)
#elif (CFG_RSTRG_0 == 16)
#define TRSTRG_0    (3<<8)
#elif (CFG_RSTRG_0 == 8)
#define TRSTRG_0    (2<<8)
#elif (CFG_RSTRG_0 == 1)
#define TRSTRG_0    (1<<8)
#else   /* == 63 */
#define TRSTRG_0    (0<<8)
#endif

#if (CFG_RTRG_0 == 48)
#define TRTRG_0     (3<<6)
#elif (CFG_RTRG_0 == 32)
#define TRTRG_0     (2<<6)
#elif (CFG_RTRG_0 == 16)
#define TRTRG_0     (1<<6)
#else   /* == 1 */
#define TRTRG_0     (0<<6)
#endif

#if (CFG_TTRG_0 == 0)
#define TTTRG_0     (3<<4)
#elif (CFG_TTRG_0 == 2)
#define TTTRG_0     (2<<4)
#elif (CFG_TTRG_0 == 16)
#define TTTRG_0     (1<<4)
#else   /* == 32 */
#define TTTRG_0     (0<<4)
#endif

#if (CFG_TXBUF_SZ0==0)
#define _ddr_rzg1_scifa_tbuf0  0
#else
VB  _ddr_rzg1_scifa_tbuf0[CFG_TXBUF_SZ0];
#endif
VB  _ddr_rzg1_scifa_rbuf0[CFG_RXBUF_SZ0];
UB  _ddr_rzg1_scifa_sbuf0[CFG_RXBUF_SZ0];
T_RZG1_SCIFA_MNG _ddr_rzg1_scifa_data0;

const T_CDEV _ddr_rzg1_scifa_cdev0 = {&_ddr_rzg1_scifa_data0, (FP)_ddr_rzg1_scifadrv};
const T_CFLG _ddr_rzg1_scifa_cflg0 = {TA_TFIFO|TA_WMUL, 0x00000000};
const T_CISR _ddr_rzg1_scifa_cisr0 = {
    TA_HLNG|TA_FPU, (VP_INT)&_ddr_rzg1_scifa_data0, CFG_INT_0, (FP)_ddr_rzg1_scifa_intr, CFG_IPL_0
};

#endif


#ifdef CFG_SCIFA_1

#if (CFG_RSTRG_1 == 60)
#define TRSTRG_1    (7<<8)
#elif (CFG_RSTRG_1 == 54)
#define TRSTRG_1    (6<<8)
#elif (CFG_RSTRG_1 == 48)
#define TRSTRG_1    (5<<8)
#elif (CFG_RSTRG_1 == 32)
#define TRSTRG_1    (4<<8)
#elif (CFG_RSTRG_1 == 16)
#define TRSTRG_1    (3<<8)
#elif (CFG_RSTRG_1 == 8)
#define TRSTRG_1    (2<<8)
#elif (CFG_RSTRG_1 == 1)
#define TRSTRG_1    (1<<8)
#else   /* == 63 */
#define TRSTRG_1    (0<<8)
#endif

#if (CFG_RTRG_1 == 48)
#define TRTRG_1     (3<<6)
#elif (CFG_RTRG_1 == 32)
#define TRTRG_1     (2<<6)
#elif (CFG_RTRG_1 == 16)
#define TRTRG_1     (1<<6)
#else   /* == 1 */
#define TRTRG_1     (0<<6)
#endif

#if (CFG_TTRG_1 == 0)
#define TTTRG_1     (3<<4)
#elif (CFG_TTRG_1 == 2)
#define TTTRG_1     (2<<4)
#elif (CFG_TTRG_1 == 16)
#define TTTRG_1     (1<<4)
#else   /* == 32 */
#define TTTRG_1     (0<<4)
#endif

#if (CFG_TXBUF_SZ1==0)
#define _ddr_rzg1_scifa_tbuf1  0
#else
VB  _ddr_rzg1_scifa_tbuf1[CFG_TXBUF_SZ1];
#endif
VB  _ddr_rzg1_scifa_rbuf1[CFG_RXBUF_SZ1];
UB  _ddr_rzg1_scifa_sbuf1[CFG_RXBUF_SZ1];
T_RZG1_SCIFA_MNG _ddr_rzg1_scifa_data1;

const T_CDEV _ddr_rzg1_scifa_cdev1 = {&_ddr_rzg1_scifa_data1, (FP)_ddr_rzg1_scifadrv};
const T_CFLG _ddr_rzg1_scifa_cflg1 = {TA_TFIFO|TA_WMUL, 0x00000000};
const T_CISR _ddr_rzg1_scifa_cisr1 = {
    TA_HLNG|TA_FPU, (VP_INT)&_ddr_rzg1_scifa_data1, CFG_INT_1, (FP)_ddr_rzg1_scifa_intr, CFG_IPL_1
};

#endif


#ifdef CFG_SCIFA_2

#if (CFG_RSTRG_2 == 60)
#define TRSTRG_2    (7<<8)
#elif (CFG_RSTRG_2 == 54)
#define TRSTRG_2    (6<<8)
#elif (CFG_RSTRG_2 == 48)
#define TRSTRG_2    (5<<8)
#elif (CFG_RSTRG_2 == 32)
#define TRSTRG_2    (4<<8)
#elif (CFG_RSTRG_2 == 16)
#define TRSTRG_2    (3<<8)
#elif (CFG_RSTRG_2 == 8)
#define TRSTRG_2    (2<<8)
#elif (CFG_RSTRG_2 == 1)
#define TRSTRG_2    (1<<8)
#else   /* == 63 */
#define TRSTRG_2    (0<<8)
#endif

#if (CFG_RTRG_2 == 48)
#define TRTRG_2     (3<<6)
#elif (CFG_RTRG_2 == 32)
#define TRTRG_2     (2<<6)
#elif (CFG_RTRG_2 == 16)
#define TRTRG_2     (1<<6)
#else   /* == 1 */
#define TRTRG_2     (0<<6)
#endif

#if (CFG_TTRG_2 == 0)
#define TTTRG_2     (3<<4)
#elif (CFG_TTRG_2 == 2)
#define TTTRG_2     (2<<4)
#elif (CFG_TTRG_2 == 16)
#define TTTRG_2     (1<<4)
#else   /* == 32 */
#define TTTRG_2     (0<<4)
#endif

#if (CFG_TXBUF_SZ2==0)
#define _ddr_rzg1_scifa_tbuf2  0
#else
VB  _ddr_rzg1_scifa_tbuf2[CFG_TXBUF_SZ2];
#endif
VB  _ddr_rzg1_scifa_rbuf2[CFG_RXBUF_SZ2];
UB  _ddr_rzg1_scifa_sbuf2[CFG_RXBUF_SZ2];
T_RZG1_SCIFA_MNG _ddr_rzg1_scifa_data2;

const T_CDEV _ddr_rzg1_scifa_cdev2 = {&_ddr_rzg1_scifa_data2, (FP)_ddr_rzg1_scifadrv};
const T_CFLG _ddr_rzg1_scifa_cflg2 = {TA_TFIFO|TA_WMUL, 0x00000000};
const T_CISR _ddr_rzg1_scifa_cisr2 = {
    TA_HLNG|TA_FPU, (VP_INT)&_ddr_rzg1_scifa_data2, CFG_INT_2, (FP)_ddr_rzg1_scifa_intr, CFG_IPL_2
};

#endif


/**
 * Initializes device driver.
 */
ER _ddr_rzg1_scifa_init(ID devid, volatile struct t_scifa *scif_port)
{
    ER_ID ercd;
    UW val;

#ifdef CFG_SCIFA_0
    if (scif_port == &REG_SCIFA0) {
        memset(&_ddr_rzg1_scifa_data0, 0x00, sizeof(_ddr_rzg1_scifa_data0));
        _ddr_rzg1_scifa_data0.port = scif_port;
        _ddr_rzg1_scifa_data0.tbuf = _ddr_rzg1_scifa_tbuf0;
        _ddr_rzg1_scifa_data0.rbuf = _ddr_rzg1_scifa_rbuf0;
        _ddr_rzg1_scifa_data0.sbuf = _ddr_rzg1_scifa_sbuf0;
        _ddr_rzg1_scifa_data0.tsize = CFG_TXBUF_SZ0;
        _ddr_rzg1_scifa_data0.rsize = CFG_RXBUF_SZ0;
        _ddr_rzg1_scifa_data0.xoff_size = CFG_XOFF_SZ0;
        _ddr_rzg1_scifa_data0.xon_size = CFG_XON_SZ0;
        _ddr_rzg1_scifa_data0.aux[0] = TRSTRG_0 | TTTRG_0 | TRTRG_0;
        _ddr_rzg1_scifa_data0.devhdr = (FP)_ddr_rzg1_scifadrv;
        if ((ercd = acre_flg((T_CFLG *)&_ddr_rzg1_scifa_cflg0)) > E_OK) {
            _ddr_rzg1_scifa_data0.flgid = (UH)ercd;
            ercd = acre_isr((T_CISR *)&_ddr_rzg1_scifa_cisr0);
            if (ercd > E_OK) {
                _ddr_rzg1_scifa_data0.isrid = (UH)ercd;
                ercd = vdef_dev(devid, (VP)&_ddr_rzg1_scifa_cdev0);
                if (ercd >= E_OK) {
                    loc_cpu();
                    val = REG_CPG_MSR.MSTPSR2;
                    val &= ~MSTP2_SCIFA0;
                    REG_CPG_MSR.SMSTPCR2 = val;
                    do {
                    } while ((REG_CPG_MSR.SMSTPCR2 & MSTP2_SCIFA0) != 0);
                    /* software reset */
                    REG_CPG_MSR.SRCR2 |= MSTP2_SCIFA0;
                    val = REG_CPG_MSR.SRCR2;
                    REG_CPG_MSR.SRSTCLR2 = MSTP2_SCIFA0;
                    val = REG_CPG_MSR.SRSTCLR2;
                    REG_SCIFA0.SCR = 0;
                    val = REG_SCIFA0.SSR;
                    REG_SCIFA0.SSR = 0;
                    REG_SCIFA0.PCR = 0x0000;  /* all pin functions as a SCIFA_0 */
                    unl_cpu();
                    ena_int(CFG_INT_0);
                } else {
                    del_flg((ID)_ddr_rzg1_scifa_data0.flgid);
                    del_isr((ID)_ddr_rzg1_scifa_data0.isrid);
                }
            } else {
                del_flg((ID)_ddr_rzg1_scifa_data0.flgid);
            }
        }
    } else
#endif

#ifdef CFG_SCIFA_1
    if (scif_port == &REG_SCIFA1) {
        memset(&_ddr_rzg1_scifa_data1, 0x00, sizeof(_ddr_rzg1_scifa_data1));
        _ddr_rzg1_scifa_data1.port = scif_port;
        _ddr_rzg1_scifa_data1.tbuf = _ddr_rzg1_scifa_tbuf1;
        _ddr_rzg1_scifa_data1.rbuf = _ddr_rzg1_scifa_rbuf1;
        _ddr_rzg1_scifa_data1.sbuf = _ddr_rzg1_scifa_sbuf1;
        _ddr_rzg1_scifa_data1.tsize = CFG_TXBUF_SZ1;
        _ddr_rzg1_scifa_data1.rsize = CFG_RXBUF_SZ1;
        _ddr_rzg1_scifa_data1.xoff_size = CFG_XOFF_SZ1;
        _ddr_rzg1_scifa_data1.xon_size = CFG_XON_SZ1;
        _ddr_rzg1_scifa_data1.aux[0] = TRSTRG_1 | TTTRG_1 | TRTRG_1;
        _ddr_rzg1_scifa_data1.devhdr = (FP)_ddr_rzg1_scifadrv;
        if ((ercd = acre_flg((T_CFLG *)&_ddr_rzg1_scifa_cflg1)) > E_OK) {
            _ddr_rzg1_scifa_data1.flgid = (UH)ercd;
            ercd = acre_isr((T_CISR *)&_ddr_rzg1_scifa_cisr1);
            if (ercd > E_OK) {
                _ddr_rzg1_scifa_data1.isrid = (UH)ercd;
                ercd = vdef_dev(devid, (VP)&_ddr_rzg1_scifa_cdev1);
                if (ercd >= E_OK) {
                    loc_cpu();
                    val = REG_CPG_MSR.MSTPSR2;
                    val &= ~MSTP2_SCIFA1;
                    REG_CPG_MSR.SMSTPCR2 = val;
                    do {
                    } while ((REG_CPG_MSR.SMSTPCR2 & MSTP2_SCIFA1) != 0);
                    /* software reset */
                    REG_CPG_MSR.SRCR2 |= MSTP2_SCIFA1;
                    val = REG_CPG_MSR.SRCR2;
                    REG_CPG_MSR.SRSTCLR2 = MSTP2_SCIFA1;
                    val = REG_CPG_MSR.SRSTCLR2;
                    REG_SCIFA1.SCR = 0;
                    val = REG_SCIFA1.SSR;
                    REG_SCIFA1.SSR = 0;
                    REG_SCIFA1.PCR = 0x0000;  /* all pin functions as a SCIFA_1 */
                    unl_cpu();
                    ena_int(CFG_INT_1);
                } else {
                    del_flg((ID)_ddr_rzg1_scifa_data1.flgid);
                    del_isr((ID)_ddr_rzg1_scifa_data1.isrid);
                }
            } else {
                del_flg((ID)_ddr_rzg1_scifa_data1.flgid);
            }
        }
    } else
#endif


#ifdef CFG_SCIFA_2
    if (scif_port == &REG_SCIFA2) {
        memset(&_ddr_rzg1_scifa_data2, 0x00, sizeof(_ddr_rzg1_scifa_data2));
        _ddr_rzg1_scifa_data2.port = scif_port;
        _ddr_rzg1_scifa_data2.tbuf = _ddr_rzg1_scifa_tbuf2;
        _ddr_rzg1_scifa_data2.rbuf = _ddr_rzg1_scifa_rbuf2;
        _ddr_rzg1_scifa_data2.sbuf = _ddr_rzg1_scifa_sbuf2;
        _ddr_rzg1_scifa_data2.tsize = CFG_TXBUF_SZ2;
        _ddr_rzg1_scifa_data2.rsize = CFG_RXBUF_SZ2;
        _ddr_rzg1_scifa_data2.xoff_size = CFG_XOFF_SZ2;
        _ddr_rzg1_scifa_data2.xon_size = CFG_XON_SZ2;
        _ddr_rzg1_scifa_data2.aux[0] = TRSTRG_2 | TTTRG_2 | TRTRG_2;
        _ddr_rzg1_scifa_data2.devhdr = (FP)_ddr_rzg1_scifadrv;
        if ((ercd = acre_flg((T_CFLG *)&_ddr_rzg1_scifa_cflg2)) > E_OK) {
            _ddr_rzg1_scifa_data2.flgid = (UH)ercd;
            ercd = acre_isr((T_CISR *)&_ddr_rzg1_scifa_cisr2);
            if (ercd > E_OK) {
                _ddr_rzg1_scifa_data2.isrid = (UH)ercd;
                ercd = vdef_dev(devid, (VP)&_ddr_rzg1_scifa_cdev2);
                if (ercd >= E_OK) {
                    loc_cpu();
                    val = REG_CPG_MSR.MSTPSR2;
                    val &= ~MSTP2_SCIFA2;
                    REG_CPG_MSR.SMSTPCR2 = val;
                    do {
                    } while ((REG_CPG_MSR.SMSTPCR2 & MSTP2_SCIFA2) != 0);
                    /* software reset */
                    REG_CPG_MSR.SRCR2 |= MSTP2_SCIFA2;
                    val = REG_CPG_MSR.SRCR2;
                    REG_CPG_MSR.SRSTCLR2 = MSTP2_SCIFA2;
                    val = REG_CPG_MSR.SRSTCLR2;
                    REG_SCIFA2.SCR = 0;
                    val = REG_SCIFA2.SSR;
                    REG_SCIFA2.SSR = 0;
                    REG_SCIFA2.PCR = 0x0000;  /* all pin functions as a SCIFA_2 */
                    unl_cpu();
                    ena_int(CFG_INT_2);
                } else {
                    del_flg((ID)_ddr_rzg1_scifa_data2.flgid);
                    del_isr((ID)_ddr_rzg1_scifa_data2.isrid);
                }
            } else {
                del_flg((ID)_ddr_rzg1_scifa_data2.flgid);
            }
        }
    } else
#endif

        ercd = E_NOEXS;
    return ercd;
}

#if (defined(CFG_SCIFA_0) || defined(CFG_SCIFA_1) || defined(CFG_SCIFA_2))

/**
  * @brief Check receipt end.
  */
BOOL _ddr_rzg1_scifa_check_chr(T_COM_RCV *ReceiveData, VB chr, UB sts)
{
    BOOL flag;

    if ((sts & (T_COM_EROR|T_COM_ERP|T_COM_ERF|T_COM_BRK)) != 0) {
        flag = TRUE;
    } else if (ReceiveData->rcnt == 0) {
        flag = TRUE;
    } else if (ReceiveData->eos != 0) {
        if ((ReceiveData->eos->flg[0] != 0) && (ReceiveData->eos->chr[0] == chr)) {
            flag = TRUE;
        } else if ((ReceiveData->eos->flg[1] != 0) && (ReceiveData->eos->chr[1] == chr)) {
            flag = TRUE;
        } else if ((ReceiveData->eos->flg[2] != 0) && (ReceiveData->eos->chr[2] == chr)) {
            flag = TRUE;
        } else if ((ReceiveData->eos->flg[3] != 0) && (ReceiveData->eos->chr[3] == chr)) {
            flag = TRUE;
        } else {
            flag = FALSE;
        }
    } else {
        flag = FALSE;
    }
    return flag;
}

/**
  * @brief Analyze SCIF status.
  * @param[in] port
  */
UB _ddr_rzg1_scifa_getssr(volatile struct t_scifa *port)
{
    UH ssr;
    UB sts = 0;

    ssr = port->SSR;
    if ((ssr & SSR_PER) != 0) {
        sts |= T_COM_ERP;   /* Parity Error */
    }
    if ((ssr & SSR_FER) != 0) {
        sts |= T_COM_ERF;   /* Framing Error */
    }
    if ((ssr & SSR_BRK) != 0) {
        sts |= T_COM_BRK;   /* Break Detect */
        port->SSR = ~SSR_BRK;
    }
    return sts;
}

/**
  * @brief Copy buffer (from local to internal).
  * @param[in/out] pk_SCIFmng
  * @param[in] TransmiteData
  */
BOOL _ddr_rzg1_scifa_copy(T_RZG1_SCIFA_MNG *pk_SCIFmng, T_COM_SND *TransmiteData)
{
    for(; TransmiteData->tcnt != 0; ) {
        if (pk_SCIFmng->tcnt < pk_SCIFmng->tsize) {
            pk_SCIFmng->tbuf[pk_SCIFmng->sndp++] = *TransmiteData->tbuf++;
            TransmiteData->tcnt--;
            if (pk_SCIFmng->sndp >= pk_SCIFmng->tsize) {
                pk_SCIFmng->sndp = 0;
            }
            pk_SCIFmng->tcnt++;
        } else {
            break;
        }
    }
    return (TransmiteData->tcnt == 0) ? TRUE : FALSE;
}

/**
  * @brief Check XOFF
  * @param[in] pk_SCIFmng
  */
void _ddr_rzg1_scifa_chk_rxoff(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    if ((pk_SCIFmng->status.bit.rx_xoff == 0) &&
        (pk_SCIFmng->rcnt >= pk_SCIFmng->xoff_size)) {
        pk_SCIFmng->status.bit.rx_xoff = 1;
        if (((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < 64) {
            pk_SCIFmng->port->FTDR = XOFF;
        } else {
            loc_cpu();
            pk_SCIFmng->status.bit.req_xon_xoff = 1;
            pk_SCIFmng->port->SCR |= SCR_TIE;  /* TIE(Transmit-FIFO-data-empty interrupt request) enabled */
            unl_cpu();
        }
    }
}

/**
  * @brief Check XON
  * @param[in] pk_SCIFmng
  */
void _ddr_rzg1_scifa_chk_rxon(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    if ((pk_SCIFmng->status.bit.rx_xoff == 1) &&
        (pk_SCIFmng->rcnt <= pk_SCIFmng->xon_size)) {
        pk_SCIFmng->status.bit.rx_xoff = 0;
        if (((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < 64) {
            pk_SCIFmng->port->FTDR = XON;
        } else {
            loc_cpu();
            pk_SCIFmng->status.bit.req_xon_xoff = 1;
            pk_SCIFmng->port->SCR |= SCR_TIE;  /* TIE(Transmit-FIFO-data-empty interrupt request) enabled */
            unl_cpu();
        }
    }
}

/**
  * @brief Transmit data from local buffer(app buffer)
  * @param[in] pk_SCIFmng
  */
void _ddr_rzg1_scifa_send_local_buf(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    for(; ((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < 64; ) {
        pk_SCIFmng->port->FTDR = *pk_SCIFmng->SndData->tbuf++;
        if ((pk_SCIFmng->port->SSR & SSR_TDFE) != 0) {
            pk_SCIFmng->port->SSR = ~SSR_TDFE;  /* TDFE(Transmit FIFO Data Empty) clear to 0. */
        }
        if ((pk_SCIFmng->port->SSR & SSR_TEND) != 0) {
            pk_SCIFmng->port->SSR = ~SSR_TEND;  /* TEND(Transmit End) clear to 0. */
        }
        if (--pk_SCIFmng->SndData->tcnt == 0) {
            pk_SCIFmng->SndData = 0;
            pk_SCIFmng->port->SCR &= ~SCR_TIE; /* TIE (Transmit-FIFO-data-empty interrupt request) disabled */
            iset_flg((ID)pk_SCIFmng->flgid, TXI_FLG);
            break;
        }
    }
}

/**
  * @brief Transmit data from internal buffer
  * @param[in] pk_SCIFmng
  */
void _ddr_rzg1_scifa_send_drv_buf(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    UH sndp;

    sndp = pk_SCIFmng->sndp - pk_SCIFmng->tcnt;
    if (pk_SCIFmng->tcnt > pk_SCIFmng->sndp) {
        sndp += pk_SCIFmng->tsize;
    }
    for(; ((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < 64; ) {
        pk_SCIFmng->port->FTDR = pk_SCIFmng->tbuf[sndp];
        if ((pk_SCIFmng->port->SSR & SSR_TDFE) != 0) {
            pk_SCIFmng->port->SSR = ~SSR_TDFE; /* TDFE(Transmit FIFO Data Empty) clear to 0. */
        }
        if ((pk_SCIFmng->port->SSR & SSR_TEND) != 0) {
            pk_SCIFmng->port->SSR = ~SSR_TEND; /* TEND(Transmit End) clear to 0. */ 
        }
        if (++sndp >= pk_SCIFmng->tsize) {
            sndp = 0;
        }
        if (--pk_SCIFmng->tcnt == 0) {
            break;
        }
    }
    if (pk_SCIFmng->SndData != 0) {
        if (_ddr_rzg1_scifa_copy(pk_SCIFmng, pk_SCIFmng->SndData) == TRUE) {
            pk_SCIFmng->SndData = 0;
            iset_flg((ID)pk_SCIFmng->flgid, TXI_FLG);
        }
    }
    if ((pk_SCIFmng->tcnt == 0) && (pk_SCIFmng->SndData == 0)) {
        pk_SCIFmng->port->SCR &= ~SCR_TIE; /* TIE(Transmit-FIFO-data-empty interrupt request) disabled */
    }
}

/**
  * @brief BRI interrupt
  * @param[in] pk_SCIFmng
  */
void _ddr_rzg1_scifa_bri(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    UH rcvp;

    /* Overrun */
    if ((pk_SCIFmng->port->SSR & SSR_ORER) != 0) {
        pk_SCIFmng->port->SSR = ~SSR_ORER; /* ORER clear to 0 */
        pk_SCIFmng->status.bit.er_buf_ovr = 1;
    }

    /* Break */
    if ((pk_SCIFmng->port->SSR & SSR_BRK) != 0) {
        pk_SCIFmng->port->SSR = ~SSR_BRK; /* BRK clear to 0 */
        if (pk_SCIFmng->RcvData != 0) {
            if (pk_SCIFmng->RcvData->sbuf != 0) {
                pk_SCIFmng->RcvData->rcnt--;
                *pk_SCIFmng->RcvData->rbuf++ = 0;
                *pk_SCIFmng->RcvData->sbuf++ = (pk_SCIFmng->status.bit.er_buf_ovr == 0) ? T_COM_BRK : (T_COM_BRK | T_COM_EROVB);
                pk_SCIFmng->status.bit.er_buf_ovr = 0;
            }
        } else if (pk_SCIFmng->rcnt < pk_SCIFmng->rsize) {
            rcvp = pk_SCIFmng->rcvp + pk_SCIFmng->rcnt;
            if (rcvp >= pk_SCIFmng->rsize) {
                rcvp -= pk_SCIFmng->rsize;
            }
            if (pk_SCIFmng->sbuf != 0) {
                pk_SCIFmng->rbuf[rcvp] = 0;
                pk_SCIFmng->sbuf[rcvp] = (pk_SCIFmng->status.bit.er_buf_ovr == 0) ? T_COM_BRK : (T_COM_BRK | T_COM_EROVB);
                pk_SCIFmng->status.bit.er_buf_ovr = 0;
                if (pk_SCIFmng->status.bit.sft_flw == 1) {
                    _ddr_rzg1_scifa_chk_rxoff(pk_SCIFmng);
                }
                if (++pk_SCIFmng->rcnt == pk_SCIFmng->rsize) {
                    pk_SCIFmng->port->SCR &= ~SCR_RIE; /* RIE(Receive-FIFO-data-full interrupt request) disabled */
                    pk_SCIFmng->port->SCR &= ~SCR_ERIE;
                    pk_SCIFmng->port->SCR &= ~SCR_BRIE;
                    pk_SCIFmng->port->SCR &= ~SCR_DRIE;
                }
            }
        }
    }
}

/**
  * @brief TXI interrupt
  * @param[in] pk_SCIFmng
  */
void _ddr_rzg1_scifa_txi(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    if (pk_SCIFmng->status.bit.req_xon_xoff == 1) {
        pk_SCIFmng->port->FTDR = (pk_SCIFmng->status.bit.rx_xoff == 0) ? XON : XOFF;
        if ((pk_SCIFmng->port->SSR & SSR_TDFE) != 0) {
            pk_SCIFmng->port->SSR = ~SSR_TDFE; /* TDFE(Transmit FIFO Data Empty) clear to 0 */
        }
        if ((pk_SCIFmng->port->SSR & SSR_TEND) != 0) {
            pk_SCIFmng->port->SSR = ~SSR_TEND; /* TEND(Transmit End) clear to 0 */
        }
        pk_SCIFmng->status.bit.req_xon_xoff = 0;
    }
    if (pk_SCIFmng->status.bit.tx_xoff == 0) {
        if (pk_SCIFmng->tsize == 0) {
            if (pk_SCIFmng->SndData != 0) {
                _ddr_rzg1_scifa_send_local_buf(pk_SCIFmng);
            } else {
                pk_SCIFmng->port->SCR &= ~SCR_TIE; /* TIE(Transmit-FIFO-data-empty interrupt request) disabled */
            }
        } else {
            if (pk_SCIFmng->tcnt != 0) {
                _ddr_rzg1_scifa_send_drv_buf(pk_SCIFmng);
            } else {
                pk_SCIFmng->port->SCR &= ~SCR_TIE; /* TIE(Transmit-FIFO-data-empty interrupt request) disabled */
            }
       }
    } else {
        pk_SCIFmng->port->SCR &= ~SCR_TIE; /* TIE(Transmit-FIFO-data-empty interrupt request) disabled */
    }

    if ((pk_SCIFmng->tcnt == 0) && (pk_SCIFmng->aux[1] != 0)) {
        iset_flg((ID)pk_SCIFmng->flgid, TEI_FLG);
        pk_SCIFmng->aux[1] = 0;
    }
}

/**
  * @brief RXI, DRI, ERI interrupt
  * @param[in] pk_SCIFmng
  */
void _ddr_rzg1_scifa_rxi(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    UH rcvp;
    VB chr;
    UB sts;

    if ((pk_SCIFmng->rcnt < pk_SCIFmng->rsize) ||
        (pk_SCIFmng->RcvData != 0)) {
        for(; (pk_SCIFmng->port->FDR & FTRDR_MASK) != 0; ) {
            sts = _ddr_rzg1_scifa_getssr(pk_SCIFmng->port);
            chr = pk_SCIFmng->port->FRDR;
            if ((pk_SCIFmng->status.bit.sft_flw == 1) && (chr == XON)) {
                pk_SCIFmng->status.bit.tx_xoff = 0;
                pk_SCIFmng->port->SCR |= SCR_TIE; /* TIE(Transmit-FIFO-data-empty interrupt request) enabled */
                if (((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < 64) {
                    _ddr_rzg1_scifa_txi(pk_SCIFmng);
                }
            } else if ((pk_SCIFmng->status.bit.sft_flw == 1) && (chr == XOFF)) {
                pk_SCIFmng->status.bit.tx_xoff = 1;
                pk_SCIFmng->port->SCR &= ~SCR_TIE; /* TIE(Transmit-FIFO-data-empty interrupt request) disabled */
            } else {
                if (pk_SCIFmng->RcvData != 0) {
                    pk_SCIFmng->RcvData->rcnt--;
                    *pk_SCIFmng->RcvData->rbuf++ = chr;
                    if (pk_SCIFmng->RcvData->sbuf != 0) {
                        *pk_SCIFmng->RcvData->sbuf++ = (pk_SCIFmng->status.bit.er_buf_ovr == 0) ? sts : (sts | T_COM_EROVB);
                    }
                    pk_SCIFmng->status.bit.er_buf_ovr = 0;
                    if (_ddr_rzg1_scifa_check_chr(pk_SCIFmng->RcvData, chr, sts) == TRUE) {
                        pk_SCIFmng->RcvData = 0;
                        iset_flg((ID)pk_SCIFmng->flgid, RXI_FLG);
                    }
                } else if (pk_SCIFmng->rcnt < pk_SCIFmng->rsize) {
                    rcvp = pk_SCIFmng->rcvp + pk_SCIFmng->rcnt;
                    if (rcvp >= pk_SCIFmng->rsize) {
                        rcvp -= pk_SCIFmng->rsize;
                    }
                    pk_SCIFmng->rbuf[rcvp] = chr;
                    if (pk_SCIFmng->sbuf != 0) {
                        pk_SCIFmng->sbuf[rcvp] = (pk_SCIFmng->status.bit.er_buf_ovr == 0) ? sts : (sts | T_COM_EROVB);
                    }
                    pk_SCIFmng->status.bit.er_buf_ovr = 0;
                    if (pk_SCIFmng->status.bit.sft_flw == 1) {
                        _ddr_rzg1_scifa_chk_rxoff(pk_SCIFmng);
                    }
                    if (++pk_SCIFmng->rcnt == pk_SCIFmng->rsize) {
                        pk_SCIFmng->port->SCR &= ~SCR_RIE; /* RIE( Receive-FIFO-data-full interrupt request) disabled */
                        pk_SCIFmng->port->SCR &= ~SCR_ERIE;
                        pk_SCIFmng->port->SCR &= ~SCR_BRIE;
                        pk_SCIFmng->port->SCR &= ~SCR_DRIE;
                        break;
                    }
                }
            }
        }

        /* Check RIE */
        if ((pk_SCIFmng->port->SCR & SCR_RIE) != 0) {
            if ((pk_SCIFmng->port->SSR & SSR_RDF) != 0) {
                pk_SCIFmng->port->SSR = ~SSR_RDF; /* RDF clear to 0 */
            }
            if ((pk_SCIFmng->port->SSR & SSR_DR) != 0) {
                pk_SCIFmng->port->SSR = ~SSR_DR; /* DR clear to 0 */
            }
        }

    } else {
        pk_SCIFmng->port->SCR &= ~SCR_RIE; /* RIE(Receive-FIFO-data-full interrupt request) disabled */
        pk_SCIFmng->port->SCR &= ~SCR_ERIE;
        pk_SCIFmng->port->SCR &= ~SCR_BRIE;
        pk_SCIFmng->port->SCR &= ~SCR_DRIE;
    }

    if ((pk_SCIFmng->port->SSR & SSR_ER) != 0) {
        pk_SCIFmng->port->SSR = ~SSR_ER;
    }
}

/**
  * SCIFA Interrupt service routine.
  */
void _ddr_rzg1_scifa_intr(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    /* RDF, DR, ER */
    if ((pk_SCIFmng->port->SSR & SSR_RDF) != 0 || 
        (pk_SCIFmng->port->SSR & SSR_DR) != 0 ||
        (pk_SCIFmng->port->SSR & SSR_ER) != 0) {
        _ddr_rzg1_scifa_rxi(pk_SCIFmng);
    }
    /* ORER, BRK */
    if ((pk_SCIFmng->port->SSR & SSR_BRK) != 0 ||
        (pk_SCIFmng->port->SSR & SSR_ORER) != 0) {
        _ddr_rzg1_scifa_bri(pk_SCIFmng);
    }
    /* TDFE */
    if ((pk_SCIFmng->port->SSR & SSR_TDFE) != 0) {
        _ddr_rzg1_scifa_txi(pk_SCIFmng);
    }
}

/**
  * @brief Fills transmit FIFO.
  * @param[in] pk_SCIFmng
  * @param[in] TransmiteData
  */
void _ddr_rzg1_scifa_send_char(T_RZG1_SCIFA_MNG *pk_SCIFmng, T_COM_SND *TransmiteData)
{
    if (pk_SCIFmng->status.bit.req_xon_xoff == 1) {
        pk_SCIFmng->port->FTDR = (pk_SCIFmng->status.bit.rx_xoff == 0) ? XON : XOFF;
        if ((pk_SCIFmng->port->SSR & SSR_TDFE) != 0) {
            pk_SCIFmng->port->SSR = ~SSR_TDFE; /* TDFE clear to 0 */
        }
        pk_SCIFmng->status.bit.req_xon_xoff = 0;
    }
    if (pk_SCIFmng->status.bit.tx_xoff == 0) {
        for(; ((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < 64; ) {
            pk_SCIFmng->port->FTDR = *TransmiteData->tbuf++;
            if ((pk_SCIFmng->port->SSR & SSR_TDFE) != 0) {
                pk_SCIFmng->port->SSR = ~SSR_TDFE; /* TDFE clear to 0 */
            }
            if ((pk_SCIFmng->port->SSR & SSR_TEND) != 0) {
                pk_SCIFmng->port->SSR = ~SSR_TEND; /* TEND clear to 0 */ 
            }
            if (--TransmiteData->tcnt == 0) {
                break;
            }
        }
    }
}

/**
  * @brief Get strings
  * @param[in] pk_SCIFmng
  * @param[in/out] ReceiveData
  */
BOOL _ddr_rzg1_scifa_recv_strings(T_RZG1_SCIFA_MNG *pk_SCIFmng, T_COM_RCV *ReceiveData)
{
    BOOL flag;
    VB chr;
    UB sts;

    for (flag = FALSE; flag == FALSE; ) {
        if (ReceiveData->rcnt == 0) {
            flag = TRUE;
        } else if (pk_SCIFmng->rcnt == 0) {
            break;
        } else {
            *ReceiveData->rbuf++ = chr = pk_SCIFmng->rbuf[pk_SCIFmng->rcvp];
            sts = pk_SCIFmng->sbuf[pk_SCIFmng->rcvp];
            if (ReceiveData->sbuf != 0) {
                *ReceiveData->sbuf++ = sts;
            }
            if (++pk_SCIFmng->rcvp >= pk_SCIFmng->rsize) {
                pk_SCIFmng->rcvp = 0;
            }
            pk_SCIFmng->rcnt--;
            ReceiveData->rcnt--;
            if (pk_SCIFmng->status.bit.sft_flw == 1) {
                _ddr_rzg1_scifa_chk_rxon(pk_SCIFmng);
            }
            flag = _ddr_rzg1_scifa_check_chr(ReceiveData, chr, sts);
        }
    }
    return flag;
}

/**
  * @brief Transmit data
  * @param[in] TransmiteData
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_snd(T_COM_SND *TransmiteData, T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    FLGPTN flgptn;
    ID tskid;
    ER ercd = E_OK;

    if ((pk_SCIFmng->status.bit.init_flg == 0) || (sns_dpn() == TRUE))
        ercd = E_OBJ;
    else {
        get_tid(&tskid);
        dis_dsp();
        if (((pk_SCIFmng->tlockid == 0) || (pk_SCIFmng->tlockid == (UH)tskid)) &&
            (pk_SCIFmng->status.bit.ena_tx == 1) &&
            (pk_SCIFmng->SndData == 0)) {
            loc_cpu();
            pk_SCIFmng->port->SCR &= ~(SCR_TIE | SCR_RIE); /* disable TIE, RIE */
            pk_SCIFmng->port->SCR &= ~SCR_ERIE;
            pk_SCIFmng->port->SCR &= ~SCR_BRIE;
            pk_SCIFmng->port->SCR &= ~SCR_DRIE;
            unl_cpu();
            if ((pk_SCIFmng->tcnt == 0) && (((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < 64)) {
                _ddr_rzg1_scifa_send_char(pk_SCIFmng, TransmiteData);
            }
            loc_cpu();
            pk_SCIFmng->port->SCR |= SCR_RIE; /* enable RIE */
            pk_SCIFmng->port->SCR |= SCR_ERIE;
            pk_SCIFmng->port->SCR |= SCR_BRIE;
            pk_SCIFmng->port->SCR |= SCR_DRIE;
            unl_cpu();
            if (_ddr_rzg1_scifa_copy(pk_SCIFmng, TransmiteData) == FALSE) {
                pk_SCIFmng->SndData = TransmiteData;
                loc_cpu();
                pk_SCIFmng->port->SCR |= SCR_TIE; /* enable TIE */
                unl_cpu();
                ena_dsp();
                if ((ercd = twai_flg((ID)pk_SCIFmng->flgid, TXI_FLG, TWF_ORW, &flgptn, TransmiteData->time)) != E_OK) {
                    loc_cpu();
                    pk_SCIFmng->port->SCR &= ~SCR_TIE; /* disable TIE */
                    unl_cpu();
                    pk_SCIFmng->SndData = 0;
                    ercd = pol_flg((ID)pk_SCIFmng->flgid, TXI_FLG, TWF_ORW, &flgptn);
                }
                clr_flg((ID)pk_SCIFmng->flgid, ~TXI_FLG);
            } else {
                if (pk_SCIFmng->tcnt != 0) {
                    loc_cpu();
                    pk_SCIFmng->port->SCR |= SCR_TIE; /* enable TIE */
                    unl_cpu();
                }
                ena_dsp();
            }
        } else {
            ena_dsp();
            ercd = E_OBJ;
        }
    }
    return ercd;
}

/**
  * @brief Receive data
  * @param[in/out] ReceiveData
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_rcv(T_COM_RCV *ReceiveData, T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    FLGPTN flgptn;
    ID tskid;
    ER ercd = E_OK;

    if ((pk_SCIFmng->status.bit.init_flg == 0) || (sns_dpn() == TRUE) ||
        (pk_SCIFmng->status.bit.ena_rx   == 0)) {
        ercd = E_OBJ;
    } else {
        get_tid(&tskid);
        dis_dsp();
        if (((pk_SCIFmng->rlockid == 0) || (pk_SCIFmng->rlockid == (UH)tskid)) &&
            (pk_SCIFmng->RcvData        == 0)) {
            loc_cpu();
            pk_SCIFmng->port->SCR &= ~SCR_RIE; /* disable RIE */
            pk_SCIFmng->port->SCR &= ~SCR_ERIE;
            pk_SCIFmng->port->SCR &= ~SCR_BRIE;
            pk_SCIFmng->port->SCR &= ~SCR_DRIE;
            unl_cpu();
            if (_ddr_rzg1_scifa_recv_strings(pk_SCIFmng, ReceiveData) == FALSE) {
                pk_SCIFmng->RcvData = ReceiveData;
                loc_cpu();
                pk_SCIFmng->port->SCR |= SCR_RIE; /* enable RIE */
                pk_SCIFmng->port->SCR |= SCR_ERIE;
                pk_SCIFmng->port->SCR |= SCR_BRIE;
                pk_SCIFmng->port->SCR |= SCR_DRIE;
                unl_cpu();
                ena_dsp();
                if ((ercd = twai_flg((ID)pk_SCIFmng->flgid, RXI_FLG, TWF_ORW, &flgptn, ReceiveData->time)) != E_OK) {
                    pk_SCIFmng->RcvData = 0;
                    ercd = pol_flg((ID)pk_SCIFmng->flgid, RXI_FLG, TWF_ORW, &flgptn);
                }
                clr_flg((ID)pk_SCIFmng->flgid, ~RXI_FLG);
            } else {
                loc_cpu();
                pk_SCIFmng->port->SCR |= SCR_RIE; /* enable RIE */
                pk_SCIFmng->port->SCR |= SCR_ERIE;
                pk_SCIFmng->port->SCR |= SCR_BRIE;
                pk_SCIFmng->port->SCR |= SCR_DRIE;
                unl_cpu();
                ena_dsp();
            }
        } else {
            ena_dsp();
            ercd = E_OBJ;
        }
    }
    return ercd;
}

/**
  * @brief Transmit data from transmit FIFO.
  * @param[in] pk_SCIFmng
  * @param[in] time
  */
ER _ddr_rzg1_scifa_cln_tx_buf(T_RZG1_SCIFA_MNG *pk_SCIFmng, TMO time)
{
    FLGPTN flgptn;
    ID tskid;
    ER ercd;

    get_tid(&tskid);
    if (((pk_SCIFmng->tlockid == 0) ||
         (pk_SCIFmng->tlockid == (UH)tskid)) &&
        ( pk_SCIFmng->SndData == 0)) {
        loc_cpu();
        if (pk_SCIFmng->tcnt != 0) {
            pk_SCIFmng->port->SCR |= SCR_TIE; /* enable TIE */
            pk_SCIFmng->aux[1] = tskid;
            unl_cpu();
            if ((ercd = twai_flg((ID)pk_SCIFmng->flgid, TEI_FLG, TWF_ORW, &flgptn, time)) != E_OK) {
                pk_SCIFmng->aux[1] = 0;
                ercd = pol_flg((ID)pk_SCIFmng->flgid, TEI_FLG, TWF_ORW, &flgptn);
            }
            clr_flg((ID)pk_SCIFmng->flgid, ~TEI_FLG);
        } else {
            unl_cpu();
            ercd = E_OK;
        }
        /* Wait for until FIFO empty */
        for(; ((pk_SCIFmng->port->SSR & SSR_TEND) == 0) && (ercd == E_OK); ) {
            ercd = dly_tsk(0);
        }
    } else {
        ercd = E_OBJ;
    }
    return ercd;
}

/**
  * @brief Reset transmit FIFO data.
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_rst_tx_buf(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    ER ercd;
    ID tskid;

    get_tid(&tskid);
    if (((pk_SCIFmng->tlockid == 0) ||
         (pk_SCIFmng->tlockid == (UH)tskid)) &&
        ( pk_SCIFmng->SndData == 0)) {
        loc_cpu();
        pk_SCIFmng->port->FCR |= FCR_TFRST;
        pk_SCIFmng->port->FCR &= ~FCR_TFRST;
        pk_SCIFmng->tcnt = 0;
        unl_cpu();
        ercd = E_OK;
    } else {
        ercd = E_OBJ;
    }
    return ercd;
}

/**
  * @brief Reset receive FIFO data.
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_rst_rx_buf(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    ER ercd;
    ID tskid;

    get_tid(&tskid);
    if (((pk_SCIFmng->rlockid == 0) ||
         (pk_SCIFmng->rlockid == (UH)tskid)) &&
        ( pk_SCIFmng->RcvData == 0)) {
        loc_cpu();
        pk_SCIFmng->port->FCR |= FCR_RFRST;
        pk_SCIFmng->port->FCR &= ~FCR_RFRST;
        pk_SCIFmng->rcnt = 0;
        unl_cpu();
        ercd = E_OK;
    } else {
        ercd = E_OBJ;
    }
    return ercd;
}

/**
  * @brief Disable transmit operation.
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_dis_send(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    ER ercd;
    ID tskid;

    get_tid(&tskid);
    if (((pk_SCIFmng->tlockid == 0) ||
         (pk_SCIFmng->tlockid == (UH)tskid)) &&
        ( pk_SCIFmng->SndData == 0) &&
        ( pk_SCIFmng->tcnt    == 0)) {
        loc_cpu();
        pk_SCIFmng->status.bit.ena_tx = 0;
        unl_cpu();
        ercd = E_OK;
    } else {
        ercd = E_OBJ;
    }
    return ercd;
}

/**
  * @brief Disable recevie operation.
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_dis_rcv(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    ER ercd;
    ID tskid;

    get_tid(&tskid);
    if (((pk_SCIFmng->rlockid == 0) ||
         (pk_SCIFmng->rlockid == (UH)tskid)) &&
        ( pk_SCIFmng->RcvData == 0) &&
        ( pk_SCIFmng->rcnt    == 0) &&
        ((pk_SCIFmng->port->FDR & FTRDR_MASK) != 0)) {
        loc_cpu();
        pk_SCIFmng->status.bit.ena_rx = 0;
        pk_SCIFmng->port->SCR &= ~(SCR_RIE | SCR_RE); /* disable RIE and RE */
        pk_SCIFmng->port->SCR &= ~SCR_ERIE;
        pk_SCIFmng->port->SCR &= ~SCR_BRIE;
        pk_SCIFmng->port->SCR &= ~SCR_DRIE;
        unl_cpu();
        ercd = E_OK;
    } else {
        ercd = E_OBJ;
    }
    return ercd;
}

/**
  * @brief Enable transmit operation.
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_ena_send(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    ER ercd;
    ID tskid;

    get_tid(&tskid);
    if ((pk_SCIFmng->rlockid == 0) ||
        (pk_SCIFmng->rlockid == (UH)tskid)) {
        loc_cpu();
        pk_SCIFmng->status.bit.ena_tx = 1;
        pk_SCIFmng->port->SCR |= SCR_TE;
        unl_cpu();
        ercd = E_OK;
    } else {
        ercd = E_OBJ;
    }
    return ercd;
}

/**
  * @brief Enable receive operation.
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_ena_rcv(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    ER ercd;
    ID tskid;

    get_tid(&tskid);
    if ((pk_SCIFmng->rlockid == 0) ||
        (pk_SCIFmng->rlockid == (UH)tskid)) {
        loc_cpu();
        pk_SCIFmng->status.bit.ena_rx = 1;
        pk_SCIFmng->port->SCR |= (SCR_RIE | SCR_RE); /* enable RIE, RE */
        pk_SCIFmng->port->SCR |= SCR_ERIE;
        pk_SCIFmng->port->SCR |= SCR_BRIE;
        pk_SCIFmng->port->SCR |= SCR_DRIE;
        unl_cpu();
        ercd = E_OK;
    } else {
        ercd = E_OBJ;
    }
    return ercd;
}

/**
  * @brief Send break signal.
  * @param[in] pk_SCIFmng
  * @param[in] time
  */
ER _ddr_rzg1_scifa_snd_brk(T_RZG1_SCIFA_MNG *pk_SCIFmng, TMO time)
{
    ER ercd;
    ID tskid;

    ercd = E_PAR;
    get_tid(&tskid);
    if (((pk_SCIFmng->tlockid == 0) ||
         (pk_SCIFmng->tlockid == (UH)tskid)) &&
        ( pk_SCIFmng->SndData == 0) &&
        ( pk_SCIFmng->tcnt    == 0)) {
        if ((pk_SCIFmng->port->SSR & SSR_TEND) != 0) {
            loc_cpu();
            pk_SCIFmng->port->PDR |= 0x0001; /* TXD pin is High */
            pk_SCIFmng->port->SCR &= ~SCR_TE;
            unl_cpu();
            dly_tsk(time);
            loc_cpu();
            pk_SCIFmng->port->PDR &= ~0x0001; /* TXD pin is Low */
            pk_SCIFmng->port->SCR |= SCR_TE;
            unl_cpu();
            ercd = E_OK;
        } else {
            ercd = E_OBJ;
        }
    } else {
        ercd = E_OBJ;
    }

    return ercd;
}

/**
  * @brief Lock transmit channel.
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_lock_trans(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    ER ercd;
    ID tskid;

    get_tid(&tskid);
    loc_cpu();
    if ((pk_SCIFmng->tlockid == 0) ||
        (pk_SCIFmng->SndData == 0)) {
        pk_SCIFmng->tlockid = (UH)tskid;
        ercd = E_OK;
    } else if (pk_SCIFmng->tlockid == (UH)tskid) {
        ercd = E_OK;
    } else {
        ercd = E_OBJ;
    }
    unl_cpu();
    return ercd;
}

/**
  * @brief Lock receive channel.
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_lock_recv(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    ER ercd;
    ID tskid;

    get_tid(&tskid);
    loc_cpu();
    if ((pk_SCIFmng->rlockid == 0) ||
        (pk_SCIFmng->RcvData == 0)) {
        pk_SCIFmng->rlockid = (UH)tskid;
        ercd = E_OK;
    } else if (pk_SCIFmng->rlockid == (UH)tskid) {
        ercd = E_OK;
    } else {
        ercd = E_OBJ;
    }
    unl_cpu();
    return ercd;
}

/**
  * @brief Unlock transmit channel.
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_unlock_trans(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    ER ercd;
    ID tskid;

    get_tid(&tskid);
    if (pk_SCIFmng->tlockid == (UH)tskid) {
        pk_SCIFmng->tlockid = 0;
        ercd = E_OK;
    } else if (pk_SCIFmng->tlockid == 0) {
        ercd = E_OK;
    } else {
        ercd = E_OBJ;
    }
    return ercd;
}

/**
  * @brief Unlock receive channel.
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_unlock_recv(T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    ER ercd;
    ID tskid;

    get_tid(&tskid);
    if (pk_SCIFmng->rlockid == (UH)tskid) {
        pk_SCIFmng->rlockid = 0;
        ercd = E_OK;
    } else if (pk_SCIFmng->rlockid == 0) {
        ercd = E_OK;
    } else {
        ercd = E_OBJ;
    }
    return ercd;
}

/**
  * @brief Control SCIF signal.
  * @param[in] pk_SerialData
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_ctr(const T_COM_CTR *pk_SerialData, T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    ER ercd = E_OK;

    if (pk_SCIFmng->status.bit.init_flg == 0) {
        ercd = E_OBJ;
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & CLN_TXBUF) == CLN_TXBUF) {
            ercd = _ddr_rzg1_scifa_cln_tx_buf(pk_SCIFmng, pk_SerialData->time);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & RST_TXBUF) == RST_TXBUF) {
            ercd = _ddr_rzg1_scifa_rst_tx_buf(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & RST_RXBUF) == RST_RXBUF) {
            ercd = _ddr_rzg1_scifa_rst_rx_buf(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & STP_TX) == STP_TX) {
            ercd = _ddr_rzg1_scifa_dis_send(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & STP_RX) == STP_RX) {
            ercd = _ddr_rzg1_scifa_dis_rcv(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & STA_TX) == STA_TX) {
            ercd = _ddr_rzg1_scifa_ena_send(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & STA_RX) == STA_RX) {
            ercd = _ddr_rzg1_scifa_ena_rcv(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & SND_BRK) == SND_BRK) {
            ercd = _ddr_rzg1_scifa_snd_brk(pk_SCIFmng, pk_SerialData->time);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & LOC_TX) == LOC_TX) {
            ercd = _ddr_rzg1_scifa_lock_trans(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & LOC_RX) == LOC_RX) {
            ercd = _ddr_rzg1_scifa_lock_recv(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & UNL_TX) == UNL_TX) {
            ercd = _ddr_rzg1_scifa_unlock_trans(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & UNL_RX) == UNL_RX) {
            ercd = _ddr_rzg1_scifa_unlock_recv(pk_SCIFmng);
        }
    }

    return ercd;
}

/**
  * @brief Reference to serial port status.
  * @param[in/out] pk_SerialRef
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifa_ref(T_COM_REF *pk_SerialRef, T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    UH status = 0;

    if (pk_SCIFmng->status.bit.init_flg == 1) {
        loc_cpu();
        pk_SerialRef->rxcnt = pk_SCIFmng->rcnt;
        pk_SerialRef->rxcnt += (pk_SCIFmng->port->FDR & FTRDR_MASK);
        pk_SerialRef->txcnt = pk_SCIFmng->tcnt;
        pk_SerialRef->txcnt += ((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK);
        unl_cpu();

        status |= T_COM_DSR|T_COM_INIT;
        if ((pk_SCIFmng->port->PDR & 0x0008) != 0) {
            status |= T_COM_CTS;
        }
        if (pk_SCIFmng->status.bit.ena_tx == 1) {
            status |= T_COM_ENATX;
        }
        if (pk_SCIFmng->status.bit.ena_rx == 1) {
            status |= T_COM_ENARX;
        }
        if (pk_SCIFmng->status.bit.rx_xoff == 1) {
            status |= T_COM_RXOFF;
        }
        if (pk_SCIFmng->status.bit.tx_xoff == 1) {
            status |= T_COM_TXOFF;
        }
    }
    pk_SerialRef->status = status;
    return E_OK;
}

/**
  * Initializes the UART function of the SCIFA.
  */
ER _ddr_rzg1_scifa_ini(const T_COM_SMOD *pk_SerialMode, T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    UW brr = 0;
    UW freq = 0;
    ER ercd = E_OK;
    T_RZG1_SCIFA_MSTS status;
    UH smr = 0;
    UH fcr = 0;
    volatile int i;

    pk_SCIFmng->port->SCR = 0;
    pk_SCIFmng->port->FCR = 0x0006; /* Reset FTDR/FRDR */
    status.word = 0;
    fcr = pk_SCIFmng->aux[0];

    /* 
     * The SCABRR setting is found by using the following equations.
     * sampling rate is 1/16
     * N = (MP / (16 x 2^2n x B)) x 10^6 - 1 
     */
    freq = CFG_MP_CLK;
    for (smr = 0; smr < 4; smr++) {
        brr = freq / (8 * pk_SerialMode->baud) + 1;
        if (brr <= 512)
            break;
        freq /= 4;
    }
    if (smr == 4) {
        ercd = E_PAR;
    }

    switch(pk_SerialMode->blen) {
        case BLEN7:
            smr |= 0x40;
            break;
        case BLEN8:
            break;
        default:
            ercd = E_PAR;
            break;
    }

    switch(pk_SerialMode->par) {
        case PAR_ODD:
            smr |= 0x30;
            break;
        case PAR_EVEN:
            smr |= 0x20;
            break;
        default:
            break;
    }

    switch(pk_SerialMode->sbit) {
        case SBIT2:
            smr |= 0x08;
            break;
        case SBIT1:
            break;
        default:
            ercd = E_PAR;
            break;
    }

    switch (pk_SerialMode->flow) {
        case FLW_HARD:
            fcr |= 0x0008;
            status.bit.hrd_flw = 1;
            break;
        case FLW_XON:
            status.bit.sft_flw = 1;
            break;
        case FLW_NONE:
            break;
        default:
            ercd = E_PAR;
            break;
    }

    if (ercd == E_OK) {
        status.bit.init_flg = 1;
        pk_SCIFmng->status.word = status.word;
        pk_SCIFmng->port->SMR = smr;    /* Setting for Asynchronous mode */
        pk_SCIFmng->port->BRR = (UB)(brr/2-1);
        /* wait */
        if (sns_dpn() == TRUE) {
            for (i = 0; i < 10000; i++) ;
        } else {
            dly_tsk(1);
        }
        pk_SCIFmng->port->FCR = fcr;
    }
    return ercd;
}

/**
  * @brief Driver main entry
  * @param[in] FuncID
  * @param[in] pk_ControlData
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scifadrv(ID FuncID, VP pk_ControlData, T_RZG1_SCIFA_MNG *pk_SCIFmng)
{
    ER ercd;

    switch(FuncID) {
        case TA_COM_INI:
            ercd = _ddr_rzg1_scifa_ini((const T_COM_SMOD *)pk_ControlData, pk_SCIFmng);
            break;
        case TA_COM_REF:
            ercd = _ddr_rzg1_scifa_ref((T_COM_REF *)pk_ControlData, pk_SCIFmng);
            break;
        case TA_COM_CTR:
            ercd = _ddr_rzg1_scifa_ctr((const T_COM_CTR *)pk_ControlData, pk_SCIFmng);
            break;
        case TA_COM_PUTS:
            ercd = _ddr_rzg1_scifa_snd((T_COM_SND *)pk_ControlData, pk_SCIFmng);
            break;
        case TA_COM_GETS:
            ercd = _ddr_rzg1_scifa_rcv((T_COM_RCV *)pk_ControlData, pk_SCIFmng);
            break;
        default: 
            ercd = E_NOSPT;
            break;
    }
    return ercd;
}
#endif
