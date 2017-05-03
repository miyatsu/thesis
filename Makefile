thesis : src/main.c src/http.c src/http.h src/cache.c src/cache.h src/log.c src/log.h src/proxy.c src/proxy.h src/config.c src/config.h src/global.h src/md5.c src/md5.h
	gcc -Wall -std=c11 -O1 -lpthread -o thesis src/main.c src/cache.c src/http.c src/log.c src/proxy.c src/config.c src/md5.c -lpthread -levent
clean :
	rm thesis
