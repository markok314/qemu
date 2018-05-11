
#include "common/itest.h"

#include "Vehicle.h"

int g_recording[1000];
struct struct_t ex_struct;


Vehicle::Vehicle(int numWheels, int weight) :
    m_numWheels(numWheels),
    m_weight(weight)
{
    
}


int Vehicle::getNumWheels() 
{
    return m_numWheels; 
}


void Vehicle::setNumWheels(int value)
{
     m_numWheels = value;
}


void Vehicle::setNumWheels(char *value)
{
    setNumWheels(value, 10);
}


void Vehicle::setNumWheels(char *value, int radix)
{
    int numWheels = 0;
    while (*value != '\0') {
        numWheels *= radix;
        if (*value <= '9') {
            numWheels += *value - '0';
        } else {
            // convert to uppercase before subtraction
            numWheels += (*value & 0xdf) - 'A' + 10;
        }
        value++;
    }
    
    m_numWheels = numWheels;
}


int Vehicle::getWeight()
{
    return m_weight;
}


int Vehicle::getWeightPerWheel()
{
    static int s_numCalls = 0;
    s_numCalls++;
    return getWeight() / getNumWheels();
}


void Vehicle::fft()
{
  int numSamples = sizeof(g_recording) / sizeof(int);
  double f = 0;
  double step = 25.0 / numSamples;
  for (int idx = 0; idx < numSamples; idx++) {
    double f2 = f * f;
    double f3 = f2 * f;
    double f5 = f3 * f2;
    double f7 = f5 * f2;
    g_recording[idx] = (f - f3/6 + f5/120 - f7/5040) * 100 / (f + 1);    
    f += step;
    if (f > 3.14) {
      f -= 3.14;
    }
  }
}


bool Vehicle::isNumWheelsOdd() {
  int numWheels = getNumWheels();
  
  if (numWheels > 100) {
    char max = 100;
    setNumWheels(&max);  // to test call hiearchy for overloads
    setNumWheels(100);  // to test call hiearchy for overloads
  }
  return numWheels % 2;
}

int test_decltype_ref(int &value) {
  return value * 2;
}


int testOverloads(int i) {
  return i * 3 + testOverloads(3, 0);
}


int testOverloads(int x, int y) {
  return x * y;
}


void testExceptionCharArray()
{
  throw "Test Error!";
}

void testExceptionInt()
{
  throw 9124;
}


void testExceptionStructValue()
{
  ex_struct.m_c = 'r';
  ex_struct.m_i = 3451;
  throw ex_struct;
}


void testExceptionStructPtr()
{
  ex_struct.m_c = 'w';
  ex_struct.m_i = 13661;
  throw &ex_struct;
}


int testCatchException()
{
  try {
    testExceptionInt();
  } catch (int ex) {
    return ex;
  }
  
  return 0;
}


