#include "header.h"

#include "..\SPH\Scene.h"



//------------------------------------------------------------------------
///  Scene params form xml
//------------------------------------------------------------------------

void Scene::_FromXML(TiXmlElement* s)
{
	const char* a;	SimParams& p = params;
		
	#define  A(str)		if (a = s->Attribute(str))
	//  string in xml, variable in code
	#define  Af(str,var)	A(str)	{  var = toFloat(a);	}
	#define  Ai(str,var)	A(str)	{  var = toInt(a);		}
	#define  Av(str,var)	A(str)	{  var = toVec3(a);		}
	
	
	///  World  - - - - - - - - - - - - - - - - - - - - -
	#define  InitS(si)	initMin =	 si*-0.5f;	initMax =	 si*0.5f;
	#define  World(sw)	p.worldMin = sw*-0.5f;	p.worldMax = sw*0.5f;

	A("World")	{	World(toVec3(a))	InitS(toVec3(a))  }
	A("Init")	{	InitS(toVec3(a))	}

	Av("WorldMin",	p.worldMin);	Av("InitMin", initMin);
	Av("WorldMax",	p.worldMax);	Av("InitMax", initMax);
	
	Ai("InitType", initType);	Ai("InitLast", initLast);
	Af("CellSize", fCellSize);


	///  cam  - - - - - - - - - - - - - - - - - - - - - -
	A("name")	strncpy_s(title, sizeof(title), a, _TRUNCATE);
	A("chapter")	bChapter = true;
	Av("CamPos", camPos);	Av("CamRot", camRot);
	Af("dropR",	dropR);		Ai("rain", rain);

	//  emitter    . . . . . . . . . . . . . . . . . . .
	// one  in attr
	Ai("EmitSize",	emit[0].size);		Av("EmitPos", emit[0].pos);		Af("EmitVel", emit[0].vel);
	Ai("EmitSize2",	emit[0].size2);		Av("EmitRot", emit[0].rot);
	// many  in <>
	TiXmlElement* ss = s;	int i = 0;
	s = ss->FirstChildElement("Emitter");
	while (s && i < NumEmit)
	{	Ai("size",	emit[i].size);	 Av("pos", emit[i].pos);  Af("vel", emit[i].vel);
		Ai("size2",	emit[i].size2);	 Av("rot", emit[i].rot);
		s = s->NextSiblingElement("Emitter");  i++;  }	s = ss;
	
	//  accels    . . . . . . . . . .
	// many  in <>
	ss = s;  i = 0;  s = ss->FirstChildElement("Accel");
	while (s && i < NumAcc)
	{	A("type")  p.acc[i].type = (AccType)toInt(a);
		Av("pos", p.acc[i].pos);  Av("size", p.acc[i].size);  Av("acc", p.acc[i].acc);
		s = s->NextSiblingElement("Accel");  i++;  }	s = ss;
		
	//  collider  . . . . . . . . . . . . . . . . . . . .
	Af("ColliderR", p.collR);
	A("ColliderPos")  {  float3 v = toVec3(a);  p.collPos = make_float4(v.x, v.y, v.z, 1);  }
	Af("spring", p.spring);  Af("damping", p.damping);  Af("shear", p.shear);


	///  simulate  . . . . . . . . . . . . . . . . . . . 
	Ai("Particles", p.numParticles);	A("ParticlesK")  {  p.numParticles = toInt(a)*1024;  }
	Ai("maxParInCell", p.maxParInCell);
	Af("TimeStep", p.timeStep);		Af("globalDamping", p.globalDamping);
	Av("Gravity", p.gravity);

	///  SPH init   * * * * * * * * * * * * * * * * * * *
	Af("particleR", p.particleR);	Af("minDist", p.minDist);
	Af("particleH", p.h);			Af("spacing", spacing);

	Af("RestDensity", p.restDensity);	Af("minDens", p.minDens);
	Af("Stiffness", p.stiffness);		Af("Viscosity", p.viscosity);

	//  Boundary  * * * * * * * * * * * * * * * * * * * *
	Af("distBndSoft", p.distBndSoft);	Af("bndStiff", p.bndStiff);
	Af("distBndHard", p.distBndHard);	Af("bndDamp", p.bndDamp);	Af("bndDampC", p.bndDampC);
	A("bndType")  p.bndType = (BndType)toInt(a);
	A("bndEffZ")  p.bndEffZ = (BndEff)toInt(a);
	A("HmapType")  p.iHmap = toInt(a);
	

	//  pump  * * * * * * * * * * * * * * * * * * * * * *
	Af("PumpAngOut", p.angOut);  Af("PumpHClose", p.hClose);  Af("PumpRadIn", p.radIn);
	Af("ExitVel", p.rVexit)  Af("ExitDist", p.rDexit);
	Af("s1", p.s1);  Af("s2", p.s2);  Af("s3", p.s3);  Af("s4", p.s4);  Af("s5", p.s5);  Af("s6", p.s6);
	//  rotor
	A("RotorSizes")  {  float3 v = toVec3(a);  p.rotSize = make_int3(v.x, v.y, v.z);  }
	Ai("RotorType", p.rotType);		Af("Rotor2Dist", p.r2Dist);
		Af("RotorVel", rVel);  Af("Rotor2Vel", r2Vel);
		A("RotorAngle")   p.rAngle = toFloat(a) *PI/180.f;
		A("Rotor2Angle")  p.r2Angle = toFloat(a) *PI/180.f;
	Af("colParR", p.rotR);  Af("colParSpc", p.rotSpc);
	Ai("RotorBlades", p.rotBlades);  Af("RotorTwist", p.rTwist);
	
	///  waves (aliases) . . . . . . . . . . . . . . . . .
	A("WaveSpeed")  {  rVel = toFloat(a);  p.rAngle = -PI/2.f;  }
	Af("WaveAmpl", p.rTwist);	Af("HSlope", p.r2Angle);

	//  Hmap  (aliases)
	Af("Hheight", p.r2Angle);	Af("Hscale", p.hClose);
	Af("HxFq", p.s1);	Af("HxOfs", p.s2);
	Af("HzFq", p.s3);	Af("HzOfs", p.s4);	Af("HholeR", p.s5);
}