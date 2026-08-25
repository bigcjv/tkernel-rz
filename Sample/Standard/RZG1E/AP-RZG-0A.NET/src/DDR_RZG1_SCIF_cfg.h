/**
 * @file    DDR_RZG1_SCIF_cfg.h
 * @brief   User configuration for SK-RZG1E Board
 * @date    2016.04.14
 * @author  Copyright (c) 2016, eForce Co.,Ltd.  All rights reserved.
 *
 * @par     History
 *          - rev 1.0 (2016.04.14) i-cho
 *            Initial version. 
 */

#ifndef _DDR_RZG1_SCIF_CFG_H_
#define _DDR_RZG1_SCIF_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * SCIFモジュールのクロックソース(Hz) 
 * P clock = 65MHz (Fixed)
 */
#define CFG_P_CLK   (65000000U)

/* MPUの定義 */
#define CFG_G1E

/* SCIF2割込み番号とレジスタ定義（SCIF2を使用しない場合は不要)
   G1M/N/E とG1Hで異なる */
#if defined(CFG_G1E) || defined(CFG_G1M) || defined(CFG_G1N)
#define REG_SCIF2  (*(volatile struct t_scif *)(PERI_65MHZ_BASE+0x058000U)) /* M/N, E */
#else
#define REG_SCIF2  (*(volatile struct t_scif *)(PERI_65MHZ_BASE+0x056000U)) /* H */
#endif

/* SCIF0の設定値 */
#define CFG_SCIF_0              /* 未使用の場合はコメントアウト             */
#define CFG_TXBUF_SZ0   (1024)  /* 送信バッファサイズ                       */
#define CFG_RXBUF_SZ0   (1024)  /* 受信バッファサイズ                       */
#define CFG_XOFF_SZ0    (512)   /* XOFF送出受信バッファデータ数トリガ       */
#define CFG_XON_SZ0     (64)    /* XON送出受信バッファデータ数トリガ        */
#define CFG_RSTRG_0     (14)    /* RTS#出力アクティブトリガ(1,4,6,8,10,12,14,15)    */
#define CFG_RTRG_0      (8)     /* レシーブFIFOデータ数トリガ (1,4,8,14)  */
#define CFG_TTRG_0      (4)     /* トランスミットFIFOデータ数トリガ(0,2,4,8) */
#define CFG_INT_0       (INT_SCIF0)    /* 割込み番号   */
#define CFG_IPL_0       (240)   /* 割込み優先度 (8,16,...,240,248)          */
#define CFG_CLK_0       (CFG_P_CLK)
#define CFG_CTSRTS_0    (0)     /* CTS/RTS未使用                            */

/* SCIF1の設定値 */
/* #define CFG_SCIF_1 */        /* 未使用の場合はコメントアウト             */
#define CFG_TXBUF_SZ1   (1024)  /* 送信バッファサイズ                       */
#define CFG_RXBUF_SZ1   (1024)  /* 受信バッファサイズ                       */
#define CFG_XOFF_SZ1    (512)   /* XOFF送出受信バッファデータ数トリガ       */
#define CFG_XON_SZ1     (64)    /* XON送出受信バッファデータ数トリガ        */
#define CFG_RSTRG_1     (14)    /* RTS#出力アクティブトリガ(1,4,6,8,10,12,14,15)    */
#define CFG_RTRG_1      (8)     /* レシーブFIFOデータ数トリガ (1,4,8,14)  */
#define CFG_TTRG_1      (4)     /* トランスミットFIFOデータ数トリガ(0,2,4,8) */
#define CFG_INT_1       (INT_SCIF1)    /* 割込み番号   */
#define CFG_IPL_1       (240)   /* 割込み優先度 (8,16,...,240,248)          */
#define CFG_CLK_1       (CFG_P_CLK)
#define CFG_CTSRTS_1    (0)     /* CTS/RTS未使用                            */

/* SCIF2の設定値 */
/* #define CFG_SCIF_2 */        /* 未使用の場合はコメントアウト             */
#define CFG_TXBUF_SZ2   (1024)  /* 送信バッファサイズ                       */
#define CFG_RXBUF_SZ2   (1024)  /* 受信バッファサイズ                       */
#define CFG_XOFF_SZ2    (512)   /* XOFF送出受信バッファデータ数トリガ       */
#define CFG_XON_SZ2     (64)    /* XON送出受信バッファデータ数トリガ        */
#define CFG_RSTRG_2     (14)    /* RTS#出力アクティブトリガ(1,4,6,8,10,12,14,15)    */
#define CFG_RTRG_2      (8)     /* レシーブFIFOデータ数トリガ (1,4,8,14)  */
#define CFG_TTRG_2      (4)     /* トランスミットFIFOデータ数トリガ(0,2,4,8) */
#define CFG_INT_2       (INT_SCIF2)    /* 割込み番号   */
#define CFG_IPL_2       (240)   /* 割込み優先度 (8,16,...,240,248)          */
#define CFG_CLK_2       (CFG_P_CLK)
#define CFG_CTSRTS_2    (0)     /* CTS/RTS未使用                            */

/* SCIF3の設定値 */
/* #define CFG_SCIF_3 */        /* 未使用の場合はコメントアウト             */
#define CFG_TXBUF_SZ3   (1024)  /* 送信バッファサイズ                       */
#define CFG_RXBUF_SZ3   (1024)  /* 受信バッファサイズ                       */
#define CFG_XOFF_SZ3    (512)   /* XOFF送出受信バッファデータ数トリガ       */
#define CFG_XON_SZ3     (64)    /* XON送出受信バッファデータ数トリガ        */
#define CFG_RSTRG_3     (14)    /* RTS#出力アクティブトリガ(1,4,6,8,10,12,14,15)    */
#define CFG_RTRG_3      (8)     /* レシーブFIFOデータ数トリガ (1,4,8,14)  */
#define CFG_TTRG_3      (4)     /* トランスミットFIFOデータ数トリガ(0,2,4,8) */
#define CFG_INT_3       (INT_SCIF3)    /* 割込み番号   */
#define CFG_IPL_3       (240)   /* 割込み優先度 (8,16,...,240,248)          */
#define CFG_CLK_3       (CFG_P_CLK)
#define CFG_CTSRTS_3    (0)     /* CTS/RTS未使用                            */

/* SCIF4の設定値 */
/* #define CFG_SCIF_4 */        /* 未使用の場合はコメントアウト             */
#define CFG_TXBUF_SZ4   (1024)  /* 送信バッファサイズ                       */
#define CFG_RXBUF_SZ4   (1024)  /* 受信バッファサイズ                       */
#define CFG_XOFF_SZ4    (512)   /* XOFF送出受信バッファデータ数トリガ       */
#define CFG_XON_SZ4     (64)    /* XON送出受信バッファデータ数トリガ        */
#define CFG_RSTRG_4     (14)    /* RTS#出力アクティブトリガ(1,4,6,8,10,12,14,15)    */
#define CFG_RTRG_4      (8)     /* レシーブFIFOデータ数トリガ (1,4,8,14)  */
#define CFG_TTRG_4      (4)     /* トランスミットFIFOデータ数トリガ(0,2,4,8) */
#define CFG_INT_4       (INT_SCIF4)    /* 割込み番号   */
#define CFG_IPL_4       (240)   /* 割込み優先度 (8,16,...,240,248)          */
#define CFG_CLK_4       (CFG_P_CLK)
#define CFG_CTSRTS_4    (0)     /* CTS/RTS未使用   */ 

/* SCIF5の設定値 */
/* #define CFG_SCIF_5 */        /* 未使用の場合はコメントアウト             */
#define CFG_TXBUF_SZ5   (1024)  /* 送信バッファサイズ                       */
#define CFG_RXBUF_SZ5   (1024)  /* 受信バッファサイズ                       */
#define CFG_XOFF_SZ5    (512)   /* XOFF送出受信バッファデータ数トリガ       */
#define CFG_XON_SZ5     (64)    /* XON送出受信バッファデータ数トリガ        */
#define CFG_RSTRG_5     (14)    /* RTS#出力アクティブトリガ(1,4,6,8,10,12,14,15)    */
#define CFG_RTRG_5      (8)     /* レシーブFIFOデータ数トリガ (1,4,8,14)  */
#define CFG_TTRG_5      (4)     /* トランスミットFIFOデータ数トリガ(0,2,4,8) */
#define CFG_INT_5       (INT_SCIF5)    /* 割込み番号   */
#define CFG_IPL_5       (240)   /* 割込み優先度 (8,16,...,240,248)          */
#define CFG_CLK_5       (CFG_P_CLK)
#define CFG_CTSRTS_5    (0)     /* CTS/RTS未使用   */              

#ifdef __cplusplus
}
#endif
#endif /* _DDR_RZG1_SCIF_CFG_H_ */
