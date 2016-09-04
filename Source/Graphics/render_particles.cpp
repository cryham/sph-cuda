#include "header.h"

#include "render_particles.h"



ParticleRenderer::ParticleRenderer() :
	m_pos(0), m_numParticles(0), m_ParRadius(0.04f), m_ParScale(1.f),
	m_fDiffuse(0.3f), m_fAmbient(0.7f), m_fPower(1.f), m_fSteps(0), m_fHueDiff(0.f),
	m_vbo(0), m_colorVbo(0)
{	m_nProg = 0/*0*/;  m_program[0] = 0;  _initGL();	}

ParticleRenderer::~ParticleRenderer()	{	m_pos = 0;	}



void ParticleRenderer::_drawPoints()
{
	if (!m_vbo)
	{	glBegin(GL_POINTS);  int a = 0;
		for (int i = 0; i < m_numParticles; ++i, a+=4)	glVertex3fv(&m_pos[a]);
		glEnd();	}
	else
	{	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_vbo);
		glVertexPointer(4, GL_FLOAT, 0, 0);
		glEnableClientState(GL_VERTEX_ARRAY);				

		if (m_colorVbo)
		{	glBindBufferARB(GL_ARRAY_BUFFER_ARB, m_colorVbo);
			glColorPointer(4, GL_FLOAT, 0, 0);
			glEnableClientState(GL_COLOR_ARRAY);	}

		glDrawArrays(GL_POINTS, 0, m_numParticles);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB, 0);
		glDisableClientState(GL_VERTEX_ARRAY); 
		glDisableClientState(GL_COLOR_ARRAY); 
	}
}


void ParticleRenderer::display()
{
	glEnable(GL_POINT_SPRITE_ARB);
	glTexEnvi(GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE);
	glEnable(GL_VERTEX_PROGRAM_POINT_SIZE_NV);
	glDepthMask(GL_TRUE);	glEnable(GL_DEPTH_TEST);

	int i = m_nProg;
	glUseProgram(m_program[i]);	//  pass vars
	glUniform1f( m_uLocPScale[i],  m_ParScale );
	glUniform1f( m_uLocPRadius[i], m_ParRadius );
	if (i==0)	{
		glUniform1f( m_uLocDiffuse, m_fDiffuse );
		glUniform1f( m_uLocAmbient, m_fAmbient );
		glUniform1f( m_uLocPower,   m_fPower );  }
	else  {
		glUniform1f( m_uLocHueDiff, m_fHueDiff );
		glUniform1f( m_uLocSteps, m_fSteps );
		/*glUniform1f( m_uLocStepsS, m_fSteps );*/  }
	
	glColor3f(1, 1, 1);
	_drawPoints();

	glUseProgram(0);
	glDisable(GL_POINT_SPRITE_ARB);
}


GLuint ParticleRenderer::_compileProgram(const char *vsource, const char *fsource)
{
	GLuint vertexShader;
	if (vsource)	{
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vsource, 0);
	glCompileShader(vertexShader);	}

	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fsource, 0);
	glCompileShader(fragmentShader);

	GLuint program = glCreateProgram();

	if (vsource)	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	glLinkProgram(program);

	// check if program linked
	GLint success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success) {		char temp[256];
		glGetProgramInfoLog(program, 256, 0, temp);
		printf("Failed to link program:\n%s\n", temp);
		glDeleteProgram(program);	program = 0;  }
	return program;
}


void ParticleRenderer::_initGL()
{
	m_scaleProg = _compileProgram(NULL, scalePixelShader);
	
	for (int i=0; i<NumProg; i++)	{
		m_program[i] = _compileProgram(vertexShader, spherePixelShader[i]);

		//  vars loc
		m_uLocPScale[i]  = glGetUniformLocation(m_program[i], "pointScale");
		m_uLocPRadius[i] = glGetUniformLocation(m_program[i], "pointRadius");	}
	m_uLocHueDiff = glGetUniformLocation(m_program[1], "fHueDiff");
	m_uLocDiffuse = glGetUniformLocation(m_program[0], "fDiffuse");
	m_uLocAmbient = glGetUniformLocation(m_program[0], "fAmbient");
	m_uLocPower   = glGetUniformLocation(m_program[0], "fPower");
	
	m_uLocSteps	= glGetUniformLocation(m_program[1], "fSteps");
	m_uLocStepsS= glGetUniformLocation(m_scaleProg, "fSteps");

	glClampColorARB(GL_CLAMP_VERTEX_COLOR_ARB, GL_FALSE);
	glClampColorARB(GL_CLAMP_FRAGMENT_COLOR_ARB, GL_FALSE);
}
