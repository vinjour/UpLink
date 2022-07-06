#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <libwebsockets.h>

static int destroy_flag = 0;
static int connection_flag = 0;

void INT_HANDLER(int signo) {
	destroy_flag = 1;
};

struct session_data {
	int fd;
};

struct pthread_routine_tool {
	struct lws_context *context;
	struct lws *wsi;
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
			char msg[128] = "";
			scanf(" %[^\n]", msg);	// scan the message on the prompt
			websocket_write_back(wsi, (char *) msg, -1);		// send the message to the server
			break;

		default:
			break;
	}

	return 0;
}

void *pthread_routine(void *tool_in) {
	struct pthread_routine_tool *tool = tool_in;

	while(!connection_flag) {		// waiting for connection with server done
		usleep(1000*20);
	}

	//*Send greeting to server*/ 
	lws_callback_on_writable(tool->wsi);	// trigger a WRITEABLE event callback
	sleep(1);
	return 0;
}

struct lws_protocols protocols[] = {		// first protocol must always be HTTP handler
	{
		"websockets",	// name
		&ws_callback,	// callback
		sizeof(struct session_data),		// per session data size
		0,		// max frame size / rx buffer
	},
	{
		NULL, NULL, 0, 0	// end of list
	}
};


int main(int argc, char **argv) {
	
	struct sigaction act;		// register the signal SIGINT handler
	act.sa_handler = INT_HANDLER;
	act.sa_flags = 0;
	sigemptyset(&act.sa_mask);
	sigaction( SIGINT, &act, 0);

	struct lws_context *context = NULL;
	struct lws_context_creation_info info;
	struct lws *wsi = NULL;

	memset(&info, 0, sizeof info);		// parameters used to create context
	info.port = CONTEXT_PORT_NO_LISTEN;
	info.iface = NULL;
	info.protocols = protocols;
	info.ssl_cert_filepath = NULL;	// ssl support
	info.ssl_private_key_filepath = NULL;	// ssl support
	info.extensions = NULL;
	info.gid = -1;
	info.uid = -1;
	info.options = 0;

	context = lws_create_context(&info);	// create a websocket handler
	printf("[Main] context created.\n");

	if (context == NULL) {
		printf("[Main] context is NULL.\n");
		return -1;
	}

	if(!wsi) {
		struct lws_client_connect_info ccinfo = {0};		// Client connection parameters
		ccinfo.context = context;
		ccinfo.address = "192.168.2.213";
		ccinfo.port = 8000;
		ccinfo.path = "/";
		ccinfo.host = lws_canonical_hostname( context );
		ccinfo.origin = "origin";
		ccinfo.protocol = protocols[0].name;
		wsi = lws_client_connect_via_info(&ccinfo);		// create a client connection
	}

	if (wsi == NULL) {
		printf("[Main] wsi create error.\n");
		return -1;
	}

	printf("[Main] wsi create success.\n");

	struct pthread_routine_tool tool;
	tool.wsi = wsi;
	tool.context = context;
	pthread_t pid;

	while(!destroy_flag) {
		lws_service(context, 1000);		// execute the event loop once, and wait for a maximum of 1000 ms
		pthread_create(&pid, NULL, pthread_routine, &tool);
		pthread_detach(pid);
	}


	printf("Exiting\n");
	lws_context_destroy(context);		// destroy the context object

	return 0;
}
