#include "zumo_state.h"

#define SPEED_MAX 100
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
#define WALL_FOLLOW_MODE 3
#define ROTATE_PUSH_MODE 4

#define GAP_JUMP_DELAY 1500 // milliseconds to turn left once a line is found before resuming line following.

/* ---------END STATE ENCODING---------- */

// Create a variable to hold a state.  This should be set to the default state.
int state = STATE_FORWARD;

// Create a variable to hold the next state.  This should also be set to the default state.
int next_state = STATE_FORWARD;

uint8_t mode[] = {LINE_FOLLOW_MODE, JUMP_GAP_MODE, LINE_FOLLOW_MODE, FINISH_MODE};
uint8_t current_mode_num = 0;

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
  motors.setSpeeds(SPEED_HALT, SPEED_MAX);
}

void stateRight()
{
  Serial.println("right");
  Serial0.println("right");
  motors.setSpeeds(SPEED_MAX, SPEED_HALT);
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
  case WALL_FOLLOW_MODE:
    wall_follow();
    break;
  case ROTATE_PUSH_MODE:
    break; // TODO: implement rotate push mode
  case FINISH_MODE:
    finish();
    break;

  default:
    finish();
    break;
  }

  current_mode_num++; // increment to the next mode after each mode is completed.
}

void stateSwitch(int _state)
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
  Serial0.println("Finished!");
  stateSwitch(STATE_HALT);
  while (true)
  {
  } // do nothing
}

void line_follow()
{
  bool stop_flag = false; // if no line or a stop line is detected, set to true to exit loop.

  while (!stop_flag)
  {
    Serial0.print("line follow - ");
    Serial0.print(position);
    Serial0.print(" - ");
    position = reflectanceSensors.readLine(sensors);
    int sensor_total = (sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + sensors[5]);
    if (state == STATE_FORWARD)
    {
      if (position < 1000) // Put your condition here
      {
        // Write your desired state here
        next_state = STATE_LEFT;
      }
      else if (position > 4000) // Put your condition here
      {
        next_state = STATE_RIGHT;
      }
      else next_state = state;
    }
    else if (state == STATE_RIGHT || state == STATE_LEFT)
    {
      if (position < 3500 && position > 1500) // Put your condition here
      {
        // Write your desired state here
        next_state = STATE_FORWARD;
      }
      else next_state = state;
    }

    if (sensor_total < 50 || sensor_total > 3000)
    {
      stop_flag = true;
      Serial0.println("No line detected!");
      next_state = STATE_HALT;
    }
    stateSwitch(next_state); // select/set motor output states
    state = next_state;
  }
}

// Jump Gap mode
void jump_gap()
{
  stateSwitch(STATE_FORWARD);
  bool line_detected = false;
  while (!line_detected)
  {

    Serial0.print("jump gap - ");
    position = reflectanceSensors.readLine(sensors);
    int sensor_total = (sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + sensors[5]);
    if (sensor_total > 500)
    {
      line_detected = true;
    }
    else
    {
      stateSwitch(STATE_FORWARD);
    }
  }
  motors.setSpeeds(-SPEED_MAX, SPEED_MAX);
  delay(GAP_JUMP_DELAY);
  stateSwitch(STATE_HALT); // line detected, stop and exit.
}

void wall_follow(){
  if ((sensors[0]+sensors[5])<10)
  {
    next_state = STATE_FORWARD;
  }
  
  if (sensors[0]>0)
  {
    next_state = STATE_RIGHT;
  }else if (sensors[5]>0)
  {
    next_state = STATE_LEFT;
  }

  
  stateSwitch(next_state);
  

}

#include "zumo_driver.h"