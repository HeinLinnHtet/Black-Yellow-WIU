#include "SceneManager.h"

SceneManager& SceneManager::GetInstance()
{
	static SceneManager instance;
	return instance;
}

SceneManager::~SceneManager()
{
	for (Scene* pScene : scene)
		delete pScene;
}

void SceneManager::PushState(Scene* pScene)
{
	size_t size = scene.size();
	if (size > 0)
		scene[size - 1]->pause();

	pScene->Init();
	scene.push_back(pScene);
}

void SceneManager::ChangeState(Scene* pScene)
{
	size_t size = scene.size();
	if (size > 0)
	{
		scene[size - 1]->Exit();
		delete scene[size - 1];
		scene.pop_back();
	}

	pScene->Init();
	scene.push_back(pScene);
}

void SceneManager::PopState()
{
	if (scene.size() > 0)
	{
		scene[scene.size() - 1]->Exit();
		scene.pop_back();
	}

	if (scene.size() > 0)
	{
		scene[scene.size() - 1]->resume();
	}
}

void SceneManager::Update(float dt)
{
	size_t size = scene.size();
	if (size == 0)
		return; //no scenes to update

	scene[size - 1]->Update(dt);
}

void SceneManager::Render()
{
	for (Scene* pScene : scene)
		pScene->Render();
}
