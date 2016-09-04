#include "header.h"

#include "param.h"
#include "paramgl.h"


void beginWinCoords()
{
	glMatrixMode(GL_MODELVIEW);   glPushMatrix();  glLoadIdentity();
	glTranslatef(0.0, glutGet(GLUT_WINDOW_HEIGHT) - 1, 0.0);
	glScalef(1.0, -1.0, 1.0);

	glMatrixMode(GL_PROJECTION);  glPushMatrix();  glLoadIdentity();
	glOrtho(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT), -1, 1);
	glMatrixMode(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
}

void endWinCoords()
{
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);  glPopMatrix();
	glMatrixMode(GL_MODELVIEW);   glPopMatrix();
}


void glPrint(int x, int y, const char *s, void *font)
{
	size_t/*int*/ i, len = strlen(s);
	glRasterPos2f(x, y);
	for (i = 0; i < len; i++)
		glutBitmapCharacter(font, s[i]);
}

//  Init  ---------------------------------------------------------------------

ParamListGL::ParamListGL(char *name) : ParamList(name)
{
	font = (void*) GLUT_BITMAP_9_BY_15;  isLMBDown = false;

	text_x = 5;  text_y = 17;  font_h = 12;  bar_h = 14;
	value_x = 140;  bar_x = 220;  bar_w = 250;  bar_dx = 20; /*margin+*/
	start_x = 0;  start_y = 0;
	
	float clrs[5][4] = {  //rgba
		{0.85,0.92,1.0, 1.0},  // txt sel
		{0.6, 0.75,0.9, 0.8},  // txt

		{0.0, 0.2, 0.4, 0.4},  // bar frame
		{0.4, 0.75,0.9, 0.4},  // bar in
		{0.6, 0.8, 1.0, 0.7}}; // bar in sel
	
	memcpy((void*) &text_clr_sel[0], (void*) &clrs[0][0], sizeof(clrs));
}


//  Render  ---------------------------------------------------------------------

void ParamListGL::Render(int x, int y, bool shadow)
{
	//beginWinCoords();
	start_x = x;  start_y = y;

	for(ParamIter p = params.begin(); p != params.end(); ++p)
	if (*p)
	{	if ((*p)->IsList())
		{	ParamListGL *list = (ParamListGL*) (*p);
			list->Render(x+10, y);
			y += text_y * list->GetSize();
		} else
		{	if (p==cur)	glColor3fv(text_clr_sel);	// text
			else		glColor3fv(text_clr_unsel);
			
			glPrint(x + text_x, y + font_h, (*p)->GetName()->c_str(), font);
			glPrint(x + value_x, y + font_h, (*p)->GetValueString().c_str(), font);

			glColor4fv(bar_clr_outer);		//  frame
			glBegin(GL_LINE_LOOP);
			glVertex2f(x + bar_x, y);					glVertex2f(x + bar_x + bar_w, y);
			glVertex2f(x + bar_x + bar_w, y + bar_h);	glVertex2f(x + bar_x, y + bar_h);
			glEnd();

			if (p==cur)	glColor4fv(bar_clr_insel);	//  fill
			else 		glColor4fv(bar_clr_inner);
			glRectf(x + bar_x, y + bar_h+1, x + bar_x + (bar_w*(*p)->GetPercentage()), y);
			y += text_y;
		}
	} else  y += text_y;
	//endWinCoords();
}


//  Mouse  ---------------------------------------------------------------------

bool ParamListGL::Mouse(int x, int y, int button, int state)
{
	if (button==GLUT_LEFT_BUTTON && state==GLUT_UP)
		isLMBDown = false;

	if ((y < start_y) || (y > (int)(start_y + (text_y * params.size()) - 1)))
		return false;

	int i = (y - start_y) / text_y;

	if (button==GLUT_LEFT_BUTTON && state==GLUT_DOWN)
	if (x > bar_x -bar_dx && x < bar_x +bar_w+bar_dx)
	{
		isLMBDown = true;
		ParamIter  curTemp = cur;  // empty-back
		for (cur = params.begin(); cur != params.end() && i > 0; cur++, i--);	//cur = (ParamIter)&params.at(i);
		if (!*cur)  cur = curTemp;

		Motion(x, y);
	}else
		isLMBDown = false;
	
	return true;
}


bool ParamListGL::Motion(int x, int y)
{
	if (!*cur)	{	isLMBDown = false;  return false;  }

	if (!isLMBDown)  return false;
	//if (x < bar_x -bar_dx || x > bar_x +bar_w+bar_dx)		return isLMBDown;
	if (x < bar_x)  {			(*cur)->SetPercentage(0.0);	 ParamBase::Changed();  return isLMBDown;	}
	if (x > bar_x + bar_w)  {	(*cur)->SetPercentage(1.0);	 ParamBase::Changed();  return isLMBDown;	}

	(*cur)->SetPercentage((x-bar_x) / (float)bar_w);  ParamBase::Changed();
	if ((y < start_y) || (y > (int)(start_y + (text_y * params.size()) - 1)))
		return isLMBDown;
	return true;
}


//  Keyboard  ---------------------------------------------------------------------

void ParamListGL::Special(int key, int /*x*/, int /*y*/)
{
	float t = 1.f;	// modifiers
	if (isKeyDown(VK_CONTROL))	t *= 4.f;
	if (isKeyDown(VK_SHIFT))	t *= 0.1f;
	//if (isKeyDown(VK_MENU))	t *= 0.1f;
	
	switch (key)
	{
		case GLUT_KEY_DOWN:		Increment(1.f);	break;
		case GLUT_KEY_UP:		Decrement(1.f);	break;
		case GLUT_KEY_RIGHT:	GetCurrent()->Increment(t);	 ParamBase::Changed();  break;
		case GLUT_KEY_LEFT:		GetCurrent()->Decrement(t);	 ParamBase::Changed();  break;
		case GLUT_KEY_HOME:		GetCurrent()->Reset();		 ParamBase::Changed();  break;
		//case GLUT_KEY_END:	GetCurrent()->SetPercentage(1.0);	break;
	}
	//glutPostRedisplay();
}
