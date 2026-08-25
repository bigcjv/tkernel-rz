/***************************************************************************
    MICRO C CUBE / COMPACT/STANDARD, NETWORK Application
    SNTP Client header file
    Copyright (c)  2012-2017, eForce Co., Ltd. All rights reserved.

    Version Information
      2012.06.07: Created
      2015.12.14: The socket ID replaced SID types
      2016.07.06: Execute static analysis tool to this source.
      2017.04.12: Added member to T_SNTP_CLIENT structure. (stt)
 ***************************************************************************/

#ifndef SNTP_CLIENT_H
#define SNTP_CLIENT_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "kernel.h"
#include "net_hdr.h"

#define SNTP_PORT           123U
#define NTP_BASE_TIME       2208988800UL    /* 1970-1900 sec */
#define UTC_FRACTION_SCAL   4294967296.0    /* 2^32 */

#define SNTP_TIMEOUT        1000

typedef struct {
    SID sid;
    UW ipa;
    TMO tmo;
    UH devnum;
    UH port;
    UB ipv;
    UB stt;
}T_SNTP_CLIENT;

ER sntp_client(T_SNTP_CLIENT *sc, UW *sec, UW *fra);

#ifdef __cplusplus
}
#endif
#endif

