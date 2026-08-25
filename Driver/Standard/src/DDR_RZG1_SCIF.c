/**
 * @file    DDR_RZG1_SCIF.c
 * @brief   RZ/G1 Serial communication interface with FIFO (SCIF)
 * @date    2016.05.12
 * @author  Copyright (c) 2016, eForce Co.,Ltd.  All rights reserved.
 *
 ********************************************************************
 * @par     History
 *          - rev 1.0 (2016.05.12) i-cho
 *            Initial version.
 ********************************************************************
 */
#include "kernel.h"

#include "DDR_COM.h"
#include "DDR_RZG1_SCIF.h"
#include "commondef.h"
/* User config header */
#include "DDR_RZG1_SCIF_cfg.h"
#include "RZG1_UC3.h"

#include <string.h>

/* External function prototypes -----------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void _ddr_rzg1_scif_intr(T_RZG1_SCIF_MNG *);
void _ddr_rzg1_scif_rxi(T_RZG1_SCIF_MNG *);
void _ddr_rzg1_scif_bri(T_RZG1_SCIF_MNG *);
void _ddr_rzg1_scif_txi(T_RZG1_SCIF_MNG *);
ER _ddr_rzg1_scifdrv(ID, VP, T_RZG1_SCIF_MNG *);


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define MSTP7_SCIF0  (1U << 21)
#define MSTP7_SCIF1  (1U << 20)
#define MSTP7_SCIF2  (1U << 19)   /* MN, E */
#define MSTP3_SCIF2  (1U << 10)   /* H */
#define MSTP7_SCIF3  (1U << 18)
#define MSTP7_SCIF4  (1U << 15)
#define MSTP7_SCIF5  (1U << 14)

/* Serial Status Register (SCFSRn) */
#define FSR_ER      ((UH)BIT7)  /* Receive Error */
#define FSR_TEND    ((UH)BIT6)  /* Transmission End */
#define FSR_TDFE    ((UH)BIT5)  /* Transmit FIFO Data Empty */
#define FSR_BRK     ((UH)BIT4)  /* Break Detect */
#define FSR_FER     ((UH)BIT3)  /* Framing Error */
#define FSR_PER     ((UH)BIT2)  /* Parity Error */
#define FSR_RDF     ((UH)BIT1)  /* Receive FIFO Data Full */
#define FSR_DR      ((UH)BIT0)  /* Receive Data Ready */

/* Serial Control Register (SCSCRn) */
#define SCR_TENDE   ((UH)BIT11) /* Transmit End Interrupt Enable */
#define SCR_TIE     ((UH)BIT7)  /* Transmit Interrupt Enable */
#define SCR_RIE     ((UH)BIT6)  /* Receive Interrupt Enable */
#define SCR_TE      ((UH)BIT5)  /* Transmit Enable */
#define SCR_RE      ((UH)BIT4)  /* Receive Enable */
#define SCR_ERIE    ((UH)BIT3)  /* Receive Error Interrupt Enable */

/* FIFO Data Count Register (SCFDRn) */
#define FTRDR_MASK  (0x001F)

/* FIFO Control Register (SCFCRn) */
#define FCR_TFRST   ((UH)BIT2)  /* Transmit FIFO Data Register Reset  */
#define FCR_RFRST   ((UH)BIT1)  /* Receive FIFO Data Register Reset  */

/* Serial Port Register (SCSPTRn) */
#define SPTR_CTSDT  ((UH)BIT4)  /* Serial Port CTS# Pin Data */
#define SPTR_SPB2DT ((UH)BIT0)  /* Serial Port Break Data */

/* Line Status Register (SCLSRn) */
#define LSR_TO      ((UH)BIT2)  /* Timeout */
#define LSR_ORER    ((UH)BIT0)  /* Overrun Error    */


/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#define MAX_FIFO_COUNT  (16)

#if defined(CFG_P_CLK)
#define SCIF_CLK         CFG_P_CLK
#else
#define SCIF_CLK         (65000000U)
#endif


#ifdef CFG_SCIF_0

#if (CFG_RSTRG_0 == 14)
#define TRSTRG_0    (7<<8)
#elif (CFG_RSTRG_0 == 12)
#define TRSTRG_0    (6<<8)
#elif (CFG_RSTRG_0 == 10)
#define TRSTRG_0    (5<<8)
#elif (CFG_RSTRG_0 == 8)
#define TRSTRG_0    (4<<8)
#elif (CFG_RSTRG_0 == 6)
#define TRSTRG_0    (3<<8)
#elif (CFG_RSTRG_0 == 4)
#define TRSTRG_0    (2<<8)
#elif (CFG_RSTRG_0 == 1)
#define TRSTRG_0    (1<<8)
#else   /* == 15 */
#define TRSTRG_0    (0<<8)
#endif

#if (CFG_RTRG_0 == 14)
#define TRTRG_0     (3<<6)
#elif (CFG_RTRG_0 == 8)
#define TRTRG_0     (2<<6)
#elif (CFG_RTRG_0 == 4)
#define TRTRG_0     (1<<6)
#else   /* == 1 */
#define TRTRG_0     (0<<6)
#endif

#if (CFG_TTRG_0 == 0)
#define TTTRG_0     (3<<4)
#elif (CFG_TTRG_0 == 2)
#define TTTRG_0     (2<<4)
#elif (CFG_TTRG_0 == 4)
#define TTTRG_0     (1<<4)
#else   /* == 8 */
#define TTTRG_0     (0<<4)
#endif

#if (CFG_TXBUF_SZ0==0)
#define _ddr_rzg1_scif_tbuf0  0
#else
VB  _ddr_rzg1_scif_tbuf0[CFG_TXBUF_SZ0];
#endif
VB  _ddr_rzg1_scif_rbuf0[CFG_RXBUF_SZ0];
UB  _ddr_rzg1_scif_sbuf0[CFG_RXBUF_SZ0];
T_RZG1_SCIF_MNG _ddr_rzg1_scif_data0;

const T_CDEV _ddr_rzg1_scif_cdev0 = {&_ddr_rzg1_scif_data0, (FP)_ddr_rzg1_scifdrv};
const T_CFLG _ddr_rzg1_scif_cflg0 = {TA_TFIFO|TA_WMUL, 0x00000000};
const T_CISR _ddr_rzg1_scif_cisr0 = {
    TA_HLNG|TA_FPU, (VP_INT)&_ddr_rzg1_scif_data0, CFG_INT_0, (FP)_ddr_rzg1_scif_intr, CFG_IPL_0
};

#endif


#ifdef CFG_SCIF_1

#if (CFG_RSTRG_1 == 14)
#define TRSTRG_1    (7<<8)
#elif (CFG_RSTRG_1 == 12)
#define TRSTRG_1    (6<<8)
#elif (CFG_RSTRG_1 == 10)
#define TRSTRG_1    (5<<8)
#elif (CFG_RSTRG_1 == 8)
#define TRSTRG_1    (4<<8)
#elif (CFG_RSTRG_1 == 6)
#define TRSTRG_1    (3<<8)
#elif (CFG_RSTRG_1 == 4)
#define TRSTRG_1    (2<<8)
#elif (CFG_RSTRG_1 == 1)
#define TRSTRG_1    (1<<8)
#else   /* == 15 */
#define TRSTRG_1    (0<<8)
#endif

#if (CFG_RTRG_1 == 14)
#define TRTRG_1     (3<<6)
#elif (CFG_RTRG_1 == 8)
#define TRTRG_1     (2<<6)
#elif (CFG_RTRG_1 == 4)
#define TRTRG_1     (1<<6)
#else   /* == 1 */
#define TRTRG_1     (0<<6)
#endif

#if (CFG_TTRG_1 == 0)
#define TTTRG_1     (3<<4)
#elif (CFG_TTRG_1 == 2)
#define TTTRG_1     (2<<4)
#elif (CFG_TTRG_1 == 4)
#define TTTRG_1     (1<<4)
#else   /* == 8 */
#define TTTRG_1     (0<<4)
#endif

#if (CFG_TXBUF_SZ1==0)
#define _ddr_rzg1_scif_tbuf1  0
#else
VB  _ddr_rzg1_scif_tbuf1[CFG_TXBUF_SZ1];
#endif
VB  _ddr_rzg1_scif_rbuf1[CFG_RXBUF_SZ1];
UB  _ddr_rzg1_scif_sbuf1[CFG_RXBUF_SZ1];
T_RZG1_SCIF_MNG _ddr_rzg1_scif_data1;

const T_CDEV _ddr_rzg1_scif_cdev1 = {&_ddr_rzg1_scif_data1, (FP)_ddr_rzg1_scifdrv};
const T_CFLG _ddr_rzg1_scif_cflg1 = {TA_TFIFO|TA_WMUL, 0x00000000};
const T_CISR _ddr_rzg1_scif_cisr1 = {
    TA_HLNG|TA_FPU, (VP_INT)&_ddr_rzg1_scif_data1, CFG_INT_1, (FP)_ddr_rzg1_scif_intr, CFG_IPL_1
};

#endif


#ifdef CFG_SCIF_2

#if (CFG_RSTRG_2 == 14)
#define TRSTRG_2    (7<<8)
#elif (CFG_RSTRG_2 == 12)
#define TRSTRG_2    (6<<8)
#elif (CFG_RSTRG_2 == 10)
#define TRSTRG_2    (5<<8)
#elif (CFG_RSTRG_2 == 8)
#define TRSTRG_2    (4<<8)
#elif (CFG_RSTRG_2 == 6)
#define TRSTRG_2    (3<<8)
#elif (CFG_RSTRG_2 == 4)
#define TRSTRG_2    (2<<8)
#elif (CFG_RSTRG_2 == 1)
#define TRSTRG_2    (1<<8)
#else   /* == 15 */
#define TRSTRG_2    (0<<8)
#endif

#if (CFG_RTRG_2 == 14)
#define TRTRG_2     (3<<6)
#elif (CFG_RTRG_2 == 8)
#define TRTRG_2     (2<<6)
#elif (CFG_RTRG_2 == 4)
#define TRTRG_2     (1<<6)
#else   /* == 1 */
#define TRTRG_2     (0<<6)
#endif

#if (CFG_TTRG_2 == 0)
#define TTTRG_2     (3<<4)
#elif (CFG_TTRG_2 == 2)
#define TTTRG_2     (2<<4)
#elif (CFG_TTRG_2 == 4)
#define TTTRG_2     (1<<4)
#else   /* == 8 */
#define TTTRG_2     (0<<4)
#endif

#if (CFG_TXBUF_SZ2==0)
#define _ddr_rzg1_scif_tbuf2  0
#else
VB  _ddr_rzg1_scif_tbuf2[CFG_TXBUF_SZ2];
#endif
VB  _ddr_rzg1_scif_rbuf2[CFG_RXBUF_SZ2];
UB  _ddr_rzg1_scif_sbuf2[CFG_RXBUF_SZ2];
T_RZG1_SCIF_MNG _ddr_rzg1_scif_data2;

const T_CDEV _ddr_rzg1_scif_cdev2 = {&_ddr_rzg1_scif_data2, (FP)_ddr_rzg1_scifdrv};
const T_CFLG _ddr_rzg1_scif_cflg2 = {TA_TFIFO|TA_WMUL, 0x00000000};
const T_CISR _ddr_rzg1_scif_cisr2 = {
    TA_HLNG|TA_FPU, (VP_INT)&_ddr_rzg1_scif_data2, CFG_INT_2, (FP)_ddr_rzg1_scif_intr, CFG_IPL_2
};

#endif


#ifdef CFG_SCIF_3

#if defined(CFG_G1H)
#error Error: SCIF3 is not supported in RZ/G1H.
#endif

#if (CFG_RSTRG_3 == 14)
#define TRSTRG_3    (7<<8)
#elif (CFG_RSTRG_3 == 12)
#define TRSTRG_3    (6<<8)
#elif (CFG_RSTRG_3 == 10)
#define TRSTRG_3    (5<<8)
#elif (CFG_RSTRG_3 == 8)
#define TRSTRG_3    (4<<8)
#elif (CFG_RSTRG_3 == 6)
#define TRSTRG_3    (3<<8)
#elif (CFG_RSTRG_3 == 4)
#define TRSTRG_3    (2<<8)
#elif (CFG_RSTRG_3 == 1)
#define TRSTRG_3    (1<<8)
#else   /* == 15 */
#define TRSTRG_3    (0<<8)
#endif

#if (CFG_RTRG_3 == 14)
#define TRTRG_3     (3<<6)
#elif (CFG_RTRG_3 == 8)
#define TRTRG_3     (2<<6)
#elif (CFG_RTRG_3 == 4)
#define TRTRG_3     (1<<6)
#else   /* == 1 */
#define TRTRG_3     (0<<6)
#endif

#if (CFG_TTRG_3 == 0)
#define TTTRG_3     (3<<4)
#elif (CFG_TTRG_3 == 2)
#define TTTRG_3     (2<<4)
#elif (CFG_TTRG_3 == 4)
#define TTTRG_3     (1<<4)
#else   /* == 8 */
#define TTTRG_3     (0<<4)
#endif

#if (CFG_TXBUF_SZ3==0)
#define _ddr_rzg1_scif_tbuf3  0
#else
VB  _ddr_rzg1_scif_tbuf3[CFG_TXBUF_SZ3];
#endif
VB  _ddr_rzg1_scif_rbuf3[CFG_RXBUF_SZ3];
UB  _ddr_rzg1_scif_sbuf3[CFG_RXBUF_SZ3];
T_RZG1_SCIF_MNG _ddr_rzg1_scif_data3;

const T_CDEV _ddr_rzg1_scif_cdev3 = {&_ddr_rzg1_scif_data3, (FP)_ddr_rzg1_scifdrv};
const T_CFLG _ddr_rzg1_scif_cflg3 = {TA_TFIFO|TA_WMUL, 0x00000000};
const T_CISR _ddr_rzg1_scif_cisr3 = {
    TA_HLNG|TA_FPU, (VP_INT)&_ddr_rzg1_scif_data3, CFG_INT_3, (FP)_ddr_rzg1_scif_intr, CFG_IPL_3
};

#endif


#ifdef CFG_SCIF_4

#if defined(CFG_G1H)
#error Error: SCIF4 is not supported in RZ/G1H.
#endif

#if (CFG_RSTRG_4 == 14)
#define TRSTRG_4    (7<<8)
#elif (CFG_RSTRG_4 == 12)
#define TRSTRG_4    (6<<8)
#elif (CFG_RSTRG_4 == 10)
#define TRSTRG_4    (5<<8)
#elif (CFG_RSTRG_4 == 8)
#define TRSTRG_4    (4<<8)
#elif (CFG_RSTRG_4 == 6)
#define TRSTRG_4    (3<<8)
#elif (CFG_RSTRG_4 == 4)
#define TRSTRG_4    (2<<8)
#elif (CFG_RSTRG_4 == 1)
#define TRSTRG_4    (1<<8)
#else   /* == 15 */
#define TRSTRG_4    (0<<8)
#endif

#if (CFG_RTRG_4 == 14)
#define TRTRG_4     (3<<6)
#elif (CFG_RTRG_4 == 8)
#define TRTRG_4     (2<<6)
#elif (CFG_RTRG_4 == 4)
#define TRTRG_4     (1<<6)
#else   /* == 1 */
#define TRTRG_4     (0<<6)
#endif

#if (CFG_TTRG_4 == 0)
#define TTTRG_4     (3<<4)
#elif (CFG_TTRG_4 == 2)
#define TTTRG_4     (2<<4)
#elif (CFG_TTRG_4 == 4)
#define TTTRG_4     (1<<4)
#else   /* == 8 */
#define TTTRG_4     (0<<4)
#endif

#if (CFG_TXBUF_SZ4==0)
#define _ddr_rzg1_scif_tbuf4  0
#else
VB  _ddr_rzg1_scif_tbuf4[CFG_TXBUF_SZ3];
#endif
VB  _ddr_rzg1_scif_rbuf4[CFG_RXBUF_SZ3];
UB  _ddr_rzg1_scif_sbuf4[CFG_RXBUF_SZ3];
T_RZG1_SCIF_MNG _ddr_rzg1_scif_data4;

const T_CDEV _ddr_rzg1_scif_cdev4 = {&_ddr_rzg1_scif_data4, (FP)_ddr_rzg1_scifdrv};
const T_CFLG _ddr_rzg1_scif_cflg4 = {TA_TFIFO|TA_WMUL, 0x00000000};
const T_CISR _ddr_rzg1_scif_cisr4 = {
    TA_HLNG|TA_FPU, (VP_INT)&_ddr_rzg1_scif_data4, CFG_INT_4, (FP)_ddr_rzg1_scif_intr, CFG_IPL_4
};

#endif

#ifdef CFG_SCIF_5

#if defined(CFG_G1H)
#error Error: SCIF5 is not supported in RZ/G1H.
#endif

#if (CFG_RSTRG_5 == 14)
#define TRSTRG_5    (7<<8)
#elif (CFG_RSTRG_5 == 12)
#define TRSTRG_5    (6<<8)
#elif (CFG_RSTRG_5 == 10)
#define TRSTRG_5    (5<<8)
#elif (CFG_RSTRG_5 == 8)
#define TRSTRG_5    (4<<8)
#elif (CFG_RSTRG_5 == 6)
#define TRSTRG_5    (3<<8)
#elif (CFG_RSTRG_5 == 4)
#define TRSTRG_5    (2<<8)
#elif (CFG_RSTRG_5 == 1)
#define TRSTRG_5    (1<<8)
#else   /* == 15 */
#define TRSTRG_5    (0<<8)
#endif

#if (CFG_RTRG_5 == 14)
#define TRTRG_5     (3<<6)
#elif (CFG_RTRG_5 == 8)
#define TRTRG_5     (2<<6)
#elif (CFG_RTRG_5 == 4)
#define TRTRG_5     (1<<6)
#else   /* == 1 */
#define TRTRG_5     (0<<6)
#endif

#if (CFG_TTRG_5 == 0)
#define TTTRG_5     (3<<4)
#elif (CFG_TTRG_5 == 2)
#define TTTRG_5    (2<<4)
#elif (CFG_TTRG_5 == 4)
#define TTTRG_5     (1<<4)
#else   /* == 8 */
#define TTTRG_5     (0<<4)
#endif

#if (CFG_TXBUF_SZ5==0)
#define _ddr_rzg1_scif_tbuf5  0
#else
VB  _ddr_rzg1_scif_tbuf5[CFG_TXBUF_SZ5];
#endif
VB  _ddr_rzg1_scif_rbuf5[CFG_RXBUF_SZ5];
UB  _ddr_rzg1_scif_sbuf5[CFG_RXBUF_SZ5];
T_RZG1_SCIF_MNG _ddr_rzg1_scif_data5;

const T_CDEV _ddr_rzg1_scif_cdev5 = {&_ddr_rzg1_scif_data5, (FP)_ddr_rzg1_scifdrv};
const T_CFLG _ddr_rzg1_scif_cflg5 = {TA_TFIFO|TA_WMUL, 0x00000000};
const T_CISR _ddr_rzg1_scif_cisr5 = {
    TA_HLNG|TA_FPU, (VP_INT)&_ddr_rzg1_scif_data5, CFG_INT_5, (FP)_ddr_rzg1_scif_intr, CFG_IPL_5
};

#endif

/**
 * Initializes device driver.
 */
ER _ddr_rzg1_scif_init(ID devid, volatile struct t_scif *scif_port)
{
    ER_ID ercd;
    UW val;

#ifdef CFG_SCIF_0
    if (scif_port == &REG_SCIF0) {
        memset(&_ddr_rzg1_scif_data0, 0x00, sizeof(_ddr_rzg1_scif_data0));
        _ddr_rzg1_scif_data0.port = scif_port;
        _ddr_rzg1_scif_data0.tbuf = _ddr_rzg1_scif_tbuf0;
        _ddr_rzg1_scif_data0.rbuf = _ddr_rzg1_scif_rbuf0;
        _ddr_rzg1_scif_data0.sbuf = _ddr_rzg1_scif_sbuf0;
        _ddr_rzg1_scif_data0.tsize = CFG_TXBUF_SZ0;
        _ddr_rzg1_scif_data0.rsize = CFG_RXBUF_SZ0;
        _ddr_rzg1_scif_data0.xoff_size = CFG_XOFF_SZ0;
        _ddr_rzg1_scif_data0.xon_size = CFG_XON_SZ0;
        _ddr_rzg1_scif_data0.aux[0] = TRSTRG_0 | TTTRG_0 | TRTRG_0;
        _ddr_rzg1_scif_data0.devhdr = (FP)_ddr_rzg1_scifdrv;
        if ((ercd = acre_flg((T_CFLG *)&_ddr_rzg1_scif_cflg0)) > E_OK) {
            _ddr_rzg1_scif_data0.flgid = (UH)ercd;
            ercd = acre_isr((T_CISR *)&_ddr_rzg1_scif_cisr0);
            if (ercd > E_OK) {
                _ddr_rzg1_scif_data0.isrid = (UH)ercd;
                ercd = vdef_dev(devid, (VP)&_ddr_rzg1_scif_cdev0);
                if (ercd >= E_OK) {
                    loc_cpu();
                    val = REG_CPG_MSR.MSTPSR7;
                    val &= ~MSTP7_SCIF0;
                    REG_CPG_MSR.SMSTPCR7 = val;
                    do {
                    } while ((REG_CPG_MSR.SMSTPCR7 & MSTP7_SCIF0) != 0);
                    /* software reset */
                    REG_CPG_MSR.SRCR7 |= MSTP7_SCIF0;
                    val = REG_CPG_MSR.SRCR7;
                    REG_CPG_MSR.SRSTCLR7 = MSTP7_SCIF0;
                    val = REG_CPG_MSR.SRSTCLR7;
                    REG_SCIF0.SCR = 0;
                    REG_SCIF0.SPTR = 0x00C3;
                    val = REG_SCIF0.FSR;
                    REG_SCIF0.FSR = 0;
                    unl_cpu();
                    ena_int(CFG_INT_0);
                } else {
                    del_flg((ID)_ddr_rzg1_scif_data0.flgid);
                    del_isr((ID)_ddr_rzg1_scif_data0.isrid);
                }
            } else {
                del_flg((ID)_ddr_rzg1_scif_data0.flgid);
            }
        }
    } else
#endif

#ifdef CFG_SCIF_1
    if (scif_port == &REG_SCIF1) {
        memset(&_ddr_rzg1_scif_data1, 0x00, sizeof(_ddr_rzg1_scif_data1));
        _ddr_rzg1_scif_data1.port = scif_port;
        _ddr_rzg1_scif_data1.tbuf = _ddr_rzg1_scif_tbuf1;
        _ddr_rzg1_scif_data1.rbuf = _ddr_rzg1_scif_rbuf1;
        _ddr_rzg1_scif_data1.sbuf = _ddr_rzg1_scif_sbuf1;
        _ddr_rzg1_scif_data1.tsize = CFG_TXBUF_SZ1;
        _ddr_rzg1_scif_data1.rsize = CFG_RXBUF_SZ1;
        _ddr_rzg1_scif_data1.xoff_size = CFG_XOFF_SZ1;
        _ddr_rzg1_scif_data1.xon_size = CFG_XON_SZ1;
        _ddr_rzg1_scif_data1.aux[0] = TRSTRG_1 | TTTRG_1 | TRTRG_1;
        _ddr_rzg1_scif_data1.devhdr = (FP)_ddr_rzg1_scifdrv;
        if ((ercd = acre_flg((T_CFLG *)&_ddr_rzg1_scif_cflg1)) > E_OK) {
            _ddr_rzg1_scif_data1.flgid = (UH)ercd;
            ercd = acre_isr((T_CISR *)&_ddr_rzg1_scif_cisr1);
            if (ercd > E_OK) {
                _ddr_rzg1_scif_data1.isrid = (UH)ercd;
                ercd = vdef_dev(devid, (VP)&_ddr_rzg1_scif_cdev1);
                if (ercd >= E_OK) {
                    loc_cpu();
                    val = REG_CPG_MSR.MSTPSR7;
                    val &= ~MSTP7_SCIF1;
                    REG_CPG_MSR.SMSTPCR7 = val;
                    do {
                    } while ((REG_CPG_MSR.SMSTPCR7 & MSTP7_SCIF1) != 0);
                    /* software reset */
                    REG_CPG_MSR.SRCR7 |= MSTP7_SCIF1;
                    val = REG_CPG_MSR.SRCR7;
                    REG_CPG_MSR.SRSTCLR7 = MSTP7_SCIF1;
                    val = REG_CPG_MSR.SRSTCLR7;
                    REG_SCIF1.SCR = 0;
                    REG_SCIF1.SPTR = 0x00C3;
                    val = REG_SCIF1.FSR;
                    REG_SCIF1.FSR = 0;
                    unl_cpu();
                    ena_int(CFG_INT_1);
                } else {
                    del_flg((ID)_ddr_rzg1_scif_data1.flgid);
                    del_isr((ID)_ddr_rzg1_scif_data1.isrid);
                }
            } else {
                del_flg((ID)_ddr_rzg1_scif_data1.flgid);
            }
        }
    } else
#endif

#ifdef CFG_SCIF_2
    if (scif_port == &REG_SCIF2) {
        memset(&_ddr_rzg1_scif_data2, 0x00, sizeof(_ddr_rzg1_scif_data2));
        _ddr_rzg1_scif_data2.port = scif_port;
        _ddr_rzg1_scif_data2.tbuf = _ddr_rzg1_scif_tbuf2;
        _ddr_rzg1_scif_data2.rbuf = _ddr_rzg1_scif_rbuf2;
        _ddr_rzg1_scif_data2.sbuf = _ddr_rzg1_scif_sbuf2;
        _ddr_rzg1_scif_data2.tsize = CFG_TXBUF_SZ2;
        _ddr_rzg1_scif_data2.rsize = CFG_RXBUF_SZ2;
        _ddr_rzg1_scif_data2.xoff_size = CFG_XOFF_SZ2;
        _ddr_rzg1_scif_data2.xon_size = CFG_XON_SZ2;
        _ddr_rzg1_scif_data2.aux[0] = TRSTRG_2 | TTTRG_2 | TRTRG_2;
        _ddr_rzg1_scif_data2.devhdr = (FP)_ddr_rzg1_scifdrv;
        if ((ercd = acre_flg((T_CFLG *)&_ddr_rzg1_scif_cflg2)) > E_OK) {
            _ddr_rzg1_scif_data2.flgid = (UH)ercd;
            ercd = acre_isr((T_CISR *)&_ddr_rzg1_scif_cisr2);
            if (ercd > E_OK) {
                _ddr_rzg1_scif_data2.isrid = (UH)ercd;
                ercd = vdef_dev(devid, (VP)&_ddr_rzg1_scif_cdev2);
                if (ercd >= E_OK) {
                    loc_cpu();
#if (defined(CFG_G1M) || defined(CFG_G1N) || defined(CFG_G1E))
                    val = REG_CPG_MSR.MSTPSR7;
                    val &= ~MSTP7_SCIF2;
                    REG_CPG_MSR.SMSTPCR7 = val;
                    do {
                    } while ((REG_CPG_MSR.SMSTPCR7 & MSTP7_SCIF2) != 0);
                    /* software reset */
                    REG_CPG_MSR.SRCR7 |= MSTP7_SCIF2;
                    val = REG_CPG_MSR.SRCR7;
                    REG_CPG_MSR.SRSTCLR7 = MSTP7_SCIF2;
                    val = REG_CPG_MSR.SRSTCLR7;
#else
                    val = REG_CPG_MSR.MSTPSR3;
                    val &= ~MSTP3_SCIF2;
                    REG_CPG_MSR.SMSTPCR3 = val;
                    do {
                    } while ((REG_CPG_MSR.SMSTPCR3 & MSTP3_SCIF2) != 0);
                    /* software reset */
                    REG_CPG_MSR.SRCR3 |= MSTP3_SCIF2;
                    val = REG_CPG_MSR.SRCR3;
                    REG_CPG_MSR.SRSTCLR3 = MSTP3_SCIF2;
                    val = REG_CPG_MSR.SRSTCLR3;
#endif
                    REG_SCIF2.SCR = 0;
                    REG_SCIF2.SPTR = 0x00C3;
                    val = REG_SCIF2.FSR;
                    REG_SCIF2.FSR = 0;
                    unl_cpu();
                    ena_int(CFG_INT_2);
                } else {
                    del_flg((ID)_ddr_rzg1_scif_data2.flgid);
                    del_isr((ID)_ddr_rzg1_scif_data2.isrid);
                }
            } else {
                del_flg((ID)_ddr_rzg1_scif_data2.flgid);
            }
        }
    } else
#endif

#ifdef CFG_SCIF_3
    if (scif_port == &REG_SCIF3) {
        memset(&_ddr_rzg1_scif_data3, 0x00, sizeof(_ddr_rzg1_scif_data3));
        _ddr_rzg1_scif_data3.port = scif_port;
        _ddr_rzg1_scif_data3.tbuf = _ddr_rzg1_scif_tbuf3;
        _ddr_rzg1_scif_data3.rbuf = _ddr_rzg1_scif_rbuf3;
        _ddr_rzg1_scif_data3.sbuf = _ddr_rzg1_scif_sbuf3;
        _ddr_rzg1_scif_data3.tsize = CFG_TXBUF_SZ3;
        _ddr_rzg1_scif_data3.rsize = CFG_RXBUF_SZ3;
        _ddr_rzg1_scif_data3.xoff_size = CFG_XOFF_SZ3;
        _ddr_rzg1_scif_data3.xon_size = CFG_XON_SZ3;
        _ddr_rzg1_scif_data3.aux[0] = TRSTRG_3 | TTTRG_3 | TRTRG_3;
        _ddr_rzg1_scif_data3.devhdr = (FP)_ddr_rzg1_scifdrv;
        if ((ercd = acre_flg((T_CFLG *)&_ddr_rzg1_scif_cflg3)) > E_OK) {
            _ddr_rzg1_scif_data3.flgid = (UH)ercd;
            ercd = acre_isr((T_CISR *)&_ddr_rzg1_scif_cisr3);
            if (ercd > E_OK) {
                _ddr_rzg1_scif_data3.isrid = (UH)ercd;
                ercd = vdef_dev(devid, (VP)&_ddr_rzg1_scif_cdev3);
                if (ercd >= E_OK) {
                    loc_cpu();
                    val = REG_CPG_MSR.MSTPSR7;
                    val &= ~MSTP7_SCIF3;
                    REG_CPG_MSR.SMSTPCR7 = val;
                    do {
                    } while ((REG_CPG_MSR.SMSTPCR7 & MSTP7_SCIF3) != 0);
                    /* software reset */
                    REG_CPG_MSR.SRCR7 |= MSTP7_SCIF3;
                    val = REG_CPG_MSR.SRCR7;
                    REG_CPG_MSR.SRSTCLR7 = MSTP7_SCIF3;
                    val = REG_CPG_MSR.SRSTCLR7;
                    REG_SCIF3.SCR = 0;
                    REG_SCIF3.SPTR = 0x00C3;
                    val = REG_SCIF3.FSR;
                    REG_SCIF3.FSR = 0;
                    unl_cpu();
                    ena_int(CFG_INT_3);
                } else {
                    del_flg((ID)_ddr_rzg1_scif_data3.flgid);
                    del_isr((ID)_ddr_rzg1_scif_data3.isrid);
                }
            } else {
                del_flg((ID)_ddr_rzg1_scif_data3.flgid);
            }
        }
    } else
#endif

#ifdef CFG_SCIF_4
    if (scif_port == &REG_SCIF4) {
        memset(&_ddr_rzg1_scif_data4, 0x00, sizeof(_ddr_rzg1_scif_data4));
        _ddr_rzg1_scif_data4.port = scif_port;
        _ddr_rzg1_scif_data4.tbuf = _ddr_rzg1_scif_tbuf4;
        _ddr_rzg1_scif_data4.rbuf = _ddr_rzg1_scif_rbuf4;
        _ddr_rzg1_scif_data4.sbuf = _ddr_rzg1_scif_sbuf4;
        _ddr_rzg1_scif_data4.tsize = CFG_TXBUF_SZ4;
        _ddr_rzg1_scif_data4.rsize = CFG_RXBUF_SZ4;
        _ddr_rzg1_scif_data4.xoff_size = CFG_XOFF_SZ4;
        _ddr_rzg1_scif_data4.xon_size = CFG_XON_SZ4;
        _ddr_rzg1_scif_data4.aux[0] = TRSTRG_4 | TTTRG_4 | TRTRG_4;
        _ddr_rzg1_scif_data4.devhdr = (FP)_ddr_rzg1_scifdrv;
        if ((ercd = acre_flg((T_CFLG *)&_ddr_rzg1_scif_cflg4)) > E_OK) {
            _ddr_rzg1_scif_data4.flgid = (UH)ercd;
            ercd = acre_isr((T_CISR *)&_ddr_rzg1_scif_cisr4);
            if (ercd > E_OK) {
                _ddr_rzg1_scif_data4.isrid = (UH)ercd;
                ercd = vdef_dev(devid, (VP)&_ddr_rzg1_scif_cdev4);
                if (ercd >= E_OK) {
                    loc_cpu();
                    val = REG_CPG_MSR.MSTPSR7;
                    val &= ~MSTP7_SCIF4;
                    REG_CPG_MSR.SMSTPCR7 = val;
                    do {
                    } while ((REG_CPG_MSR.SMSTPCR7 & MSTP7_SCIF4) != 0);
                    /* software reset */
                    REG_CPG_MSR.SRCR7 |= MSTP7_SCIF4;
                    val = REG_CPG_MSR.SRCR7;
                    REG_CPG_MSR.SRSTCLR7 = MSTP7_SCIF4;
                    val = REG_CPG_MSR.SRSTCLR7;
                    REG_SCIF4.SCR = 0;
                    REG_SCIF4.SPTR = 0x00C3;
                    val = REG_SCIF4.FSR;
                    REG_SCIF4.FSR = 0;
                    unl_cpu();
                    ena_int(CFG_INT_4);
                } else {
                    del_flg((ID)_ddr_rzg1_scif_data4.flgid);
                    del_isr((ID)_ddr_rzg1_scif_data4.isrid);
                }
            } else {
                del_flg((ID)_ddr_rzg1_scif_data4.flgid);
            }
        }
    } else
#endif


#ifdef CFG_SCIF_5
    if (scif_port == &REG_SCIF5) {
        memset(&_ddr_rzg1_scif_data5, 0x00, sizeof(_ddr_rzg1_scif_data5));
        _ddr_rzg1_scif_data5.port = scif_port;
        _ddr_rzg1_scif_data5.tbuf = _ddr_rzg1_scif_tbuf5;
        _ddr_rzg1_scif_data5.rbuf = _ddr_rzg1_scif_rbuf5;
        _ddr_rzg1_scif_data5.sbuf = _ddr_rzg1_scif_sbuf5;
        _ddr_rzg1_scif_data5.tsize = CFG_TXBUF_SZ5;
        _ddr_rzg1_scif_data5.rsize = CFG_RXBUF_SZ5;
        _ddr_rzg1_scif_data5.xoff_size = CFG_XOFF_SZ5;
        _ddr_rzg1_scif_data5.xon_size = CFG_XON_SZ5;
        _ddr_rzg1_scif_data5.aux[0] = TRSTRG_5 | TTTRG_5 | TRTRG_5;
        _ddr_rzg1_scif_data5.devhdr = (FP)_ddr_rzg1_scifdrv;
        if ((ercd = acre_flg((T_CFLG *)&_ddr_rzg1_scif_cflg5)) > E_OK) {
            _ddr_rzg1_scif_data5.flgid = (UH)ercd;
            ercd = acre_isr((T_CISR *)&_ddr_rzg1_scif_cisr5);
            if (ercd > E_OK) {
                _ddr_rzg1_scif_data5.isrid = (UH)ercd;
                ercd = vdef_dev(devid, (VP)&_ddr_rzg1_scif_cdev5);
                if (ercd >= E_OK) {
                    loc_cpu();
                    val = REG_CPG_MSR.MSTPSR7;
                    val &= ~MSTP7_SCIF5;
                    REG_CPG_MSR.SMSTPCR7 = val;
                    do {
                    } while ((REG_CPG_MSR.SMSTPCR7 & MSTP7_SCIF5) != 0);
                    /* software reset */
                    REG_CPG_MSR.SRCR7 |= MSTP7_SCIF5;
                    val = REG_CPG_MSR.SRCR7;
                    REG_CPG_MSR.SRSTCLR7 = MSTP7_SCIF5;
                    val = REG_CPG_MSR.SRSTCLR7;
                    REG_SCIF5.SCR = 0;
                    REG_SCIF5.SPTR = 0x00C3;
                    val = REG_SCIF5.FSR;
                    REG_SCIF5.FSR = 0;
                    unl_cpu();
                    ena_int(CFG_INT_5);
                } else {
                    del_flg((ID)_ddr_rzg1_scif_data5.flgid);
                    del_isr((ID)_ddr_rzg1_scif_data5.isrid);
                }
            } else {
                del_flg((ID)_ddr_rzg1_scif_data5.flgid);
            }
        }
    } else
#endif


        ercd = E_NOEXS;

    return ercd;
}

#if (defined(CFG_SCIF_0) || defined(CFG_SCIF_1) || defined(CFG_SCIF_2) || defined(CFG_SCIF_3) || defined(CFG_SCIF_4)|| defined(CFG_SCIF_5))

/**
  * @brief Check receipt end.
  */
BOOL _ddr_rzg1_scif_check_chr(T_COM_RCV *ReceiveData, VB chr, UB sts)
{
    BOOL flag = FALSE;

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
UB _ddr_rzg1_scif_getssr(volatile struct t_scif *port)
{
    UH ssr;
    UB sts = 0;

    ssr = port->FSR;
    if ((ssr & FSR_PER) != 0) {
        sts |= T_COM_ERP;   /* Parity Error */
    }
    if ((ssr & FSR_FER) != 0) {
        sts |= T_COM_ERF;   /* Framing Error */
    }
    if ((ssr & FSR_BRK) != 0) {
        sts |= T_COM_BRK;   /* Break Detect */
        port->FSR = ~FSR_BRK;
    }
    return sts;
}

/**
  * @brief Copy buffer (from local to internal).
  * @param[in/out] pk_SCIFmng
  * @param[in] TransmiteData
  */
BOOL _ddr_rzg1_scif_copy(T_RZG1_SCIF_MNG *pk_SCIFmng, T_COM_SND *TransmiteData)
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
void _ddr_rzg1_scif_chk_rxoff(T_RZG1_SCIF_MNG *pk_SCIFmng)
{
    if ((pk_SCIFmng->status.bit.rx_xoff == 0) &&
        (pk_SCIFmng->rcnt >= pk_SCIFmng->xoff_size)) {
        pk_SCIFmng->status.bit.rx_xoff = 1;
        if (((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < MAX_FIFO_COUNT) {
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
void _ddr_rzg1_scif_chk_rxon(T_RZG1_SCIF_MNG *pk_SCIFmng)
{
    if ((pk_SCIFmng->status.bit.rx_xoff == 1) &&
        (pk_SCIFmng->rcnt <= pk_SCIFmng->xon_size)) {
        pk_SCIFmng->status.bit.rx_xoff = 0;
        if (((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < MAX_FIFO_COUNT) {
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
void _ddr_rzg1_scif_send_local_buf(T_RZG1_SCIF_MNG *pk_SCIFmng)
{
    for (; ((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < MAX_FIFO_COUNT; ) {
        pk_SCIFmng->port->FTDR = *pk_SCIFmng->SndData->tbuf++;
        if ((pk_SCIFmng->port->FSR & FSR_TDFE) != 0) {
            pk_SCIFmng->port->FSR = ~FSR_TDFE;  /* TDFE(Transmit FIFO Data Empty) clear to 0. */
        }
        if ((pk_SCIFmng->port->FSR & FSR_TEND) != 0) {
            pk_SCIFmng->port->FSR = ~FSR_TEND;  /* TEND(Transmit End) clear to 0. */
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
void _ddr_rzg1_scif_send_drv_buf(T_RZG1_SCIF_MNG *pk_SCIFmng)
{
    UH sndp;

    sndp = pk_SCIFmng->sndp - pk_SCIFmng->tcnt;
    if (pk_SCIFmng->tcnt > pk_SCIFmng->sndp) {
        sndp += pk_SCIFmng->tsize;
    }
    for (; ((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < MAX_FIFO_COUNT; ) {
        pk_SCIFmng->port->FTDR = pk_SCIFmng->tbuf[sndp];
        if ((pk_SCIFmng->port->FSR & FSR_TDFE) != 0) {
            pk_SCIFmng->port->FSR = ~FSR_TDFE; /* TDFE(Transmit FIFO Data Empty) clear to 0. */
        }
        if ((pk_SCIFmng->port->FSR & FSR_TEND) != 0) {
            pk_SCIFmng->port->FSR = ~FSR_TEND; /* TEND(Transmit End) clear to 0. */
        }
        if (++sndp >= pk_SCIFmng->tsize) {
            sndp = 0;
        }
        if (--pk_SCIFmng->tcnt == 0) {
            break;
        }
    }
    if (pk_SCIFmng->SndData != 0) {
        if (_ddr_rzg1_scif_copy(pk_SCIFmng, pk_SCIFmng->SndData) == TRUE) {
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
void _ddr_rzg1_scif_bri(T_RZG1_SCIF_MNG *pk_SCIFmng)
{
    UH rcvp;

    /* Overrun */
    if ((pk_SCIFmng->port->LSR & LSR_ORER) != 0) {
        pk_SCIFmng->port->LSR = ~LSR_ORER; /* ORER clear to 0 */
        pk_SCIFmng->status.bit.er_buf_ovr = 1;
    }

    /* Break */
    if ((pk_SCIFmng->port->FSR & FSR_BRK) != 0) {
        pk_SCIFmng->port->FSR = ~FSR_BRK; /* BRK clear to 0 */
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
                    _ddr_rzg1_scif_chk_rxoff(pk_SCIFmng);
                }
                if (++pk_SCIFmng->rcnt == pk_SCIFmng->rsize) {
                    pk_SCIFmng->port->SCR &= ~SCR_RIE; /* RIE(Receive-FIFO-data-full interrupt request) disabled */
                    //pk_SCIFmng->port->SCR &= ~SCR_ERIE;
                    //pk_SCIFmng->port->SCR &= ~SCR_BRIE;
                    //pk_SCIFmng->port->SCR &= ~SCR_DRIE;
                }
            }
        }
    }
}


/**
  * @brief TXI interrupt
  * @param[in] pk_SCIFmng
  */
void _ddr_rzg1_scif_txi(T_RZG1_SCIF_MNG *pk_SCIFmng)
{
    if (pk_SCIFmng->status.bit.req_xon_xoff == 1) {
        pk_SCIFmng->port->FTDR = (pk_SCIFmng->status.bit.rx_xoff == 0) ? XON : XOFF;
        if ((pk_SCIFmng->port->FSR & FSR_TDFE) != 0) {
            pk_SCIFmng->port->FSR = ~FSR_TDFE; /* TDFE(Transmit FIFO Data Empty) clear to 0 */
        }
        if ((pk_SCIFmng->port->FSR & FSR_TEND) != 0) {
            pk_SCIFmng->port->FSR = ~FSR_TEND; /* TEND(Transmit End) clear to 0 */
        }
        pk_SCIFmng->status.bit.req_xon_xoff = 0;
    }
    if (pk_SCIFmng->status.bit.tx_xoff == 0) {
        if (pk_SCIFmng->tsize == 0) {
            if (pk_SCIFmng->SndData != 0) {
                _ddr_rzg1_scif_send_local_buf(pk_SCIFmng);
            } else {
                pk_SCIFmng->port->SCR &= ~SCR_TIE; /* TIE(Transmit-FIFO-data-empty interrupt request) disabled */
            }
        } else {
            if (pk_SCIFmng->tcnt != 0) {
                _ddr_rzg1_scif_send_drv_buf(pk_SCIFmng);
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
void _ddr_rzg1_scif_rxi(T_RZG1_SCIF_MNG *pk_SCIFmng)
{
    UH rcvp;
    VB chr;
    UB sts;

    if ((pk_SCIFmng->rcnt < pk_SCIFmng->rsize) ||
        (pk_SCIFmng->RcvData != 0)) {
        for (; (pk_SCIFmng->port->FDR & FTRDR_MASK) != 0; ) {
            sts = _ddr_rzg1_scif_getssr(pk_SCIFmng->port);
            chr = pk_SCIFmng->port->FRDR;
            if ((pk_SCIFmng->status.bit.sft_flw == 1) && (chr == XON)) {
                pk_SCIFmng->status.bit.tx_xoff = 0;
                pk_SCIFmng->port->SCR |= SCR_TIE; /* TIE(Transmit-FIFO-data-empty interrupt request) enabled */
                if (((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < MAX_FIFO_COUNT) {
                    _ddr_rzg1_scif_txi(pk_SCIFmng);
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
                    if (_ddr_rzg1_scif_check_chr(pk_SCIFmng->RcvData, chr, sts) == TRUE) {
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
                        _ddr_rzg1_scif_chk_rxoff(pk_SCIFmng);
                    }
                    if (++pk_SCIFmng->rcnt == pk_SCIFmng->rsize) {
                        pk_SCIFmng->port->SCR &= ~SCR_RIE; /* RIE( Receive-FIFO-data-full interrupt request) disabled */
                        //pk_SCIFmng->port->SCR &= ~SCR_ERIE;
                        //pk_SCIFmng->port->SCR &= ~SCR_BRIE;
                        //pk_SCIFmng->port->SCR &= ~SCR_DRIE;
                        break;
                    }
                }
            }
        }

        /* Check RIE */
        if ((pk_SCIFmng->port->SCR & SCR_RIE) != 0) {
            if ((pk_SCIFmng->port->FSR & FSR_RDF) != 0) {
                pk_SCIFmng->port->FSR = ~FSR_RDF; /* RDF clear to 0 */
            }
            if ((pk_SCIFmng->port->FSR & FSR_DR) != 0) {
                pk_SCIFmng->port->FSR = ~FSR_DR; /* DR clear to 0 */
            }
        }

    } else {
        pk_SCIFmng->port->SCR &= ~SCR_RIE; /* RIE(Receive-FIFO-data-full interrupt request) disabled */
        //pk_SCIFmng->port->SCR &= ~SCR_ERIE;
        //pk_SCIFmng->port->SCR &= ~SCR_BRIE;
        //pk_SCIFmng->port->SCR &= ~SCR_DRIE;
    }

    if ((pk_SCIFmng->port->FSR & FSR_ER) != 0) {
        pk_SCIFmng->port->FSR = ~FSR_ER;
    }
}


/**
  * SCIF Interrupt service routine.
  */
void _ddr_rzg1_scif_intr(T_RZG1_SCIF_MNG *pk_SCIFmng)
{
    /* RDF, DR, ER */
    if ((pk_SCIFmng->port->FSR & FSR_RDF) != 0 ||
        (pk_SCIFmng->port->FSR & FSR_DR) != 0 ||
        (pk_SCIFmng->port->FSR & FSR_ER) != 0) {
        _ddr_rzg1_scif_rxi(pk_SCIFmng);
    }
    /* ORER, BRK */
    if ((pk_SCIFmng->port->FSR & FSR_BRK) != 0 ||
        (pk_SCIFmng->port->LSR & LSR_ORER) != 0) {
        _ddr_rzg1_scif_bri(pk_SCIFmng);
    }
    /* TDFE */
    if ((pk_SCIFmng->port->FSR & FSR_TDFE) != 0) {
        _ddr_rzg1_scif_txi(pk_SCIFmng);
    }
}

/**
  * @brief Fills transmit FIFO.
  * @param[in] pk_SCIFmng
  * @param[in] TransmiteData
  */
void _ddr_rzg1_scif_send_char(T_RZG1_SCIF_MNG *pk_SCIFmng, T_COM_SND *TransmiteData)
{
    if (pk_SCIFmng->status.bit.req_xon_xoff == 1) {
        pk_SCIFmng->port->FTDR = (pk_SCIFmng->status.bit.rx_xoff == 0) ? XON : XOFF;
        if ((pk_SCIFmng->port->FSR & FSR_TDFE) != 0) {
            pk_SCIFmng->port->FSR = ~FSR_TDFE; /* TDFE clear to 0 */
        }
        pk_SCIFmng->status.bit.req_xon_xoff = 0;
    }
    if (pk_SCIFmng->status.bit.tx_xoff == 0) {
        for (; ((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < MAX_FIFO_COUNT; ) {
            pk_SCIFmng->port->FTDR = *TransmiteData->tbuf++;
            if ((pk_SCIFmng->port->FSR & FSR_TDFE) != 0) {
                pk_SCIFmng->port->FSR = ~FSR_TDFE; /* TDFE clear to 0 */
            }
            if ((pk_SCIFmng->port->FSR & FSR_TEND) != 0) {
                pk_SCIFmng->port->FSR = ~FSR_TEND; /* TEND clear to 0 */
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
BOOL _ddr_rzg1_scif_recv_strings(T_RZG1_SCIF_MNG *pk_SCIFmng, T_COM_RCV *ReceiveData)
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
                _ddr_rzg1_scif_chk_rxon(pk_SCIFmng);
            }
            flag = _ddr_rzg1_scif_check_chr(ReceiveData, chr, sts);
        }
    }
    return flag;
}

/**
  * @brief Transmit data
  * @param[in] TransmiteData
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scif_snd(T_COM_SND *TransmiteData, T_RZG1_SCIF_MNG *pk_SCIFmng)
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
            //pk_SCIFmng->port->SCR &= ~SCR_ERIE;
            //pk_SCIFmng->port->SCR &= ~SCR_BRIE;
            //pk_SCIFmng->port->SCR &= ~SCR_DRIE;
            unl_cpu();
            if ((pk_SCIFmng->tcnt == 0) && (((pk_SCIFmng->port->FDR >> 8) & FTRDR_MASK) < MAX_FIFO_COUNT)) {
                _ddr_rzg1_scif_send_char(pk_SCIFmng, TransmiteData);
            }
            loc_cpu();
            pk_SCIFmng->port->SCR |= SCR_RIE; /* enable RIE */
            //pk_SCIFmng->port->SCR |= SCR_ERIE;
            //pk_SCIFmng->port->SCR |= SCR_BRIE;
            //pk_SCIFmng->port->SCR |= SCR_DRIE;
            unl_cpu();
            if (_ddr_rzg1_scif_copy(pk_SCIFmng, TransmiteData) == FALSE) {
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
ER _ddr_rzg1_scif_rcv(T_COM_RCV *ReceiveData, T_RZG1_SCIF_MNG *pk_SCIFmng)
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
            //pk_SCIFmng->port->SCR &= ~SCR_ERIE;
            //pk_SCIFmng->port->SCR &= ~SCR_BRIE;
            //pk_SCIFmng->port->SCR &= ~SCR_DRIE;
            unl_cpu();
            if (_ddr_rzg1_scif_recv_strings(pk_SCIFmng, ReceiveData) == FALSE) {
                pk_SCIFmng->RcvData = ReceiveData;
                loc_cpu();
                pk_SCIFmng->port->SCR |= SCR_RIE; /* enable RIE */
                //pk_SCIFmng->port->SCR |= SCR_ERIE;
                //pk_SCIFmng->port->SCR |= SCR_BRIE;
                //pk_SCIFmng->port->SCR |= SCR_DRIE;
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
                //pk_SCIFmng->port->SCR |= SCR_ERIE;
                //pk_SCIFmng->port->SCR |= SCR_BRIE;
                //pk_SCIFmng->port->SCR |= SCR_DRIE;
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
ER _ddr_rzg1_scif_cln_tx_buf(T_RZG1_SCIF_MNG *pk_SCIFmng, TMO time)
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
        for(; ((pk_SCIFmng->port->FSR & FSR_TEND) == 0) && (ercd == E_OK); ) {
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
ER _ddr_rzg1_scif_rst_tx_buf(T_RZG1_SCIF_MNG *pk_SCIFmng)
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
ER _ddr_rzg1_scif_rst_rx_buf(T_RZG1_SCIF_MNG *pk_SCIFmng)
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
ER _ddr_rzg1_scif_dis_send(T_RZG1_SCIF_MNG *pk_SCIFmng)
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
ER _ddr_rzg1_scif_dis_rcv(T_RZG1_SCIF_MNG *pk_SCIFmng)
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
        //pk_SCIFmng->port->SCR &= ~SCR_ERIE;
        //pk_SCIFmng->port->SCR &= ~SCR_BRIE;
        //pk_SCIFmng->port->SCR &= ~SCR_DRIE;
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
ER _ddr_rzg1_scif_ena_send(T_RZG1_SCIF_MNG *pk_SCIFmng)
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
ER _ddr_rzg1_scif_ena_rcv(T_RZG1_SCIF_MNG *pk_SCIFmng)
{
    ER ercd;
    ID tskid;

    get_tid(&tskid);
    if ((pk_SCIFmng->rlockid == 0) ||
        (pk_SCIFmng->rlockid == (UH)tskid)) {
        loc_cpu();
        pk_SCIFmng->status.bit.ena_rx = 1;
        pk_SCIFmng->port->SCR |= (SCR_RIE | SCR_RE); /* enable RIE, RE */
        //pk_SCIFmng->port->SCR |= SCR_ERIE;
        //pk_SCIFmng->port->SCR |= SCR_BRIE;
        //pk_SCIFmng->port->SCR |= SCR_DRIE;
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
ER _ddr_rzg1_scif_snd_brk(T_RZG1_SCIF_MNG *pk_SCIFmng, TMO time)
{
    ER ercd;
    ID tskid;

    ercd = E_PAR;
    get_tid(&tskid);
    if (((pk_SCIFmng->tlockid == 0) ||
         (pk_SCIFmng->tlockid == (UH)tskid)) &&
        ( pk_SCIFmng->SndData == 0) &&
        ( pk_SCIFmng->tcnt    == 0)) {
        if ((pk_SCIFmng->port->FSR & FSR_TEND) != 0) {
            loc_cpu();
            pk_SCIFmng->port->SPTR &= ~SPTR_SPB2DT; /* TXD pin is Low */
            pk_SCIFmng->port->SCR &= ~SCR_TE;
            unl_cpu();
            dly_tsk(time);
            loc_cpu();
            pk_SCIFmng->port->SPTR |= SPTR_SPB2DT; /* TXD pin is High */
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
ER _ddr_rzg1_scif_lock_trans(T_RZG1_SCIF_MNG *pk_SCIFmng)
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
ER _ddr_rzg1_scif_lock_recv(T_RZG1_SCIF_MNG *pk_SCIFmng)
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
ER _ddr_rzg1_scif_unlock_trans(T_RZG1_SCIF_MNG *pk_SCIFmng)
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
ER _ddr_rzg1_scif_unlock_recv(T_RZG1_SCIF_MNG *pk_SCIFmng)
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
ER _ddr_rzg1_scif_ctr(const T_COM_CTR *pk_SerialData, T_RZG1_SCIF_MNG *pk_SCIFmng)
{
    ER ercd = E_OK;

    if (pk_SCIFmng->status.bit.init_flg == 0) {
        ercd = E_OBJ;
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & CLN_TXBUF) == CLN_TXBUF) {
            ercd = _ddr_rzg1_scif_cln_tx_buf(pk_SCIFmng, pk_SerialData->time);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & RST_TXBUF) == RST_TXBUF) {
            ercd = _ddr_rzg1_scif_rst_tx_buf(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & RST_RXBUF) == RST_RXBUF) {
            ercd = _ddr_rzg1_scif_rst_rx_buf(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & STP_TX) == STP_TX) {
            ercd = _ddr_rzg1_scif_dis_send(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & STP_RX) == STP_RX) {
            ercd = _ddr_rzg1_scif_dis_rcv(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & STA_TX) == STA_TX) {
            ercd = _ddr_rzg1_scif_ena_send(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & STA_RX) == STA_RX) {
            ercd = _ddr_rzg1_scif_ena_rcv(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & SND_BRK) == SND_BRK) {
            ercd = _ddr_rzg1_scif_snd_brk(pk_SCIFmng, pk_SerialData->time);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & LOC_TX) == LOC_TX) {
            ercd = _ddr_rzg1_scif_lock_trans(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & LOC_RX) == LOC_RX) {
            ercd = _ddr_rzg1_scif_lock_recv(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & UNL_TX) == UNL_TX) {
            ercd = _ddr_rzg1_scif_unlock_trans(pk_SCIFmng);
        }
    }

    if (ercd == E_OK) {
        if ((pk_SerialData->command & UNL_RX) == UNL_RX) {
            ercd = _ddr_rzg1_scif_unlock_recv(pk_SCIFmng);
        }
    }

    return ercd;
}

/**
  * @brief Reference to serial port status.
  * @param[in/out] pk_SerialRef
  * @param[in] pk_SCIFmng
  */
ER _ddr_rzg1_scif_ref(T_COM_REF *pk_SerialRef, T_RZG1_SCIF_MNG *pk_SCIFmng)
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
        if ((pk_SCIFmng->port->SPTR & SPTR_CTSDT) == 0) {
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
  * Initializes the UART function of the SCIF.
  */
ER _ddr_rzg1_scif_ini(const T_COM_SMOD *pk_SerialMode, T_RZG1_SCIF_MNG *pk_SCIFmng)
{
    UW brr;
    UW freq;
    ER ercd = E_OK;
    T_RZG1_SCIF_MSTS status;
    UH smr;
    UH fcr;
    volatile int i;

    pk_SCIFmng->port->SCR = 0;
    pk_SCIFmng->port->FCR = 0x0006;  /* Reset FTDR/FRDR */
    status.word = 0;
    fcr = pk_SCIFmng->aux[0];

    /*
     * The SCABRR setting is found by using the following equations.
     * N = (Pclock / (64 x 2^(2n-1) x B)) x 10^6 - 1
     */
    freq = SCIF_CLK;
    for (smr = 0; smr < 4; smr++) {
        brr = freq / (16 * pk_SerialMode->baud) + 1;
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

    switch(pk_SerialMode->flow) {
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
ER _ddr_rzg1_scifdrv(ID FuncID, VP pk_ControlData, T_RZG1_SCIF_MNG *pk_SCIFmng)
{
    ER ercd;

    switch (FuncID) {
        case TA_COM_INI:
            ercd = _ddr_rzg1_scif_ini((const T_COM_SMOD *)pk_ControlData, pk_SCIFmng);
            break;
        case TA_COM_REF:
            ercd = _ddr_rzg1_scif_ref((T_COM_REF *)pk_ControlData, pk_SCIFmng);
            break;
        case TA_COM_CTR:
            ercd = _ddr_rzg1_scif_ctr((const T_COM_CTR *)pk_ControlData, pk_SCIFmng);
            break;
        case TA_COM_PUTS:
            ercd = _ddr_rzg1_scif_snd((T_COM_SND *)pk_ControlData, pk_SCIFmng);
            break;
        case TA_COM_GETS:
            ercd = _ddr_rzg1_scif_rcv((T_COM_RCV *)pk_ControlData, pk_SCIFmng);
            break;
        default:
            ercd = E_NOSPT;
            break;
    }
    return ercd;
}
#endif
