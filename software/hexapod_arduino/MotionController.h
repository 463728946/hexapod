#ifndef MOTION_CONTROLLER_H
#define MOTION_CONTROLLER_H

#include <Adafruit_PWMServoDriver.h>
#include "motion.h"
#include "config.h"

#define SERVOMIN 125  // Minimum value, 0 deg
#define SERVOMID 350  // Middle value, 90 deg
#define SERVOMAX 575  // Maximum value, 180 deg

#define DELAY_MS 12  // Servo delay

static int left_legs[3][3] = { { 0, 1, 2 }, { 4, 5, 6 }, { 8, 9, 10 } };
static int right_legs[3][3] = { { 15, 14, 13 }, { 11, 10, 9 }, { 7, 6, 5 } };

// Offset to correct the installation error. Offset value is the number of ticks
static int left_offset_ticks[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };
static int right_offset_ticks[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };

class MotionController {
private:
  Adafruit_PWMServoDriver left_pwm = Adafruit_PWMServoDriver(0x40);
  Adafruit_PWMServoDriver right_pwm = Adafruit_PWMServoDriver(0x41);

  MotionMode current_motion = MotionMode::Mode_Standby;
  MotionMode next_motion = MotionMode::Mode_Standby;

  void posture_calibration();
  void boot_up_motion(int lut_size, int lut[][6][3]);
  void exec_motion(int lut_size, int lut[][6][3]);
  void exec_transition(int start_pos[][6][3], int start_pos_idx,
                       int end_pos[][6][3], int end_pos_idx);

public:
  void begin(int freq);
  void exec_next_motion(MotionMode cmd);
  void single_debugging(int pwm, int channel, int value, int offset);
};

inline void MotionController::begin(int freq) {
  left_pwm.begin();
  left_pwm.setPWMFreq(freq);

  right_pwm.begin();
  right_pwm.setPWMFreq(freq);

  boot_up_motion(lut_standup_length, lut_standup);
}

inline void MotionController::single_debugging(int pwm, int channel, int value, int offset) {
  if (pwm == 0) {
    left_pwm.setPWM(channel, 0, value + offset);
  } else if (pwm == 1) {
    right_pwm.setPWM(channel, 0, value + offset);
  }
}

inline void MotionController::exec_next_motion(MotionMode cmd) {
  next_motion = cmd;
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
}

inline void MotionController::posture_calibration() {
  for (int leg_idx = 0; leg_idx < 3; leg_idx++) {
    for (int joint_idx = 0; joint_idx < 3; joint_idx++) {
      right_pwm.setPWM(right_legs[leg_idx][joint_idx], 0,
                       SERVOMID + right_offset_ticks[leg_idx][joint_idx]);
      left_pwm.setPWM(left_legs[leg_idx][joint_idx], 0,
                      SERVOMID + left_offset_ticks[leg_idx][joint_idx]);
    }
  }
}

inline void MotionController::boot_up_motion(int lut_size, int lut[][6][3]) {
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

inline void MotionController::exec_motion(int lut_size, int lut[][6][3]) {
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

inline void MotionController::exec_transition(int start_pos[][6][3], int start_pos_idx, int end_pos[][6][3], int end_pos_idx) {
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

#endif