#include "header.h"
#include "App.h"



///  Help  ----------------------------------------------------------------------------
void App::RenderHelp()
{
	int sx = glutGet(GLUT_WINDOW_WIDTH), sy = glutGet(GLUT_WINDOW_HEIGHT), Fy = 19;
	char s[200];
	float w = 0.2;
	if (float(sx)/float(sy) < 1.5f)  w = 0.1f;
	
	//  background
 	glEnable(GL_BLEND);  glColor4f(0.1, 0.1, 0.2, 0.7);
 	glRectf(sx*w, sy*0.1, sx*(1-w), sy*0.9);	glDisable(GL_BLEND);
	
	//  Title
	int x = sx*0.5-150,  y = sy*0.13;
	glColor3f(0.8, 0.9, 1.0);	void* fnt = GLUT_BITMAP_HELVETICA_18;
	glPrint(x,y, "CUDA Smoothed Particle Hydrodynamics", fnt);
	
	#define  line(w,h)  glBegin(GL_LINES);	V(x,y+h,0);  V(x+w,y+h,0);  glEnd();
	#define  sub(s) \
		glColor3f(0.6, 0.7, 1.0);	glPrint(x,y, "    "s, fnt);  line(250,3);\
		y+=Fy*2;	glColor3f(0.8, 0.9, 1.0);
	
	#define  wr(s, yy)	glPrint(x,y, s, fnt);  y+=Fy*yy;
	
	///---------
	x = sx*w+50;  y = sy*0.2;  fnt = GLUT_BITMAP_9_BY_15;
	sub("show/hide");
	wr( "F1 - this Help screen", 2);
	
	wr( "F2, S - Parameter Sliders", 1);	line(250,-Fy+3);
	wr( "  Tab/Tilde - next/previous page", 2);
	wr( "  Up/Down - prev/next", 1);
	wr( "  Left/Right - dec/inc", 1);
	wr( "    with Shift *0.1,  with Ctrl *4", 1);
	wr( "  Home - reset", 2);
	
	wr( "F3 - Scene Info", 1);	// F6 ..
	wr( "F4 - Times [ms] (next, Shift-F4 prev)", 2);	// F7

	wr( "BackSpace,Enter or /,* - prev/next Color Type", 1);
	wr( "F9, L - Scale & Hue coloring", 2);
	
	wr( "F11 - Reload Scenes.xml", 2);

	///---------
	sub("simulation scene");
	wr( "PageDown, PageUp - next/previous", 1);
	wr( "  with Ctrl - next/prev chapter", 2);

	wr( "P - pause      Space - single step", 1);

	wr( "F5 - restart", 1);
	wr( "  Shift-F5 - init random", 1);
	wr( "F8 - drop sphere at random", 1);
	wr( "  Shift-F8 - at center", 2);
	
	///---------
	x = sx*0.52;  y = sy*0.2;
	sub("mouse control mode");
	wr( "C - camera,  V - collider,  D - dye", 1);
	wr( "e,E - emitters,  a,A - accelerators", 2);

	wr( "  Camera", 1);  line(100,-Fy+3);
	wr( "Left - Move,  Middle - Zoom,  Right - Rotate", 2);
	
	wr( "  Collider, Emitter, Accel, Dye", 1);  line(300,-Fy+3);
	wr( "Left - Move XZ,  Right - Move XY", 1);
	wr( "Middle - Rotate (Emitter)", 2);
	
	sub("additional");
	wr( "T - show current pos & rot", 1);
	wr( "0 - show xyz  length = 0,1 [m]", 1);
	wr( "F6 - zero time counter", 1);
	wr( "F7, R - reset particle vis R & near cut", 2);

	wr( "  dec/inc", 1);  line(100,-Fy+3);
	wr( "-,+ - emitter size ", 1);
	wr( "[,] - emitter velocity", 1);
	wr( "<,> - prev/next emitter/accel", 2);

	wr( "1,2 - time bars scale", 1);
	wr( "3,4 - times average interval", 2);
}
