#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LOG_LENGTH 10
#define MAX_BUFFER_SLOT 6
#define MAX_LOOPS 30

char logbuf[MAX_BUFFER_SLOT][MAX_LOG_LENGTH];

int count;
void flushlog();
pthread_mutex_t loglock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buf_flushed = PTHREAD_COND_INITIALIZER;

struct _args
{
   unsigned int interval;
};

void *wrlog(void *data)
{
   char str[MAX_LOG_LENGTH];
   int id = *(int*) data;
   free(data);
   sprintf(str, "%d", id);
   
   usleep(20);
   pthread_mutex_lock(&loglock);
   while(count >= MAX_BUFFER_SLOT)
   {
      pthread_cond_wait(&buf_flushed, &loglock);
   }
   strcpy(logbuf[count], str);
   count++;/* Only increase count to size MAX_BUFFER_SLOT*/
   printf("wrlog(): %d \n", id);
   pthread_mutex_unlock(&loglock);
   return 0;
}

void flushlog()
{
   int i;
   char nullval[MAX_LOG_LENGTH];
   
   pthread_mutex_lock(&loglock);
   printf("flushlog()\n");
   sprintf(nullval, "%d", -1);
   for (i = 0; i < count; i++)
   {
      printf("Slot  %i: %s\n", i, logbuf[i]);
      strcpy(logbuf[i], nullval);
   }

   fflush(stdout);

   /*Reset buffer */
   count = 0;
   pthread_cond_broadcast(&buf_flushed);

   pthread_mutex_unlock(&loglock);
   return;

}

void *timer_start(void *args)
{
   while (1)
   {
      flushlog();
      /*Waiting until the next timeout */
      usleep(((struct _args *) args)->interval);
   }
}

int main()
{
   int i;
   count = 0;
   pthread_t tid[MAX_LOOPS];
   pthread_t lgrid;
   int id[MAX_LOOPS];

   struct _args args;
   args.interval = 500e3;
   /*500 msec ~ 500 * 1000 usec */

   /*Setup periodically invoke flushlog() */
   pthread_create(&lgrid, NULL, &timer_start, (void*) &args);

   /*Asynchronous invoke task writelog */
   for (i = 0; i < MAX_LOOPS; i++) {
      int* pid = malloc(sizeof(int));
      *pid = i;
      pthread_create(&tid[i], NULL, wrlog, pid);
  }

   for (i = 0; i < MAX_LOOPS; i++)
      pthread_join(tid[i], NULL);

   sleep(5);

   return 0;
}
