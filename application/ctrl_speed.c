#include "app_state.h"
#include "ctrl_logic.h"

Speed_t Speed_AutoAdjust(uint16_t pv, uint16_t sv)// 定量模式下自动调整速度档位
{
    if (sv == 0) return SPEED_3;
    uint16_t diff = sv - pv;

    if (diff > 5) return SPEED_1;// 阈值需要现场调试决定
    if (diff > 2) return SPEED_2;
    return SPEED_3;
}
