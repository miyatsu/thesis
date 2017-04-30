#ifndef __HTTP_H__
#define __HTTP_H__

#include <stdint.h>


typedef struct
{
	uint16_t	len;
	char *		ptr;
}str_t;

typedef enum
{
	METHOD_UNKNOW = 0,
	METHOD_GET,
	METHOD_POST,
	METHOD_OTHER
}method_t;

typedef struct
{
	str_t	req;		/* Full request content. */
	str_t	req_line;
	str_t	header_body;
	str_t	content;

	method_t	method;
	//head_list_t	head_list;
	str_t		url;
	uint8_t		version;
}request_t;

void http_init(char * buff, int len);
void parse(void);

#endif	/* __HTTP_H__ */

