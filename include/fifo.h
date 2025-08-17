#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>

typedef struct {
    atomic_uint_least32_t    val;
    atomic_uint_least32_t    flag;
} fifo_idx_t;

#define declare_fifo(type)                                                              \
                                                                                        \
typedef struct {                                                                        \
    fifo_idx_t  head;                                                                   \
    fifo_idx_t  tail;                                                                   \
    size_t      size;                                                                   \
    type   elem[];                                                                      \
} fifo_##type##_t;


#define implement_fifo(type)                                                            \
                                                                                        \
static inline bool type##_fifo_init(fifo_##type##_t** q, size_t num) {                  \
    static const fifo_idx_t null_idx = { .val=0, .flag=0 };                             \
    size_t ms = sizeof(fifo_##type##_t)+num*sizeof(type);                               \
    (*q)=malloc(ms);                                                                    \
    if (NULL==(*q)) return false;                                                       \
    (*q)->head = null_idx;                                                              \
    (*q)->tail = null_idx;                                                              \
    (*q)->size = num;                                                                   \
    return true;                                                                        \
}                                                                                       \
                                                                                        \
static inline void type##_fifo_init_at(fifo_##type##_t* q, size_t num) {                \
    static const fifo_idx_t null_idx = { .val=0, .flag=0 };                             \
    q->head = null_idx;                                                                 \
    q->tail = null_idx;                                                                 \
    q->size = num;                                                                      \
}                                                                                       \
                                                                                        \
                                                                                        \
static inline void type##_fifo_release(fifo_##type##_t* q) {                            \
    free(q);                                                                            \
}                                                                                       \
                                                                                        \
static inline bool type##_next( fifo_##type##_t* q, fifo_idx_t* idx ) {                 \
    uint32_t val = atomic_load(&idx->val);                                              \
    uint32_t flag = atomic_load(&idx->flag);                                            \
    if ( val+1>=q->size ) {                                                             \
        val = 0;                                                                        \
        flag ^= 1;                                                                      \
        atomic_store(&idx->val, val);                                                   \
        atomic_store(&idx->flag, flag);                                                 \
        return true;                                                                    \
    }                                                                                   \
    else {                                                                              \
        atomic_store(&idx->val, val + 1);                                               \
        return false;                                                                   \
    }                                                                                   \
}                                                                                       \
                                                                                        \
static inline bool type##_full( fifo_##type##_t* q ) {                                  \
    uint32_t hval = atomic_load(&q->head.val);                                          \
    uint32_t tval = atomic_load(&q->tail.val);                                          \
    uint32_t hflag = atomic_load(&q->head.flag);                                        \
    uint32_t tflag = atomic_load(&q->tail.flag);                                        \
    return (hval == tval && hflag != tflag);                                            \
}                                                                                       \
                                                                                        \
static inline bool type##_empty( fifo_##type##_t* q ) {                                 \
    uint32_t hval = atomic_load(&q->head.val);                                          \
    uint32_t tval = atomic_load(&q->tail.val);                                          \
    uint32_t hflag = atomic_load(&q->head.flag);                                        \
    uint32_t tflag = atomic_load(&q->tail.flag);                                        \
    return (hval == tval && hflag == tflag);                                            \
}                                                                                       \
                                                                                        \
static inline type* type##_head( fifo_##type##_t* q ) {                                 \
    if ( type##_empty(q) ) return NULL;                                                 \
    return &q->elem[q->head.val];                                                       \
}                                                                                       \
                                                                                        \
static inline type* type##_tail( fifo_##type##_t* q ) {                                 \
    if ( type##_empty(q) ) return NULL;                                                 \
    return &q->elem[q->tail.val];                                                       \
}                                                                                       \
                                                                                        \
static inline bool type##_push( fifo_##type##_t* q, type* data) {                       \
    if (!type##_full(q)) {                                                              \
        q->elem[q->head.val] = *data;                                                   \
        type##_next(q,&q->head);                                                        \
        return true;                                                                    \
    }                                                                                   \
    return false;                                                                       \
}                                                                                       \
                                                                                        \
static inline bool type##_pop( fifo_##type##_t* q ) {                                   \
    if ( !type##_empty(q) ) {                                                           \
        type##_next(q, &q->tail);                                                       \
        return true;                                                                    \
    }                                                                                   \
    return false;                                                                       \
}

#endif // __FIFO_H__
