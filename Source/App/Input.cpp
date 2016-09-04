#include "header.h"
#include "App.h"
#include "..\CUDA\Params.cuh"



///  Mouse move
//-----------------------------------------------------------------------------
void App::Motion(int x, int y)
{
	float sp = 0.001, spr = 0.07;	// speed,rot
	float dx = x - mx, dy = y - my;
	
	if (bSliders)
	if (curParList->Motion(x, y))  {	mx = x;  my = y;	return;  }

	///  sel tabs  ****
	mselTab = -1;	if (bSliders)
	if (x < tabPosX[NumTabs-1] + tabWidths[NumTabs-1])
	//if (x < tabPosX[nTabRow2-1] + tabWidths[nTabRow2-1])
	if (y > yParList-23 && y < yParList + 10)  // 1st line
	{	for (int i=0; i<nTabRow2; i++)  if (x > tabPosX[i])  mselTab = i;	}  // search
	else  if (y >= yParList + 10 && y < yParList + 38)  // 2nd
	{	for (int i=nTabRow2; i<NumTabs; i++)  if (x > tabPosX[i])  mselTab = i;	}
	
	Scene& q = psys->scn;
	float3* p = NULL;

	if (mselTab == -1 && buttons > 0)  switch (mode)  {
		case M_VIEW:
			if (buttons == 2)		{	q.camPos.z += dy *sp;	}	// M  zoom
			else if (buttons == 1)	{	q.camPos.x += dx *sp;	q.camPos.y -= dy *sp;	}	// L  move
			else if (buttons == 4)	{	q.camRot.x += dy *spr;	q.camRot.y += dx *spr;	}	// R  rotate
			break;

		case M_EMIT:
			{	Emitter& em = q.emit[q.ce];
				if (buttons == 2)
				{	// M  rot emit
					em.rot.x -= dy *spr*2;	em.rot.y -= dx *spr*2;
					if (em.rot.x > 90)	em.rot.x = 90;  if (em.rot.x <-90)	em.rot.x = -90;
					if (em.rot.y >360)	{	em.rot.y -= 360;  em.rotLag.y -= 360;  }  // for sliders
					if (em.rot.y < 0)	{	em.rot.y += 360;  em.rotLag.y += 360;  }
				}
				p = &em.pos;	}	break;
		case M_MOVE:  p = (float3*)&colliderPos;   break;
		case M_ACCEL: p = &q.accPos[q.ca];   break;
		case M_DYE:   p = &dyePos;/*&q.params.dyePos;*/  break;
	}

	if (mode != M_VIEW && p)
	{	float a = q.camRot.y *PI/180.f, c = cosf(a), s = sinf(a);
		dx *= sp;  dy *= sp;

		// L  move xz
		if (buttons == 1)		{	p->x += dx*c - dy*s;  p->z += dx*s + dy*c;	}
		// R  move xy
		else if (buttons == 4)	{	p->x += dx*c;  p->y -= dy;  p->z += dx*s;	}
	}
	mx = x;  my = y;
}


///  next/prev, update
//-----------------------------------------------------------------------------
int App::iHueType = 2, App::iClrType = CLR_VelAcc;
void App::updHue() {
	int h = pParRend->m_nProg;	psys->scn.params.iHue = h;
	psys->scn.params.clrType = (h == 1) ? (ClrType)HueType[iHueType] : (ClrType)iClrType;
	ParamBase::Changed();/*will call updBrCnt*/  }
void App::updBrCnt()
{
	if (pParRend->m_nProg==0)  {
		psys->scn.params.brightness = brightness;
		psys->scn.params.contrast = contrast;	}
	else  {  // hue
		psys->scn.params.brightness = hue_bright;
		psys->scn.params.contrast = hue_contr;	}
}

void App::prevClr() {	if (pParRend->m_nProg)
	iHueType = (iHueType -1 + HUE_ALL) % HUE_ALL; else
	iClrType = (iClrType -1 + CLR_ALL) % CLR_ALL;	updHue();	}
void App::nextClr() {	if (pParRend->m_nProg)
	iHueType = (iHueType +1) % HUE_ALL; else
	iClrType = (iClrType +1) % CLR_ALL;		updHue();	}

void App::prevMode() {	mode = (Modes)( ((int)(mode) - 1 + M_ALL) % M_ALL );	updTabVis();  }
void App::nextMode() {	mode = (Modes)( ((int)(mode) + 1) % M_ALL );  updTabVis();  }

void App::prevTab() {	do {  curTab = (curTab-1  + NumTabs) % NumTabs;  }  while (tabHidden[curTab]==1);	setCurL();  }
void App::nextTab() {	do {  curTab = (curTab+1) % NumTabs;			 }  while (tabHidden[curTab]==1);	setCurL();  }

void App::prevEmit() {	psys->scn.ce = (psys->scn.ce-1  + NumEmit) % NumEmit;	setCurL();  }
void App::nextEmit() {	psys->scn.ce = (psys->scn.ce+1) % NumEmit;				setCurL();  }
void App::prevAcc() {	psys->scn.ca = (psys->scn.ca-1  + NumAcc) % NumAcc;	setCurL();  }
void App::nextAcc() {	psys->scn.ca = (psys->scn.ca+1) % NumAcc;			setCurL();  }


void App::setCurL(bool norm) {
	switch (curTab)  {
		case 2:  curParList = parLisE[psys->scn.ce];  break;
		case 3:  curParList = parLisA[psys->scn.ca];  break;
		case 5:  curParList = parLisV[pParRend->m_nProg];  break;
		default:  curParList = parLists[curTab];  break;	}
	tabNames[2][8] = '1'+psys->scn.ce;
	tabNames[3][6] = '1'+psys->scn.ca;  // num
	if (norm)  updTabVis();  }

void App::updTabVis()	// hide unused tabs
{
	SimParams& p = psys->scn.params;
	tabHidden[8] = p.bndEffZ != BND_EFF_WAVE ? 1:0;
	tabHidden[9] = p.bndType != BND_PUMP_Y ? 1:0;
	tabHidden[10]= p.rotType == 0 ? 1:0;
	tabHidden[4] = p.dyeType > 0 || mode == M_DYE ? 0:1;
	tabHidden[11]= p.iHmap == 0 ? 1:0;

	bool b = false;  int i=0;  // search active emitters
	if (psys->scn.rain > 0)  b = true;  else
	if (mode == M_EMIT)  b = true;  else
	while (!b && i < NumEmit)  {  if (psys->scn.emit[i].size > 0)  b = true;  i++;  }
	tabHidden[2] = !b;
	
	b = false;  i=0;	// accels
	if (mode == M_ACCEL)  b = true;  else
	while (!b && i < NumAcc)  {  if (p.acc[i].type != ACC_Off)  b = true;  i++;  }
	tabHidden[3] = !b;
	
	// next if this tab hidden
	if (tabHidden[curTab]==1)
	{	do {  curTab = (curTab+1) % NumTabs;  }  while (tabHidden[curTab]==1);
		setCurL(false);  }
}

void App::togHue()	{
	pParRend->m_nProg = pParRend->m_nProg > 0 ?0:1;  updHue();  setCurL();  /*updTabVis();*/  }


///  Key pressed
//-----------------------------------------------------------------------------

void App::KeyPressed(unsigned char k, int x, int y)
{	switch (k)
	{	case '\033':  exit(0);  break;
		
		case 'p':	bPaused = !bPaused;	break;
		case VK_SPACE:	Simulate();		break;
		
		case VK_BACK:    prevClr();  /*prevMode();*/  break;
		case VK_RETURN:  nextClr();  /*nextMode();*/  break;
			
		case 'c':	mode = M_VIEW;	updTabVis();  break;
		case 'v':	mode = M_MOVE;	updTabVis();  break;
		case 'e':	if (mode == M_EMIT)  nextEmit();  else  { mode = M_EMIT;  updTabVis(); }  break;
		case 'E':	if (mode == M_EMIT)  prevEmit();  else  { mode = M_EMIT;  updTabVis(); }  break;
		case 'a':	if (mode == M_ACCEL)  nextAcc();  else  { mode = M_ACCEL; updTabVis(); }  break;
		case 'A':	if (mode == M_ACCEL)  prevAcc();  else  { mode = M_ACCEL; updTabVis(); }  break;
		case 'd':	mode = M_DYE;  updTabVis();  break;
		case 'D':	psys->scn.params.dyeType = psys->scn.params.dyeType == 0 ? 1:0;  ParamBase::Changed();  break;
		
		case ',':	if (mode==M_ACCEL)  prevAcc();  else  prevEmit();  break;  //-
		case '.':	if (mode==M_ACCEL)  nextAcc();  else  nextEmit();  break;
		
		case '`':	prevTab();  break;	case VK_TAB:	nextTab();  break;
		case '/':	prevClr();  break;	case '*':	nextClr();  break;
		case '=':
		case '+':	psys->scn.emit[psys->scn.ce].IncSize();  break;
		case '-':	psys->scn.emit[psys->scn.ce].DecSize();  break;
		
		case '[':	parLisE[psys->scn.ce]->GetParam("Velocity")->Decrement(1);  break;
		case ']':	parLisE[psys->scn.ce]->GetParam("Velocity")->Increment(1);  break;
	
		case 's':	bSliders = !bSliders;  break;	case 't':	bText = !bText;  break;
		case 'l':	togHue();  break;				case '0':	bXyz = !bXyz;    break;
		case 'r':	updVisParR();	break;
		
		case '3':  if (timAvgCnt > 0)   timAvgCnt--;  break;	case '1':	if (barsScale > 0.f)  barsScale -= 1.f;  break;
		case '4':  if (timAvgCnt < 20)  timAvgCnt++;  break;	case '2':	barsScale += 1.f;  break;
	}
}

void App::KeySpecial(int k, int x, int y)
{
	int mods = glutGetModifiers();
	shift = mods & GLUT_ACTIVE_SHIFT;
	ctrl = mods & GLUT_ACTIVE_CTRL;

	switch (k)
	{	case GLUT_KEY_F1:  bHelp = !bHelp;	break;	case GLUT_KEY_F2:  bSliders = !bSliders; break;
		case GLUT_KEY_F3:  bInfo = !bInfo;	break;
		case GLUT_KEY_F4:	if (shift)
			iTimes = (iTimes -1+NumTimM)%NumTimM;  else  iTimes = (iTimes +1)%NumTimM;  break;
		
		case GLUT_KEY_PAGE_UP:	  psys->PrevScene(ctrl);  fSimTime = 0.f;  break;
		case GLUT_KEY_PAGE_DOWN:  psys->NextScene(ctrl);  fSimTime = 0.f;  break;

		case GLUT_KEY_F5:	psys->Reset(shift ? 1 : psys->scn.initType);  fSimTime = 0.f;  break;	// type

		case GLUT_KEY_F6:	fSimTime = 0.f;  break;
		case GLUT_KEY_F7:	updVisParR();	break;

		case GLUT_KEY_F8:	psys->Drop(shift ? 0 : 1);  fSimTime = 0.f;  break;
		
		case GLUT_KEY_F9:	togHue();	break;
		//case GLUT_KEY_F10:	break;
		
		case GLUT_KEY_F11:	psys->LoadScenes();  App::updHue();  break;
	}
	if (bSliders)	curParList->Special(k, x, y);
}


// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / 
//  Camera

void App::UpdateCamera()
{
	//  update camera params  --
	if (foFov != fFov || foNear != fNear)
	{	glMatrixMode(GL_PROJECTION);	glLoadIdentity();
		gluPerspective(fFov, fAspect, fNear, 100.0);
		pParRend->setFOV(fFov);
		foFov = fFov;  foNear = fNear;
	}
	if (fBackgr != foBackgr)  {  glClearColor(0.2*fBackgr, 0.3*fBackgr, 0.4*fBackgr, 1.0);  foBackgr = fBackgr;  }

	//  update view
	glMatrixMode(GL_MODELVIEW);  glLoadIdentity();
	Scene& sc = psys->scn;  Emitter& em = sc.emit[sc.ce];
	for (int iter=0; iter<3; ++iter)
	{
		camPosLag += (sc.camPos - camPosLag) * inertia;
		camRotLag += (sc.camRot - camRotLag) * inertia;
		em.posLag += (em.pos - em.posLag) * inertia;
		em.rotLag += (em.rot - em.rotLag) * inertia;
	}
	glTranslatef(camPosLag.x, camPosLag.y, camPosLag.z);
	glRotatef(camRotLag.x, 1,0,0);  glRotatef(camRotLag.y, 0,1,0);
	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
}


void App::Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN)		buttons |= 1<<button;
	else if (state == GLUT_UP)	buttons = 0;
	
	///  change tab  ****
	if (buttons == 1 && mselTab >= 0 && tabHidden[mselTab]==0)  {	curTab = mselTab;  setCurL();  }

	mx = x;  my = y;
	if (bSliders)	curParList->Mouse(x, y, button, state);
}


// transform vector3 by transpose of matrix
void App::mulTr(float *v, float *r, GLfloat *m)
{
	r[0] = v[0]*m[0] + v[1]*m[1] + v[2]*m[2];
	r[1] = v[0]*m[4] + v[1]*m[5] + v[2]*m[6];
	r[2] = v[0]*m[8] + v[1]*m[9] + v[2]*m[10];
}
