#include "GL\glew.h"
#include "ScenePrizeCounter.h"

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_inverse.hpp>
#include <glm/gtc/constants.hpp>

//Include GLFW
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "KeyboardController.h"
#include "MouseController.h"
#include "LoadTGA.h"
#include "LoadOBJ.h"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <Windows.h>
#include <sstream>

ScenePrizeCounter::ScenePrizeCounter()
{
}

ScenePrizeCounter::~ScenePrizeCounter()
{
}

void ScenePrizeCounter::Init()
{
	// Set background color to dark blue
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	//Enable depth buffer and depth testing
	glEnable(GL_DEPTH_TEST);

	//Enable back face culling
	glEnable(GL_CULL_FACE);

	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	// Load the shader programs
	m_programID = LoadShaders("Shader//Texture.vertexshader",
		"Shader//Texture.fragmentshader");
	m_programID = LoadShaders("Shader//Texture.vertexshader", "Shader//Text.fragmentshader");
	glUseProgram(m_programID);

	// Get a handle for our "MVP" uniform
	m_parameters[U_MVP] = glGetUniformLocation(m_programID, "MVP");
	m_parameters[U_MODELVIEW] = glGetUniformLocation(m_programID, "MV");
	m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE] = glGetUniformLocation(m_programID, "MV_inverse_transpose");
	m_parameters[U_MATERIAL_AMBIENT] = glGetUniformLocation(m_programID, "material.kAmbient");
	m_parameters[U_MATERIAL_DIFFUSE] = glGetUniformLocation(m_programID, "material.kDiffuse");
	m_parameters[U_MATERIAL_SPECULAR] = glGetUniformLocation(m_programID, "material.kSpecular");
	m_parameters[U_MATERIAL_SHININESS] = glGetUniformLocation(m_programID, "material.kShininess");
	m_parameters[U_LIGHT0_TYPE] = glGetUniformLocation(m_programID, "lights[0].type");
	m_parameters[U_LIGHT0_POSITION] = glGetUniformLocation(m_programID, "lights[0].position_cameraspace");
	m_parameters[U_LIGHT0_COLOR] = glGetUniformLocation(m_programID, "lights[0].color");
	m_parameters[U_LIGHT0_POWER] = glGetUniformLocation(m_programID, "lights[0].power");
	m_parameters[U_LIGHT0_KC] = glGetUniformLocation(m_programID, "lights[0].kC");
	m_parameters[U_LIGHT0_KL] = glGetUniformLocation(m_programID, "lights[0].kL");
	m_parameters[U_LIGHT0_KQ] = glGetUniformLocation(m_programID, "lights[0].kQ");
	m_parameters[U_LIGHT0_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[0].spotDirection");
	m_parameters[U_LIGHT0_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[0].cosCutoff");
	m_parameters[U_LIGHT0_COSINNER] = glGetUniformLocation(m_programID, "lights[0].cosInner");
	m_parameters[U_LIGHT0_EXPONENT] = glGetUniformLocation(m_programID, "lights[0].exponent");

	m_parameters[U_LIGHT1_TYPE] = glGetUniformLocation(m_programID, "lights[1].type");
	m_parameters[U_LIGHT1_POSITION] = glGetUniformLocation(m_programID, "lights[1].position_cameraspace");
	m_parameters[U_LIGHT1_COLOR] = glGetUniformLocation(m_programID, "lights[1].color");
	m_parameters[U_LIGHT1_POWER] = glGetUniformLocation(m_programID, "lights[1].power");
	m_parameters[U_LIGHT1_KC] = glGetUniformLocation(m_programID, "lights[1].kC");
	m_parameters[U_LIGHT1_KL] = glGetUniformLocation(m_programID, "lights[1].kL");
	m_parameters[U_LIGHT1_KQ] = glGetUniformLocation(m_programID, "lights[1].kQ");
	m_parameters[U_LIGHT1_SPOTDIRECTION] = glGetUniformLocation(m_programID, "lights[1].spotDirection");
	m_parameters[U_LIGHT1_COSCUTOFF] = glGetUniformLocation(m_programID, "lights[1].cosCutoff");
	m_parameters[U_LIGHT1_COSINNER] = glGetUniformLocation(m_programID, "lights[1].cosInner");
	m_parameters[U_LIGHT1_EXPONENT] = glGetUniformLocation(m_programID, "lights[1].exponent");

	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");

	// Initialise camera properties
	camera.Init(glm::vec3(10,3,0), glm::vec3(0,0,0), glm::vec3(0,50,0));
	playerposition = camera.position;

	//Burger - 10
	Burger = GOPrize(10, "Hamburger", "A juicy hamburger preserved by the magic of not being real", 1.4, 1.4, 1.4, 0.5, 3.2, 0.1);
	BurgStock = GOPrizeStock(10, Burger, 5, 2);

	//Chips - 4
	Chips = GOPrize(400, "Chips", "Crunchy chips from a brand that will absolutely sue us for this", 1.2, 1.2, 1.2, 0.0, -3.0, 0.3);
	ChipStock = GOPrizeStock(4, Chips, 2, 2);

	//Bob-Omb - 1
	BobOmb = GOPrize(10000, "Bob-Omb Plush", "Be careful, he might explode!", 5, 5, 5, 5, 5, -1.5);
	BobOmbStock = GOPrizeStock(1, BobOmb, 1, 1);

	//Freddy Plushies - 4
	Freddy = GOPrize(5000, "Freddy Fazbear Plush", "Fazbear Entertainment is not liable for damages if the plush starts talking to you", 1.4, 2.0, 2.0, 0.0, 3.0, 2.1);
	FreddyStock = GOPrizeStock(4, Freddy, 2, 2);

	//Dalek Figurines - 6
	Dalek = GOPrize(4000, "Dalek Figurine", "It makes laser sounds! Exterminate! Exterminate!", 1.35, 1.12, 1.12, 0.4, -2, 2.3);
	DalekStock = GOPrizeStock(6, Dalek, 3, 2);

	//Miku Figurines - 4
	Miku = GOPrize(3000, "Hatsune Miku Figurine", "Comes with a CD in the base!", 0.96, 1.50, 1.50, 0.29, 0, 1.7);
	MikuStock = GOPrizeStock(4, Miku, 2, 2);

	prizecount = BurgStock.GetStock() +
		ChipStock.GetStock() +
		BobOmbStock.GetStock() +
		FreddyStock.GetStock() +
		DalekStock.GetStock() +
		MikuStock.GetStock();

	// Init VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT], m_parameters[U_MATERIAL_DIFFUSE], m_parameters[U_MATERIAL_SPECULAR], m_parameters[U_MATERIAL_SHININESS]);

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_SPHERE] = MeshBuilder::GenerateSphere("Sun", glm::vec3(1.f, 1.f, 1.f), 1.f, 16, 16);
	meshList[GEO_CUBE] = MeshBuilder::GenerateCube("Arm", glm::vec3(0.5f, 0.5f, 0.5f), 1.f);
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("Left", glm::vec3(0.4f, 0.4f, 0.4f), 100.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Images//redstrip_texture.tga");
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("Right", glm::vec3(0.4f, 0.4f, 0.4f), 100.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Images//redstrip_texture.tga");
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("Top", glm::vec3(0.4f, 0.4f, 0.4f), 100.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Images//top.tga");
	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("Bottom", glm::vec3(0.4f, 0.4f, 0.4f), 100.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Images//redstrip_texture.tga");
	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("Front", glm::vec3(0.4f, 0.4f, 0.4f), 100.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Images//redstrip_texture.tga");
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("Back", glm::vec3(0.4f, 0.4f, 0.4f), 100.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Images//redstrip_texture.tga");

	// 16 x 16 is the number of columns and rows for the text
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");

	meshList[GEO_SHELF] = MeshBuilder::GenerateOBJMTL("Shelf", "Models//Brett.obj", "Models//Brett.mtl");
	meshList[GEO_SHELF]->textureID = LoadTGA("Images//woodtex.tga");

	meshList[GEO_MIKU] = MeshBuilder::GenerateOBJMTL("Miku", "Models//MikuFigure.obj", "Models//MikuFigure.mtl");
	meshList[GEO_MIKU]->textureID = LoadTGA("Images//roomitems_002_body.tga");

	meshList[GEO_BURGER] = MeshBuilder::GenerateOBJMTL("Burger", "Models//Hamburger.obj", "Models//Hamburger.mtl");
	meshList[GEO_BURGER]->textureID = LoadTGA("Images//Hamburger_BaseColor.tga");

	meshList[GEO_CHIPS] = MeshBuilder::GenerateOBJMTL("Chips", "Models//chips.obj", "Models//chips.mtl");
	meshList[GEO_CHIPS]->textureID = LoadTGA("Images//Lays.tga");

	meshList[GEO_DALEK] = MeshBuilder::GenerateOBJMTL("Dalek", "Models//dalek.obj", "Models//dalek.mtl");
	meshList[GEO_DALEK]->textureID = LoadTGA("Images//dalek_None_color.tga");

	meshList[GEO_FREDDY] = MeshBuilder::GenerateOBJMTL("Freddy", "Models//freddy.obj", "Models//freddy.mtl");
	meshList[GEO_FREDDY]->textureID = LoadTGA("Images//FreddyPlush_UVs_Mesh_lambert1_BaseColor.tga");

	meshList[GEO_BOBOMB] = MeshBuilder::GenerateOBJMTL("BobOmb", "Models//Bomb-Omb.obj", "Models//Bomb-Omb.mtl");
	meshList[GEO_BOBOMB]->textureID = LoadTGA("Images//Bomb_Color.tga");

	glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);

	glUniform1i(m_parameters[U_NUMLIGHTS], NUM_LIGHTS);

	light[0].position = glm::vec3(-1.10709, 7.27968, 1.36014);
	light[0].color = glm::vec3(1, 1, 1);
	light[0].type = Light::LIGHT_POINT;
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = 45.f;
	light[0].cosInner = 30.f;
	light[0].exponent = 3.f;
	light[0].spotDirection = glm::vec3(0.f, 1.f, 0.f);

	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, &light[0].color.r);
	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], cosf(glm::radians<float>(light[0].cosCutoff)));
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], cosf(glm::radians<float>(light[0].cosInner)));
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);

	light[1].position = glm::vec3(0, 20, 0);
	light[1].color = glm::vec3(0, 0.3, 1);
	light[1].type = Light::LIGHT_SPOT;
	light[1].power = 1;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = 45.f;
	light[1].cosInner = 30.f;
	light[1].exponent = 3.f;
	light[1].spotDirection = glm::vec3(0.f, 1.f, 0.f);

	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, &light[1].color.r);
	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], cosf(glm::radians<float>(light[1].cosCutoff)));
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], cosf(glm::radians<float>(light[1].cosInner)));
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);

	light[2].position = glm::vec3(0, 10, 0);
	light[2].color = glm::vec3(1, 1, 1);
	light[2].type = Light::LIGHT_DIRECTIONAL;
	light[2].power = 0.3;
	light[2].kC = 1.f;
	light[2].kL = 0.01f;
	light[2].kQ = 0.001f;
	light[2].cosCutoff = 45.f;
	light[2].cosInner = 30.f;
	light[2].exponent = 3.f;
	light[2].spotDirection = glm::vec3(-1.f, 0.f, 0.f);

	glUniform3fv(m_parameters[U_LIGHT2_COLOR], 1, &light[2].color.r);
	 glUniform1i(m_parameters[U_LIGHT2_TYPE], light[2].type);
	 glUniform1f(m_parameters[U_LIGHT2_POWER], light[2].power);
	 glUniform1f(m_parameters[U_LIGHT2_KC], light[2].kC);
	 glUniform1f(m_parameters[U_LIGHT2_KL], light[2].kL);
	 glUniform1f(m_parameters[U_LIGHT2_KQ], light[2].kQ);
	 glUniform1f(m_parameters[U_LIGHT2_COSCUTOFF], cosf(glm::radians<float>(light[2].cosCutoff)));
	 glUniform1f(m_parameters[U_LIGHT2_COSINNER], cosf(glm::radians<float>(light[2].cosInner)));
	 glUniform1f(m_parameters[U_LIGHT2_EXPONENT], light[2].exponent);

	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	enableLight = true;

	textshowing = true;

	

	/*
	
	Plan: Finish the Prize Counter

		Tie rendering to object creation?
			Designate coordinates on the shelf per object
				Less customisability, more practicality
					Items disappear from shelf when bought,
					items can go out of stock
		
		Create a per-object stock
			Tie number of objects rendered to object stock
				Rows and columns rendering
					Use for loop to render, 3 layers of for loop
					---------------------------------------------
					- objects less than object stock
					  - rows less than column
						- less than max number of objects per row
					---------------------------------------------
				New class of ObjectStock
					Use to keep track of objects
						Create objects inside ObjStock?
							ObjStock constructor takes in number of Obj to create

		Create player inventory?
			Not exactly necessary at the moment
				No max capacity
		
		Bought items will appear in the hub world or main menu
			
	*/
}

void ScenePrizeCounter::Update(double dt)
{
	//std::cout << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << std::endl;

	HandleKeyPress();

	/*nearbox = false;
	nearsafe = false;
	nearwheatley = false;
	neardoor = false;

	//std::cout << light[1].power << std::endl;
	//std::cout << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << std::endl;
	//Wheatley/Chair and Player collision
	if (OverlapAABB2AABB(
		glm::vec3(camera.position.x - 1, camera.position.y, camera.position.z - 1),
		glm::vec3(camera.position.x + 1, camera.position.y, camera.position.z + 1),
		glm::vec3(wheatleyposition.x - wheatExtent.x - 2, wheatleyposition.y, wheatleyposition.z - wheatExtent.z - 2),
		glm::vec3(wheatleyposition.x + wheatExtent.x + 2, wheatleyposition.y, wheatleyposition.z + wheatExtent.z + 2))
		)
	{
		if (camera.position.x > wheatleyposition.x) {
			camera.position.x += 0.125;
		}
		else if (camera.position.x < wheatleyposition.x) {
			camera.position.x -= 0.14;
		}
		if (camera.position.z > wheatleyposition.z) {
			camera.position.z += 0.14;
		}
		else if (camera.position.z < wheatleyposition.z) {
			camera.position.z -= 0.14;
		}
		//std::cout << "Collided!" << std::endl;
	}

	//Box and Player collision
	if (OverlapAABB2AABB(
		glm::vec3(camera.position.x - 1, camera.position.y, camera.position.z - 1),
		glm::vec3(camera.position.x + 1, camera.position.y, camera.position.z + 1),
		glm::vec3(boxposition.x - boxExtent.x - 2, boxposition.y, boxposition.z - boxExtent.z - 2),
		glm::vec3(boxposition.x + boxExtent.x + 2, boxposition.y, boxposition.z + boxExtent.z + 2))
		)
	{
		if (camera.position.x > boxposition.x) {
			camera.position.x += 0.125;
		}
		else if (camera.position.x < boxposition.x) {
			camera.position.x -= 0.14;
		}
		if (camera.position.z > boxposition.z) {
			camera.position.z += 0.14;
		}
		else if (camera.position.z < boxposition.z) {
			camera.position.z -= 0.14;
		}
		//std::cout << "Collided!" << std::endl;
	}

	//Safe and Player collision
	if (OverlapAABB2AABB(
		glm::vec3(camera.position.x - 1, camera.position.y, camera.position.z - 1),
		glm::vec3(camera.position.x + 1, camera.position.y, camera.position.z + 1),
		glm::vec3(safeposition.x - safeExtent.x - 2, safeposition.y, safeposition.z - safeExtent.z - 2),
		glm::vec3(safeposition.x + safeExtent.x + 2, safeposition.y, safeposition.z + safeExtent.z + 2))
		)
	{
		if (camera.position.x > safeposition.x) {
			camera.position.x += 0.125;
		}
		else if (camera.position.x < safeposition.x) {
			camera.position.x -= 0.14;
		}
		if (camera.position.z > safeposition.z) {
			camera.position.z += 0.14;
		}
		else if (camera.position.z < safeposition.z) {
			camera.position.z -= 0.14;
		}
		//std::cout << "Collided!" << std::endl;
	}

	//Door and Player collision
	if (OverlapAABB2AABB(
		glm::vec3(camera.position.x - 1, camera.position.y, camera.position.z - 1),
		glm::vec3(camera.position.x + 1, camera.position.y, camera.position.z + 1),
		glm::vec3(doorposition.x - doorExtent.x - 2, doorposition.y, doorposition.z - doorExtent.z - 2),
		glm::vec3(doorposition.x + doorExtent.x + 2, doorposition.y, doorposition.z + doorExtent.z + 2))
		)
	{
		if (camera.position.x > doorposition.x) {
			camera.position.x += 0.125;
		}
		else if (camera.position.x < doorposition.x) {
			camera.position.x -= 0.14;
		}
		if (camera.position.z > doorposition.z) {
			camera.position.z += 0.14;
		}
		else if (camera.position.z < doorposition.z) {
			camera.position.z -= 0.14;
		}
		//std::cout << "Collided!" << std::endl;
	}

	//Add in Crate and Safe if/else statements


	if (WithinRange(camera.target, boxposition, 5))
	{
		std::cout << "Range Box" << std::endl;
		if (!keyget)
		{
			nearbox = true;
			if (MouseController::GetInstance()->IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
			{
				keyget = true;
				std::cout << "Key get" << std::endl;

			}
		}
	}

	if (WithinRange(camera.target, safeposition, 4))
	{
		std::cout << "Range" << std::endl;
		if (keyget){
			std::cout << "Key get" << std::endl;
			nearsafe = true;
			if (MouseController::GetInstance()->IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
			{
				hammerget = true;
			}
		}
	}

	if (WithinRange(camera.target, wheatleyposition, 7))
	{
		if (!wheatleypickedup)
		{
			nearwheatley = true;
			if (hammerget && !wheatleyfree)
			{
				if (MouseController::GetInstance()->IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
				{
					wheatleyfree = true;
				}
			}

			if (wheatleyfree)
			{
				if (MouseController::GetInstance()->IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
				{
					wheatleypickedup = true;

				}
			}
		}
	}

	if (WithinRange(camera.target, doorposition, 10))
	{
		if (wheatleypickedup)
		{
			neardoor = true;
			if (MouseController::GetInstance()->IsButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
			{
				bigjumpscare = true;

			}
		}
	}*/
	
	if (camera.position.x > 24.45f) {
			camera.position.x -= 0.24;
	}
	else if (camera.position.x < 1.5) {
			camera.position.x += 0.24;
	}
	if (camera.position.z < -5.7f) {
			camera.position.z += 0.24;
	}
	else if (camera.position.z > 5.5f) {
			camera.position.z -= 0.24;
	}
	//std::cout << "Collided!" << std::endl;
	
	//blue rat go fast

	if (camera.position.x >= 49.0f)
	{
		camera.position.x -= 0.14;
	}
	else if (camera.position.x <= -49.0f)
	{
		camera.position.x += 0.14;
	}
	if (camera.position.z >= 49.0f)
	{
		camera.position.z -= 0.14;
	}
	else if (camera.position.z <= -49.0f)
	{
		camera.position.z += 0.14;
	}

	camera.Update(dt);
}

void ScenePrizeCounter::Render()
{
	// Clear color buffer every frame
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Load view matrix stack and set it with camera position, target position and up direction
	viewStack.LoadIdentity();
	viewStack.LookAt(
		camera.position.x, camera.position.y, camera.position.z,
		camera.target.x, camera.target.y, camera.target.z,
		camera.up.x, camera.up.y, camera.up.z
	);

	// Load identity matrix into the model stack
	modelStack.LoadIdentity();

	//Point Light
	glm::vec3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
	glm::vec3 lightDirection_cameraspace = viewStack.Top() * glm::vec4(lightDir, 0);
	glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightDirection_cameraspace));

	//Spotlight
	glm::vec3 lightPosition_cameraspace1 = viewStack.Top() * glm::vec4(light[1].position, 1);
	glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, glm::value_ptr(lightPosition_cameraspace1));
	glm::vec3 spotDirection_cameraspace1 = viewStack.Top() * glm::vec4(light[1].spotDirection, 0);
	glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, glm::value_ptr(lightPosition_cameraspace1));

	//Directional
	glm::vec3 lightPosition_cameraspace2 = viewStack.Top() * glm::vec4(light[2].position, 1);
	glUniform3fv(m_parameters[U_LIGHT2_POSITION], 1, glm::value_ptr(lightPosition_cameraspace2));


	RenderSkybox();
	//RenderMeshOnScreen(meshList[GEO_PLANE], 50, 50, 10, 10);

	/*modelStack.PushMatrix();
	meshList[GEO_CHAIR]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_CHAIR]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_CHAIR]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_CHAIR]->material.kShininess = 5.0f;
	modelStack.Translate(-1.3, -2.5, 0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(15.0f, 15.0f, 15.0f);
	RenderMesh(meshList[GEO_CHAIR], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	meshList[GEO_DOOR]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_DOOR]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_DOOR]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_DOOR]->material.kShininess = 5.0f;
	modelStack.Translate(doorposition.x, doorposition.y, doorposition.z);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Rotate(90, -1, 0, 0);
	modelStack.Scale(0.05f, 0.05f, 0.05f);
	RenderMesh(meshList[GEO_DOOR], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	meshList[GEO_CRATE]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_CRATE]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_CRATE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_CRATE]->material.kShininess = 5.0f;
	modelStack.Translate(boxposition.x, boxposition.y, boxposition.z);
	modelStack.Scale(5.f, 5.f, 5.f);
	RenderMesh(meshList[GEO_CRATE], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	meshList[GEO_SAFE]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_SAFE]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_SAFE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_SAFE]->material.kShininess = 5.0f;
	modelStack.Translate(safeposition.x, safeposition.y, safeposition.z);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(20.f, 20.f, 20.f);
	RenderMesh(meshList[GEO_SAFE], true);
	modelStack.PopMatrix();*/

	modelStack.PushMatrix();
	meshList[GEO_SHELF]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_SHELF]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_SHELF]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_SHELF]->material.kShininess = 5.0f;
	modelStack.Translate(0, -2.5, 0);
	modelStack.Rotate(0, 1, 0, 0);
	modelStack.Scale(4.5f, 4.5f, 4.5f);
	RenderMesh(meshList[GEO_SHELF], true);
	modelStack.PopMatrix();


	//Burger - 10

	/*
	* for every burger object, render one burger
	* {
			modelStack.PushMatrix();
			meshList[GEO_BURGER]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_BURGER]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_BURGER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			meshList[GEO_BURGER]->material.kShininess = 5.0f;
			modelStack.Translate(0.5 - (j * (Burger.width / 2) ), 
								 0.1, 
								 3.2 - (k * (Burger.depth / 2) ) );
			modelStack.Rotate(60, 0, 1, 0);
			modelStack.Scale(0.1f, 0.15f, 0.1f);
			RenderMesh(meshList[GEO_BURGER], true);
			modelStack.PopMatrix();
		}
	
	*/
	for (int i = 0; i < BurgStock.GetStock(); i++)
	{

		if (BurgStock.prizearray[i].bought == false)
		{
			modelStack.PushMatrix();
			meshList[GEO_BURGER]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_BURGER]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_BURGER]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			meshList[GEO_BURGER]->material.kShininess = 5.0f;
			modelStack.Translate(BurgStock.prizearray[i].position.x,
				BurgStock.prizearray[i].position.y,
				BurgStock.prizearray[i].position.z);
			modelStack.Rotate(60, 0, 1, 0);
			modelStack.Scale(0.1f, 0.15f, 0.1f);
			RenderMesh(meshList[GEO_BURGER], true);
			modelStack.PopMatrix();
		}

	}

	//Chips - 4\

	for (int i = 0; i < ChipStock.GetStock(); i++)
	{

		if (ChipStock.prizearray[i].bought == false)
		{
			modelStack.PushMatrix();
			meshList[GEO_CHIPS]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_CHIPS]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_CHIPS]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			meshList[GEO_CHIPS]->material.kShininess = 5.0f;
			modelStack.Translate(ChipStock.prizearray[i].position.x,
				ChipStock.prizearray[i].position.y,
				ChipStock.prizearray[i].position.z);
			modelStack.Rotate(60, 0, 1, 0);
			modelStack.Rotate(45, 1, 0, 0);
			modelStack.Scale(0.3f, 0.3f, 0.3f);
			RenderMesh(meshList[GEO_CHIPS], true);
			modelStack.PopMatrix();
		}

	}

	//Bob-Omb - 1
	
	for (int i = 0; i < BobOmbStock.GetStock(); i++)
	{

		if (BobOmbStock.prizearray[i].bought == false)
		{
			modelStack.PushMatrix();
			meshList[GEO_BOBOMB]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_BOBOMB]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_BOBOMB]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			meshList[GEO_BOBOMB]->material.kShininess = 5.0f;
			modelStack.Translate(BobOmbStock.prizearray[i].position.x,
				BobOmbStock.prizearray[i].position.y,
				BobOmbStock.prizearray[i].position.z);
			modelStack.Rotate(0, 1, 0, 0);
			modelStack.Scale(4.5f, 4.5f, 4.5f);
			RenderMesh(meshList[GEO_BOBOMB], true);
			modelStack.PopMatrix();
		}

	}

	//Freddy Plushies - 4

	for (int i = 0; i < FreddyStock.GetStock(); i++)
	{

		if (FreddyStock.prizearray[i].bought == false)
		{
			modelStack.PushMatrix();
			meshList[GEO_FREDDY]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_FREDDY]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_FREDDY]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			meshList[GEO_FREDDY]->material.kShininess = 5.0f;
			modelStack.Translate(FreddyStock.prizearray[i].position.x,
				FreddyStock.prizearray[i].position.y,
				FreddyStock.prizearray[i].position.z);
			modelStack.Rotate(90, 0, 1, 0);
			modelStack.Rotate(15, -1, 0, 0);
			modelStack.Scale(3.5f, 3.5f, 3.5f);
			RenderMesh(meshList[GEO_FREDDY], true);
			modelStack.PopMatrix();
		}

	}

	//Dalek Figurines - 6

	for (int i = 0; i < DalekStock.GetStock(); i++)
	{

		if (DalekStock.prizearray[i].bought == false)
		{
			modelStack.PushMatrix();
			meshList[GEO_DALEK]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_DALEK]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_DALEK]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			meshList[GEO_DALEK]->material.kShininess = 5.0f;
			modelStack.Translate(DalekStock.prizearray[i].position.x,
				DalekStock.prizearray[i].position.y,
				DalekStock.prizearray[i].position.z);
			modelStack.Rotate(90, 0, 1, 0);
			modelStack.Scale(0.07f, 0.07f, 0.07f);
			RenderMesh(meshList[GEO_DALEK], true);
			modelStack.PopMatrix();
		}

	}

	//Miku Figurines - 4

	for (int i = 0; i < MikuStock.GetStock(); i++)
	{

		if (MikuStock.prizearray[i].bought == false)
		{
			modelStack.PushMatrix();
			meshList[GEO_MIKU]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_MIKU]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_MIKU]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			meshList[GEO_MIKU]->material.kShininess = 5.0f;
			modelStack.Translate(MikuStock.prizearray[i].position.x,
				MikuStock.prizearray[i].position.y,
				MikuStock.prizearray[i].position.z);
			modelStack.Rotate(90, 0, 1, 0);
			modelStack.Scale(0.3f, 0.3f, 0.3f);
			RenderMesh(meshList[GEO_MIKU], true);
			modelStack.PopMatrix();
		}

	}

	modelStack.PushMatrix();
	meshList[GEO_SPHERE]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_SPHERE]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_SPHERE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_SPHERE]->material.kShininess = 5.0f;
	// Render light
	modelStack.Translate(light[1].position.x, light[1].position.y, light[1].position.z);
	modelStack.Scale(0.1f, 0.5f, 0.5f);
	RenderMesh(meshList[GEO_SPHERE], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	meshList[GEO_FRONT]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_FRONT]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_FRONT]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_FRONT]->material.kShininess = 5.0f;

	modelStack.Translate(0.f, 0.f, -6.0f);
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_FRONT], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	meshList[GEO_BACK]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BACK]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_BACK]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_BACK]->material.kShininess = 5.0f;

	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Translate(0.f, 0.f, -8.2375f);
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_BACK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	meshList[GEO_LEFT]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_LEFT]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_LEFT]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_LEFT]->material.kShininess = 5.0f;

	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Translate(0.f, 0.f, -2.0f);
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	// Skybox should be rendered without light

	RenderMesh(meshList[GEO_LEFT], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	meshList[GEO_RIGHT]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_RIGHT]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_RIGHT]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_RIGHT]->material.kShininess = 5.0f;


	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Translate(0.f, 0.f, -24.95f);
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	// Skybox should be rendered without light

	RenderMesh(meshList[GEO_RIGHT], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	meshList[GEO_TOP]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_TOP]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_TOP]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_TOP]->material.kShininess = 5.0f;

	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Translate(0.f, 0.f, -7.475f);
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	modelStack.Rotate(90, 0, 0, 1);
	// Skybox should be rendered without light

	RenderMesh(meshList[GEO_TOP], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	//scale, translate, rotate
	RenderTextOnScreen(meshList[GEO_TEXT], "Welcome to the prize corner!", glm::vec3(1, 1, 1), 20, 120, 500);
	modelStack.PopMatrix();

	PickItem(BurgStock);
	PickItem(ChipStock);
	PickItem(BobOmbStock);
	PickItem(FreddyStock);
	PickItem(DalekStock);
	PickItem(MikuStock);


	//if (nearbox)
	//{
	//	modelStack.PushMatrix();
	//	//scale, translate, rotate
	//	RenderTextOnScreen(meshList[GEO_TEXT], "Search the Crate", glm::vec3(0, 1, 0), 20, 50, 470);
	//	modelStack.PopMatrix();
	//}

}

void ScenePrizeCounter::RenderMesh(Mesh* mesh, bool enableLight)
{
	glm::mat4 MVP, modelView, modelView_inverse_transpose;

	MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));
	modelView = viewStack.Top() * modelStack.Top();
	glUniformMatrix4fv(m_parameters[U_MODELVIEW], 1, GL_FALSE, glm::value_ptr(modelView));
	if (enableLight)
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 1);
		modelView_inverse_transpose = glm::inverseTranspose(modelView);
		glUniformMatrix4fv(m_parameters[U_MODELVIEW_INVERSE_TRANSPOSE], 1, GL_FALSE, glm::value_ptr(modelView_inverse_transpose));

		//load material
		glUniform3fv(m_parameters[U_MATERIAL_AMBIENT], 1, &mesh->material.kAmbient.r);
		glUniform3fv(m_parameters[U_MATERIAL_DIFFUSE], 1, &mesh->material.kDiffuse.r);
		glUniform3fv(m_parameters[U_MATERIAL_SPECULAR], 1, &mesh->material.kSpecular.r);
		glUniform1f(m_parameters[U_MATERIAL_SHININESS], mesh->material.kShininess);
	}
	else
	{
		glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	}

	if (mesh->textureID > 0)
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->textureID);
		glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	}
	else
	{
		glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 0);
	}

	mesh->Render();

	if (mesh->textureID > 0)
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}

void ScenePrizeCounter::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
{
	glDisable(GL_DEPTH_TEST);
	glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, - 1000.f, 1000.f); // dimension of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity();

	// To do: Use modelStack to position GUI on screen
	modelStack.Translate(x, y, 0);
	// To do: Use modelStack to scale the GUI
	modelStack.Scale(sizex, sizey, 1);

	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void ScenePrizeCounter::RenderText(Mesh* mesh, std::string text, glm::vec3 color)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;
	
	// Enable blending  
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Disable back face culling
	glDisable(GL_CULL_FACE);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);

	for (unsigned i = 0; i < text.length(); ++i)
	{
		glm::mat4 characterSpacing = glm::translate(
			glm::mat4(1.f),
			glm::vec3(i * 1.0f, 0, 0));
		glm::mat4 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void ScenePrizeCounter::RenderTextOnScreen(Mesh* mesh, std::string text, glm::vec3 color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;
	// Enable blending  
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, -100.f, 100.f); // dimension of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity(); //Reset modelStack
	modelStack.Translate(x, y, 0);
	modelStack.Scale(size, size, size);

	glUniform1i(m_parameters[U_TEXT_ENABLED], 1);
	glUniform3fv(m_parameters[U_TEXT_COLOR], 1, &color.r);
	glUniform1i(m_parameters[U_LIGHTENABLED], 0);
	glUniform1i(m_parameters[U_COLOR_TEXTURE_ENABLED], 1);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mesh->textureID);
	glUniform1i(m_parameters[U_COLOR_TEXTURE], 0);
	for (unsigned i = 0; i < text.length(); ++i)
	{
		glm::mat4 characterSpacing = glm::translate(
			glm::mat4(1.f),
			glm::vec3(0.5f + i * 1.0f, 0.5f, 0)
		);
		glm::mat4 MVP = projectionStack.Top() * viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE, glm::value_ptr(MVP));
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

}

void ScenePrizeCounter::RenderSkybox()
{
	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	meshList[GEO_FRONT]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_FRONT]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_FRONT]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_FRONT]->material.kShininess = 5.0f;

	modelStack.Translate(0.f, 0.f, -49.9f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_FRONT], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	meshList[GEO_BACK]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BACK]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_BACK]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_BACK]->material.kShininess = 5.0f;

	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Translate(0.f, 0.f, -49.9f);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_BACK], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	meshList[GEO_LEFT]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_LEFT]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_LEFT]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_LEFT]->material.kShininess = 5.0f;

	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Translate(0.f, 0.f, -49.9f);
	// Skybox should be rendered without light

	RenderMesh(meshList[GEO_LEFT], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	meshList[GEO_RIGHT]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_RIGHT]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_RIGHT]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_RIGHT]->material.kShininess = 5.0f;

	modelStack.Rotate(270, 0, 1, 0);
	modelStack.Translate(0.f, 0.f, -49.9f);
	// Skybox should be rendered without light

	RenderMesh(meshList[GEO_RIGHT], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	meshList[GEO_TOP]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_TOP]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_TOP]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_TOP]->material.kShininess = 5.0f;

	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Translate(0.f, 0.f, -49.9f);
	modelStack.Rotate(90, 0, 0, 1);
	// Skybox should be rendered without light

	RenderMesh(meshList[GEO_TOP], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	meshList[GEO_BOTTOM]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BOTTOM]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_BOTTOM]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_BOTTOM]->material.kShininess = 5.0f;

	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Translate(0.f, 0.f, -3.0f);
	modelStack.Rotate(90, 0, 0, 1);
	// Skybox should be rendered without light

	RenderMesh(meshList[GEO_BOTTOM], true);
	modelStack.PopMatrix();

	// Do the rest of the quads with
	// appropriate positions and rotations
	// so that the camera is inside the skybox
}

void ScenePrizeCounter::HandleMouseInput()
{
	static bool isLeftUp = false;
	static bool isRightUp = false;
	// Process Left button
	if (!isLeftUp && MouseController::GetInstance() -> IsButtonDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		isLeftUp = true;
		std::cout << "LBUTTON DOWN" << std::endl;
	}
	else if (isLeftUp && MouseController::GetInstance() -> IsButtonUp(GLFW_MOUSE_BUTTON_LEFT))
	{
		isLeftUp = false;
		std::cout << "LBUTTON UP" << std::endl;
	}
	// Continue to do for right button

	if (!isLeftUp && MouseController::GetInstance() -> IsButtonDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		isLeftUp = true;
		std::cout << "LBUTTON DOWN" << std::endl;
		// Step 3
		// transform into UI space
		double x = MouseController::GetInstance() -> GetMousePositionX();
		double y = 600 - MouseController::GetInstance() -> GetMousePositionY();
		// Check if mouse click position is within the GUI box
		// Change the boundaries as necessary
		if (x > 0 && x < 100 && y > 0 && y < 100) {
			std::cout << "GUI IS CLICKED" << std::endl;
		}
		// End of step 3
	}
}

bool ScenePrizeCounter::isLooking(const FPCamera& camera, const glm::vec3& buttonPos, float thresholdAngleDegrees)
{
	glm::vec3 forward = glm::normalize(camera.target - camera.position);
	glm::vec3 toButton = glm::normalize(buttonPos -camera.position);
	float dotProduct = glm::dot(forward, toButton);
	float threshold = glm::cos(glm::radians(thresholdAngleDegrees));
	return (dotProduct >= threshold);
}

void ScenePrizeCounter::buy(GOPrize item, GOPlayer buyer, GOPrizeStock& itemstock)
{
	if (item.cost > buyer.ticketcount)
	{
		RenderTextOnScreen(meshList[GEO_TEXT], "Not enough tickets!", glm::vec3(1, 0, 0), 20, 50, 470);
	}
	else if (item.cost < buyer.ticketcount)
	{
		int OldStock;
		int BuyerInv;
		OldStock = itemstock.GetStock();
		BuyerInv = buyer.Inventory.GetStock();
		buynumber++;

		//Remove stock of that item
		buyer.ticketcount -= item.cost;
		itemstock.SetStock((OldStock - 1), item);
		buyer.Inventory.SetStock((BuyerInv + 1), item);
		std::cout << "Item purchased!" << std::endl << itemstock.GetStock() << std::endl << OldStock << std::endl;
		std::cout << buynumber << std::endl;
		RenderTextOnScreen(meshList[GEO_TEXT], "Item has been purchased!", glm::vec3(0, 1, 0), 20, 50, 470);
	}
}

void ScenePrizeCounter::PickItem(GOPrizeStock& itemstock)
{
	std::ostringstream buystatement;
	buystatement << "Buy a " << itemstock.StockedItem.name << "?";
	std::string bstmt = buystatement.str();

	for (int i = 0; i < itemstock.GetStock(); i++)
	{
		if (isLooking(camera, itemstock.prizearray[i].position, 4))
		{
			RenderTextOnScreen(meshList[GEO_TEXT], bstmt, glm::vec3(0, 1, 0), 20, 50, 30);

			if (KeyboardController::GetInstance()->IsKeyPressed('E'))
			{
				buy(itemstock.prizearray[i], player, itemstock);
			}
		}
	}

}


void ScenePrizeCounter::Exit()
{
	// Cleanup VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		if (meshList[i])
		{
			delete meshList[i];
		}
	}
	glDeleteVertexArrays(1, &m_vertexArrayID);
	glDeleteProgram(m_programID);
}

void ScenePrizeCounter::HandleKeyPress()
{
	if (KeyboardController::GetInstance()->IsKeyPressed(0x31))
	{
		// Key press to enable culling
		glEnable(GL_CULL_FACE);
	}
	if (KeyboardController::GetInstance()->IsKeyPressed(0x32))
	{
		// Key press to disable culling
		glDisable(GL_CULL_FACE);
	}
	if (KeyboardController::GetInstance()->IsKeyPressed(0x33))
	{
		// Key press to enable fill mode for the polygon
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); //default fill mode
	}
	if (KeyboardController::GetInstance()->IsKeyPressed(0x34))
	{
		// Key press to enable wireframe mode for the polygon
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); //wireframe mode
	}

	if (KeyboardController::GetInstance()->IsKeyPressed(VK_SPACE))
	{
		// Change to black background
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	}

	if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_0))
	{
		// Toggle light on or off
	/*	enableLight = !enableLight;*/

		if (light[0].power <= 0.1f)
			light[0].power = 1.f;
		else
			light[0].power = 0.1f;
		glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	}

	/*if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_TAB))
	{
		if (light[0].type == Light::LIGHT_POINT) {
			light[0].type = Light::LIGHT_DIRECTIONAL;
		}
		else if (light[0].type == Light::LIGHT_DIRECTIONAL) {
			light[0].type = Light::LIGHT_SPOT;
		}
		else {
			light[0].type = Light::LIGHT_POINT;
		}

		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}*/

}
