/***************************************************************************
    MICRO C CUBE / COMPACT/STANDARD, NETWORK Application
    FTP Server Configuration
    Copyright (c) 2014-2017, eForce Co., Ltd. All rights reserved.

    Version Information
      2014.03.18: Created
 ***************************************************************************/

#include "ffsys.h"    /* File system */

/* Configuration */
#define CFG_FTPS_DRV_NAME        'C'                 /* Drive name */
#define CFG_FTPS_PATH_MAX        PATH_MAX            /* Maximum length of file path */
#define CFG_FTPS_CMD_TMO         5000                /* 5 sec */
#define CFG_FTPS_DAT_TMO         5000                /* 5 sec */
#define CFG_FTPS_IDLE_TMO        (5 * 60 * 1000)     /* 5 minute */
#define CFG_FTPS_SES_NUM         1                   /* Number of sessionss */
