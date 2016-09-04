#include "header.h"
#include "App.h"
#include "..\SPH\SPH.h"



///  Collider  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

void App::DrawCollider()
{
	SimParams& par = psys->scn.params;

	///  Draw Rotor
	if (par.rotType > 0)
	{
		int sx = par.rotSize.x, sy = par.rotSize.y, sz = par.rotSize.z, cb = par.rotBlades;
		float r = par.rotR, sp = par.rotSpc, ca = PI2/cb, x2 = sx*0.5f, y2 = sy*0.5f;
		
		int nRots = par.r2Dist > 0.f ? 2 : 1;
		float4 p0 = par.collPos;
		if (nRots > 1)	p0.x += par.r2Dist*0.5f;

		aq = PI2 / float(rendAq);  // low quality
		for (int i=0; i<nRots; i++)
		{
		float aa = (i == 0) ? par.rAngle : par.r2Angle;
		float tw = par.rTwist;  if (i==1)  tw *= par.r2twist;
		switch (par.rotType)
		{
		#define	drawRot\
				float clr = float(y+1)/float(sy+1) * 0.5f, scl = mode == M_MOVE ? 0.8f : 0.5f;	glColor3f(clr/4, scl+clr/3, clr);\
				glPushMatrix();  glTranslatef(p0.x + pc.x, p0.y + pc.y, p0.z + pc.z);	drawSphere(r);	glPopMatrix();
		case 1:		// pump rotor Z -
			for(int z= 1; z<=sz; z++)
			for(int x= 0; x<=sx; x++)	for(int c= 0; c<cb; c++)
			{
				float a = -aa + c*ca+ z*par.rTwist, cs = cosf(a)*sp, sn = -sinf(a)*sp;
				for(int y= 0; y<=sy; y++)
				{
					float3 pc = make_float3((x-x2)*cs - z*sn, (x-x2)*sn + z*cs, sp*y);	drawRot
				}
			}	break;

		case 2:		//  pump rotor Y |
			for(int z= 1; z<=sz; z++)
			for(int x= 0; x<=sx; x++)	for(int c= 0; c<cb; c++)
			{
				float a = -aa + c*ca+ z*par.rTwist, cs = cosf(a)*sp, sn = -sinf(a)*sp;
				for(int y= 0; y<=sy; y++)
				{
					float3 pc = make_float3((x-x2)*cs - z*sn, sp*y, (x-x2)*sn + z*cs);	drawRot
				}
			}	break;

		case 3:		// propeller Z -
			for(int c= 0; c<cb; c++)	{	float a = aa + c*ca;	// h = d-  x = r|
			for(int h= 0; h<=sy; h++)
			{	float dh = 0;  if (h==sy-1) dh=0.5f; else if (h==sy)  dh=1.2f;
				for(int x= 0; x<=sz; x++)
				{		float d = dh;  if (x==sz && d==0) d=0.4f;
						float k = cosf( (x-x2)*0.2f + PI*0.6f);  //d modif -^.
						float at = tw*(d*-0.05f -(h-y2)*k);					
					float ac = a + at, cs = cosf(ac)*sp, sn = -sinf(ac)*sp;
					float3 pc = make_float3(x*cs, x*sn, sp*(-d+h-y2) *k );
					r = par.rotR*abs(1-d);  int y=x;  drawRot
				}
			}	}  break;
		}
		p0.x -= par.r2Dist;
		}
		aq = PI2 / 40.f;
	}else	
	{	//  collider
		if (mode == M_MOVE)  glColor3f(0.3, 0.7, 1.0); else  glColor3f(0.0, 0.5, 1.0);
		glPushMatrix();  float4 pos = psys->scn.params.collPos;
		glTranslatef(pos.x, pos.y, pos.z);
		glutWireSphere(psys->scn.params.collR, 20, 10);  glPopMatrix();
	}

	///  Draw Emitters
	for (int i=0; i < NumEmit; i++)
	{
		Emitter& em = psys->scn.emit[i];
		bool sel = mode == M_EMIT && i == psys->scn.ce;
		if (em.size > 0 || sel)
		{
			if (sel)  glColor3f(1.0, 1.0, 0.5); else  glColor3f(0.7, 0.7, 0.0);
			glPushMatrix();		glTranslatef(em.posLag.x,em.posLag.y,em.posLag.z);
			glRotatef(em.rotLag.y, 0,1,0);  glRotatef(-em.rotLag.x, 1,0,0);
			float spc = psys->scn.spacing;	int er = max(1,em.size), er2 = (em.size2 == 0) ? er : em.size2;
			glScalef(spc * er, spc * er2, spc);
			glutWireCube(1.f);	glPopMatrix();
		}
	}

	///  Draw Accels
	SimParams& p = psys->scn.params;
	for (int i=0; i < NumAcc; i++)
	{
		Accel& ac = p.acc[i];
		bool sel = mode == M_ACCEL && i == psys->scn.ca;
		if (sel)  glColor3f(1.0, 0.3, 0.6); else  glColor3f(0.7, 0.1, 0.2);
		switch (ac.type)
		{
		case ACC_Off:  if (!sel)  break;
		case ACC_Box:
			glPushMatrix();		glTranslatef(ac.pos.x, ac.pos.y, ac.pos.z);
			glScalef(ac.size.x*2, ac.size.y*2, ac.size.z*2);
			glutWireCube(1.f);	glPopMatrix();
			break;
	
		case ACC_CylY:
			glPushMatrix();		glTranslatef(ac.pos.x, ac.pos.y, ac.pos.z);
			glScalef(ac.size.x, 1.f, ac.size.z);
			drawCylY(1.f, -ac.size.y,ac.size.y);	glPopMatrix();
			break;
			
		case ACC_CylYsm:
			glPushMatrix();		glTranslatef(ac.pos.x, ac.pos.y, ac.pos.z);
			drawCylY(ac.size.x, -ac.size.y,ac.size.y);
			drawCylY(ac.size.z, -ac.size.y,ac.size.y);	glPopMatrix();
			break;
	
		}
	}
	
	///  Draw Dye
	bool sel = mode == M_DYE;
	if (sel || p.dyeType > 0)
	{
		if (sel)  glColor3f(0.9, 0.9, 0.9);  else  glColor3f(0.4, 0.4, 0.4);
		glPushMatrix();		glTranslatef(p.dyePos.x, p.dyePos.y, p.dyePos.z);
		switch (p.dyeType)	{
			case 1:  glScalef(p.dyeSize.x*2, p.dyeSize.y*2, p.dyeSize.z*2);  glutWireCube(1.f);  break;
			case 2:  drawSphere(p.dyeSize.y);  break;	}
		glPopMatrix();
	}
}


//  draw Bounds  ______________________________________________________________________________________________

void App::DrawBounds()
{
	SimParams* p = &psys->scn.params;
	float minX = p->worldMinD.x, maxX = p->worldMaxD.x,
		  minY = p->worldMinD.y, maxY = p->worldMaxD.y,
		  minZ = p->worldMinD.z, maxZ = p->worldMaxD.z;
	
	//  xyz
	if (bXyz)
	{	glBegin(GL_LINES);	float m=-0.001f, s = 0.1f+m;
		glColor3f(1,1,1);	V(minX+m, minY+m, minZ+m);	glColor3f(1,0,0);	V(minX+s, minY+m, minZ+m);
		glColor3f(1,1,1);	V(minX+m, minY+m, minZ+m);	glColor3f(0,1,0);	V(minX+m, minY+s, minZ+m);
		glColor3f(1,1,1);	V(minX+m, minY+m, minZ+m);	glColor3f(0,0,1);	V(minX+m, minY+m, minZ+s);	glEnd();	}
	
	if (mode == M_VIEW)  glColor3f(0.3, 0.7, 1.0);  else  glColor3f(0.0, 0.5, 1.0);
	switch (p->bndType)
	{	default:
		case BND_BOX:
		{	float minY2 = minY;  bool bWave = p->bndEffZ == BND_EFF_WAVE;
			if (bWave)  minY2 += /*p->distBndSoft*/ - (maxZ - minZ)*-p->r2Angle /*+ p->particleR/2.f*/;
			drawWireBox(minX, minY, minY2, minZ, maxX, maxY, maxZ);
			if (bWave)
			{	float scurz = minZ + p->rTwist*(1.f + sinf(p->rAngle)), smaxz = minZ + p->rTwist*2.f;
				glBegin(GL_LINES);	V(minX,maxY,smaxz);  V(maxX,maxY,smaxz);  glEnd();  // max sin
				glColor3f(0.3,0.9,0.9);
				glBegin(GL_LINE_LOOP);	// rect cur sin
				V(minX,minY,scurz);  V(minX,maxY,scurz);
				V(maxX,maxY,scurz);  V(maxX,minY,scurz);  glEnd();
			}	}  break;
		
		case BND_CYL_Y:		drawCylY(maxX, minY, maxY);  break;
		case BND_CYL_Z:		drawCylZ(minY, minZ, maxZ);  break;
		
		case BND_CYL_YZ:{	float r = maxX;
			drawCircleY(r, 0);	drawCircleY(r, 0);	glBegin(GL_LINES);
			V( 0, minY, r);  V( 0, maxY, r);
			V( 0, minY,-r);  V( 0, maxY,-r);	glEnd();
			r = minY;
			drawCircleZ(r, 0);	drawCircleZ(r, 0);	glBegin(GL_LINES);
			V(0,  r, minZ);  V(0,  r, maxZ);
			V(0, -r, minZ);  V(0, -r, maxZ);	glEnd();	}	break;

		case BND_SPHERE:	drawSphere(minY);	break;

		//-------------------------------------------------
		case BND_PUMP_Y:
		{	float b2 = p->distBndSoft, b = b2/2, r = p->particleR,
				hc = p->hClose -b2+r,	rad = maxX, ang = p->angOut;
			float xs = sinf(ang * p->s3)*rad -b-r, zs = cosf(ang * p->s3)*rad * p->s4 +b+2*r;
			float xS = 0.09f*p->s4 +b2-r;

			// box out
			if (ang < 0.5f)		// ^
				drawWireBox(-xs, zs,zs, minZ, xs, maxY, hc);
			else  {		// S
				glBegin(GL_LINES);	V(xS, zs, minZ);  V( xS, zs, hc);
				V(xS, maxY, minZ);  V(maxX,maxY, minZ);		V( xS, maxY, hc);	V(maxX, maxY, hc);
				V(xS, maxY, minZ);  V( xS, maxY, hc);		V(maxX, maxY, minZ);	V(maxX, maxY, hc);
				V(xS, zs  , minZ);  V( xS, maxY, minZ);		V( xS, zs, hc);		V( xS, maxY, hc);
				V(maxX, 0, minZ);	V(maxX, maxY, minZ);	V(maxX, 0, hc);		V(maxX, maxY, hc);	glEnd();	}

			drawCylZ(maxX, minZ, hc);	// cyl frame
			drawCylZ(maxX*p->radIn-b, maxZ, hc);	// cyl in
		}	break;
	}
}
