#include "request.h"
#include "../include/httpserver.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Headers {
    struct Header* headers;
    int size;
};

struct Headers parseHeaders(char* headerStr) {
    int headerAmount = 0;

    char* headerStrNext = headerStr;
    while(1) {
        char* result = strchr(headerStrNext, '\n');
        if (result != NULL) {
            headerAmount += 1;
            headerStrNext = result;
        } else {
            break;
        }
    }

    struct Headers headers = { malloc(headerAmount * sizeof(struct Header)), headerAmount };

    

    return headers;
}

struct Request* parseRequest(char* requestStr) {
    struct Request request;

    // get the method and remove it from the requestStr
    int index = strcspn(requestStr, " ");
    if (index == strlen(requestStr)) {
        return NULL;
    }
    char* method = malloc(index + 1);
    memcpy(method, requestStr, index);
    method[index] = 0x0;
    printf("method: %s\n", method);
    requestStr += index + 1;
    request.method = method;

    // get the url and remove it from the requestStr
    index = strcspn(requestStr, " ");
    char* url = malloc(index + 1);
    memcpy(url, requestStr, index);
    url[index] = 0x0;
    printf("url: %s\n", url);
    requestStr += index + 1;
    request.url = url;

    // get the version and remove it from the requestStr
    index = strcspn(requestStr, "\n");
    char* version = malloc(index + 1);
    memcpy(version, requestStr, index);
    version[index] = 0x0;
    printf("version: %s\n", version);
    requestStr += index + 1;
    request.version = version;

    // get headers and remove them from the requestStr
    char* bodyStart = strstr(requestStr, "\r\n\r\n");
    index = bodyStart - requestStr + 3;
    char* headerStr = malloc(index + 1);
    memcpy(headerStr, requestStr, index);
    headerStr[index] = 0x0;
    printf("headers: %s\n", headerStr);
    requestStr += index + 1;
    struct Headers headers = parseHeaders(headerStr);
    request.headers = headers.headers;
    request.headersSize = headers.size;

    // set the rest to the body
    char* body = malloc(strlen(requestStr));
    body[strlen(requestStr)] = 0x0;
    memcpy(body, requestStr, strlen(requestStr));
    printf("body: %s\nbodylen: %lu\n", body, strlen(body));
    request.body = body;

    struct Request* requestPtr = malloc(sizeof(request));
    *requestPtr = request;

    return requestPtr;
}
