#include <sys/socket.h>
#include <sys/un.h>

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#include "../filter/filter.h"

#include "../parameters/parameters.h"

#include "server.h"

static int connect_downstream
(
   struct JH_server_worker worker [const restrict static 1]
)
{
   struct sockaddr_un addr;

   const int old_errno = errno;

   errno = 0;

   if ((worker->downstream_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
   {
      JH_FATAL
      (
         stderr,
         "Unable to create socket: %s.",
         strerror(errno)
      );

      errno = old_errno;

      return -1;
   }

   errno = old_errno;

   memset((void *) &addr, (int) 0, sizeof(addr));

   addr.sun_family = AF_UNIX;

   strncpy
   (
      (char *) addr.sun_path,
      JH_parameters_get_dest_socket_name(worker->params.server_params),
      (sizeof(addr.sun_path) - ((size_t) 1))
   );

   errno = 0;

   if
   (
      connect
      (
         worker->downstream_socket,
         (struct sockaddr *) &addr,
         sizeof(addr)
      )
      == -1
   )
   {
      JH_FATAL
      (
         stderr,
         "Unable to connect to address: %s.",
         strerror(errno)
      );

      errno = old_errno;

      close(worker->downstream_socket);

      worker->downstream_socket = -1;

      return -1;
   }

   errno = old_errno;

   return 0;
}

static int initialize
(
   struct JH_server_worker worker [const restrict static 1],
   void * input
)
{
   memcpy
   (
      (void *) &(worker->params),
      (const void *) input,
      sizeof(struct JH_server_thread_parameters)
   );

   pthread_barrier_wait(&(worker->params.thread_collection->barrier));

   return connect_downstream(worker);
}

static void finalize
(
   struct JH_server_worker worker [const restrict static 1]
)
{
   if (worker->downstream_socket != -1)
   {
      close(worker->downstream_socket);

      worker->downstream_socket = -1;
   }

   if (worker->params.socket != -1)
   {
      close(worker->params.socket);

      worker->params.socket = -1;
   }

   pthread_mutex_lock(&(worker->params.thread_collection->mutex));

   worker->params.thread_collection->threads[worker->params.thread_id].state =
      JH_SERVER_JOINING_THREAD;

   pthread_mutex_unlock(&(worker->params.thread_collection->mutex));
}

void * JH_server_worker_main (void * input)
{
   int status;
   int timeout_count;
   struct JH_filter filter;
   struct JH_server_worker worker;

   initialize(&worker, input);

   if (JH_filter_initialize(&filter) < 0)
   {
      finalize(&worker);

      return NULL;
   }

   timeout_count = 0;

   while (JH_server_is_running())
   {
      status =
         JH_filter_step
         (
            &filter,
            worker.params.socket,
            worker.downstream_socket
         );

      if (status == 0)
      {
         timeout_count = 0;
      }
      else if (status == 1)
      {
         timeout_count += 1;

         if (timeout_count == 2)
         {
            break;
         }
         else
         {
            sleep(JH_SERVER_WORKER_MAX_WAITING_TIME);
         }
      }
      else
      {
         break;
      }
   }

   JH_filter_finalize(&filter);

   finalize(&worker);

   return NULL;
}
