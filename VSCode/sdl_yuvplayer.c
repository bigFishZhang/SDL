#include <stdio.h>
#include <string.h>
#include <SDL2/SDL.h>
/* ffplay -i 1080.mp4
Input #0, mov,mp4,m4a,3gp,3g2,mj2, from '1080.mp4':q=    0B f=0/0
  Metadata:
    major_brand     : isom
    minor_version   : 512
    compatible_brands: isomiso2avc1mp41
    encoder         : Lavf57.71.100
  Duration: 00:01:00.02, start: 0.000000, bitrate: 2064 kb/s
    Stream #0:0(eng): Video: h264 (High) (avc1 / 0x31637661), yuv420p, 1920x1080 [SAR 1:1 DAR 16:9], 1928 kb/s, 25 fps, 25 tbr, 12800 tbn, 50 tbc (default)
    Metadata:
      handler_name    : VideoHandler
    Stream #0:1(eng): Audio: aac (LC) (mp4a / 0x6134706D), 44100 Hz, stereo, fltp, 129 kb/s (default)
    Metadata:
      handler_name    : SoundHandler
   2.51 A-V: -0.024 fd=   0 aq=   21KB vq=   27KB sq=    0B f=0/0
*/

//事件消息
#define REFRESH_EVENT (SDL_USEREVENT + 1)
#define QUIT_EVENT (SDL_USEREVENT + 2)

int thread_exit = 0;

// 控制播放速率
int refresh_video_timer(void *udata)
{
  thread_exit = 0;
  while (!thread_exit)
  {
    SDL_Event event;
    event.type = REFRESH_EVENT;
    SDL_PushEvent(&event);
    SDL_Delay(40); //每40毫秒推送一次更新消息 每秒25帧
  }
  thread_exit = 0;
  //结束消息
  SDL_Event event;
  event.type = QUIT_EVENT;
  SDL_PushEvent(&event);
  return 0;
}

int main(int argc, char *argv[])
{
  FILE *video_file = NULL;

  SDL_Event event;
  SDL_Rect rect;

  Uint32 pixformt = 0;

  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  SDL_Texture *texture = NULL;
  SDL_Thread *timer_thread = NULL;

  int w_width = 1920, w_height = 1080;
  const int video_width = 1920, video_height = 1080;

  Uint8 *video_pos = NULL;

  unsigned int video_buff_len = 0;
  Uint8 *video_buf = NULL;

  const char *path = "1080.yuv";

  const unsigned int yuv_frame_len = video_width * video_height * 12 / 8; //对齐
  unsigned int tmp_yuv_frame_len = yuv_frame_len;
  if (yuv_frame_len & 0xF) //最后4位不为0
  {
    tmp_yuv_frame_len = (yuv_frame_len & 0xFFF0) + 0x10; //设置后4位为0 进位对齐
  }

  // 1 初始化SDL
  if (SDL_Init(SDL_INIT_VIDEO))
  {
    fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
    return -1;
  }

  // 2 创建window
  window = SDL_CreateWindow("YUV420 Player",
                            SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED,
                            w_width, w_height,
                            SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

  if (!window)
  {
    fprintf(stderr, "Failed to create window, %s\n", SDL_GetError());
    goto __FAIL;
  }

  // 3 创建渲染器
  renderer = SDL_CreateRenderer(window, -1, 0);
  if (!renderer)
  {
    fprintf(stderr, "Failed to create renderer, %s\n", SDL_GetError());
    goto __FAIL;
  }

  //  确定pixformt
  /**< Planar mode: Y + U + V  (3 planes) */ /**< Planar mode: Y + V + U  (3 planes) */
                                             // pixformt = SDL_PIXELFORMAT_YV12;
  pixformt = SDL_PIXELFORMAT_IYUV;

  // 4 创建纹理
  texture = SDL_CreateTexture(renderer,
                              pixformt,
                              SDL_TEXTUREACCESS_STREAMING,
                              video_width,
                              video_height);
  if (!texture)
  {
    fprintf(stderr, "Failed to create texture, %s\n", SDL_GetError());
    goto __FAIL;
  }

  // 初始化视频buffer
  video_buf = (Uint8 *)malloc(tmp_yuv_frame_len);
  if (!video_buf)
  {
    fprintf(stderr, "Failed to alloce yuv frame space %s\n", SDL_GetError());
    goto __FAIL;
  }

  // 5 打开文件
  video_file = fopen(path, "r");
  if (!video_file)
  {
    fprintf(stderr, "Failed to open video_file, %s\n", SDL_GetError());
    goto __FAIL;
  }
  // 6 读取一帧数据
  if ((video_buff_len = fread(video_buf, 1, yuv_frame_len, video_file)) <= 0)
  {
    fprintf(stderr, "Failed to read data from yuv file!\n");
    goto __FAIL;
  }
  printf("video_buff_len  %d \n", video_buff_len);
  // set video postion
  video_pos = video_buf;

  // 7 创建线程
  timer_thread = SDL_CreateThread(refresh_video_timer, NULL, NULL);

  // 8 监听事件
  do
  {
    //wait
    SDL_WaitEvent(&event);
    // 9 更新纹理
    if (event.type == REFRESH_EVENT)
    {

      SDL_UpdateTexture(texture, NULL, video_pos, video_width);

      //待处理，窗口大小调整
      rect.x = 0;
      rect.y = 0;
      rect.w = w_width;
      rect.h = w_height;

      SDL_RenderClear(renderer);
      SDL_RenderCopy(renderer, texture, NULL, &rect);
      SDL_RenderPresent(renderer);

      //读取下一帧数据
      if ((video_buff_len = fread(video_buf, 1, yuv_frame_len, video_file)) <= 0)
      {
        thread_exit = 1;
        continue;
      }
      printf("video_buff_len  %d \n", video_buff_len);
    }
    else if (event.type == SDL_WINDOWEVENT)
    {
      // 调整窗口
      SDL_GetWindowSize(window, &w_width, &w_height);
    }
    else if (event.type == SDL_QUIT)
    {
      thread_exit = 1;
    }
    else if (event.type == QUIT_EVENT)
    {
      break;
    }
  } while (1);

__FAIL:
  // 10 释放资源
  if (video_buf)
  {
    free(video_buf);
  }
  //close file
  if (video_file)
  {
    fclose(video_file);
  }
  SDL_DestroyTexture(texture);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}