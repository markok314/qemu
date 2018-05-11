#ifndef I_TEST__H
#define I_TEST__H

struct struct_t 
{
    char m_c;
    int m_i;
    float m_f;
    double m_d;
    
    char m_ac[7];
    int m_ai[7];
    float m_af[7];
    double m_ad[7];
    
    char *m_pc;
    int *m_pi;
    float *m_pf;
    double *m_pd; 
};

union HouseNumber_t 
{
  int m_inum;
  char m_cnum;
};

struct Address_t 
{
  char m_street[120];
  int m_zipCode;
  union HouseNumber_t m_houseNumber;
};

struct Person_t 
{
  char m_name[20];
  struct Address_t m_address;
  int m_age;
  struct Address_t m_pastAddresses[5];
  int *m_coords;
  struct Address_t *mp_tempAddress;
  struct Address_t *mp_volatileAddresses[5];
};

struct StatusStruct_t 
{
  char m_status;
  char m_value;
};


typedef struct struct_t struct_td;
typedef struct StatusStruct_t StatusStruct_td;
  
void iTestMain(void);
int add_int(int a, int b);
void initGlobals();

#define TRUE 1
#define FALSE 0

#define TEST_MACRO_1 45
#define TEST_MACRO_2(x) ((x) + 1)

#define INT_TYPE32 int

#define SENSOR_0 0
#define SENSOR_1 1
#define E_ARGUMENT -100001
#define E_INTERNAL -100002
#define LINEAR_RANGE 5500
#define A_SCALE 2
#define LIN_SCALE 3;
#define NONLINEAR_SCALE 4;

#endif  // I_TEST__H

