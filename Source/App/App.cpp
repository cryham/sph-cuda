#include "header.h"

#include "App.h"
//#include "..\SPH\SPH.h"


//  static vars
int App::mx, App::my, App::buttons = 0, App::rendAq = 6;
bool App::shift = 0, App::ctrl = 0;
float3 App::camPosLag,  App::camRotLag, App::dyePos;  float4 App::colliderPos;
int App::emitId = 0, App::cntRain = 0;
float App::modelView[16], App::fAspect, App::inertia = 0.06;

App::Modes App::mode = M_VIEW;
bool App::bWindowed = 0, App::bVsyncOff = 1, App::bShowInfo = 1;	int App::iTimes = 0, App::WSizeX = 1600, App::WSizeY = 1170;
bool App::bPaused = 0, App::bSliders = 1, App::bText = 0, App::bInfo = 0, App::bHelp = 0, App::bXyz = 0, App::bScale = 1;
float App::fFov = 67.5f, App::fNear = 0.15f, App::fBackgr = 1.f, App::foFov,App::foNear,App::foBackgr;

char App::s[256];	uint App::timer[NumTim] = {0,};  float App::tm[NumTim] = {0.f,}, App::tdt = 0.f, App::tdts = 0.f, App::tm_si = 0.f;
float App::aq = PI2/40.f, App::barsScale = 30.f, App::fSimTime = 0.f;  int App::timAvgCnt = 2, App::timCnt = 0;

cSPH* App::psys = NULL;  ParticleRenderer* App::pParRend = NULL;

ParamListGL* App::curParList = NULL, *App::parLists[NumTabs] = {NULL,}, *App::parLisE[NumEmit] = {NULL,}, *App::parLisA[NumAcc] = {NULL,}, *App::parLisV[2] = {NULL,};
int App::tabWidths[NumTabs], App::tabPosX[NumTabs], App::tabHidden[NumTabs]={0,},
	App::yParList = 200, App::curTab = 0, App::mselTab = -1, App::nTabRow2 = 5, App::nChg = 0;
char App::tabNames[NumTabs][20] =
	{"Simulation", "Collider",	"Emitter  ", "Accel   ",  "Dye   ",
	 "Visual ", "Camera  ",		"Bounds  ", "Waves  ",	"Pump  ", "Rotor ", "Hmap   " };
float3 App::tabClrs[NumTabs] = {
	{0.1,0.4,1.0}, {0.2,0.5,0.9},  {0.6,0.6,0.1}, {0.9,0.2,0.3},  {0.5,0.5,0.5},
	{0.4,0.5,1.1}, {0.5,0.4,1.1},  {0.8,0.4,0.1}, {0.1,0.7,0.8},  {0.3,0.6,0.1}, {0.1,0.6,0.3}, {0.7,0.2,0.45} }; // {0.4,0.5,0.2} 
// vis
float App::brightness = 0.3, App::contrast = 0.3, App::hue_bright = 0.04, App::hue_contr = 0.2;




///  Init  ----------------------------------------------------------------------
void App::init()
{
	glEnable(GL_DEPTH_TEST);
	glutReportErrors();

	psys = new cSPH(); 

	pParRend = new ParticleRenderer;
	pParRend->setColorBuffer(psys->colorVbo);

	for (int i=0; i<NumTim; i++)
	if (CUT_SAFE_CALL(cutCreateTimer(&timer[i])) == CUTFalse)
		printf("Can't create timer %d !\n", i);

	initParams();

	if (!bWindowed)	glutFullScreen();
	if (bVsyncOff)	wglSwapIntervalEXT(0);
}

#define  DEL(a)   if (a)  {  delete a;  a = NULL;  }
void App::destroy()
{
	DEL(psys);	DEL(pParRend);	int i;
	
	for (i=0; i<NumTabs; i++)	DEL(parLists[i]);
	for (i=0; i<NumEmit; i++)	DEL(parLisE[i]);
	for (i=0; i<NumAcc; i++)	DEL(parLisA[i]);
	for (i=0; i<2; i++)			DEL(parLisV[i]);

	for (i=0; i<NumTim; i++)
	if (CUT_SAFE_CALL(cutDeleteTimer(timer[i])) == CUTFalse)
		printf("Can't delete timer %d !\n", i);
}


void App::reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);  glLoadIdentity();
	fAspect = (float)w / (float)h;
	gluPerspective(fFov, fAspect, fNear, 100.0);

	glMatrixMode(GL_MODELVIEW);	 glLoadIdentity();
	glViewport(0, 0, w, h);

	pParRend->setWindowSize(w, h);
	pParRend->setFOV(fFov);
}

void idle()
{
	glutPostRedisplay();
}


//-----------------------------------------------------------------------------
///  main
//-----------------------------------------------------------------------------

int main(int argc, char** argv)
{
	cSPH::LoadOptions();

	if (!cudaInitB(argc, argv, App::bShowInfo)) {  fprintf(stderr, "Press Enter to exit.");  getchar();  exit(-1);  }
	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowSize(App::WSizeX, App::WSizeY);
	glutCreateWindow("CUDA SPH");
	glewInit();
	if (!glewIsSupported("GL_VERSION_2_0 GL_VERSION_1_5 GL_ARB_multitexture GL_ARB_vertex_buffer_object"))
	{	fprintf(stderr, "Required OpenGL extensions missing:\n" " GL_VERSION_2_0 GL_ARB_multitexture GL_ARB_vertex_buffer_object\n" "Press Enter to exit.");
		getchar();  exit(-1);  }

	App::init();
	{
		glutDisplayFunc(&App::Render);	glutReshapeFunc(&App::reshape);
		glutMouseFunc(&App::Mouse);		glutMotionFunc(&App::Motion);
		glutPassiveMotionFunc(&App::Motion);
		glutKeyboardFunc(&App::KeyPressed);	glutSpecialFunc(&App::KeySpecial);
		glutIdleFunc(idle);

		glutMainLoop();
	}
	App::destroy();
	return 0;
}
