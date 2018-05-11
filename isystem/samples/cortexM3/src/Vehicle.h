#ifndef vehicle__H
#define vehicle__H

extern "C"
{
    int testCatchException();
}

int testOverloads(int i);
int testOverloads(int x, int y);

/**
 * Test class for testIDEA.
 */
class Vehicle
{
  private:
    int m_numWheels;
    int m_weight;

  public:
    Vehicle(int numWheels, int weight);
  
    int getNumWheels();
    void setNumWheels(int value);
    void setNumWheels(char *value);
    void setNumWheels(char *value, int radix);
    int getWeight();
    int getWeightPerWheel();
    void fft();
    bool isNumWheelsOdd();
};

#endif  
