#include "main.h"

int destroy_flag = 0;
int connection_flag = 0;

void INT_HANDLER(int signo) {
	destroy_flag = 1;
};

int websocket_write_back(struct lws *wsi_in, char *str, int str_size_in) {
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

// when an event occurs on a connection under a certain protocol, the ws_callback function executed
int ws_callback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {

	switch (reason) {		// reason : the event that caused the callback

		case LWS_CALLBACK_CLIENT_ESTABLISHED:	// callback after connecting to the server
			printf("[Main Service] Connect with server success.\n");
			connection_flag = 1;
			break;

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:		// callback after an error during connection
			printf("[Main Service] Connect with server error.\n");
			destroy_flag = 1;
			connection_flag = 0;
			break;

		case LWS_CALLBACK_CLOSED:		// callback after closing the connection
			printf("[Main Service] LWS_CALLBACK_CLOSED\n");
			destroy_flag = 1;
			connection_flag = 0;
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:		// callback after receiving the server data
			printf("[Main Service] Client received : %s\n", (char *)in);
			break;

		case LWS_CALLBACK_CLIENT_WRITEABLE:		// callback when this client can send data
			printf("[Main Service] On writeable is called\n");
			//char msg[128] = "";
			//scanf(" %[^\n]", msg);	// scan the message on the prompt
			//websocket_write_back(wsi, (char *) msg, -1);		// send the message to the server
			break;

		default:
			break;
	}

	return 0;
}
