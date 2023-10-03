#include "../include/httpserver.h"
#include "request.h"
#include <asm-generic/socket.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_MESSAGE_SIZE 4096

struct Args {
    int connection_file_descriptor;
    struct Handlers handlers;
};

struct ServerConfig newServerConfig(int port) {
    struct Handlers handlers = { malloc(0), 0 };
    struct ServerConfig config = { handlers, port };
    return config;
}

int addHandler(struct Handlers* handlers, char* url, handlerFunction handler) {
    // make space for new handlers function
    handlers->handlersPtr = realloc(handlers->handlersPtr, (handlers->size + 1) * sizeof(struct Handler*));
    if(handlers->handlersPtr == NULL) {
        return -1;
    }
    // put url and handler into the new space
    handlers->handlersPtr[handlers->size].url = url;
    handlers->handlersPtr[handlers->size].handler = handler;
    handlers->size += 1;
    return 0;
}

void* handler(void* arg) {
    printf("recieved connection");
    struct Args* args = (struct Args*)arg;
    
    char buffer[MAX_MESSAGE_SIZE] = {};
    char* requestStr = malloc(0);
    int requestSize = 0;
    
    for(int readCode;;) {
        readCode = read(args->connection_file_descriptor, buffer, sizeof(buffer));
        printf("%i\n", readCode);

        // if readCode is -1 then there is some sort of error
        if(readCode == -1) {
            close(args->connection_file_descriptor);
            free(args);
            return NULL;
        }
        
        // if readCode is less that the size of the buffer, than that means we are done reading
        if(readCode < sizeof(buffer)) {
            requestStr = realloc(requestStr, requestSize + readCode);
            memcpy(requestStr + requestSize, buffer, readCode);
            requestSize += readCode;
            break;
        }
        
        // append the buffer to the request string
        requestStr = realloc(requestStr, requestSize + sizeof(buffer));
        memcpy(requestStr + requestSize, buffer, sizeof(buffer));
        requestSize += sizeof(buffer);
    }

    printf("%s", requestStr);

    struct Request* request = parseRequest(requestStr);


    //free request
    free(request->url);
    free(request->body);
    free(request->method);
    free(request->version);
    for(int i = 0; i < request->headersSize; i++) {
        free(request->headers[i].key);
        free(request->headers[i].value);
    }
    free(request->headers);

    free(args);
    close(args->connection_file_descriptor);
    return NULL;
}

int startServer(struct ServerConfig config) {
    struct sockaddr_in server_sockaddr_in;

    server_sockaddr_in.sin_family = AF_INET;
    server_sockaddr_in.sin_addr.s_addr = htonl(INADDR_ANY);
    server_sockaddr_in.sin_port = htons(config.port);

    int iSetOption = 1;
    int socket_file_descriptor = socket(AF_INET, SOCK_STREAM, 0);
    // set SO_REUSEADDR so that we can immediatly use the port again after the termination of the program
    setsockopt(socket_file_descriptor, SOL_SOCKET, SO_REUSEADDR, (char*)&iSetOption, sizeof((char*)&iSetOption)); 

    int err = bind(socket_file_descriptor, (struct sockaddr *)&server_sockaddr_in, sizeof(server_sockaddr_in));
    if(err == -1) {
        perror("bind");
        return -1;
    }

    listen(socket_file_descriptor, 5);

    while(1) {
        struct sockaddr_in client_sockaddr_in;
        socklen_t len = sizeof(client_sockaddr_in);

        int connection_file_descriptor = accept(socket_file_descriptor, (struct sockaddr *)&client_sockaddr_in, &len);

        pthread_t thread;
        int err;
        
        struct Args* args = malloc(sizeof(struct Args*));
        args->connection_file_descriptor = connection_file_descriptor;
        
        // create then detach a thread to handle the connection
        err = pthread_create(&thread, NULL, &handler, (void*)args);
        if(err) {
            printf("An error occured: %d", err);
            return 1;
        }
        err = pthread_detach(thread);
        if(err) {
            printf("An error occured: %d", err);
            return 1;
        }
    }

    close(socket_file_descriptor);
    return 0;
}

