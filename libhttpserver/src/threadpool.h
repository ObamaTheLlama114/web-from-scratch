#ifndef __threadpool_h__
#define __threadpool_h__

struct ThreadPool;

typedef void (*ThreadHandler)(void* arg);

int createThreadPool();
int createThread(struct ThreadPool*, ThreadHandler);

#endif
