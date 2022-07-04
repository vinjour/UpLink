all: server

server: server.c
	gcc -g -Wall $< -o $@ `pkg-config libwebsockets --libs --cflags` -fuse-ld=gold -Wl,--no-map-whole-files -Wl,--no-keep-memory -Wl,--no-keep-files-mapped

clean:
	rm -f server
	rm -rf server.dSYM