#pragma once
#include "..\SPH\SPH.h"
#include "..\Graphics\render_particles.h"
#include "..\Graphics\paramgl.h"



static const int NumTabs = 12;
static const int NumTim = 6;	// cuda timers
static const int NumTimM = 5;	// view modes

class App
{
public:
	///  Vars
	//  Camera
	static  int mx, my, buttons, rendAq;
	static  float3 camPosLag, camRotLag, dyePos;
	static  float4 colliderPos;  static  float modelView[16], inertia;
	static  float fAspect, fFov,fNear, fBackgr,  foFov,foNear,foBackgr;

	//  Emitter
	static  int  emitId, cntRain;
	static  void UpdateEmitter();

	//  Info
	static  enum Modes { M_VIEW = 0, M_MOVE, M_EMIT, M_ACCEL, M_DYE,  M_ALL } mode;
	static  bool bSliders, bText, bInfo, bHelp, bXyz, bScale;  static int iTimes, WSizeX, WSizeY;;  // bShow
	static  bool bPaused, bWindowed, bVsyncOff, bShowInfo;  // init opts

	static  char s[256];
	static  uint timer[NumTim];  static float tm[NumTim],tdt,tdts,tm_si;
	static  int timCnt,timAvgCnt;  static float barsScale, fSimTime;
		   
	//  System
	static  cSPH* psys;  // SPH system
	static  ParticleRenderer* pParRend;
	
	//  Sliders
	static  ParamListGL* curParList, *parLists[NumTabs], *parLisE[NumEmit], *parLisA[NumAcc], *parLisV[2]; //vis,Hue
	static  char tabNames[NumTabs][20];  static  float3 tabClrs[NumTabs];
	static  int tabWidths[NumTabs], tabPosX[NumTabs], tabHidden[NumTabs], curTab, yParList, mselTab, nTabRow2, nChg;
	static  void  updHue(),updBrCnt();  static int iHueType, iClrType;
	static  float brightness,contrast, hue_bright,hue_contr;
		   
public:
	///  Methods
	static  void init(),  initParams(), destroy();
	static  void Render(),  RenderText(),RenderHelp(), DrawBounds(),DrawCollider();
	static  void Simulate();
	
	//  Input
	static  void Mouse(int button, int state, int x, int y);
	static  void Motion(int x, int y);
		   
	static  void KeyPressed(unsigned char key, int x, int y);
	static  void KeySpecial(int k, int x, int y);
	static  bool shift, ctrl;
												  
	//  other
	static  void UpdateCamera(), reshape(int w, int h);
	static  void mulTr(float *v, float *r, GLfloat *m);
	static  void prevTab(),nextTab(), prevClr(),nextClr(), setCurL(bool norm=true), updTabVis(), updVisParR(),
				prevMode(),nextMode(), prevEmit(),nextEmit(), prevAcc(),nextAcc(), togHue();
	//  draw
	static  void drawWireBox(float minX, float minY, float minY2, float minZ, float maxX, float maxY, float maxZ);
	static  void drawSphere(float r);
	static  void drawCircleY(float r, float y),	drawCircleZ(float r, float z), drawCircleX(float r, float x);	static  float aq;
	static  void drawCylY(float r, float minY, float maxY), drawCylZ(float r, float minZ, float maxZ);
};


extern "C" void cudaInit(int argc, char **argv);
extern "C" bool cudaInitB(int argc, char **argv, bool showInfo);
