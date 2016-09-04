#include "header.h"

#include "..\SPH\Scene.h"



void Scene::InitDefault()	//  Init
{
	SimParams& p = params;
	///  simulate  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	p.numParticles = 7*8*1024;		p.maxParInCell = 16 /*=8 4*/;
	p.timeStep = 0.0026f; /*3*/		p.globalDamping = 1.00f;
	p.gravity = make_float3(0, -9.81, 0);

	///  SPH init   * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
	p.particleR = 0.004;	p.minDist = 1.0; /* 0.8  p.particleR*/
	p.h = 0.01;	 //p.h2 = p.h*p.h;
	spacing = 1.38 /* p.particleR*/;

	p.restDensity = 1000;	p.minDens = 1.0; /* 0.9  p.restDensity*/
	p.stiffness = 3.0;  p.viscosity = 0.5;

	///  World  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	float3 w = make_float3(0.2, 0.25, 0.2);
	p.worldMin = -w;  p.worldMax = w;	initMin = -w;  initMax = w;
	fCellSize = p.particleR*2.0;  // (=h) smooth_r sph..
	dropR = 5;	rain = 0;  initType = 0;  initLast = 1;

	//  Boundary  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	p.distBndSoft = 8;	  p.distBndHard = 1;	//* p.particleR
	p.bndStiff = 30000;	  p.bndDamp = 256;	p.bndDampC = 60;
	p.bndType = BND_BOX;  p.bndEffZ = BND_EFF_NONE;		p.iHmap = 0;
	
	//  collider  . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . 
	p.collR = 0.05;  p.collPos.x = -77.f;
	p.spring = 80;  p.damping = 0.02;  p.shear = 0.1;
	
	//  pump    * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
	p.angOut = 0.7;  p.hClose = -0.03;  p.radIn = 0.4;
	p.s1 = 0.90; p.s2 = 0.90;  p.s3 = 1.198; p.s4 = 0.7782;  p.s5 = 0.8696; p.s6 = 0.843;  // S
	p.rVexit = 0.03;	p.rDexit = 0.04;
	//  rotor
	p.rotType = 0;  p.rAngle = 0;
	p.rotBlades = 4;	p.rTwist = 0.04;	p.rotSize = make_int3(0,4,12 /*0,1,9*/);
	p.rotR = 3;  p.rotSpc = 0.7;
	//  2nd
	p.r2Dist = 0;	p.r2Angle = 0;  p.r2twist = 1;
	rVel = 0;  r2Vel = 1;


	///  cam   - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	strcpy(title, "No name");	bChapter = false;
	camPos = make_float3(0, 0.04, -0.44);	camRot.x = 5;  camRot.y = 0;
	
	ce = 0;  ca = 0;
	//  accels . . . . . .
	for (int i=0; i < NumAcc; i++)	{
		p.acc[i].pos = make_float3(0,0,0);  p.acc[i].size = make_float3(0.005,0.010,0.005);
		p.acc[i].acc = make_float3(0,10,0);  p.acc[i].type = ACC_Off;  }

	//  vis  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
	p.clrType = CLR_VelAcc;  p.brightness = 0.3;  p.contrast = 0.3;  p.iHue = 0;
	//  dye  ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` ` 
	p.dyeType = /*0,2*/0;  p.dyeClear = 2;  p.dyeFade = 1.f;
	p.dyePos = make_float3(0, -0.21, 0);  p.dyeSize = make_float3(0.008, 0.012, 0.008);
}


void Scene::Update()
{
	_UpdatePar();	_UpdateGrid();
}


//  ParticleR dependent params
void Scene::_UpdatePar()
{
	SimParams& p = params;
	p.minDist *= p.particleR;	p.h2 = p.h*p.h;
	spacing *= p.particleR;

	p.Poly6Kern = 315.0f / (64.0f * PI * pow(p.h, 9));
	p.SpikyKern = -0.5f* -45.0f / (PI * pow(p.h, 6));
	p.LapKern = 45.0f / (PI * pow( p.h, 6));

	p.minDens = 1.f/pow(p.minDens* p.restDensity, 2.f);
	// mass = dens*vol, vol = 4/3*pi*r^3
	p.particleMass = p.restDensity* 4.f/3.f* PI* pow(p.particleR, 3.f);

	p.distBndSoft *= p.particleR;
	p.distBndHard *= p.particleR;

	p.rotR *= p.particleR;  p.rotSpc *= p.rotR;
}


//  Grid    = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = 
void Scene::_UpdateGrid()
{
	SimParams& p = params;
	float b = p.distBndSoft - p.particleR;  float3 b3 = make_float3(b,b,b);
	p.worldMinD = p.worldMin + b3;  p.worldMaxD = p.worldMax - b3;	initMin += b3;  initMax -= b3;
	p.worldSize = p.worldMax - p.worldMin;	p.worldSizeD = p.worldMaxD - p.worldMinD;
	
	p.cellSize = make_float3(fCellSize,fCellSize,fCellSize);
	p.gridSize.x = ceil( p.worldSize.x / p.cellSize.x);  p.gridSize.y = ceil( p.worldSize.y / p.cellSize.y);
	p.gridSize.z = ceil( p.worldSize.z / p.cellSize.z);
	p.gridSize_yx = p.gridSize.y * p.gridSize.x;
	p.numCells = p.gridSize.x * p.gridSize.y * p.gridSize.z;

	if (p.collPos.x == -77.f)
		p.collPos = make_float4(p.worldMin.x+b- p.collR*1.2f, p.worldMin.y+b + p.collR*1.f, 0, 1);	//--
}
