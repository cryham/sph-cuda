#pragma once
//  Simulation Parameters
//-------------------------

#ifndef __DEVICE_EMULATION__
#define USE_TEX 1
#endif

#ifdef USE_TEX
	// macro does either global read or texture fetch
	#define  FETCH(t,i)  tex1Dfetch(t##Tex, i)
#else
	#define  FETCH(t,i)  t[i]
#endif

#include <vector_types.h>

#define PI   3.141592654f  //3.141592653589793
#define PI2  2.f*PI

#define cstr  const static char


enum BndType  {  BND_BOX=0, BND_CYL_Y, BND_CYL_Z, BND_CYL_YZ, BND_SPHERE, BND_PUMP_Y,  BND_ALL, BND_DW = 0xFFFFffff };
cstr BndNames[BND_ALL][20] =
				{"Box", "Cylinder Y", "Cylinder Z", "Cylinders Y,Z", "Sphere", "Pump Y"};

enum BndEff   {  BND_EFF_NONE=0, BND_EFF_WRAP, BND_EFF_CYCLE, BND_EFF_WAVE,  BEF_ALL, BEF_DW = 0xFFFFffff };
cstr BndEffNames[BEF_ALL][20] =
				{"None", "Wrap Z", "Cycle Z", "Wave Z"};

enum ClrType  {  CLR_Dens=0, CLR_Accel, CLR_DensAcc, CLR_Vel, CLR_VelAcc, CLR_VelRGB, CLR_None,  CLR_ALL, CLR_DW = 0xFFFFffff };
cstr ClrNames[CLR_ALL][20] =
				{"Density", "Acceleration", "Density + Accel", "Velocity", "Velocity + Accel", "Velocity RGB", "None (last)" };
const static int HUE_ALL = 3;
const static ClrType HueType[HUE_ALL] = { CLR_Dens, CLR_Accel, CLR_Vel };


//  Accelerator
enum AccType  {  ACC_Off=0, ACC_Box, ACC_CylY, ACC_CylYsm, /*CylZ..*/ ACC_ALL, ACC_DW = 0xFFFFffff };
cstr AccNames[CLR_ALL][20] =
				{"Off", "Box", "Cylind Y ellipse", "Cylind Y smooth" };

struct Accel
{
	float3 pos, size, acc;
	AccType type;
};

const int NumAcc = 4;  // max


struct SimParams
{
	///  simulation  --------
	float timeStep;
	uint numParticles, maxParInCell;

	//  gravity
	float3 gravity;  float globalDamping;

	
	//  Grid  ---------------
	uint3 gridSize;  float3 cellSize;
	uint gridSize_yx, numCells;
	//  World
	float3 worldMin, worldMax, worldSize,
		  worldMinD, worldMaxD, worldSizeD;  // draw, -boundary

	
	///  SPH  ---------------
	float particleR, h, h2;		// smoothing radius
	float SpikyKern, LapKern, Poly6Kern;	// kernel consts
	
	//  Fluid
	float particleMass, restDensity,
		stiffness, viscosity,	minDens, minDist;  // stability


	//  Boundary
	float distBndHard, distBndSoft;
	float bndDamp, bndStiff, bndDampC;
	BndType bndType;  BndEff bndEffZ;
			

	///  sphere collider  ----
	float4 collPos;  float collR;
	float spring, damping, shear;	// collision
	

	///  Visual  ------------
	ClrType clrType;  int iHue;
	float brightness, contrast;
	
	//  Dye
	int dyeType, dyeClear;
	float dyeFade;  float3 dyePos, dyeSize;
	
	
	///  Accels
	Accel acc[NumAcc];	int iHmap;


	///  pump
	float angOut, hClose, radIn;  // dim
	float rVexit,rDexit, s1,s2, s3,s4, s5,s6;  // bnd-

	//  rotor
	float rAngle, rTwist;
	int  rotType, rotBlades;
	int3 rotSize;	float rotR, rotSpc;
	//  2nd
	float r2Dist, r2Angle, r2twist, ff2;
};
