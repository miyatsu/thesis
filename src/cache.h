#ifndef __CACHE_H__
#define __CACHE_H__

#include <event2/bufferevent.h>

void cache_init(char * buff, int len);
void get_request_line_md5(unsigned char *encrpty, char *);
void cache_hit(struct bufferevent * bev, int fd);
void cache_miss(struct bufferevent * bev, int fd);

#endif
