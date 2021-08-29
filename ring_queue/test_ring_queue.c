#include <string.h>
#include <stdio.h>
#include "ring_queue.h"
#include "utility.h"

RingQueue_TypeDef RingQueue;
char QueueBuff[200] = {0};

char str[][64] = {
    "123456789\r\n",
    "11111111111\r\n",
    "abc\r\n",
    "333333333333333333333333333\r\n",
    "aaaaaaaaaaaaaaaaaaa\r\n",
    "bbbbbbbbbbbbbbbbbbbbbbbbb"
};

int main(int argc, char *argv[])
{
    char buff[1024];

    rq_init(&RingQueue, QueueBuff, sizeof(QueueBuff));
    rq_push_bytes(&RingQueue, str[0], strlen(str[0]));
    rq_push_bytes(&RingQueue, str[1], strlen(str[1]));
    for(int i = 2; i < members(str); ++i)
    {
        for(int j = 0; j < strlen(str[i]); ++j)
            rq_push(&RingQueue, str[i][j]);
    }

    while(rq_count(&RingQueue) > 0)
    {
        if(rq_read_string(&RingQueue, buff, sizeof(buff)) != NULL)
        {
            printf("%s", buff);
        }
        else
        {
            printf("还有数据，但是没有换行符\r\n");
            break;
        }
    }
    printf("------------------------\r\n");
    rq_push_bytes(&RingQueue, str[2], strlen(str[2]));
    printf("%s", rq_read_string(&RingQueue, buff, sizeof(buff)));

    return 0;
}