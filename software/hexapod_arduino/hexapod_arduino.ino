#include <PS2X_lib.h>

/** PWM */
#include <Adafruit_PWMServoDriver.h>

/** I2C */
#include <Wire.h>

/** Robot Configuration */
#include "config.h"
/** Motion Path LUT */
#include "motion.h"

Adafruit_PWMServoDriver left_pwm = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver right_pwm = Adafruit_PWMServoDriver(0x41);

MotionMode current_motion = MotionMode::Mode_Standby;
MotionMode next_motion = MotionMode::Mode_Standby;

PS2X ps2x;

/**
   @brief Sets up the hexapod robot system.

   This function initializes the serial communication, sets up the WiFi access
   point, configures the Over-the-Air (OTA) update functionality, initializes
   the PWM drivers for the servos, and starts listening for UDP packets.
*/
void setup() {
  Serial.begin(115200);

  while (ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, pressures, rumble) != 0)
    ;

  // Initialize the PCA9685 library
  left_pwm.begin();
  left_pwm.setPWMFreq(60);  // Set the PWM frequency of the PCA9685

  right_pwm.begin();
  right_pwm.setPWMFreq(60);  // Set the PWM frequency of the PCA9685


  boot_up_motion(lut_standup_length, lut_standup);

  //  posture_calibration();
}

/**
   @brief Main loop of the hexapod robot program.

   This function continuously checks the `next_motion` variable to determine the
   desired motion mode. Based on the mode, it calls the `exec_motion` function
   with the appropriate LUT and length. It also handles OTA updates using
   `ArduinoOTA.handle()`.
*/
void loop() {
  ps2x.read_gamepad(false, 0);
  handlePS2Controls();

  if (next_motion == MotionMode::Mode_Walk_0) {
    exec_motion(lut_walk_0_length, lut_walk_0);
  } else if (next_motion == MotionMode::Mode_Walk_180) {
    exec_motion(lut_walk_180_length, lut_walk_180);
  } else if (next_motion == MotionMode::Mode_Walk_R45) {
    exec_motion(lut_walk_r45_length, lut_walk_r45);
  } else if (next_motion == MotionMode::Mode_Walk_R90) {
    exec_motion(lut_walk_r90_length, lut_walk_r90);
  } else if (next_motion == MotionMode::Mode_Walk_R135) {
    exec_motion(lut_walk_r135_length, lut_walk_r135);
  } else if (next_motion == MotionMode::Mode_Walk_L45) {
    exec_motion(lut_walk_l45_length, lut_walk_l45);
  } else if (next_motion == MotionMode::Mode_Walk_L90) {
    exec_motion(lut_walk_l90_length, lut_walk_l90);
  } else if (next_motion == MotionMode::Mode_Walk_L135) {
    exec_motion(lut_walk_l135_length, lut_walk_l135);
  } else if (next_motion == MotionMode::Mode_Fast_Forward) {
    exec_motion(lut_fast_forward_length, lut_fast_forward);
  } else if (next_motion == MotionMode::Mode_Fast_Backward) {
    exec_motion(lut_fast_backward_length, lut_fast_backward);
  } else if (next_motion == MotionMode::Mode_Turn_Left) {
    exec_motion(lut_turn_left_length, lut_turn_left);
  } else if (next_motion == MotionMode::Mode_Turn_Right) {
    exec_motion(lut_turn_right_length, lut_turn_right);
  } else if (next_motion == MotionMode::Mode_Climb_Forward) {
    exec_motion(lut_climb_forward_length, lut_climb_forward);
  } else if (next_motion == MotionMode::Mode_Climb_Backward) {
    exec_motion(lut_climb_backward_length, lut_climb_backward);
  } else if (next_motion == MotionMode::Mode_Rotate_X) {
    exec_motion(lut_rotate_x_length, lut_rotate_x);
  } else if (next_motion == MotionMode::Mode_Rotate_Y) {
    exec_motion(lut_rotate_y_length, lut_rotate_y);
  } else if (next_motion == MotionMode::Mode_Rotate_Z) {
    exec_motion(lut_rotate_z_length, lut_rotate_z);
  } else if (next_motion == MotionMode::Mode_Twist) {
    exec_motion(lut_twist_length, lut_twist);
  } else {
    exec_motion(lut_standby_length, lut_standby);
  }

  delay(10);  // PS2 add new
}

void handlePS2Controls() {

  if (ps2x.Button(PSB_L3)) next_motion = Mode_Standby;
  if (ps2x.Button(PSB_R3)) next_motion = Mode_Twist;
  if (ps2x.Button(PSB_TRIANGLE)) next_motion = Mode_Rotate_X;
  if (ps2x.Button(PSB_CIRCLE)) next_motion = Mode_Rotate_Y;
  if (ps2x.Button(PSB_SQUARE)) next_motion = Mode_Rotate_Z;

  int LY = ps2x.Analog(PSS_LY);
  int LX = ps2x.Analog(PSS_LX);
  int RY = ps2x.Analog(PSS_RY);
  int RX = ps2x.Analog(PSS_RX);

  // 组合控制
  if (LY < 100 && RY < 100) next_motion = Mode_Fast_Forward;
  if (LY > 150 && RY > 150) next_motion = Mode_Fast_Backward;

  // 左摇杆8方向
  if (LY < 100 && LX < 100) next_motion = Mode_Walk_L135;
  else if (LY < 100 && LX > 150) next_motion = Mode_Walk_R135;
  else if (LY > 150 && LX < 100) next_motion = Mode_Walk_L45;
  else if (LY > 150 && LX > 150) next_motion = Mode_Walk_R45;
  else if (LY < 100) next_motion = Mode_Walk_180;
  else if (LY > 150) next_motion = Mode_Walk_0;
  else if (LX < 100) next_motion = Mode_Walk_L90;
  else if (LX > 150) next_motion = Mode_Walk_R90;

  // 右摇杆
  if (RY < 100) next_motion = Mode_Climb_Forward;
  else if (RY > 150) next_motion = Mode_Climb_Backward;
  else if (RX < 100) next_motion = Mode_Turn_Left;
  else if (RX > 150) next_motion = Mode_Turn_Right;
}


/**
   @brief Calibrates the posture of the hexapod robot.

   This function sets the PWM values for each joint of the hexapod's legs to a
   neutral position, defined by SERVOMID and the offset values stored in
   right_offset_ticks and left_offset_ticks. This helps ensure that the robot
   starts in a balanced and stable posture.
*/
void posture_calibration() {
  for (int leg_idx = 0; leg_idx < 3; leg_idx++) {
    for (int joint_idx = 0; joint_idx < 3; joint_idx++) {
      right_pwm.setPWM(right_legs[leg_idx][joint_idx], 0,
                       SERVOMID + right_offset_ticks[leg_idx][joint_idx]);
      left_pwm.setPWM(left_legs[leg_idx][joint_idx], 0,
                      SERVOMID + left_offset_ticks[leg_idx][joint_idx]);
    }
  }
}

void boot_up_motion(int lut_size, int lut[][6][3]) {
  for (int leg_idx = 0; leg_idx < 3; leg_idx++) {
    for (int joint_idx = 0; joint_idx < 3; joint_idx++) {
      right_pwm.setPWM(right_legs[leg_idx][joint_idx], 0,
                       lut[0][leg_idx][joint_idx] + right_offset_ticks[leg_idx][joint_idx]);
      delay(50);
      left_pwm.setPWM(left_legs[leg_idx][joint_idx], 0,
                      lut[0][leg_idx + 3][joint_idx] + left_offset_ticks[leg_idx][joint_idx]);
      delay(50);
    }
  }

  for (int lut_idx = 0; lut_idx < lut_size; lut_idx++) {
    for (int leg_idx = 0; leg_idx < 3; leg_idx++) {
      for (int joint_idx = 0; joint_idx < 3; joint_idx++) {
        right_pwm.setPWM(right_legs[leg_idx][joint_idx], 0,
                         lut[lut_idx][leg_idx][joint_idx] + right_offset_ticks[leg_idx][joint_idx]);
        left_pwm.setPWM(left_legs[leg_idx][joint_idx], 0,
                        lut[lut_idx][leg_idx + 3][joint_idx] + left_offset_ticks[leg_idx][joint_idx]);
      }
    }
    delay(DELAY_MS);
  }
}

/**
   @brief Executes a motion sequence defined by a lookup table (LUT).

   This function iterates through the provided LUT, setting the PWM values for
   each joint of the hexapod's legs. It handles transitions between different
   motion modes and ensures smooth movement.

   @param lut_size The size of the LUT.
   @param lut The LUT containing the PWM values for each joint at each step of
   the motion.
*/
void exec_motion(int lut_size, int lut[][6][3]) {
  int mid_step = (int)(lut_size / 2);
  if (current_motion == MotionMode::Mode_Standby) {
    exec_transition(lut_standby, 0, lut, 0);
  }
  current_motion = next_motion;

  for (int lut_idx = 0; lut_idx < lut_size; lut_idx++) {
    for (int leg_idx = 0; leg_idx < 3; leg_idx++) {
      for (int joint_idx = 0; joint_idx < 3; joint_idx++) {
        right_pwm.setPWM(right_legs[leg_idx][joint_idx], 0,
                         lut[lut_idx][leg_idx][joint_idx] + right_offset_ticks[leg_idx][joint_idx]);
        left_pwm.setPWM(left_legs[leg_idx][joint_idx], 0,
                        lut[lut_idx][leg_idx + 3][joint_idx] + left_offset_ticks[leg_idx][joint_idx]);
      }
    }

    if (mid_step > 0) {
      if (lut_idx % mid_step == 0 && current_motion != next_motion) {
        exec_transition(lut, lut_idx, lut_standby, 0);
        delay(DELAY_MS);
        break;
      }
    }
    delay(DELAY_MS);
  }
}

/**
   @brief Executes a smooth transition between two motion positions.

   This function takes two motion positions (start_pos and end_pos) and their
   respective indices in their respective LUTs. It calculates the difference
   between the start and end positions for each joint and determines the number
   of steps required for a smooth transition. The function then iterates through
   these steps, adjusting the PWM values for each joint to gradually move from
   the start position to the end position.

   @param start_pos The starting position LUT.
   @param start_pos_idx The index of the starting position in the start_pos LUT.
   @param end_pos The ending position LUT.
   @param end_pos_idx The index of the ending position in the end_pos LUT.
*/
void exec_transition(int start_pos[][6][3], int start_pos_idx,
                     int end_pos[][6][3], int end_pos_idx) {
  int tick_step = 6;
  int max_step = 0;
  int signed_ticks[6][3];

  int current_pos[6][3];
  int diff;

  for (int leg_idx = 0; leg_idx < 6; leg_idx++) {
    for (int joint_idx = 0; joint_idx < 3; joint_idx++) {
      diff = end_pos[end_pos_idx][leg_idx][joint_idx] - start_pos[start_pos_idx][leg_idx][joint_idx];
      current_pos[leg_idx][joint_idx] =
        start_pos[start_pos_idx][leg_idx][joint_idx];
      if (diff < 0) {
        signed_ticks[leg_idx][joint_idx] = -tick_step;
      } else {
        signed_ticks[leg_idx][joint_idx] = tick_step;
      }
      max_step = max(max_step, abs(diff));
    }
  }
  max_step = ceil(max_step / tick_step);
  for (int step_idx = 0; step_idx < max_step; step_idx++) {
    for (int leg_idx = 0; leg_idx < 3; leg_idx++) {
      for (int joint_idx = 0; joint_idx < 3; joint_idx++) {
        if (abs(current_pos[leg_idx][joint_idx] - end_pos[end_pos_idx][leg_idx][joint_idx]) > tick_step) {
          current_pos[leg_idx][joint_idx] = current_pos[leg_idx][joint_idx] + signed_ticks[leg_idx][joint_idx];
        } else {
          current_pos[leg_idx][joint_idx] =
            end_pos[end_pos_idx][leg_idx][joint_idx];
        }

        if (abs(current_pos[leg_idx + 3][joint_idx] - end_pos[end_pos_idx][leg_idx + 3][joint_idx]) > tick_step) {
          current_pos[leg_idx + 3][joint_idx] =
            current_pos[leg_idx + 3][joint_idx] + signed_ticks[leg_idx + 3][joint_idx];
        } else {
          current_pos[leg_idx + 3][joint_idx] =
            end_pos[end_pos_idx][leg_idx + 3][joint_idx];
        }

        right_pwm.setPWM(right_legs[leg_idx][joint_idx], 0,
                         current_pos[leg_idx][joint_idx] + right_offset_ticks[leg_idx][joint_idx]);
        left_pwm.setPWM(left_legs[leg_idx][joint_idx], 0,
                        current_pos[leg_idx + 3][joint_idx] + left_offset_ticks[leg_idx][joint_idx]);
      }
    }
  }
}
