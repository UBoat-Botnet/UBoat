#include "HttpQuery.h"
#include "StringOperations.h"
#include "WebsiteChecker.h"

bool CheckIfWebsiteIsUp(char* url)
{
	char* host;
	char* path;
	bool res = URLToHostPath(url, &host, &path);
	if (!res) return false;

	res = CheckIfWebsiteIsUp(host, path);

	FreeHostPathResult(host, path);

	return res;
}

bool CheckIfWebsiteIsUp(char* host, char* path)
{
	char* data;
	NameValuePair** headers;
	int length;
	int result = HTTPQuery(host, path, NULL, &headers, &data, &length);
	FreeHTTPResponse(headers, data);

	return (result == 200);
}

bool URLToHostPath(char* url, char** host, char** path)
{
	int length1;
	char** result1 = SplitString(url, "://", &length1);

	if (length1 > 2 || strcmp(result1[0], "http")) //ensure it is not a https url
	{
		FreeSplitStringBuffer(result1, length1);
		return false;
	}

	int length2;
	char** result2 = SplitString(result1[length1 - 1], "/" , &length2, false);

	FreeSplitStringBuffer(result1, length1);
	
	int length = 0;
	int index = 0;

	for (int i = 1; i < length2; i++)
		length += strlen(result2[i]) + 1;

	if (!length) length = 1;

	char* retHost = (char*)malloc(strlen(result2[0]) + 1);
	char* retPath = (char*)malloc(length + 1);

	retHost[strlen(result2[0])] = '\0';
	retPath[length] = '\0';

	//8====D

	memcpy_s(retHost, strlen(result2[0]), result2[0], strlen(result2[0]));
	
	for (int i = 1; i < length2; i++)
	{
		retPath[index] = '/';
		index += 1;
		if(strlen(result2[i])) memcpy_s(retPath + index, length - index, result2[i], strlen(result2[i]));
		index += strlen(result2[i]);
	}
	
	if (!index)
		retPath[0] = '/';

	FreeSplitStringBuffer(result2, length2);

	*host = retHost;
	*path = retPath;

	return true;
}

void FreeHostPathResult(char* host, char* path)
{
	free(host);
	free(path);
}