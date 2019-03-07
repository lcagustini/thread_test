#include <SDL2/SDL.h>
#include <pthread.h>
#include <stdint.h>
#include <limits.h>

#define POINTS_LEN 10000000
#define THREAD_NUM 4

typedef struct {
    uint64_t x;
    uint64_t y;
} vec2;

vec2 points[POINTS_LEN] = {0};
uint64_t seeds[THREAD_NUM] = {0};
int ready[THREAD_NUM] = {0};

pthread_mutex_t mutex;
pthread_cond_t cond;

uint64_t xorshift64(uint64_t* state) {
    uint64_t x = *state;
    x^= x << 13;
    x^= x >> 7;
    x^= x << 17;
    *state = x;
    return x;
}

void *calc(void *args) {
    long aux = (long) args;

    while (1) {
        uint64_t s = 111*(aux+90);
        for (int i = POINTS_LEN/THREAD_NUM * aux; i < ((aux == THREAD_NUM-1) ? POINTS_LEN : (POINTS_LEN/THREAD_NUM * (aux+1))); i++) {
            points[i].x = xorshift64(&s);
            points[i].y = xorshift64(&s);
        }

        pthread_mutex_lock(&mutex);
        ready[aux] = 1;
        pthread_cond_wait(&cond, &mutex);
        pthread_mutex_unlock(&mutex);
    }
}

int main(void){
    pthread_t threadIds[THREAD_NUM];

    for (int i = 0; i < THREAD_NUM; i++) {
        seeds[i] = i;
        ready[i] = 0;
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);

    for(long i = 0; i < THREAD_NUM; i++){
        pthread_create(&threadIds[i], NULL, calc, (void *) i);
    }

    Uint64 curTime = SDL_GetPerformanceCounter();
    Uint64 lastTime = 0;
    double dt = 0;
    while (1) {
        pthread_mutex_lock(&mutex);
        int allDone = 1;
        for(int i = 0; i < THREAD_NUM; i++){
            allDone &= ready[i];
        }
        pthread_mutex_unlock(&mutex);

        if (allDone) {
            for(int i = 0; i < THREAD_NUM; i++){
                ready[i] = 0;
            }

            pthread_cond_broadcast(&cond);

            lastTime = curTime;
            curTime = SDL_GetPerformanceCounter();
            dt = (double) ((abs(curTime - lastTime)) / (double) SDL_GetPerformanceFrequency());
            printf("FPS: %lf\n", 1/dt);
        }
    }

    return 0;
}
