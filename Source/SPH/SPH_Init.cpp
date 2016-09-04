#include "header.h"

#include "..\SPH\SPH.h"
#include "..\App\App.h"



cSPH::cSPH() : bInitialized(false)
{
	hPos = 0;  hVel = 0;
	dPos[0] = dPos[1] = 0;	dVel[0] = dVel[1] = 0;
	curPosRead = curVelRead = 0;  curPosWrite = curVelWrite = 1;
	
	LoadScenes();
}
cSPH::~cSPH() {  _FreeMem();  }



//  Init particles
//..............................................................................................................

void cSPH::Reset(int type)
{
	SimParams& p = scn.params;
	float r = p.particleR, spc = scn.spacing, b = p.distBndSoft;
	float3 vr = make_float3(r,r,r), cp = make_float3(p.collPos.x,p.collPos.y,p.collPos.z);
	float3 wMin = p.worldMinD, wSize = p.worldSizeD;
	float3 imin = scn.initMin, imax = scn.initMax;
	float4 pos, vel0 = make_float4(0,0,0,0);
	imin.y += r;	wMin.x+=r; wMin.y+=r; wMin.z+=r;  wSize.x-=2*r;wSize.y-=2*r;wSize.z-=2*r;
	uint i=0;

	if (type == 1)	//  random
		for(i=0; i < p.numParticles; ++i) 
		{
			hPos[i].x = wMin.x + wSize.x*frand();	hPos[i].y = wMin.y + wSize.y*frand();
			hPos[i].z = wMin.z + wSize.z*frand();	hPos[i].w = 1.f;  hVel[i] = vel0;
		}
	else  {		//  volume
		#define rst(a)		pos.##a = imin.##a;
		#define Inc(a)		pos.##a += spc;  if (pos.##a >= imax.##a)
		rst(x)  rst(y)  rst(z)  pos.w = 1;
		#define Inc3(a,b,c)		Inc(a) {  rst(a)  Inc(b) {  rst(b)  Inc(c) rst(c)  }  }
		#define INC			switch (scn.initLast)  {  default:\
				case 1:  Inc3(x,z,y);  break;\
				case 0:  Inc3(y,z,x);  break;\
				case 2:  Inc3(x,y,z);  break;	}

		//  pump
		if (p.bndType == BND_PUMP_Y)
		{
			float rad = scn.initMax.x, hc = p.hClose- b*0.90f, rin = rad * p.radIn -b/2,
				xs = sinf(p.angOut * p.s3)*rad -b/2 -p.particleR;

			while (i < p.numParticles)
			{
				if ((p.angOut <  0.5f && pos.y > 0 && pos.z < hc &&  pos.x < xs && pos.x > -xs) ||	// ^
					(p.angOut >= 0.5f && pos.y > 0 && pos.z < hc &&  pos.x > 0.09f*p.s4) ||		// S
					sqrt(pos.x*pos.x+pos.y*pos.y)*1.01f < (pos.z < hc ? rad : rin) )
				{	hPos[i] = pos;	hVel[i] = vel0;  i++;	}	INC
			}
		} else {

			while (i < p.numParticles)
			{
				if (length3(pos-cp) > p.collR || p.rotType > 0)  // outside collider
				if ( p.bndType == BND_BOX || scn.initType >= 9/**/ || p.bndType == BND_CYL_YZ || 
					(p.bndType == BND_CYL_Y && sqrt(pos.x*pos.x+pos.z*pos.z) <  imax.x) ||
					(p.bndType == BND_CYL_Z && sqrt(pos.x*pos.x+pos.y*pos.y) < -imin.y) ||
					(p.bndType == BND_SPHERE && length3(pos) < -imin.y) )
				{	hPos[i] = pos;	hVel[i] = vel0;  i++;	}	INC
			}
		}
	}
	setArray(0, hPos, 0, p.numParticles);
	setArray(1, hVel, 0, p.numParticles);
}


//..............................................................................................................

void cSPH::Drop(bool bRandom)	//  drop a sphere of particles
{
	SimParams& p = scn.params;
	float r = scn.dropR, spc = scn.spacing, r2 = r*r, db = (r+1)*spc, d = 0.5f;
	float3 r3 = {db,db,db};
	float4 vel0 = {0,0,0,0};  float3 pos = {0.5f, 0.5f, 0.5f};

	if (bRandom)
	{	BndType t = p.bndType;
		if (t == BND_BOX || t == BND_CYL_Y)  pos.y = 1.0f;  else  pos.y = 0.75f;
		if (t == BND_BOX || t == BND_CYL_Z)
		{	pos.x = frand();  pos.z = frand();	}
		else  //if (t == BND_CYL_Y)
		{	float r = frand(), k = random(0,2*PI);
			pos.x = d+d*cosf(k)*r;  pos.z = d-d*sinf(k)*r;	}
	}
	float3 posw = p.worldMinD+r3 + pos * (p.worldSizeD-r3*2);
	
	uint size = 0, id = App::emitId, a = id;
	for(int z=-r; z<=r; z++)
	for(int y=-r; y<=r; y++)
	for(int x=-r; x<=r; x++)
		if (x*x + y*y + z*z <= r2 && a < p.numParticles)
		{
			float3 pp = make_float3(x,y,z)*spc + posw;
			memcpy(&hPos[a], &pp.x, 4*sizeof(float));
			hVel[a] = vel0;		size++;  a++;
		}

	setArray(0, &hPos[id], id, size);
	setArray(1, &hVel[id], id, size);

	/* inc Id */  uint num = p.numParticles;
	App::emitId += size;  if (App::emitId >= num)  App::emitId -= num;
}
