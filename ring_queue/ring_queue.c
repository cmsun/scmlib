#include <string.h>
#include "ring_queue.h"
#include "utility.h"

void rq_init(RingQueue_TypeDef *rq, uint8_t *buff, uint32_t size)
{
    rq->buff = buff;
    rq->capacity = size;
    rq->count = 0;
    rq->front = 0;
    rq->rear = 0;
}

uint32_t rq_push(RingQueue_TypeDef *rq, uint8_t byte)
{
    uint32_t byte2push = 0;
    if(rq->count < rq->capacity)
    {
        rq->buff[rq->front++] = byte;
        rq->front %= rq->capacity;
        rq->count++;
        byte2push = 1;
    }
    return byte2push;
}

uint32_t rq_push_bytes(RingQueue_TypeDef *rq, const uint8_t *bytes, uint32_t n)
{
    uint32_t byte2push = 0;
    uint32_t copy1st, copy2nd;
    
    if(rq->count < rq->capacity)
    {        
        byte2push = min(rq_remain(rq), n);
        
        if(rq->front + byte2push <= rq->capacity)
        {
            memcpy(rq->buff + rq->front, bytes, byte2push);
        }
        else
        {
            copy1st = rq->capacity - rq->front;
            copy2nd = byte2push - copy1st;
            memcpy(rq->buff + rq->front, bytes, copy1st);
            memcpy(rq->buff, bytes + copy1st, copy2nd);
        }
        
        rq->count += byte2push;
        rq->front += byte2push;
        rq->front %= rq->capacity;
    }
    
    return byte2push;
}

uint32_t rq_pop_bytes(RingQueue_TypeDef *rq, uint32_t n)
{
    uint32_t byte2pop = 0;
    if(rq->count > 0)
    {
        byte2pop = min(rq->count, n);
        rq->rear += byte2pop;
        rq->rear %= rq->capacity;
        rq->count -= byte2pop;
    }
    return byte2pop;
}

uint32_t rq_read_only(RingQueue_TypeDef *rq, uint8_t *buff, uint32_t n)
{
    uint32_t bytes2read = 0;
    uint32_t copy1st, copy2nd;
    
    if(rq->count > 0)
    {        
        bytes2read = min(rq->count, n);
        
        if(rq->front > rq->rear || rq->capacity - rq->rear >= bytes2read)
        {
            memcpy(buff, rq->buff + rq->rear, bytes2read);
        }
        else
        {
            copy1st = rq->capacity - rq->rear;
            copy2nd = bytes2read - copy1st;
            memcpy(buff, rq->buff + rq->rear, copy1st);
            memcpy(buff + copy1st, rq->buff, copy2nd);
        }
    }
    
    return bytes2read;
}

uint32_t rq_read_bytes(RingQueue_TypeDef *rq, uint8_t *buff, uint32_t n)
{
    uint32_t cnt = 0;
    cnt = rq_read_only(rq, buff, n);
    if(cnt > 0)
    {
        rq_pop_bytes(rq, cnt);
    }
    return cnt;
}

//如果找到换行符\n则将字符串复制到传入的buff里，并返回传入的buff指针。
//如果缓存空间已经填满但是没找到换行符，则将数据复制到传入的buff里，然后清空队列并返回传入的buff指针。
//如果队列里有数据但是没有找到换行符\n则返回空指针，队列数据不变。
//如果队列里第一个字符串长度比传入的buff空间要大，则复制部分字符串填满传入的buff并加上尾零，清除掉队列里的第一个字符串。
char *rq_read_string(RingQueue_TypeDef *rq, uint8_t *buff, uint32_t size)
{
    uint32_t index, bytes2read = 0;
    char *straddr = NULL;

    if(rq->count > 0)
    {
        index = rq->rear;
        while(index != rq->front || bytes2read == 0)
        {
            bytes2read++;
            if(rq->buff[index] == '\n' || bytes2read == rq->capacity)
            {
                rq_read_only(rq, buff, min(bytes2read, size));
                rq_pop_bytes(rq, bytes2read);
                buff[bytes2read<size ? bytes2read : size-1] = '\0';
                straddr = buff;
                break;
            }
            index++;
            index %= rq->capacity;
        }
    }

    return straddr;
}

