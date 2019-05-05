#include <stdio.h>
#include <SDL2/SDL.h>

int main(int argc, char *argv[])
{
  printf("Hello SDL !\n");
  int quit = 1;
  SDL_Event event;
  SDL_Window *sdl_window = NULL;
  SDL_Renderer *renderer = NULL;

  SDL_Init(SDL_INIT_VIDEO);

  sdl_window = SDL_CreateWindow("Hello SDL",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                500,
                                500,
                                SDL_WINDOW_SHOWN);
  if (!sdl_window)
  {
    printf("Create window failed! \n");
    goto __FAIL;
  }
  renderer = SDL_CreateRenderer(sdl_window, -1, 0);
  if (!renderer)
  {
    printf("Create renderer failed! \n");
    goto __FAIL;
  }

  SDL_SetRenderDrawColor(renderer, 123, 123, 123, 255);
  SDL_RenderClear(renderer);

  SDL_RenderPresent(renderer);
  do
  {

    SDL_WaitEvent(&event);
    switch (event.type)
    {
    case SDL_QUIT:
      quit = 0;
      break;
    default:
      SDL_Log("event type is %d \n", event.type);
    }
  } while (quit);

__FAIL:
  if (renderer)
  {
    SDL_DestroyRenderer(renderer);
  }
  if (sdl_window)
  {
    SDL_DestroyWindow(sdl_window);
  }
  SDL_Quit();
  return 0;
}