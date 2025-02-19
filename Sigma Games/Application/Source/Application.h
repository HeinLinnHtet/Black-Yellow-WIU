
#ifndef APPLICATION_H
#define APPLICATION_H

#include "timer.h"

class Application
{
public:

	enum SCENE_NUM
	{
		SCENE_MENU = 0,
		SCENE_BAB,
		SCENE_RINGTOSS,
		SCENE_PENALTY,
		SCENE_DART,
		TOTAL_SCENE
	};

	Application();
	~Application();
	void Init();
	void Run();
	void Exit();
	static bool IsKeyPressed(unsigned short key);

private:

	//Declare a window object
	StopWatch m_timer;
	bool enablePointer = true;
	bool showPointer = true;

	SCENE_NUM sceneNum;
	bool isEnterUp = false;
};

#endif