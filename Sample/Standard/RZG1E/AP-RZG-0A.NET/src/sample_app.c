/***************************************************************************
    NetApp Sample (Operating as command in shell)
    Copyright (c)  2014, eForce Co., Ltd. All rights reserved.

    2014-04-02: Created.
 ***************************************************************************/
#include "kernel.h"
#include "sample_netapp_cfg.h"      /* include Sample Application Settings */
#include "ping_client.h"
#include "dns_client.h"


/* Extend of dns_get_ipaddr (include ip_aton) */
ER dns_get_ipa_opt(UW dnsd_ipa, VB *str, UW *ipa)
{
    ER ercd;
    
    *ipa = ip_aton(str);
    if (*ipa != 0U) {
        return E_OK;
    }

    ercd = dns_get_ipaddr(ID_UDP_DNS, dnsd_ipa, str, ipa);
    if (E_OK != ercd) {
        *ipa = ip_aton(str);
        if (*ipa) {
            ercd = E_OK;
        }
    }

    return ercd;
}

/* Command 'ipcfg' */
ER shell_usr_cmd_ipcfg(VP ctrl, INT argc, VB *argv[])
{
    T_NET_ADR adr;
    ER ercd;
    UW tmp;
    VB buf[32];

    ercd = net_ref(0, NET_IP4_CFG, (VP)&adr);
    if (ercd != E_OK) {
        return ercd;
    }

    shell_puts(ctrl, SPL_LF" Enter new values..");

    shell_puts(ctrl, SPL_LF" IP Address  : ");
    shell_gets(ctrl, (char *)buf, sizeof(buf));
    tmp = ip_aton((char *)buf);
    if (tmp != 0) adr.ipaddr = tmp;

    shell_puts(ctrl, SPL_LF" Subnet Mask : ");
    shell_gets(ctrl, (char *)buf, sizeof(buf));
    tmp = ip_aton((char *)buf);
    if (tmp != 0) adr.mask = tmp;

    shell_puts(ctrl, SPL_LF" Gateway     : ");
    shell_gets(ctrl, (char *)buf, sizeof(buf));
    tmp = ip_aton((char *)buf);
    if (tmp != 0) adr.gateway = tmp;

    net_cfg(0, NET_IP4_CFG, (VP)&adr);
    
    return ercd;
}


/* Command 'ping' */
ER shell_usr_cmd_ping(VP ctrl, INT argc, VB *argv[])
{
    UB cnt;
    ER ercd;
    UW ipa;
    T_PING_CLIENT pingc;

    net_memset(&pingc, 0, sizeof(pingc));
    pingc.sid = ID_ICMP_PING;

    if(argc == 3)
        pingc.len = net_atoi(argv[2]);
    else
        pingc.len = 1000;

    /* hostname */
    ercd = dns_get_ipa_opt(SPL_DNS_SERVER, argv[1], &ipa);
    if (E_OK != ercd) {
        return ercd;
    }

    pingc.devnum = 1;
    pingc.ipa = ipa;
    if (pingc.ipa == 0) {
        shell_puts(ctrl, SPL_LF" invalid ip address");
        return E_OBJ;
    }

    for(cnt=0; cnt<1; cnt++) {
        ercd = ping_client(&pingc);
        if(ercd != E_OK) {
            shell_puts(ctrl, SPL_LF" ping request timed out");
            break;
        }
        shell_puts(ctrl, SPL_LF" ping request successful ");
    }

    return ercd;
}


#ifdef IPV6_SUP
/* Command 'ping6' */
ER shell_usr_cmd_ping6(VP ctrl, INT argc, VB *argv[])
{
    UB cnt;
    ER ercd;
    T_PING_CLIENT_V6 pingc;

    net_memset(&pingc, 0, sizeof(pingc));
    pingc.sid = ID_ICMP_PING6;

    if(argc == 3)
        pingc.len = atoi(argv[2]);
    else
        pingc.len = 1000;
        
    pingc.devnum = 1;
    ip6_aton(argv[1], pingc.ip6addr);
    for(cnt=0; cnt<1; cnt++) {
        ercd = ping6_client(&pingc);
        if(ercd != E_OK) {
            shell_puts(ctrl, SPL_LF" ping request timed out");
            return;
        }
        shell_puts(ctrl, SPL_LF" ping request successful ");
    }
    
}
#endif

/* Command 'dns' */
ER shell_usr_cmd_dns(VP ctrl, INT argc, VB *argv[])
{
    char ip_str[40];
    UW dns_server, ip[4];
    ER ercd;

    dns_server = SPL_DNS_SERVER;

#ifdef IPV6_SUP    
    if (argc == 3) {
        if(!net_strcmp(argv[2], "AAAA")) {
            ercd = dns_get_ip6addr(0, dns_server, argv[1], ip);
            if (ercd == E_OK) {
                ip6_ntoa(ip_str,ip);
                shell_puts(ctrl, SPL_LF);
                shell_puts(ctrl, ip_str);
            }
            else {
                shell_puts(ctrl, SPL_LF" Error! dns resolver");
            }
        }
        else {
            ercd = E_PAR;
        }
    }
    else
#endif 
    {       
        ercd = dns_get_ipaddr(ID_UDP_DNS, dns_server, argv[1], ip);
        if (ercd == E_OK) {
            ip_ntoa(ip_str, ip[0]);
            shell_puts(ctrl, SPL_LF);
            shell_puts(ctrl, ip_str);
        }
        else {
            shell_puts(ctrl, SPL_LF" Error! dns resolver");
        }
    }

    return ercd;
}
