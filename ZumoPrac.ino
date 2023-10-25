#include "zumo_state.h"

#define SPEED_MAX 200
#define SPEED_HALT 0

/* -----------STATE ENCODING---------- */

// These defines define the state encoding - a mapping from a name to a numerical code
// Add new state definitions by copying the lines here and incrementing the number by one.
// Note: make sure each encoding is unique

#define STATE_FORWARD 0
#define STATE_LEFT 1
#define STATE_RIGHT 2
#define STATE_HALT 3

#define FINISH_MODE 0
#define LINE_FOLLOW_MODE 1
#define JUMP_GAP_MODE 2
// #define WALL_FOLLOW_MODE 3
// #define ROTATE_PUSH_MODE 4

#define ROTATE_DELAY 750  // milliseconds to rotate once a line is found before resuming line following.
#define FORWARDS_DELAY 500 // milliseconds to drive forward after finding the line before rotating.

/* ---------END STATE ENCODING---------- */

// Create a variable to hold a state.  This should be set to the default state.
int state = STATE_FORWARD;

// Create a variable to hold the next state.  This should also be set to the default state.
int next_state = STATE_FORWARD;

uint8_t mode[] = {LINE_FOLLOW_MODE, JUMP_GAP_MODE, LINE_FOLLOW_MODE, FINISH_MODE};
uint8_t current_mode_num = 0;
int sensor_total = 0;

// motor output States
void stateForward()
{
  Serial.println("forward");
  Serial0.println("forward");
  motors.setSpeeds(SPEED_MAX, SPEED_MAX);
}

void stateLeft()
{
  Serial.println("left");
  Serial0.println("left");
  motors.setSpeeds(SPEED_HALT, SPEED_MAX * 2);
}

void stateRight()
{
  Serial.println("right");
  Serial0.println("right");
  motors.setSpeeds(SPEED_MAX * 2, SPEED_HALT);
}

void stateHalt()
{
  Serial.println("halt");
  Serial0.println("halt");
  motors.setSpeeds(SPEED_HALT, SPEED_HALT);
}

/* -------END STATE IMPLEMENTATION---------- */

/* -----------STATE CHOICE---------- */

void selectState()
{
  switch (mode[current_mode_num])
  {
  case LINE_FOLLOW_MODE:
    line_follow();
    break;
  case JUMP_GAP_MODE:
    jump_gap();
    break;
  // case WALL_FOLLOW_MODE:
  //   wall_follow();
  //   break;
  // case ROTATE_PUSH_MODE:
  //   break; // TODO: implement rotate push mode
  case FINISH_MODE:
    finish();
    break;

  default:
    finish();
    break;
  }

  current_mode_num++; // increment to the next mode after each mode is completed.
}

void setMotorState(int _state)
{
  switch (_state)
  {
  case STATE_FORWARD:
    stateForward();
    break;

  case STATE_LEFT:
    stateLeft();
    break;

  case STATE_RIGHT:
    stateRight();
    break;

  case STATE_HALT:
    stateHalt();
    break;

  default:
    stateHalt();
  }
}

// Modes

void finish()
{
  Serial0.print("Finish - ");
  setMotorState(STATE_HALT);
  Serial0.println("Finished!!!");
  while (true)
  {
  } // do nothing
}

void line_follow()
{
  bool stop_flag = false; // if no line or a stop line is detected, set to true to exit loop.
  state = STATE_FORWARD;

  while (!stop_flag)
  {
    Serial0.print("line follow - ");
    // Serial0.print(position);
    // Serial0.print(" - ");
    position = reflectanceSensors.readLine(sensors);
    sensor_total = (sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + sensors[5]);
    if (state == STATE_FORWARD)
    {
      if (position < 1000)
      {
        next_state = STATE_LEFT;
      }
      else if (position > 4000)
      {
        next_state = STATE_RIGHT;
      }
      else
        next_state = state; // set the next state the same as the current state
    }
    else if (state == STATE_RIGHT || state == STATE_LEFT)
    {
      if (position < 3500 && position > 1500)
      {

        next_state = STATE_FORWARD;
      }
      else
        next_state = state; // set the next state the same as the current state
    }

    if (sensor_total < 500 || sensor_total > 3000)
    {
      stop_flag = true;
      Serial0.println("No line detected, move to next state.");
      next_state = STATE_HALT;
    }
    state = next_state;
    setMotorState(state); // select/set motor output states
  }
}

// Jump Gap mode
void jump_gap()
{
  setMotorState(STATE_FORWARD);
  bool line_detected = false;
  while (!line_detected)
  {

    Serial0.print("jump gap - ");
    position = reflectanceSensors.readLine(sensors);
    sensor_total = (sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + sensors[5]);
    if (sensor_total > 500)
    {
      line_detected = true;
    }
    else
    {
      setMotorState(STATE_FORWARD);
    }
  }

  // motors.setSpeeds(SPEED_MAX, SPEED_MAX);
  // delay(FORWARDS_DELAY);

  // potential alternative to above, drive forwards until driving past line, then rotate.
  sensor_total = (sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + sensors[5]);
  while (sensor_total > 500)
  {
    position = reflectanceSensors.readLine(sensors);
    sensor_total = (sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + sensors[5]);
    setMotorState(STATE_FORWARD);
  }
  // Serial0.println(" past line");

  motors.setSpeeds(-SPEED_MAX, SPEED_MAX);
  delay(ROTATE_DELAY);
  setMotorState(STATE_FORWARD);
  delay(FORWARDS_DELAY);
  setMotorState(STATE_HALT); // line detected, stop and exit.
}

// void wall_follow()
// {
//   // get initial sensor readings...
//   position = reflectanceSensors.readLine(sensors);
//   bool line_detected = false;
//   while (!line_detected)
//   {
//     position = reflectanceSensors.readLine(sensors); // refresh sensor readings each loop.

//     if ((sensors[0] + sensors[5]) < 500)
//     {
//       next_state = STATE_FORWARD;
//     }
//     else if (sensors[0] > 300)
//     {
//       next_state = STATE_RIGHT;
//     }
//     else if (sensors[5] > 300)
//     {
//       next_state = STATE_LEFT;
//     }

//     if ((sensors[1] + sensors[2] + sensors[3] + sensors[4]) > 500)
//     {
//       line_detected = true;
//     }

//     setMotorState(next_state);
//   }
// }

#include "zumo_driver.h"