#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>

struct sockaddr_in addr;

int success = 0;
int failed = 0;
int failed_conn = 0;

char request[] = "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\n\r\n";

void* thread_func(void *arg)
{
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if ( sockfd <= 0 )
	{
		perror("socket error.\n");
		exit(-1);
	}
	int connect_result = connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	if ( connect_result < 0 )
	{
		goto FAILED_CONN;
	}
	write(sockfd, &request, sizeof(request));
	char str[13] = {0};
	read(sockfd, &str, 13);
	if (0 == strncmp(str, "HTTP/1.1 200", 12))
	{
		goto SUCCESS;
	}
	else
	{
		goto FAILED;
	}
SUCCESS:
	success++;
	close(sockfd);
	return (void *)NULL;
FAILED:
	failed++;
	close(sockfd);
	return (void *)NULL;
FAILED_CONN:
	failed_conn++;
	close(sockfd);
	return (void *)NULL;
}

int main(int argc, char * argv[])
{
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	if ( 1!= inet_pton(AF_INET, (const char*)argv[1], &addr.sin_addr) )
	{
		printf("address error!\n");
		exit(-1);
	}
	addr.sin_port = htons(8080);

	int n = 0;
	for ( int i = 0; argv[2][i] != '\0'; ++i )
	{
		n *= 10;
		n += argv[2][i] - '0';
	}
	pthread_t * tid = malloc(sizeof(pthread_t) * n);
	for ( int i = 0; i < n; ++i )
	{
		pthread_create(&tid[i], NULL, thread_func, NULL);
	}
	sleep(1);
	for ( int i = 0; i < n; ++i )
	{
		void * ret;
		pthread_join(tid[i], &ret);

	}
	printf("\n\n");
	printf("success : %d\n", success);
	printf("failed_conn : %d\n", failed_conn);
	printf("failed : %d\n", failed);
	return 0;
}
