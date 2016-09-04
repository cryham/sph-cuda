#include "header.h"
#include "App.h"
#include "..\SPH\SPH.h"



///  Update   . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .	.

void App::UpdateEmitter()
{
	Scene& sc = psys->scn;
	SimParams* p = &sc.params;

	//  inc rotor angle
	if (sc.rVel != 0.f)
	{	p->rAngle += sc.rVel * p->timeStep;
		if (p->r2Dist > 0.f)
		{
			p->r2Angle += sc.rVel*sc.r2Vel * p->timeStep;
			p->r2twist = sc.r2Vel < 0.f ? -1.f: 1.f;
		}
		ParamBase::Changed();  }
		
	if (p->dyeClear > 0)  {  p->dyeClear--;  ParamBase::Changed();  }


	//  coll pos upd  </-- fun(pos&) ...
	const float mind = 0.000707f;  // need update dist
	const float mind2 = mind*mind;
	float4 d = colliderPos - p->collPos;
	if (abs(d.x) > mind || abs(d.y) > mind || abs(d.z) > mind)
	//if (d.x*d.x + d.y*d.y + d.z*d.z > mind2)
	{
		for (int iter=0; iter<3; ++iter)
		{	p->collPos += d * inertia;
			d = colliderPos - p->collPos;	}
		ParamBase::Changed();
	}

	//  acc pos upd
	Accel& ac = p->acc[sc.ca];
	float3 ad = sc.accPos[sc.ca] - ac.pos;
	if (ad.x*ad.x + ad.y*ad.y + ad.z*ad.z > mind2)
	{
		for (int iter=0; iter<3; ++iter)
		{	ac.pos += ad * inertia;
			ad = sc.accPos[sc.ca] - ac.pos;	}
		ParamBase::Changed();
	}

	//  dye pos upd
	//float3 ad = sc.accPos[sc.ca] - ac.pos;
	float3 dd = dyePos - p->dyePos;
	if (abs(dd.x) > mind || abs(dd.y) > mind || abs(dd.z) > mind)
	{
		for (int iter=0; iter<3; ++iter)
		{	p->dyePos += dd * inertia;
			dd = dyePos - p->dyePos;	}
		ParamBase::Changed();
	}

	//  emitters
	for (int e = 0; e < NumEmit; e++)
	{
		Emitter& em = sc.emit[e];
		if (em.size > 0)
		{	int eX = em.size, eY = (em.size2 == 0) ? eX : em.size2, size = eX*eY;
			float spc = sc.spacing;
			static float4 pos[100], vel[100];  // [max_emitRate^2
			
			//  rotation
			glLoadIdentity();   glRotatef(em.rotLag.x, 1,0,0);  glRotatef(-em.rotLag.y, 0,1,0);
			glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
			float3 ev = make_float3(0,0, em.vel/* (0.003f/p->timeStep)*/);//
			float4 vel4;	mulTr((float*)&ev, &vel4.x, modelView);

			int i = 0;	float z = (eX-1)*0.5f, z2 = (eY-1)*0.5f;
			for (int y=0; y < eY; y++)
			for (int x=0; x < eX; x++, i++)
			{
				float3 pp = make_float3((x-z)*spc, (y-z2)*spc, -spc);  mulTr(&pp.x, &pos[i].x, modelView);
				pos[i] += em.posLag;	vel[i] = vel4;
			}
			
			psys->setArray(0, pos, emitId, size);  // set
			psys->setArray(1, vel, emitId, size);

			/* inc Id */  uint num = sc.params.numParticles;  // -size?
			emitId += size;  if (emitId >= num)  emitId -= num;
		}
	}
	
	if (sc.rain > 0)	//  Rain
	{	cntRain++;	if (cntRain >= sc.rain)
		{	cntRain = 0;	psys->Drop(true);	}	}
}
