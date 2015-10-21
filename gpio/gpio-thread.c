#include <inttypes.h>
#include <stdio.h>

#include <errno.h>
#include "byteorder.h"
#include "thread.h"
#include "xtimer.h"
#include "periph/gpio.h"

#define ONE_SECOND (1000000U)
static kernel_pid_t gpio_thread_pid = KERNEL_PID_UNDEF;
char gpio_thread_stack[THREAD_STACKSIZE_DEFAULT];
gpio_t freq_counter_pin = GPIO_PIN(PB, 03); //  The GPIO pin where we are measuring frequency.
int freq_counter_pin_value;   //  Stores the last known value of the freq_counter_pin
uint32_t freq_counter = 0;  //  Records the number of oscillations per second on freq_counter_pin.

/**
 * @brief   Frequency input pin handler.  Anytime the value
 *          at this pin changes, we check to see if there
 *          has actually been a state change (i.e. 3.3V vs GND),
 *          and increment out freq_counter.
 */
static void freqCounterISR(void *arg)
{
  //puts("gTest Interrupt Fired!");
  int _freq_counter_pin_value = gpio_read( freq_counter_pin );
  if (_freq_counter_pin_value != freq_counter_pin_value) {
    //  Only increment freq_counter half the time
    //  (On RISING GPIO events)
    if (_freq_counter_pin_value) {
      freq_counter++;
    }
    //  Store the new pin value for comparison next iteration.
    freq_counter_pin_value = _freq_counter_pin_value;
  }
}

static void *_gpio_thread(void *arg)
{
  (void)arg;

  //  Every second, report the number of oscillations
  //  recorded by (int)freq_counter.
  uint32_t last_wakeup = xtimer_now();
  while (1) {
    xtimer_usleep_until(&last_wakeup, ONE_SECOND);
    printf( "f = %lu\n", freq_counter );
    freq_counter = 0; // Reset freq_counter.
  }

  return NULL;
}

kernel_pid_t gpio_thread_getpid(void)
{
    return gpio_thread_pid;
}

kernel_pid_t gpio_thread_init(void)
{
    if (gpio_thread_pid == KERNEL_PID_UNDEF) {
      // (1) Initialize an interrupt on freq_counter_pin.
      gpio_init_int( freq_counter_pin, GPIO_NOPULL, GPIO_BOTH, freqCounterISR, NULL );

      // (2) Create a thread to handle computing the frequency
      //     every 1 second.
      gpio_thread_pid = thread_create(gpio_thread_stack, sizeof(gpio_thread_stack), THREAD_PRIORITY_MAIN-1, 0, _gpio_thread, NULL, "gpioThread");
    }
    return gpio_thread_pid;
}
