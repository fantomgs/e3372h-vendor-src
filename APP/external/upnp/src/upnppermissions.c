/* $Id: upnppermissions.c,v 1.14 2009/12/22 17:21:43 nanard Exp $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2006 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */
 
   /**********************问题单修改记录******************************************
    日期              修改人         问题单号           修改内容
  
 2012.03.14        z00203875     2031401440    修改打LOG方式    
 2012.04.27        z00203875     2041707023    Add LAN IP Restriction
******************************************************************************/

#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <unistd.h>
#include "config.h"
#include "upnppermissions.h"

int
read_permission_line(struct upnpperm * perm,
                     char * p)
{
	char * q;
	int n_bits;

	/* first token: (allow|deny) */
	while(isspace(*p))
		p++;
	if(0 == memcmp(p, "allow", 5))
	{
		perm->type = UPNPPERM_ALLOW;
		p += 5;
	}
	else if(0 == memcmp(p, "deny", 4))
	{
		perm->type = UPNPPERM_DENY;
		p += 4;
	}
	else
	{
		return -1;
	}

	/* second token: eport or eport_min-eport_max */
	while(isspace(*p))
		p++;
	if(!isdigit(*p))
		return -1;
	for(q = p; isdigit(*q); q++);
	if(*q=='-')
	{
		*q = '\0';
		perm->eport_min = (u_short)atoi(p);
		q++;
		p = q;
		while(isdigit(*q))
			q++;
		*q = '\0';
		perm->eport_max = (u_short)atoi(p);
	}
	else
	{
		*q = '\0';
		perm->eport_min = perm->eport_max = (u_short)atoi(p);
	}
	p = q + 1;
	while(isspace(*p))
		p++;

	/* third token:  ip/mask */
	if(!isdigit(*p))
		return -1;
	for(q = p; isdigit(*q) || (*q == '.'); q++);
	if(*q=='/')
	{
		*q = '\0';
		if(!inet_aton(p, &perm->address))
			return -1;
		q++;
		p = q;
		while(isdigit(*q))
			q++;
		*q = '\0';
		n_bits = atoi(p);
		perm->mask.s_addr = htonl(n_bits ? (0xffffffff << (32 - n_bits)) : 0);
	}
	else
	{
		*q = '\0';
		if(!inet_aton(p, &perm->address))
			return -1;
		perm->mask.s_addr = 0xffffffff;
	}
	p = q + 1;

	/* fourth token: iport or iport_min-iport_max */
	while(isspace(*p))
		p++;
	if(!isdigit(*p))
		return -1;
	for(q = p; isdigit(*q); q++);
	if(*q=='-')
	{
		*q = '\0';
		perm->iport_min = (u_short)atoi(p);
		q++;
		p = q;
		while(isdigit(*q))
			q++;
		*q = '\0';
		perm->iport_max = (u_short)atoi(p);
	}
	else
	{
		*q = '\0';
		perm->iport_min = perm->iport_max = (u_short)atoi(p);
	}
#ifdef DEBUG
	printf("perm rule added : %s %hu-%hu %08x/%08x %hu-%hu\n",
	       (perm->type==UPNPPERM_ALLOW)?"allow":"deny",
	       perm->eport_min, perm->eport_max, ntohl(perm->address.s_addr),
	       ntohl(perm->mask.s_addr), perm->iport_min, perm->iport_max);
#endif
	return 0;
}

#ifdef USE_MINIUPNPDCTL
void
write_permlist(int fd, const struct upnpperm * permary,
               int nperms)
{
	int l;
	const struct upnpperm * perm;
	int i;
	char buf[128];
	write(fd, "Permissions :\n", 14);
	for(i = 0; i<nperms; i++)
	{
		perm = permary + i;
		l = snprintf(buf, sizeof(buf), "%02d %s %hu-%hu %08x/%08x %hu-%hu\n",
	       i,
    	   (perm->type==UPNPPERM_ALLOW)?"allow":"deny",
	       perm->eport_min, perm->eport_max, ntohl(perm->address.s_addr),
	       ntohl(perm->mask.s_addr), perm->iport_min, perm->iport_max);
		if(l<0)
			return;
		write(fd, buf, l);
	}
}
#endif

/* match_permission()
 * returns: 1 if eport, address, iport matches the permission rule
 *          0 if no match */
static int
match_permission(const struct upnpperm * perm,
                 u_short eport, struct in_addr address, u_short iport)
{
	if( (eport < perm->eport_min) || (perm->eport_max < eport))
		return 0;
	if( (iport < perm->iport_min) || (perm->iport_max < iport))
		return 0;
	if( (address.s_addr & perm->mask.s_addr)
	   != (perm->address.s_addr & perm->mask.s_addr) )
		return 0;
	return 1;
}

int
check_upnp_rule_against_permissions(const struct upnpperm * permary,
                                    int n_perms,
                                    u_short eport, struct in_addr address,
                                    u_short iport)
{
	int i;
	for(i=0; i<n_perms; i++)
	{
		if(match_permission(permary + i, eport, address, iport))
		{
            NP_UPNP_DEBUG("UPnP permission rule %d matched : port mapping %s\n",
			       i, (permary[i].type == UPNPPERM_ALLOW)?"accepted":"rejected");
			return (permary[i].type == UPNPPERM_ALLOW);
		}
	}
    /*BEGIN 2041707023 zhoujianchun 00203875 2012.4.27 modified */
    NP_UPNP_DEBUG("no permission rule matched : reject by default (n_perms=%d)\n", n_perms);
    return 0;	/* Default : reject, in case of port larger than 65535 or smaller than 0 */
    /*END   2041707023 zhoujianchun 00203875 2012.4.27 modified */
}

