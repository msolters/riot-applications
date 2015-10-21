/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for GPIO R/W & Interrupts
 *
 * @author      Mark Solters <msolters@driblet.io>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "shell_commands.h"
#include "periph/gpio.h"
#include "xtimer.h"

/**
 * @brief   Maybe you are a golfer?!
 */
#define FREQ_COUNTER_INTERVAL (1000000U)
uint32_t freq_counter = 0;  //  Records the number of oscillations per second on freq_counter_pin.
gpio_t freq_counter_pin = GPIO_PIN(PB, 03); //  The GPIO pin where we are measuring frequency.
int freq_counter_pin_value;   //  Stores the last known value of the freq_counter_pin

/**
 * @brief   Frequency input pin handler.  Anytime the value
 *          at this pin changes, we check to see if there
 *          has actually been a state change (i.e. 3.3V vs GND),
 *          and increment out freq_counter.
 */
static void freqCounterISR(void *arg) {
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

int main(void)
{
    puts("GPIO App");

    //  Initialize an interrupt on freq_counter_pin.
    gpio_init_int( freq_counter_pin, GPIO_NOPULL, GPIO_BOTH, freqCounterISR, NULL );

    //  Every second, report the number of oscillations
    //  recorded by (int)freq_counter.
    uint32_t last_wakeup = xtimer_now();
    while (1) {
      xtimer_usleep_until(&last_wakeup, FREQ_COUNTER_INTERVAL);
      printf( "f = %lu\n", freq_counter );
      freq_counter = 0; // Reset freq_counter.
    }

    /* start the shell */
    puts("Initialization successful - starting the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
