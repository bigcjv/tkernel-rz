/************************************************************************
    Network Application

    Copyright (c) 2012 eForce Co., Ltd. All rights reserved.

    2012-08-06: Created.
    2015-04-15: Delete ID_CONTENTS_MPF(Not use in this sample)
 ************************************************************************/

#include "kernel.h"
#include "kernel_id.h"
#include "net_hdr.h"
#include "dhcp_client.h"
#include "http_server.h"
#include "ftp_server.h"
#include "shell.h"
#include "net_cfg.h"
#include "net_strlib.h"

/* Configuration */
#define DHCP_ENB      1       /* Enable DHCP */
#define ID_NET_DEV    1       /* Network device number */

/* Task */
void net_console_tsk(VP_INT);
static const T_CTSK net_ctsk_console = {
    TA_HLNG, (VP_INT)0, (FP)net_console_tsk, 6, 0x400, 0, "Shell(UART)"
};

/* Control Block */
static T_SHELL_CTL net_ctl_console;

/*********************************************
    Shell Task for UART Terminal Console
 *********************************************/
void net_console_tsk(VP_INT exinf)
{
    net_memset(&net_ctl_console, 0, sizeof(T_SHELL_CTL));

    net_ctl_console.typ = TYP_COM;
    net_ctl_console.com_id = DID_UART0;

    shell_sta(&net_ctl_console);
}

/*******************************
    HTTP Contents
 *******************************/
const char index_html[] =
    "<html>\
    <title> ::: uNet3 HTTP Server ::: </title>\
    <body>\
    <br>\
    <center>\
    <table border=2 width=75%><tr><th><bold>uNet3 HTTP Server</bold></th></tr></table>\
    <br><br>\
    <form method=POST action=\"cgi_val.cgi\">\
    <tb>\
    <tr>cgi_val</tr>\
    <tr><input type=TEXT name=cgi_val value=1></tr>\
    <tr><input type=SUBMIT value=Set></tr>\
    </tb>\
    </form>\
    </center>\
    <hr>\
    <br><br><div align=\"right\"><font size=3>Powered by uC3, eForce Co.,Ltd.</font></div>\
    </body>\
    </html>";

/*******************************
    HTTP Content List
 *******************************/
extern void CgiScript(T_HTTP_SERVER *http);

T_HTTP_FILE const content_list[] =
{
    {"/", "text/html", index_html, sizeof(index_html), NULL},
    {"/cgi_val.cgi", "", NULL, 0, CgiScript},
    {"", "", NULL, 0, NULL}
};

/*******************************
    HTTP Server Task
 *******************************/
ID ID_SOC_HTTP;
T_HTTP_SERVER net_ctl_https;
extern const T_CMPF c_net_mpf;

void httpd_tsk(VP_INT exinf)
{
    net_memset(&net_ctl_https, 0, sizeof(net_ctl_https));

    gHTTP_FILE = (T_HTTP_FILE*)content_list;
    net_ctl_https.ver       = IP_VER4;
    net_ctl_https.SocketID  = ID_SOC_HTTP;
    http_server(&net_ctl_https);
}

const T_CTSK net_ctsk_https = {TA_HLNG|TA_ACT, (VP_INT)0, (FP)httpd_tsk, 6, 0x400, 0, "HttpServerTask"};

/*******************************
    FTP Server Task
 *******************************/
ID ID_SOC_FTP_CTL;
ID ID_SOC_FTP_DATA;
T_FTP_SERVER net_ctl_ftps;

void ftp_tsk(VP_INT exinf)
{
    net_memset(&net_ctl_ftps, 0, sizeof(net_ctl_ftps));

    net_ctl_ftps.dev_num = ID_NET_DEV;
    net_ctl_ftps.ctl_sid = ID_SOC_FTP_CTL;
    net_ctl_ftps.dat_sid = ID_SOC_FTP_DATA;

    ftp_server(&net_ctl_ftps);
}

const T_CTSK net_ctsk_ftps = {TA_HLNG|TA_ACT, (VP_INT)0, (FP)ftp_tsk, 6, 0x800, 0, "FtpServerTask"};


/*******************************
    Socket ID Initialization (Generate Configurator)
 *******************************/
ID ID_UDP_DHCP;
ID ID_ICMP_PING;
#ifdef IPV6_SUP
ID ID_ICMP_PING6;
#endif
ID ID_UDP_DNS;

ER net_ini_soc(void)
{
    ER ercd;
    T_NODE lo_host;

    lo_host.num = ID_NET_DEV;
    lo_host.ver = IP_VER4;
    lo_host.ipa = INADDR_ANY;


    /* DHCPc */
    lo_host.port = 68;      /* BOOTP PORT */
    ercd = cre_soc(IP_PROTO_UDP, &lo_host);
    if (0 >= ercd) {
        return ercd;
    }
    ID_UDP_DHCP = ercd;
    cfg_soc(ercd, SOC_TMO_SND, (VP)5000);
    cfg_soc(ercd, SOC_TMO_RCV, (VP)5000);

    /* ---- */
    lo_host.port = 0;   /* port num set is application */

    /* PING */
    ercd = cre_soc(IP_PROTO_ICMP, &lo_host);
    if (0 >= ercd) {
        return ercd;
    }
    ID_ICMP_PING = ercd;

#ifdef IPV6_SUP
    /* PING6 */
    ercd = cre_soc(IP_PROTO_ICMPV6, &lo_host);
    if (0 >= ercd) {
        return ercd;
    }
    ID_ICMP_PING6 = ercd;
#endif

    /* DNSc */
    ercd = cre_soc(IP_PROTO_UDP, &lo_host);
    if (0 >= ercd) {
        return ercd;
    }
    ID_UDP_DNS = ercd;

    /* ---- */
    /* HTTPd */
    lo_host.port = 80;
    ercd = cre_soc(IP_PROTO_TCP, &lo_host);
    if (0 >= ercd) {
        return ercd;
    }
    ID_SOC_HTTP = ercd;
    cfg_soc(ercd, SOC_TMO_SND, (VP)25000);
    cfg_soc(ercd, SOC_TMO_RCV, (VP)25000);
    cfg_soc(ercd, SOC_TMO_CON, (VP)25000);
    cfg_soc(ercd, SOC_TMO_CLS, (VP)25000);

    /* FTPd */
    lo_host.port = 21;
    ercd = cre_soc(IP_PROTO_TCP, &lo_host);
    if (0 >= ercd) {
        return ercd;
    }
    ID_SOC_FTP_CTL = ercd;
    cfg_soc(ercd, SOC_TMO_SND, (VP)5000);
    cfg_soc(ercd, SOC_TMO_RCV, (VP)5000);
    cfg_soc(ercd, SOC_TMO_CON, (VP)5000);

    lo_host.port = 20;
    ercd = cre_soc(IP_PROTO_TCP, &lo_host);
    if (0 >= ercd) {
        return ercd;
    }
    ID_SOC_FTP_DATA = ercd;

    return E_OK;
}

extern void puts_com_opt(VB *msg);  /* from main.c */

/*******************************
    Network Initialization
 *******************************/
ER net_setup(void)
{
    ER ercd;
    #if (DHCP_ENB == 1)
    T_HOST_ADDR dhcp_addr;
    INT i;
    #endif

    /* Initialize TCP/IP Stack */
    ercd = net_ini();
    if (ercd != E_OK) {
        return ercd;
    }

    /* Initialize Ethernet Driver */
    ercd = net_dev_ini(ID_NET_DEV);
    if (ercd != E_OK) {
        return ercd;
    }
    dly_tsk(1000);

    /* Initialize Socket ID */
    ercd = net_ini_soc();
    if (ercd != E_OK) {
        return ercd;
    }

    /* DHCP Client */
    #if (DHCP_ENB == 1)
    dly_tsk(2000);
    puts_com_opt("\r\nDHCP: ");
    net_memset(&dhcp_addr, 0, sizeof(dhcp_addr));
    dhcp_addr.socid = ID_UDP_DHCP;
    for (i = 0; i < 3; i++) {
        ercd = dhcp_client(&dhcp_addr);
        if (ercd == E_OK) {
            break;
        }
        puts_com_opt("Retry ");
    }
    if (ercd == E_OK) {
        puts_com_opt("Success\r\n\r\n");
    } else {
        puts_com_opt("Not Found\r\n\r\n");
    }
    #endif

    /* Starting Server Task  */
    gHTTP_FILE = (T_HTTP_FILE*)content_list;
    ercd = acre_tsk((T_CTSK*)&net_ctsk_https);
    if (ercd <= E_OK) {
        return ercd;
    }
    act_tsk((ID)ercd);

    ercd = acre_tsk((T_CTSK*)&net_ctsk_ftps);
    if (ercd <= E_OK) {
        return ercd;
    }
    act_tsk((ID)ercd);

    return E_OK;
}

ER net_sta_console(void)
{
    ER ercd;

    /* Create Shell Task */
    ercd = acre_tsk((T_CTSK*)&net_ctsk_console);
    if (ercd <= E_OK) {
        return ercd;
    }

    /* Start for UART terminal console */
    ercd = sta_tsk((ID)ercd, 0x00);

    return ercd;
}
