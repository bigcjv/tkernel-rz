/**
 * @file    DDR_RZG1_SCIF.h
 * @brief   RZ/G1 Serial communication interface with FIFO (SCIF)
 * @date    2016.04.14
 * @author  Copyright (c) 2016, eForce Co.,Ltd.  All rights reserved.
 *
 ********************************************************************
 * @par     History
 *          - rev 1.0 (2016.04.14) i-cho
 *            Initial version.
 ********************************************************************
 */
#ifndef _DDR_RZG1_SCIF_H_
#define _DDR_RZG1_SCIF_H_

#ifdef __cplusplus
extern "C"
{
#endif

typedef union t_rzg1_scif_msts {
    UH          word;
    struct {
        UH          init_flg:1;
        UH          ena_tx:1;
        UH          ena_rx:1;
        UH          sft_flw:1;
        UH          hrd_flw:1;
        UH          sns_brk:1;
        UH          tx_xoff:1;
        UH          rx_xoff:1;
        UH          req_xon_xoff:1;
        UH          er_buf_ovr:1;
        UH          dummy:6;
    } bit;
} T_RZG1_SCIF_MSTS;

typedef struct t_rzg1_scif_mng {
    T_RZG1_SCIF_MSTS    status;
    UH                  flgid;
    UH                  sndp;
    UH                  rcvp;
    UH                  tcnt;
    UH                  rcnt;
    UH                  tsize;
    UH                  rsize;
    UH                  tlockid;
    UH                  rlockid;
    UH                  xoff_size;
    UH                  xon_size;
    UH                  isrid;
    FP                  devhdr;
    volatile struct t_scif *port;
    T_COM_SND           *SndData;
    T_COM_RCV           *RcvData;
    VB                  *tbuf;
    VB                  *rbuf;
    UB                  *sbuf;
    UH                  aux[2];
} T_RZG1_SCIF_MNG;

#define TXI_FLG     0x00000001
#define RXI_FLG     0x00000002
#define TEI_FLG     0x00000004


/**
 * Initializes device driver.
 * @param ID
 * @param t_scifa
 */
ER _ddr_rzg1_scif_init(ID, volatile struct t_scif *);

#ifdef __cplusplus
}
#endif

#endif  /* _DDR_RZG1_SCIF_H_ */
