ktrix : main.c http.c http.h cache.c cache.h log.c log.h proxy.c proxy.h config.c config.h global.h md5.c md5.h
	gcc -Wall -std=c11 -O1 -lpthread -o ktrix main.c cache.c http.c log.c proxy.c config.c md5.c -lpthread -levent
clean :
	rm ktrix
