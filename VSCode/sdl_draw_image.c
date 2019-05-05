//SDL 图片渲染
#include <stdio.h>
// #include <stdafx.h>
#include <SDL2/SDL.h>

// #define IMG_PATH "/Users/bigfish/test/SDL_logo.png"
#define IMG_PATH "/Users/bigfish/test/AAC05.bmp"

#undef main //SDL 中定义了main，所以这里去掉，不然会执行错误！

int main(int argc, char *argv[])
{
  SDL_Init(SDL_INIT_VIDEO);
  int quit = 1;
  SDL_Event event;
  SDL_Window *window;
  SDL_Surface *surfaceBMP;

  SDL_Renderer *renderer;
  SDL_Texture *texture;

  //获取窗口指针，未指定位置信息，宽高为800x600
  window = SDL_CreateWindow("SDL_bmp_title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);

  surfaceBMP = SDL_LoadBMP(IMG_PATH);

  renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  //创建纹理
  texture = SDL_CreateTextureFromSurface(renderer, surfaceBMP);
  //清空渲染器
  SDL_RenderClear(renderer);
  //将纹理显示在渲染器上
  SDL_RenderCopy(renderer, texture, NULL, NULL);
  //显示渲染器内容
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

  //释放资源
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_FreeSurface(surfaceBMP);
  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}