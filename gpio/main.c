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
#include "thread.h"

/**
 * @brief   Maybe you are a golfer?!
 */
//extern kernel_pid_t process_msgs_getpid(void);
extern kernel_pid_t gpio_thread_init(void);

int main(void)
{
    puts("GPIO App");
    gpio_thread_init();

    /* start the shell */
    puts("Initialization successful - starting the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
