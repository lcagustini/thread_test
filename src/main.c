#include <SDL2/SDL.h>
#include <pthread.h>

#define POINTS_LEN 100000
#define THREAD_NUM 1

typedef struct {
    float x;
    float y;
} vec2;

vec2 points[POINTS_LEN] = {0};
unsigned int seeds[THREAD_NUM] = {0};
int ready[THREAD_NUM] = {0};

pthread_mutex_t mutex;

void *calc(void *args) {
    long aux = (long) args;

    while (1) {
        pthread_mutex_lock(&mutex);
        int r = ready[aux];
        pthread_mutex_unlock(&mutex);
        if (!r) {
            for (int i = POINTS_LEN/THREAD_NUM * aux; i < ((aux == THREAD_NUM-1) ? POINTS_LEN : (POINTS_LEN/THREAD_NUM * (aux+1))); i++) {
                points[i].x += (float)rand_r(&seeds[aux])/(float)(RAND_MAX) > 0.5 ? rand_r(&seeds[aux])/((float)RAND_MAX/5) : -rand_r(&seeds[aux])/((float)RAND_MAX/5);
                points[i].y += (float)rand_r(&seeds[aux])/(float)(RAND_MAX) > 0.5 ? rand_r(&seeds[aux])/((float)RAND_MAX/5) : -rand_r(&seeds[aux])/((float)RAND_MAX/5);
            }

            pthread_mutex_lock(&mutex);
            ready[aux] = 1;
            pthread_mutex_unlock(&mutex);
        }
    }
}

int main(void){
    pthread_t threadIds[THREAD_NUM];

    for (int i = 0; i < THREAD_NUM; i++) {
        seeds[i] = time(NULL);
        ready[i] = 0;
    }

    pthread_mutex_init(&mutex, NULL);

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
            pthread_mutex_lock(&mutex);
            for(int i = 0; i < THREAD_NUM; i++){
                ready[i] = 0;
            }
            pthread_mutex_unlock(&mutex);

            lastTime = curTime;
            curTime = SDL_GetPerformanceCounter();
            dt = (double) ((abs(curTime - lastTime)) / (double) SDL_GetPerformanceFrequency());
            printf("FPS: %lf\n", 1/dt);
        }
    }

    return 0;
}
