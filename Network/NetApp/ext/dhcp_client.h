/***************************************************************************
    MICRO C CUBE / COMPACT/STANDARD, NETWORK Application
    DHCP Client header file
    Copyright (c)  2008-2015, eForce Co., Ltd. All rights reserved.

    Version Information
      2008.11.30: Created
      2011.11.24: Supported RENEW, RELEASE, DECLINE, INFORM 
      2012.10.02: Accepted more than two DNS server address
      2014.04.11: Corrected to "UH" a type of "dev_num".
      2015.05.01: Add the feature of any options reference.
 ***************************************************************************/

#ifndef DHCP_CLIENT_H
#define DHCP_CLIENT_H
#ifdef __cplusplus
extern "C" {
#endif

/*----DHCP Configurables------------------------------------*/
#define DHCP_SND_TMO    5000    /* UDP Tx time in sec   */
#define DHCP_RCV_TMO    5000    /* UDP Rx time in sec   */
#define DHCP_RETRY_CNT     3    /* DHCP msg retry count */
    
#define ENA_DHCP_UOPT           /* Enabled user option feature */
/*----DHCP Configurables------------------------------------*/

/* DHCP UDP Port                */

#define DHCP_SERVER_PORT        67
#define DHCP_CLIENT_PORT        68

/* DHCP State                   */

#define DHCP_STS_INIT           0
#define DHCP_STS_INITREBOOT     1
#define DHCP_STS_REBOOTING      2
#define DHCP_STS_REQUESTING     3
#define DHCP_STS_BOUND          4
#define DHCP_STS_SELECTING      5
#define DHCP_STS_REBINDING      6
#define DHCP_STS_RENEWING       7

/* DHCP Messsage */

typedef struct t_dhcp_msg {
  UB    op;
  UB    htype;
  UB    hlen;
  UB    hops;
  UW    xid;
  UH    secs;
  UH    flags;
  UW    ciaddr;
  UW    yiaddr;
  UW    siaddr;
  UW    giaddr;
  char  chaddr[16];
  char  sname[64];
  char  file[128];
  UB    opt[312];
}T_DHCP_MSG;

#define DHCP_HDR_LEN        236
#define DHCP_MSG_SZ         548

/* DHCP Message Fields      */
#define DHCP_OPC_BOOTREQ    1
#define DHCP_OPC_BOOTREPLY  2
#define DHCP_ETH_TYPE       1   /*Ethernet(10MB) IANA:arp-parameters*/
#define DHCP_ETH_LEN        6
#define DHCP_FLG_BCAST      0x8000

/* DHCP Messages Type (RFC 2132)*/
#define DHCP_MSG_DISCOVER   1
#define DHCP_MSG_OFFER      2
#define DHCP_MSG_REQUEST    3
#define DHCP_MSG_DECLINE    4
#define DHCP_MSG_ACK        5
#define DHCP_MSG_NAK        6
#define DHCP_MSG_RELEASE    7
#define DHCP_MSG_INFORM     8

/* DHCP Options */
#define DHCP_OPT_PAD            0   /*:1*/
#define DHCP_OPT_SUBNET         1   /*:4*/
#define DHCP_OPT_ROUTER         3   /*:4n*/
#define DHCP_OPT_DNS            6   /*:4n*/
#define DHCP_OPT_REQIPADDR      50  /*:4*/
#define DHCP_OPT_IPLEASE        51  /*:1*/
#define DHCP_OPT_DHCPMSGTYPE    53  /*:1*/
#define DHCP_OPT_SERVERIDENT    54  /*:4*/
#define DHCP_OPT_PRMLST          55  /* Parameter Request List */
#define DHCP_OPT_RENETM          58  /*:4 Renewal Time */
#define DHCP_OPT_REBITM          59  /*:4 Rebinding Time */
#define DHCP_OPT_CLIENT          61  /*:n Client ID */
#define DHCP_OPT_END            255 /*:1*/


#ifdef ACD_SUP
#define ARP_CHECK_OFF       0
#define ARP_CHECK_ON        1
#endif

#define DNS_SERVER_NUM      2

typedef struct t_dhcp_ctl {
    UW  server;     /* DHCP server address  */
    UW  xid;        /* Random trans id      */
    UB  *opt_ptr;   /* rcv_msg processing   */
    UH  opt_len;    /* rcv_msg processing   */
    UH  flg;        /* rcv_msg processing   */
    T_DHCP_MSG  *snd_msg;   /* Tx message   */
    T_DHCP_MSG  *rcv_msg;   /* Rx message   */
    T_NET_BUF   *pkt[2];    /* top address  */
}T_DHCP_CTL;

#ifdef ENA_DHCP_UOPT
#define DHCP_UOPT_STR       0x80    /* string */
#define DHCP_UOPT_IPA       0x40    /* address */
#define DHCP_UOPT_BIN       0x20    /* binary */

#define DHCP_UOPT_STS_SET   0x01    /* set option */

/* User settins structure */
typedef struct t_dhcp_uopt {
    UB code;
    UB len;
    UB ary;
    UB flag;
    VP val;
} T_DHCP_UOPT;

/* User settings macros */
#define SET_DHCP_UOPT(_uopt_,_code_,_pval_,_len_,_ary_,_type_) \
    _uopt_.len  = _len_;    _uopt_.ary  = _ary_;    _uopt_.flag = _type_;    \
    _uopt_.code = _code_;   _uopt_.val  = _pval_;
    
/* for single element */
#define SET_DHCP_UOPT_BIN1(_uopt_,_code_,_pval_)        SET_DHCP_UOPT(_uopt_,_code_,_pval_,1,1,DHCP_UOPT_BIN)
#define SET_DHCP_UOPT_BIN2(_uopt_,_code_,_pval_)        SET_DHCP_UOPT(_uopt_,_code_,_pval_,2,1,DHCP_UOPT_BIN)
#define SET_DHCP_UOPT_BIN4(_uopt_,_code_,_pval_)        SET_DHCP_UOPT(_uopt_,_code_,_pval_,4,1,DHCP_UOPT_BIN)
#define SET_DHCP_UOPT_IPA(_uopt_,_code_,_pval_)         SET_DHCP_UOPT(_uopt_,_code_,_pval_,4,1,DHCP_UOPT_IPA)
#define SET_DHCP_UOPT_STR(_uopt_,_code_,_buf_,_len_)    SET_DHCP_UOPT(_uopt_,_code_,_buf_,1,_len_,DHCP_UOPT_STR)

/* for multiple elements */
#define SET_DHCP_UOPTS_BIN1(_uopt_,_code_,_pval_,_len_)     SET_DHCP_UOPT(_uopt_,_code_,_pval_,1,_len_,DHCP_UOPT_BIN)
#define SET_DHCP_UOPTS_BIN2(_uopt_,_code_,_pval_,_len_)     SET_DHCP_UOPT(_uopt_,_code_,_pval_,2,_len_,DHCP_UOPT_BIN)
#define SET_DHCP_UOPTS_BIN4(_uopt_,_code_,_pval_,_len_)     SET_DHCP_UOPT(_uopt_,_code_,_pval_,4,_len_,DHCP_UOPT_BIN)
#define SET_DHCP_UOPTS_IPA(_uopt_,_code_,_pval_,_len_)      SET_DHCP_UOPT(_uopt_,_code_,_pval_,4,_len_,DHCP_UOPT_IPA)

#endif

typedef struct t_dhcp_client {
    T_DHCP_CTL ctl;
    UW ipaddr;  /* Host addres          */
    UW subnet;  /* Subnet               */
    UW gateway; /* Gateway              */
    UW dhcp;    /* DHCP server address  */
    UW dns[DNS_SERVER_NUM];
                /* DNS  server address  */
    UW lease;   /* IP Lease time        */
    UW t1;      /* Renew  Time          */
    UW t2;      /* Rebind Time          */
    UB mac[6];  /* Interface HW address */
    UH dev_num; /* Interface number     */
    UB state;   /* DHCP client state    */
    UH socid;   /* UDP socket           */
#ifdef ACD_SUP
    UB arpchk;  /* Do ARP Check*/
#endif
#ifdef ENA_DHCP_UOPT
    T_DHCP_UOPT *uopt;  /* DHCP user options */
    UB uopt_len;
#endif
}T_DHCP_CLIENT;

/* DHCP Client API's */
ER dhcp_bind(T_DHCP_CLIENT *dhcp);
ER dhcp_renew(T_DHCP_CLIENT *dhcp);
ER dhcp_reboot(T_DHCP_CLIENT *dhcp);
ER dhcp_release(T_DHCP_CLIENT *dhcp);
ER dhcp_inform(T_DHCP_CLIENT *dhcp);

/* API (older version) */
#define T_HOST_ADDR T_DHCP_CLIENT
ER dhcp_client(T_DHCP_CLIENT *dhcp);

#ifdef __cplusplus
}
#endif
#endif /* DHCP_CLIENT_H */

