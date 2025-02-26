#ifndef SCENE_MODEL_H
#define SCENE_MODEL_H

#include "Scene.h"
#include "Mesh.h"
#include "AltAzCamera.h"
#include "FPCamera.h"
#include "CollisionDetection.h"
#include "PhysicsObject.h"
#include "MouseController.h"
#include "MatrixStack.h"
#include "SceneManager.h"
#include "SceneMenu.h"
#include "Light.h"
struct futtie : public PhysicsObject
{

	float radius;
	futtie();


};

struct goal : public PhysicsObject
{

	glm::vec3 dimension;
	

};
struct wall : public PhysicsObject
{

	glm::vec3 dimension;
	

};
struct goal_collision : public PhysicsObject
{
	glm::vec3 dimension;
};

class SceneModel : public Scene
{
public:
	enum GEOMETRY_TYPE
	{
		GEO_AXES,
		GEO_QUAD,
		GEO_SPHERE,
		GEO_CUBE,
		GEO_LEFT,
		GEO_FRONT,
		GEO_BACK,
		GEO_BOTTOM,
		GEO_RIGHT,
		GEO_TOP,
		GEO_PLANE,
		GEO_FUTBALL,
		GEO_GROUND,
		GEO_GUI,
		GEO_GOAL,
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
		U_NUMLIGHTS,
		U_LIGHTENABLED,
		U_TEXT_ENABLED,
		U_TEXT_COLOR,
		// Step 5 - Add the enumerators below
		U_COLOR_TEXTURE_ENABLED,
		U_COLOR_TEXTURE,
		U_TOTAL,
	};

	SceneModel();
	~SceneModel();
	bool camlock;
	virtual void Init();
	virtual void Update(double dt);
	virtual void Render();
	virtual void Exit();

	
private:
	static const int ballnum = 5;
	int score;
	int screenheight = 600;
	int screenwidth = 800;
	/*GLFWwindow* window;*/
	double mouseX, mouseY;
	futtie ball;
	
	goal_collision goalleft;
	goal_collision goalright;
	goal_collision goaltop;
	goal_collision goalfront;

	int failure_count;
	goal goalpost;

	wall leftwall;
	wall rightwall;
	wall topwall;
	wall frontwall;
	wall backwall;
	wall ground;
	glm::vec3 Impulse;
	void HandleKeyPress();
	void RenderMesh(Mesh* mesh, bool enableLight);
	float fps = 0;
	unsigned m_vertexArrayID;
	Mesh* meshList[NUM_GEOMETRY];
	
	int health = 50;
	double shotpower;
	
	bool fruitActive = true; //track fruit
	bool flashlightOn = false; // Track flashlight
	glm::vec3 flashlightPosition;
	glm::vec3 flashlightDirection;
	unsigned m_programID;
	unsigned m_parameters[U_TOTAL];
	FPCamera camera;
	void applyForceToBall(glm::vec3& ballVelocity, glm::vec3 forceDirection, float forceMagnitude);
	int projType = 1; // fix to 0 for orthographic, 1 for projection
	bool CheckRaySphereIntersection(glm::vec3 rayOrigin, glm::vec3 rayDir, glm::vec3 sphereCenter, float sphereRadius);
	MatrixStack modelStack, viewStack, projectionStack;
	
	static const int NUM_LIGHTS = 1;
	Light light[NUM_LIGHTS];
	bool enableLight;
	void RenderMeshOnScreen(Mesh* mesh, float x, float y,
		float sizex, float sizey);
	void RenderStamina(Mesh* mesh, float x, float y, float sizex, float sizey);
	void HandleMouseInput();
	void RenderText(Mesh* mesh, std::string text, glm::vec3	color);
	void RenderTextOnScreen(Mesh* mesh, std::string text, glm::vec3 color, float size, float x, float y);
	void OnMouseClick(int button, int action);

	bool isLooking(const FPCamera& camera, const glm::vec3& buttonPos, float thresholdAngleDegrees);
	bool rendercheck;
	bool EndGame();
};

#endif