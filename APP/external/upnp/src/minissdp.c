/* $Id: minissdp.c,v 1.27 2011/05/23 12:39:41 nanard Exp $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2006-2011 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */
 
   /**********************问题单修改记录******************************************
    日期              修改人         问题单号           修改内容
  
 2012.03.14        z00203875     2031401440    修改打LOG方式
 2012.03.26        z00203875     2032603705    LOG打到SDT中  
 2012.07.06        z00203875     2061908201    图标低概率不出现
 2012.08.29        z00203875     2082304944    UPnP认证测试
 2012.09.29        z00203875     2092807655    WIN7系统图标无拨号菜单
 2012.12.24        y44942        2121101411    消除Fortify告警
 2013.08.22        y00248130     3082010042    消除Fortify告警
 2014.08.13        n00202065     4081205734    CNNVD-201307-535   MiniUPnP ‘Minissdp.c’信息泄露漏洞 2013-08-09
                                                     攻击者可利用该漏洞获取敏感信息，有助于发起进一步攻击
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
/* BEGIN 2092807655 zhoujianchun 00203875 2012.9.28 modified */
#include <ctype.h>
/* END   2092807655 zhoujianchun 00203875 2012.9.28 modified */
#include "config.h"
#include "upnpdescstrings.h"
#include "miniupnpdpath.h"
#include "upnphttp.h"
#include "upnpglobalvars.h"
#include "minissdp.h"
#include "upnputils.h"
#include "codelength.h"
#include "log_lib.h"

/* SSDP ip/port */
#define SSDP_PORT (1900)
#define SSDP_MCAST_ADDR ("239.255.255.250")
#define LL_SSDP_MCAST_ADDR ("FF02::C")
#define SL_SSDP_MCAST_ADDR ("FF05::C")

/* AddMulticastMembership()
 * param s		socket
 * param ifaddr	ip v4 address
 */
static int
AddMulticastMembership(int s, in_addr_t ifaddr)
{
	struct ip_mreq imr;	/* Ip multicast membership */

    /* setting up imr structure */
    imr.imr_multiaddr.s_addr = inet_addr(SSDP_MCAST_ADDR);
    /*imr.imr_interface.s_addr = htonl(INADDR_ANY);*/
    imr.imr_interface.s_addr = ifaddr;	/*inet_addr(ifaddr);*/
	
	if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (void *)&imr, sizeof(struct ip_mreq)) < 0)
	{
        NP_UPNP_ERROR("setsockopt(udp, IP_ADD_MEMBERSHIP): %\n", strerror(errno));
		return -1;
    }

	return 0;
}

/* AddMulticastMembershipIPv6()
 * param s	socket (IPv6)
 * To be improved to target specific network interfaces */
#ifdef ENABLE_IPV6
static int
AddMulticastMembershipIPv6(int s)
{
	struct ipv6_mreq mr;
	/*unsigned int ifindex;*/

	memset(&mr, 0, sizeof(mr));
	inet_pton(AF_INET6, LL_SSDP_MCAST_ADDR, &mr.ipv6mr_multiaddr);
	/*mr.ipv6mr_interface = ifindex;*/
	mr.ipv6mr_interface = 0; /* 0 : all interfaces */
#ifndef IPV6_ADD_MEMBERSHIP
#define IPV6_ADD_MEMBERSHIP IPV6_JOIN_GROUP
#endif
	if(setsockopt(s, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mr, sizeof(struct ipv6_mreq)) < 0)
	{
		NP_UPNP_ERROR("setsockopt(udp, IPV6_ADD_MEMBERSHIP): %s\n", strerror(errno));
		return -1;
	}
	inet_pton(AF_INET6, SL_SSDP_MCAST_ADDR, &mr.ipv6mr_multiaddr);
	if(setsockopt(s, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mr, sizeof(struct ipv6_mreq)) < 0)
	{
		NP_UPNP_ERROR("setsockopt(udp, IPV6_ADD_MEMBERSHIP): %s\n", strerror(errno));
		return -1;
	}
	return 0;
}
#endif

/* Open and configure the socket listening for 
 * SSDP udp packets sent on 239.255.255.250 port 1900
 * SSDP v6 udp packets sent on FF02::C, or FF05::C, port 1900 */
int
OpenAndConfSSDPReceiveSocket(int ipv6)
{
	int s;
	struct sockaddr_storage sockname;
	socklen_t sockname_len;
	struct lan_addr_s * lan_addr;
	int j = 1;

	if( (s = socket(ipv6 ? PF_INET6 : PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		NP_UPNP_ERROR("socket(udp): %s\n, strerror(errno)");
		return -1;
	}

	memset(&sockname, 0, sizeof(struct sockaddr_storage));
	if(ipv6) {
		struct sockaddr_in6 * saddr = (struct sockaddr_in6 *)&sockname;
		saddr->sin6_family = AF_INET6;
		saddr->sin6_port = htons(SSDP_PORT);
		saddr->sin6_addr = in6addr_any;
		sockname_len = sizeof(struct sockaddr_in6);
	} else {
		struct sockaddr_in * saddr = (struct sockaddr_in *)&sockname;
		saddr->sin_family = AF_INET;
		saddr->sin_port = htons(SSDP_PORT);
		/* NOTE : it seems it doesnt work when binding on the specific address */
		/*saddr->sin_addr.s_addr = inet_addr(UPNP_MCAST_ADDR);*/
		saddr->sin_addr.s_addr = htonl(INADDR_ANY);
		/*saddr->sin_addr.s_addr = inet_addr(ifaddr);*/
		sockname_len = sizeof(struct sockaddr_in);
	}

	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &j, sizeof(j)) < 0)
	{
		NP_UPNP_WARN("setsockopt(udp, SO_REUSEADDR): %s\n", strerror(errno));
	}


	if(bind(s, (struct sockaddr *)&sockname, sockname_len) < 0)
	{
		NP_UPNP_ERROR("bind(udp%s): %s\n", ipv6 ? "6" : "", strerror(errno));
		close(s);
		return -1;
	}

#ifdef ENABLE_IPV6
	if(ipv6)
	{
		AddMulticastMembershipIPv6(s);
	}
	else
#endif
	{
		for(lan_addr = lan_addrs.lh_first; lan_addr != NULL; lan_addr = lan_addr->list.le_next)
		{
			if(AddMulticastMembership(s, lan_addr->addr.s_addr) < 0)
			{
				NP_UPNP_WARN("Failed to add multicast membership for interface %s\n", 
				       lan_addr->str);
			}
		}
	}

	return s;
}

/* open the UDP socket used to send SSDP notifications to
 * the multicast group reserved for them */
static int
OpenAndConfSSDPNotifySocket(in_addr_t addr)
{
	int s;
	unsigned char loopchar = 0;
	int bcast = 1;
	struct in_addr mc_if;
	struct sockaddr_in sockname;
	
	if( (s = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
	{
		NP_UPNP_ERROR("socket(udp_notify): %s\n", strerror(errno));
		return -1;
	}

	mc_if.s_addr = addr;	/*inet_addr(addr);*/

	if(setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, (char *)&loopchar, sizeof(loopchar)) < 0)
	{
		NP_UPNP_ERROR("setsockopt(udp_notify, IP_MULTICAST_LOOP): %s\n", strerror(errno));
		close(s);
		return -1;
	}

	if(setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF, (char *)&mc_if, sizeof(mc_if)) < 0)
	{
		NP_UPNP_ERROR("setsockopt(udp_notify, IP_MULTICAST_IF): %s\n", strerror(errno));
		close(s);
		return -1;
	}
	
	if(setsockopt(s, SOL_SOCKET, SO_BROADCAST, &bcast, sizeof(bcast)) < 0)
	{
		NP_UPNP_ERROR("setsockopt(udp_notify, SO_BROADCAST): %s\n", strerror(errno));
		close(s);
		return -1;
	}

	memset(&sockname, 0, sizeof(struct sockaddr_in));
    sockname.sin_family = AF_INET;
    sockname.sin_addr.s_addr = addr;	/*inet_addr(addr);*/

    if (bind(s, (struct sockaddr *)&sockname, sizeof(struct sockaddr_in)) < 0)
	{
		NP_UPNP_ERROR("bind(udp_notify): %s\n", strerror(errno));
		close(s);
		return -1;
    }

	return s;
}

int
OpenAndConfSSDPNotifySockets(int * sockets)
/*OpenAndConfSSDPNotifySockets(int * sockets,
                             struct lan_addr_s * lan_addr, int n_lan_addr)*/
{
	int i, j;
	struct lan_addr_s * lan_addr;

	for(i=0, lan_addr = lan_addrs.lh_first; lan_addr != NULL; lan_addr = lan_addr->list.le_next, i++)
	{
		sockets[i] = OpenAndConfSSDPNotifySocket(lan_addr->addr.s_addr);
		if(sockets[i] < 0)
		{
			for(j=0; j<i; j++)
			{
				close(sockets[j]);
				sockets[j] = -1;
			}
			return -1;
		}
	}
	return 0;
}
/* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
static const char * const known_service_types[] =
{
	"upnp:rootdevice",
	"urn:schemas-upnp-org:device:InternetGatewayDevice:",
	"urn:schemas-upnp-org:device:WANConnectionDevice:",
	"urn:schemas-upnp-org:device:WANDevice:",
	"urn:schemas-upnp-org:service:WANCommonInterfaceConfig:",
	"urn:schemas-upnp-org:service:WANIPConnection:",
	//"urn:schemas-upnp-org:service:WANPPPConnection:",
#ifdef ENABLE_L3F_SERVICE
	"urn:schemas-upnp-org:service:Layer3Forwarding:",
#endif
#ifdef ENABLE_6FC_SERVICE
	"url:schemas-upnp-org:service:WANIPv6FirewallControl:",
#endif
	0
};
/* END   2082304944 zhoujianchun 00203875 2012.8.29 added */

/*
 * response from a LiveBox (Wanadoo)
HTTP/1.1 200 OK
CACHE-CONTROL: max-age=1800
DATE: Thu, 01 Jan 1970 04:03:23 GMT
EXT:
LOCATION: http://192.168.0.1:49152/gatedesc.xml
SERVER: Linux/2.4.17, UPnP/1.0, Intel SDK for UPnP devices /1.2
ST: upnp:rootdevice
USN: uuid:75802409-bccb-40e7-8e6c-fa095ecce13e::upnp:rootdevice

 * response from a Linksys 802.11b :
HTTP/1.1 200 OK
Cache-Control:max-age=120
Location:http://192.168.5.1:5678/rootDesc.xml
Server:NT/5.0 UPnP/1.0
ST:upnp:rootdevice
USN:uuid:upnp-InternetGatewayDevice-1_0-0090a2777777::upnp:rootdevice
EXT:
 */

/* not really an SSDP "announce" as it is the response
 * to a SSDP "M-SEARCH" */
static void
SendSSDPAnnounce2(int s, const struct sockaddr * addr,
                  const char * st, int st_len, const char * suffix,
                  /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 modified */
                  const char * host, unsigned short port, int idx)
                  /* END   2082304944 zhoujianchun 00203875 2012.8.29 modified */
{
	int l, n;
	char buf[512];
	socklen_t addrlen;
    /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
    char date[MAX_DATE_LEN] = {0};
    get_formatted_date(date);
    if(-1 == idx) /* st is uuid */
    {
        l = snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\r\n"
            /*BEGIN 2061908201 zhoujianchun 00203875 2012.07.06 modified */
            "CACHE-CONTROL: max-age=1800\r\n" /* follow protocol's advice */
            /*END   2061908201 zhoujianchun 00203875 2012.07.06 modified */
            "ST: %s\r\n"
            "USN: %s\r\n"
            "EXT:\r\n"
            "SERVER: " MINIUPNPD_SERVER_STRING "\r\n"
            "LOCATION: http://%s:%u" ROOTDESC_PATH "\r\n"
            "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n" /* UDA v1.1 */
            "01-NLS: %u\r\n" /* same as BOOTID. UDA v1.1 */
            "BOOTID.UPNP.ORG: %u\r\n" /* UDA v1.1 */
            "CONFIGID.UPNP.ORG: %u\r\n" /* UDA v1.1 */
            "DATE: %s\r\n"
            "\r\n",
            st, st,
            host, (unsigned int)port,
            upnp_bootid, upnp_bootid, upnp_configid, date);
        if(l<0)
        {
            NP_UPNP_DEBUG("SendSSDPAnnounce2(): snprintf failed %m");
            return;
        }
        else if((unsigned)l>=sizeof(buf))
        {
            NP_UPNP_DEBUG("SendSSDPAnnounce2(): truncated output");
            l = sizeof(buf) - 1;
        }
        addrlen = (addr->sa_family == AF_INET6) 
            ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
        n = sendto(s, buf, l, 0, addr, addrlen);
        NP_UPNP_DEBUG("SSDP Announce %d bytes to %s:%d ST: %.*s\n",n,
                inet_ntoa(((const struct sockaddr_in *)addr)->sin_addr),
                ntohs(((const struct sockaddr_in *)addr)->sin_port), l, buf);
        if(n < 0)
        {
            NP_UPNP_ERROR("sendto(udp): %s\n", strerror(errno));
        }
        return;
    }
    /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
    /*BEGIN 2061908201 zhoujianchun 00203875 2012.07.06 modified */
    /* 
     * follow guideline from document "UPnP Device Architecture 1.0"
     * uppercase is recommended.
     * DATE: is recommended
     * SERVER: OS/ver UPnP/1.0 miniupnpd/1.0
     * - check what to put in the 'Cache-Control' header 
     *
     * have a look at the document "UPnP Device Architecture v1.1
     */
    l = snprintf(buf, sizeof(buf), "HTTP/1.1 200 OK\r\n"
            /*BEGIN 2061908201 zhoujianchun 00203875 2012.07.06 modified */
            "CACHE-CONTROL: max-age=1800\r\n" /* follow protocol's advice */
            /*END   2061908201 zhoujianchun 00203875 2012.07.06 modified */
            "ST: %.*s%s\r\n"
            "USN: %s::%.*s%s\r\n"
            "EXT:\r\n"
            "SERVER: " MINIUPNPD_SERVER_STRING "\r\n"
            "LOCATION: http://%s:%u" ROOTDESC_PATH "\r\n"
            /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 modified */
            "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n" /* UDA v1.1 */
            /* END   2082304944 zhoujianchun 00203875 2012.8.29 modified */
            "01-NLS: %u\r\n" /* same as BOOTID. UDA v1.1 */
            "BOOTID.UPNP.ORG: %u\r\n" /* UDA v1.1 */
            "CONFIGID.UPNP.ORG: %u\r\n" /* UDA v1.1 */
            /* BEGIN   2082304944 zhoujianchun 00203875 2012.8.29 added */
            "DATE: %s\r\n"
            /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
            "\r\n",
            st_len, st, suffix,
            /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 modified */
            uuid_value[idx], st_len, st, suffix,
            /* END   2082304944 zhoujianchun 00203875 2012.8.29 modified */
            host, (unsigned int)port,
            /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 modified */
            upnp_bootid, upnp_bootid, upnp_configid, date);
    if(l<0)
    {
        NP_UPNP_DEBUG("SendSSDPAnnounce2(): snprintf failed %m");
        return;
    }
    else if((unsigned)l>=sizeof(buf))
    {
        NP_UPNP_DEBUG("SendSSDPAnnounce2(): truncated output");
        l = sizeof(buf) - 1;
    }
    addrlen = (addr->sa_family == AF_INET6)
        ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
    n = sendto(s, buf, l, 0, addr, addrlen);
    NP_UPNP_DEBUG("SSDP Announce %d bytes to %s:%d ST: %.*s\n",n,
            inet_ntoa(((const struct sockaddr_in *)addr)->sin_addr),
            ntohs(((const struct sockaddr_in *)addr)->sin_port), l, buf);
    if(n < 0)
    {
        NP_UPNP_ERROR("sendto(udp): %s\n", strerror(errno));
    }
    /*END   2061908201 zhoujianchun 00203875 2012.07.06 modified */
}

static void
SendSSDPNotifies(int s, const char * host, unsigned short port,
                 unsigned int lifetime)
{
    NP_UPNP_INFO("enter SendSSDPNotifies\n");
    NP_UPNP_DEBUG("host is %s\n", host);
#ifdef DEBUG
    struct lan_addr_s *lan_addr;
    struct sockaddr_in addr;
    socklen_t len = sizeof(addr);
    int k;
    for(k = 0, lan_addr = lan_addrs.lh_first; lan_addr != NULL; 
            lan_addr = lan_addr->list.le_next, k++)
    {
        NP_UPNP_DEBUG("--%s--\n", lan_addr->str);
    }
    if(getsockname(s, (struct sockaddr *)&addr, &len) < 0)
    {
        perror("getsockname()");
    }
    else
    {
        char buf[100];
        if(!inet_ntop(AF_INET, &(addr.sin_addr), buf, 100))
        {
            perror("inet_ntop()");
        }
        NP_UPNP_DEBUG("listenning addr is %s\n", buf);
    }

#endif
	struct sockaddr_in sockname;
	int l, n, i=0;
	char bufr[512];
    /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
    char date[MAX_DATE_LEN] = {0};
    /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */

	memset(&sockname, 0, sizeof(struct sockaddr_in));
	sockname.sin_family = AF_INET;
	sockname.sin_port = htons(SSDP_PORT);
	sockname.sin_addr.s_addr = inet_addr(SSDP_MCAST_ADDR);

	while(known_service_types[i])
	{
        /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
        get_formatted_date(date);
        /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
		l = snprintf(bufr, sizeof(bufr), 
			"NOTIFY * HTTP/1.1\r\n"
			"HOST: %s:%d\r\n"
			"CACHE-CONTROL: max-age=%u\r\n"
			"lOCATION: http://%s:%d" ROOTDESC_PATH"\r\n"
			"SERVER: " MINIUPNPD_SERVER_STRING "\r\n"
			"NT: %s%s\r\n"
			"USN: %s::%s%s\r\n"
			"NTS: ssdp:alive\r\n"
           /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 modified */
			"OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n" /* UDA v1.1 */
           /* END   2082304944 zhoujianchun 00203875 2012.8.29 modified */
			"01-NLS: %u\r\n" /* same as BOOTID field. UDA v1.1 */
			"BOOTID.UPNP.ORG: %u\r\n" /* UDA v1.1 */
			"CONFIGID.UPNP.ORG: %u\r\n" /* UDA v1.1 */
            /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
			"DATE: %s\r\n"
            /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
			"\r\n",
			SSDP_MCAST_ADDR, SSDP_PORT,
			lifetime,
			host, port,
#ifdef IGD_V2
			known_service_types[i], (i==0?"":((i==4 || i==6)?"1":"2")),
            /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 modified */
			uuid_value[i], known_service_types[i], (i==0?"":((i==4 || i==6)?"1":"2")),
#else
			known_service_types[i], (i==0?"":"1"),
			uuid_value[i], known_service_types[i], (i==0?"":"1"),
#endif
			upnp_bootid, upnp_bootid, upnp_configid, date);
		if(l<0)
		{
			NP_UPNP_DEBUG("SendSSDPAnnounce2(): snprintf failed %m");
			return;
		}
		else if((unsigned)l>=sizeof(bufr))
		{
			NP_UPNP_DEBUG("SendSSDPAnnounce2(): truncated output");
			l = sizeof(bufr) - 1;
		}
		n = sendto(s, bufr, l, 0,
			(struct sockaddr *)&sockname, sizeof(struct sockaddr_in) );
		if(n < 0)
		{
			NP_UPNP_ERROR("sendto(udp_notify=%d, %s): %s\n", s, host, strerror(errno));
		}
		i++;
	}
    /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
    for(i = 0; i < DEV_NUM; i++)
    {
        get_formatted_date(date);
		l = snprintf(bufr, sizeof(bufr), 
			"NOTIFY * HTTP/1.1\r\n"
			"HOST: %s:%d\r\n"
			"CACHE-CONTROL: max-age=%u\r\n"
			"lOCATION: http://%s:%d" ROOTDESC_PATH"\r\n"
			"SERVER: " MINIUPNPD_SERVER_STRING "\r\n"
			"NT: %s\r\n"
			"USN: %s\r\n"
			"NTS: ssdp:alive\r\n"
			"OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n" /* UDA v1.1 */
			"01-NLS: %u\r\n" /* same as BOOTID field. UDA v1.1 */
			"BOOTID.UPNP.ORG: %u\r\n" /* UDA v1.1 */
			"CONFIGID.UPNP.ORG: %u\r\n" /* UDA v1.1 */
			"DATE: %s\r\n"
			"\r\n",
			SSDP_MCAST_ADDR, SSDP_PORT,
			lifetime,
			host, port,
			uuid_value[i], uuid_value[i],
			upnp_bootid, upnp_bootid, upnp_configid, date);
		if(l<0)
		{
			NP_UPNP_DEBUG("SendSSDPAnnounce2(): snprintf failed %m");
			return;
		}
		else if((unsigned)l>=sizeof(bufr))
		{
			NP_UPNP_DEBUG("SendSSDPAnnounce2(): truncated output");
			l = sizeof(bufr) - 1;
		}
		n = sendto(s, bufr, l, 0,
			(struct sockaddr *)&sockname, sizeof(struct sockaddr_in) );
		if(n < 0)
		{
			NP_UPNP_ERROR("sendto(udp_notify=%d, %s): %s\n", s, host, strerror(errno));
		}
    }
    /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
}

void
SendSSDPNotifies2(int * sockets,
                  unsigned short port,
                  unsigned int lifetime)
{
	int i;
	struct lan_addr_s * lan_addr;
	for(i=0, lan_addr = lan_addrs.lh_first; lan_addr != NULL; lan_addr = lan_addr->list.le_next, i++)
	{
		SendSSDPNotifies(sockets[i], lan_addr->str, port, lifetime);
	}
}

/* ProcessSSDPRequest()
 * process SSDP M-SEARCH requests and responds to them */
void
ProcessSSDPRequest(int s, unsigned short port)
{
	int n;
	char bufr[1500];
	socklen_t len_r;
    /* BEGIN   2082304944 zhoujianchun 00203875 2012.8.29 added */
    int i = 0;
    char *ptr = NULL;
    /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
#ifdef ENABLE_IPV6
	struct sockaddr_storage sendername;
	len_r = sizeof(struct sockaddr_storage);
#else
	struct sockaddr_in sendername;
	len_r = sizeof(struct sockaddr_in);
#endif

	n = recvfrom(s, bufr, sizeof(bufr), 0,
	             (struct sockaddr *)&sendername, &len_r);
	if(n < 0)
	{
		NP_UPNP_ERROR("recvfrom(udp): %s\n", strerror(errno));
		return;
	}
    /* BEGIN   2082304944 zhoujianchun 00203875 2012.8.29 added */
    for(i = 0; i < n; i++)
    {
        /* BEGIN 2092807655 zhoujianchun 00203875 2012.9.28 modified */
        if((0 == strncasecmp(bufr + i, "MAN:", strlen("MAN:")))
                &&(('\n' == (char)(*(bufr + i - 1))) || ('\r' == (char)(*(bufr + i - 1)))))
        /* END   2092807655 zhoujianchun 00203875 2012.9.28 modified */
        {
            ptr = bufr + i + strlen("MAN:");
            while(isspace(*ptr))
            {
                ptr++;
            }
            if(0 != strncasecmp(ptr, "\"ssdp:discover\"", strlen("\"ssdp:discover\"")))
            {
                NP_UPNP_ERROR("M-SEARCH WITH WRONG MAN HEADER.\n");
                return;
            }
        }
    }
    /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
	ProcessSSDPData(s, bufr, n, (struct sockaddr *)&sendername, port);

}

void
ProcessSSDPData(int s, const char *bufr, int n,
                const struct sockaddr * sender, unsigned short port) {
	int i, l;
	struct lan_addr_s * lan_addr = NULL;
	const char * st = NULL;
	int st_len = 0;
	char sender_str[64];
	const char * announced_host = NULL;
    /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
    char has_mx = 0;
    /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */

	/* get the string representation of the sender address */
	sockaddr_to_string(sender, sender_str, sizeof(sender_str));

	if(memcmp(bufr, "NOTIFY", 6) == 0)
	{
		/* ignore NOTIFY packets. We could log the sender and device type */
		return;
	}
	else if(memcmp(bufr, "M-SEARCH", 8) == 0)
	{
		i = 0;
		while(i < n)
		{
			while((i < n - 1) && (bufr[i] != '\r' || bufr[i+1] != '\n'))
				i++;
			i += 2;
            /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
            if((i < n - 3) && (0 == strncasecmp(bufr+i, "mx:", 3)))
            {
                has_mx = 1;
            }
            /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
			if((i < n - 3) && (strncasecmp(bufr+i, "st:", 3) == 0))
			{
				st = bufr+i+3;
				st_len = 0;
				while((*st == ' ' || *st == '\t') && (st < bufr + n))
					st++;
				while(st[st_len]!='\r' && st[st_len]!='\n'
				     && (st + st_len < bufr + n))
					st_len++;
			}
		}
        /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
        if(0 == has_mx)
        {
            NP_UPNP_DEBUG("M-SEARCH, NO MX: RETURN.\n");
            return;
        }
        /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
		if(st && (st_len > 0))
		{
            /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
            if(strncmp("upnp:rootdevice", st, st_len) 
                    && strncmp("ssdp:all", st, st_len) 
                    && strncmp("urn:schemas-upnp-org:device:InternetGatewayDevice:1", st, st_len)
                    && strncmp("urn:schemas-upnp-org:device:WANConnectionDevice:1", st, st_len)
                    && strncmp("urn:schemas-upnp-org:device:WANDevice:1", st, st_len)
                    && strncmp("urn:schemas-upnp-org:service:WANCommonInterfaceConfig:1", st, st_len)
                    && strncmp("urn:schemas-upnp-org:service:WANIPConnection:1", st, st_len)
                    && strncmp("urn:schemas-upnp-org:service:Layer3Forwarding:1", st, st_len)
                    && strncmp("urn:schemas-upnp-org:service:WANIPv6FirewallControl:1", st, st_len)
                    && strncmp(uuidvalue_root, st, st_len)
                    && strncmp(uuidvalue_wan, st, st_len)
                    && strncmp(uuidvalue_wan_conn, st, st_len)
              )
            {
                NP_UPNP_DEBUG("ST IS NOT CORRECT, RETURN.\n");
                return;
            }
            /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
			/* TODO : doesnt answer at once but wait for a random time */
			NP_UPNP_DEBUG("SSDP M-SEARCH from %s ST: %.*s\n",
			       sender_str, st_len, st);
			/* find in which sub network the client is */
			if(sender->sa_family == AF_INET)
			{
				for(lan_addr = lan_addrs.lh_first;
				    lan_addr != NULL;
				    lan_addr = lan_addr->list.le_next)
				{
					if( (((const struct sockaddr_in *)sender)->sin_addr.s_addr & lan_addr->mask.s_addr)
				   == (lan_addr->addr.s_addr & lan_addr->mask.s_addr))
						break;
				}
				if (lan_addr == NULL)
				{
					NP_UPNP_ERROR("Can't find in which sub network the client is\n");
					return;
				}
				announced_host = lan_addr->str;
			}
#ifdef ENABLE_IPV6
			else
			{
				/* IPv6 address with brackets */
				announced_host = ipv6_addr_for_http_with_brackets;
			}
#endif
			/* Responds to request with a device as ST header */
			for(i = 0; known_service_types[i]; i++)
			{
				l = (int)strlen(known_service_types[i]);
				if(l<=st_len && (0 == memcmp(st, known_service_types[i], l)))
				{
					NP_UPNP_DEBUG("Single search found\n");
					SendSSDPAnnounce2(s, sender,
					                  st, st_len, "",
                                      /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 modified */
					                  announced_host, port, i);
                                      /* END   2082304944 zhoujianchun 00203875 2012.8.29 modified */
					break;
				}
			}
			/* Responds to request with ST: ssdp:all */
			/* strlen("ssdp:all") == 8 */
			if(st_len==8 && (0 == memcmp(st, "ssdp:all", 8)))
			{
				NP_UPNP_DEBUG("ssdp:all found\n");
				for(i=0; known_service_types[i]; i++)
				{
					l = (int)strlen(known_service_types[i]);
					SendSSDPAnnounce2(s, sender,
                                      /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 modified */
					                  known_service_types[i], l, i==0 ? "" : "1",
					                  announced_host, port, i);
                                      /* END   2082304944 zhoujianchun 00203875 2012.8.29 modified */
				}
                /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
                for(i = 0; i < DEV_NUM; i++)
                {
				    SendSSDPAnnounce2(s, sender, uuid_value[i], st_len, "", announced_host, port, -1);
                }
                /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
			}
			/* responds to request by UUID value */
			l = (int)strlen(uuidvalue);
            /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 modified */
			if(l==st_len && 0 == memcmp(st, uuidvalue_root, l)) 
			{
				NP_UPNP_DEBUG("ssdp:uuid found\n");
				SendSSDPAnnounce2(s, sender, uuidvalue_root, st_len, "",
				                  announced_host, port, -1);
			}
            /* END   2082304944 zhoujianchun 00203875 2012.8.29 modified */
            /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
            else if(l==st_len && 0 == memcmp(st, uuidvalue_wan, l))
            {
				NP_UPNP_DEBUG("ssdp:uuid found\n");
				SendSSDPAnnounce2(s, sender, uuidvalue_wan, st_len, "", announced_host, port, -1);
            }
            else if(l==st_len && 0 == memcmp(st, uuidvalue_wan_conn, l))
            {
				NP_UPNP_DEBUG("ssdp:uuid found\n");
				SendSSDPAnnounce2(s, sender, uuidvalue_wan_conn, st_len, "", announced_host, port, -1);
            }
            /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
		}
		else
		{
			NP_UPNP_DEBUG("Invalid SSDP M-SEARCH from %s\n", sender_str);
		}
	}
	else
	{
		NP_UPNP_DEBUG("Unknown udp packet received from %s\n", sender_str);
	}
}

/* This will broadcast ssdp:byebye notifications to inform 
 * the network that UPnP is going down. */
int
SendSSDPGoodbye(int * sockets, int n_sockets)
{
	struct sockaddr_in sockname;
	int n, l;
	int i, j;
	char bufr[512];

    memset(&sockname, 0, sizeof(struct sockaddr_in));
    sockname.sin_family = AF_INET;
    sockname.sin_port = htons(SSDP_PORT);
    sockname.sin_addr.s_addr = inet_addr(SSDP_MCAST_ADDR);

    NP_UPNP_DEBUG("enter SendSSDPGoodbye\n");

	for(j=0; j<n_sockets; j++)
	{
	    for(i=0; known_service_types[i]; i++)
	    {
	        l = snprintf(bufr, sizeof(bufr),
                 "NOTIFY * HTTP/1.1\r\n"
                 "HOST: %s:%d\r\n"
                 "NT: %s%s\r\n"
                 "USN: %s::%s%s\r\n"
                 "NTS: ssdp:byebye\r\n"
                 /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 modified */
				 "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n" /* UDA v1.1 */
                 /* END   2082304944 zhoujianchun 00203875 2012.8.29 modified */
				 "01-NLS: %u\r\n" /* same as BOOTID field. UDA v1.1 */
				 "BOOTID.UPNP.ORG: %u\r\n" /* UDA v1.1 */
				 "CONFIGID.UPNP.ORG: %u\r\n" /* UDA v1.1 */
                 "\r\n",
                 SSDP_MCAST_ADDR, SSDP_PORT,
				 known_service_types[i], (i==0?"":"1"),
                 /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 modified */
                 uuid_value[i], known_service_types[i], (i==0?"":"1"),
                 /* END   2082304944 zhoujianchun 00203875 2012.8.29 modified */
                 upnp_bootid, upnp_bootid, upnp_configid);
			if(l<0)
			{
				NP_UPNP_DEBUG("SendSSDPAnnounce2(): snprintf failed %m");
				return -1;
			}
			else if((unsigned)l>=sizeof(bufr))
			{
				NP_UPNP_DEBUG("SendSSDPAnnounce2(): truncated output");
				l = sizeof(bufr) - 1;
			}
			n = sendto(sockets[j], bufr, l, 0,
					(struct sockaddr *)&sockname, sizeof(struct sockaddr_in) );
			if(n < 0)
			{
				NP_UPNP_ERROR("SendSSDPGoodbye: sendto(udp_shutdown=%d): %s\n",
				       sockets[j], strerror(errno));
				return -1;
			}
    	}
        /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
        for(i = 0; i < DEV_NUM; i++)
        {
	        l = snprintf(bufr, sizeof(bufr),
                 "NOTIFY * HTTP/1.1\r\n"
                 "HOST: %s:%d\r\n"
                 "NT: %s\r\n"
                 "USN: %s\r\n"
                 "NTS: ssdp:byebye\r\n"
				 "OPT: \"http://schemas.upnp.org/upnp/1/0/\"; ns=01\r\n" /* UDA v1.1 */
				 "01-NLS: %u\r\n" /* same as BOOTID field. UDA v1.1 */
				 "BOOTID.UPNP.ORG: %u\r\n" /* UDA v1.1 */
				 "CONFIGID.UPNP.ORG: %u\r\n" /* UDA v1.1 */
                 "\r\n",
                 SSDP_MCAST_ADDR, SSDP_PORT,
                 uuid_value[i], uuid_value[i],
                 upnp_bootid, upnp_bootid, upnp_configid);
			if(l<0)
			{
				NP_UPNP_DEBUG("SendSSDPAnnounce2(): snprintf failed %m");
				return -1;
			}
			else if((unsigned)l>=sizeof(bufr))
			{
				NP_UPNP_DEBUG("SendSSDPAnnounce2(): truncated output");
				l = sizeof(bufr) - 1;
			}
			n = sendto(sockets[j], bufr, l, 0,
					(struct sockaddr *)&sockname, sizeof(struct sockaddr_in) );
			if(n < 0)
			{
				NP_UPNP_ERROR("SendSSDPGoodbye: sendto(udp_shutdown=%d): %s\n",
				       sockets[j], strerror(errno));
				return -1;
			}

        }
        /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */
	}
	return 0;
}

/* SubmitServicesToMiniSSDPD() :
 * register services offered by MiniUPnPd to a running instance of
 * MiniSSDPd */
int
SubmitServicesToMiniSSDPD(const char * host, unsigned short port) {
	struct sockaddr_un addr;
	int s;
	unsigned char buffer[2048];
	char strbuf[256];
	unsigned char * p;
	int i, l;

	s = socket(AF_UNIX, SOCK_STREAM, 0);
	if(s < 0)
    {
		NP_UPNP_ERROR("socket(unix): %s\n", strerror(errno));
		return -1;
	}
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, minissdpdsocketpath, sizeof(addr.sun_path));
	if(connect(s, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) < 0) {
		NP_UPNP_ERROR("connect(\"%s\"): %s\n", minissdpdsocketpath, strerror(errno));
        close(s);
		return -1;
	}
	for(i = 0; known_service_types[i]; i++) {
		buffer[0] = 4;
		p = buffer + 1;
		l = (int)strlen(known_service_types[i]);
		if(i > 0)
			l++;
		CODELENGTH(l, p);
		memcpy(p, known_service_types[i], l);
		if(i > 0)
			p[l-1] = '1';
		p += l;
		l = snprintf(strbuf, sizeof(strbuf), "%s::%s%s", 
		             uuidvalue, known_service_types[i], (i==0)?"":"1");
		CODELENGTH(l, p);
		memcpy(p, strbuf, l);
		p += l;
		l = (int)strlen(MINIUPNPD_SERVER_STRING);
		CODELENGTH(l, p);
		memcpy(p, MINIUPNPD_SERVER_STRING, l);
		p += l;
		l = snprintf(strbuf, sizeof(strbuf), "http://%s:%u" ROOTDESC_PATH,
		             host, (unsigned int)port);
		CODELENGTH(l, p);
		memcpy(p, strbuf, l);
		p += l;
		if(write(s, buffer, p - buffer) < 0) {
			NP_UPNP_ERROR("write(): %s", strerror(errno));
            /*BEGIN 3082010042 y00248130 2013-8-22 added/modified*/
            close(s);             
            /* END  3082010042 y00248130 2013-8-22 added/modified*/
			return -1;
		}
	}
 	close(s);
	return 0;
}

