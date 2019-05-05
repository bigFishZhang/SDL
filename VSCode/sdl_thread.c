// SDL 线程使用

#include <stdio.h>
#include <SDL2/SDL.h>

static int test_thread(void *ptr)
{

  printf("%s \n", ptr);
  int cnt;
  for (cnt = 0; cnt < 15; cnt++)
  {
    printf("Thread counter:%d \n", cnt);
    SDL_Delay(50);
  }
  return cnt;
}

int main(int argc, char *argv[])
{
  SDL_Thread *thread;
  int threadReturnValue;
  char *arg = "hell SDL_Thread";
  thread = SDL_CreateThread(test_thread, "test_thread", (void *)arg);

  if (NULL == thread)
  {
    printf("\nSDL_CreateThread failed: %s\n", SDL_GetError());
  }
  else
  {
    //等待线程结束
    SDL_WaitThread(thread, &threadReturnValue);
    printf("\nThread returned value: %d", threadReturnValue);
  }

  return 0;
}
