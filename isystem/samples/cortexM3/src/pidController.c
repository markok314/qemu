
#include "projectDefs.h"
#include "pidController.h"

#ifdef USE_FLOAT_TYPE
static float g_kp = 3.4f;
static float g_ki = 2.2f;

float piController(float time, float e)
{
    static float prevTime = 0;
    static float integ = 0;
    
    float dt = time - prevTime;
    prevTime = time;

    integ += e * dt;

    return g_kp * e + g_ki * integ;
}


void controllerLoop()
{
  static float time = 0;
  static float inVal = 0;
  static float outVal = 0;
 
  g_kp = 3.4f;
  g_ki = 2.2f;

  while (1) {
      outVal = piController(time, inVal);
      if (outVal < 10) {
        break;
      }
  }
}

#endif
