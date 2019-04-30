//SDL 图形绘制    矩形  三角形
#include <stdio.h>
#include <SDL2/SDL.h>

#define POINT_COUNT 5
#define T_POINT_COUNT 4
// points to draw
static SDL_Point points[POINT_COUNT] = {
    {50, 50},
    {60, 50},
    {60, 60},
    {50, 60},
    {50, 50}};

static SDL_Point triangle_points[T_POINT_COUNT] = {
    {150, 150},
    {200, 150},
    {175, 175},
    {150, 150}};

int main(int argc, char *argv[])
{
  SDL_Window *sdl_window;
  SDL_Renderer *renderer;
  int quit = 1;
  SDL_Event event;
  // 0 init sdl
  SDL_Init(SDL_INIT_VIDEO);

  //1 create window
  sdl_window = SDL_CreateWindow("triangle",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                600,
                                600,
                                SDL_WINDOW_SHOWN);
  //2 create renderer
  renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_SOFTWARE);

  //3 set renderer color (set background color  Blue)
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);

  //4 clear color
  SDL_RenderClear(renderer);

  //5 draw rect
  SDL_Rect rect = {0, 0, 400, 400};
  SDL_RenderDrawRect(renderer, &rect);

  //Green
  SDL_SetRenderDrawColor(renderer, 0, 128, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &rect);

  SDL_Rect big_rect = {10, 10, 200, 200};
  SDL_RenderDrawRect(renderer, &big_rect);

  // Yellow
  SDL_SetRenderDrawColor(renderer, 255, 255, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderFillRect(renderer, &big_rect);

  // draw  triangle
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLines(renderer, triangle_points, T_POINT_COUNT);

  // draw rectangular
  SDL_SetRenderDrawColor(renderer, 128, 0, 128, SDL_ALPHA_OPAQUE);
  SDL_RenderDrawLines(renderer, points, POINT_COUNT);

  // show window
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