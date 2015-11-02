/* $Id: getifaddr.c,v 1.11 2011/05/15 08:59:27 nanard Exp $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2006-2011 Thomas Bernard 
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */
 
   /**********************问题单修改记录******************************************
    日期              修改人         问题单号           修改内容
  
 2012.03.14        z00203875     2031401440    修改打LOG方式    
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#if defined(sun)
#include <sys/sockio.h>
#endif
#include "config.h"
#include "getifaddr.h"
#if defined(USE_GETIFADDRS) || defined(ENABLE_IPV6)
#include "ifaddrs.h"
#endif

int
getifaddr(const char * ifname, char * buf, int len)
{
#ifndef USE_GETIFADDRS
#ifdef DEBUG
    printf("enter getifaddr, USE_GETIFADDRS micro is defined.\n");
    printf("args for getifaddr are: ifname = %s, buf = %s, len = %d\n", ifname, buf, len);
#endif
	/* use ioctl SIOCGIFADDR. Works only for ip v4 */
	/* SIOCGIFADDR struct ifreq *  */
	int s;
	struct ifreq ifr;
	int ifrlen;
	struct sockaddr_in * addr;
	ifrlen = sizeof(ifr);

	if(!ifname || ifname[0]=='\0')
		return -1;
	s = socket(PF_INET, SOCK_DGRAM, 0);
	if(s < 0)
	{
		NP_UPNP_ERROR("socket(PF_INET, SOCK_DGRAM): %s\n", strerror(errno));
		return -1;
	}
	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	if(ioctl(s, SIOCGIFADDR, &ifr, &ifrlen) < 0)
	{
		NP_UPNP_ERROR("ioctl(s, SIOCGIFADDR, ...): %s\n", strerror(errno));
		close(s);
		return -1;
	}
	addr = (struct sockaddr_in *)&ifr.ifr_addr;
	if(!inet_ntop(AF_INET, &addr->sin_addr, buf, len))
	{
		NP_UPNP_ERROR("inet_ntop(): %s\n", strerror(errno));
		close(s);
		return -1;
	}
	close(s);
#else /* ifndef USE_GETIFADDRS */
	/* Works for all address families (both ip v4 and ip v6) */
	struct ifaddrs * ifap;
	struct ifaddrs * ife;

	if(!ifname || ifname[0]=='\0')
		return -1;
	if(getifaddrs(&ifap)<0)
	{
		NP_UPNP_ERROR("getifaddrs: %s\n", strerror(errno));
		return -1;
	}
	for(ife = ifap; ife; ife = ife->ifa_next)
	{
		/* skip other interfaces */
		if(0 != strcmp(ifname, ife->ifa_name))
			continue;
		switch(ife->ifa_addr->sa_family)
		{
		case AF_INET:
			inet_ntop(ife->ifa_addr->sa_family,
			          &((struct sockaddr_in *)ife->ifa_addr)->sin_addr,
			          buf, len);
			break;
/*
		case AF_INET6:
			inet_ntop(ife->ifa_addr->sa_family,
			          &((struct sockaddr_in6 *)ife->ifa_addr)->sin6_addr,
			          buf, len);
*/
		}
	}
	freeifaddrs(ifap);
#endif
	return 0;
}

#ifdef ENABLE_IPV6
int
find_ipv6_addr(const char * ifname,
               char * dst, int n)
{
	struct ifaddrs * ifap;
	struct ifaddrs * ife;
	const struct sockaddr_in6 * addr;
	char buf[64];
	int r = 0;

	if(!dst)
		return -1;

	if(getifaddrs(&ifap)<0)
	{
		NP_UPNP_ERROR("getifaddrs: %s\n", strerror(errno));
		return -1;
	}
	for(ife = ifap; ife; ife = ife->ifa_next)
	{
		/* skip other interfaces if one was specified */
		if(ifname && (0 != strcmp(ifname, ife->ifa_name)))
			continue;
		if(ife->ifa_addr->sa_family == AF_INET6)
		{
			addr = (const struct sockaddr_in6 *)ife->ifa_addr;
			if(!IN6_IS_ADDR_LOOPBACK(&addr->sin6_addr)
			   && !IN6_IS_ADDR_LINKLOCAL(&addr->sin6_addr))
			{
				inet_ntop(ife->ifa_addr->sa_family,
				          &addr->sin6_addr,
				          buf, sizeof(buf));
				/* add brackets */
				snprintf(dst, n, "[%s]", buf);
				r = 1;
			}
		}
	}
	return r;
}
#endif

