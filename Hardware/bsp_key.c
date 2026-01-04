#include "bsp_key.h"
#include "gpio.h"


#define KEY_COUNT 11            // 按键数量
#define REPEAT_START_TIME  200  // 长按起始时间，200次扫描，若10ms周期，长按时间为2000ms
#define REPEAT_INTERVAL    20   // 自动连发，每100ms一次

typedef struct 
{
    GPIO_TypeDef* port;
    uint16_t pin;
} KeyMap_t;

static const KeyMap_t key_map[] = {
    [Key_Mode_Simulation]    = {GPIOC, S_Simulation_Pin},
    [Key_Mode_AutoHand]      = {GPIOC, S_Auto_Hand_Pin},
    [Key_Adjust_Up]          = {GPIOC, S_Weight__Pin},
    [Key_Adjust_Down]        = {GPIOC, S_Weight_C9_Pin},
    [Key_Supply_ON]          = {GPIOC, S_Add_Water_Pin},
    [Key_Supply_OFF]         = {GPIOC, S_Stop_Add_Pin},
    [Key_Drainage_OFF]        = {GPIOC, S_Drainage_Water_Pin},
    [Key_Drainage_ON]       = {GPIOC, S_Stop_Drainage_Pin},
    [Key_Manual_SetWay]      = {GPIOC, S_Mode_Pin},
    [Key_Manual_SetSpeed]    = {GPIOC, S_Speed_Pin},
    [Key_Manual_SetDrainDir] = {GPIOC, S_IN_EX_Pin},
};

static const KeyAction_t key_action[KEY_COUNT] = {
    [Key_Mode_AutoHand]      = KEY_ACTION_TOGGLE,
    [Key_Mode_Simulation]    = KEY_ACTION_TOGGLE,

    [Key_Adjust_Up]          = KEY_ACTION_REPEAT,
    [Key_Adjust_Down]        = KEY_ACTION_REPEAT,

    [Key_Supply_ON]          = KEY_ACTION_SIMPLE,
    [Key_Supply_OFF]         = KEY_ACTION_SIMPLE,
    [Key_Drainage_ON]        = KEY_ACTION_SIMPLE,
    [Key_Drainage_OFF]       = KEY_ACTION_SIMPLE,
    [Key_Manual_SetWay]      = KEY_ACTION_SIMPLE,
    [Key_Manual_SetDrainDir] = KEY_ACTION_SIMPLE,
    [Key_Manual_SetSpeed]    = KEY_ACTION_SIMPLE,
};

typedef struct {
    uint8_t stable_level;   // 当前稳定状态（1=松开, 0=按下）
    uint8_t last_level;     // 上一次采样值
    uint8_t counter;        // 连续一致计数，用于防抖
    KeyEvent_t event;       // 事件标志，刚按下/刚释放
    uint16_t press_time;    // 已经按下了多久
    uint16_t repeat_time;   // 距离上次重复触发的计时
} KeyState_t;

static KeyState_t key_states[KEY_COUNT];

void Key_Init(void)//按键状态初始化
{
    for (int i = 0; i < KEY_COUNT; i++)
    {
        uint8_t level = HAL_GPIO_ReadPin(key_map[i].port, key_map[i].pin);

        key_states[i].stable_level = level;
        key_states[i].last_level   = level;
				key_states[i].event        = KEY_EVENT_NONE;
        key_states[i].counter      = 0;
        key_states[i].press_time   = 0;
        key_states[i].repeat_time  = 0;
    }
}

void Key_ScanTick(void) //按键扫描周期调用 
{
    for (int i = 0; i < KEY_COUNT; i++)
    {
        KeyState_t* ks = &key_states[i];

        uint8_t level = HAL_GPIO_ReadPin(key_map[i].port, key_map[i].pin);

        if (level == ks->last_level)// 消抖 
        {
            if (ks->counter < 5)
                ks->counter++;
        } 
        else 
        {
            ks->counter = 0;
        }

        ks->last_level = level;

        if (ks->counter >= 3)  // 连续3次采样一致则认为稳定
        {
            if (level != ks->stable_level)
            {
                ks->stable_level = level;

                if (key_action[i] == KEY_ACTION_TOGGLE)
                {
                    // 自锁开关：只更新 stable_level，不产生事件
                    ks->event = KEY_EVENT_NONE;
                }
                else
                {
                    // 普通按键：产生 PRESS/RELEASE 事件
                    if (level == GPIO_PIN_RESET)
                    {
                        ks->event = KEY_EVENT_PRESS;
                        ks->press_time  = 0;
                        ks->repeat_time = 0;
                    }
                    else
                    {
                        ks->event = KEY_EVENT_RELEASE;
                    }
                }
            }

            ks->counter = 0;
        }


        if (key_action[i] == KEY_ACTION_REPEAT && ks->stable_level == GPIO_PIN_RESET)
        {
            // 如果这一轮刚刚产生了 PRESS，就先让 PRESS 先被上层消费
            // 下一轮再开始计时 repeat（体验更自然）
            if (ks->event != KEY_EVENT_PRESS)
            {
                ks->press_time++;

                if (ks->press_time == REPEAT_START_TIME)
                {
                    ks->event = KEY_EVENT_REPEAT;
                    ks->repeat_time = 0;
                }
                else if (ks->press_time > REPEAT_START_TIME)
                {
                    ks->repeat_time++;

                    if (ks->repeat_time >= REPEAT_INTERVAL)
                    {
                        ks->repeat_time = 0;
                        ks->event = KEY_EVENT_REPEAT;
                    }
                }
            }
        }
        else
        {
            // 没按住时，把计时器清掉（防止松开后再次按下接着计时）
            ks->press_time = 0;
            ks->repeat_time = 0;
        }

    }
}

bool Key_ON(Key_t key)
{
    return (key_states[key].stable_level == GPIO_PIN_RESET);// 只用于返回自锁按键状态
}

bool Key_WasPressed(Key_t key)// 下降沿触发
{
    if (key_states[key].event == KEY_EVENT_PRESS) {
        key_states[key].event = KEY_EVENT_NONE;
        return true;
    }
    return false;
}

bool Key_WasReleased(Key_t key)// 上升沿触发
{
    if (key_states[key].event == KEY_EVENT_RELEASE) {
        key_states[key].event = KEY_EVENT_NONE;
        return true;
    }
    return false;
}

bool Key_WasRepeated(Key_t key)// 连续触发
{
    if (key_states[key].event == KEY_EVENT_REPEAT) {
        key_states[key].event = KEY_EVENT_NONE;
        return true;
    }
    return false;
}
