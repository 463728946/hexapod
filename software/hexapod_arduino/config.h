/**

  Configuration file

  - Copyright (C) 2024 - PRESENT  rookidroid.com
  - E-mail: info@rookidroid.com
  - Website: https://rookidroid.com/

                        **
                       ****
                        **
                        **
                        **
                        **

        **********************************
      **************************************
     ****************************************
     ********      ************      ********
     *******        **********        *******
     *******        **********        *******
     ********      ************      ********
     ****************************************
     ****************************************
     ****************************************
     ****************************************


            **************************

                ******************

*/

#ifndef CONFIG_H
#define CONFIG_H

#define SERVOMIN 125 // Minimum value, 0 deg
#define SERVOMID 350 // Middle value, 90 deg
#define SERVOMAX 575 // Maximum value, 180 deg

#define DELAY_MS 12 // Servo delay

/** Motion Mode */
enum MotionMode
{
  Mode_Standby,
  Mode_Walk_0,
  Mode_Walk_180,
  Mode_Walk_R45,
  Mode_Walk_R90,
  Mode_Walk_R135,
  Mode_Walk_L45,
  Mode_Walk_L90,
  Mode_Walk_L135,
  Mode_Fast_Forward,
  Mode_Fast_Backward,
  Mode_Turn_Left,
  Mode_Turn_Right,
  Mode_Climb_Forward,
  Mode_Climb_Backward,
  Mode_Rotate_X,
  Mode_Rotate_Y,
  Mode_Rotate_Z,
  Mode_Twist,
};

// Servo connections to the PCA9685 driver
// {{leg1_join1, leg1_join2, leg1_join3},
//  {leg2_join1, leg2_join2, leg2_join3},
//  {leg3_join1, leg3_join2, leg3_join3}}
static int left_legs[3][3] = {{0, 1, 2}, {4, 5, 6}, {8, 9, 10}};
static int right_legs[3][3] = {{15, 14, 13}, {11, 10, 9}, {7, 6, 5}};

// Offset to correct the installation error. Offset value is the number of ticks
static int left_offset_ticks[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
static int right_offset_ticks[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

/** PS2 Configurations */
#define PS2_DAT 19
#define PS2_CMD 23
#define PS2_SEL 5
#define PS2_CLK 18
#define pressures false
#define rumble false

#endif // CONFIG_H
