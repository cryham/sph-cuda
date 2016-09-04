#pragma once

#include "param.h"


void beginWinCoords(), endWinCoords();
void glPrint(int x, int y, const char *s, void *font);


class ParamListGL : public ParamList
{
public:
	ParamListGL(char *name = "");

	void Render(int x, int y, bool shadow = false);
	bool Mouse(int x, int y, int button=GLUT_LEFT_BUTTON, int state=GLUT_DOWN);
	bool Motion(int x, int y);
	void Special(int key, int x, int y);

private:
	int bar_x, bar_w, bar_h, bar_dx;  // pos, dim
	int text_x, text_y,  value_x, font_h;
	int start_x, start_y;	void *font;

	float text_clr_sel[4], text_clr_unsel[4];  // clr
	float bar_clr_outer[4], bar_clr_inner[4], bar_clr_insel[4];
	bool isLMBDown;
};
