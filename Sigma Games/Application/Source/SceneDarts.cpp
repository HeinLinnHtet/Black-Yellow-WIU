#include "GL\glew.h"
#include "SceneDarts.h"

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
#include <mmsystem.h>

#pragma comment(lib,"winmm.lib")

void BalloonRack::addtorack(Balloon addingballoon, int numballoon)
{
	for (int i = 0; i < (numballoon / 6); i++)
	{
		for (int j = 0; j < 6; j++)
		{
			balloonstack[ballooncount] = addingballoon;
			balloonstack[ballooncount].pos.y += addingballoon.pos.y + (i * addingballoon.dimensions.y);
			balloonstack[ballooncount].points += addingballoon.points * i;
			balloonstack[ballooncount].pos.z += addingballoon.pos.z + (j * addingballoon.dimensions.z);
			ballooncount++;
		}
	}

}

SceneDarts::SceneDarts()
{
}

SceneDarts::~SceneDarts()
{
}

void SceneDarts::Init()
{

	player.pos = camera.position;

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
	player.pos = camera.position;

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

	meshList[GEO_DIFFICULTY] = MeshBuilder::GenerateQuad("Diffy", glm::vec3(1, 1, 1), 100.f);

	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Pause", glm::vec3(1, 1, 1), 100.f);
	meshList[GEO_PLANE]->textureID = LoadTGA("Images//pausescreen.tga");

	meshList[GEO_WIN] = MeshBuilder::GenerateQuad("Win", glm::vec3(1, 1, 1), 100.f);
	meshList[GEO_WIN]->textureID = LoadTGA("Images//winscreen.tga");

	// 16 x 16 is the number of columns and rows for the text
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");

	meshList[GEO_SHELF] = MeshBuilder::GenerateOBJMTL("Shelf", "Models//balloonshelf.obj", "Models//balloonshelf.mtl");
	meshList[GEO_SHELF]->textureID = LoadTGA("Images//woodtex2.tga");

	meshList[GEO_STALL] = MeshBuilder::GenerateOBJMTL("Stall", "Models//stall.obj", "Models//stall.mtl");
	meshList[GEO_STALL]->textureID = LoadTGA("Images//woodtex2.tga");

	meshList[GEO_BALLOON] = MeshBuilder::GenerateOBJMTL("Balloon", "Models//balloon.obj", "Models//balloon.mtl");
	meshList[GEO_BALLOON]->textureID = LoadTGA("Images//gltf_embedded_0.tga");

	meshList[GEO_DART] = MeshBuilder::GenerateOBJMTL("Dart", "Models//dartnew.obj", "Models//dartnew.mtl");
	meshList[GEO_DART]->textureID = LoadTGA("Images//throwing_dart_diffuse.tga");

	glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);

	//Balloon Object Creation
	Balloon balloon(-5, -1, -2.25, 100);
	numofballoons = 24;
	rack.addtorack(balloon, numofballoons);

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

	light[1].position = glm::vec3(0, 10, 0);
	light[1].color = glm::vec3(0, 0, 0);
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

	light[2].position = glm::vec3(0, 20, 0);
	light[2].color = glm::vec3(1, 1, 1);
	light[2].type = Light::LIGHT_DIRECTIONAL;
	light[2].power = 1000;
	light[2].kC = 1.f;
	light[2].kL = 1.01f;
	light[2].kQ = 1.001f;
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
}

void SceneDarts::Update(double dt)
{
	//std::cout << camera.position.x << ", " << camera.position.y << ", " << camera.position.z << std::endl;

	HandleKeyPress();
	
	/*if (camera.position.x > 24.45f) {
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
	}*/
	//std::cout << "Collided!" << std::endl;
	
	//blue rat go fast

	if (!gamepaused)
	{
		if (!difficultychosen)
		{
			if (KeyboardController::GetInstance()->IsKeyPressed('1'))
			{
				difficulty = 1;
				difficultychosen = true;
			}
			else if (KeyboardController::GetInstance()->IsKeyPressed('2'))
			{
				difficulty = 1.5;
				difficultychosen = true;
			}
			else if (KeyboardController::GetInstance()->IsKeyPressed('3'))
			{
				difficulty = 2.5;
				difficultychosen = true;
			}
		}

		glm::vec3 oldpos = dart.pos;
		static float gravity = -10;

		meshList[GEO_BAR] = MeshBuilder::GenerateQuad("Bar", glm::vec3(1 - (chargeup / 5), chargeup / 5, 0), 100.f);

		charging = false;
		camera.position = playerpos;

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

		//euler integration
		glm::vec3 initialVel = dart.vel;
		dart.AddForce(glm::vec3(0, -1, 0) * gravity);
		dart.vel.y += gravity * dt;
		dart.pos += (dart.vel + initialVel) * 0.5f;

		if (dartcount != -1)
		{
			if (dart.pos.y <= 4.5 && dart.shooting == false)
			{
				dart.vel.y = -dart.vel.y * 0.0001;
				dart.pos = oldpos;
			}

			if (dartcount != 0)
			{
				if (MouseController::GetInstance()->IsButtonDown(0))
				{
					chargeup += (difficulty * difficulty) * 0.05;
					charging = true;
				}
				else
				{
					if (chargeup >= 0)
					{
						chargeup -= 0.1;
					}
				}

				if (charging)
				{

					if (chargeup >= 5)
					{
						chargeup = 0;
					}

					if (KeyboardController::GetInstance()->IsKeyPressed('E') && dart.shooting == false)
					{
						glm::vec3 direction = glm::normalize(camera.target - camera.position);

						// Apply the calculated impulse to the ball
						dart.AddImpulse(direction * glm::vec3(chargeup * 0.6));
						dart.shooting = true;
						PlaySound(TEXT("dartfly.wav"), NULL, SND_FILENAME | SND_ASYNC);
						chargeup = 0;
						dartcount--;
					}
				}
			}

			if (dart.shooting)
			{
				if (dartcount != 0)
				{
					if (dart.pos.x <= -10 || dart.pos.y <= -2.5)
					{
						dart.pos = { 12, 7, 0 };
						dart.vel = { 0,0,0 };
						dart.hit = false;
						dart.shooting = false;
					}
				}
			}

			for (int i = 0; i < rack.ballooncount; i++)
			{
				CollisionData cd;
				if (OverlapAABB2AABB(dart, dart.dimensions.x, dart.dimensions.y, dart.dimensions.z, rack.balloonstack[i], rack.balloonstack[i].dimensions.x, rack.balloonstack[i].dimensions.y, rack.balloonstack[i].dimensions.z, cd) && dart.hit == false && rack.balloonstack[i].popped == false)
				{
					rack.balloonstack[i].popped = true;
					ticketcount += rack.balloonstack[i].points;
					PlaySound(TEXT("balloonpop.wav"), NULL, SND_FILENAME | SND_ASYNC);
					dart.hit = true;
				}
			}
		}


		//std::cout << dart.pos.x << ", " << dart.pos.y << ", " << dart.pos.z << std::endl;

		camera.Update(dt);
	}

}

void SceneDarts::Render()
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

	/*modelStack.PushMatrix();
	meshList[GEO_CUBE]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_CUBE]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_CUBE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_CUBE]->material.kShininess = 5.0f;
	modelStack.Translate(dart.pos.x,
		dart.pos.y,
		dart.pos.z);
	modelStack.Scale(dart.dimensions.x,
		dart.dimensions.y,
		dart.dimensions.z);
	RenderMesh(meshList[GEO_CUBE], true);
	modelStack.PopMatrix();*/

	modelStack.PushMatrix();
	meshList[GEO_DART]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_DART]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_DART]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_DART]->material.kShininess = 5.0f;
	modelStack.Translate(dart.pos.x - 1, dart.pos.y, dart.pos.z);
	modelStack.Rotate(90, 0, 1, 0);

	modelStack.Scale(3.0f, 3.0f, 3.0f);
	RenderMesh(meshList[GEO_DART], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	meshList[GEO_STALL]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_STALL]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_STALL]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_STALL]->material.kShininess = 5.0f;
	modelStack.Translate(7,-2.5,0);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(3,2.8,3);
	RenderMesh(meshList[GEO_STALL], true);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	meshList[GEO_SHELF]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_SHELF]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_SHELF]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_SHELF]->material.kShininess = 5.0f;
	modelStack.Translate(shelf.pos.x, shelf.pos.y, shelf.pos.z);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(3.0f, 3.4f, 2.8f);
	RenderMesh(meshList[GEO_SHELF], true);
	modelStack.PopMatrix();

	for (int i = 0; i < rack.ballooncount; i++)
	{

		if (rack.balloonstack[i].popped == false)
		{

			/*modelStack.PushMatrix();
			meshList[GEO_CUBE]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_CUBE]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_CUBE]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			meshList[GEO_CUBE]->material.kShininess = 5.0f;
			modelStack.Translate(rack.balloonstack[i].pos.x + 2.5,
								 rack.balloonstack[i].pos.y + 4.3,
								 rack.balloonstack[i].pos.z - 0.3);
			modelStack.Scale(rack.balloonstack[i].dimensions.x,
							 rack.balloonstack[i].dimensions.y,
							 rack.balloonstack[i].dimensions.z);
			RenderMesh(meshList[GEO_CUBE], true);
			modelStack.PopMatrix();*/

			
			modelStack.PushMatrix();
			meshList[GEO_BALLOON]->material.kAmbient = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_BALLOON]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
			meshList[GEO_BALLOON]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
			meshList[GEO_BALLOON]->material.kShininess = 5.0f;
			modelStack.Translate(rack.balloonstack[i].pos.x,
								 rack.balloonstack[i].pos.y,
								 rack.balloonstack[i].pos.z);
			modelStack.Rotate(90, 0, 1, 0);
			modelStack.Rotate(30, 1, 0, 0);
			modelStack.Scale(12.0f, 12.0f, 12.0f);
			RenderMesh(meshList[GEO_BALLOON], true);
			modelStack.PopMatrix();
		}

	}

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	meshList[GEO_FRONT]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_FRONT]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	meshList[GEO_FRONT]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_FRONT]->material.kShininess = 5.0f;

	modelStack.Translate(0.f, 0.f, -8.2375f);
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
	modelStack.Translate(0.f, 0.f, -10.0f);
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
	//// Offset in Z direction by -50 units
	//meshList[GEO_TOP]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	//meshList[GEO_TOP]->material.kDiffuse = glm::vec3(1.0f, 1.0f, 1.0f);
	//meshList[GEO_TOP]->material.kSpecular = glm::vec3(0.9f, 0.9f, 0.9f);
	//meshList[GEO_TOP]->material.kShininess = 5.0f;

	//modelStack.Rotate(90, 1, 0, 0);
	//modelStack.Translate(0.f, 0.f, -7.475f);
	//modelStack.Scale(0.5f, 0.5f, 0.5f);
	//modelStack.Rotate(90, 0, 0, 1);
	//// Skybox should be rendered without light

	//RenderMesh(meshList[GEO_TOP], true);
	//modelStack.PopMatrix();

	modelStack.PushMatrix();
	RenderMeshOnScreen(meshList[GEO_BAR], 30, 300, 0.25, chargeup);
	modelStack.PopMatrix();

	std::ostringstream count;
	count << "Number of tickets: " << ticketcount;
	std::string tktct = count.str();

	if (dartcount > -1)
	{

		modelStack.PushMatrix();
		//scale, translate, rotate
		RenderTextOnScreen(meshList[GEO_TEXT], tktct, glm::vec3(0.4, 1, 0.5), 20, 120, 500);
		modelStack.PopMatrix();

		std::ostringstream dartc;
		dartc << "Darts Remaining: " << dartcount;
		std::string dtct = dartc.str();

		modelStack.PushMatrix();
		//scale, translate, rotate
		RenderTextOnScreen(meshList[GEO_TEXT], dtct, glm::vec3(1, 1, 1), 20, 120, 30);
		modelStack.PopMatrix();

	}

	if (dartcount <= 0)
	{
		modelStack.PushMatrix();
		RenderMeshOnScreen(meshList[GEO_WIN], 400, 300, 8, 6);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		//scale, translate, rotate
		RenderTextOnScreen(meshList[GEO_TEXT], tktct, glm::vec3(0, 0, 0), 15, 250, 240);
		modelStack.PopMatrix();
	}

	if (gamepaused)
	{
		modelStack.PushMatrix();
		RenderMeshOnScreen(meshList[GEO_PLANE], 400, 280, 12, 6.75);
		modelStack.PopMatrix();
	}

	if (!difficultychosen)
	{
		modelStack.PushMatrix();
		RenderMeshOnScreen(meshList[GEO_DIFFICULTY], 400, 280, 12, 6.75);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		//scale, translate, rotate
		RenderTextOnScreen(meshList[GEO_TEXT], "Select your difficulty", glm::vec3(0, 0, 0), 30, 90, 500);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		//scale, translate, rotate
		RenderTextOnScreen(meshList[GEO_TEXT], "1 : EASY", glm::vec3(0, 0, 0), 30, 120, 440);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		//scale, translate, rotate
		RenderTextOnScreen(meshList[GEO_TEXT], "2 : NORMAL", glm::vec3(0, 0, 0), 30, 120, 380);
		modelStack.PopMatrix();

		modelStack.PushMatrix();
		//scale, translate, rotate
		RenderTextOnScreen(meshList[GEO_TEXT], "3 : DIFFICULT", glm::vec3(0, 0, 0), 30, 120, 320);
		modelStack.PopMatrix();
	}
}

void SceneDarts::RenderMesh(Mesh* mesh, bool enableLight)
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

void SceneDarts::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
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

void SceneDarts::RenderText(Mesh* mesh, std::string text, glm::vec3 color)
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

void SceneDarts::RenderTextOnScreen(Mesh* mesh, std::string text, glm::vec3 color, float size, float x, float y)
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

void SceneDarts::RenderSkybox()
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

void SceneDarts::HandleMouseInput()
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


void SceneDarts::Exit()
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

void SceneDarts::HandleKeyPress()
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

	if (KeyboardController::GetInstance()->IsKeyPressed('P'))
	{
		if (gamepaused)
		{
			gamepaused = false;
		}
		else
		{
			gamepaused = true;
		}
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

shelf::shelf()
{
	pos = { -5, -2.5, 0 };
	mass = 0;
	bounciness = 0;
}

shelf::~shelf()
{
}

dart::dart()
{
	pos = { 12, 7, 0 };
	mass = 0.5;
	bounciness = 0;
}

dart::~dart()
{
}
