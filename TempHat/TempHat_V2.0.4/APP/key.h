#ifndef __KEY_H
#define __KEY_H

#include <stdint.h>
#include <stdbool.h>

#define KEY_CLICK_TRIGGER       50      //按下50毫秒触发单击动作
#define KEY_CLICK_VALID         300     //单击后300毫秒内没有其它动作，则单击动作有效
#define KEY_LONG_PRESS_TRIGGER  2000    //按下2秒触发长按动作

typedef enum {
    KEY_STATE_PRESS = 0,
    KEY_STATE_RELEASE,
} KEY_STATE;

typedef enum {
    KEY_ACTION_CLICK,
    KEY_ACTION_DOUBLE_CLICK,
    KEY_ACTION_LONG_PRESS,
    KEY_ACTION_NONE
} KEY_ACTION;

typedef struct KeyDataStruct_TypeDef {
    KEY_STATE state;
    uint32_t press_timestamp;
    bool click_valid;
    KEY_ACTION action;
} KeyDataStruct_TypeDef;

void key_init(void);
void key_task_loop(void);
KEY_ACTION key_action(void);

#endif