#include <SDL2/SDL.h>
#include <pthread.h>

#define POINTS_LEN 100000
#define THREAD_NUM 4

typedef struct {
    float x;
    float y;
} vec2;

vec2 points[POINTS_LEN] = {0};

void *calc(void *args) {
    long aux = (long) args;

    for (int i = POINTS_LEN/THREAD_NUM * aux; i < ((aux == THREAD_NUM-1) ? POINTS_LEN : (POINTS_LEN/THREAD_NUM * (aux+1))); i++) {
        points[i].x += (float)rand()/(float)(RAND_MAX) > 0.5 ? rand()/((float)RAND_MAX/5) : -rand()/((float)RAND_MAX/5);
        points[i].y += (float)rand()/(float)(RAND_MAX) > 0.5 ? rand()/((float)RAND_MAX/5) : -rand()/((float)RAND_MAX/5);
    }
}

int main(void){
    pthread_t threadIds[THREAD_NUM];

    Uint64 curTime = SDL_GetPerformanceCounter();
    Uint64 lastTime = 0;
    double dt = 0;
    while (1) {
        for(long i = 0; i < THREAD_NUM; i++){
            pthread_create(&threadIds[i], NULL, calc, (void *) i);
        }
        for(int i = 0; i < THREAD_NUM; i++){
            pthread_join(threadIds[i], NULL);
        }

        lastTime = curTime;
        curTime = SDL_GetPerformanceCounter();
        dt = (double) ((abs(curTime - lastTime)) / (double) SDL_GetPerformanceFrequency());
        printf("FPS: %lf\n", 1/dt);
    }

    return 0;
}
