#include "audio.h"
#include "arm_math.h"
#include "arm_const_structs.h"

#define FFT_LEN               AUDIO_BUFFER_SIZE
#define REAL_PART(x)          (x * 2 + 0)
#define IMAG_PART(x)          (x * 2 + 1)

////////////////////////////////////////////////////////////////////////////////
//
// module prototypes
//
////////////////////////////////////////////////////////////////////////////////
static void audio_process_bypass(float32_t* mag, int len);

////////////////////////////////////////////////////////////////////////////////
//
// module privates
//
////////////////////////////////////////////////////////////////////////////////
static float32_t                 _input_buffer[FFT_LEN*2];
//static float32_t                 _magnitudes[FFT_LEN * 2];
//static flaot32_t                 _output_buffer[FFT_LEN * 2];

////////////////////////////////////////////////////////////////////////////////
//
// private DSP kernels
//
////////////////////////////////////////////////////////////////////////////////
static void
audio_process_bypass(float32_t* mag, int len)
{
  len++;
}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
////////////////////////////////////////////////////////////////////////////////
void
audio_init(void)
{
}

void
audio_process(audio_buffer_t* b)
{
  for(int i = 0; i < FFT_LEN; i++)
  {
    _input_buffer[REAL_PART(i)] = b->buffer[i];
    _input_buffer[IMAG_PART(i)] = 0;
  }

  // forward
  arm_cfft_f32(&arm_cfft_sR_f32_len128, _input_buffer, 0, 1);

  audio_process_bypass(_input_buffer, FFT_LEN);

  // inverse
  arm_cfft_f32(&arm_cfft_sR_f32_len128, _input_buffer, 1, 1);

  for(int i = 0; i < FFT_LEN; i++)
  {
    b->buffer[i] = (uint16_t)_input_buffer[REAL_PART(i)];
  }
}
