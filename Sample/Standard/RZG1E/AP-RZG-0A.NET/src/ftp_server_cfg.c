/***************************************************************************
    MICRO C CUBE / COMPACT/STANDARD, NETWORK Application
    FTP Server Configuration
    Copyright (c) 2014, eForce Co., Ltd. All rights reserved.

    Version Information  2014.03.18: Created
 ***************************************************************************/

#include "kernel.h"
#include "net_hdr.h"
#include "ftp_server.h"

/* Login user table (Max. 256 users) (DEV_ANY: All device is allowed) */
const T_FTP_USR_TBL ftp_usr_tbl[] = {
    {DEV_ANY, "", ""},                /* Anyone can login (No user name,password) */
    {DEV_ANY, "User", "Password"},
    
    {0x00, 0x00, 0x00}    /* Terminate mark (Do not change) */
};

