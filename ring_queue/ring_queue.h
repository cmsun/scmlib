#ifndef _RING_QUEUE_H_
#define _RING_QUEUE_H_

#include <stdint.h>

typedef struct 
{
    uint8_t *buff;      //缓存数据的数组地址
    uint32_t capacity;  //缓存数据的数组容量
    uint32_t count;     //入栈的数据计数
    uint32_t front;     //新数据入栈的位置
    uint32_t rear;      //数据出栈的位置
} RingQueue_TypeDef;

void rq_init(RingQueue_TypeDef *rq, uint8_t *buff, uint32_t size);
uint32_t rq_push(RingQueue_TypeDef *rq, uint8_t byte);
uint32_t rq_push_bytes(RingQueue_TypeDef *rq, const uint8_t *bytes, uint32_t n);
uint32_t rq_pop_bytes(RingQueue_TypeDef *rq, uint32_t n);
uint32_t rq_read_only(RingQueue_TypeDef *rq, uint8_t *buff, uint32_t n);
uint32_t rq_read_bytes(RingQueue_TypeDef *rq, uint8_t *buff, uint32_t n);
char *rq_read_string(RingQueue_TypeDef *rq, uint8_t *buff, uint32_t size);
static inline uint32_t rq_data_size(RingQueue_TypeDef *rq) { return rq->count; }
static inline uint32_t rq_remain_space(RingQueue_TypeDef *rq) { return rq->capacity - rq->count; }

#endif