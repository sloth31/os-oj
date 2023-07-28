#include "co.h"
#include <stdlib.h>
#include <setjmp.h>
#define STACK_SIZE 64000
enum co_status
{
  CO_NEW = 1, // 新创建，还未执行过
  CO_RUNNING, // 已经执行过
  CO_WAITING, // 在 co_wait 上等待
  CO_DEAD,    // 已经结束，但还未释放资源
};

struct co
{
  char *name;
  void (*entry)(void *);//co_start入口函数
  void *arg;
  enum co_status status;//协程状态
  struct co *waiter;//处于等待状态的其他协程
  jmp_buf context;//寄存器上下文
  uint8_t stack[STACK_SIZE];
};

struct co *co_start(const char *name, void (*func)(void *), void *arg)
{
  return NULL;
}

void co_wait(struct co *co)
{
}

void co_yield ()
{
}
