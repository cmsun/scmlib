#include "configure.h"
#include "tick.h"
#include "key.h"

KeyDataStruct_TypeDef key = {
    .state = KEY_STATE_RELEASE,
    .press_timestamp = 0,
    .click_valid = false,
    .action = KEY_ACTION_NONE,
};

void key_init(void)
{
    _GPIO_Init_InPut(KEY_GPIO_PORT, KEY_GPIO_PIN);
}

static inline KEY_STATE key_temp_state(void)
{
    KEY_STATE temp;
    static uint32_t tick = 0;
    static KEY_STATE state = KEY_STATE_RELEASE;
    
    temp = (KEY_STATE)(_GPIO_ReadInputPin(KEY_GPIO_PORT, KEY_GPIO_PIN) != 0);
    if(state != temp)
    {
        if(tick_value() - tick >= 5)
        {
            tick = tick_value();
            state = temp;
        }
    }
    else
    {
        tick = tick_value();
    }
        
    return state;
}

static inline void key_clear(void)
{
    key.click_valid = false;
    key.action = KEY_ACTION_NONE;
}

static void key_judge(void)
{
    if(key.action == KEY_ACTION_NONE)                   //判断单击动作（此时单击动作无效），判断长按动作
    {
        if(key.state == KEY_STATE_RELEASE)              //松开状态
        {
            if(key_temp_state() == KEY_STATE_PRESS)     //松开状态变成按下状态
            {
                key.state = KEY_STATE_PRESS;
                key.press_timestamp = tick_value();     //记录按下的时间
            }
        }
        else if(key.state == KEY_STATE_PRESS)           //按下状态
        {
            if(key_temp_state() == KEY_STATE_RELEASE)   //按下状态变成松开状态
            {
                key.state = KEY_STATE_RELEASE;
                if(tick_value() - key.press_timestamp >= KEY_CLICK_TRIGGER)//按下状态持续大于30毫秒，判断为单击动作（单击动作无效）
                    key.action = KEY_ACTION_CLICK;
            }
            else if(tick_value() - key.press_timestamp >= KEY_LONG_PRESS_TRIGGER)//按下状态持续2秒钟，判断为长按动作
            {
                key.state = KEY_STATE_RELEASE;
                key.action = KEY_ACTION_LONG_PRESS;
            }
        }
    }
    else if(key.action == KEY_ACTION_CLICK && !key.click_valid)//判断单击动作是否有效、判断双击动作、判断长按动作
    {
        if(key.state == KEY_STATE_RELEASE)              //松开状态
        {
            if(key_temp_state() == KEY_STATE_PRESS)     //松开状态变成按下状态
            {
                key.state = KEY_STATE_PRESS;
                key.press_timestamp = tick_value();     //记录按下的时间
            }
            else if(tick_value() - key.press_timestamp >= KEY_CLICK_VALID)//单击之后超过500毫秒没有其它操作，则单击动作有效
            {
                key.click_valid = true;
            }
        }
        else if(key.state == KEY_STATE_PRESS)           //按下状态
        {
            if(key_temp_state() == KEY_STATE_RELEASE)   //松开状态
            {
                key.state = KEY_STATE_RELEASE;
                if(tick_value() - key.press_timestamp >= KEY_CLICK_TRIGGER)//单击后再按下持续大于30毫秒然后放开，判断为双击动作
                    key.action = KEY_ACTION_DOUBLE_CLICK;
            }
            else if(tick_value() - key.press_timestamp >= KEY_LONG_PRESS_TRIGGER)//虽然已经单击但是单击生效前再按下持续2秒钟，判断为长按动作
            {
                key.state = KEY_STATE_RELEASE;
                key.action = KEY_ACTION_LONG_PRESS;
            }
        }
    }
}

void key_task_loop(void)
{
    static uint32_t tick = 0;
    static uint32_t timeout = 0;
    if(tick_value() - tick > 5)
    {
        tick = tick_value();
        key_judge();
        
        //一秒钟内没有读取按键值，则清除按键值
        if(key.action == KEY_ACTION_NONE)
            timeout = tick;
        else if(tick - timeout > 1000)
            key_clear();
    }
}

KEY_ACTION key_action(void)
{
    KEY_ACTION action = KEY_ACTION_NONE;
    if(key.action == KEY_ACTION_CLICK)  //单击有效时才会返回单击动作
    {
        if(key.click_valid)
        {
            action = KEY_ACTION_CLICK;
            key_clear();
        }
    }
    else                                //其它按键动作直接有效
    {
        action = key.action;
        key_clear();
    }
    return action;
}