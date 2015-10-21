#include <inttypes.h>
#include <stdio.h>

#include <errno.h>
#include "byteorder.h"
#include "thread.h"
#include "net/gnrc.h"

#include "od.h"

#define MSG_Q_SIZE  (8U)
static kernel_pid_t _pid = KERNEL_PID_UNDEF;
char process_msg_stack[THREAD_STACKSIZE_DEFAULT];

static void process_msgs_parse(gnrc_pktsnip_t *pkt) {
  int snips = 0;
  int size = 0;
  gnrc_pktsnip_t *snip = pkt;

  while (snip != NULL) {
      //printf("~~ SNIP %2i - size: %3u byte, type: ", snips,
      //       (unsigned int)snip->size);
      //od(snip->data, snip->size, 0, OD_FLAGS_BYTES_CHAR);
      if (snips == 0) {
        printf(snip->data);
        printf("\n");
      }
      ++snips;
      size += snip->size;
      snip = snip->next;
  }

  //printf("~~ PKT    - %2i snips, total size: %3i byte\n", snips, size);
  gnrc_pktbuf_release(pkt);
}

static void *_process_msgs(void *arg) {
  (void)arg;

  msg_t msg, reply;
  msg_t msg_queue[MSG_Q_SIZE];

  /* setup the message queue */
  msg_init_queue(msg_queue, MSG_Q_SIZE);

  reply.content.value = (uint32_t)(-ENOTSUP);
  reply.type = GNRC_NETAPI_MSG_TYPE_ACK;

  while (1) {
      msg_receive(&msg);

      switch (msg.type) {
          case GNRC_NETAPI_MSG_TYPE_RCV:
              //puts("Process Msgs: data received:");
              process_msgs_parse( (gnrc_pktsnip_t *)msg.content.ptr );
              break;
          case GNRC_NETAPI_MSG_TYPE_SND:
              puts("Process Msgs: data to send:");
              break;
          case GNRC_NETAPI_MSG_TYPE_GET:
          case GNRC_NETAPI_MSG_TYPE_SET:
              msg_reply(&msg, &reply);
              break;
          default:
              puts("Process Msgs: received something unexpected");
              break;
      }
  }

  return NULL;
}

kernel_pid_t process_msgs_getpid(void)
{
    return _pid;
}

kernel_pid_t process_msgs_init(void)
{
    if (_pid == KERNEL_PID_UNDEF) {
        _pid = thread_create(process_msg_stack, sizeof(process_msg_stack), THREAD_PRIORITY_MAIN-1,
                             0, _process_msgs, NULL, "processMsgs");
    }
    return _pid;
}
