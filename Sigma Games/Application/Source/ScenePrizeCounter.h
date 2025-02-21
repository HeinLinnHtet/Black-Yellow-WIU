#ifndef SCENE_PRIZECOUNTER_H
#define SCENE_PRIZECOUNTER_H

#include "Scene.h"
#include "Mesh.h"
#include "AltAzCamera.h"
#include "MatrixStack.h"
#include "Light.h"
#include "FPCamera.h"

#include "CollisionDetection.h"
#include "PhysicsObject.h"
#include "ObjectPool.h"

#include "GOPrize.h"
#include "Player.h"
#include "GOPrizeStock.h"
#include "Windows.h"
#include <iostream>
#include "string.h"

class ScenePrizeCounter : public Scene
{
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_LEFT,
		GEO_RIGHT,
		GEO_TOP,
		GEO_BOTTOM,
		GEO_FRONT,
		GEO_BACK,
		GEO_SPHERE,
		GEO_CUBE,
		GEO_PLANE,
		GEO_SHELF,

		GEO_BURGER,
		GEO_CHIPS,
		GEO_MIKU,
		GEO_DALEK,
		GEO_FREDDY,
		GEO_BOBOMB,

		GEO_TEXT,
		NUM_GEOMETRY,
	};

	enum UNIFORM_TYPE
	{
		U_MVP = 0,
		U_MODELVIEW,
		U_MODELVIEW_INVERSE_TRANSPOSE,
		U_MATERIAL_AMBIENT,
		U_MATERIAL_DIFFUSE,
		U_MATERIAL_SPECULAR,
		U_MATERIAL_SHININESS,
		U_LIGHT0_TYPE,
		U_LIGHT0_POSITION,
		U_LIGHT0_COLOR,
		U_LIGHT0_POWER,
		U_LIGHT0_KC,
		U_LIGHT0_KL,
		U_LIGHT0_KQ,
		U_LIGHT0_SPOTDIRECTION,
		U_LIGHT0_COSCUTOFF,
		U_LIGHT0_COSINNER,
		U_LIGHT0_EXPONENT,
		U_LIGHT1_TYPE,
		U_LIGHT1_POSITION,
		U_LIGHT1_COLOR,
		U_LIGHT1_POWER,
		U_LIGHT1_KC,
		U_LIGHT1_KL,
		U_LIGHT1_KQ,
		U_LIGHT1_SPOTDIRECTION,
		U_LIGHT1_COSCUTOFF,
		U_LIGHT1_COSINNER,
		U_LIGHT1_EXPONENT,
		U_LIGHT2_TYPE,
		U_LIGHT2_POSITION,
		U_LIGHT2_COLOR,
		U_LIGHT2_POWER,
		U_LIGHT2_KC,
		U_LIGHT2_KL,
		U_LIGHT2_KQ,
		U_LIGHT2_SPOTDIRECTION,
		U_LIGHT2_COSCUTOFF,
		U_LIGHT2_COSINNER,
		U_LIGHT2_EXPONENT,
		U_NUMLIGHTS,
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_LIGHTENABLED,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		U_TOTAL,

	};

	ScenePrizeCounter();
	~ScenePrizeCounter();

	glm::vec3 playerposition;
	
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

private:
	void HandleKeyPress();
	void RenderMesh(Mesh* mesh, bool enableLight);
	void RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey);
	void RenderText(Mesh* mesh, std::string text, glm::vec3 color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, glm::vec3 color, float size, float x, float y);
	void RenderSkybox();
	void HandleMouseInput();
	bool isLooking(const FPCamera& camera, const glm::vec3& buttonPos, float thresholdAngleDegrees);
	void buy(GOPrize item, GOPlayer buyer, GOPrizeStock& itemstock);
	void PickItem(GOPrizeStock& itemstock);

	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];

	GOPlayer player;

	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];

	FPCamera camera;
	int projType = 1; // fix to 0 for orthographic, 1 for projection

	MatrixStack modelStack, viewStack, projectionStack;

	static const int NUM_LIGHTS = 3;
	Light light[NUM_LIGHTS];
	bool enableLight;

	bool textshowing;

	int prizecount;

	int ticketcount;

	//Remove later
	int buynumber = 0;

	GOPrize Burger;
	GOPrizeStock BurgStock;

	//Chips - 4
	GOPrize Chips;
	GOPrizeStock ChipStock;

	//Bob-Omb - 1
	GOPrize BobOmb;
	GOPrizeStock BobOmbStock;

	//Freddy Plushies - 4
	GOPrize Freddy;
	GOPrizeStock FreddyStock;

	//Dalek Figurines - 6
	GOPrize Dalek;
	GOPrizeStock DalekStock;

	//Miku Figurines - 4
	GOPrize Miku;
	GOPrizeStock MikuStock;
};

#endif