#include "co.h"
#include <stdlib.h>
#include<stdio.h>
#include <setjmp.h>
#include <time.h>
#define STACK_SIZE 64000
#define POOL_SIZE 128
enum co_status
{
  CO_NEW = 1, // 新创建，还未执行过
  CO_RUNNING, // 已经执行过
  CO_WAITING, // 在 co_wait 上等待
  CO_DEAD,    // 已经结束，但还未释放资源
};
int g_count = 0;
jmp_buf jump_buffer;
struct co
{
  char *name;
  void (*entry)(void *);//co_start入口函数
  void *arg;
  enum co_status status;//协程状态
  struct co *waiter;//处于等待状态的其他协程
  jmp_buf context;//寄存器上下文
  uint8_t stack[STACK_SIZE];
  int index;
} co1, *co_pool[POOL_SIZE];
int coptr = 0;

struct co *co_start(const char *name, void (*func)(void *), void *arg)
{
  int is_full = 0;
  int i = 0;
  if (coptr >= POOL_SIZE)
  {
      return NULL;
  }
  co_pool[coptr] = (struct co *)malloc(sizeof(co1));
  co_pool[coptr]->name = (char *)name;
  co_pool[coptr]->entry = func;
  co_pool[coptr]->arg = arg;
  co_pool[coptr]->status = CO_NEW;
  co_pool[coptr]->index = coptr;
  co_pool[coptr]->waiter = NULL;
  coptr++;
  return co_pool[coptr - 1];
}

void co_wait(struct co *co)
{
  co->status = CO_WAITING;
  int index_cur = co->index;
  int num = setjmp(jump_buffer);
  if(g_count<200)
  {
  if(num==0)
  {
  co->entry(co->arg);
  }
  else
  {
  co_pool[num-1]->entry(co_pool[num-1]->arg);
  }
  }
  else
  {
    free(co);
  for (int i = co->index; i < coptr;i++)//后面的协程前移
  {
    co_pool[i] = co_pool[i + 1];
  }
  co_pool[coptr] = NULL;
  coptr--;
  }
}

void co_yield ()
{
  
  int destination = rand() % coptr;
  longjmp(jump_buffer, destination+1);
}
void entry1(void *arg) {
  for (int i = 0; i < 100; i++) {

    printf("%s[%d] ", (const char *)arg, g_count++);
    co_yield();
  }
}
 int main()
 {
  srand((unsigned)time(NULL));
  struct co *co1 = co_start("co1", entry1, "X");
  struct co *co2 = co_start("co2", entry1, "Y");
  co_wait(co1);
  co_wait(co2);
  printf("Done");
  return 0;
 }