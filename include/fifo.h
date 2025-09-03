#ifndef __FIFO_H__
#define __FIFO_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdatomic.h>

//typedef struct {
//    atomic_uint_least32_t    val;
//    atomic_uint_least32_t    flag;
//} fifo_idx_t;

typedef atomic_uint_least32_t   fifo_idx_t;

#define declare_fifo(type)                                                              \
                                                                                        \
typedef struct {                                                               \
    fifo_idx_t  head;                                                                   \
    fifo_idx_t  tail;                                                                   \
    size_t      size;                                                                   \
    type   elem[];                                                                      \
} fifo_##type##_t;


#define implement_fifo(type)                                                            \
                                                                                        \
static inline bool type##_fifo_init(fifo_##type##_t** q, size_t num) {                  \
    static const fifo_idx_t null_idx = 0;                                               \
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
    static const fifo_idx_t null_idx = 0;                                               \
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
static inline fifo_idx_t type##_next( fifo_##type##_t* q, fifo_idx_t val ) {            \
    val = (val+1) % q->size;                                                            \
    return val;                                                                         \
}                                                                                       \
                                                                                        \
static inline bool type##_full( fifo_##type##_t* q ) {                                  \
    volatile uint32_t hval = atomic_load(&q->head);                                     \
    volatile uint32_t tval = atomic_load(&q->tail);                                     \
    volatile uint32_t hnext=type##_next(q,hval);                                        \
    return (hnext == tval);                                                             \
}                                                                                       \
                                                                                        \
static inline bool type##_empty( fifo_##type##_t* q ) {                                 \
    volatile uint32_t hval = atomic_load(&q->head);                                     \
    volatile uint32_t tval = atomic_load(&q->tail);                                     \
    return (hval == tval);                                                              \
}                                                                                       \
                                                                                        \
static inline type* type##_head( fifo_##type##_t* q ) {                                 \
    volatile uint32_t head = atomic_load(&q->head);                                     \
    if ( type##_empty(q) ) return NULL;                                                 \
    return &q->elem[head];                                                              \
}                                                                                       \
                                                                                        \
static inline type* type##_tail( fifo_##type##_t* q ) {                                 \
    volatile uint32_t tail = atomic_load(&q->tail);                                     \
    if ( type##_empty(q) ) return NULL;                                                 \
    return &q->elem[tail];                                                              \
}                                                                                       \
                                                                                        \
static inline bool type##_push( fifo_##type##_t* q, type* data) {                       \
    if (!type##_full(q)) {                                                              \
        volatile uint32_t head=atomic_load(&q->head);                                   \
        q->elem[head] = *data;                                                          \
        atomic_store(&q->head, type##_next(q,head));                                    \
        return true;                                                                    \
    }                                                                                   \
    return false;                                                                       \
}                                                                                       \
                                                                                        \
static inline bool type##_pop( fifo_##type##_t* q ) {                                   \
    if ( !type##_empty(q) ) {                                                           \
        volatile uint32_t tail=atomic_load(&q->tail);                                   \
        atomic_store(&q->tail, type##_next(q,tail));                                    \
        return true;                                                                    \
    }                                                                                   \
    return false;                                                                       \
}                                                                                       \

#endif // __FIFO_H__
