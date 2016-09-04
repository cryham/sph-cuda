#include "header.h"
#include "App.h"
#include "..\SPH\SPH.h"



///  Simulate
void App::Simulate()
{
	UpdateEmitter();
	psys->Update();
	pParRend->setVertexBuffer(psys->getPosBuffer(), psys->scn.params.numParticles);
	fSimTime += psys->scn.params.timeStep;
}


///  Render  ---------------------------------------------------------------------------------------------
void App::Render()
{
	if (!bPaused)  Simulate();

	CUT_SAFE_CALL(cutStartTimer(timer[0]));
	UpdateCamera();
  ///  Render
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);  

	DrawBounds();
	DrawCollider();

	pParRend->display();  // particles

	RenderText();
	glutSwapBuffers();
	
	//glutReportErrors();
	CUT_SAFE_CALL(cutStopTimer(timer[0]));
}



// / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / / 
void App::drawWireBox(float minX, float minY, float minY2, float minZ, float maxX, float maxY, float maxZ)
{	glBegin(GL_LINES);
	V(minX, minY, minZ);	V(maxX, minY, minZ);	V(minX, maxY, minZ);	V(maxX, maxY, minZ);
	V(minX, minY, minZ);	V(minX, maxY, minZ);	V(maxX, minY, minZ);	V(maxX, maxY, minZ);
	V(minX, minY2,maxZ);	V(maxX, minY2,maxZ);	V(minX, maxY, maxZ);	V(maxX, maxY, maxZ);
	V(minX, minY2,maxZ);	V(minX, maxY, maxZ);	V(maxX, minY2,maxZ);	V(maxX, maxY, maxZ);
	V(minX, minY, minZ);	V(minX, minY2,maxZ);	V(minX, maxY, minZ);	V(minX, maxY, maxZ);
	V(maxX, minY, minZ);	V(maxX, minY2,maxZ);	V(maxX, maxY, minZ);	V(maxX, maxY, maxZ);	glEnd();	}

void App::drawSphere(float r)  {
	if (aq < PI) {  drawCircleX(r,0);  drawCircleY(r,0);  drawCircleZ(r,0);  } else
	if (aq==PI)  {	glBegin(GL_LINES);  V(-r,0,0); V(r,0,0);  V(0,-r,0); V(0,r,0);  V(0,0,-r); V(0,0,r);  glEnd();  /*+*/  }
}
void App::drawCircleY(float r, float y){	glBegin(GL_LINE_LOOP);	for (float a=0.f; a<PI2; a+=aq)  V(cosf(a)*r, y, sinf(a)*r);	glEnd();	}
void App::drawCircleZ(float r, float z){	glBegin(GL_LINE_LOOP);	for (float a=0.f; a<PI2; a+=aq)  V(cosf(a)*r, sinf(a)*r, z);	glEnd();	}
void App::drawCircleX(float r, float x){	glBegin(GL_LINE_LOOP);	for (float a=0.f; a<PI2; a+=aq)  V(x, cosf(a)*r, sinf(a)*r);	glEnd();	}

void App::drawCylY(float r, float minY, float maxY)
{
	drawCircleY(r, minY);	drawCircleY(r, maxY);	glBegin(GL_LINES);
	V( r, minY, 0);   V( r, maxY, 0);
	V(-r, minY, 0);   V(-r, maxY, 0);   glEnd();
}
void App::drawCylZ(float r, float minZ, float maxZ)
{
	drawCircleZ(r, minZ);	drawCircleZ(r, maxZ);	glBegin(GL_LINES);
	V(0,  r, minZ);   V(0,  r, maxZ);
	V(0, -r, minZ);   V(0, -r, maxZ);   glEnd();
}