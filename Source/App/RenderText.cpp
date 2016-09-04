#include "header.h"
#include "App.h"



void App::RenderText()
{
	SimParams* p = &psys->scn.params;

	beginWinCoords();  void* fnt = GLUT_BITMAP_9_BY_15;
	int x = 0, y = 20-2, Fy = 20, yf2 = -Fy+13;
	int ScreenX = glutGet(GLUT_WINDOW_WIDTH), ScreenY = glutGet(GLUT_WINDOW_HEIGHT);
	
	
	///  Fps, Scene
	glColor3f(0.0, 1.0, 1.0);
	if (p->numParticles >= 1024)
		sprintf(s, "Fps %5.1f  particles %4.1fk", psys->tim.FR/**1000.f/tm_s/**/, p->numParticles/1024.f);
	else
		sprintf(s, "Fps %5.1f  particles %4d", psys->tim.FR, p->numParticles);
	glPrint(0,y, s, fnt);	//y += Fy;

	glColor3f(0.5, 1.0, 0.5);
	sprintf(s, "Scene: %d/%d  %s", psys->curScene+1, psys->scenes.size(), psys->scn.title);
	glPrint(300,y, s, fnt);	y += Fy + 5;

	glColor3f(0.5, 0.8, 1.0);
	sprintf(s, "Colors: %s %s", ClrNames[p->clrType], pParRend->m_nProg > 0 ? "Hue":"");  glPrint(0,y, s, fnt);	y += Fy + 10;
	if (nChg > 0)  nChg--;	/*sprintf(s, "%2d  si %3d", nChg, sizeof(SimParams));  glPrint(0,y, s, fnt);	y += Fy + 10;/**/

	
	///  Times, grid
	float sdt = psys->scn.params.timeStep;	if (sdt < 0.00001f)  sdt = 0.00001f;
	glColor3f(0.8, 0.96, 1.0);
	{	x = 200;	const bool bars = true;
		float sc = barsScale;
		#define bar(str,tm)  sprintf(s, str, tm);  glPrint(0,y, s,fnt);  if (bars) glRectf(x,y, x + (tm)/*powf(tm,0.8f)*/*sc, y+yf2);  y+=Fy;
		switch (iTimes)  {
		case 0:  if (!bText)  break;	/*T inf*/
			sprintf(s, "Times  Scale: %5.1f  AvgCnt: %2d  ", barsScale, timAvgCnt);  glPrint(0,y, s,fnt);  y+=Fy;  break;
		case 1:	{
			sprintf(s, "Time:%7.3f s  real: %4.1f x", fSimTime, 0.001f*tdt/sdt);	glPrint(0,y, s,fnt);  y+=Fy;  }  break;
		case 2:	{	float tm_23 = tm[2]+tm[3], tm_54 = tm[5]-tm[4];  //  t1-4
			bar("t1: %5.2f  Integrate",	tm[1]);		bar("t2: %5.2f  Sort",	tm_23);
			bar("t3: %5.2f  Density",	tm[4]);		bar("t4: %5.2f  Force",	tm_54);  }  break;
		case 3: {	//  short
			bar("t1: %5.2f  Integrate", tm[1]);		bar("t4: %5.2f  SPH", tm[5]);
			bar("ts: %5.2f  Simulate", tm_si);		bar("ta: %5.2f  All",  tdt);  }  break;
		case 4: {  float tm_d = tdt -tm_si -tm[0];  //  diff
			bar("ts: %5.2f  Simulate", tm_si);		bar("ta: %5.2f  All", tdt);
			bar("tr: %5.2f  Draw",  tm[0]);			bar("td: %5.2f  diff", tm_d);  }  break;  }
		//sprintf(s, "onBnd %3d %2d"
		//	,psys->hCounters[0], psys->hCounters[1]);	glPrint(0,y, s, fnt);	y += Fy*2;/**/
	}
	// reset timers
	if (!bPaused)
	{
		tdts += psys->tim.dt*1000.f;
		if (++timCnt > timAvgCnt)
		{	timCnt = 0;		tdt = tdts/float(timAvgCnt+1);  tdts = 0.f;
			for (int i=0; i < NumTim; i++)
			{	tm[i] = cutGetAverageTimerValue(timer[i]);
				CUT_SAFE_CALL(cutResetTimer(timer[i]));		}
			tm_si = tm[1]+tm[2]+tm[3]+tm[5];
		}
	}else	{	tdt = psys->tim.dt*1000.f;  tdts = 0.f;  timCnt = 0;
		tm[0] = cutGetAverageTimerValue(timer[0]);
		CUT_SAFE_CALL(cutResetTimer(timer[0]));		}
	
	///  Scene info
	if (bInfo)
	{	x = ScreenX -260;  y = 20;
		#define wr(n)		 glPrint(x,y,s,fnt);  y += Fy*n;
		#define line(st,n)   glBegin(GL_LINES);  V(x,y+3,0);  V(x+250,y+3,0);  glEnd();  strcpy(s,"  "st);  wr(n);
		/**/line("Scene Info", 1);
		sprintf(s, "Particles    %d", p->numParticles);  wr(1);
		sprintf(s, "Max In Cell  %d", p->maxParInCell);  wr(2);
		
		sprintf(s, "Grid cells  %d", p->numCells);  wr(1);
		sprintf(s, "Grid size   %d %d %d", p->gridSize.x, p->gridSize.y, p->gridSize.z);  wr(1);
		sprintf(s, "Cell dim   %7.2f mm", 1000.f*p->cellSize.x);  wr(2);
		
		//sprintf(s, "WorldG %6.2f %6.2f %6.2f", p->gridSize.x*p->cellSize.x, p->gridSize.y*p->cellSize.y, p->gridSize.z*p->cellSize.z);  wr(1);
		sprintf(s, "World dim [m] x [m] x [m]");  wr(1);
		sprintf(s, "  %6.3f %6.3f %6.3f", p->worldSize.x, p->worldSize.y, p->worldSize.z);  wr(1);
		sprintf(s, "World - bounds dim [m]");  wr(1);
		sprintf(s, "  %6.3f %6.3f %6.3f", p->worldSizeD.x, p->worldSizeD.y, p->worldSizeD.z);  wr(2);

		/**/line("Boundaries", 1);
		sprintf(s, "Hard dist  %7.2f mm", 1000.f*p->distBndHard);  wr(1);
		sprintf(s, "Soft dist  %7.2f mm", 1000.f*p->distBndSoft);  wr(2);
		
		sprintf(s, "Damping  %6.0f",	p->bndDamp);   wr(1);
		sprintf(s, "Stiffness  %6.0f",	p->bndStiff);  wr(2);
		
		sprintf(s, "Type:  %s",   BndNames[p->bndType]);	 wr(1);
		sprintf(s, "Effect:  %s", BndEffNames[p->bndEffZ]);  wr(2);

		/**/line("SPH", 1);
		sprintf(s, "particle R   %7.2f mm", 1000.f*p->particleR);  wr(1);
		sprintf(s, "smoothing R  %7.2f mm", 1000.f*p->h);  wr(1);
		sprintf(s, "min dist     %7.2f mm", 1000.f*p->minDist);  wr(1);
		sprintf(s, "spacing      %7.2f mm", 1000.f*psys->scn.spacing);  wr(2);
		
		sprintf(s, "mass     %7.4f g",	1000.f*p->particleMass);  wr(1);
		sprintf(s, "density   %6.1f kg/m3",	p->restDensity);   wr(1);
		sprintf(s, "min dens  %6.1f kg/m3", 1.f/sqrt(p->minDens));  wr(2);
		
		sprintf(s, "Time   %12.4f s", fSimTime);  wr(1);
		sprintf(s, "RealTime x %6.2f", 0.001f*tdt/sdt);  wr(1);
	}
	
	
	///  Scale Line  ----
	void* fnt1 = GLUT_BITMAP_HELVETICA_12, *fntL = GLUT_BITMAP_HELVETICA_18;
	if (pParRend->m_nProg)
	{		//  Pos
		float x1 = ScreenX*0.35f, x2 = ScreenX-1, y1=ScreenY-1,y2=y1-15,y3=y2-5;  // bottom
		//float x1 = ScreenX*0.4f, x2 = ScreenX-10, y1=10,y2=y1+15,y3=y2+20;  // top

		glUseProgram(pParRend->m_scaleProg);
		glUniform1f( pParRend->m_uLocStepsS, pParRend->m_fSteps );

		glBegin(GL_QUADS);	// hue bar
		const float m = 0;//-0.1;
		glTexCoord2f(m,0);  glVertex2f(x1,y1);	glTexCoord2f(1,0);  glVertex2f(x2,y1);
		glTexCoord2f(1,1);  glVertex2f(x2,y2);	glTexCoord2f(m,1);  glVertex2f(x1,y2);
		glEnd();	glUseProgram(0);

		glBegin(GL_LINES);	glColor3f(1,1,1);	// lines |
		for (int i=0; i<=6; i++)  {
			float x = float(i)/6.f * (x2-x1) +x1;	glVertex2f(x,y2+4);	glVertex2f(x,y3-2);	}
		glEnd();

		for (int i=0; i<=6; i++)	// values
		{	float x = float(i)/6.f * (x2-x1) +x1;
			
			float c = float(i)/6.f;  // [0..1]
			float val = (c - p->brightness)/ p->contrast;

			switch (p->clrType)
			{	//  opposite to Kernel.cu  coloring
				case CLR_Vel:	val = val/2.5f;									sprintf(s, "%6.3f", val);	break;
				case CLR_Dens:	val = val/4.f *p->restDensity + p->restDensity;	sprintf(s, "%6.0f", val);	break;
				case CLR_Accel:	val = val/0.02f;								sprintf(s, "%6.2f", val);	break;

				// f = 0.00001f / par.particleMass * length(addVel) / par.timeStep;		// CLR_Force
				//case CLR_Accel:	val = val*(p->particleMass / 0.000005f) * p->timeStep;	sprintf(s, "%6.3f", val);	break;
				// f = 0.00001f * length(addVel) / (par.timeStep * par.particleMass);
				//case CLR_Accel:	val = val/0.000005f *p->particleMass * p->timeStep;		sprintf(s, "%6.3f", val);	break;
				default:  strcpy(s, "");	break;
			}
			glPrint(x -56, y3, s ,fntL);
		}
		// label
		switch (p->clrType)
		{
			case CLR_Vel:	sprintf(s, "Velocity [m/s]");	break;
			case CLR_Dens:	sprintf(s, "Density [kg/m3]");	break;
			case CLR_Accel:	sprintf(s, "Accel [m/s2]");	break;
			default:  strcpy(s, "");	break;
		}
		glPrint(ScreenX -136, y3-30, s ,fnt);
	}

	//*  alpha start
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	if (iTimes > 1)  // upd rect
	if (nChg > 0)	{	glColor4f(0.5, 0.8, 1.0, nChg*0.1f);	glRectf(0,yParList+42, 7,yParList+45);	}


	///  Sliders, Tabs  ****
	if (bSliders)
	{	y = yParList;	int xs;
		for (int i=0; i<NumTabs; i++)
		{	if (i==nTabRow2)  y += Fy+6;  // inc row
			x = tabPosX[i];  xs = tabWidths[i];  float3 c = tabClrs[i];
		if (tabHidden[i] == 0)	{
		//  back rect
			if (i == curTab)	glColor4f(c.x, c.y, c.z, 0.5);
			else if (i == mselTab)  glColor4f(c.x, c.y, c.z, 0.25);
			else  glColor4f(c.x, c.y, c.z, 0.1);
			glRectf(x, y-Fy+5, x+xs -8, y+5);
		//  text
			if (i == curTab || i == mselTab)  glColor4f(c.x+0.5, c.y+0.5, c.z+0.5, 1.0);
			else  glColor4f(c.x+0.5, c.y+0.5, c.z+0.5, 0.8);
			strcpy(s, tabNames[i]);  glPrint(x +4,y, s, fnt1);	}
		}
		y = yParList + Fy*3;
		if (tabHidden[curTab] == 0)
		if (curParList)  curParList->Render(0, y);
	}
	

	///  Camera, Mode
	if (bSliders) 
	{	int yy = ScreenY -7;
		if (bText)	{
			glColor3f(0.5, 1.0, 1.0);
			switch (mode)	{
				case M_VIEW: sprintf(s, "pos: %6.3f %6.3f %6.3f  rot: %5.1f %6.1f", camPosLag.x,camPosLag.y,camPosLag.z, camRotLag.x,camRotLag.y); break;
				case M_MOVE: sprintf(s, "pos: %6.3f %6.3f %6.3f", p->collPos.x, p->collPos.y, p->collPos.z); break;
				case M_EMIT: {	Emitter& e = psys->scn.emit[psys->scn.ce];
					sprintf(s, "pos: %6.3f %6.3f %6.3f  rot: %5.1f %6.1f", e.pos.x,e.pos.y,e.pos.z, e.rot.x,e.rot.y);  } break;
				case M_ACCEL:{	Accel& a = p->acc[psys->scn.ca];  sprintf(s, "pos: %6.3f %6.3f %6.3f", a.pos.x,a.pos.y,a.pos.z);  } break;
				case M_DYE:  {	float3& pos = psys->scn.params.dyePos;  sprintf(s, "pos: %6.3f %6.3f %6.3f", pos.x,pos.y,pos.z);  } break;
			}	glPrint(150, yy, s, fnt);	}
		//  Mode
		switch(mode)
		{	case M_VIEW: glColor4f(0.0, 0.2, 0.4, 0.3);  glRectf(0,yy-Fy+3,145,yy+6);	glColor3f(0.5, 0.8, 1.0);  strcpy(s, "Mode: Camera");  break;
			case M_MOVE: glColor4f(0.0, 0.3, 0.3, 0.3);  glRectf(0,yy-Fy+3,145,yy+6);	glColor3f(0.5, 1.0, 1.0);  strcpy(s, "Mode: Collider");  break;
			case M_EMIT: glColor4f(0.3, 0.3, 0.0, 0.3);  glRectf(0,yy-Fy+3,145,yy+6);	glColor3f(1.0, 1.0, 0.5);  sprintf(s, "Mode: Emitter %d", psys->scn.ce+1);  break;
			case M_ACCEL:glColor4f(0.3, 0.1, 0.1, 0.3);  glRectf(0,yy-Fy+3,145,yy+6);	glColor3f(1.0, 0.5, 0.8);  sprintf(s, "Mode: Accel %d", psys->scn.ca+1);  break;
			case M_DYE:  glColor4f(0.2, 0.2, 0.2, 0.3);  glRectf(0,yy-Fy+3,145,yy+6);	glColor3f(0.9, 0.9, 0.9);  sprintf(s, "Mode: Dye");  break;  }
		glPrint(0,yy, s, fnt);
	}


	//  Particles debug
	#if 0
	int a = 0;
	for (int i=0; i<50; i++, a+=4)
	{	sprintf(s, "p %+4.2f, %+4.2f, %+4.2f", psys->hPos[a], psys->hPos[a+1], psys->hPos[a+2], psys->hPos[a+3]);	glPrint(0,y, s, fnt);
		sprintf(s, "v %+4.2f, %+4.2f, %+4.2f", psys->hVel[a], psys->hVel[a+1], psys->hVel[a+2], psys->hVel[a+3]);	glPrint(250,y, s, fnt);  y += Fy;	}
	#endif

	if (bHelp)  RenderHelp();
	
	endWinCoords();
	glDisable(GL_BLEND);
}
