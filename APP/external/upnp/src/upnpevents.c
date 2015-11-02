/* $Id: upnpevents.c,v 1.17 2011/06/27 11:24:00 nanard Exp $ */
/* MiniUPnP project
 * http://miniupnp.free.fr/ or http://miniupnp.tuxfamily.org/
 * (c) 2008-2011 Thomas Bernard
 * This software is subject to the conditions detailed
 * in the LICENCE file provided within the distribution */
 
   /**********************问题单修改记录******************************************
    日期              修改人         问题单号           修改内容
  
 2012.03.14        z00203875     2031401440    修改打LOG方式
 2012.03.26        z00203875     2032603705    LOG打到SDT中    
 2012.08.29        z00203875     2082304944    UPnP认证测试
******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include "config.h"
#include "upnpevents.h"
#include "miniupnpdpath.h"
#include "upnpglobalvars.h"
#include "upnpdescgen.h"

#ifdef ENABLE_EVENTS
/*enum subscriber_service_enum {
 EWanCFG = 1,
 EWanIPC,
 EL3F
};*/

/* stuctures definitions */
struct subscriber {
	LIST_ENTRY(subscriber) entries;
	struct upnp_event_notify * notify;
	time_t timeout;
	uint32_t seq;
	enum subscriber_service_enum service;
	char uuid[42];
	char callback[];
};

struct upnp_event_notify {
	LIST_ENTRY(upnp_event_notify) entries;
    int s;  /* socket */
    enum { ECreated=1,
	       EConnecting,
	       ESending,
	       EWaitingForResponse,
	       EFinished,
	       EError } state;
    struct subscriber * sub;
    char * buffer;
    int buffersize;
	int tosend;
    int sent;
	const char * path;
#ifdef ENABLE_IPV6
	int ipv6;
	char addrstr[48];
#else
	char addrstr[16];
#endif
	char portstr[8];
};

/* prototypes */
static void
upnp_event_create_notify(struct subscriber * sub);

/* Subscriber list */
LIST_HEAD(listhead, subscriber) subscriberlist = { NULL };

/* notify list */
LIST_HEAD(listheadnotif, upnp_event_notify) notifylist = { NULL };

/* create a new subscriber */
static struct subscriber *
newSubscriber(const char * eventurl, const char * callback, int callbacklen)
{
	struct subscriber * tmp;
	if(!eventurl || !callback || !callbacklen)
		return NULL;
	tmp = calloc(1, sizeof(struct subscriber)+callbacklen+1);
	if(!tmp)
		return NULL;
	if(strcmp(eventurl, WANCFG_EVENTURL)==0)
		tmp->service = EWanCFG;
	else if(strcmp(eventurl, WANIPC_EVENTURL)==0)
		tmp->service = EWanIPC;
#ifdef ENABLE_L3F_SERVICE
	else if(strcmp(eventurl, L3F_EVENTURL)==0)
		tmp->service = EL3F;
#endif
	else {
		free(tmp);
		return NULL;
	}
	memcpy(tmp->callback, callback, callbacklen);
	tmp->callback[callbacklen] = '\0';
	/* make a dummy uuid */
	/* TODO: improve that */
	strncpy(tmp->uuid, uuidvalue, sizeof(tmp->uuid));
	tmp->uuid[sizeof(tmp->uuid)-1] = '\0';
	snprintf(tmp->uuid+37, 5, "%04lx", random() & 0xffff);
	return tmp;
}

/* creates a new subscriber and adds it to the subscriber list
 * also initiate 1st notify
 * TODO : add a check on the number of subscriber in order to
 * prevent memory overflow... */
const char *
upnpevents_addSubscriber(const char * eventurl,
                         const char * callback, int callbacklen,
                         int timeout)
{
    NP_UPNP_INFO("enter upnpevents_addSubscriber\n");
    NP_UPNP_DEBUG("eventurl is %s\n", eventurl);
    NP_UPNP_DEBUG("callback is %s\n", callback);
    NP_UPNP_DEBUG("callbacklen is %d\n", callbacklen);
    NP_UPNP_DEBUG("timeout is %d\n", timeout);

	struct subscriber * tmp;
	/*static char uuid[42];*/
	/* "uuid:00000000-0000-0000-0000-000000000000"; 5+36+1=42bytes */
	NP_UPNP_DEBUG("addSubscriber(%s, %.*s, %d)", eventurl, callbacklen, callback, timeout);
	/*strncpy(uuid, uuidvalue, sizeof(uuid));
	uuid[sizeof(uuid)-1] = '\0';*/
	tmp = newSubscriber(eventurl, callback, callbacklen);
	if(!tmp)
		return NULL;
	if(timeout)
		tmp->timeout = time(NULL) + timeout;
	LIST_INSERT_HEAD(&subscriberlist, tmp, entries);
	upnp_event_create_notify(tmp);
	return tmp->uuid;
}

/* renew a subscription (update the timeout) */
int
renewSubscription(const char * sid, int sidlen, int timeout)
{
	struct subscriber * sub;
	for(sub = subscriberlist.lh_first; sub != NULL; sub = sub->entries.le_next) {
		if(memcmp(sid, sub->uuid, 41) == 0) {
			sub->timeout = (timeout ? time(NULL) + timeout : 0);
			return 0;
		}
	}
	return -1;
}

int
upnpevents_removeSubscriber(const char * sid, int sidlen)
{
    NP_UPNP_INFO("enter upnpevents_removeSubscriber\n");
	struct subscriber * sub;
	if(!sid)
    {
        NP_UPNP_DEBUG("sid is null, return -1\n");
		return -1;
    }
    if(subscriberlist.lh_first == NULL)
    {
        NP_UPNP_DEBUG("subscriberlist.lh_first == NULL\n");
    }
	for(sub = subscriberlist.lh_first; sub != NULL; sub = sub->entries.le_next)
    {
        NP_UPNP_DEBUG("sub != NULL\n");
		if(memcmp(sid, sub->uuid, 41) == 0)
        {
            NP_UPNP_DEBUG("memcmp(sid, sub->uuid, 41) == 0\n");
			if(sub->notify)
            {
				sub->notify->sub = NULL;
			}
			LIST_REMOVE(sub, entries);
			free(sub);
			return 0;
		}
	}
    NP_UPNP_INFO("leave upnpevents_removeSubscriber\n");
    NP_UPNP_INFO("return -1\n");
	return -1;
}

/* notifies all subscriber of a number of port mapping change
 * or external ip address change */
void
upnp_event_var_change_notify(enum subscriber_service_enum service)
{
	struct subscriber * sub;
	for(sub = subscriberlist.lh_first; sub != NULL; sub = sub->entries.le_next) {
		if(sub->service == service && sub->notify == NULL)
			upnp_event_create_notify(sub);
	}
}

/* create and add the notify object to the list */
static void
upnp_event_create_notify(struct subscriber * sub)
{
	struct upnp_event_notify * obj;
	int flags;
	obj = calloc(1, sizeof(struct upnp_event_notify));
	if(!obj) {
		NP_UPNP_ERROR("%s: calloc(): %s\n", "upnp_event_create_notify", strerror(errno));
		return;
	}
	obj->sub = sub;
	obj->state = ECreated;
#ifdef ENABLE_IPV6
	obj->s = socket((obj->sub->callback[7] == '[') ? PF_INET6 : PF_INET,
	                SOCK_STREAM, 0);
#else
	obj->s = socket(PF_INET, SOCK_STREAM, 0);
#endif
	if(obj->s<0) {
		NP_UPNP_ERROR("%s: socket(): %s\n", "upnp_event_create_notify", strerror(errno));
		goto error;
	}
	if((flags = fcntl(obj->s, F_GETFL, 0)) < 0) {
		NP_UPNP_ERROR("%s: fcntl(..F_GETFL..): %s\n",
		       "upnp_event_create_notify", strerror(errno));
		goto error;
	}
	if(fcntl(obj->s, F_SETFL, flags | O_NONBLOCK) < 0)
    {
        NP_UPNP_DEBUG("set non block socket failed\n");
		goto error;
	}
	if(sub)
		sub->notify = obj;
	LIST_INSERT_HEAD(&notifylist, obj, entries);
	return;
error:
	if(obj->s >= 0)
		close(obj->s);
	free(obj);
}

static void
upnp_event_notify_connect(struct upnp_event_notify * obj)
{
	int i;
	const char * p;
	unsigned short port;
#ifdef ENABLE_IPV6
	struct sockaddr_storage addr;
#else
	struct sockaddr_in addr;
#endif
	if(!obj)
		return;
	memset(&addr, 0, sizeof(addr));
	i = 0;
	if(obj->sub == NULL) {
		obj->state = EError;
		return;
	}
	p = obj->sub->callback;
	p += 7;	/* http:// */
#ifdef ENABLE_IPV6
	if(*p == '[') {	/* ip v6 */
		p++;
		obj->ipv6 = 1;
		while(*p != ']' && i < (sizeof(obj->addrstr)-1))
			obj->addrstr[i++] = *(p++);
		if(*p == ']')
			p++;
	} else {
#endif
		while(*p != '/' && *p != ':' && i < (int)(sizeof(obj->addrstr)-1))
			obj->addrstr[i++] = *(p++);
#ifdef ENABLE_IPV6
	}
#endif
	obj->addrstr[i] = '\0';
	if(*p == ':') {
		obj->portstr[0] = *p;
		i = 1;
		p++;
		port = (unsigned short)atoi(p);
		while(*p != '/') {
			if(i<7) obj->portstr[i++] = *p;
			p++;
		}
		obj->portstr[i] = 0;
	} else {
		port = 80;
		obj->portstr[0] = '\0';
	}
	obj->path = p;
#ifdef ENABLE_IPV6
	if(obj->ipv6) {
		struct sockaddr_in6 * sa = (struct sockaddr_in6 *)&addr;
		sa->sin6_family = AF_INET6;
		inet_pton(AF_INET6, obj->addrstr, &(sa->sin6_addr));
		sa->sin6_port = htons(port);
	} else {
		struct sockaddr_in * sa = (struct sockaddr_in *)&addr;
		sa->sin_family = AF_INET;
		inet_pton(AF_INET, obj->addrstr, &(sa->sin_addr));
		sa->sin_port = htons(port);
	}
#else
	addr.sin_family = AF_INET;
	inet_aton(obj->addrstr, &addr.sin_addr);
	addr.sin_port = htons(port);
#endif
	NP_UPNP_DEBUG("%s: '%s' %hu '%s'\n", "upnp_event_notify_connect",
	       obj->addrstr, port, obj->path);
	obj->state = EConnecting;
	if(connect(obj->s, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
		if(errno != EINPROGRESS && errno != EWOULDBLOCK) {
			NP_UPNP_ERROR("%s: connect(): %s\n", "upnp_event_notify_connect", strerror(errno));
			obj->state = EError;
		}
	}
}

static void upnp_event_prepare(struct upnp_event_notify * obj)
{
	static const char notifymsg[] = 
		"NOTIFY %s HTTP/1.1\r\n"
		"Host: %s%s\r\n"
		"Content-Type: text/xml\r\n"
		"Content-Length: %d\r\n"
		"NT: upnp:event\r\n"
		"NTS: upnp:propchange\r\n"
		"SID: %s\r\n"
		"SEQ: %u\r\n"
		"Connection: close\r\n"
		"Cache-Control: no-cache\r\n"
		"\r\n"
		"%.*s\r\n";
	char * xml;
	int l;
	if(obj->sub == NULL) {
		obj->state = EError;
		return;
	}
	switch(obj->sub->service) {
	case EWanCFG:
		xml = getVarsWANCfg(&l);
		break;
	case EWanIPC:
		xml = getVarsWANIPCn(&l);
		break;
#ifdef ENABLE_L3F_SERVICE
	case EL3F:
		xml = getVarsL3F(&l);
		break;
#endif
#ifdef ENABLE_6FC_SERVICE
	case E6FC:
		xml = getVars6FC(&l);
		break;
#endif
#ifdef ENABLE_DP_SERVICE
	case EDP:
		xml = getVarsDP(&l);
		break;
#endif
	default:
		xml = NULL;
		l = 0;
	}
	//obj->buffersize = 1024;
	obj->buffersize = 4096;
	obj->buffer = malloc(obj->buffersize);
	/*if(!obj->buffer) {
	}*/
	obj->tosend = snprintf(obj->buffer, obj->buffersize, notifymsg,
	                       obj->path, obj->addrstr, obj->portstr, l+2,
	                       obj->sub->uuid, obj->sub->seq,
	                       l, xml);
	if(xml) {
		free(xml);
		xml = NULL;
	}
	obj->state = ESending;
}

static void upnp_event_send(struct upnp_event_notify * obj)
{
	int i;
	NP_UPNP_DEBUG("%s: sending event notify message to %s:%s\n",
	       "upnp_event_send", obj->addrstr, obj->portstr);
	NP_UPNP_DEBUG("%s: msg: %s\n",
	       "upnp_event_send", obj->buffer + obj->sent);

    NP_UPNP_DEBUG("upnp_event_send, send string is:\n");
    NP_UPNP_DEBUG("%s\n\n", obj->buffer);

	//i = send(obj->s, obj->buffer + obj->sent, obj->tosend - obj->sent, 0);
	while(EAGAIN == (i = send(obj->s, obj->buffer + obj->sent, obj->tosend - obj->sent, 0)));
	if(i<0)
    {
        NP_UPNP_DEBUG("upnp_event_send, send returns < 0\n");
        NP_UPNP_DEBUG("error message is: %s\n", strerror(errno));
		NP_UPNP_ERROR("%s: send(): %m", "upnp_event_send");
		obj->state = EError;
		return;
	}
	else if(i != (obj->tosend - obj->sent))
		NP_UPNP_DEBUG("%s: %d bytes send out of %d", "upnp_event_send", i, obj->tosend - obj->sent);
	obj->sent += i;
	if(obj->sent == obj->tosend)
		obj->state = EWaitingForResponse;
}

static void upnp_event_recv(struct upnp_event_notify * obj)
{
	int n;
	n = recv(obj->s, obj->buffer, obj->buffersize, 0);
	if(n<0) {
		NP_UPNP_ERROR("%s: recv(): %s\n", "upnp_event_recv", strerror(errno));
		obj->state = EError;
		return;
	}
	NP_UPNP_DEBUG("%s: (%dbytes) %.*s\n", "upnp_event_recv",
	       n, n, obj->buffer);
	obj->state = EFinished;
	if(obj->sub)
		obj->sub->seq++;
}

static void
upnp_event_process_notify(struct upnp_event_notify * obj)
{
	switch(obj->state) {
	case EConnecting:
		/* now connected or failed to connect */
        NP_UPNP_DEBUG("------- upnp_event_process_notify EConnecting switch ----------\n");
		upnp_event_prepare(obj);
		upnp_event_send(obj);
		break;
	case ESending:
        NP_UPNP_DEBUG("------- upnp_event_process_notify ESending switch ----------\n");
		upnp_event_send(obj);
		break;
	case EWaitingForResponse:
        NP_UPNP_DEBUG("------- upnp_event_process_notify EWaitingForResponse switch ----------\n");
		upnp_event_recv(obj);
		break;
	case EFinished:
        NP_UPNP_DEBUG("------- upnp_event_process_notify EFinished switch ----------\n");
		close(obj->s);
		obj->s = -1;
		break;
	default:
		NP_UPNP_ERROR("upnp_event_process_notify: unknown state\n");
	}
}

void upnpevents_selectfds(fd_set *readset, fd_set *writeset, int * max_fd)
{
	struct upnp_event_notify * obj;
	for(obj = notifylist.lh_first; obj != NULL; obj = obj->entries.le_next) {
		NP_UPNP_DEBUG("upnpevents_selectfds: %p %d %d\n",
		       obj, obj->state, obj->s);
		if(obj->s >= 0) {
			switch(obj->state) {
			case ECreated:
				upnp_event_notify_connect(obj);
				if(obj->state != EConnecting)
					break;
			case EConnecting:
			case ESending:
				FD_SET(obj->s, writeset);
				if(obj->s > *max_fd)
					*max_fd = obj->s;
				break;
			case EWaitingForResponse:
				FD_SET(obj->s, readset);
				if(obj->s > *max_fd)
					*max_fd = obj->s;
				break;
			default:
				;
			}
		}
	}
}

void upnpevents_processfds(fd_set *readset, fd_set *writeset)
{
	struct upnp_event_notify * obj;
	struct upnp_event_notify * next;
	struct subscriber * sub;
	struct subscriber * subnext;
	time_t curtime;
	for(obj = notifylist.lh_first; obj != NULL; obj = obj->entries.le_next) {
		NP_UPNP_DEBUG("%s: %p %d %d %d %d\n",
		       "upnpevents_processfds", obj, obj->state, obj->s,
		       FD_ISSET(obj->s, readset), FD_ISSET(obj->s, writeset));
		if(obj->s >= 0) {
			if(FD_ISSET(obj->s, readset) || FD_ISSET(obj->s, writeset))
				upnp_event_process_notify(obj);
		}
	}
	obj = notifylist.lh_first;
	while(obj != NULL) {
		next = obj->entries.le_next;
		if(obj->state == EError || obj->state == EFinished) {
			if(obj->s >= 0) {
				close(obj->s);
			}
			if(obj->sub)
				obj->sub->notify = NULL;
			/* remove also the subscriber from the list if there was an error */
			if(obj->state == EError && obj->sub) {
				LIST_REMOVE(obj->sub, entries);
				free(obj->sub);
			}
			if(obj->buffer) {
				free(obj->buffer);
			}
			LIST_REMOVE(obj, entries);
			free(obj);
		}
		obj = next;
	}
	/* remove timeouted subscribers */
    /* BEGIN 2082304944 zhoujianchun 00203875 2012.8.29 added */
    curtime = time(NULL);
	for(sub = subscriberlist.lh_first; sub != NULL; ) {
		subnext = sub->entries.le_next;
		if(sub->timeout && curtime > sub->timeout && sub->notify == NULL) {
			LIST_REMOVE(sub, entries);
			free(sub);
		}
		sub = subnext;
	}
    /* END   2082304944 zhoujianchun 00203875 2012.8.29 added */}

#ifdef USE_MINIUPNPDCTL
void write_events_details(int s) {
	int n;
	char buff[80];
	struct upnp_event_notify * obj;
	struct subscriber * sub;
	write(s, "Events details :\n", 17);
	for(obj = notifylist.lh_first; obj != NULL; obj = obj->entries.le_next) {
		n = snprintf(buff, sizeof(buff), " %p sub=%p state=%d s=%d\n",
		             obj, obj->sub, obj->state, obj->s);
		write(s, buff, n);
	}
	write(s, "Subscribers :\n", 14);
	for(sub = subscriberlist.lh_first; sub != NULL; sub = sub->entries.le_next) {
		n = snprintf(buff, sizeof(buff), " %p timeout=%d seq=%u service=%d\n",
		             sub, (int)sub->timeout, sub->seq, sub->service);
		write(s, buff, n);
		n = snprintf(buff, sizeof(buff), "   notify=%p %s\n",
		             sub->notify, sub->uuid);
		write(s, buff, n);
		n = snprintf(buff, sizeof(buff), "   %s\n",
		             sub->callback);
		write(s, buff, n);
	}
}
#endif

void SendUpnpConenctSuccessNotify(void)
{
    NP_UPNP_INFO("enter SendUpnpConenctSuccessNotify\n");
    struct upnp_event_notify *obj;
    int flags = 1;
    upnp_event_var_change_notify(EWanIPC);
    if(notifylist.lh_first == NULL)
    {
        NP_UPNP_DEBUG("notifylist.lh_first is NULL\n");
    }
    for(obj = notifylist.lh_first; obj != NULL; obj = obj->entries.le_next)
    {
        NP_UPNP_DEBUG("notifylist for loop\n");
        /*if(fcntl(obj->s, F_SETFL, flags | O_NONBLOCK) < 0)
        {
            printf("#### fcntl error ####\n");
        }*/
        upnp_event_notify_connect(obj);
        upnp_event_process_notify(obj);
    }
    NP_UPNP_INFO("leave SendUpnpConenctSuccessNotify\n");
}

#endif

