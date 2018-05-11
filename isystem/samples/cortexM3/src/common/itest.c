#include "itest.h"

#include <string.h>
#include <stdarg.h>

#include "projectDefs.h"
#include "stringUtils.h"
#include "listUtils.h"
#include "ArrayList.h"
#include "Vehicle.h"
#include <stdint.h>

int testVehicle();

// global variables of miscellaneous types
char g_char1;
char g_char2;
char g_char3;
uint8_t g_char4;

unsigned char g_uchar1;
unsigned char g_uchar2;
unsigned char g_uchar3;

short g_short1;
short g_short2;
short g_short3;

unsigned short g_ushort1;
unsigned short g_ushort2;
unsigned short g_ushort3;

int g_int1;
int g_int2;
int g_int3;

unsigned g_uint1;
unsigned g_uint2;
unsigned g_uint3;

long g_long1;
long g_long2;
long g_long3;

unsigned long g_ulong1;
unsigned long g_ulong2;
unsigned long g_ulong3;

#ifdef USE_FLOAT_TYPE
float g_float1;
float g_float2;
float g_float3;

double g_double1;
double g_double2;
double g_double3;
#endif

char g_charArray1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
unsigned char g_ucharArray2[] = {10, 20, 30, 40, 50, 60, 70, 80, 90, 100};

short g_shortArray1[] = {101, -201, 301, -401, 501, 601, 701, 801, 901, -1001};
unsigned short g_ushortArray1[] = {101, 201, 301, 401, 501, 601, 701, 801, 901, 1001};

int g_intArray1[] = {101, -201, 301, -401, 501, 601, 701, 801, 901, -1001};
int g_intArray2[] = {102, -202, 302, -402, 502, 602, 702, 802, 902, -1002};
int g_intArray3[] = {103, -203, 303, -403, 503, 603, 703, 803, 903, -1003};

unsigned g_uintArray1[] = {101, 201, 301, 401, 501, 601, 701, 801, 901, 1001};
unsigned g_uintArray2[] = {102, 202, 302, 402, 502, 602, 702, 802, 902, 1002};
unsigned g_uintArray3[] = {103, 203, 303, 403, 503, 603, 703, 803, 903, 1003};

long g_longArray1[] = {101, -201, 301, -401, 501, 601, 701, 801, 901, -1001};
unsigned long g_ulongArray1[] = {101, 201, 301, 401, 501, 601, 701, 801, 901, 1001};

#ifdef USE_FLOAT_TYPE
float g_floatArray1[] = {1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.02345};
float g_floatArray2[] = {2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3.02345};
float g_floatArray3[] = {3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4.02345};

double g_doubleArray1[] = {1.1, 1.2, 1.3, 1.4, 1.5, 1.6, 1.7, 1.8, 1.9, 2.02345};
double g_doubleArray2[] = {2.1, 2.2, 2.3, 2.4, 2.5, 2.6, 2.7, 2.8, 2.9, 3.02345};
double g_doubleArray3[] = {3.1, 3.2, 3.3, 3.4, 3.5, 3.6, 3.7, 3.8, 3.9, 4.02345};
#endif

struct struct_t g_struct;
struct Person_t g_person;
struct Person_t *gp_person;

const char *g_pchar = "literal string 1";
short *g_pshort;
unsigned char *g_puchar = (unsigned char *)"literal string 2";
int *g_pint;
long *g_plong;
unsigned int *g_puint;

const char **g_ppchar;
unsigned char **g_ppuchar;
int **g_ppint;
unsigned int **g_ppuint;

#ifdef USE_FLOAT_TYPE
float *g_pfloat;
double *g_pdouble;

float **g_ppfloat;
double **g_ppdouble;
#endif

typedef enum {E_ONE, E_TWO, E_THREE, E_FOUR, E_FIVE} enum_t;
enum_t g_enum;


union union_t {
    char m_c;
    int m_i;
    #ifdef USE_FLOAT_TYPE
    float m_f;
    double m_d;
    #endif

    char m_ac[8];
    int m_ai[8];
    #ifdef USE_FLOAT_TYPE
    float m_af[8];
    double m_ad[8];
    #endif
    
    char *m_pc;
    int *m_pi;
    #ifdef USE_FLOAT_TYPE
    float *m_pf;
    double *m_pd;
    #endif
} g_union;

typedef union union_t union_td;

struct bitA_t
{
    int m_b1:3;
    int m_b2:2;
    int m_b3:5;
    int m_b4:4;
} bitA;

struct_td g_structA;

struct complexStruct_t {
    int m_i;
    struct_td m_struct;
    char m_c;
    union_td m_union;
    #ifdef USE_FLOAT_TYPE
    float m_f;
    #endif
} g_complexStruct;

typedef struct complexStruct_t complexStruct_td;
complexStruct_td *g_pcomplexStruct;
void *g_vptr;

typedef char (* pfuncTestChar1_t)(char a);


#define ARRAY_2_DIM_1 5
#define ARRAY_2_DIM_2 10

char g_charArray2D[ARRAY_2_DIM_1][ARRAY_2_DIM_2];
int g_intArray2D[ARRAY_2_DIM_1][ARRAY_2_DIM_2];
unsigned g_uintArray2D[ARRAY_2_DIM_1][ARRAY_2_DIM_2];
#ifdef USE_FLOAT_TYPE
float g_floatArray2D[ARRAY_2_DIM_1][ARRAY_2_DIM_2];
double g_doubleArray2D[ARRAY_2_DIM_1][ARRAY_2_DIM_2];
#endif
struct_td g_structArray2D[ARRAY_2_DIM_1][ARRAY_2_DIM_2];

// Module initialization functions, not called directly from test
// of isystem.test.



void initGlobals()
{
    g_char1 = 'a';
    g_char2 = 'b';
    g_char3 = 'c';
    g_char4 = 'd';

    g_uchar1 = 'u';
    g_uchar2 = 'v';
    g_uchar3 = 'z';

    g_short1 = 1000;
    g_short2 = -2000;
    g_short3 = 3000;

    g_ushort1 = 1000;
    g_ushort2 = 2000;
    g_ushort3 = 3000;

    g_int1 = 10000;
    g_int2 = -20000;
    g_int3 = 30000;

    g_uint1 = 1000000;
    g_uint2 = 2000000;
    g_uint3 = 3000000;

    g_long1 = 1000000 ;
    g_long2 = -2000000;
    g_long3 = 3000000;

    g_ulong1 = 1000000;
    g_ulong2 = 2000000;
    g_ulong3 = 3000000;

    #ifdef USE_FLOAT_TYPE
    g_float1 = 3.14;
    g_float2 = 3.25;
    g_float3 = 3.16;

    g_double1 = 3.14;
    g_double2 = 3.18;
    g_double3 = 3.19;
    #endif

    g_pshort = g_shortArray1;
    g_pint = g_intArray1;
    g_plong = g_longArray1;
    g_puint = g_uintArray1;

    #ifdef USE_FLOAT_TYPE
    g_pfloat = g_floatArray1;
    g_pdouble = g_doubleArray1;
    #endif

    g_ppchar = &g_pchar;
    g_ppuchar = &g_puchar;
    g_ppint = &g_pint;
    g_ppuint = &g_puint;

    #ifdef USE_FLOAT_TYPE
    g_ppfloat = &g_pfloat;
    g_ppdouble = &g_pdouble;
    #endif


    g_enum = E_FOUR;

    g_struct.m_c = 'q';
    g_struct.m_i = 'w';     
    g_struct.m_f = 'e';
    g_struct.m_d = 'r';

    for (unsigned i = 0; i < sizeof(g_struct.m_ac); i++) {
        g_struct.m_ac[i] = i * 2; 
        g_struct.m_ai[i] = i * 2;  
        #ifdef USE_FLOAT_TYPE
        g_struct.m_af[i] = i * 2;
        g_struct.m_ad[i] = i * 2;
        #endif
    }

    g_struct.m_pc = &g_char1;
    g_struct.m_pi = &g_int1;   
    #ifdef USE_FLOAT_TYPE
    g_struct.m_pf = &g_float1;
    g_struct.m_pd = &g_double1;
    #endif

    // g_union
    g_union.m_c = 'a';
    
    // g_complexStruct
    g_complexStruct.m_i = 123;

    g_complexStruct.m_struct.m_c = 'q';
    g_complexStruct.m_struct.m_i = 'w';
    g_complexStruct.m_struct.m_f = 'e';
    g_complexStruct.m_struct.m_d = 'r';
    
    for (unsigned i = 0; i < sizeof(g_struct.m_ac); i++) {
        g_complexStruct.m_struct.m_ac[i] = i * 3;
        g_complexStruct.m_struct.m_ai[i] = i * 3;
        #ifdef USE_FLOAT_TYPE
        g_complexStruct.m_struct.m_af[i] = i * 3;
        g_complexStruct.m_struct.m_ad[i] = i * 3;
        #endif
    }
    
    g_complexStruct.m_struct.m_pc = &g_char1;  
    g_complexStruct.m_struct.m_pi = &g_int1;
    
    #ifdef USE_FLOAT_TYPE
    g_complexStruct.m_struct.m_pf = &g_float1; 
    g_complexStruct.m_struct.m_pd = &g_double1;
    g_complexStruct.m_f = 2.74;
    #endif

        
    g_complexStruct.m_c = 'd'; 
    g_complexStruct.m_union.m_i = 234;

    // bit field
    bitA.m_b1 = 1;
    bitA.m_b2 = 2;
    bitA.m_b3 = 3;
    bitA.m_b4 = 14;

    g_vptr = 0;
        
    // 2 dim arrays
    for (unsigned i = 0; i < ARRAY_2_DIM_1; i++) {
        for (unsigned j = 0; j < ARRAY_2_DIM_2; j++) {
            g_charArray2D[i][j] = i * j;
            g_intArray2D[i][j] = i * j;
            g_uintArray2D[i][j] = i * j;
            #ifdef USE_FLOAT_TYPE
            g_floatArray2D[i][j] = i * j;
            g_doubleArray2D[i][j] = i * j;
            #endif
        }
    }
    
    g_pcomplexStruct = &g_complexStruct;
}


void initStructT(struct_td *pStruct, int value)
{
    pStruct->m_c = value & 0x7f;
    pStruct->m_i = value + 1;
    
    pStruct->m_ac[4] = (value + 1) & 0x7f;
    pStruct->m_ai[1] = value + 5;
    
    pStruct->m_pc = &g_char1;
    pStruct->m_pi = &g_int1;
    
    #ifdef USE_FLOAT_TYPE
      pStruct->m_f = value + 2;
      pStruct->m_d = value + 3;
    
      pStruct->m_af[2] = value + 6;
      pStruct->m_ad[3] = value + 7;

      pStruct->m_pf = &g_float1;
      pStruct->m_pd = &g_double1;
    #endif
}


void modifyStructT(struct_td *pStruct, int value)
{
    pStruct->m_c += value & 0x7f;
    pStruct->m_i += value;
    
    pStruct->m_ac[4] += value & 0x7f;
    pStruct->m_ai[1] += value;
    
    *(pStruct->m_pc) += value;
    *(pStruct->m_pi) += value;

    #ifdef USE_FLOAT_TYPE
    
      pStruct->m_f += value;
      pStruct->m_d += value;

      pStruct->m_af[2] += value;
      pStruct->m_ad[3] += value;

      *(pStruct->m_pf) += value;
      *(pStruct->m_pd) += value;
    
    #endif 
}


void setUnionT(union_td *pUnion, int value)
{
    pUnion->m_i = value;
}


void setBitT(struct bitA_t *pBit, int value)
{
    pBit->m_b1 = value & 7;
    pBit->m_b2 = value & 3;
    pBit->m_b3 = value & 31;
    pBit->m_b4 = value & 15;
}


void setComplexStructT(complexStruct_td *pComplexStruct, int value)
{
    pComplexStruct->m_i = value * 2;
    initStructT(&pComplexStruct->m_struct, value);
    pComplexStruct->m_c = value * 3 & 0x7f;
    setUnionT(&pComplexStruct->m_union, value);
    
    #ifdef USE_FLOAT_TYPE
    pComplexStruct->m_f = value * value;
    #endif
}


// The functions to be tested from isystem.test start here

void func1() {
    // do nothing
}

 
void funcTestGlobals() {
    g_char1 = 'a';
    g_uchar2 = 'u';
    g_short3 = 1000;
    g_ushort1 = 1000;
    g_int2 = 10000;
    g_uint3 = 1000000;
    g_long2 = 1000000 ;
    g_ulong3 = 3000000;
    #ifdef USE_FLOAT_TYPE
    g_float1 = 3.14;
    g_double3 = 3.19;
    #endif

    g_ppchar = &g_pchar;
    g_ppuchar = &g_puchar;
    g_ppint = &g_pint;
    g_ppuint = &g_puint;

    #ifdef USE_FLOAT_TYPE
    g_ppfloat = &g_pfloat;
    g_ppdouble = &g_pdouble;
    #endif

    g_enum = E_THREE;

    g_struct.m_c = 'q';
    g_struct.m_i = 'w';     
    g_struct.m_f = 'e';
    g_struct.m_d = 'r';

    for (unsigned i = 0; i < sizeof(g_struct.m_ac); i++) {
        g_struct.m_ac[i] = i * 2 + 1; 
        g_struct.m_ai[i] = i * 2 + 1;
        
        #ifdef USE_FLOAT_TYPE
          g_struct.m_af[i] = i * 2 + 1;
          g_struct.m_ad[i] = i * 2 + 1;
        #endif
    }

    g_struct.m_pc = &g_char1;
    g_struct.m_pi = &g_int1;   
    #ifdef USE_FLOAT_TYPE
    g_struct.m_pf = &g_float1;
    g_struct.m_pd = &g_double1;
    #endif

    // g_union
    g_union.m_c = 'b';
    
    // g_complexStruct
    g_complexStruct.m_i = 123;

    g_complexStruct.m_struct.m_c = '1';
    g_complexStruct.m_struct.m_i = '2';
    
    #ifdef USE_FLOAT_TYPE
      g_complexStruct.m_struct.m_f = '3';
      g_complexStruct.m_struct.m_d = '4';
    #endif
    
    for (unsigned i = 0; i < sizeof(g_struct.m_ac); i++) {
        
        g_complexStruct.m_struct.m_ac[i] = i * 3 + 5;
        g_complexStruct.m_struct.m_ai[i] = i * 3 + 5;
        
        #ifdef USE_FLOAT_TYPE
          g_complexStruct.m_struct.m_af[i] = i * 3 + 5;
          g_complexStruct.m_struct.m_ad[i] = i * 3 + 5;
        #endif
    }
    
    g_complexStruct.m_struct.m_pc = &g_char1;  
    g_complexStruct.m_struct.m_pi = &g_int1;
    
    #ifdef USE_FLOAT_TYPE
      g_complexStruct.m_struct.m_pf = &g_float1; 
      g_complexStruct.m_struct.m_pd = &g_double1;
    #endif

        
    g_complexStruct.m_c = 'e'; 
    g_complexStruct.m_union.m_i = 2345;
    #ifdef USE_FLOAT_TYPE
    g_complexStruct.m_f = 6.28;
    #endif

    // bit field
    bitA.m_b1 = 2;
    bitA.m_b2 = 3;
    bitA.m_b3 = 5;
    bitA.m_b4 = 13;

    // 2 dim arrays
    for (unsigned i = 0; i < ARRAY_2_DIM_1; i++) {
        for (unsigned j = 0; j < ARRAY_2_DIM_2; j++) {
            g_charArray2D[i][j] = i * j + 11;
            g_intArray2D[i][j] = i * j + 11;
            g_uintArray2D[i][j] = i * j + 11;
            #ifdef USE_FLOAT_TYPE
            g_floatArray2D[i][j] = i * j + 11;
            g_doubleArray2D[i][j] = i * j + 11;
            #endif
        }
    }
    
}

char funcTestChar1(char a) {
    return a + 1;
}

char funcTestPChar1(char *pa) {
    (*pa)++;
    return *pa;
}

char funcTestChar2(char a, char b, char c) {
    return a + b + c;
}

unsigned char funcTestUChar1(unsigned char a) {
    return a + 1;
}

unsigned char funcTestUChar2(unsigned char a, unsigned char b, unsigned char c) {
    return a + b + c;
}

short funcTestShort1(short a) {
    return a + 1;
}
 
short funcTestShort2(short a, short b, short c) {
    return a + b + c;
}

unsigned short funcTestUShort2(unsigned short a, unsigned short b, unsigned short c) {
    return a + b + c;
}

int funcTestInt1(int a) {
    return a + 1;
}
 
int funcTestInt2(int a, int b, int c) {
    return a + b + c + 1;
}

unsigned int funcTestUInt1(unsigned a) {
    return a + 1;
}
 
unsigned int funcTestUInt2(unsigned a, unsigned b, unsigned c) {
    return a + b + c + 1;
}

long funcTestLong1(long a) {
    return a + 1;
}
 
long funcTestLong2(long a, long b, long c) {
    return a + b + c + 1;
}

#ifdef USE_FLOAT_TYPE
    float funcTestFloat1(float a) {
        return a + 1;
    }

    float funcTestFloat2(float a, float b, float c) {
        return a + b + c + 1;
    }

    double funcTestDouble1(double a) {
        return a + 1;
    }

    double funcTestDouble2(double a, double b, double c) {
        return a + b + c + 1;
    }
#endif

char *funcTestCharArray1(char a[]) {
    a[0] = 'f';
    return a;
}

char *funcTestCharArray2(char a[], char b[], char c[]) {
    a[0] = 'f';
    b[1] = 'g';
    c[2] = 'h';
    return c;
}

unsigned char *funcTestUCharArray1(unsigned char a[]) {
    a[0] = 'f';
    return a;
}

unsigned char *funcTestUCharArray2(unsigned char a[], unsigned char b[], unsigned char c[]) {
    a[0] = 'f';
    b[1] = 'g';
    c[2] = 'h';
    return c;
}


short *funcTestShortArray1(short a[]) {
    a[0] = 1234;
    return a;
}

short *funcTestShortArray2(short a[], short b[], short c[]) {
    a[0] = 1235;
    b[1] = 1236;
    c[2] = 1237;
    return c;
}


int *funcTestIntArray1(int a[]) {
    a[0] = 1000000;
    return a;
}

int *funcTestIntArray2(int a[], int b[], int c[]) {
    a[0] = 1000001;
    b[1] = -1000002;
    c[2] = 1000003;
    return c;
}


unsigned int *funcTestUIntArray1(unsigned int a[]) {
    a[0] = 1000000;
    return a;
}

unsigned int *funcTestUIntArray2(unsigned int a[], unsigned int b[], unsigned int c[]) {
    a[0] = 1000000;
    b[1] = 2000000;
    c[2] = 3000000;
    return c;
}


long *funcTestLongArray1(long a[]) {
    a[0] = 4000000;
    return a;
}

long *funcTestLongArray2(long a[], long b[], long c[]) {
    a[0] = 4000001;
    b[1] = 4000002;
    c[2] = 4000003;
    return c;
}


unsigned long *funcTestULongArray1(unsigned long a[]) {
    a[0] = 22345677;
    return a;
}

unsigned long *funcTestULongArray2(unsigned long a[], unsigned long b[], unsigned long c[]) {
    a[0] = 12345677;
    b[1] = 12345678;
    c[2] = 12345679;
    return c;
}


#ifdef USE_FLOAT_TYPE
    float *funcTestFloatArray1(float a[]) {
        a[0] = 1234.1234;
        return a;
    }

    float *funcTestFloatArray2(float a[], float b[], float c[]) {
        a[0] = 12.1;
        b[1] = 12.2;
        c[2] = 12.3;
        return c;
    }


    double *funcTestDoubleArray1(double a[]) {
        a[0] = 123.45;
        return a;
    }

    double *funcTestDoubleArray2(double a[], double b[], double c[]) {
        a[0] = 124;
        b[1] = 124.1;
        c[2] = 124.2;
        return c;
    }
#endif


char *funcTest_P_charArray1(char *a) {
    *a = 'w';
    return a;
}

unsigned char *funcTest_P_ucharArray1(unsigned char *a) {
    *a = 'q';
    return a;
}


short *funcTest_P_shortArray1(short *a) {
    *a = 12341;
    return a;
}

unsigned short *funcTest_P_ushortArray1(unsigned short *a) {
    *a = 12342;
    return a;
}

int *funcTest_P_intArray1(int *a) {
    *a = -12343;
    return a;
}

unsigned int *funcTest_P_uintArray1(unsigned int *a) {
    *a = 12344;
    return a;
}

long *funcTest_P_longArray1(long *a) {
    *a = 12345;
    return a;
}

unsigned long *funcTest_P_ulongArray1(unsigned long *a) {
    *a = -12346;
    return a;
}


#ifdef USE_FLOAT_TYPE
    float *funcTest_P_floatArray1(float *a) {
        *a = 12347.3;
        return a;
    }

    double *funcTest_P_doubleArray1(double *a) {
        *a = 12348.34;
        return a;
    }
#endif


// pointer to pointer
char **funcTest_PP_charArray1(char **a) {
    **a = 'w';
    return a;
}

unsigned char **funcTest_PP_ucharArray1(unsigned char **a) {
    **a = 'r';
    return a;
}


short **funcTest_PP_shortArray1(short **a) {
    **a = -12341;
    return a;
}

unsigned short **funcTest_PP_ushortArray1(unsigned short **a) {
    **a = 12342;
    return a;
}

int **funcTest_PP_intArray1(int **a) {
    **a = 12343;
    return a;
}

unsigned int **funcTest_PP_uintArray1(unsigned int **a) {
    **a = 12344;
    return a;
}

long **funcTest_PP_longArray1(long **a) {
    **a = -12345;
    return a;
}

unsigned long **funcTest_PP_ulongArray1(unsigned long **a) {
    **a = -12346;
    return a;
}


#ifdef USE_FLOAT_TYPE
    float **funcTest_PP_floatArray1(float **a) {
        **a = 12347;
        return a;
    }

    double **funcTest_PP_doubleArray1(double **a) {
        **a = 12348;
        return a;
    }
#endif


// enum
enum_t functTestEnum(enum_t value) {
    return value;
}

enum_t *functTestPEnum(enum_t *value) {
    return value;
}

void *functTestVPEnum(enum_t **value) {
    return *value;
}

// struct

struct_td functTestStruct(struct_td value, int delta) {

    modifyStructT(&value, delta);
    return value;       
}

struct_td *functTestPStruct(struct_td *value) {
    modifyStructT(value, 4);
    return value;
}

void *functTestVPStruct(struct_td **value) {
    modifyStructT(*value, 5);
    return *value;
}



// union

union_td functTestUnion(union_td value) {

    setUnionT(&value, 6);
    return value;
}

union_td *functTestPUnion(union_td *value) {
    setUnionT(value, 7);
    return value;
}

void *functTestVPUnion(union_td **value) {
    setUnionT(*value, 8);
    return *value;
}


// bitArray
struct bitA_t funcTestBit(struct bitA_t bitValue) {
    setBitT(&bitValue, 10);
    return bitValue;
}

struct bitA_t *funcTestPBit(struct bitA_t *bitValue) {
    setBitT(bitValue, 11);
    return bitValue;
}

void *funcTestVPBit(struct bitA_t **bitValue) {
    setBitT(*bitValue, 12);
    return *bitValue;
}


// complexStruct
complexStruct_td functTestComplexStruct(complexStruct_td value) {

    setComplexStructT(&value, 13);
    return value;
}

complexStruct_td *functTestPComplexStruct(complexStruct_td *value) {
    setComplexStructT(value, 14);
    return value;
}

void *functTestVPComplexStruct(complexStruct_td **value) {
    setComplexStructT(*value, 15);
    return *value;
}


// void ptr
void *funcTestVoidPtr(void *ptr) {
    ((complexStruct_td *)ptr)->m_i = 20;
    return ptr;
}

// 2 dim array
char *funcTestcharArray2D(char charArray2D[][ARRAY_2_DIM_2]) {
    charArray2D[2][2] = 'x';
    return charArray2D[2];
}

int *funcTestintArray2D(int intArray2D[][ARRAY_2_DIM_2]) {
    intArray2D[2][2] = 876;
    return intArray2D[2];
}


unsigned *funcTestunsignedArray2D(unsigned unsignedArray2D[][ARRAY_2_DIM_2]) {
    unsignedArray2D[2][2] = 877;
    return unsignedArray2D[2];
}


#ifdef USE_FLOAT_TYPE
    float *funcTestfloatArray2D(float floatArray2D[][ARRAY_2_DIM_2]) {
        floatArray2D[2][2] = 876.876;
        return floatArray2D[2];
    }


    double *funcTestdoubleArray2D(double doubleArray2D[][ARRAY_2_DIM_2]) {
        doubleArray2D[2][2] = 877.877;
        return doubleArray2D[2];
    }


    struct_td *funcTeststruct_tArray2D(struct_td struct_tArray2D[][ARRAY_2_DIM_2]) {
        struct_tArray2D[2][2].m_i = 123;
        struct_tArray2D[2][2].m_pd = &g_double1;
        return struct_tArray2D[2];
    }

    struct_td *funcTestStructArray(struct_td structArray[]) {
        structArray[2].m_i = 123;
        structArray[2].m_pd = &g_double1;
        return structArray;
    }
#endif


char funtTestFuncPtr(pfuncTestChar1_t pfunction) {
    return pfunction('3');
}


#ifdef USE_FLOAT_TYPE
    // misc combinations of parameters
    int funcTestMiscParams1(char a, short b, int c, long d, float e, double f)
    {
        return a + b + c + d + e + f;
    }


    double funcTestMiscParams2(char a, union_td *p_union, char c, long d, float e, char f, char g)
    {
        return a + p_union->m_d + c + d + e + f + g;
    }


    double funcTestMiscParams3(int a, int *p_int, unsigned char c, double *p_double, struct_td s)
    {
        return a + *p_int + c + *p_double + s.m_ai[2];
    }


    void *funcTestMiscParams4(struct bitA_t bitStruct, float floatArray[][ARRAY_2_DIM_2]) {

        floatArray[ARRAY_2_DIM_1 - 1][ARRAY_2_DIM_2 - 1] = 6.28;
        floatArray[0][0] = 12.56;

        return 0;
    }
#endif


// stubs
int stubbedFunc1(char *text, int idx, char replacement)
{
    text[idx] = replacement;
    return idx + 2;
    
}


int repl_stubbedFunc1(char *text, int idx, char replacement)
{
    return idx + 10;
}


int *stubbedFunc3(int *i)
{
    g_int1 = 1969;
    *i += 3;
    return i;
}

void stubbedFuncIntPtr(int *i)
{
    *i = 5;
}

void testStubbedFuncIntPtr()
{
    int param = 0;
    g_int1 = 10;
    
    stubbedFuncIntPtr(&param);

    if (param == 3) {
        g_int1 = 2;                  
    }
}


#ifdef USE_FLOAT_TYPE
    float stubbedFunc2(struct_td data, int i)
    {
        g_int1 = 1969;
        return data.m_f * i;
    }


    struct_td *funcTestStubs(unsigned short us, char *text)
    {
        struct_td data;

        data.m_i = stubbedFunc1(text, 2, 'y');
        data.m_f = 9.81;

        for (unsigned i = 0; i < sizeof(g_struct.m_ac); i++) {
            data.m_ad[i] = 3.14 * i;
            data.m_af[i] = 1.23;  // init, to make test deterministic
            data.m_af[i] = stubbedFunc2(data, i);
        }

        data.m_pc = &g_char1;

        memcpy(&g_struct, &data, sizeof(data));

        g_struct.m_pi = stubbedFunc3(&g_int2);

        return &g_struct;
    }
#endif


int funcTestStubsSimple(int *y) 
{
  int r = *y;
  int *w;
  w = stubbedFunc3(y);
  
  return r + *w + *y;
}


int testStubSteps()
{
  int sum = 0;
  
  sum += stubbedFunc1(&g_char1, 0, 'a');
  sum += stubbedFunc1(&g_char2, 0, 'a');
  sum += stubbedFunc1(&g_char3, 0, 'a');
  
  return sum;
}


int stubbedFuncInt() {
  return 123;
}

int repl_stubbedFuncInt() {
  return 234;
}

int funcForIntStubTest() {
  int numItems = 34;
  numItems = stubbedFuncInt();
  numItems = stubbedFuncInt();
  numItems = stubbedFuncInt();
  return numItems;
}

int funcTestStubsNested() {
  int retVal;
  
  retVal = funcTestInt1(funcTestShort1(6));
 
  if (42 == stubbedFuncInt()) {
    return retVal; 
  }
  
  return 456;
}


#ifdef USE_FLOAT_TYPE
    float stubbedFuncFloat() {
      return 1.2;
    }


    float funcForFloatStubTest() {
      return stubbedFuncFloat();
    }
#endif


int *funcTestStubScript(int multiplier)
{
  int numItems;
  
  for (numItems = 0; numItems < 3; numItems++) {
    g_intArray1[numItems] = funcTestInt1(numItems) * multiplier;
  } 
  
  return g_intArray1;
}


int testCustomStubActivation()
{
  int numItems = 7;
  numItems += stubbedFuncInt(); // returns 123
  numItems *= 2;                // tp_tcsa_1
  stubbedFuncIntPtr(&numItems); // numItems = 5
  numItems *= 2;                // tp_tcsa_2
  numItems += funcTestStubsNested();
  
  return numItems; // returns 11 if no stubs
}


int stubWArray(char *name)
{
  return name[0];
}


int funcTestStubWArray()
{
  char name[10];
  strcpy(name, "AB");
  stubWArray(name);
  return name[0];
}


// this function is intended for testing of cumulative coverage 
int complexFunction(int a, int b, int c)
{
  int result = 0;
  
  if (a) {
    if (b) {
      result = a * 3 + b;
    } else {
      result = a * 4;
    }
  } else {
    if (b) {
      result = b + c;
    } else {
      result = c * 2;
    }
  }
  
  if (c == 4) {
    return 3000;
  }
  
  return result;
}


int add_int(int a, int b) 
{
  return a + b;
}


// analyzers will be tested on functions in files coverageC and profilerC



typedef union {
    unsigned short word;
    struct {
        unsigned short F1:1; // 0
        unsigned short F2:1; // 1
    }bit;
} T_TestWord;

T_TestWord g_test_word;

T_TestWord stubTW(void)
{
    T_TestWord tw;
    tw.bit.F1 = 1;
    return tw;
}

void testfunctionTW(void)
{
    g_test_word = stubTW();
}
 
void instrumentedLog(int pos)
{
  g_int1 = pos;
}

/*
 * Compile this function without and with optimization (-O2),
 * and compere dissassembly output. Then enable/disable call to
 * instrumentation function and compare outputs again.
 */
int statementConditionCvrgTest(int a, int b, int c)
{
    int d = 0;
    if ((a > 0  ||  b > 0)  &&  c > 0) {
      // to demonstrate diff between instrumented and
      // non-istrumented code, see dissassmbly when
      // the following call is present/removed.
      instrumentedLog(23);     
      d = 1;
    }
    return d;
}


int testWTimer()
{
  g_int2 = 0;
  
  while (g_int2 == 0);
  
  return g_int2;
}


int testPointTest(int p1) 
{
    int x = 0;          // TID: tp_id_4
    int i;
    
    for (i = 0; i < 10; i++) {
        x += p1;        // myComplexTestPoint
    }
    
    if  (i < 0) {
        x -= 10;  // will never execute - test point should detect this
    }
    if (p1 > 10) {
        x += p1;        // add items to items  TID: tp_id_4
    } else {
        x -= p1;        // tp_id_14
    }
    
    return x;
}


int seqDiagTest(int a)
{
  int z = a, i;
  
  if (a < 10) {
    z = funcTestInt1(11);
  } 
  
  for (i = 0; i < 3; i++) {
    funcTestStubsNested();
  }
                      
  funcTestChar2(1, 2, 3);
  
  funcTestLong1(10);
  
  for (i = 0; i < 100; i++) {
    funcTestUInt1(10);
    
    if (i % 3) {
      funcTestUInt2(10, 20, 30);
    }
  }
  
  return z;
}


int timeoutTest()
{
  int r = 0;
  int i;
  
  for (i = 0; i < 10000000; i += 3) {
    r += i/5;
  }
  return r;
}


int stringlen(char *str) 
{
  int cnt = 0;
  while (str[cnt] != '\0') {
    cnt++;
  }
  
  return cnt;
}


#ifdef USE_FLOAT_TYPE

    int testVarArgs(char *str, ...)
    {
      va_list argp;
      int i, result;
      double d;

      va_start(argp, str);

      switch (str[0]) {
      case 'i':
        i = va_arg(argp, int);
        result = i * 2 + stringlen(str);
        break;
      case 'd':
        d = va_arg(argp, double);
        result = d * 4 + stringlen(str); 
      }

      va_end(argp);

      return result; 
    }
#endif


StatusStruct_td getStatus()
{
  StatusStruct_td status;
  status.m_status = 'a';
  status.m_value = 'r';
  
  return status;
}


int testStubbedStructRetVal() 
{
  StatusStruct_td status = getStatus();
  
  return status.m_status == 'x'  &&  status.m_value == 'y';
}


int getTemperature0()
{
  return 10;
}


int getTemperature1()
{
  return 11;
}


int getPressure0()
{
  return 20;
}

int getPressure1()
{
  return 22;
}


int getAltitude(int sensorID)
{
  int temperature = 0;
  int pressure = 0;
  int altitude;
  
  if (sensorID != SENSOR_0  &&  sensorID != SENSOR_1) {
    return E_ARGUMENT;
  }  
  
  switch (sensorID) {       // testPoint_1
  case SENSOR_0:
      temperature = getTemperature0();
      pressure = getPressure0();
      break;
  case SENSOR_1:
      temperature = getTemperature1();
      pressure = getPressure1();
      break;
  default:
      return E_INTERNAL;
  }
  
  if (temperature > LINEAR_RANGE) {
    altitude = pressure * A_SCALE + temperature / LIN_SCALE;
  } else {
    altitude = pressure * A_SCALE + temperature / NONLINEAR_SCALE;
  }
  
  return altitude;
}


int isYoungerThanAndLivesInCapital(struct Person_t *person, int age) 
{ 
  if (person->m_age < age  &&  person->m_address.m_zipCode == 1000) {
    return TRUE;
  }
  
  return FALSE;
}


// call functions from other modules, so that linker does not eliminate them
void processStrings()
{
  char data[] = "abcdefghijkl";
  char outStr[100];
  
  strToLower(data);
  strToUpper(data);
  
  strFindChar(data, 'b');
  strSubstr(data, 2, 3, outStr, sizeof(outStr));
  strSubstrChar(data, 'b', 'e', outStr, sizeof(outStr));
  strReplaceChar(data, 'c', 'x');
  
}


void processLists()
{
  int inList[] = {34, 12, 1, 2, 5, 9};
  
  maxElement(inList, sizeof(inList)/sizeof(int));
  minElement(inList, sizeof(inList)/sizeof(int));
  sumList(inList, sizeof(inList)/sizeof(int));
  countGreaterThan(inList, sizeof(inList)/sizeof(int), 5);
  countLessThan(inList, sizeof(inList)/sizeof(int), 5);
}


int testArrayList()
{
  ArrayList<int> al(20, 10);
  al.add(10);
  al.add(20);
  al.add(30);
  
  int x = 0;
  x += al.get(0) + al.get(1) + al.get(2);
  
  ArrayList<struct_td> als(20, 10);
  struct_td s;
  s.m_c = 'a';
  s.m_i = 42;
  
  als.add(s);
  x += als.get(0).m_i;  
    
  return x;
}


int testVehicle() {
  Vehicle v(1, 2);
  
  v.setNumWheels(10);
  v.isNumWheelsOdd();
  return v.getNumWheels();
}


// see also debug.c 
static int static_func(int a, int b) 
{
  return a * b;
}


int speedTest(int noOfLoops) 
{
    int result = 0;
    for (int i = 0; i < noOfLoops; i++) {
        for (int j = 0; j < 1000000; j++) {
            result += j * i; // overflows, but we don't care about result
        }
    }
    
    return result;
}


void flameGraphTest_L2_a()
{
    seqDiagTest(5);
    funcTestStubsNested();
}


void flameGraphTest_L2_b()
{
    funcTestStubsNested();
}


void flameGraphTest_L2_c()
{
    testCustomStubActivation();
}


void flameGraphTest_L1_a()
{
    flameGraphTest_L2_a();
    flameGraphTest_L2_b();
    flameGraphTest_L2_c();
    flameGraphTest_L2_a();   
    flameGraphTest_L2_c();
    flameGraphTest_L2_a();   
}


void flameGraphTest_L1_b()
{
    funcTestStubsNested();
}


void flameGraphTest_L1_c()
{
    testCustomStubActivation();
}


void flameGraphTest_L0_b()
{
    flameGraphTest_L1_a();
    flameGraphTest_L1_b();
}


void flameGraphTest_L0_a()
{
    flameGraphTest_L1_a();
    flameGraphTest_L1_b();
    flameGraphTest_L1_c();
}


// Init function to be called from main(). Contains all initialization
// for itest tests.
void iTestMain(void)
{
    initGlobals();
    add_int(3, 4);
    testVehicle();
    testOverloads(3);
    static_func(9, 3);
}
