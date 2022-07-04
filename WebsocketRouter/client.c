#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <libwebsockets.h>

static struct lws *web_socket = NULL;

#define EXAMPLE_RX_BUFFER_BYTES (10)

static int callback_example(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	switch(reason) {
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
			lws_callback_on_writable(wsi);
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:
			// Handle incomming messages here
			break;

		case LWS_CALLBACK_CLIENT_WRITEABLE: {
			unsigned char buf[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];
			unsigned char *p = &buf[LWS_SEND_BUFFER_PRE_PADDING];
			size_t n = sprintf((char *)p, "%u", rand());
			lws_write(wsi, p, n, LWS_WRITE_TEXT);
			break;
		}

		case LWS_CALLBACK_CLOSED:
		
		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:
			web_socket = NULL;
			break;

		default:
			break;
	}

	return 0;
}

enum protocols {
	PROTOCOL_EXAMPLE = 0,
	PROTOCOL_COUNT
};

static struct lws_protocols protocols[] = {		// first protocol must always be HTTP handler
	{
		"example-protocol",	// name
		callback_example,	// callback
		0,		// per session data size
		EXAMPLE_RX_BUFFER_BYTES,		// max frame size / rx buffer
	},
	{
		NULL, NULL, 0, 0	// end of list
	}
};

int main(int argc, char **argv) {		// server url will be http://192.168.2.195:8000

	struct lws_context_creation_info info;
	memset( &info, 0, sizeof(info) );

	info.port = CONTEXT_PORT_NO_LISTEN;
	info.protocols = protocols;
	info.gid = -1;
	info.uid = -1;

	struct lws_context *context = lws_create_context(&info);	// create lws context representing this server

	if (context == NULL) {
		printf("lws init failed\n");
		return -1;
	}

	printf("Starting server...\n");

	time_t old = 0;
	
	while(1) {		// infinite loop, to end this server send SIGTERM (CTRL+C)
		struct timeval tv;
		gettimeofday( &tv, NULL );

		if( !web_socket && tv.tv_sec != old ) {	// connect if we are not connected to the server
			struct lws_client_connect_info ccinfo = {0};
			ccinfo.context = context;
			ccinfo.address = "192.168.2.195";
			ccinfo.port = 8000;
			ccinfo.path = "/";
			ccinfo.host = lws_canonical_hostname( context );
			ccinfo.origin = "origin";
			ccinfo.protocol = protocols[PROTOCOL_EXAMPLE].name;
			web_socket = lws_client_connect_via_info(&ccinfo);
		}

		if( tv.tv_sec != old ) {		// send a random number to the server every second
			lws_callback_on_writable( web_socket );	
			old = tv.tv_sec;
		}

		lws_service(context, 250);	// lws-service will process all waiting events with their callback functions every 250ms
	}

	lws_context_destroy(context);

	return 0;
}