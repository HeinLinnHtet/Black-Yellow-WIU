#include "SceneBankABall.h"
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
#include "MouseController.h"
#include "LoadTGA.h"

#include <iostream>
SceneBankABall::SceneBankABall()
{
	power = 0;
	powerLevel = 0;
}

SceneBankABall::~SceneBankABall()
{
}

void SceneBankABall::Init()
{
	// Set background color to dark blue
	glClearColor(0.0f, 0.0f, 0.5f, 0.0f);

	//Enable depth buffer and depth testing
	glEnable(GL_DEPTH_TEST);

	//Enable back face culling
	//glEnable(GL_CULL_FACE);

	//Default to fill mode
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Generate a default VAO for now
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	// Load the shader programs
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

	m_parameters[U_COLOR_TEXTURE_ENABLED] = glGetUniformLocation(m_programID, "colorTextureEnabled");
	m_parameters[U_COLOR_TEXTURE] = glGetUniformLocation(m_programID, "colorTexture");
	m_parameters[U_LIGHTENABLED] = glGetUniformLocation(m_programID, "lightEnabled");
	m_parameters[U_NUMLIGHTS] = glGetUniformLocation(m_programID, "numLights");

	m_parameters[U_TEXT_ENABLED] = glGetUniformLocation(m_programID, "textEnabled");
	m_parameters[U_TEXT_COLOR] = glGetUniformLocation(m_programID, "textColor");


	// Initialise camera properties
	camera.Init(glm::vec3(0, 2, 9), glm::vec3(1, 2, 0), glm::vec3(0, 1, 0));

	// Init VBO here
	for (int i = 0; i < NUM_GEOMETRY; ++i)
	{
		meshList[i] = nullptr;
	}

	meshList[GEO_AXES] = MeshBuilder::GenerateAxes("Axes", 10000.f, 10000.f, 10000.f);
	meshList[GEO_PLANE] = MeshBuilder::GenerateQuad("Plane", glm::vec3(1.f, 1.f, 1.f), 10.f);
	//meshList[GEO_PLANE]->textureID = LoadTGA("Images//color.tga");
	meshList[GEO_LEFT] = MeshBuilder::GenerateQuad("Left", glm::vec3(1.f, 1.f, 1.f), 10.f);
	meshList[GEO_LEFT]->textureID = LoadTGA("Images//left.tga");
	meshList[GEO_FRONT] = MeshBuilder::GenerateQuad("Front", glm::vec3(1.f, 1.f, 1.f), 10.f);
	meshList[GEO_FRONT]->textureID = LoadTGA("Images//front.tga");
	meshList[GEO_RIGHT] = MeshBuilder::GenerateQuad("Right", glm::vec3(1.f, 1.f, 1.f), 10.f);
	meshList[GEO_RIGHT]->textureID = LoadTGA("Images//right.tga");
	meshList[GEO_BACK] = MeshBuilder::GenerateQuad("Back", glm::vec3(1.f, 1.f, 1.f), 10.f);
	meshList[GEO_BACK]->textureID = LoadTGA("Images//back.tga");
	meshList[GEO_TOP] = MeshBuilder::GenerateQuad("Top", glm::vec3(1.f, 1.f, 1.f), 10.f);
	meshList[GEO_TOP]->textureID = LoadTGA("Images//top.tga");
	meshList[GEO_BOTTOM] = MeshBuilder::GenerateQuad("Bottom", glm::vec3(1.f, 1.f, 1.f), 10.f);
	meshList[GEO_BOTTOM]->textureID = LoadTGA("Images//bottom.tga");
	meshList[GEO_TEXT] = MeshBuilder::GenerateText("text", 16, 16);
	meshList[GEO_TEXT]->textureID = LoadTGA("Images//calibri.tga");
	meshList[GEO_LIGHT] = MeshBuilder::GenerateSphere("DirectionalLight", glm::vec3(1, 1, 1), 1.f, 16, 16);

	meshList[GEO_STAMINABAR] = MeshBuilder::GenerateQuad("Stamina", glm::vec3(1.f, 1.f, 1.f), 1.f);
	meshList[GEO_POWERBAR] = MeshBuilder::GenerateQuad("Power", glm::vec3(1.f, 1.f, 1.f), 1.f);
	meshList[GEO_POWERBAR]->textureID = LoadTGA("Images//power.tga");

	meshList[GEO_FLASHLIGHTLIGHT] = MeshBuilder::GenerateSphere("SpotLight", glm::vec3(1, 1, 1), 1.f, 16, 16);

	meshList[GEO_BALL] = MeshBuilder::GenerateOBJMTL("toyball", "Models//worn_baseball_ball.obj", "Models//worn_baseball_ball.mtl");
	meshList[GEO_BALL]->textureID = LoadTGA("Images//lambert1.tga");
	meshList[GEO_BASKET] = MeshBuilder::GenerateOBJMTL("Basket", "Models//Basket.obj", "Models//Basket.mtl");
	meshList[GEO_BASKET]->textureID = LoadTGA("Images//BasketSingle001a.tga");
	meshList[GEO_TENT] = MeshBuilder::GenerateOBJMTL("Tent", "Models//tent.obj", "Models//tent.mtl");
	meshList[GEO_COUNTER] = MeshBuilder::GenerateOBJMTL("Counter", "Models//folding_table.obj", "Models//folding_table.mtl");
	meshList[GEO_COUNTER]->textureID = LoadTGA("Images//Folding_Table.tga");
	meshList[GEO_BANK] = MeshBuilder::GenerateQuad("Bank", glm::vec3(1.f, 1.f, 1.f), 3.5f);
	meshList[GEO_BANK]->textureID = LoadTGA("Images//BABeasy.tga");

	glm::mat4 projection = glm::perspective(45.0f, 4.0f / 3.0f, 0.1f, 1000.0f);
	projectionStack.LoadMatrix(projection);


	glUniform1i(m_parameters[U_NUMLIGHTS], 2);

	light[0].position = glm::vec3(0, 5, 0);
	light[0].color = glm::vec3(1, 1, 1);
	light[0].type = Light::LIGHT_DIRECTIONAL;
	light[0].power = 1;
	light[0].kC = 1.f;
	light[0].kL = 0.01f;
	light[0].kQ = 0.001f;
	light[0].cosCutoff = 45.f;
	light[0].cosInner = 30.f;
	light[0].exponent = 3.f;
	light[0].spotDirection = glm::vec3(0.f, 1.f, 0.f);

	light[1].position = camera.position;
	light[1].color = glm::vec3(1, 1, 1);
	light[1].type = Light::LIGHT_SPOT;
	light[1].power = 0;
	light[1].kC = 1.f;
	light[1].kL = 0.01f;
	light[1].kQ = 0.001f;
	light[1].cosCutoff = 10.f;
	light[1].cosInner = 10.f;
	light[1].exponent = 1.f;

	glm::vec3 flashlightView = glm::normalize(camera.position - camera.target);
	light[1].spotDirection = flashlightView;

	//float yawRad = glm::radians(glm::degrees(atan2(camera.camView.x, camera.camView.z))); // Convert yawAngle to radians if needed
	//light[1].spotDirection = glm::vec3(-sin(yawRad), camera.view.y, -cos(yawRad));
	//light[1].spotDirection = glm::normalize(light[1].spotDirection);

	glUniform3fv(m_parameters[U_LIGHT0_COLOR], 1, glm::value_ptr(light[0].color));
	glUniform1i(m_parameters[U_LIGHT0_TYPE], light[0].type);
	glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
	glUniform1f(m_parameters[U_LIGHT0_KC], light[0].kC);
	glUniform1f(m_parameters[U_LIGHT0_KL], light[0].kL);
	glUniform1f(m_parameters[U_LIGHT0_KQ], light[0].kQ);
	glUniform1f(m_parameters[U_LIGHT0_COSCUTOFF], cosf(glm::radians<float>(light[0].cosCutoff)));
	glUniform1f(m_parameters[U_LIGHT0_COSINNER], cosf(glm::radians<float>(light[0].cosInner)));
	glUniform1f(m_parameters[U_LIGHT0_EXPONENT], light[0].exponent);
	glUniform3fv(m_parameters[U_LIGHT0_SPOTDIRECTION], 1, &light[0].spotDirection[0]);

	glUniform3fv(m_parameters[U_LIGHT1_COLOR], 1, glm::value_ptr(light[1].color));
	glUniform1i(m_parameters[U_LIGHT1_TYPE], light[1].type);
	glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	glUniform1f(m_parameters[U_LIGHT1_KC], light[1].kC);
	glUniform1f(m_parameters[U_LIGHT1_KL], light[1].kL);
	glUniform1f(m_parameters[U_LIGHT1_KQ], light[1].kQ);
	glUniform1f(m_parameters[U_LIGHT1_COSCUTOFF], cosf(glm::radians<float>(light[1].cosCutoff)));
	glUniform1f(m_parameters[U_LIGHT1_COSINNER], cosf(glm::radians<float>(light[1].cosInner)));
	glUniform1f(m_parameters[U_LIGHT1_EXPONENT], light[1].exponent);
	glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, &light[1].spotDirection[1]);


	enableLight = true;

	power = 0;
	powerLevel = 0;
}

void SceneBankABall::Update(double dt)
{
	HandleKeyPress();
	HandleMouseInput();

	if (KeyboardController::GetInstance()->IsKeyDown('I'))
		light[0].position.z -= static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('K'))
		light[0].position.z += static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('J'))
		light[0].position.x -= static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('L'))
		light[0].position.x += static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('O'))
		light[0].position.y -= static_cast<float>(dt) * 5.f;
	if (KeyboardController::GetInstance()->IsKeyDown('P'))
		light[0].position.y += static_cast<float>(dt) * 5.f;


	double temp = 1.f / dt;
	fps = glm::round(temp * 100.f) / 100.f;

	//light[1].position = camera.position;
	//glm::vec3 flashlightView = glm::normalize(camera.position - camera.target);
	//light[1].spotDirection = flashlightView;

	if (!isGasFilled)
	{
		camera.Update(dt);
	}

	//std::cout << "X: " << camera.position.x << std::endl;
	//std::cout << "Z: " << camera.position.z << std::endl;

	//if (camera.position.x >= 20)
	//{
	//	camera.position.x = 19;
	//}
	//if (camera.position.x <= -20)
	//{
	//	camera.position.x = -19;
	//}
	//if (camera.position.z >= 20)
	//{
	//	camera.position.z = 19;
	//}
	//if (camera.position.z <= -20)
	//{
	//	camera.position.z = -19;
	//}


}

void SceneBankABall::Render()
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

	modelStack.PushMatrix();
	// Render objects
	RenderMesh(meshList[GEO_AXES], false);
	modelStack.PopMatrix();

	if (light[0].type == Light::LIGHT_DIRECTIONAL)
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

	if (light[1].type == Light::LIGHT_DIRECTIONAL)
	{
		glm::vec3 lightDir(light[1].position.x, light[1].position.y, light[1].position.z);
		glm::vec3 lightDirection_cameraspace = viewStack.Top() * glm::vec4(lightDir, 0);
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, glm::value_ptr(lightDirection_cameraspace));
	}
	else if (light[0].type == Light::LIGHT_SPOT)
	{
		glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(light[1].position, 1);
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));
		glm::vec3 spotDirection_cameraspace = viewStack.Top() * glm::vec4(light[1].spotDirection, 0);
		glUniform3fv(m_parameters[U_LIGHT1_SPOTDIRECTION], 1, glm::value_ptr(spotDirection_cameraspace));
	}
	else {
		// Calculate the light position in camera space
		glm::vec3 lightPosition_cameraspace = viewStack.Top() * glm::vec4(light[1].position, 1);
		glUniform3fv(m_parameters[U_LIGHT1_POSITION], 1, glm::value_ptr(lightPosition_cameraspace));
	}

	//RenderSkybox();
	std::string temp(std::to_string(power));



	modelStack.PushMatrix();
	modelStack.Scale(.25f, .25f, .25f);
	modelStack.Rotate(0, 0, 0, 1);
	modelStack.Translate(0.f, 0.f, 0.f);
	meshList[GEO_BALL]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BALL]->material.kDiffuse = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_BALL]->material.kSpecular = glm::vec3(0.2f, 0.2f, 0.2f);
	meshList[GEO_BALL]->material.kShininess = 5.0f;

	//	 Render objects using new RenderMesh()
	RenderMesh(meshList[GEO_BALL], enableLight);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Scale(2.5f, 1.5f, 2.f);
	modelStack.Rotate(0, 0, 0, 1);
	modelStack.Translate(0.f, -.5f, -4.5f);
	meshList[GEO_BASKET]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_BASKET]->material.kDiffuse = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_BASKET]->material.kSpecular = glm::vec3(0.2f, 0.2f, 0.2f);
	meshList[GEO_BASKET]->material.kShininess = 5.0f;

	//	 Render objects using new RenderMesh()
	RenderMesh(meshList[GEO_BASKET], enableLight);
	modelStack.PopMatrix();
	

	modelStack.PushMatrix();
	modelStack.Scale(10.f, 2.5f, 3.f);
	modelStack.Rotate(0, 0, 0, 1);
	modelStack.Translate(0.f, 0.f, 0.f);
	meshList[GEO_COUNTER]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_COUNTER]->material.kDiffuse = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_COUNTER]->material.kSpecular = glm::vec3(0.2f, 0.2f, 0.2f);
	meshList[GEO_COUNTER]->material.kShininess = 5.0f;

	//	 Render objects using new RenderMesh()
	RenderMesh(meshList[GEO_COUNTER], enableLight);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Scale(1.5f, 1.5f, 1.5f);
	modelStack.Rotate(-20, 1, 0, 0);
	modelStack.Translate(0.f, 5.f , -5.5f);
	meshList[GEO_BANK]->material.kAmbient = glm::vec3(0.6f, 0.6f, 0.6f);
	meshList[GEO_BANK]->material.kDiffuse = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_BANK]->material.kSpecular = glm::vec3(0.2f, 0.2f, 0.2f);
	meshList[GEO_BANK]->material.kShininess = 3.0f;

	//	 Render objects using new RenderMesh()
	RenderMesh(meshList[GEO_BANK], enableLight);
	modelStack.PopMatrix();


	modelStack.PushMatrix();
	modelStack.Scale(0.15f, 0.2f, 0.15f);
	modelStack.Rotate(0, 0, 0, 1);
	modelStack.Translate(0.f, -5.f, -1.0f);
	meshList[GEO_TENT]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_TENT]->material.kDiffuse = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_TENT]->material.kSpecular = glm::vec3(0.2f, 0.2f, 0.2f);
	meshList[GEO_TENT]->material.kShininess = 5.0f;

	//	 Render objects using new RenderMesh()
	RenderMesh(meshList[GEO_TENT], enableLight);
	modelStack.PopMatrix();


	//	 Render objects using new RenderMesh()
	modelStack.PushMatrix();
	RenderMeshOnScreen(meshList[GEO_POWERBAR], 775, 0, 50, power * 3.5);
	modelStack.PopMatrix();
	RenderTextOnScreen(meshList[GEO_TEXT], temp, glm::vec3(1, 0, 0), 20, 770, power * 1.75);

	// Light
	modelStack.PushMatrix();
	modelStack.Scale(0.5f, 0.5f, 0.5f);
	modelStack.Rotate(0, 0, 0, 1);
	modelStack.Translate(light[0].position.x, 50, light[0].position.z);
	meshList[GEO_LIGHT]->material.kAmbient = glm::vec3(0.1f, 0.1f, 0.1f);
	meshList[GEO_LIGHT]->material.kDiffuse = glm::vec3(0.9f, 0.9f, 0.9f);
	meshList[GEO_LIGHT]->material.kSpecular = glm::vec3(0.2f, 0.2f, 0.2f);
	meshList[GEO_LIGHT]->material.kShininess = 5.0f;

	//	 Render objects using new RenderMesh()
	RenderMesh(meshList[GEO_LIGHT], enableLight);
	modelStack.PopMatrix();






	// Out of bounds dialogue

	//if (camera.position.x >= 18 || camera.position.x <= -18 || camera.position.z >= 18 || camera.position.z <= -19)
	//{
	//	RenderTextOnScreen(meshList[GEO_TEXT], "I shouldn't travel far from my car..", glm::vec3(1, 0, 0), 40, 20, 50);
	//}


}

void SceneBankABall::RenderMesh(Mesh* mesh, bool enableLight)
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

void SceneBankABall::RenderSkybox()
{
	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(0.f, 0.f, -50.f);
	modelStack.Scale(10, 10, 10);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_FRONT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(0.f, 0.f, 50.f);
	modelStack.Rotate(180, 0, 1, 0);
	modelStack.Scale(10, 10, 10);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_BACK], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Y direction by -50 units
	modelStack.Translate(0.f, 50.f, 0.f);
	modelStack.Rotate(90, 1, 0, 0);
	modelStack.Rotate(90, 0, 0, 1);
	modelStack.Scale(10, 10, 10);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_TOP], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Y direction by 0 units
	modelStack.Translate(0.f, -50.f, 0.f);
	modelStack.Rotate(-90, 1, 0, 0);
	modelStack.Scale(10, 10, 10);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_BOTTOM], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(-50.f, 0.f, 0.f);
	modelStack.Rotate(90, 0, 1, 0);
	modelStack.Scale(10, 10, 10);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_LEFT], false);
	modelStack.PopMatrix();

	modelStack.PushMatrix();
	// Offset in Z direction by -50 units
	modelStack.Translate(50.f, 0.f, 0.f);
	modelStack.Rotate(-90, 0, 1, 0);
	modelStack.Scale(10, 10, 10);
	// Skybox should be rendered without light
	RenderMesh(meshList[GEO_RIGHT], false);
	modelStack.PopMatrix();
}

void SceneBankABall::RenderMeshOnScreen(Mesh* mesh, float x, float y, float sizex, float sizey)
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
	// To do: Use modelStack to position GUI on screen
	modelStack.Translate(x, y, 0);
	modelStack.Scale(sizex, sizey, 1);
	// To do: Use modelStack to scale the GUI
	RenderMesh(mesh, false); //UI should not have light
	projectionStack.PopMatrix();
	viewStack.PopMatrix();
	modelStack.PopMatrix();
	glEnable(GL_DEPTH_TEST);
}

void SceneBankABall::HandleMouseInput()
{
	static bool isLeftUp = false;
	static bool isRightUp = false;
	// Process Left button
	//if (!isLeftUp && MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_LEFT))
	//{
	//	isLeftUp = true;
	//	std::cout << "LBUTTON DOWN" << std::endl;

	//	double x = MouseController::GetInstance()->GetMousePositionX();
	//	double y = 600 - MouseController::GetInstance()->GetMousePositionY();
	//	// Check if mouse click position is within the GUI box
	//	// Change the boundaries as necessary
	//	if (x > 0 && x < 100 && y > 0 && y < 100) {
	//		std::cout << "GUI IS CLICKED" << std::endl;
	//	}
	//}
	//else if (isLeftUp && MouseController::GetInstance()->IsButtonUp(GLFW_MOUSE_BUTTON_LEFT))
	//{
	//	isLeftUp = false;
	//	std::cout << "LBUTTON UP" << std::endl;
	//}
	//if (!isRightUp && MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_RIGHT))
	//{
	//	isRightUp = true;
	//	std::cout << "RBUTTON DOWN" << std::endl;
	//}
	//else if (isRightUp && MouseController::GetInstance()->IsButtonUp(GLFW_MOUSE_BUTTON_RIGHT))
	//{
	//	isRightUp = false;
	//	std::cout << "RBUTTON UP" << std::endl;
	//}

	static bool increasing = true;
	static bool isLMBup = false;
	
	if (MouseController::GetInstance()->IsButtonDown(GLFW_MOUSE_BUTTON_LEFT)) {
		if (!isLMBup) {
			isLMBup = true;
			powerLevel = 0.0;  // Reset power level
			increasing = true;  // Start increasing power
			std::cout << "LMB DOWN - Start Power Increase" << std::endl;
		}

		// Update power level
		if (increasing) {
			powerLevel += 1.0;
			if (powerLevel >= MAX_POWER) {
				increasing = false;  // Switch to decreasing power once the max is reached
			}
		}
		else {
			powerLevel -= 1.0;
			if (power <= 0.0) {
				increasing = true;  // Switch to increasing power once the minimum is reached
			}
		}
		std::cout << "Current Power Level: " << powerLevel << std::endl;

	}
	else if (isLMBup) {
		std::cout << "LMB UP - Final Power Level: " << powerLevel << std::endl;

		std::cout << "Applying Power: " << powerLevel << std::endl;
		// Reset everything after releasing the button
		power = powerLevel;
		powerLevel = 0.0;
		isLMBup = false;
	}
}

void SceneBankABall::RenderText(Mesh* mesh, std::string text, glm::vec3 color)
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

void SceneBankABall::RenderTextOnScreen(Mesh* mesh, std::string text, glm::vec3 color, float size, float x, float y)
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
			glm::vec3(0.5f + i * 0.5f, 0.5f, 0)
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


void SceneBankABall::Exit()
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

void SceneBankABall::HandleKeyPress()
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

		//if (light[0].power <= 0.1f)
		//	light[0].power = 1.0f;
		//else
		//	light[0].power = 0.1f;

		light[0].power = 1.f;
		light[1].power = 1.f;
		glUniform1f(m_parameters[U_LIGHT0_POWER], light[0].power);
		glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
	}

	if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_TAB))
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
	}
	if (isFlashPicked && !isGasFilled)
	{
		if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_F)) // If it's picked up, can toggle flashlight
		{
			if (light[1].power <= 0.f)
				light[1].power = 0.55f;
			else
				light[1].power = 0.f;
			glUniform1f(m_parameters[U_LIGHT1_POWER], light[1].power);
		}
	}
	else if (!isFlashPicked) // Otherwise default = off.
	{
		light[1].power = 0.f;
	}
	if (isLookingFlash) {
		if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_E))
		{
			// Trigger the flashlight
			isFlashPicked = true;
			isLookingFlash = false;
		}
	}
	if (isLookingFill) {
		if (KeyboardController::GetInstance()->IsKeyPressed(GLFW_KEY_E))
		{
			// Trigger the flashlight
			isGasFilled = true;
			isLookingFill = false;
		}
	}




	//if (KeyboardController::GetInstance()->IsKeyDown(GLFW_KEY_LEFT_SHIFT) && canSprint) {
	//	if (stamina > 0) {
	//		camera.ZOOM_SPEED = 5.f;
	//		stamina--;  // Decrease stamina while sprinting
	//		canSprint = true;
	//	}
	//	else {
	//		camera.ZOOM_SPEED = 2.f;
	//		canSprint = false;  // Prevent sprinting if stamina is zero
	//	}
	//}
	//else {
	//	camera.ZOOM_SPEED = 2.f;  // Reset to normal speed when not sprinting
	//	if (stamina < MAX_STAMINA && KeyboardController::GetInstance()->IsKeyUp(GLFW_KEY_LEFT_SHIFT)) {
	//		stamina++;  // Replenish stamina when not sprinting
	//	}
	//}

	//if (stamina == 0) {
	//	canSprint = false;
	//}
	//else {
	//	canSprint = true;
	//}
}
