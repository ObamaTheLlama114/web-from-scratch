#include <stdio.h>
#include "../../libhttpserver/include/httpserver.h"

int main() {
    printf("starting api\n");
    struct ServerConfig config = newServerConfig(8080);
    startServer(config);
}
