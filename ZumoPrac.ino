#include "zumo_state.h"

#define SPEED_MAX 150
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
/* ---------END STATE ENCODING---------- */

// Create a variable to hold a state.  This should be set to the default state.
int state = STATE_FORWARD;

// Create a variable to hold the next state.  This should also be set to the default state.
int next_state = STATE_FORWARD;

uint8_t mode[] = {LINE_FOLLOW_MODE, JUMP_GAP_MODE, LINE_FOLLOW_MODE, FINISH_MODE};
uint8_t current_mode_num = 0;


//States
void stateForward()
{
  Serial.println("forward");+
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
  
  default:
    break;
  }
  
  while (mode[current_mode_num] == FINISH_MODE)
  {
    stateHalt();
    Serial.println("Finished!");
  }
  
}

void stateSwitch(state){
      switch (state)
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

//Modes
void line_follow()
{
  bool no_line = false;

  while (!no_line)
  {
    position = reflectanceSensors.readLine(sensors);
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
      else if ((sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + sensors[5]) < 500)
      {
        no_line = true;
      }
    }

    if (state == STATE_LEFT)
    {
      if (position > 2000) // Put your condition here
      {
        // Write your desired state here
        next_state = STATE_FORWARD;
      }
      else if ((sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + sensors[5]) < 500)
      {
        no_line = true;
      }
    }

    if (state == STATE_RIGHT)
    {
      if (position < 3000) // Put your condition here
      {
        // Write your desired state here
        next_state = STATE_FORWARD;
      }
      else if ((sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + sensors[5]) < 500)
      {
        no_line = true;
      }
    }

    if (no_line)
    {
      Serial0.println("No line detected!");
      next_state = STATE_HALT;
    }

    stateSwitch(next_state);


  }
  current_mode++;
}

//Jump Gap mode
void jump_gap()
{
  bool no_line = false;

  while (!no_line)
  {
    position = reflectanceSensors.readLine(sensors);
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
      else if ((sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + sensors[5]) < 500)
      {
        no_line = true;
      }
    }

    if (state == STATE_LEFT)
    {
      if (position > 2000) // Put your condition here
      {
        // Write your desired state here
        next_state = STATE_FORWARD;
      }
      else if ((sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + sensors[5]) < 500)
      {
        no_line = true;
      }
    }

    if (state == STATE_RIGHT)
    {
      if (position < 3000) // Put your condition here
      {
        // Write your desired state here
        next_state = STATE_FORWARD;
      }
      else if ((sensors[0] + sensors[1] + sensors[2] + sensors[3] + sensors[4] + sensors[5]) < 500)
      {
        no_line = true;
      }
    }

    if (no_line)
    {
      Serial0.println("No line detected!");
      next_state = STATE_HALT;
    }

    state = next_state;

    switch (state)
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
  current_mode++;
}


#include "zumo_driver.h"