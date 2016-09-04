#pragma once

#include "..\CUDA\Params.cuh"
#include "..\pch\timer.h"
#include "..\SPH\Scene.h"



class cSPH
{
public:		///  Methods

	cSPH();  ~cSPH();
	void _InitMem(), _FreeMem();	bool bInitialized;

	void Update();	// simulate
	void Reset(int type), Drop(bool bRandom);  // init volume
	float3 DropPos;

	//  Scenes
	static void LoadOptions();
	void LoadScenes(), InitScene(), NextScene(bool chapter=false),PrevScene(bool chapter=false),UpdScene();
	vector<Scene> scenes;	Scene scn;	int curScene;

	//  utility
	float4* getArray(bool pos);
	void setArray(bool pos, const float4* data, int start, int count);

	uint getPosBuffer() const {  return posVbo[curPosRead];  }

	uint createVBO(uint size);
	void colorRamp(float t, float *r);


public:		///  Data

	//  h-host CPU,  d-device GPU
	float4*  hPos,*hVel, *dPos[2],*dVel[2], *dSortedPos,*dSortedVel;

	uint*  hParHash,*dParHash[2], *hCellStart,*dCellStart;
	int*  hCounters,*dCounters[2];  // debug

	float* dPressure, *dDensity, *dDyeColor;

	uint posVbo[2], colorVbo;	//  GL vbo
	uint curPosRead, curVelRead, curPosWrite, curVelWrite;  // swap

	//  timer-
	Timer tim;	//float t1,t2,t3,t4,t5;
};
