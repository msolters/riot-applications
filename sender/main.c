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
 * @brief       Test application for AT86RF2xx network device driver
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdio.h>

#include "shell.h"
#include "shell_commands.h"
#include "net/gnrc/pktdump.h"
#include "net/gnrc.h"
#include "xtimer.h"

#define MAX_ADDR_LEN  (8U)

int bcastData(char *_data)
{
    kernel_pid_t dev;
    uint8_t addr[MAX_ADDR_LEN];
    size_t addr_len;
    gnrc_pktsnip_t *pkt;
    gnrc_netif_hdr_t *nethdr;
    uint8_t flags = 0x00;

    /* parse interface */
    dev = (kernel_pid_t)atoi("4");//(kernel_pid_t)atoi(argv[1]);

    /* parse address */
    addr_len = gnrc_netif_addr_from_str(addr, sizeof(addr), "bcast");
    flags |= GNRC_NETIF_HDR_FLAGS_BROADCAST;

    /* put packet together */
    pkt = gnrc_pktbuf_add(NULL, _data, strlen(_data), GNRC_NETTYPE_UNDEF);
    pkt = gnrc_pktbuf_add(pkt, NULL, sizeof(gnrc_netif_hdr_t) + addr_len,
                          GNRC_NETTYPE_NETIF);
    nethdr = (gnrc_netif_hdr_t *)pkt->data;
    gnrc_netif_hdr_init(nethdr, 0, addr_len);
    gnrc_netif_hdr_set_dst_addr(nethdr, addr, addr_len);
    nethdr->flags = flags;
    /* and send it */
    if (gnrc_netapi_send(dev, pkt) < 1) {
        puts("error: unable to send\n");
        gnrc_pktbuf_release(pkt);
        return 1;
    }

    return 0;
}


/**
 * @brief   Maybe you are a golfer?!
 */
#define SINTERVAL (3000000U)
int msgs = 0;

int main(void)
{
    //gnrc_netreg_entry_t dump;

    puts("Sender App");
    uint32_t last_wakeup = xtimer_now();
    while (1) {
      xtimer_usleep_until(&last_wakeup, SINTERVAL);
      //printf( "It is now %"PRIu32"\n", xtimer_now() );
      char msg[25];
      sprintf(msg, "Msg %u: It is now %lu", msgs, xtimer_now());
      bcastData(msg);
      msgs++;
    }

    /* register the pktdump thread */
    /*puts("Register the packet dump thread for GNRC_NETTYPE_UNDEF packets");
    dump.pid = gnrc_pktdump_getpid();
    dump.demux_ctx = GNRC_NETREG_DEMUX_CTX_ALL;
    gnrc_netreg_register(GNRC_NETTYPE_UNDEF, &dump);
    */

    /* start the shell */
    puts("Initialization successful - starting the shell now");
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(NULL, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}
