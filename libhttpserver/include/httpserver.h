#ifndef __api_framework_h__
#define __api_framework_h__

typedef void (*handlerFunction)(struct Request, struct Response);

struct Header{
    char* key;
    char* value;
};
struct Request {
    char* method;
    char* url;
    char* version;
    struct Header* headers;
    int headersSize;
    char* body;
};
struct Response {};
struct Handler {
    char* url;
    handlerFunction handler;
};
struct Handlers {
    struct Handler* handlersPtr;
    int size;
};
struct ServerConfig {
    struct Handlers handlers;
    int port;
};

struct ServerConfig newServerConfig(int port);
int addHandler(struct Handlers* handlers, char* url, handlerFunction handler);
int startServer(struct ServerConfig config);

#endif
