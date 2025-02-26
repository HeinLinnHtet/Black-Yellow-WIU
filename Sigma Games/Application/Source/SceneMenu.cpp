#include "SceneMenu.h"
#include "GL\glew.h"

// GLM Headers
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_inverse.hpp>

//Include GLFW
#include <GLFW/glfw3.h>

#include "shader.hpp"
#include "Application.h"
#include "MeshBuilder.h"
#include "KeyboardController.h"
#include "LoadTGA.h"
#include <MouseController.h>
#include <iostream>


SceneMenu::SceneMenu()
{
}

SceneMenu::~SceneMenu()
{
}

void SceneMenu::Init()
{
	// Set background color to dark blue
	glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

	//Enable depth buffer and depth testing
	glEnable(GL_DEPTH_TEST);

	/*Enable back face culling*/
	glEnable(GL_CULL_FACE);

	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	// Load the shader programs
	m_programID = LoadShaders("Shader//Texture.vertexshader",
		"Shader//Texture.fragmentshader");
	m_programID = LoadShaders("Shader//Texture.vertexshader",
		"Shader//Text.fragmentshader");
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
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");
	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	m_parameters[U_TEXT_ENABLED] =	glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID,	"textColor");
	camlock = false;
	// Initialise camera properties
	camera.Init({ 36,5,10 }, { -3,5,0 }, { 0,5,0 });
	// Init VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}

	Mesh::SetMaterialLoc(m_parameters[U_MATERIAL_AMBIENT], m_parameters[U_MATERIAL_DIFFUSE],
		m_parameters[U_MATERIAL_SPECULAR],
		m_parameters[U_MATERIAL_SHININESS]);

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);


	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Images//circus_skybox.tga");
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Images//circus_skybox.tga");
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Images//tenttop.tga");
	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Images//circus_skybox.tga");
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Images//circus_skybox.tga");
	meshList[GEO_GROUND] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 100.f);
	meshList[GEO_GROUND]->textureID = LoadTGA("Images//floor_rt.tga");
	meshList[GEO_DOOR] = MeshBuilder::GenerateOBJMTL("door", "Models//door.obj", "Models//door.mtl");
	meshList[GEO_DOOR]->textureID = LoadTGA("Images//Door_albedo.tga");


	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	//meshList[GEO_FUTBALL] = MeshBuilder::GenerateOBJMTL("soccer", "Models//soccer.obj", "Models//soccer.mtl");
	//meshList[GEO_FUTBALL]->textureID = LoadTGA("Images//soccer_ball.tga");
	//meshList[GEO_GOAL] = MeshBuilder::GenerateOBJMTL("soccer", "Models//goal.obj", "Models//goal.mtl");
	//meshList[GEO_GOAL]->textureID = LoadTGA("Images//goal1.tga");*/
	//meshList[GEO_STAMINA] = MeshBuilder::GenerateQuad("Quad", glm::vec3(5, 5, 5));
	//meshList[GEO_GUI] = MeshBuilder::GenerateQuad("Quad", glm::vec3(0.3, 0.3, 1));

	



	//meshList[GEO_TABLE] = MeshBuilder::GenerateOBJ("desk", "Models//Desk.obj"); 	// Step 7 - Load the OBJ file
	//meshList[GEO_TABLE]->textureID = LoadTGA("Images//desk.tga");


	//meshList[GEO_BENCH] = MeshBuilder::GenerateOBJMTL("model2", "Models//rusted_bench.obj", "Models//rusted_bench.mtl");
	//meshList[GEO_BENCH]->textureID = LoadTGA("Images//bench_colour.tga");


	//meshList[GEO_MODEL1] = MeshBuilder::GenerateOBJ("Doorman", "Models//doorman.obj"); 	// Step 7 - Load the OBJ file
	//meshList[GEO_MODEL1]->textureID = LoadTGA("Images//doorman.tga");


	//meshList[GEO_FRUIT] = MeshBuilder::GenerateOBJMTL("fruit", "Models//pumpkin_scan.obj", "Models//pumpkin_scan.mtl"); 	// Step 7 - Load the OBJ file
	//meshList[GEO_FRUIT]->textureID = LoadTGA("Images//pumpkin.tga");
	//// Step 5 - Load the models

	//meshList[GEO_MODEL_MTL1] =	MeshBuilder::GenerateOBJMTL("model2","Models//house_type01.obj",	"Models//house_type01.mtl");

	//meshList[GEO_TREE] = MeshBuilder::GenerateOBJ("model2", "Models//Skeletal_Tree_FREE.obj");
	//meshList[GEO_TREE]->textureID = LoadTGA("Images//Tree_Base_color.tga");

	//// With texture
	//meshList[GEO_MODEL_MTL2] =	MeshBuilder::GenerateOBJMTL("model3",	"Models//cottage_obj.obj",	"Models//cottage_obj.mtl");
	//meshList[GEO_MODEL_MTL2]->textureID = LoadTGA("Images//cottage_diffuse.tga");
	//// 16 x 16 is the number of columns and rows for the text
	//meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16,	16);
	//meshList[GEO_TEXT]->textureID = LoadTGA("Images//TNOFONT.tga");

	//meshList[GEO_ENEMY] = MeshBuilder::GenerateOBJMTL("model2", "Models//Ice_Scream_Man.obj", "Models//Ice_Scream_Man.mtl");
	//meshList[GEO_ENEMY]->textureID = LoadTGA("Images//Ice Scream Man.tga");

		glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);


	


	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	light[0].position = glm::vec3(0, 5, 0);
	light[0].color = glm::vec3(1, 1, 1);
	light[0].type = Light::LIGHT_POINT;
	light[0].power = 0.1f;
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

	
	enableLight = true;

	playcam.pos = camera.position;
	
	playcam.radius = 5;
	door.dimension = glm::vec3{ 4,4,4 };
	door.pos = glm::vec3{12,1,10};

	dartsdoor.dimension = glm::vec3{4,4,4};
	dartsdoor.pos = glm::vec3(12,1, -10);

	bankaball.dimension = glm::vec3(4, 4, 4);
	bankaball.pos = glm::vec3(-12, 1, 10);


	ringtossball.dimension = glm::vec3(4, 4, 4);
	ringtossball.pos = glm::vec3(-12, 1, -10);

	prizedoor.dimension = glm::vec3(4, 4, 4);
	prizedoor.pos = glm::vec3(0, 1, 0);

	 interactionTriggered = false;
}


void SceneMenu::Update(double dt)
{
	CollisionData cd;
	glm::vec3 doormax = glm::vec3{ door.pos.x + (door.dimension.x / 2),
							   door.pos.y + (door.dimension.y / 2),
							   door.pos.z + (door.dimension.z / 2) };

	glm::vec3 doormin = glm::vec3{ door.pos.x - (door.dimension.x / 2),
							   door.pos.y - (door.dimension.y / 2),
							   door.pos.z - (door.dimension.z / 2) };

	float temp = 1.f / dt;
	fps = glm::round(temp * 100.f) / 100.f;
	HandleKeyPress();

	if (KeyboardController::GetInstance()->IsKeyDown('I'))
		light[0].position.z -= static_cast<float>(dt) * 25.f;
	if (KeyboardController::GetInstance()->IsKeyDown('K'))
		light[0].position.z += static_cast<float>(dt) * 25.f;
	if (KeyboardController::GetInstance()->IsKeyDown('J'))
		light[0].position.x -= static_cast<float>(dt) * 25.f;
	if (KeyboardController::GetInstance()->IsKeyDown('L'))
		light[0].position.x += static_cast<float>(dt) * 25.f;
	if (KeyboardController::GetInstance()->IsKeyDown('O'))
		light[0].position.y -= static_cast<float>(dt) * 25.f;
	if (KeyboardController::GetInstance()->IsKeyDown('P'))
		light[0].position.y += static_cast<float>(dt) * 25.f;
	/*if (KeyboardController::GetInstance()->IsKeyPressed('5'))
	{
		camlock = !camlock;
	}

	if (camlock)
	{
		camera.Update(dt);
	}
	*/
	if (OverlapSphere2AABB(playcam, playcam.radius, door, doormin, doormax, cd)) {
		std::cout << "collision\n";
		ResolveCollision(cd);
		playcam.vel = glm::vec3(0);


	}
	camera.Update(dt);



}

void SceneMenu::Render()
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

	if (light[0].type == Light::LIGHT_DIRECTION)
	{
		glm::vec3 lightDir(light[0].position.x, light[0].position.y, light[0].position.z);
		glm::vec3 lightDirection_cameraspace = viewStack.Top() * glm::vec4(lightDir, 0);
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightDirection_cameraspace));
	}
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(light[0].position, 1);
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));
		glm::vec3 spotDirection_cameraspace = viewStack.Top() * glm::vec4(light[0].spotDirection, 0);
		glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, glm::value_ptr(spotDirection_cameraspace));
	}
	else {
		// Calculate the light position in camera space
		glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(light[0].position, 1);
		glUniform3fv(m_parameters[U_LIGHT0_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));
	}

	



	//ground
	modelStack.PushMatrix();
	
	modelStack.Translate(0,0,0);
	modelStack.Rotate(270, 1.f, 0.f, 0.f);
	RenderMesh(meshList[GEO_GROUND], false);
	modelStack.PopMatrix();

	//front
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 35, -50.f);
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();


	//back
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 35, 50.f);
	modelStack.Rotate(180, 0.f, 1.f, 0.f);
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();


	//roof
	modelStack.PushMatrix();
	modelStack.Translate(0.f, 75, 0.f);
	modelStack.Rotate(90, 1.f, 0.f, 0.f);
	
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();

	
	//left wall
	modelStack.PushMatrix();
	modelStack.Translate(-50.f, 35, 0.f);
	modelStack.Rotate(90, 0.f, 1.f, 0.f);
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();


	//right 
	modelStack.PushMatrix();
	modelStack.Translate(50.f, 35, 0.f);
	modelStack.Rotate(-90, 0.f, 1.f, 0.f);
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();

	// Step 8 - Render the OBJ file
	modelStack.PushMatrix();
	/* Apply scale, translate, rotate
	 Apply necessary materials*/
	  modelStack.Scale(door.dimension.x,door.dimension.y,door.dimension.z);
	  modelStack.Translate(door.pos.x, door.pos.y, door.pos.z);
	 /*Add the materials before rendering the object*/
	meshList[GEO_DOOR]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_DOOR]->material.kDiffuse = glm::vec3(0.5f, 0.0f, 0.0f);
	meshList[GEO_DOOR]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_DOOR]->material.kShininess = 5.0f;
	/*Test_Material(GEO_MODEL1, 0.1, 0.1, 0.1f, 0.5f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f, 1.f);*/
	RenderMesh(meshList[GEO_DOOR], enableLight);
	modelStack.PopMatrix();

	// Step 8 - Render the OBJ file
	modelStack.PushMatrix();
	/* Apply scale, translate, rotate
	 Apply necessary materials*/
	modelStack.Scale(dartsdoor.dimension.x, dartsdoor.dimension.y, dartsdoor.dimension.z);
	modelStack.Translate(dartsdoor.pos.x, dartsdoor.pos.y, dartsdoor.pos.z);
	/*Add the materials before rendering the object*/
	meshList[GEO_DOOR]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_DOOR]->material.kDiffuse = glm::vec3(0.5f, 0.0f, 0.0f);
	meshList[GEO_DOOR]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_DOOR]->material.kShininess = 5.0f;
	/*Test_Material(GEO_MODEL1, 0.1, 0.1, 0.1f, 0.5f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f, 1.f);*/
	RenderMesh(meshList[GEO_DOOR], enableLight);
	modelStack.PopMatrix();


	// Step 8 - Render the OBJ file
	modelStack.PushMatrix();
	/* Apply scale, translate, rotate
	 Apply necessary materials*/
	modelStack.Scale(bankaball.dimension.x, bankaball.dimension.y, bankaball.dimension.z);
	modelStack.Translate(bankaball.pos.x, bankaball.pos.y, bankaball.pos.z);
	/*Add the materials before rendering the object*/
	meshList[GEO_DOOR]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_DOOR]->material.kDiffuse = glm::vec3(0.5f, 0.0f, 0.0f);
	meshList[GEO_DOOR]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_DOOR]->material.kShininess = 5.0f;
	/*Test_Material(GEO_MODEL1, 0.1, 0.1, 0.1f, 0.5f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f, 1.f);*/
	RenderMesh(meshList[GEO_DOOR], enableLight);
	modelStack.PopMatrix();

	// Step 8 - Render the OBJ file
	modelStack.PushMatrix();
	/* Apply scale, translate, rotate
	 Apply necessary materials*/
	modelStack.Scale(ringtossball.dimension.x, ringtossball.dimension.y, ringtossball.dimension.z);
	modelStack.Translate(ringtossball.pos.x, ringtossball.pos.y, ringtossball.pos.z);
	/*Add the materials before rendering the object*/
	meshList[GEO_DOOR]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_DOOR]->material.kDiffuse = glm::vec3(0.5f, 0.0f, 0.0f);
	meshList[GEO_DOOR]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_DOOR]->material.kShininess = 5.0f;
	/*Test_Material(GEO_MODEL1, 0.1, 0.1, 0.1f, 0.5f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f, 1.f);*/
	RenderMesh(meshList[GEO_DOOR], enableLight);
	modelStack.PopMatrix();



	// Step 8 - Render the OBJ file
	modelStack.PushMatrix();
	/* Apply scale, translate, rotate
	 Apply necessary materials*/
	modelStack.Scale(prizedoor.dimension.x, prizedoor.dimension.y, prizedoor.dimension.z);
	modelStack.Translate(prizedoor.pos.x, prizedoor.pos.y, prizedoor.pos.z);
	/*Add the materials before rendering the object*/
	meshList[GEO_DOOR]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_DOOR]->material.kDiffuse = glm::vec3(0.5f, 0.0f, 0.0f);
	meshList[GEO_DOOR]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_DOOR]->material.kShininess = 5.0f;
	/*Test_Material(GEO_MODEL1, 0.1, 0.1, 0.1f, 0.5f, 0.5f, 0.5f, 0.9f, 0.9f, 0.9f, 1.f);*/
	RenderMesh(meshList[GEO_DOOR], enableLight);
	modelStack.PopMatrix();


	
	modelStack.PushMatrix();
	modelStack.Translate(0, 10,3);
	modelStack.Rotate(90, 0, 1, 0);
	RenderText(meshList[GEO_TEXT], "Prize Counter", glm::vec3(0, 1, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-45, 10, -35);
	modelStack.Rotate(90, 0, 1, 0);
	RenderText(meshList[GEO_TEXT], "Ring Toss Game", glm::vec3(0, 1, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(-45, 10, 40 );
	modelStack.Rotate(90, 0, 1, 0);
	RenderText(meshList[GEO_TEXT], "Bank A Ball Game", glm::vec3(0, 1, 0));
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	modelStack.Translate(45, 10, 35);
	modelStack.Rotate(270, 0, 1, 0);
	RenderText(meshList[GEO_TEXT], "Penalty Game", glm::vec3(0, 1, 0));
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Translate(45, 10, -35);
	modelStack.Rotate(270, 0, 1, 0);
	RenderText(meshList[GEO_TEXT], "Dart Game", glm::vec3(0, 1, 0));
	modelStack.PopMatrix();


	// No transform needed
	if (isLooking(camera, door.pos, 20)) /*&& glm::distance(camera.position, door.pos) < 5)*/
	{
		std::cout << "hello";
		RenderTextOnScreen(meshList[GEO_TEXT], "Press Q to enter PG: ", glm::vec3(0, 1, 0), 30, 15, 100);
		if (KeyboardController::GetInstance()->IsKeyDown('Q')) {
			SceneManager::GetInstance().ChangeState(new SceneModel);
		}
	}
	 else if (isLooking(camera, ringtossball.pos, 20)/* && glm::distance(camera.position, ringtossball.pos) < 5*/)
	{

		std::cout << "hello2";
		RenderTextOnScreen(meshList[GEO_TEXT], "Press Q to enter RT: ", glm::vec3(0, 1, 0), 30, 15, 100);
		if (KeyboardController::GetInstance()->IsKeyDown('Q')) {
			SceneManager::GetInstance().ChangeState(new SceneModel);
		}
	}
	else if ( isLooking(camera, prizedoor.pos, 20) /*&& glm::distance(camera.position, prizedoor.pos) < 5*/)
	{
		std::cout << "hello3";
		RenderTextOnScreen(meshList[GEO_TEXT], "Press Q to enter Prize Room: ", glm::vec3(0, 1, 0), 30, 15, 100);
		if (KeyboardController::GetInstance()->IsKeyDown('Q')) {
			SceneManager::GetInstance().ChangeState(new SceneModel);
		}
	}
	else if (isLooking(camera, bankaball.pos, 20)/* && glm::distance(camera.position, bankaball.pos) < 5*/)
	{
		std::cout << "hello4";
		RenderTextOnScreen(meshList[GEO_TEXT], "Press Q to enter BB: ", glm::vec3(0, 1, 0), 30, 15, 100);
		if (KeyboardController::GetInstance()->IsKeyDown('Q')) {
			SceneManager::GetInstance().ChangeState(new SceneModel);
		}
	}
	else if (isLooking(camera, dartsdoor.pos, 20)/* && glm::distance(camera.position, dartsdoor.pos) < 5*/)
	{
		std::cout << "hello5";
		RenderTextOnScreen(meshList[GEO_TEXT], "Press Q to enter DD: ", glm::vec3(0, 1, 0), 30, 15, 100);
		if (KeyboardController::GetInstance()->IsKeyDown('Q')) {
			SceneManager::GetInstance().ChangeState(new SceneModel);
		}
	}

}

void SceneMenu::RenderMesh(Mesh* mesh, bool enableLight)
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

	// Step 9 - Modify the code to include the logic below
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


void SceneMenu::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
{
	glDisable(GL_DEPTH_TEST);
	glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, -
		1000.f, 1000.f); // dimension of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	//// To do: Use modelStack to position GUI on screen
	modelStack.Translate(15, 20, 0);
	// To do: Use modelStack to scale the GUI
	modelStack.Scale(100, 100, 100);
	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneMenu::RenderStamina(Mesh* mesh, float x, float y, float sizex, float sizey)
{

	glDisable(GL_DEPTH_TEST);
	glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, -
		1000.f, 1000.f); // dimension of screen UI
	projectionStack.PushMatrix();
	projectionStack.LoadMatrix(ortho);
	viewStack.PushMatrix();
	viewStack.LoadIdentity(); //No need camera for ortho mode
	modelStack.PushMatrix();
	modelStack.LoadIdentity();
	//// To do: Use modelStack to position GUI on screen
	modelStack.Translate(15, 500, 0);
	// To do: Use modelStack to scale the GUI
	modelStack.Scale(5 * camera.CURRENT_STAMINA, 50, 50);
	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneMenu::HandleMouseInput()
{
	static bool isLeftUp = false;
	static bool isRightUp = false;
	// Process Left button
	if (!isLeftUp && MouseController::GetInstance() -> IsButtonDown(GLFW_MOUSE_BUTTON_LEFT))
	{
		isLeftUp = true;
		std::cout << "LBUTTON DOWN" << std::endl;
		// Step 3
       // transform into UI space
		double x = MouseController::GetInstance() -> GetMousePositionX();
        double y = 800 - MouseController::GetInstance() -> GetMousePositionY();
      // Check if mouse click position is within the GUI box
      // Change the boundaries as necessary
      if (x > 0 && x < 100 && y > 0 && y < 100) {
	     std::cout << "GUI IS CLICKED" << std::endl;
      } 
	}
	else if (isLeftUp && MouseController::GetInstance() -> IsButtonUp(GLFW_MOUSE_BUTTON_LEFT))
	{
		isLeftUp = false;
		std::cout << "LBUTTON UP" << std::endl;
	}
	//right
	if (!isRightUp && MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
	{
		isLeftUp = true;
		std::cout << "RBUTTON DOWN" << std::endl;
	}
	else if (isRightUp && MouseController::GetInstance()->IsButtonUp(GLFW_MOUSE_BUTTON_RIGHT))
	{
		isLeftUp = false;
		std::cout << "RBUTTON UP" << std::endl;
	}

}


void SceneMenu::Exit()
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



void SceneMenu::HandleKeyPress()
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

	if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_TAB))
	{
		if (light[0].type == Light::LIGHT_POINT) {
			light[0].type = Light::LIGHT_DIRECTION;
		}
		else if (light[0].type == Light::LIGHT_DIRECTION) {
			light[0].type = Light::LIGHT_SPOT;
		}
		else {
			light[0].type = Light::LIGHT_POINT;
		}

		glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	}

}

//
//void SceneModel::Test_Material(GEOMETRY_TYPE obj, float AmR, float AmG, float AmB, float DifA, float DifG, float DifB, float SpA, float SpG, float SpB, float Shiny)
//{
//	meshList[obj]->material.kAmbient = glm::vec3(AmR, AmG, AmB);
//	meshList[obj]->material.kDiffuse = glm::vec3(DifA, DifG, DifB);
//	meshList[obj]->material.kSpecular = glm::vec3(SpA, SpG, SpB);
//	meshList[obj]->material.kShininess = Shiny;
//
//}


void SceneMenu::RenderText(Mesh* mesh, std::string text, glm::vec3
	color)
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
		glm::mat4 MVP = projectionStack.Top() * viewStack.Top() *
			modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE,
			glm::value_ptr(MVP));
		mesh->Render((unsigned)text[i] * 6, 6);
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glUniform1i(m_parameters[U_TEXT_ENABLED], 0);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}
void SceneMenu::RenderTextOnScreen(Mesh* mesh, std::string
	text, glm::vec3 color, float size, float x, float y)
{
	if (!mesh || mesh->textureID <= 0) //Proper error check
		return;
	// Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);
	glm::mat4 ortho = glm::ortho(0.f, 800.f, 0.f, 600.f, -
		100.f, 100.f); // dimension of screen UI
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
		glm::mat4 MVP = projectionStack.Top() *
			viewStack.Top() * modelStack.Top() * characterSpacing;
		glUniformMatrix4fv(m_parameters[U_MVP], 1, GL_FALSE,
			glm::value_ptr(MVP));
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

bool SceneMenu::isLooking(const FPCamera& camera, const glm::vec3& buttonPos, float thresholdAngleDegrees)
{
	glm::vec3 forward = glm::normalize(camera.target - camera.position);
	glm::vec3 toButton = glm::normalize(buttonPos - camera.position);
	float dotProduct = glm::dot(forward, toButton);
	float threshold = glm::cos(glm::radians(thresholdAngleDegrees));
	return (dotProduct >= threshold);
}

