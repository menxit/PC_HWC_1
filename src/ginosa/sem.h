#ifdef __APPLE__
#include <dispatch/dispatch.h>
#else
#include <semaphore.h>
#endif

typedef struct pc_sem_t {
#ifdef __APPLE__
  dispatch_semaphore_t sem;
#else
  sem_t                   sem;
#endif
} pc_sem_t;

static inline long pc_sem_init(pc_sem_t *s, unsigned int value) {
#ifdef __APPLE__
  dispatch_semaphore_t *sem = &s->sem;

  *sem = dispatch_semaphore_create(value);

  return 0;
#else
  return sem_init(&s->sem, 0, value);
#endif
}

static inline long pc_sem_wait(pc_sem_t *s) {
#ifdef __APPLE__
  return dispatch_semaphore_wait(s->sem, DISPATCH_TIME_FOREVER);
#else
  return sem_wait(&s->sem);
#endif
}

static inline long pc_sem_trywait(pc_sem_t *s) {
#ifdef __APPLE__
  return dispatch_semaphore_wait(s->sem, DISPATCH_TIME_NOW);
#else
  return sem_trywait(&s->sem);
#endif
}

static inline long pc_sem_post(pc_sem_t *s) {
#ifdef __APPLE__
  return dispatch_semaphore_signal(s->sem);
#else
  return sem_post(&s->sem);
#endif
}
