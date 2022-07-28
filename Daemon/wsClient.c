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

	free(out);		// free the buffer

	return n;
}


// analyses the message received from the server and acts on the requested action
int parseMsgAndValidateActions(FILE *fp) {

	json_t *msg, *action, *mac;
	char command[MAXBUF];
	
	msg =  json_loads(msgRcv, 0, NULL);
	action = json_object_get(msg, "action");
	mac = json_object_get(msg, "mac");
	
	if (strcmp(json_string_value(action), "router") == 0) {
		sprintf(command, "nft add element inet filter allow_host { %s }", json_string_value(mac));
		system(command);		// create a new element in NFTables to allow the router
		fprintf(fp, "add router : %s\n", json_string_value(mac));
	}
	
	else if (strcmp(json_string_value(action), "accept") == 0) {
		sprintf(command, "nft add element inet filter allow_host { %s }", json_string_value(mac));
		system(command);		// create a new element in NFTables to allow the client
		fprintf(fp, "add client : %s\n", json_string_value(mac));
	}

	else if (strcmp(json_string_value(action), "delete") == 0) {
		sprintf(command, "nft delete element inet filter allow_host { %s }", json_string_value(mac));
		system(command);		// delete the element in NFTables to put the client in guest zone
		fprintf(fp, "delete client : %s\n", json_string_value(mac));
	}
	
	else {
		return 0;
	}

	msgRcv[0] = '\0';
	json_decref(mac);
	json_decref(action);
	json_decref(msg);
	
	return 0;
}


// when an event occurs on a connection under a certain protocol, the webSocketCallback function executed
int webSocketCallback(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {

	FILE *fp= NULL;
	umask(0);
	fp = fopen ("WSFile.txt", "a+");

	switch (reason) {		// reason : the event that caused the callback

		case LWS_CALLBACK_CLIENT_ESTABLISHED:	// callback after connecting to the server
			lws_callback_on_writable(wsi);
			break;

		case LWS_CALLBACK_CLIENT_CONNECTION_ERROR:		// callback after an error during connection
			break;

		case LWS_CALLBACK_CLOSED:		// callback after closing the connection
			break;

		case LWS_CALLBACK_CLIENT_RECEIVE:		// callback after receiving the server data
			strcpy(msgRcv, (char*)in);
			fprintf(fp,"%s\n", msgRcv);
			parseMsgAndValidateActions(fp);
			lws_callback_on_writable(wsi);
			break;

		case LWS_CALLBACK_CLIENT_WRITEABLE:		// callback when this client can send data
			break;

		default:
			break;
	}
	fclose(fp);
	return 0;
}


struct session_data {
	int fd;
};


struct lws_protocols protocols[] = {
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


// create websocket handler and return context
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


// create client connection parameters and return ccinfo
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
