//  Precompiled Header
#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <GL/glew.h>
#include <GL/wglew.h>
#include <GL/glut.h>

#define V(x,y,z)  glVertex3f(x,y,z);

#include <math.h>
#include <vector>


#include <cutil.h>
#include <vector_functions.h>
typedef unsigned int uint;

#include "..\CUDA\Params.cuh"


#include <tinyxml.h>
using namespace std;


//  float3 missings
inline  void  float3set(float* a, float* b)	{  a[0] = b[0];  a[1] = b[1];  a[2] = b[2];  }

inline  void  operator+= (float3& a, float3 &b) {  a.x += b.x;  a.y += b.y;  a.z += b.z;  }
inline  void  operator+= (float4& a, float3 &b) {  a.x += b.x;  a.y += b.y;  a.z += b.z;  }
inline  void  operator+= (float4& a, float4 &b) {  a.x += b.x;  a.y += b.y;  a.z += b.z;  }
inline  void  operator-= (float3& a, float3 &b) {  a.x -= b.x;  a.y -= b.y;  a.z -= b.z;  }
inline  float3  operator-(float3& a) {  float3 v;  v.x = -a.x;  v.y = -a.y;  v.z = -a.z;  return v;  }

inline  float3  operator+(float3& a, float3 &b) {  float3 v;  v.x = a.x + b.x;  v.y = a.y + b.y;  v.z = a.z + b.z;  return v;  }
inline  float3  operator-(float3& a, float3 &b) {  float3 v;  v.x = a.x - b.x;  v.y = a.y - b.y;  v.z = a.z - b.z;  return v;  }
inline  float4  operator-(float4& a, float4 &b) {  float4 v;  v.x = a.x - b.x;  v.y = a.y - b.y;  v.z = a.z - b.z;  return v;  }
inline  float3  operator-(float4& a, float3 &b) {  float3 v;  v.x = a.x - b.x;  v.y = a.y - b.y;  v.z = a.z - b.z;  return v;  }

inline  float3  operator*(float3 const &a, float b) {  return make_float3(a.x*b, a.y*b, a.z*b);  }
inline  float3  operator*(float3& a, float3 &b) {  float3 v;  v.x = a.x * b.x;  v.y = a.y * b.y;  v.z = a.z * b.z;  return v;  }
inline  float4  operator*(float4& a, float b) {  return make_float4(a.x*b, a.y*b, a.z*b, 0);  }

inline  float  length3(float3& a) {  return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);  }
inline  float  length3(float4& a) {  return sqrt(a.x*a.x + a.y*a.y + a.z*a.z);  }


static bool isKeyDown(int vKey)
{
	return	GetAsyncKeyState(vKey)!=0;
}

//  ---  random  ---
static const float  fRand_Max = 1.f/float(RAND_MAX);

inline  float  frand()  {  return rand() * fRand_Max;  }

static  float  random(float a,float b)
{
	return  (b-a) * float(rand()) * fRand_Max + a;
}

//  from str
static  float3  toVec3(const char* str)	{	float3 v;	sscanf(str, "%f %f %f", &v.x, &v.y, &v.z);	return v;	}
inline  float  toFloat(const char* str)	{	return (float)atof(str);	}
inline  int  toInt(const char* str)		{	return (int)strtol(str, NULL, 0);	}
