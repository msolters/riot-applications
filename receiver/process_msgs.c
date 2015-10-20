#include <inttypes.h>
#include <stdio.h>

#include <errno.h>
#include "byteorder.h"
#include "thread.h"
#include "net/gnrc.h"

#include "xtimer.h"

#define MSG_Q_SIZE  (8U)
static kernel_pid_t _pid = KERNEL_PID_UNDEF;
char process_msg_stack[THREAD_STACKSIZE_DEFAULT];

static void *_process_msgs(void *arg) {
  (void)arg;
  /*uint32_t last_wakeup = xtimer_now();
  while (1) {
    xtimer_usleep_until(&last_wakeup, (1000000U));
    printf( "It is now %"PRIu32"\n", xtimer_now() );
    //bcastData("Hello World!");
  }*/

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
              puts("Process Msgs: data received:");
              //_dump((gnrc_pktsnip_t *)msg.content.ptr);
              break;
          case GNRC_NETAPI_MSG_TYPE_SND:
              puts("Process Msgs: data to send:");
              //_dump((gnrc_pktsnip_t *)msg.content.ptr);
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
