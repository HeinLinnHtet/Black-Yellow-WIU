#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Mesh.h"
#include "Vertex.h"
#include "LoadOBJ.h"

/******************************************************************************/
/*!
		Class MeshBuilder:
\brief	Provides methods to generate mesh of different shapes
*/
/******************************************************************************/
class MeshBuilder
{
public:
	static Mesh* GenerateAxes(const std::string &meshName, float lengthX, float lengthY, float lengthZ);

	static Mesh* GenerateSphere(const std::string& meshName,
		glm::vec3 color,
		float radius = 1.f,
		int numSlice = 360,
		int numStack = 360);

	static Mesh* GenerateRing(const std::string& meshName, glm::vec3 color, float innerR, float outerR, int numSlice, int numStack);

	static Mesh* GenerateCube(const std::string& meshName, 
		glm::vec3 color, 
		float length = 1.f);

	static Mesh* GenerateHemisphere(const std::string& meshName, 
		glm::vec3 color,
		float radius = 1.f,
		int numSlice = 360,
		int numStack = 360);

	static Mesh* GenerateCylinder(const std::string& meshName, glm::vec3 color, unsigned numSlice, float radius, float height);

	static Mesh* GenerateQuad(const std::string& meshName, glm::vec3 color, float length);

	static Mesh* GenerateOBJ(const std::string& meshName, const std::string& file_path);

	static Mesh* GenerateText(const std::string& meshName, unsigned numRow, unsigned numCol);

	static Mesh* GenerateOBJMTL(const std::string& meshName, const std::string& file_path, const std::string& mtl_path);

};


#endif