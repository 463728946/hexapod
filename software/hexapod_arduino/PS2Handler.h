
#ifndef PS2HANDLER_H
#define PS2HANDLER_H

#include <PS2X_lib.h>
#include "config.h"

class PS2Handler
{
public:
    void begin(int clk, int cmd, int sel, int dat);
    MotionMode convertMotionCommand();

private:
    PS2X ps2x;
};

inline void
PS2Handler::begin(int clk, int cmd, int sel, int dat)
{
    int error = ps2x.config_gamepad(clk, cmd, sel, dat, false, false);
    if (error == 3)
        Serial.println("Controller refusing to enter Pressures mode, may not support it. ");
    else if (error == 2)
        Serial.println("Controller found but not accepting commands. see readme.txt to enable debug. Visit www.billporter.info for troubleshooting tips");
    else if (error == 1)
        Serial.println("No controller found, check wiring, see readme.txt to enable debug. visit www.billporter.info for troubleshooting tips");
    else if (error == 0)
    {
        byte type = ps2x.readType();
        switch (type)
        {
        case 0:
            Serial.print("Unknown Controller type found ");
            break;
        case 1:
            Serial.print("DualShock Controller found ");
            break;
        case 2:
            Serial.print("GuitarHero Controller found ");
            break;
        case 3:
            Serial.print("Wireless Sony DualShock Controller found ");
            break;
        }
    }
    else
    {
        Serial.println("config_gamepad error");
    }
}

inline MotionMode PS2Handler::convertMotionCommand()
{
    if (ps2x.Button(PSB_L3))
        return Mode_Standby;
    if (ps2x.Button(PSB_R3))
        return Mode_Twist;
    if (ps2x.Button(PSB_TRIANGLE))
        return Mode_Rotate_X;
    if (ps2x.Button(PSB_CIRCLE))
        return Mode_Rotate_Y;
    if (ps2x.Button(PSB_SQUARE))
        return Mode_Rotate_Z;

    int LY = ps2x.Analog(PSS_LY);
    int LX = ps2x.Analog(PSS_LX);
    int RY = ps2x.Analog(PSS_RY);
    int RX = ps2x.Analog(PSS_RX);

    // 快速前进/后退
    if (LY < 100 && RY < 100)
        return Mode_Fast_Forward;
    if (LY > 150 && RY > 150)
        return Mode_Fast_Backward;

    // 左摇杆8方向
    if (LY < 100 && LX < 100)
        return Mode_Walk_L135;
    if (LY < 100 && LX > 150)
        return Mode_Walk_R135;
    if (LY > 150 && LX < 100)
        return Mode_Walk_L45;
    if (LY > 150 && LX > 150)
        return Mode_Walk_R45;
    if (LY < 100)
        return Mode_Walk_180;
    if (LY > 150)
        return Mode_Walk_0;
    if (LX < 100)
        return Mode_Walk_L90;
    if (LX > 150)
        return Mode_Walk_R90;

    // 右摇杆
    if (RY < 100)
        return Mode_Climb_Forward;
    if (RY > 150)
        return Mode_Climb_Backward;
    if (RX < 100)
        return Mode_Turn_Left;
    if (RX > 150)
        return Mode_Turn_Right;

    return Mode_Standby;
}

#endif