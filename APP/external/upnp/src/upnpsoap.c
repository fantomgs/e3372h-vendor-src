/* $Id: upnpsoap.c,v 1.88 2011/11/18 11:21:18 nanard Exp $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2006-2011 Thomas Bernard 
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */
 
   /**********************问题单修改记录******************************************
    日期              修改人         问题单号           修改内容
  
 2012.03.14        z00203875     2031401440    修改打LOG方式  
 2012.03.26        z00203875     2032603705    LOG打到SDT中
 2012.04.03        z00203875     2032802867    无法在不删除规则的
                                                     情况下禁用规则 
 2012.08.29        z00203875     2082304944    UPnP认证测试
 2014.11.21        n00202065     4110603649    禁用upnp wan侧连接断开拨号功能
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

#include "config.h"
#include "upnpglobalvars.h"
#include "upnphttp.h"
#include "upnpsoap.h"
#include "upnpreplyparse.h"
#include "upnpredirect.h"
#include "getifaddr.h"
#include "getifstats.h"
#include "getconnstatus.h"
#include "upnpurns.h"
#include "firewallapi.h"
#include "upnpevents.h"

#ifndef USE_MOCKER
#include "appinterface.h"
#endif

static void
BuildSendAndCloseSoapResp(struct upnphttp * h,
                          const char * body, int bodylen)
{
    NP_UPNP_INFO("enter BuildSendAndCloseSoapResp\n");

    static const char beforebody[] =
        "<?xml version=\"1.0\"?>\r\n"
        "<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
        "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
        "<s:Body>";

    static const char afterbody[] =
        "</s:Body>"
        "</s:Envelope>\r\n";

    BuildHeader_upnphttp(h, 200, "OK",  sizeof(beforebody) - 1
        + sizeof(afterbody) - 1 + bodylen );

    memcpy(h->res_buf + h->res_buflen, beforebody, sizeof(beforebody) - 1);
    h->res_buflen += sizeof(beforebody) - 1;

    memcpy(h->res_buf + h->res_buflen, body, bodylen);
    h->res_buflen += bodylen;

    memcpy(h->res_buf + h->res_buflen, afterbody, sizeof(afterbody) - 1);
    h->res_buflen += sizeof(afterbody) - 1;

    SendRespAndClose_upnphttp(h);
}

static void
GetConnectionTypeInfo(struct upnphttp * h, const char * action)
{
    static const char resp[] =
        "<u:GetConnectionTypeInfoResponse "
        "xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
        "<NewConnectionType>IP_Routed</NewConnectionType>"
        "<NewPossibleConnectionTypes>IP_Routed</NewPossibleConnectionTypes>"
        "</u:GetConnectionTypeInfoResponse>";
    BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
}

static void
GetTotalBytesSent(struct upnphttp * h, const char * action)
{
    int r;

    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<NewTotalBytesSent>%lu</NewTotalBytesSent>"
        "</u:%sResponse>";

    char body[512];
    int bodylen;
    struct ifdata data;

    r = getifstats(ext_if_name, &data);
    bodylen = snprintf(body, sizeof(body), resp,
             action, "urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1",
             r<0?0:data.obytes, action);
    BuildSendAndCloseSoapResp(h, body, bodylen);
}

static void
GetTotalBytesReceived(struct upnphttp * h, const char * action)
{
    int r;

    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<NewTotalBytesReceived>%lu</NewTotalBytesReceived>"
        "</u:%sResponse>";

    char body[512];
    int bodylen;
    struct ifdata data;

    r = getifstats(ext_if_name, &data);
    bodylen = snprintf(body, sizeof(body), resp,
             action, "urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1",
             r<0?0:data.ibytes, action);
    BuildSendAndCloseSoapResp(h, body, bodylen);
}

static void
GetTotalPacketsSent(struct upnphttp * h, const char * action)
{
    int r;

    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<NewTotalPacketsSent>%lu</NewTotalPacketsSent>"
        "</u:%sResponse>";

    char body[512];
    int bodylen;
    struct ifdata data;

    r = getifstats(ext_if_name, &data);
    bodylen = snprintf(body, sizeof(body), resp,
             action, "urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1",
             r<0?0:data.opackets, action);
    BuildSendAndCloseSoapResp(h, body, bodylen);
}

static void
GetTotalPacketsReceived(struct upnphttp * h, const char * action)
{
    int r;

    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<NewTotalPacketsReceived>%lu</NewTotalPacketsReceived>"
        "</u:%sResponse>";

    char body[512];
    int bodylen;
    struct ifdata data;

    r = getifstats(ext_if_name, &data);
    bodylen = snprintf(body, sizeof(body), resp,
             action, "urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1",
             r<0?0:data.ipackets, action);
    BuildSendAndCloseSoapResp(h, body, bodylen);
}

static void
GetCommonLinkProperties(struct upnphttp * h, const char * action)
{
    /* WANAccessType : set depending on the hardware :
     * DSL, POTS (plain old Telephone service), Cable, Ethernet */
    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        /*"<NewWANAccessType>DSL</NewWANAccessType>"*/
        "<NewWANAccessType>Cable</NewWANAccessType>"
        "<NewLayer1UpstreamMaxBitRate>%lu</NewLayer1UpstreamMaxBitRate>"
        "<NewLayer1DownstreamMaxBitRate>%lu</NewLayer1DownstreamMaxBitRate>"
        "<NewPhysicalLinkStatus>%s</NewPhysicalLinkStatus>"
        "</u:%sResponse>";

    char body[2048];
    int bodylen;
    struct ifdata data;
    const char * status = "Up"; /* Up, Down (Required),
                                 * Initializing, Unavailable (Optional) */
    char ext_ip_addr[INET_ADDRSTRLEN];

    if((downstream_bitrate == 0) || (upstream_bitrate == 0))
    {
        if(getifstats(ext_if_name, &data) >= 0)
        {
            if(downstream_bitrate == 0) downstream_bitrate = data.baudrate;
            if(upstream_bitrate == 0) upstream_bitrate = data.baudrate;
        }
    }
    if(getifaddr(ext_if_name, ext_ip_addr, INET_ADDRSTRLEN) < 0) {
        status = "Down";
    }
    bodylen = snprintf(body, sizeof(body), resp,
        action, "urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1",
        upstream_bitrate, downstream_bitrate,
        status, action);
    BuildSendAndCloseSoapResp(h, body, bodylen);
}

static void
GetStatusInfo(struct upnphttp * h, const char * action)
{
    NP_UPNP_INFO("enter GetStatusInfo\n");
    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<NewConnectionStatus>%s</NewConnectionStatus>"
        "<NewLastConnectionError>ERROR_NONE</NewLastConnectionError>"
        "<NewUptime>%ld</NewUptime>"
        "</u:%sResponse>";

    char body[512];
    int bodylen;
    time_t uptime;
    const char * status = "Connected";
    /* ConnectionStatus possible values :
     * Unconfigured, Connecting, Connected, PendingDisconnect,
     * Disconnecting, Disconnected */
    char ext_ip_addr[16];
    if(getifaddr(ext_if_name, ext_ip_addr, 16) < 0) {
        status = "Disconnected";
    }
    status = get_wan_connection_status_str(ext_if_name);//需要修改
    uptime = (time(NULL) - startup_time);
    bodylen = snprintf(body, sizeof(body), resp,
        action, "urn:schemas-upnp-org:service:WANIPConnection:1",
        status, (long)uptime, action);  
    BuildSendAndCloseSoapResp(h, body, bodylen);
    NP_UPNP_INFO("leave GetStatusInfo\n");
}

static void
GetNATRSIPStatus(struct upnphttp * h, const char * action)
{
    NP_UPNP_INFO("enter GetNATRSIPStatus\n");

    static const char resp[] =
        "<u:GetNATRSIPStatusResponse "
        "xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
        "<NewRSIPAvailable>0</NewRSIPAvailable>"
        "<NewNATEnabled>1</NewNATEnabled>"
        "</u:GetNATRSIPStatusResponse>";
    /* 2.2.9. RSIPAvailable
     * This variable indicates if Realm-specific IP (RSIP) is available
     * as a feature on the InternetGatewayDevice. RSIP is being defined
     * in the NAT working group in the IETF to allow host-NATing using
     * a standard set of message exchanges. It also allows end-to-end
     * applications that otherwise break if NAT is introduced
     * (e.g. IPsec-based VPNs).
     * A gateway that does not support RSIP should set this variable to 0. */
    BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
    NP_UPNP_INFO("leave GetNATRSIPStatus\n");
}

static void
GetExternalIPAddress(struct upnphttp * h, const char * action)
{
    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<NewExternalIPAddress>%s</NewExternalIPAddress>"
        "</u:%sResponse>";

    char body[512];
    int bodylen;
    char ext_ip_addr[INET_ADDRSTRLEN];
    /* Does that method need to work with IPv6 ?
     * There is usually no NAT with IPv6 */

#ifndef MULTIPLE_EXTERNAL_IP
    if(use_ext_ip_addr)
    {
        strncpy(ext_ip_addr, use_ext_ip_addr, INET_ADDRSTRLEN);
    }
    else if(getifaddr(ext_if_name, ext_ip_addr, INET_ADDRSTRLEN) < 0)
    {
        NP_UPNP_ERROR("Failed to get ip address for interface %s",
            ext_if_name);
        strncpy(ext_ip_addr, "0.0.0.0", INET_ADDRSTRLEN);
    }
#else
    struct lan_addr_s * lan_addr;
    strncpy(ext_ip_addr, "0.0.0.0", INET_ADDRSTRLEN);
    for(lan_addr = lan_addrs.lh_first; lan_addr != NULL; lan_addr = lan_addr->list.le_next)
    {
        if( (h->clientaddr.s_addr & lan_addr->mask.s_addr)
           == (lan_addr->addr.s_addr & lan_addr->mask.s_addr))
        {
            strncpy(ext_ip_addr, lan_addr->ext_ip_str, INET_ADDRSTRLEN);
            break;
        }
    }
#endif
    bodylen = snprintf(body, sizeof(body), resp,
                  action, "urn:schemas-upnp-org:service:WANIPConnection:1",
                  ext_ip_addr, action);
    BuildSendAndCloseSoapResp(h, body, bodylen);
}

/* AddPortMapping method of WANIPConnection Service
 * Ignored argument : NewEnabled */
static void
AddPortMapping(struct upnphttp * h, const char * action)
{
    int r;

    static const char resp[] =
        "<u:AddPortMappingResponse "
        "xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\"/>";

    struct NameValueParserData data;
    char * int_ip, * int_port, * ext_port, * protocol, * desc;
    char * leaseduration_str;
    unsigned int leaseduration;
    char * r_host;
    char * enabled_str;
    unsigned short iport, eport;

    struct hostent *hp; /* getbyhostname() */
    char ** ptr; /* getbyhostname() */
    struct in_addr result_ip;/*unsigned char result_ip[16];*/ /* inet_pton() */

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    int_ip = GetValueFromNameValueList(&data, "NewInternalClient");
    if (!int_ip)
    {
        ClearNameValueList(&data);
        SoapError(h, 402, "Invalid Args");
        return;
    }

    /* IGD 2 MUST support both wildcard and specific IP address values
     * for RemoteHost (only the wildcard value was REQUIRED in release 1.0) */
    r_host = GetValueFromNameValueList(&data, "NewRemoteHost");
#ifndef SUPPORT_REMOTEHOST
#ifdef UPNP_STRICT
    if (r_host && (strlen(r_host) > 0) && (0 != strcmp(r_host, "*")))
    {
        ClearNameValueList(&data);
        SoapError(h, 726, "RemoteHostOnlySupportsWildcard");
        return;
    }
#endif
#endif

    /* if ip not valid assume hostname and convert */
    if (inet_pton(AF_INET, int_ip, &result_ip) <= 0) 
    {
        hp = gethostbyname(int_ip);
        if(hp && hp->h_addrtype == AF_INET) 
        { 
            for(ptr = hp->h_addr_list; ptr && *ptr; ptr++)
            {
                int_ip = inet_ntoa(*((struct in_addr *) *ptr));
                result_ip = *((struct in_addr *) *ptr);
                /* TODO : deal with more than one ip per hostname */
                break;
            }
        } 
        else 
        {
            NP_UPNP_ERROR("Failed to convert hostname '%s' to ip address", int_ip); 
            ClearNameValueList(&data);
            SoapError(h, 402, "Invalid Args");
            return;
        }               
    }

    /* check if NewInternalAddress is the client address */
    if(GETFLAG(SECUREMODEMASK))
    {
        if(h->clientaddr.s_addr != result_ip.s_addr)
        {
            NP_UPNP_DEBUG("Client %s tried to redirect port to %s",
                   inet_ntoa(h->clientaddr), int_ip);
            ClearNameValueList(&data);
            SoapError(h, 718, "ConflictInMappingEntry");
            return;
        }
    }

    int_port = GetValueFromNameValueList(&data, "NewInternalPort");
    ext_port = GetValueFromNameValueList(&data, "NewExternalPort");
    protocol = GetValueFromNameValueList(&data, "NewProtocol");
    desc = GetValueFromNameValueList(&data, "NewPortMappingDescription");
    leaseduration_str = GetValueFromNameValueList(&data, "NewLeaseDuration");
    enabled_str = GetValueFromNameValueList(&data, "NewEnabled");
    NP_UPNP_DEBUG("enabled_str == %s\n", enabled_str);
    NP_UPNP_DEBUG("atoi(enabled_str) == %d\n", atoi(enabled_str));

    if (!int_port || !ext_port || !protocol)
    {
        ClearNameValueList(&data);
        SoapError(h, 402, "Invalid Args");
        return;
    }

    eport = (unsigned short)atoi(ext_port);
    iport = (unsigned short)atoi(int_port);

    leaseduration = leaseduration_str ? atoi(leaseduration_str) : 0;
#ifdef IGD_V2
    /* PortMappingLeaseDuration can be either a value between 1 and
     * 604800 seconds or the zero value (for infinite lease time).
     * Note that an infinite lease time can be only set by out-of-band
     * mechanisms like WWW-administration, remote management or local
     * management.
     * If a control point uses the value 0 to indicate an infinite lease
     * time mapping, it is REQUIRED that gateway uses the maximum value
     * instead (e.g. 604800 seconds) */
    if(leaseduration == 0 || leaseduration > 604800)
        leaseduration = 604800;
#endif

    NP_UPNP_DEBUG("%s: ext port %hu to %s:%hu protocol %s for: %s leaseduration=%u rhost=%s\n",
           action, eport, int_ip, iport, protocol, desc, leaseduration, r_host);

    r = upnp_redirect(r_host, eport, int_ip, iport, protocol, desc, leaseduration, atoi(enabled_str));
    NP_UPNP_DEBUG("upnp_redirect(...) returns %d\n", r);

    ClearNameValueList(&data);

    /* possible error codes for AddPortMapping :
     * 402 - Invalid Args
     * 501 - Action Failed
     * 715 - Wildcard not permited in SrcAddr
     * 716 - Wildcard not permited in ExtPort
     * 718 - ConflictInMappingEntry
     * 724 - SamePortValuesRequired (deprecated in IGD v2)
     * 725 - OnlyPermanentLeasesSupported
             The NAT implementation only supports permanent lease times on
             port mappings (deprecated in IGD v2)
     * 726 - RemoteHostOnlySupportsWildcard
             RemoteHost must be a wildcard and cannot be a specific IP
             address or DNS name (deprecated in IGD v2)
     * 727 - ExternalPortOnlySupportsWildcard
             ExternalPort must be a wildcard and cannot be a specific port
             value (deprecated in IGD v2)
     * 728 - NoPortMapsAvailable
             There are not enough free prots available to complete the mapping
             (added in IGD v2)
     * 729 - ConflictWithOtherMechanisms (added in IGD v2) */
    switch(r)
    {
    case 0: /* success */
        BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
        break;
    case -2:    /* already redirected */
    case -3:    /* not permitted */
        SoapError(h, 718, "ConflictInMappingEntry");
        break;
    default:
        SoapError(h, 501, "ActionFailed");
    }
}

/* AddAnyPortMapping was added in WANIPConnection v2 */
static void
AddAnyPortMapping(struct upnphttp * h, const char * action)
{
    int r;
    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<NewReservedPort>%hu</NewReservedPort>"
        "</u:%sResponse>";

    char body[512];
    int bodylen;

    struct NameValueParserData data;
    const char * int_ip, * int_port, * ext_port, * protocol, * desc;
    const char * r_host;
    char *enabled_str;
    unsigned short iport, eport;
    const char * leaseduration_str;
    unsigned int leaseduration;

    struct hostent *hp; /* getbyhostname() */
    char ** ptr; /* getbyhostname() */
    struct in_addr result_ip;/*unsigned char result_ip[16];*/ /* inet_pton() */

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    r_host = GetValueFromNameValueList(&data, "NewRemoteHost");
    ext_port = GetValueFromNameValueList(&data, "NewExternalPort");
    protocol = GetValueFromNameValueList(&data, "NewProtocol");
    int_port = GetValueFromNameValueList(&data, "NewInternalPort");
    int_ip = GetValueFromNameValueList(&data, "NewInternalClient");
    /* NewEnabled */
    desc = GetValueFromNameValueList(&data, "NewPortMappingDescription");
    leaseduration_str = GetValueFromNameValueList(&data, "NewLeaseDuration");
    enabled_str = GetValueFromNameValueList(&data, "NewEnabled");

    leaseduration = leaseduration_str ? atoi(leaseduration_str) : 0;
    if(leaseduration == 0)
        leaseduration = 604800;

    eport = (unsigned short)atoi(ext_port);
    iport = (unsigned short)atoi(int_port);

    if (!int_ip)
    {
        ClearNameValueList(&data);
        SoapError(h, 402, "Invalid Args");
        return;
    }
#ifndef SUPPORT_REMOTEHOST
#ifdef UPNP_STRICT
    if (r_host && (strlen(r_host) > 0) && (0 != strcmp(r_host, "*")))
    {
        ClearNameValueList(&data);
        SoapError(h, 726, "RemoteHostOnlySupportsWildcard");
        return;
    }
#endif
#endif

    /* if ip not valid assume hostname and convert */
    if (inet_pton(AF_INET, int_ip, &result_ip) <= 0) 
    {
        hp = gethostbyname(int_ip);
        if(hp && hp->h_addrtype == AF_INET) 
        { 
            for(ptr = hp->h_addr_list; ptr && *ptr; ptr++)
            {
                int_ip = inet_ntoa(*((struct in_addr *) *ptr));
                result_ip = *((struct in_addr *) *ptr);
                /* TODO : deal with more than one ip per hostname */
                break;
            }
        } 
        else 
        {
            NP_UPNP_ERROR("Failed to convert hostname '%s' to ip address\n", int_ip); 
            ClearNameValueList(&data);
            SoapError(h, 402, "Invalid Args");
            return;
        }               
    }

    /* check if NewInternalAddress is the client address */
    if(GETFLAG(SECUREMODEMASK))
    {
        if(h->clientaddr.s_addr != result_ip.s_addr)
        {
            NP_UPNP_DEBUG("Client %s tried to redirect port to %s\n",
                   inet_ntoa(h->clientaddr), int_ip);
            ClearNameValueList(&data);
            SoapError(h, 606, "Action not authorized");
            return;
        }
    }

    /* TODO : accept a different external port 
     * have some smart strategy to choose the port */
    for(;;) {
        r = upnp_redirect(r_host, eport, int_ip, iport, protocol, desc, leaseduration, atoi(enabled_str));
        if(r==-2 && eport < 65535) {
            eport++;
        } else {
            break;
        }
    }

    ClearNameValueList(&data);

    switch(r)
    {
    case 0: /* success */
        bodylen = snprintf(body, sizeof(body), resp,
                      action, "urn:schemas-upnp-org:service:WANIPConnection:2",
                      eport, action);
        BuildSendAndCloseSoapResp(h, body, bodylen);
        break;
    case -2:    /* already redirected */
        SoapError(h, 718, "ConflictInMappingEntry");
        break;
    case -3:    /* not permitted */
        SoapError(h, 606, "Action not authorized");
        break;
    default:
        SoapError(h, 501, "ActionFailed");
    }
}

static void
GetSpecificPortMappingEntry(struct upnphttp * h, const char * action)
{
    int r;

    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<NewInternalPort>%u</NewInternalPort>"
        "<NewInternalClient>%s</NewInternalClient>"
        "<NewEnabled>%d</NewEnabled>"
        "<NewPortMappingDescription>%s</NewPortMappingDescription>"
        "<NewLeaseDuration>%u</NewLeaseDuration>"
        "</u:%sResponse>";

    char body[1024];
    int bodylen;
    struct NameValueParserData data;
    const char * r_host, * ext_port, * protocol;
    unsigned short eport, iport;
    char int_ip[32];
    char desc[64];
    int enabled;
    unsigned int leaseduration = 0;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    r_host = GetValueFromNameValueList(&data, "NewRemoteHost");
    ext_port = GetValueFromNameValueList(&data, "NewExternalPort");
    protocol = GetValueFromNameValueList(&data, "NewProtocol");

    if(!ext_port || !protocol)
    {
        ClearNameValueList(&data);
        SoapError(h, 402, "Invalid Args");
        return;
    }
#ifndef SUPPORT_REMOTEHOST
#ifdef UPNP_STRICT
    if (r_host && (strlen(r_host) > 0) && (0 != strcmp(r_host, "*")))
    {
        ClearNameValueList(&data);
        SoapError(h, 726, "RemoteHostOnlySupportsWildcard");
        return;
    }
#endif
#endif

    eport = (unsigned short)atoi(ext_port);

    /* TODO : add r_host as an input parameter ...
     * We prevent several Port Mapping with same external port
     * but different remoteHost to be set up, so that is not
     * a priority. */
    r = upnp_get_redirection_infos(eport, protocol, &iport,
                                   int_ip, sizeof(int_ip),
                                   desc, sizeof(desc),
                                   NULL, 0,
                                   &leaseduration, &enabled);

    if(r < 0)
    {       
        SoapError(h, 714, "NoSuchEntryInArray");
    }
    else
    {
        NP_UPNP_DEBUG("%s: rhost='%s' %s %s found => %s:%u desc='%s'\n",
               action,
               r_host, ext_port, protocol, int_ip, (unsigned int)iport, desc);
        bodylen = snprintf(body, sizeof(body), resp,
                action, "urn:schemas-upnp-org:service:WANIPConnection:1",
                (unsigned int)iport, int_ip, enabled, desc, leaseduration,
                action);
        BuildSendAndCloseSoapResp(h, body, bodylen);
    }

    ClearNameValueList(&data);
}

static void
DeletePortMapping(struct upnphttp * h, const char * action)
{
    int r;

    static const char resp[] =
        "<u:DeletePortMappingResponse "
        "xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
        "</u:DeletePortMappingResponse>";

    struct NameValueParserData data;
    const char * r_host, * ext_port, * protocol;
    unsigned short eport;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    r_host = GetValueFromNameValueList(&data, "NewRemoteHost");
    ext_port = GetValueFromNameValueList(&data, "NewExternalPort");
    protocol = GetValueFromNameValueList(&data, "NewProtocol");

    if(!ext_port || !protocol)
    {
        ClearNameValueList(&data);
        SoapError(h, 402, "Invalid Args");
        return;
    }
#ifndef SUPPORT_REMOTEHOST
#ifdef UPNP_STRICT
    if (r_host && (strlen(r_host) > 0) && (0 != strcmp(r_host, "*")))
    {
        ClearNameValueList(&data);
        SoapError(h, 726, "RemoteHostOnlySupportsWildcard");
        return;
    }
#endif
#endif

    eport = (unsigned short)atoi(ext_port);

    /* TODO : if in secure mode, check the IP
     * Removing a redirection is not a security threat,
     * just an annoyance for the user using it. So this is not
     * a priority. */

    NP_UPNP_DEBUG("%s: external port: %hu, protocol: %s\n", 
        action, eport, protocol);

    r = upnp_delete_redirection(eport, protocol);

    if(r < 0)
    {   
        SoapError(h, 714, "NoSuchEntryInArray");
    }
    else
    {
        BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
    }

    ClearNameValueList(&data);
}

/* DeletePortMappingRange was added in IGD spec v2 */
static void
DeletePortMappingRange(struct upnphttp * h, const char * action)
{
    int r = -1;
    static const char resp[] =
        "<u:DeletePortMappingRangeResponse "
        "xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:2\">"
        "</u:DeletePortMappingRangeResponse>";
    struct NameValueParserData data;
    const char * protocol;
    unsigned short startport, endport;
    int manage;
    unsigned short * port_list;
    unsigned int i, number = 0;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    startport = (unsigned short)atoi(GetValueFromNameValueList(&data, "NewStartPort"));
    endport = (unsigned short)atoi(GetValueFromNameValueList(&data, "NewEndPort"));
    protocol = GetValueFromNameValueList(&data, "NewProtocol");
    manage = atoi(GetValueFromNameValueList(&data, "NewManage"));

    /* possible errors :
       606 - Action not authorized
       730 - PortMappingNotFound
       733 - InconsistentParameter
     */
    if(startport > endport)
    {
        SoapError(h, 733, "InconsistentParameter");
        ClearNameValueList(&data);
        return;
    }

    port_list = upnp_get_portmappings_in_range(startport, endport,
                                               protocol, &number);
    for(i = 0; i < number; i++)
    {
        r = upnp_delete_redirection(port_list[i], protocol);
        /* TODO : check return value for errors */
    }
    free(port_list);
    BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);

    ClearNameValueList(&data);
}

static void
GetGenericPortMappingEntry(struct upnphttp * h, const char * action)
{
    int r;
    
    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<NewRemoteHost>%s</NewRemoteHost>"
        "<NewExternalPort>%u</NewExternalPort>"
        "<NewProtocol>%s</NewProtocol>"
        "<NewInternalPort>%u</NewInternalPort>"
        "<NewInternalClient>%s</NewInternalClient>"
        "<NewEnabled>%d</NewEnabled>"
        "<NewPortMappingDescription>%s</NewPortMappingDescription>"
        "<NewLeaseDuration>%u</NewLeaseDuration>"
        "</u:%sResponse>";

    int index = 0;
    unsigned short eport, iport;
    const char * m_index;
    char protocol[4], iaddr[32];
    char desc[64];
    char rhost[40];
    int enabled;
    unsigned int leaseduration = 0;
    struct NameValueParserData data;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    m_index = GetValueFromNameValueList(&data, "NewPortMappingIndex");

    if(!m_index)
    {
        ClearNameValueList(&data);
        SoapError(h, 402, "Invalid Args");
        return;
    }   

    index = (int)atoi(m_index);

    NP_UPNP_DEBUG("%s: index=%d\n", action, index);

    rhost[0] = '\0';
    r = upnp_get_redirection_infos_by_index(index, &eport, protocol, &iport,
                                            iaddr, sizeof(iaddr),
                                            desc, sizeof(desc),
                                            rhost, sizeof(rhost),
                                            &leaseduration, &enabled);

    if(r < 0)
    {
        SoapError(h, 713, "SpecifiedArrayIndexInvalid");
    }
    else
    {
        int bodylen;
        char body[2048];
        bodylen = snprintf(body, sizeof(body), resp,
            //action, "urn:schemas-upnp-org:service:WANIPConnection:1", "NewRemoteHost",
            action, "urn:schemas-upnp-org:service:WANIPConnection:1", "",
            (unsigned int)eport, protocol, (unsigned int)iport, iaddr, enabled, desc,
            leaseduration, action);
        BuildSendAndCloseSoapResp(h, body, bodylen);
    }

    ClearNameValueList(&data);
}

/* GetListOfPortMappings was added in the IGD v2 specification */
static void
GetListOfPortMappings(struct upnphttp * h, const char * action)
{
    static const char resp_start[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<NewPortListing><![CDATA[";
    static const char resp_end[] =
        "]]></NewPortListing>"
        "</u:%sResponse>";

    static const char list_start[] =
        "<p:PortMappingList xmlns:p=\"urn:schemas-upnp-org:gw:WANIPConnection\""
        " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\""
        " xsi:schemaLocation=\"urn:schemas-upnp-org:gw:WANIPConnection"
        " http://www.upnp.org/schemas/gw/WANIPConnection-v2.xsd\">";
    static const char list_end[] =
        "</p:PortMappingList>";

    static const char entry[] =
        "<p:PortMappingEntry>"
        "<p:NewRemoteHost>%s</p:NewRemoteHost>"
        "<p:NewExternalPort>%hu</p:NewExternalPort>"
        "<p:NewProtocol>%s</p:NewProtocol>"
        "<p:NewInternalPort>%hu</p:NewInternalPort>"
        "<p:NewInternalClient>%s</p:NewInternalClient>"
        "<p:NewEnabled>%d</p:NewEnabled>"
        "<p:NewDescription>%s</p:NewDescription>"
        "<p:NewLeaseTime>%u</p:NewLeaseTime>"
        "</p:PortMappingEntry>";

    char * body;
    size_t bodyalloc;
    int bodylen;

    int r = -1;
    unsigned short iport;
    char int_ip[32];
    char desc[64];
    char rhost[64];
    int enabled;
    unsigned int leaseduration = 0;

    struct NameValueParserData data;
    unsigned short startport, endport;
    const char * protocol;
    int manage;
    int number;
    unsigned short * port_list;
    unsigned int i, list_size = 0;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    startport = (unsigned short)atoi(GetValueFromNameValueList(&data, "NewStartPort"));
    endport = (unsigned short)atoi(GetValueFromNameValueList(&data, "NewEndPort"));
    protocol = GetValueFromNameValueList(&data, "NewProtocol");
    manage = atoi(GetValueFromNameValueList(&data, "NewManage"));
    number = atoi(GetValueFromNameValueList(&data, "NewNumberOfPorts"));
    if(number == 0) number = 1000;  /* return up to 1000 mappings by default */

    if(startport > endport)
    {
        SoapError(h, 733, "InconsistentParameter");
        ClearNameValueList(&data);
        return;
    }
/*
build the PortMappingList xml document :

<p:PortMappingList xmlns:p="urn:schemas-upnp-org:gw:WANIPConnection"
xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance"
xsi:schemaLocation="urn:schemas-upnp-org:gw:WANIPConnection
http://www.upnp.org/schemas/gw/WANIPConnection-v2.xsd">
<p:PortMappingEntry>
<p:NewRemoteHost>202.233.2.1</p:NewRemoteHost>
<p:NewExternalPort>2345</p:NewExternalPort>
<p:NewProtocol>TCP</p:NewProtocol>
<p:NewInternalPort>2345</p:NewInternalPort>
<p:NewInternalClient>192.168.1.137</p:NewInternalClient>
<p:NewEnabled>1</p:NewEnabled>
<p:NewDescription>dooom</p:NewDescription>
<p:NewLeaseTime>345</p:NewLeaseTime>
</p:PortMappingEntry>
</p:PortMappingList>
*/
    bodyalloc = 4096;
    body = malloc(bodyalloc);
    if(!body)
    {
        ClearNameValueList(&data);
        SoapError(h, 501, "ActionFailed");
        return;
    }
    bodylen = snprintf(body, bodyalloc, resp_start,
                  action, "urn:schemas-upnp-org:service:WANIPConnection:2");
    memcpy(body+bodylen, list_start, sizeof(list_start));
    bodylen += (sizeof(list_start) - 1);

    port_list = upnp_get_portmappings_in_range(startport, endport,
                                               protocol, &list_size);
    /* loop through port mappings */
    for(i = 0; number > 0 && i < list_size; i++)
    {
        /* have a margin of 1024 bytes to store the new entry */
        if(bodylen + 1024 > (int)bodyalloc)
        {
            bodyalloc += 4096;
            body = realloc(body, bodyalloc);
            if(!body)
            {
                ClearNameValueList(&data);
                SoapError(h, 501, "ActionFailed");
                free(port_list);
                return;
            }
        }
        rhost[0] = '\0';
        r = upnp_get_redirection_infos(port_list[i], protocol, &iport,
                                       int_ip, sizeof(int_ip),
                                       desc, sizeof(desc),
                                       rhost, sizeof(rhost),
                                       &leaseduration, &enabled);
        if(r == 0)
        {
            bodylen += snprintf(body+bodylen, bodyalloc-bodylen, entry,
                                rhost, port_list[i], protocol,
                                iport, int_ip, enabled, desc, leaseduration);
            number--;
        }
    }
    free(port_list);
    port_list = NULL;

    memcpy(body+bodylen, list_end, sizeof(list_end));
    bodylen += (sizeof(list_end) - 1);
    bodylen += snprintf(body+bodylen, bodyalloc-bodylen, resp_end,
                        action);
    BuildSendAndCloseSoapResp(h, body, bodylen);
    free(body);

    ClearNameValueList(&data);
}

#ifdef ENABLE_L3F_SERVICE
static void
SetDefaultConnectionService(struct upnphttp * h, const char * action)
{
    static const char resp[] =
        "<u:SetDefaultConnectionServiceResponse "
        "xmlns:u=\"urn:schemas-upnp-org:service:Layer3Forwarding:1\">"
        "</u:SetDefaultConnectionServiceResponse>";
    struct NameValueParserData data;
    char * p;
    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    p = GetValueFromNameValueList(&data, "NewDefaultConnectionService");
    if(p)
    {
        NP_UPNP_DEBUG("%s(%s) : Ignored\n", action, p);
    }
    ClearNameValueList(&data);
    /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
    if(NULL != p)
    {
        BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
    }
    else
    {
        SoapError(h, 402, "Invalid Args");
    }
    /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
}

static void
GetDefaultConnectionService(struct upnphttp * h, const char * action)
{
    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"urn:schemas-upnp-org:service:Layer3Forwarding:1\">"
        "<NewDefaultConnectionService>%s:WANConnectionDevice:1,"
        "urn:upnp-org:serviceId:WANIPConn1</NewDefaultConnectionService>"
        "</u:%sResponse>";
    /* example from UPnP_IGD_Layer3Forwarding 1.0.pdf :
     * uuid:44f5824f-c57d-418c-a131-f22b34e14111:WANConnectionDevice:1,
     * urn:upnp-org:serviceId:WANPPPConn1 */
    char body[1024];
    int bodylen;

    bodylen = snprintf(body, sizeof(body), resp,
                       action, uuidvalue, action);
    BuildSendAndCloseSoapResp(h, body, bodylen);
}
#endif

/* Added for compliance with WANIPConnection v2 */
static void
SetConnectionType(struct upnphttp * h, const char * action)
{
    /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 modified */
    const char * connection_type = NULL;
    /* END   2082304944 zhoujianchun 00203875 2012.8.29 modified */
    struct NameValueParserData data;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    connection_type = GetValueFromNameValueList(&data, "NewConnectionType");
    /* Unconfigured, IP_Routed, IP_Bridged */
    ClearNameValueList(&data);
    /* always return a ReadOnly error */
    /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
    if(NULL != connection_type)
    {
        SoapError(h, 731, "ReadOnly");
    }
    else
    {
        SoapError(h, 402, "Invalid Args");
    }
    /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
}

#ifdef MBB_FEATURE_UPNP_CERTIFICATION
/* Added for compliance with WANIPConnection v2 */
/*****************************************************************************
 Function   : RequestConnection
 Description: UPNP request connection function
 Input      : struct upnphttp * h,const char * action
 Return     : void
 Other      :
 *****************************************************************************/
static void
RequestConnection(struct upnphttp * h, const char * action)
{
    char body[512] = {0};
    int bodylen = 0;
    int dial_rst = 0;
    int conn_stat = UPNP_UNCONFIGURED;

    NP_UPNP_DEBUG("RequestConnection()\n");

    conn_stat = get_wan_connection_status(ext_if_name);
    /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
    bodylen = snprintf(body, sizeof(body),
            "<u:%sResponse xmlns:u=\"%s\">\n%s\n</u:%sResponse>", action,
            "urn:schemas-upnp-org:service:WANIPConnection:1", "", action);
    BuildSendAndCloseSoapResp(h, body, bodylen);
    /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
    switch(conn_stat)
    {
        case UPNP_CONNECTED:
            break;

        case UPNP_DISCONNECTED:
        case UPNP_PENDINGDISCONNECT:
        case UPNP_DISCONNECTING:

#ifndef USE_MOCKER
            dial_rst = UPnPDialUp();
            NP_UPNP_DEBUG("UPnP: UPnPDialUp() returns %d\n", dial_rst);
#endif
            if(dial_rst >= 0)
            {
                bodylen = snprintf(body, sizeof(body), 
                "<u:%sResponse xmlns:u=\"%s\">\n%s\n</u:%sResponse>", action, 
                "urn:schemas-upnp-org:service:WANIPConnection:1", "", action);
                BuildSendAndCloseSoapResp(h, body, bodylen);
            }
            else
            {
                NP_UPNP_ERROR("Send Disconect call message to CMS FAILL\n");
                SoapError(h, 704, "There was a failure in setting up the IP or PPP connection with the service provider");
            }
            break;
        case UPNP_CONNECTING:
            SoapError(h, 705, "The connection is already in the process of being setup");
            //eventing
            break;
        case UPNP_UNCONFIGURED:
            SoapError(h, 706, "Current ConnectionStatus is Unconfigured");
            //eventing
            break;
        default:
            NP_UPNP_ERROR("call stat not match!!\n");
            SoapError(h,704,"call fail");
            break;                  
    }
}

/* Added for compliance with WANIPConnection v2 */
/*****************************************************************************
 Function   : ForceTermination
 Description: UPNP Terminate connection function
 Input      : struct upnphttp * h,const char * action
 Return     : void
 Other      :
 *****************************************************************************/
static void
ForceTermination(struct upnphttp * h, const char * action)
{
    char body[512] = {0};
    int bodylen = 0;
    int dial_rst = 0;
    int conn_stat = UPNP_UNCONFIGURED;

    NP_UPNP_WARN("ForceTermination()\n");
    
    conn_stat = get_wan_connection_status(ext_if_name);

    switch(conn_stat)
    {
        case UPNP_CONNECTED:
        case UPNP_CONNECTING:
#ifndef USE_MOCKER
            dial_rst = UPnPDialDown();
            NP_UPNP_DEBUG("UPnP: UPnPDialDown() returns %d\n", dial_rst);
#endif
            if(dial_rst >= 0)
            {
                bodylen = snprintf(body, sizeof(body), 
                "<u:%sResponse xmlns:u=\"%s\">\n%s\n</u:%sResponse>", action, 
                "urn:schemas-upnp-org:service:WANIPConnection:1", "", action);
                BuildSendAndCloseSoapResp(h, body, bodylen);    
            }
            else
            {
                NP_UPNP_ERROR("Send Disconect call message to CMS FAILL\n");
                SoapError(h, 606, "Terminate CALL FAIL!!");
            }
            break;
        case UPNP_DISCONNECTED:
        case UPNP_PENDINGDISCONNECT:
            SoapError(h,711,"ConnectionAlreadyTerminated");
            break;
        case UPNP_DISCONNECTING:
            SoapError(h,707,"DisconnectInProgress!");
            break;
        case UPNP_UNCONFIGURED:
            SoapError(h, 706, "Current ConnectionStatus is Unconfigured");
            break;
        default:
            NP_UPNP_ERROR("call stat not match!!\n");
            SoapError(h,704,"call fail");
            break;          
            
    }
}
#endif

/*
If a control point calls QueryStateVariable on a state variable that is not
buffered in memory within (or otherwise available from) the service,
the service must return a SOAP fault with an errorCode of 404 Invalid Var.

QueryStateVariable remains useful as a limited test tool but may not be
part of some future versions of UPnP.
*/
static void
QueryStateVariable(struct upnphttp * h, const char * action)
{
    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<return>%s</return>"
        "</u:%sResponse>";

    char body[512];
    int bodylen;
    struct NameValueParserData data;
    const char * var_name;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    /*var_name = GetValueFromNameValueList(&data, "QueryStateVariable"); */
    /*var_name = GetValueFromNameValueListIgnoreNS(&data, "varName");*/
    var_name = GetValueFromNameValueList(&data, "varName");


    if(!var_name)
    {
        SoapError(h, 402, "Invalid Args");
    }
    else if(strcmp(var_name, "ConnectionStatus") == 0)
    {   
        const char * status = "Connected";
        char ext_ip_addr[16];
        if(getifaddr(ext_if_name, ext_ip_addr, 16) < 0) {
            status = "Disconnected";
        }
        //status = get_wan_connection_status_str(ext_if_name);
        bodylen = snprintf(body, sizeof(body), resp,
                           action, "urn:schemas-upnp-org:control-1-0",
                           status, action);
        BuildSendAndCloseSoapResp(h, body, bodylen);
    }
#if 0
    /* not usefull */
    else if(strcmp(var_name, "ConnectionType") == 0)
    {   
        bodylen = snprintf(body, sizeof(body), resp, "IP_Routed");
        BuildSendAndCloseSoapResp(h, body, bodylen);
    }
    else if(strcmp(var_name, "LastConnectionError") == 0)
    {   
        bodylen = snprintf(body, sizeof(body), resp, "ERROR_NONE");
        BuildSendAndCloseSoapResp(h, body, bodylen);
    }
#endif
    else if(strcmp(var_name, "PortMappingNumberOfEntries") == 0)
    {
        char strn[10];
        snprintf(strn, sizeof(strn), "%i",
                 upnp_get_portmapping_number_of_entries());
        bodylen = snprintf(body, sizeof(body), resp,
                           action, "urn:schemas-upnp-org:control-1-0",
                           strn, action);
        BuildSendAndCloseSoapResp(h, body, bodylen);
    }
    else
    {
        NP_UPNP_DEBUG("%s: Unknown: %s\n", action, var_name?var_name:"");
        SoapError(h, 404, "Invalid Var");
    }

    ClearNameValueList(&data);  
}

#ifdef ENABLE_6FC_SERVICE
#ifndef ENABLE_IPV6
#error "ENABLE_6FC_SERVICE needs ENABLE_IPV6"
#endif
/* WANIPv6FirewallControl actions */
static void
GetFirewallStatus(struct upnphttp * h, const char * action)
{
    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<FirewallEnabled>%d</FirewallEnabled>"
        "<InboundPinholeAllowed>%d</InboundPinholeAllowed>"
        "</u:%sResponse>";

    char body[512];
    int bodylen;

    bodylen = snprintf(body, sizeof(body), resp,
        action, "urn:schemas-upnp-org:service:WANIPv6FirewallControl:1",
        ipv6fc_firewall_enabled, ipv6fc_inbound_pinhole_allowed, action);
    BuildSendAndCloseSoapResp(h, body, bodylen);
}

static int
CheckStatus(struct upnphttp * h)
{
    if (!ipv6fc_firewall_enabled)
    {
        SoapError(h, 702, "FirewallDisabed");
        return 0;
    }
    else if(!ipv6fc_inbound_pinhole_allowed)
    {
        SoapError(h, 703, "InboundPinholeNotAllowed");
        return 0;
    }
    else
        return 1;
}

static int
DataVerification(struct upnphttp * h, char * int_ip, unsigned short * int_port, const char * protocol, char * leaseTime)
{
    //int n;
    // **  Internal IP can't be wildcarded
    if (!int_ip)
    {
        SoapError(h, 708, "WildCardNotPermittedInSrcIP");
        return 0;
    }
    
    if (!strchr(int_ip, ':'))
    {
        SoapError(h, 402, "Invalid Args");
        return 0;
    }

    // ** Internal port can't be wilcarded. 
//  printf("\tint_port: *%d*\n", *int_port);
    if (*int_port == 0)
    {
        SoapError(h, 706, "InternalPortWilcardingNotAllowed");
        return 0;
    }

    // ** Protocol can't be wilcarded and can't be an unknown port (here deal with only UDP, TCP, UDPLITE)
//  printf("\tprotocol: *%s*\n", protocol);
    if (atoi(protocol) == 65535)
    {
        SoapError(h, 707, "ProtocolWilcardingNotAllowed");
        return 0;
    }
    else if (atoi(protocol) != IPPROTO_UDP
            && atoi(protocol) != IPPROTO_TCP
#ifdef IPPROTO_UDPITE
            && atoi(protocol) != IPPROTO_UDPLITE
#endif
            )
    {
        SoapError(h, 705, "ProtocolNotSupported");
        return 0;
    }

    // ** Lease Time can't be wilcarded nor >86400.
//  printf("\tlease time: %s\n", leaseTime);
    if(!leaseTime || !atoi(leaseTime) || atoi(leaseTime)>86400)
    {
        /* lease duration is never infinite, nor wilcarded. In this case, use default value */
        NP_UPNP_WARN("LeaseTime=%s not supported, (ip=%s)\n", leaseTime, int_ip);
        SoapError(h, 402, "Invalid Args");
        return 0;
    }

    return 1;
}

#if 0
static int connecthostport(const char * host, unsigned short port, char * result)
{
    int s, n;
    char hostname[INET6_ADDRSTRLEN];
    char port_str[8], ifname[8], tmp[4];
    struct addrinfo *ai, *p;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    /* hints.ai_flags = AI_ADDRCONFIG; */
#ifdef AI_NUMERICSERV
    hints.ai_flags = AI_NUMERICSERV;
#endif
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC; /* AF_INET, AF_INET6 or AF_UNSPEC */
    /* hints.ai_protocol = IPPROTO_TCP; */
    snprintf(port_str, sizeof(port_str), "%hu", port);
    strcpy(hostname, host);
    if(!strncmp(host, "fe80", 4))
    {
        printf("Using an linklocal address\n");
        strcpy(ifname, "%");
        snprintf(tmp, sizeof(tmp), "%d", linklocal_index);
        strcat(ifname, tmp);
        strcat(hostname, ifname);
        printf("host: %s\n", hostname);
    }
    n = getaddrinfo(hostname, port_str, &hints, &ai);
    if(n != 0)
    {
        fprintf(stderr, "getaddrinfo() error : %s\n", gai_strerror(n));
        return -1;
    }
    s = -1;
    for(p = ai; p; p = p->ai_next)
    {
#ifdef DEBUG
        char tmp_host[256];
        char tmp_service[256];
        printf("ai_family=%d ai_socktype=%d ai_protocol=%d ai_addrlen=%d\n ",
               p->ai_family, p->ai_socktype, p->ai_protocol, p->ai_addrlen);
        getnameinfo(p->ai_addr, p->ai_addrlen, tmp_host, sizeof(tmp_host),
                    tmp_service, sizeof(tmp_service),
                    NI_NUMERICHOST | NI_NUMERICSERV);
        printf(" host=%s service=%s\n", tmp_host, tmp_service);
#endif
        inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)p->ai_addr)->sin6_addr), result, INET6_ADDRSTRLEN);
        return 0;
    }
    freeaddrinfo(ai);
}
#endif

/* Check the security policy right */
static int
PinholeVerification(struct upnphttp * h, char * int_ip, unsigned short * int_port)
{
    int n;
    /* Pinhole InternalClient address must correspond to the action sender */
    NP_UPNP_DEBUG("Checking internal IP@ and port (Security policy purpose)\n");
    char senderAddr[INET6_ADDRSTRLEN]="";
    //char str[INET6_ADDRSTRLEN]="";
    //connecthostport(int_ip, *int_port, str);
    //printf("int_ip: %s / str: %s\n", int_ip, str);
    
    struct addrinfo hints, *ai, *p;
    struct in6_addr result_ip;/*unsigned char result_ip[16];*/ /* inet_pton() */ //IPv6 Modification

    hints.ai_socktype = SOCK_STREAM;
    hints.ai_family = AF_UNSPEC;

    /* if ip not valid assume hostname and convert */
    if (inet_pton(AF_INET6, int_ip, &result_ip) <= 0) //IPv6 Modification
    {

        n = getaddrinfo(int_ip, NULL, &hints, &ai);//hp = gethostbyname(int_ip);
        if(!n && ai->ai_family == AF_INET6) //IPv6 Modification
        {
            for(p = ai; p; p = p->ai_next)//ptr = hp->h_addr_list; ptr && *ptr; ptr++)
            {
                inet_ntop(AF_INET6, (struct in6_addr *) p, int_ip, sizeof(struct in6_addr)); ///IPv6 Modification
                result_ip = *((struct in6_addr *) p);
                fprintf(stderr, "upnpsoap / AddPinhole: assuming int addr = %s", int_ip);
                /* TODO : deal with more than one ip per hostname */
                break;
            }
        }
        else
        {
            NP_UPNP_ERROR("Failed to convert hostname '%s' to ip address\n", int_ip);
            SoapError(h, 402, "Invalid Args");
            return -1;
        }
        freeaddrinfo(p);
    }

    if(inet_ntop(AF_INET6, &(h->clientaddr_v6), senderAddr, INET6_ADDRSTRLEN)<=0)
    {
        //printf("Failed to inet_ntop\n");
        NP_UPNP_ERROR("inet_ntop: %s\n", strerror(errno));
    }
    NP_UPNP_DEBUG("\tPinholeVerification:\n\t\tCompare sender @: %s\n\t\t  to intClient @: %s\n", senderAddr, int_ip);
    if(strcmp(senderAddr, int_ip) != 0)
    if(h->clientaddr_v6.s6_addr != result_ip.s6_addr)
    {
        NP_UPNP_DEBUG("Client %s tried to access pinhole for internal %s and is not authorized to do it\n",
               senderAddr, int_ip);
        SoapError(h, 606, "Action not authorized");
        return 0;
    }

    /* Pinhole InternalPort must be greater than or equal to 1024 */
    if (*int_port < 1024)
    {
        NP_UPNP_DEBUG("Client %s tried to access pinhole with port < 1024 and is not authorized to do it\n",
               senderAddr);
        SoapError(h, 606, "Action not authorized");
        return 0;
    }
    return 1;
}

static void
AddPinhole(struct upnphttp * h, const char * action)
{
    int r;
    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<UniqueID>%d</UniqueID>"
        "</u:%sResponse>";
    char body[512];
    int bodylen;
    struct NameValueParserData data;
    char * rem_host, * rem_port, * int_ip, * int_port, * protocol, * leaseTime;
    int uid = 0;
    unsigned short iport, rport;

    if(CheckStatus(h)==0)
        return;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    rem_host = GetValueFromNameValueList(&data, "RemoteHost");
    rem_port = GetValueFromNameValueList(&data, "RemotePort");
    int_ip = GetValueFromNameValueList(&data, "InternalClient");
    int_port = GetValueFromNameValueList(&data, "InternalPort");
    protocol = GetValueFromNameValueList(&data, "Protocol");
    leaseTime = GetValueFromNameValueList(&data, "LeaseTime");

    rport = (unsigned short)atoi(rem_port);
    iport = (unsigned short)atoi(int_port);

    // **  As there is no security policy, InternalClient must be equal to the CP's IP address.
    if(DataVerification(h, int_ip, &iport, protocol, leaseTime) == 0
       || PinholeVerification(h, int_ip, &iport) <= 0)
    {
        ClearNameValueList(&data);
        return ;
    }

    // ** RemoteHost can be wilcarded or an IDN.
    /*printf("\trem_host: %s\n", rem_host);*/
    if (rem_host!=NULL && !strchr(rem_host, ':'))
    {
        ClearNameValueList(&data);
        SoapError(h, 402, "Invalid Args");
        return;
    }
    /*printf("\tAddr check passed.\n");*/

    NP_UPNP_DEBUG("%s: (inbound) from [%s]:%hu to [%s]:%hu with protocol %s during %ssec\n", 
            action, rem_host?rem_host:"anywhere", rport, int_ip, iport, protocol, leaseTime);

    r = upnp_add_inboundpinhole(rem_host, rport, int_ip, iport, protocol, leaseTime, &uid);

    switch(r)
    {
        case 1:         /* success */
            bodylen = snprintf(body, sizeof(body), resp, action, "urn:schemas-upnp-org:service:WANIPv6FirewallControl:1", uid, action);
            BuildSendAndCloseSoapResp(h, body, bodylen);
            break;
        case -1:    /* not permitted */
            SoapError(h, 701, "PinholeSpaceExhausted");
            break;
        default:
            SoapError(h, 501, "ActionFailed");
            break;
    }
    ClearNameValueList(&data);
}

static void
UpdatePinhole(struct upnphttp * h, const char * action)
{
    int r, n;
    static const char resp[] =
        "<u:UpdatePinholeResponse "
        "xmlns:u=\"urn:schemas-upnp-org:service:WANIPv6FirewallControl:1\">"
        "</u:UpdatePinholeResponse>";
    struct NameValueParserData data;
    const char * uid, * leaseTime;
    char iaddr[40], proto[6], lt[12];
    unsigned short iport;

    if(CheckStatus(h)==0)
        return;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    uid = GetValueFromNameValueList(&data, "UniqueID");
    leaseTime = GetValueFromNameValueList(&data, "NewLeaseTime");

    if(!uid || !leaseTime || !atoi(leaseTime) || atoi(leaseTime) > 86400)
    {
        ClearNameValueList(&data);
        SoapError(h, 402, "Invalid Args");
        return;
    }

    // Check that client is not deleting an pinhole he doesn't have access to, because of its public access
    n = upnp_get_pinhole_info(0, 0, iaddr, &iport, proto, uid, lt);
    if (n > 0)
    {
        if(PinholeVerification(h, iaddr, &iport)==0)
        {
            ClearNameValueList(&data);
            return ;
        }
    }

    NP_UPNP_DEBUG("%s: (inbound) updating lease duration to %s for pinhole with ID: %s\n", action, leaseTime, uid);

    r = upnp_update_inboundpinhole(uid, leaseTime);

    if(r < 0)
    {
        if(r == -4 || r == -1)
            SoapError(h, 704, "NoSuchEntry");
        else
            SoapError(h, 501, "ActionFailed");
    }
    else
    {
        BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
    }
    ClearNameValueList(&data);
}

static void
GetOutboundPinholeTimeout(struct upnphttp * h, const char * action)
{
    if (!ipv6fc_firewall_enabled)
    {
        SoapError(h, 702, "FirewallDisabed");
        return;
    }
    int r;

    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<OutboundPinholeTimeout>%d</OutboundPinholeTimeout>"
        "</u:%sResponse>";

    char body[512];
    int bodylen;
    struct NameValueParserData data;
    char * int_ip, * int_port, * rem_host, * rem_port, * protocol;
    int opt=0, proto=0;
    unsigned short iport, rport;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    int_ip = GetValueFromNameValueList(&data, "InternalClient");
    int_port = GetValueFromNameValueList(&data, "InternalPort");
    rem_host = GetValueFromNameValueList(&data, "RemoteHost");
    rem_port = GetValueFromNameValueList(&data, "RemotePort");
    protocol = GetValueFromNameValueList(&data, "Protocol");

    rport = (unsigned short)atoi(rem_port);
    iport = (unsigned short)atoi(int_port);
    proto = atoi(protocol);

    NP_UPNP_DEBUG("%s: retrieving timeout for outbound pinhole from [%s]:%hu to [%s]:%hu protocol %s\n", 
            action, int_ip, iport,rem_host, rport, protocol);

    r = upnp_check_outbound_pinhole(proto, &opt);

    switch(r)
    {
        case 1: /* success */
            bodylen = snprintf(body, sizeof(body), resp, action, "urn:schemas-upnp-org:service:WANIPv6FirewallControl:1", opt, action);
            BuildSendAndCloseSoapResp(h, body, bodylen);
            break;
        case -5:    /* Protocol not supported */
            SoapError(h, 705, "ProtocolNotSupported");
            break;
        default:
            SoapError(h, 501, "ActionFailed");
    }
    ClearNameValueList(&data);
}

static void
DeletePinhole(struct upnphttp * h, const char * action)
{
    if(CheckStatus(h)==0)
        return;
    int r, n;

    static const char resp[] =
        "<u:DeletePinholeResponse "
        "xmlns:u=\"urn:schemas-upnp-org:service:WANIPv6FirewallControl:1\">"
        "</u:DeletePinholeResponse>";

    struct NameValueParserData data;
    const char * uid;
    char iaddr[40], proto[6], lt[12];
    unsigned short iport;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    uid = GetValueFromNameValueList(&data, "UniqueID");

    if(!uid)
    {
        ClearNameValueList(&data);
        SoapError(h, 402, "Invalid Args");
        return;
    }

    // Check that client is not deleting an pinhole he doesn't have access to, because of its public access
    n = upnp_get_pinhole_info(0, 0, iaddr, &iport, proto, uid, lt);
    if (n > 0)
    {
        if(PinholeVerification(h, iaddr, &iport)==0)
        {
            ClearNameValueList(&data);
            return ;
        }
    }

    NP_UPNP_DEBUG("%s: (inbound) delete pinhole with ID: %s\n", action, uid);

    r = upnp_delete_inboundpinhole(uid);

    if(r <= 0)
    {
        NP_UPNP_DEBUG("%s: (inbound) failed to remove pinhole with ID: %s\n", action, uid);
        if(r==-4)
            SoapError(h, 704, "NoSuchEntry");
        else
            SoapError(h, 501, "ActionFailed");
    }
    else
    {
        NP_UPNP_DEBUG("%s: (inbound) pinhole successfully removed\n", action);
        BuildSendAndCloseSoapResp(h, resp, sizeof(resp)-1);
    }
    ClearNameValueList(&data);
}

static void
CheckPinholeWorking(struct upnphttp * h, const char * action)
{
    if(CheckStatus(h)==0)
        return;
    int r, d;

    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<IsWorking>%d</IsWorking>"
        "</u:%sResponse>";

    char body[512];
    int bodylen;
    struct NameValueParserData data;
    const char * uid;
    char eaddr[40], iaddr[40], proto[6], lt[12];
    unsigned short eport, iport;
    int isWorking = 0;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    uid = GetValueFromNameValueList(&data, "UniqueID");

    if(!uid)
    {
        ClearNameValueList(&data);
        SoapError(h, 402, "Invalid Args");
        return;
    }

    // Check that client is not checking a pinhole he doesn't have access to, because of its public access
    r = upnp_get_pinhole_info(eaddr, eport, iaddr, &iport, proto, uid, lt);
    if (r > 0)
    {
        if(PinholeVerification(h, iaddr, &iport)==0)
        {
            ClearNameValueList(&data);
            return ;
        }
        else
        {
            int rulenum_used, rulenum = 0;
            d = upnp_check_pinhole_working(uid, eaddr, iaddr, &eport, &iport, proto, &rulenum_used);
            if(d < 0)
            {
                if(d == -4)
                {
                    NP_UPNP_DEBUG("%s: rule for ID=%s, no trace found for this pinhole\n", action, uid);
                    SoapError(h, 709, "NoPacketSent");
                    ClearNameValueList(&data);
                    return ;
                }
                else
                {
                // d==-5 not same table // d==-6 not same chain // d==-7 not found a rule but policy traced
                    isWorking=0;
                    NP_UPNP_DEBUG("%s: rule for ID=%s is not working, packet going through %s\n", 
                            action, uid, (d==-5)?"the wrong table":((d==-6)?"the wrong chain":"a chain policy"));
                    bodylen = snprintf(body, sizeof(body), resp,
                        action, "urn:schemas-upnp-org:service:WANIPv6FirewallControl:1",
                        isWorking, action);
                    BuildSendAndCloseSoapResp(h, body, bodylen);
                }
            }
            else
            {
                /*check_rule_from_file(uid, &rulenum);*/
                if(rulenum_used == rulenum)
                {
                    isWorking=1;
                    NP_UPNP_DEBUG("%s: rule for ID=%s is working properly\n", action, uid);
                }
                else
                {
                    isWorking=0;
                    NP_UPNP_DEBUG("%s: rule for ID=%s is not working\n", action, uid);
                }
                bodylen = snprintf(body, sizeof(body), resp,
                        action, "urn:schemas-upnp-org:service:WANIPv6FirewallControl:1",
                        isWorking, action);
                BuildSendAndCloseSoapResp(h, body, bodylen);
            }
        }
    }
    else if(r == -4 || r == -1)
    {
        SoapError(h, 704, "NoSuchEntry");
    }
    else
    {
        SoapError(h, 501, "ActionFailed");
        ClearNameValueList(&data);
        return ;
    }
    ClearNameValueList(&data);
}

static void
GetPinholePackets(struct upnphttp * h, const char * action)
{
    if(CheckStatus(h)==0)
        return;
    int r, n;

    static const char resp[] =
        "<u:%sResponse "
        "xmlns:u=\"%s\">"
        "<PinholePackets>%d</PinholePackets>"
        "</u:%sResponse>";

    char body[512];
    int bodylen;
    struct NameValueParserData data;
    const char * uid;
    char iaddr[40], proto[6], lt[12];
    unsigned short iport;
    int pinholePackets = 0;

    ParseNameValue(h->req_buf + h->req_contentoff, h->req_contentlen, &data);
    uid = GetValueFromNameValueList(&data, "UniqueID");

    if(!uid)
    {
        ClearNameValueList(&data);
        SoapError(h, 402, "Invalid Args");
        return;
    }

    // Check that client is not getting infos of a pinhole he doesn't have access to, because of its public access
    r = upnp_get_pinhole_info(0, 0, iaddr, &iport, proto, uid, lt);
    if (r > 0)
    {
        if(PinholeVerification(h, iaddr, &iport)==0)
        {
            ClearNameValueList(&data);
            return ;
        }
    }

    n = upnp_get_pinhole_packets(uid, &pinholePackets);
    if(n > 0)
    {
        bodylen = snprintf(body, sizeof(body), resp,
                action, "urn:schemas-upnp-org:service:WANIPv6FirewallControl:1",
                pinholePackets, action);
        BuildSendAndCloseSoapResp(h, body, bodylen);
    }
    else if(r == -4 || r == -1)
    {
        SoapError(h, 704, "NoSuchEntry");
    }
    else
    {
        SoapError(h, 501, "ActionFailed");
        ClearNameValueList(&data);
        return ;
    }
    ClearNameValueList(&data);
}
#endif


/* Windows XP as client send the following requests :
 * GetConnectionTypeInfo
 * GetNATRSIPStatus
 * ? GetTotalBytesSent - WANCommonInterfaceConfig
 * ? GetTotalBytesReceived - idem
 * ? GetTotalPacketsSent - idem
 * ? GetTotalPacketsReceived - idem
 * GetCommonLinkProperties - idem
 * GetStatusInfo - WANIPConnection
 * GetExternalIPAddress
 * QueryStateVariable / ConnectionStatus!
 */
static const struct 
{
    const char * methodName; 
    void (*methodImpl)(struct upnphttp *, const char *);
}
soapMethods[] =
{
    /* WANCommonInterfaceConfig */
    { "QueryStateVariable", QueryStateVariable},
    { "GetTotalBytesSent", GetTotalBytesSent},
    { "GetTotalBytesReceived", GetTotalBytesReceived},
    { "GetTotalPacketsSent", GetTotalPacketsSent},
    { "GetTotalPacketsReceived", GetTotalPacketsReceived},
    { "GetCommonLinkProperties", GetCommonLinkProperties},
    { "GetStatusInfo", GetStatusInfo},
    /* WANIPConnection */
    { "GetConnectionTypeInfo", GetConnectionTypeInfo },
    { "GetNATRSIPStatus", GetNATRSIPStatus},
    { "GetExternalIPAddress", GetExternalIPAddress},
    { "AddPortMapping", AddPortMapping},
    { "DeletePortMapping", DeletePortMapping},
    { "GetGenericPortMappingEntry", GetGenericPortMappingEntry},
    { "GetSpecificPortMappingEntry", GetSpecificPortMappingEntry},
/* Required in WANIPConnection:2 */
    { "SetConnectionType", SetConnectionType},
#ifdef MBB_FEATURE_UPNP_CERTIFICATION
    { "RequestConnection", RequestConnection},
    { "ForceTermination", ForceTermination},
#endif
    { "AddAnyPortMapping", AddAnyPortMapping},
    { "DeletePortMappingRange", DeletePortMappingRange},
    { "GetListOfPortMappings", GetListOfPortMappings},
#ifdef ENABLE_L3F_SERVICE
    /* Layer3Forwarding */
    { "SetDefaultConnectionService", SetDefaultConnectionService},
    { "GetDefaultConnectionService", GetDefaultConnectionService},
#endif
#ifdef ENABLE_6FC_SERVICE
    /* WANIPv6FirewallControl */
    { "GetFirewallStatus", GetFirewallStatus},
    { "AddPinhole", AddPinhole},
    { "UpdatePinhole", UpdatePinhole},
    { "GetOutboundPinholeTimeout", GetOutboundPinholeTimeout},
    { "DeletePinhole", DeletePinhole},
    { "CheckPinholeWorking", CheckPinholeWorking},
    { "GetPinholePackets", GetPinholePackets},
#endif
    { 0, 0 }
};

void
ExecuteSoapAction(struct upnphttp * h, const char * action, int n)
{
    char * p;
    char * p2;
    int i, len, methodlen;

    i = 0;
    p = strchr(action, '#');

    if(p)
    {
        p++;
        p2 = strchr(p, '"');
        if(p2)
            methodlen = p2 - p;
        else
            methodlen = n - (p - action);
        while(soapMethods[i].methodName)
        {
            len = strlen(soapMethods[i].methodName);
            if(strncmp(p, soapMethods[i].methodName, len) == 0)
            {
                soapMethods[i].methodImpl(h, soapMethods[i].methodName);
                return;
            }
            i++;
        }

        NP_UPNP_WARN("SoapMethod: Unknown: %.*s\n", methodlen, p);
    }

    SoapError(h, 401, "Invalid Action");
}

/* Standard Errors:
 *
 * errorCode errorDescription Description
 * -------- ---------------- -----------
 * 401      Invalid Action  No action by that name at this service.
 * 402      Invalid Args    Could be any of the following: not enough in args,
 *                          too many in args, no in arg by that name, 
 *                          one or more in args are of the wrong data type.
 * 403      Out of Sync     Out of synchronization.
 * 501      Action Failed   May be returned in current state of service
 *                          prevents invoking that action.
 * 600-699  TBD             Common action errors. Defined by UPnP Forum
 *                          Technical Committee.
 * 700-799  TBD             Action-specific errors for standard actions.
 *                          Defined by UPnP Forum working committee.
 * 800-899  TBD             Action-specific errors for non-standard actions. 
 *                          Defined by UPnP vendor.
*/
void
SoapError(struct upnphttp * h, int errCode, const char * errDesc)
{
    static const char resp[] = 
        "<s:Envelope "
        "xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" "
        "s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
        "<s:Body>"
        "<s:Fault>"
        "<faultcode>s:Client</faultcode>"
        "<faultstring>UPnPError</faultstring>"
        "<detail>"
        "<UPnPError xmlns=\"urn:schemas-upnp-org:control-1-0\">"
        "<errorCode>%d</errorCode>"
        "<errorDescription>%s</errorDescription>"
        "</UPnPError>"
        "</detail>"
        "</s:Fault>"
        "</s:Body>"
        "</s:Envelope>";

    char body[2048];
    int bodylen;

    NP_UPNP_DEBUG("Returning UPnPError %d: %s\n", errCode, errDesc);
    bodylen = snprintf(body, sizeof(body), resp, errCode, errDesc);
    BuildResp2_upnphttp(h, 500, "Internal Server Error", body, bodylen);
    SendRespAndClose_upnphttp(h);
}

