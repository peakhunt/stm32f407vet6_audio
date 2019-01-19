#include "stm32f4xx_hal.h"
#include "gpio.h"

#include "app.h"
#include "audio_buffer.h"
#include "adc_read.h"
#include "dac_write.h"
#include "audio.h"

#include "event_dispatcher.h"
#include "shell.h"

bool _bypass   = false;

void
app_init_f(void)
{
  event_dispatcher_init();
}

void
app_init(void)
{
  audio_buffer_init();
  adc_read_init();
  dac_write_init();
  // audio_init();

  __disable_irq();
  shell_init();
  __enable_irq();
}

void
app_start(void)
{
  adc_read_start();
  dac_write_start();
}

static void
app_startup_accumulate(void)
{
  #define START_UP_DELAY      4

  audio_buffer_t* b;
  int count = 0;
  static audio_buffer_t* startup_buf[START_UP_DELAY];

  while(count < START_UP_DELAY)
  {
    b = adc_read_in();
    if(b != NULL)
    {
      startup_buf[count] = b;
      count++;
    }
  }

  for(count = 0; count < START_UP_DELAY; count++)
  {
    if(_bypass == false)
    {
      audio_process(startup_buf[count]);
    }
    dac_write_put(startup_buf[count]);
  }
}

void
app_loop(void)
{
  audio_buffer_t* b;

  // turn off all leds
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET);

  app_startup_accumulate();

  while(1)
  {
    b = adc_read_in();

    if(b != NULL)
    {
      if(_bypass == false)
      {
        audio_process(b);
      }
      dac_write_put(b);
    }
    event_dispatcher_dispatch();
  }
}
