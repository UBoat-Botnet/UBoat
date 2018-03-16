#ifndef HTTPQUERY_H
#define HTTPQUERY_H

#include "Sockets.h"

typedef struct NameValuePair
{
	const char* Name;
	const char* Value;
	NameValuePair(char* name, char* value) {
		Name = name;
		Value = value;
	}
} NameValuePair;

char to_hex(char code);
char* url_encode(char *str);
int HTTPQuery(char* host, char* path, NameValuePair** headers, NameValuePair*** responseHeaders, char** responseData, int* dataLength, bool isString = true, char* protocol = "GET", NameValuePair** postData = 0);
void FreeHTTPResponse(NameValuePair** header, char* data);
bool GetSocket(char* host, sockaddr_in* address);

#endif