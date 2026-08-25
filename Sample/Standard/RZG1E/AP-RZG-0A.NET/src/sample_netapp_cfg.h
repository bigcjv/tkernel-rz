/***************************************************************************
    Network Sample Application Configuration
    Copyright (c)  2014, eForce Co., Ltd. All rights reserved.

    2014-04-02: Created.
 ***************************************************************************/
#ifndef _SAMPLE_NETAPP_CFG_H
#define _SAMPLE_NETAPP_CFG_H

#include "net_hdr.h"
#include "net_strlib.h"
#include "shell.h"

extern ER dns_get_ipa_opt(UW dnsd_ipa, VB *str, UW *ipa);

/*****************************************************************************
    Socket ID & Kernel ID
*****************************************************************************/
/* socket ID */
extern SID ID_ICMP_PING;
#ifdef IPV6_SUP
extern SID ID_ICMP_PING6;
#endif
extern SID ID_UDP_DNS;

/*****************************************************************************
    Sample Application Settings
*****************************************************************************/
#define SPL_LF              "\r\n"          /* Line feed code */

/* Default DNS server */
#define SPL_DNS_SERVER      ((192U << 24) | (168U << 16) | (1U << 8) | (251U))      /* 192.168.1.251 */

#endif /* _SAMPLE_NETAPP_CFG_H */
