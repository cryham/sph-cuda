#include "header.h"

#include "..\SPH\SPH.h"
#include "..\CUDA\System.cuh"
#include "..\CUDA\radixsort.cuh"
#include "..\Graphics\param.h"
#include "..\App\App.h"



///----------------------------------------------------------------------------------------------------------------------------------
///  Update
///----------------------------------------------------------------------------------------------------------------------------------
void cSPH::Update()
{
	if (!bInitialized)  return;
	/*-*/tim.update(true);

	//  update sim constants, when changed
	if (ParamBase::bChangedAny == true)
	{	ParamBase::bChangedAny = false;  /**/if (App::nChg < 20)  App::nChg += 10;
		App::updTabVis();/*-*/	App::updBrCnt();
		setParameters(&scn.params);
		/*tim.update();	t5 = 1000.0*tim.dt;	/**/	}
	
	SimParams& p = scn.params;
	uint2* parHash = (uint2*)dParHash[0];


	///  integrate  (and boundary)
	//**/hCounters[0] = 0;  hCounters[1] = 0;
	//**/copyToDevice(dCounters[curPosWrite], hCounters, 0, 4*sizeof(int));

	CUT_SAFE_CALL(cutStartTimer(App::timer[1]));
	
	integrate(posVbo[curPosRead], posVbo[curPosWrite],
		dVel[curVelRead], dVel[curVelWrite],  p.numParticles/*, dCounters[curPosWrite]*/);
	
	CUT_SAFE_CALL(cutStopTimer(App::timer[1]));
	
	//**/copyFromDevice(hCounters, dCounters[curPosRead], 0, 4*sizeof(int));

	swap(curPosRead, curPosWrite);
	swap(curVelRead, curVelWrite);

	// debug -slow
	//copyFromDevice(hPos, 0, posVbo[curPosRead], sizeof(float)*4*50/*p.numParticles*/);
	//copyFromDevice(hVel, 0, colorVbo2/*dVel[curVelRead], 0,*/, sizeof(float)*4*50/*p.numParticles*/);


	///  sort  calculate hash & sort particles
	CUT_SAFE_CALL(cutStartTimer(App::timer[2]));

	calcHash(posVbo[curPosRead], parHash, p.numParticles);
	RadixSort((KeyValuePair*) dParHash[0], (KeyValuePair*) dParHash[1], p.numParticles,
		/*bits*/p.numCells >= 65536 ? 32 : 16);

	CUT_SAFE_CALL(cutStopTimer(App::timer[2]));


	///  reorder particle arrays into sorted order and find start of each cell
	CUT_SAFE_CALL(cutStartTimer(App::timer[3]));

	reorder(posVbo[curPosRead], dVel[curVelRead], dSortedPos, dSortedVel,
		parHash, dCellStart,  p.numParticles, p.numCells);

	CUT_SAFE_CALL(cutStopTimer(App::timer[3]));


	///  collisions  (sph density & force)
	CUT_SAFE_CALL(cutStartTimer(App::timer[5]));
	
	collide(App::timer[4], posVbo[curPosRead], posVbo[curPosWrite], /**/colorVbo,
		dSortedPos, dSortedVel, dVel[curVelRead], dVel[curVelWrite],
		dPressure, dDensity, dDyeColor,//
		parHash, dCellStart,  p.numParticles,	p.numCells);
	
	CUT_SAFE_CALL(cutStopTimer(App::timer[5]));

	swap(curVelRead, curVelWrite);
}
///----------------------------------------------------------------------------------------------------------------------------------
