#pragma once

#include "..\CUDA\Params.cuh"



class Emitter
{
public:
	float3  pos,rot, posLag,rotLag;
	float vel;  int size, size2;

	Emitter() : vel(1.5f), size(0), size2(0)  {  pos.x=pos.y=pos.z=0.f;  rot.x=rot.y=0.f;  posLag=pos;  rotLag=rot;  }
	
	void IncSize()  {  if (size < 10)  size++;  }
	void DecSize()  {  if (size > 0)   size--;  }
};

const static int NumEmit = 4;	// max


class Scene
{
public:
	//  name
	char title[40];
	bool bChapter;

	//  simulation params
	SimParams params;	float fCellSize;

	//  init params
	float3 initMin, initMax;
	int initType, initLast/*012 xyz*/;
	float spacing;  // init distance

	//  camera
	float3 camPos, camRot;
	float4 collidPos;

	//  emitter
	int ce;  // current	index
	Emitter emit[NumEmit];
	
	float dropR;  int rain;
	
	//  accel
	int ca;  // current
	float3 accPos[NumAcc];
	
	// rotor
	float rVel, r2Vel;
	
	
	///  update
	Scene()	{	InitDefault();	Update();  }	//  default scene only
	Scene(TiXmlElement* s)  {
		InitDefault();  _FromXML(s);  Update();	}	//  From XML

	void InitDefault(), _FromXML(TiXmlElement* s),
		Update(), _UpdatePar(),_UpdateGrid();
};
