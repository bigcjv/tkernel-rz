/**
 * @brief   Ethernet driver configuration
 * @date    2016.02.16
 * @author  Copyright (c) 2016, eForce Co.,Ltd.  All rights reserved.
 *
 ********************************************************************
 * @par     History
 *          - rev 1.0 (2016.02.16) i-cho
 *            Initial version.
 */
#if !defined(_DDR_RZG1_ETH_CFG_H_)
#define _DDR_RZG1_ETH_CFG_H_

#include "RZG1_UC3.h"    /* Register definitions */


/* Interrupt priority
   イーサネットの割込み優先度 (8,16,...,240,248) */

#define CFG_ETH_IPL     (224)


/* AXI bus clock (Default = 260000000)
   Ether module のクロック (マクロを設定しない場合は260000000U) */

/* #define CFG_ETH_BUS_CLK (260000000U) */


/* PHY mode
   0 = Auto select mode (Default)
   1 = 10M Half duplex (Manual mode)
   2 = 10M Full/Half (Duplex auto select mode)
   3 = 100M Half duplex (Manual mode)
   4 = 100M Full/Half (Duplex auto select mode)
   PHYのネゴシエーションモード設定 (マクロを設定しない場合は0(自動)) */

/* #define CFG_ETH_PHY_MOD    (0) */


/* Address filter mode
   0 = Filter disable (Perfect filtering) (Default)
   1 = Promiscuous mode (receive all packets)
   フィルタモード設定 (マクロを設定しない場合は0)
   0 = 自分のMACアドレス宛, ブロードキャストを受信
   1 = すべてのフレームを受信 */

/* #define CFG_ETH_FILTER_MOD    (0) */

/* Task priority (Default = 4)
   タスクの優先度 (マクロを設定しない場合は4)
   1. CFG_ETH_TSK_SND_PRI     : 送信タスク
   2. CFG_ETH_TSK_RCV_PRI     : 受信タスク(受信パケットを3へ渡す)
   3. CFG_ETH_TSK_RCV_TCP_PRI : 受信パケットをTCP/IPプロトコルスタックで処理 */

/* #define CFG_ETH_TSK_SND_PRI        (4) */
/* #define CFG_ETH_TSK_RCV_PRI        (4) */
/* #define CFG_ETH_TSK_RCV_TCP_PRI    (4) */

/* PHY address (Default = 1)
   PHYのアドレス設定 (マクロを設定しない場合は1) */

#define CFG_ETH_PHY_ADR    (1)

/* Tx and Rx dma ring buffer count (Default = 8)
   送信と受信バッファの数 (マクロを設定しない場合は8) */

/* #define CFG_ETH_BUF_TX_CNT    (8) */
/* #define CFG_ETH_BUF_RX_CNT    (8) */

/* Place a buffer in the cache area (Default = 0)
   送受信バッファをキャッシュ領域に配置した場合は1を設定
   1 = キャッシュ領域に配置した場合
   0 = 非キャッシュ領域に配置した場合
   (マクロを設定しない場合は 0) */

/* #define CFG_ETH_BUF_CACHE    (0) */

/* Section name of uncached Tx/Rx dma ring buffer (Default = ".uncache")
   送信と受信バッファを配置する非キャッシュ領域のセクション名
   (マクロを設定しない場合は".uncache") */

#define CFG_ETH_BUF_AREA    "UNCACHE"

#endif  /* _DDR_RZG1_ETH_CFG_H_ */
