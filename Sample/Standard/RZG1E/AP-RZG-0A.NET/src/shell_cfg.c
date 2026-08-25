/***************************************************************************
    MICRO C CUBE / COMPACT/STANDARD, NETWORK Application
    Simple shell for Telnet Server configuration
    Copyright (c) 2014, eForce Co., Ltd. All rights reserved.
    
    Version Information  2014.03.18: Created
 ***************************************************************************/

#include "kernel.h"
#include "shell.h"

/* Login user table (Max. 256 users) */
const T_SHELL_USR_TBL shell_usr_tbl[] = {
    {"", ""},               /* Anyone can login (No user name,password) */
    {"User", "Password"},
    
    {0x00, 0x00}    /* Terminate mark (Do not change) */
};

/* Macros */
#define EXT_FUNC(x)    extern ER (x) (VP ctrl, INT argc, VB *argv[])

/* Command functions (Include commands) */
EXT_FUNC(shell_cmd_ip);
EXT_FUNC(shell_cmd_quit);
EXT_FUNC(shell_cmd_help);

/* Command functions (User commands) */
/* EXT_FUNC(usr_cmd); */
EXT_FUNC(shell_usr_cmd_ipcfg);
EXT_FUNC(shell_usr_cmd_dns);
EXT_FUNC(shell_usr_cmd_ping);
#ifdef IPV6_SUP
EXT_FUNC(shell_usr_cmd_ping6);
#endif


/* Command table  (Max. 256 commands) */
const T_SHELL_CMD_TBL shell_cmd_tbl[]  = {
    /* Include command (shell_cmd_xxx), User command (shell_usr_xxx) */
    /* func,        cmd_name,  descr,                       usage,  arg_num */

    {shell_cmd_ip,          "ip",       "Display IP Address",   "",         0},
    {shell_usr_cmd_ipcfg,   "ipcfg",    "Configure IP Address", "",         0},
    {shell_usr_cmd_dns,     "dns",      "DNS Resolver",         "<domain name> [type]", 1},
    {shell_usr_cmd_ping,    "ping",     "Ping Request",         "<remote ip> [length]", 1},
#ifdef IPV6_SUP
    {shell_usr_cmd_ping6,   "ping6",    "Ping6 Request",        "<remote ip> [length]", 1},
#endif
    {shell_cmd_quit,        "quit",     "Quit shell",           "",     0},
    {shell_cmd_help,  "?",     "Display help message",          "",         0},
    
    {0x00, 0x00, 0x00, 0x00, 0}    /* Terminate mark (Do not change) */
};

