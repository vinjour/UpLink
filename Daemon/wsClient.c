#include "main.h"


int writeToServer(struct lws *wsi_in, char *str, int str_size_in) {
	if (str == NULL || wsi_in == NULL) {
 		return -1;
	}
	
	int n;
	int len;
	unsigned char *out = NULL;

	if (str_size_in < 1) {
		len = strlen(str);
	} else {
		len = str_size_in;
	}

 	out = (unsigned char *)malloc(sizeof(unsigned char)*(LWS_SEND_BUFFER_PRE_PADDING + len + LWS_SEND_BUFFER_POST_PADDING));
	memcpy (out + LWS_SEND_BUFFER_PRE_PADDING, str, len );		// setup the buffer
	unsigned char *p = &out[LWS_SEND_BUFFER_PRE_PADDING];
	n = lws_write(wsi_in, p, len, LWS_WRITE_TEXT);		// write out

	printf("[websocket_write_back] %s\n", str);
	free(out);		// free the buffer

	return n;
}

// when an event occurs on a connection under a certain protocol, the webSocketCallback function executed
int webSocketCallback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {

	int destroy_flag = 0, connection_flag = 0;

	switch (reason) {		// reason : the event that caused the callback

		case LWS_CALLBACK_CLIENT_ESTABLISHED:	// callback after connecting to the server
			printf("[Main Service] Connect with server success.\n");
			connection_flag = 1;
			break;

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:		// callback after an error during connection
			printf("[Main Service] Connect with server error.\n");
			connection_flag = 0;
			break;

		case LWS_CALLBACK_CLOSED:		// callback after closing the connection
			printf("[Main Service] LWS_CALLBACK_CLOSED\n");
			destroy_flag = 1;
			connection_flag = 0;
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:		// callback after receiving the server data
			printf("[Main Service] Client received : %s\n", (char *)in);
			connection_flag = 1;
			break;

		case LWS_CALLBACK_CLIENT_WRITEABLE:		// callback when this client can send data
			printf("[Main Service] On writeable is called\n");
			connection_flag = 1;
			break;

		default:
			connection_flag = 1;
			break;
	}

	return 0;
}


struct session_data {
	int fd;
};


struct lws_protocols protocols[] = {		// first protocol must always be HTTP handler
	{
		"websockets",	// name
		&webSocketCallback,		// callback
		sizeof(struct session_data),		// per session data size
		0,		// max frame size / rx buffer
	},
	{
		NULL, NULL, 0, 0	// end of list
	}
};


struct lws_context *createContext(FILE *fp) {

	struct lws_context_creation_info info;
	memset(&info, 0, sizeof(info));		// parameters used to create context
	info.port = CONTEXT_PORT_NO_LISTEN;
	info.iface = NULL;
	info.protocols = protocols;
	info.ssl_cert_filepath = NULL;	// ssl support
	info.ssl_private_key_filepath = NULL;	// ssl support
	info.extensions = NULL;
	info.gid = -1;
	info.uid = -1;
	info.options = 0;

	struct lws_context *context = lws_create_context(&info);		// create a websocket handler
	if (context == NULL) {
		fprintf(fp, "[Main] context is NULL.\n");
	}

	return context;
}

struct lws_client_connect_info createInfoForWSI(struct lws_context *context) {

	struct lws_client_connect_info ccinfo = {0};		// client connection parameters
	ccinfo.context = context;
	ccinfo.address = IPADDRESS;
	ccinfo.port = PORT;
	ccinfo.path = "/";
	ccinfo.host = lws_canonical_hostname(context);
	ccinfo.origin = "origin";
	ccinfo.protocol = protocols[0].name;

	return ccinfo;
}
