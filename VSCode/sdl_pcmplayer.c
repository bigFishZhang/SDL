#include <stdio.h>
#include <SDL2/SDL.h>

#define BLOCK_SIZE 1024000

static Uint8 *audio_buf = NULL; //读取的音频buffer
static Uint8 *audio_pos = NULL; //当前位置
static size_t buffer_len = 0;   //读取的音频长度

//callback function for audio devcie  udata是设置的回调参数
void read_audio_data(void *udata, Uint8 *stream, int len)
{

  if (buffer_len == 0)
  {
    return;
  }
  // 清空 TODO 这么粗暴的操作导致还没播完的buffer被清除了声音断断续续
  SDL_memset(stream, 0, len);
  // 计算本次读取的数据大小
  len = (len < buffer_len) ? len : buffer_len;
  // 数据拷贝到stream指向的地址
  SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
  //移动位置
  audio_pos += len;
  //计算剩余的buffer_len
  buffer_len -= len;
}

int main(int argc, char *argv[])
{
  int ret = -1;

  FILE *audio_fd = NULL;

  SDL_AudioSpec spec;

  char *path = "1080.pcm";

  //SDL initialize
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER))
  {
    fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
    return ret;
  }

  //open pcm file
  audio_fd = fopen(path, "r");

  if (!audio_fd)
  {
    fprintf(stderr, "Failed to open pcm file!\n");
    goto __FAIL;
  }

  //初始化Buffer
  audio_buf = (Uint8 *)malloc(BLOCK_SIZE);
  if (!audio_buf)
  {
    goto __FAIL;
  }
  // rewind(audio_fd);
  //SDL_AudioSpec
  spec.freq = 44100;
  spec.format = AUDIO_S16SYS;
  spec.channels = 2;
  spec.silence = 0;
  spec.samples = 1024;
  spec.callback = read_audio_data;
  spec.userdata = NULL;

  //open audio devcie
  if (SDL_OpenAudio(&spec, NULL))
  {
    fprintf(stderr, "Failed to open audio device, %s\n", SDL_GetError());
    goto __FAIL;
  }

  //play audio
  SDL_PauseAudio(0);

  do
  {
    //read data from pcm file
    buffer_len = fread(audio_buf, 1, BLOCK_SIZE, audio_fd);
    fprintf(stderr, "block size is %zu\n", buffer_len);

    audio_pos = audio_buf;

    //the main thread wait for a moment
    while (audio_pos < (audio_buf + buffer_len)) // 还有数据
    {
      SDL_Delay(1);
    }

  } while (buffer_len != 0); //读到文件尾

  //close audio device
  //Todo 缓冲区的数据可能还没有处理完
  SDL_CloseAudio();

  ret = 0;

__FAIL:
  //release some resources
  if (audio_buf)
  {
    free(audio_buf);
  }

  if (audio_fd)
  {
    fclose(audio_fd);
  }

  //quit SDL
  SDL_Quit();

  return ret;
}