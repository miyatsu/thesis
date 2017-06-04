#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>

#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/bufferevent.h>

#include "md5.h"
#include "http.h"
#include "cache.h"
#include "log.h"
#include "global.h"

char g_cache_dir[128] = {0};

void cache_init(char * buff, int len)
{
	for ( int i = 0; i < len; ++i )
	{
		g_cache_dir[i] = buff[i];
	}
	return ;
}

/* Watch out!! This function never been tested before, be careful when call it. */
void get_request_line_md5(unsigned char *encrypt, char *hexstr)
{
	MD5_CTX md5;
	uint8_t decrypt[16];

	MD5Init(&md5);
	MD5Update(&md5, encrypt, strlen((char *)encrypt));
	MD5Final(&md5, decrypt);
	for ( int i = 0; i < 16; ++i )
	{
		sprintf(&hexstr[i*2], "%02x", decrypt[i]);
	}
	return ;
}

void send_cache_to_client(int fd, short events, void * ptr)
{
	LOG(LOG_LEVEL_INFO, "Send cached data to client.\n");
	if ( events & EV_READ )
	{
		struct evbuffer * buff = bufferevent_get_output((struct bufferevent *)ptr);
		int n = 0;
		char str[1024] = {0};
		while ( ( n = read(fd, str, 1024) ) > 0 )
		{
			evbuffer_add(buff, str, n);
		}
		close(fd);
	}
	/* Ignore all other events. */
}

void cache_hit(struct bufferevent * bev, int fd)
{
	LOG(LOG_LEVEL_INFO, "Cache hit.\n");
	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
	event_base_once(g_base, fd, EV_READ, send_cache_to_client, bev, NULL);
	return ;
}

typedef struct
{
	struct bufferevent * bev;
	int fd;
}origin_arg_t;

void read_from_origin_cb(struct bufferevent * bev, void * ptr)
{
	LOG(LOG_LEVEL_INFO, "Receive data from origin server.\n");
	origin_arg_t * arg = ptr;
	if ( !arg )
	{
		LOG(LOG_LEVEL_FATAL, "No arg!\n");
		exit(-1);
	}
	struct evbuffer * origin = bufferevent_get_input(bev);
	struct evbuffer * client = bufferevent_get_output(arg->bev);
	size_t str_len = evbuffer_get_length(origin);
	char * str = malloc(str_len);
	evbuffer_copyout(origin, str, str_len);
	LOG(LOG_LEVEL_INFO, "Write data into cache file.\n");
	evbuffer_add_buffer(client, origin);
	LOG(LOG_LEVEL_INFO, "Send uncached data to client.\n");
	for ( int i = 0, n = 0; i < str_len; i += n )
	{
		n = write(arg->fd, str, str_len);
	}
	free(str);
}


void event_of_origin_cb(struct bufferevent * bev, short events, void * ptr)
{
	if ( events & ( BEV_EVENT_EOF | BEV_EVENT_ERROR) )
	{
		if ( events | BEV_EVENT_EOF )
		{
			bufferevent_free(bev);
		}
		origin_arg_t * arg = ptr;
		close(arg->fd);
		free(arg);
	}
}

void cache_miss(struct bufferevent * bev, int fd)
{
	LOG(LOG_LEVEL_INFO, "Cache miss.\n");
	struct bufferevent * bev_out = bufferevent_socket_new(g_base, -1, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS);
	if ( !bev_out )
	{
		LOG(LOG_LEVEL_FATAL, "Create bufferevent failed.");
		bufferevent_free(bev_out);
		exit(-1);
	}
	if ( bufferevent_socket_connect(bev_out, (struct sockaddr *)&g_remote_addr, sizeof(struct sockaddr_in)) < 0 )
	{
		LOG(LOG_LEVEL_FATAL, "Can NOT connect to remote srever.");
		exit(-1);
	}

	/* At this point, we are managed to recive data from origin. */

	origin_arg_t * arg = malloc(sizeof(origin_arg_t));
	if ( !arg )
	{
		LOG(LOG_LEVEL_FATAL, "No memory.");
		exit(-1);
	}
	arg->bev = bev;
	arg->fd = fd;

	bufferevent_setcb(bev_out, read_from_origin_cb, NULL, event_of_origin_cb, arg);
	bufferevent_enable(bev_out, EV_READ | EV_WRITE);

	struct evbuffer * src = bufferevent_get_input(bev);
	struct evbuffer * dst = bufferevent_get_output(bev_out);
	evbuffer_add_buffer(dst, src);
}

#ifdef DEBUG

//This example is copy from : http://www.cnblogs.com/alexthecoder/p/4990835.html

#include "md5.h"
void md5_use_example()
{
	MD5_CTX md5;
	MD5Init(&md5);
	int i;
	char encrypt[100] = {"admin"};
	unsigned char decrypt[16];
	MD5Update(&md5, encrypt, strlen(encrypt));
	MD5Final(&md5, decrypt);

	unsigned char hexstr[32];
	for ( int i = 0; i < 16; ++i )
	{
		sprintf(&hexstr[i*2], "%02x", decrypt[i]);
	}

	printf("%s\n", hexstr);
}

#endif

