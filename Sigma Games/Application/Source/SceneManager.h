#pragma once

#include <vector>
#include "Scene.h"

class SceneManager
{
public:
	static SceneManager& GetInstance();

	void PushState(Scene* pScene);
	void ChangeState(Scene* pScene);
	void PopState();

	void Update(float dt);
	void Render();

private:
	std::vector<Scene*> scene;

	SceneManager() = default;
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	SceneManager& operator=(const SceneManager&) = delete;
};
