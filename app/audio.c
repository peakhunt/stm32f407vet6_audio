#include "audio.h"
#include "arm_math.h"
#include "arm_const_structs.h"

#define FFT_LEN               AUDIO_BUFFER_SIZE
#define REAL_PART(x)          (x * 2 + 0)
#define IMAG_PART(x)          (x * 2 + 1)

#if 0   // complex FFT
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

////////////////////////////////////////////////////////////////////////////////
//
// private DSP kernels
//
////////////////////////////////////////////////////////////////////////////////
static void
audio_process_bypass(float32_t* mag, int len)
{
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
#else // real fFT
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
static float32_t                _input_buffer[FFT_LEN*2];
static float32_t                _output_buffer[FFT_LEN*2];

static arm_rfft_instance_f32          _fwd_fft;
static arm_cfft_radix4_instance_f32   _fwd_cfft;

static arm_rfft_instance_f32          _inv_fft;
static arm_cfft_radix4_instance_f32   _inv_cfft;

////////////////////////////////////////////////////////////////////////////////
//
// private DSP kernels
//
////////////////////////////////////////////////////////////////////////////////
static void
audio_process_bypass(float32_t* mag, int len)
{
}

////////////////////////////////////////////////////////////////////////////////
//
// public interfaces
//
////////////////////////////////////////////////////////////////////////////////
void
audio_init(void)
{
  arm_rfft_init_f32(&_fwd_fft, &_fwd_cfft, FFT_LEN, 0, 1);
  arm_rfft_init_f32(&_inv_fft, &_inv_cfft, FFT_LEN, 1, 1);
}

void
audio_process(audio_buffer_t* b)
{
  for(int i = 0; i < FFT_LEN; i++)
  {
    _input_buffer[i] = b->buffer[i];
  }

  arm_rfft_f32(&_fwd_fft, _input_buffer, _output_buffer);

  //
  // XXX _output_buffer is a complex buffer
  //

  audio_process_bypass(_output_buffer, FFT_LEN);

  // inverse
  arm_rfft_f32(&_inv_fft, _output_buffer, _input_buffer);

  for(int i = 0; i < FFT_LEN; i++)
  {
    b->buffer[i] = (uint16_t)_input_buffer[i];
  }
}
#endif
