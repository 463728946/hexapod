#include "PS2Handler.h"
#include "MotionController.h"
#include "config.h"

#define PS2_DAT 19
#define PS2_CMD 23
#define PS2_SEL 5
#define PS2_CLK 18

MotionController motionController;
PS2Handler ps2;

PS2X ps2x;

void setup()
{
  Serial.begin(115200);
  motionController.begin(60);
  ps2.begin(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT);
}

void loop()
{
  MotionMode cmd = ps2.convertMotionCommand();

  motionController.exec_next_motion(cmd);

  delay(10); // PS2 add new
}
