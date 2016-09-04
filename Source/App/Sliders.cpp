#include "header.h"
#include "App.h"
#include "..\CUDA\Params.cuh"



///  Params
///-----------------------------------------------------------------------------
void App::updVisParR()
{
	SimParams* p = &psys->scn.params;
	pParRend->m_ParRadius =  p->particleR * 0.71; /*0.15  0.71*/
	fNear = p->particleR / 0.004 * 0.15f;
}
void App::initParams()
{
	// create parameter list
	Scene* sc = &psys->scn;
	SimParams* p = &sc->params;

	int x = 0;	//  init tabs
	for (int i=0; i<NumTabs; i++)
	{	parLists[i] = new ParamListGL(tabNames[i]);
		tabWidths[i] = strlen(tabNames[i]) *7.7;  //*10;
		if (i==nTabRow2)  x = 0;
		tabPosX[i] = x;  x += tabWidths[i] -1;
	}
	for (int i=0; i<NumEmit; i++)	parLisE[i] = new ParamListGL(tabNames[2]);
	for (int i=0; i<NumAcc; i++)	parLisA[i] = new ParamListGL(tabNames[3]);
	for (int i=0; i<2; i++)			parLisV[i] = new ParamListGL(tabNames[5]);
	
	#define  addF  parList->AddParam(new Param<float>
	#define  addI  parList->AddParam(new Param<int>
	#define  addE  parList->AddEmpty();

	
	///  Simulation  ------
	ParamListGL* parList = parLists[0];
	addF("Time step", 0, 0.004, 0.0001,	&p->timeStep, 1.5));
	addF("global damp", 0.9, 1, 0.002,	&p->globalDamping));
	addE
	addF("Gravity y",	-20, 0, 0.5,	&p->gravity.y));
	addF("accel z",		-5,  5,	0.2,	&p->gravity.z));
	addE
	addF("Stiffness",	0.9,100, 0.5,	&p->stiffness, 3.0));
	addF("Viscosity",	0.05, 4, 0.1,	&p->viscosity, 1.5));
	
	
	///  Collider
	parList = parLists[1];
	addF("Radius",	0.001, 0.1, 0.005,	&p->collR));
	addE
	addF("spring",		0, 200, 8,		&p->spring));
	addF("damping",		0, 0.3, 0.01,	&p->damping));
	addF("shear",		0, 0.3, 0.01,	&p->shear));
	addE
	addI("Type",		0, 4,	1,		&p->rotType));

	
	///  Emitters
	parList = parLists[2];	addE
	for (int e = 0; e < NumEmit; e++)	{
		parList = parLisE[e];
		addI("Size",		0, 10,	1,		&sc->emit[e].size));
		addI("Size2",		0, 10,	1,		&sc->emit[e].size2));
		addE
		addF("Velocity",	0, 5,	0.05,	&sc->emit[e].vel));
		addE
		addF("Rot x",	  -90, 90,	15,		&sc->emit[e].rot.x));
		addF("Rot y",		0, 360,	15,		&sc->emit[e].rot.y));
		addE
		addF("Drop R",		0.2, 20, 0.5,	&sc->dropR));
		addI("Rain interval", 0, 200, 1,	&sc->rain, 2));	}
	
	///  Accels
	parList = parLists[3];	addE
	for (int a = 0; a < NumAcc; a++)	{
		parList = parLisA[a];
		addF("Size x (r1)",	0, 0.6,	0.01,	&p->acc[a].size.x, 3));
		addF("Size y",		0, 0.6,	0.01,	&p->acc[a].size.y, 3));
		addF("Size z (r2)",	0, 0.6,	0.01,	&p->acc[a].size.z, 3));
		addE	const float am = 250;
		addF("Accel x",	  -am, am,	1,		&p->acc[a].acc.x));
		addF("Accel y",	  -am, am,	1,		&p->acc[a].acc.y));
		addF("Accel z",	  -am, am,	1,		&p->acc[a].acc.z));
		addE
		addI("Type",	  0, ACC_ALL-1, 1,	(int*)&p->acc[a].type));	}
	

	///  Dye  ````````
	parList = parLists[4];
	addI("type",		0, 2, 1,		(int*)&p->dyeType));
	addF("Fade speed",	0, 20, 0.1,		&p->dyeFade, 4));
	addE	const float md = 0.0005;
	addF("size x",		md, 0.4, 0.01,	&p->dyeSize.x, 3));
	addF("size y (r)",	md, 0.4, 0.01,	&p->dyeSize.y, 3));
	addF("size z",		md, 0.4, 0.01,	&p->dyeSize.z, 3));

	
	///  Visual  ***
	parList = parLists[5];  addE
	parList = parLisV[0];
	addF("brightness",	0, 0.5, 0.02,	&brightness));
	addF("contrast",	0, 1.5, 0.02,	&contrast));
	addE
	addF("L ambient",	0, 1,	0.05,	&pParRend->m_fAmbient));
	addF("L diffuse",	0, 2,	0.05,	&pParRend->m_fDiffuse, 1.5));
	addF("L power",		0, 4,	0.05,	&pParRend->m_fPower, 2));
	
	///  hue scale
	parList = parLisV[1];
	addF("diffuse",	0, 0.5, 0.05,	&pParRend->m_fHueDiff, 1.5));
	addF("offset",	0, 1.0, 0.02,	&hue_bright, 1.5));
	addE
	addF("Range",	0.001, 2.0, 0.02,&hue_contr, 1.5));
	addF("Steps",	0, 60, 1,		&pParRend->m_fSteps, 1.5));
	
	
	///  Camera  ***
	parList = parLists[6];
	addF("particle R",	0.0001, 0.004, 0.0002, &pParRend->m_ParRadius, 1.5));
	addI("rotor Quality",1, 16,	1,		&App::rendAq, 2));
	addE
	addF("Near Cut", 0.01, 0.5, 0.004,	&fNear));
	addF("Fov",			15, 90, 1.5,	&fFov, 0.7));
	addF("inertia",		0, 0.3, 0.04,	&inertia, 2));
	addE
	addF("Background",	0, 3,	0.05,	&fBackgr));
	
	
	///  Bounds  ------
	parList = parLists[7];
	addF("stiffness",	0, 160000, 1000,&p->bndStiff));
	addF("damping",		0, 500, 10,		&p->bndDamp));
	addF("cyl damping",	0, 500, 10,		&p->bndDampC));
	addE
	addF("exit vel",	0.0, 1.5, 0.01, &p->rVexit, 2));//-
	addF("exit dist", 0.00, 0.08, 0.004,&p->rDexit));//-
	addE
	addI("Hmap Type",	0, 2,	1,		&p->iHmap)); // +

	///  waves  .
	parList = parLists[8];
	addF("Speed",		0, 12, 0.5,		&sc->rVel, 1.5));
	addF("Amplitude",	0, 0.4, 0.004,	&p->rTwist));
	addF("slope H",		0, 0.4, 0.004,	&p->r2Angle));

	
	///  Pump  ------
	parList = parLists[9];
	addF("Height",	 -0.07, 0.03, 0.005,&p->hClose));
	addF("In radius",	0.1, 0.8, 0.01,	&p->radIn));
	addF("Out angle",	0.1, 1.0, 0.01,	&p->angOut));
	addE
	addF("s1 hcl cyl",	0.6, 1.0, 0.01,	&p->s1));
	addF("s2 hcl flat",	0.6, 1.0, 0.01,	&p->s2));
	addF("s3 out^ang xs",1.1,2.2, 0.01,	&p->s3));
	addF("s4 out^zs",	0.7, 1.0, 0.01,	&p->s4));
	addF("s5 cylS rad",	0.7, 1.0, 0.01,	&p->s5));
	addF("s6 outS rad",	0.7, 1.0, 0.01,	&p->s6));

	
	///  Rotor
	parList = parLists[10];
	addF("Speed rad/s",-10, 40, 1,	&sc->rVel, 1.5));	//35
	addE
	addI("Width",		0, 10,	1,		&p->rotSize.x));
	addI("Height",		0, 14,	1,		&p->rotSize.y));
	addI("Length",		0, 24,	1,		&p->rotSize.z));
	addE
	addI("Blades",		1, 8,	1,		&p->rotBlades));
	addF("Twist",	 -0.5, 0.5,	0.004,	&p->rTwist));
	addE
	addF("2nd dist",	0, 0.3, 0.01,	&p->r2Dist));
	addF("speed2 mul",	-2, 2, 1,		&sc->r2Vel));
	
	
	///  Hmap ...
	parList = parLists[11];
	addF("Y Height",   -0.8, 0.8, 0.01, &p->r2Angle));
	addF("Y Scale",		0.0, 0.8, 0.01,	&p->hClose));
	addE
	addF("X sin fq",	0.0, 6.0, 0.1,	&p->s1, 2));
	addF("X ofs",		-180, 180, 15,	&p->s2));
	addF("Z sin fq",	0.0, 6.0, 0.1,	&p->s3, 2));
	addF("Z ofs",		-180, 180, 15,	&p->s4));
	addE
	addF("hole R from",	-1.0, 1.0, 0.05,&p->s5, 2.5));
	//addF("s6 power",	0.0, 8.0, 0.01,	&p->s6));
	//addF("Out angle",	0.1, 1.0, 0.01,	&p->angOut, radIn));
	//addE
	//addF("radius",		0, 0.04, 0.004,	&p->rotR,   1.5));
	//addF("spacing",		0, 0.03, 0.004,	&p->rotSpc, 1.5));

	curTab = 0;  setCurL();  updHue();	updVisParR();
}