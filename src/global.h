#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <netinet/in.h>

#include <event2/event.h>
#include <event2/listener.h>

extern int g_log_fd;
extern struct sockaddr_in g_addr;

extern struct event_base * g_base;
extern struct evconnlistener * g_listener;

#endif

