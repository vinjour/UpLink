#include <libwebsockets.h>
#include <string.h>
#include <stdio.h>

#define EXAMPLE_RX_BUFFER_BYTES (10)

static int callback_http( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len ) {
	switch (reason) {
		case LWS_CALLBACK_HTTP:
			lws_serve_http_file( wsi, "example.html", "text/html", NULL, 0 );
			break;
		default:
			break;
	}
	return 0;
}

struct payload {
	unsigned char data[LWS_SEND_BUFFER_PRE_PADDING + EXAMPLE_RX_BUFFER_BYTES + LWS_SEND_BUFFER_POST_PADDING];
	size_t len;
} received_payload;

static int callback_example( struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len ) {
	switch (reason) {
		case LWS_CALLBACK_ESTABLISHED: 	// log message that someone is connecting
			printf("connection established\n");
			break;
		case LWS_CALLBACK_RECEIVE:
			memcpy( &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], in, len );
			received_payload.len = len;
			lws_callback_on_writable_all_protocol( lws_get_context( wsi ), lws_get_protocol( wsi ) );
			break;

		case LWS_CALLBACK_SERVER_WRITEABLE:
			lws_write( wsi, &received_payload.data[LWS_SEND_BUFFER_PRE_PADDING], received_payload.len, LWS_WRITE_TEXT );
			break;

		default:
			break;
	}

	return 0;
}

enum protocols {
	PROTOCOL_HTTP = 0,
	PROTOCOL_EXAMPLE,
	PROTOCOL_COUNT
};

static struct lws_protocols protocols[] = {		// The first protocol must always be the HTTP handler
	{
		"http-only",   // name 
		callback_http, // callback
		0,             // no per session data size
		0,             // max frame size / rx buffer
	},
	{
		"example-protocol",
		callback_example,
		0,
		EXAMPLE_RX_BUFFER_BYTES,
	},
	{
		NULL, NULL, 0, 0	// end of list
	}
};

int main( int argc, char **argv ) {

	// server url will be http://localhost:8000
	 int port = 8000;
	struct lws_context *context;
	struct lws_context_creation_info context_info = {
        .port = port, .iface = NULL, .protocols = protocols, .extensions = NULL,
        .ssl_cert_filepath = NULL, .ssl_private_key_filepath = NULL, .ssl_ca_filepath = NULL,
        .gid = -1, .uid = -1, .options = 0, NULL, .ka_time = 0, .ka_probes = 0, .ka_interval = 0
	};

	context = lws_create_context(&context_info);		// create lws context representing this server

	if (context == NULL) {
        	fprintf(stderr, "lws init failed\n");
        	return -1;
	}
	
	printf("starting server...\n");

	while(1) {		// infinite loop, to end this server send SIGTERM (CTRL+C)
		lws_service( context, 100 );		// lws_service will process all waiting events with their callback functions and then wait 100 ms
	}

	lws_context_destroy (context);

	return 0;
}