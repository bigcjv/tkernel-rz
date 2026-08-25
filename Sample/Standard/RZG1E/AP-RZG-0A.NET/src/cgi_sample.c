/************************************************************************
    CGI Script
    
    Copyright (c) 2014 eForce Co., Ltd. All rights reserved.
    
    2014-05-02: Created.
 ************************************************************************/

#include "kernel.h"
#include "net_hdr.h"
#include "http_server.h"
#include "net_strlib.h"

extern void puts_com_opt(VB *msg);  /* from main.c */

/* Response page  */
const char cgi_okay[] =
    "<html>\
    <title> ::: uNet3 HTTP Server ::: </title>\
    <body>\
    <br>\
    <center>\
    <table border=2 width=75%><tr><th><bold>uNet3 HTTP Server</bold></th></tr></table>\
    <br>\
    <p>Setup completed</p>\
    <a href=/>[Return]<a>\
    </center>\
    <hr>\
    <br><br><div align=\"right\"><font size=3>Powered by uC3, eForce Co.,Ltd.</font></div>\
    </body>\
    </html>";

/*******************************
    CGI Script
 *******************************/
UINT led_dly = 1000;

void CgiScript(T_HTTP_SERVER *http)
{
    char *cgi_name[2];
    char *cgi_value[2];
    int  cgi_cnt;
    T_HTTP_HEADER *env = &http->hdr;
    VB buf[32];

    /* Parse the query string to argument list */
    CgiGetParam(env->Content, env->ContentLen, cgi_name, cgi_value, &cgi_cnt);

    /* Get led_dly */
    if (cgi_cnt && net_strcmp(cgi_name[0], "cgi_val") == 0) {
        led_dly = net_atoi(cgi_value[0]);
        puts_com_opt("\r\nCGI: led_dly set to ");
        puts_com_opt(net_itoa(led_dly, buf, 10));
        puts_com_opt("\r\n");
    }

    /* Response page */
    HttpSendText(http, (char *)cgi_okay, sizeof(cgi_okay));

    return;
}

