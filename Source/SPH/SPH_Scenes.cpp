#include "header.h"

#include "..\SPH\SPH.h"
#include "..\CUDA\System.cuh"
#include "..\App\App.h"



void cSPH::InitScene()
{
	_FreeMem();  _InitMem();
	
	Reset(scn.initType);

	//  app upd   preserve..
	App::camPosLag = scn.camPos;	App::camRotLag = scn.camRot;
	for(int i=0; i < NumEmit; i++)
	{	scn.emit[i].posLag = scn.emit[i].pos;
		scn.emit[i].rotLag = scn.emit[i].rot;	}
	for(int i=0; i < NumAcc; i++)
		scn.accPos[i] = scn.params.acc[i].pos;

	App::colliderPos = scn.params.collPos;
	scn.params.dyePos = App::dyePos; //+
	//App::dyePos = scn.params.dyePos;
	App::emitId = 0;	//ParamBase::Changed();
}


void cSPH::UpdScene()
{
	scn = scenes[curScene];  InitScene();	App::updHue();
}


void cSPH::NextScene(bool chapter)
{
	do {  curScene++;  if (curScene >= scenes.size())  curScene = 0;	}
	while (chapter && !scenes[curScene].bChapter);
	UpdScene();
}
void cSPH::PrevScene(bool chapter)
{
	do {  curScene--;  if (curScene < 0)  curScene = scenes.size()-1;	}
	while (chapter && !scenes[curScene].bChapter);
	UpdScene();
}


//  Load xml
//------------------------------------------------------------------------

void cSPH::LoadScenes()
{
	scenes.clear();
	TiXmlDocument file;  file.LoadFile("Scenes.xml");
	TiXmlElement* root = file.RootElement(), *s = NULL, *opt = NULL;
	if (!root) {	printf("\nError!  Can't load Scenes.xml\n");	}
	else	{
		s = root->FirstChildElement("Scene");		if (!s)  printf("Warning:  No <Scene> in xml.\n");
		opt = root->FirstChildElement("Options");	if (!opt)  printf("Warning:  No <Options> in xml.\n");
	}

	///  Scenes  ---------------------------------
	int i = -1, ch = 0;  curScene = 0;
	while (s)
	{
		if (s->Attribute("default") || s->Attribute("def"))  curScene = i+1;
		
		Scene sc(s);	scenes.push_back(sc);
		if (sc.bChapter)  ch++;
		s = s->NextSiblingElement("Scene");  i++;
	}

	if (i == -1)	// empty
	{	Scene sc;  scenes.push_back(sc);  scn = sc;  }
	else	scn = scenes[curScene];
	
	scenes[0].bChapter = true;	//while!
	
	if (root)
		printf("Loaded %d scenes in %d chapters from xml.\n", i+1, ch);
	InitScene();
}


///  load program settings

void cSPH::LoadOptions()
{
	TiXmlDocument file;  file.LoadFile("Scenes.xml");
	TiXmlElement* root = file.RootElement(), *s = NULL, *opt = NULL;
	if (!root) {	/*printf("\nError!  Can't load Scenes.xml\n");*/	}
	else	{
		opt = root->FirstChildElement("Options");	if (!opt)  printf("Warning:  No <Options> in xml.\n");
	}
	
	///  App options  ----------------------------
	const char* a = NULL;
	if (opt)  {
		a = opt->Attribute("Windowed");  if (a)  App::bWindowed = toInt(a) > 0;
		a = opt->Attribute("WSizeX");	 if (a)  App::WSizeX = toInt(a);
		a = opt->Attribute("WSizeY");	 if (a)  App::WSizeY = toInt(a);
		a = opt->Attribute("VSyncOff");  if (a)  App::bVsyncOff = toInt(a) > 0;

		a = opt->Attribute("timAvgCnt");  if (a)  App::timAvgCnt = toInt(a);
		a = opt->Attribute("barsScale");  if (a)  App::barsScale = toInt(a);
		
		a = opt->Attribute("showInfo");  if (a)  App::bShowInfo = toInt(a) > 0;  }
	//...
}