/*
Semestre 2021-1
Práctica 11: Animación Sencilla y animación compleja
Texturizado: se agrega código para transparencia y blending en el shader
Skybox: Se agrega Skybox como textura ligada a la cámara. 
*/
//para cargar imagen
#define STB_IMAGE_IMPLEMENTATION

#include <stdio.h>
#include <string.h>
#include <cmath>
#include <vector>


#include <glew.h>
#include <glfw3.h>

#include <glm.hpp>
#include <gtc\matrix_transform.hpp>
#include <gtc\type_ptr.hpp>
//para probar el importer
//#include<assimp/Importer.hpp>

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
//para iluminación
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

#include"Model.h"
#include "Skybox.h"
#include"SpotLight.h"
#include "Sphere.h"
#include "cilindro.h"
#include "Cono.h"

const float toRadians = 3.14159265f / 180.0f;
float movCoche;
float movOffset;
float movMuslo;
float movPie;
float dedos;
bool avanza;
Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture dadoTexture;
Texture pisoTexture;
Texture Tagave;
Texture caja;
Texture trono;
Texture trono1;
Texture tronoAs;
Texture tronoDer;
Texture tronoIzq;
Texture metal;
Texture plata;
Texture plano;
Texture plano1;
Texture pieljulien;
Texture pie;
Texture luz;
Texture senal;
Texture marmol;
Texture cuidado;
Texture lapida;
Texture cruz;
Texture elefan;
Texture hierro;
Texture madera;
Texture cemento;
Texture banos;
Texture banos1;
Texture restaurante;
Texture botiquin;
Texture mascarilla;
Texture telefono;
Texture flecha;

//materiales
Material Material_brillante;
Material Material_opaco;
//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];
SpotLight spotLights[MAX_SPOT_LIGHTS];

//Model Kitt_M;
//Model Llanta_M;
//Model Camino_M;
//Model Blackhawk_M;
Model spring;
Model trunk;
Model trampoline;
Model tunel;

Skybox skybox;

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

//datos de esfera
Sphere sp = Sphere(1.0, 20, 20);
Cilindro cilindro(1.0);
Cono cono(1.0);

// Vertex Shader
static const char* vShader = "shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "shaders/shader_light.frag";
//cálculo del promedio de las normales para sombreado de Phong
void calcAverageNormals(unsigned int * indices, unsigned int indiceCount, GLfloat * vertices, unsigned int verticeCount, 
						unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);
		
		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}

void CreateObjects() 
{
	unsigned int indices[] = {		
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	GLfloat vertices[] = {
	//	x      y      z			u	  v			nx	  ny    nz
		-1.0f, -1.0f, -0.6f,	0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 1.0f,		0.5f, 0.0f,		0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, -0.6f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f,		0.5f, 1.0f,		0.0f, 0.0f, 0.0f
	};

	unsigned int floorIndices[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat floorVertices[] = {
		-10.0f, 0.0f, -10.0f,	0.0f, 0.0f,		0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, -10.0f,	10.0f, 0.0f,	0.0f, -1.0f, 0.0f,
		-10.0f, 0.0f, 10.0f,	0.0f, 10.0f,	0.0f, -1.0f, 0.0f,
		10.0f, 0.0f, 10.0f,		10.0f, 10.0f,	0.0f, -1.0f, 0.0f
	};

	unsigned int vegetacionIndices[] = {
		0, 1, 2,
		0, 2, 3,
		4,5,6,
		4,6,7
	};

	GLfloat vegetacionVertices[] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.5f, 0.5f, 0.0f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,

		0.0f, -0.5f, -0.5f,		0.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.5f,		1.0f, 0.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.5f,		1.0f, 1.0f,		0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, -0.5f,		0.0f, 1.0f,		0.0f, 0.0f, 0.0f,


	};


	/*calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);*/

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);

	/*calcAverageNormals(vegetacionIndices, 12, vegetacionVertices, 64, 8, 5);

	Mesh *obj4 = new Mesh();
	obj4->CreateMesh(vegetacionVertices, vegetacionIndices, 64, 12);
	meshList.push_back(obj4);*/

}

//void CrearCubo()
//{
//	unsigned int cubo_indices[] = {
//		// front
//		0, 1, 2,
//		2, 3, 0,
//		// right
//		4, 5, 6,
//		6, 7, 4,
//		// back
//		8, 9, 10,
//		10, 11, 8,
//
//		// left
//		12, 13, 14,
//		14, 15, 12,
//		// bottom
//		16, 17, 18,
//		18, 19, 16,
//		// top
//		20, 21, 22,
//		22, 23, 20,
//	};
//	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales
//// average normals
//	GLfloat cubo_vertices[] = {
//		// front
//		//x		y		z		S		T			NX		NY		NZ
//		-0.5f, -0.5f,  0.5f,	0.27f,  0.35f,		0.0f,	0.0f,	-1.0f,	//0
//		0.5f, -0.5f,  0.5f,		0.48f,	0.35f,		0.0f,	0.0f,	-1.0f,	//1
//		0.5f,  0.5f,  0.5f,		0.48f,	0.64f,		0.0f,	0.0f,	-1.0f,	//2
//		-0.5f,  0.5f,  0.5f,	0.27f,	0.64f,		0.0f,	0.0f,	-1.0f,	//3
//		// right
//		//x		y		z		S		T
//		0.5f, -0.5f,  0.5f,	    0.52f,  0.35f,		-1.0f,	0.0f,	0.0f,
//		0.5f, -0.5f,  -0.5f,	0.73f,	0.35f,		-1.0f,	0.0f,	0.0f,
//		0.5f,  0.5f,  -0.5f,	0.73f,	0.64f,		-1.0f,	0.0f,	0.0f,
//		0.5f,  0.5f,  0.5f,	    0.52f,	0.64f,		-1.0f,	0.0f,	0.0f,
//		// back
//		-0.5f, -0.5f, -0.5f,	0.77f,	0.35f,		0.0f,	0.0f,	1.0f,
//		0.5f, -0.5f, -0.5f,		0.98f,	0.35f,		0.0f,	0.0f,	1.0f,
//		0.5f,  0.5f, -0.5f,		0.98f,	0.64f,		0.0f,	0.0f,	1.0f,
//		-0.5f,  0.5f, -0.5f,	0.77f,	0.64f,		0.0f,	0.0f,	1.0f,
//
//		// left
//		//x		y		z		S		T
//		-0.5f, -0.5f,  -0.5f,	0.0f,	0.35f,		1.0f,	0.0f,	0.0f,
//		-0.5f, -0.5f,  0.5f,	0.23f,  0.35f,		1.0f,	0.0f,	0.0f,
//		-0.5f,  0.5f,  0.5f,	0.23f,	0.64f,		1.0f,	0.0f,	0.0f,
//		-0.5f,  0.5f,  -0.5f,	0.0f,	0.64f,		1.0f,	0.0f,	0.0f,
//
//		// bottom
//		//x		y		z		S		T
//		-0.5f, -0.5f,  0.5f,	0.27f,	0.02f,		0.0f,	1.0f,	0.0f,
//		0.5f,  -0.5f,  0.5f,	0.48f,  0.02f,		0.0f,	1.0f,	0.0f,
//		 0.5f,  -0.5f,  -0.5f,	0.48f,	0.31f,		0.0f,	1.0f,	0.0f,
//		-0.5f, -0.5f,  -0.5f,	0.27f,	0.31f,		0.0f,	1.0f,	0.0f,
//
//		//UP
//		 //x		y		z		S		T
//		 -0.5f, 0.5f,  0.5f,	0.27f,	0.68f,		0.0f,	-1.0f,	0.0f,
//		 0.5f,  0.5f,  0.5f,	0.48f,  0.68f,		0.0f,	-1.0f,	0.0f,
//		  0.5f, 0.5f,  -0.5f,	0.48f,	0.98f,		0.0f,	-1.0f,	0.0f,
//		 -0.5f, 0.5f,  -0.5f,	0.27f,	0.98f,		0.0f,	-1.0f,	0.0f,
//
//	};
//	
//	Mesh *cubo = new Mesh();
//	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
//	meshList.push_back(cubo);
//
//}
void CrearCubo1()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 6, 7,
		4, 7, 5,
		// left
		8,9,10,
		9,10,11,
		// back
		12,13,14,
		13,14,15,
		//arriba
		16,17,19,
		17,18,19,
		//abajo
		20,21,22,
		20,22,23

	};

	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		u		v
		-0.5f, -0.5f,  0.5f,	0.01f,	0.5f,		0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		0.5f,	0.5f,		0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		0.5f,	0.99f,		0.0f,	0.0f,	-1.0f,//2
		-0.5f,  0.5f,  0.5f,	0.01f,	0.99f,		0.0f,	0.0f,	-1.0f,
		//derecho
		0.5f, -0.5f,  0.5f,		0.5f,	0.5f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,		0.5f,	0.99f,		-1.0f,	0.0f,	0.0f,//5
		0.5f, -0.5f, -0.5f,		0.95f,	0.5f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f, -0.5f,		0.95f,	0.99f,		-1.0f,	0.0f,	0.0f,//7
		//izquierdo
		-0.5f, -0.5f,  0.5f,	0.95f,	0.5f,		1.0f,	0.0f,	0.0f,//8
		-0.5f,  0.5f,  0.5f,	0.95f,	0.99f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f, -0.5f,	0.5f,	0.5f,		1.0f,	0.0f,	0.0f,
		-0.5f, 0.5f, -0.5f,		0.5f,	0.99f,		1.0f,	0.0f,	0.0f,//11
		// back
		-0.5f, -0.5f, -0.5f,	0.95f,	0.5f,		0.0f,	0.0f,	1.0f,//12
		-0.5f, 0.5f, -0.5f,		0.95f,	0.99f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.5f,	0.5f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.5f,	0.99f,		0.0f,	0.0f,	1.0f,//15
		//arriba
		0.5f,  0.5f,  0.5f,		0.95f,	0.5f,		0.0f,	1.0f,	0.0f,//16
		-0.5f,  0.5f,  0.5f,	0.5f,	0.5f,		0.0f,	1.0f,	0.0f,
		-0.5f, 0.5f, -0.5f,		0.5f,	0.99f,		0.0f,	1.0f,	0.0f,//18	
		0.5f,  0.5f, -0.5f,		0.95f,	0.99f,		0.0f,	1.0f,	0.0f,
		//abajo
		-0.5f, -0.5f,  0.5f,	0.5f,	0.99f,		0.0f,	-1.0f,	0.0f,//20
		0.5f, -0.5f,  0.5f,		0.95f,	0.99f,		0.0f,	-1.0f,	0.0f,
		0.5f, -0.5f, -0.5f,		0.95f,	0.5f,		0.0f,	-1.0f,	0.0f,//22
		-0.5f, -0.5f, -0.5f,	0.5f,	0.5f,		0.0f,	-1.0f,	0.0f,



	};
	Mesh *cubo1 = new Mesh();
	cubo1->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo1);

}

void CrearCubo2()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 6, 7,
		4, 7, 5,
		// left
		8,9,10,
		9,10,11,
		// back
		12,13,14,
		13,14,15,
		//arriba
		16,17,19,
		17,18,19,
		//abajo
		20,21,22,
		20,22,23

	};

	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		u		v
		-0.5f, -0.5f,  0.5f,	0.01f,	0.5f,		0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		0.5f,	0.5f,		0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		0.5f,	0.99f,		0.0f,	0.0f,	-1.0f,//2
		-0.5f,  0.5f,  0.5f,	0.01f,	0.99f,		0.0f,	0.0f,	-1.0f,
		//derecho
		0.5f, -0.5f,  0.5f,		0.5f,	0.5f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,		0.5f,	0.99f,		-1.0f,	0.0f,	0.0f,//5
		0.5f, -0.5f, -0.5f,		0.95f,	0.5f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f, -0.5f,		0.95f,	0.99f,		-1.0f,	0.0f,	0.0f,//7
		//izquierdo
		-0.5f, -0.5f,  0.5f,	0.95f,	0.5f,		1.0f,	0.0f,	0.0f,//8
		-0.5f,  0.5f,  0.5f,	0.95f,	0.99f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f, -0.5f,	0.5f,	0.5f,		1.0f,	0.0f,	0.0f,
		-0.5f, 0.5f, -0.5f,		0.5f,	0.99f,		1.0f,	0.0f,	0.0f,//11
		// back
		-0.5f, -0.5f, -0.5f,	0.95f,	0.5f,		0.0f,	0.0f,	1.0f,//12
		-0.5f, 0.5f, -0.5f,		0.95f,	0.99f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.5f,	0.5f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.5f,	0.99f,		0.0f,	0.0f,	1.0f,//15
		//arriba
		0.5f,  0.5f,  0.5f,		0.02f,	0.02f,		0.0f,	1.0f,	0.0f,//16
		-0.5f,  0.5f,  0.5f,	0.02f,	0.02f,		0.0f,	1.0f,	0.0f,
		-0.5f, 0.5f, -0.5f,		0.02f,	0.4f,		0.0f,	1.0f,	0.0f,//18	
		0.5f,  0.5f, -0.5f,		0.02f,	0.4f,		0.0f,	1.0f,	0.0f,
		//abajo
		-0.5f, -0.5f,  0.5f,	0.02f,	0.4f,		0.0f,	-1.0f,	0.0f,//20
		0.5f, -0.5f,  0.5f,		0.02f,	0.4f,		0.0f,	-1.0f,	0.0f,
		0.5f, -0.5f, -0.5f,		0.02f,	0.02f,		0.0f,	-1.0f,	0.0f,//22
		-0.5f, -0.5f, -0.5f,	0.02f,	0.02f,		0.0f,	-1.0f,	0.0f,



	};
	Mesh *cubo1 = new Mesh();
	cubo1->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo1);

}

void CrearFlipper()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 6, 7,
		4, 7, 5,
		// left
		8,9,10,
		9,10,11,
		// back
		12,13,14,
		13,14,15,
		//arriba
		16,17,18,
		//abajo
		19,20,21,

	};

	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		u		v
		-0.5f, -0.5f,  0.5f,	0.01f,	0.5f,		0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.0f,		0.5f,	0.5f,		0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.0f,		0.5f,	0.99f,		0.0f,	0.0f,	-1.0f,//2
		-0.5f,  0.5f,  0.5f,	0.01f,	0.99f,		0.0f,	0.0f,	-1.0f,
		//derecho
		0.5f, -0.5f,  0.0f,		0.5f,	0.5f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.0f,		0.5f,	0.99f,		-1.0f,	0.0f,	0.0f,//5
		0.5f, -0.5f, 0.0f,		0.95f,	0.5f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f, 0.0f,		0.95f,	0.99f,		-1.0f,	0.0f,	0.0f,//7
		//izquierdo
		-0.5f, -0.5f,  0.5f,	0.95f,	0.5f,		1.0f,	0.0f,	0.0f,//8
		-0.5f,  0.5f,  0.5f,	0.95f,	0.99f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f, -0.5f,	0.5f,	0.5f,		1.0f,	0.0f,	0.0f,
		-0.5f, 0.5f, -0.5f,		0.5f,	0.99f,		1.0f,	0.0f,	0.0f,//11
		// back
		-0.5f, -0.5f, -0.5f,	0.95f,	0.5f,		0.0f,	0.0f,	1.0f,//12
		-0.5f, 0.5f, -0.5f,		0.95f,	0.99f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, 0.0f,		0.5f,	0.5f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, 0.0f,		0.5f,	0.99f,		0.0f,	0.0f,	1.0f,//15
		//arriba
		0.5f,  0.5f,  0.0f,		0.02f,	0.02f,		0.0f,	1.0f,	0.0f,//16
		-0.5f,  0.5f,  0.5f,	0.02f,	0.02f,		0.0f,	1.0f,	0.0f,
		-0.5f, 0.5f, -0.5f,		0.02f,	0.4f,		0.0f,	1.0f,	0.0f,
		//abajo
		-0.5f, -0.5f,  0.5f,	0.02f,	0.4f,		0.0f,	-1.0f,	0.0f,//19
		0.5f, -0.5f,  0.0f,		0.02f,	0.4f,		0.0f,	-1.0f,	0.0f,//20
		-0.5f, -0.5f, -0.5f,	0.02f,	0.02f,		0.0f,	-1.0f,	0.0f,



	};
	Mesh *cubo1 = new Mesh();
	cubo1->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo1);

}

void CrearFlipperDer()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 6, 7,
		4, 7, 5,
		// left
		8,9,10,
		9,10,11,
		// back
		12,13,14,
		13,14,15,
		//arriba
		16,17,18,
		//abajo
		19,20,21,

	};

	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		u		v
		-0.5f, -0.5f,  0.0f,	0.01f,	0.5f,		0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		0.5f,	0.5f,		0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		0.5f,	0.99f,		0.0f,	0.0f,	-1.0f,//2
		-0.5f,  0.5f,  0.0f,	0.01f,	0.99f,		0.0f,	0.0f,	-1.0f,
		//derecho
		0.5f, -0.5f,  0.5f,		0.5f,	0.5f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,		0.5f,	0.99f,		-1.0f,	0.0f,	0.0f,//5
		0.5f, -0.5f, -0.5f,		0.95f,	0.5f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f, -0.5f,		0.95f,	0.99f,		-1.0f,	0.0f,	0.0f,//7
		//izquierdo
		-0.5f, -0.5f,  0.0f,	0.95f,	0.5f,		1.0f,	0.0f,	0.0f,//8
		-0.5f,  0.5f,  0.0f,	0.95f,	0.99f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f, 0.0f, 	0.5f,	0.5f,		1.0f,	0.0f,	0.0f,
		-0.5f, 0.5f, 0.0f,		0.5f,	0.99f,		1.0f,	0.0f,	0.0f,//11
		// back
		-0.5f, -0.5f, 0.0f,	0.95f,	0.5f,		0.0f,	0.0f,	1.0f,//12
		-0.5f, 0.5f, 0.0f,		0.95f,	0.99f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.5f,	0.5f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.5f,	0.99f,		0.0f,	0.0f,	1.0f,//15
		//arriba
		0.5f,  0.5f,  0.5f,		0.02f,	0.02f,		0.0f,	1.0f,	0.0f,//16
		-0.5f, 0.5f, 0.0f,		0.02f,	0.4f,		0.0f,	1.0f,	0.0f,//17	
		0.5f,  0.5f, -0.5f,		0.02f,	0.4f,		0.0f,	1.0f,	0.0f,
		//abajo
		-0.5f, -0.5f,  0.0f,	0.02f,	0.4f,		0.0f,	-1.0f,	0.0f,//19
		0.5f, -0.5f,  0.5f,		0.02f,	0.4f,		0.0f,	-1.0f,	0.0f,
		0.5f, -0.5f, -0.5f,		0.02f,	0.02f,		0.0f,	-1.0f,	0.0f,//21



	};
	Mesh *cubo1 = new Mesh();
	cubo1->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo1);

}
void CreateShaders()
{
	Shader *shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(*shader1);
}

int main() 
{
	mainWindow = Window(1366, 768); // 1280, 1024 or 1024, 768
	mainWindow.Initialise();

	CreateObjects();
	//CrearCubo();
	CrearCubo1();
	CrearCubo2();
	CrearFlipper();
	CrearFlipperDer();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTextureA();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTextureA();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTextureA();
	dadoTexture = Texture("Textures/dado.tga");
	dadoTexture.LoadTextureA();
	pisoTexture= Texture("Textures/piso.tga");
	pisoTexture.LoadTextureA();
	Tagave = Texture("Textures/Agave.tga");
	Tagave.LoadTextureA();
	caja = Texture("Textures/cajaMada.tga");
	caja.LoadTextureA();
	trono = Texture("Textures/trono.tga");
	trono.LoadTextureA();
	trono1 = Texture("Textures/trono1.tga");
	trono1.LoadTextureA();
	tronoAs = Texture("Textures/tronoAs.tga");
	tronoAs.LoadTextureA();
	tronoDer = Texture("Textures/tronoDer.tga");
	tronoDer.LoadTextureA();
	tronoIzq = Texture("Textures/tronoIzq.tga");
	tronoIzq.LoadTextureA();
	metal = Texture("Textures/metal.tga");
	metal.LoadTextureA();
	plata = Texture("Textures/plata.tga");
	plata.LoadTextureA();
	plano = Texture("Textures/plano.tga");
	plano.LoadTextureA();
	plano1 = Texture("Textures/plano1.tga");
	plano1.LoadTextureA();
	pieljulien = Texture("Textures/pieljulien.tga");
	pieljulien.LoadTextureA();
	pie = Texture("Textures/pie.tga");
	pie.LoadTextureA();
	luz = Texture("Textures/luz.tga");
	luz.LoadTextureA();
	senal = Texture("Textures/senal.tga");
	senal.LoadTextureA();
	marmol = Texture("Textures/marmol.tga");
	marmol.LoadTextureA();
	cuidado = Texture("Textures/cuidado.tga");
	cuidado.LoadTextureA();
	lapida = Texture("Textures/lapida.tga");
	lapida.LoadTextureA();
	cruz = Texture("Textures/cruz.tga");
	cruz.LoadTextureA();
	elefan = Texture("Textures/elefan.tga");
	elefan.LoadTextureA();
	hierro = Texture("Textures/hierro.tga");
	hierro.LoadTextureA();
	madera = Texture("Textures/madera.tga");
	madera.LoadTextureA();
	cemento = Texture("Textures/cemento.tga");
	cemento.LoadTextureA();
	banos = Texture("Textures/banos.tga");
	banos.LoadTextureA();
	banos1 = Texture("Textures/banos1.tga");
	banos1.LoadTextureA();
	restaurante = Texture("Textures/restaurante.tga");
	restaurante.LoadTextureA();
	mascarilla= Texture("Textures/mascarilla.tga");
	mascarilla.LoadTextureA();
	botiquin = Texture("Textures/botiquin.tga");
	botiquin.LoadTextureA();
	telefono = Texture("Textures/telefono.tga");
	telefono.LoadTextureA();
	flecha = Texture("Textures/flecha.tga");
	flecha.LoadTextureA();
	
	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);

	/*Kitt_M = Model();
	Kitt_M.LoadModel("Models/kitt.3ds");
	Llanta_M = Model();
	Llanta_M.LoadModel("Models/k_rueda.3ds");
	Blackhawk_M = Model();
	Blackhawk_M.LoadModel("Models/uh60.obj");
	Camino_M = Model();
	Camino_M.LoadModel("Models/railroad track.obj");*/
	spring = Model();
	spring.LoadModel("Models/spring1.obj");
	trunk = Model();
	trunk.LoadModel("Models/trunk1.obj");
	trampoline = Model();
	trampoline.LoadModel("Models/trampoline.obj");
	tunel = Model();
	tunel.LoadModel("Models/tunel.obj");
	//luz direccional, sólo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
								0.3f, 0.3f,
								0.0f, 0.0f, -1.0f);
//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaración de primer luz puntual
	pointLights[0] = PointLight(1.0f, 0.0f, 0.0f,
								0.0f, 1.0f,
								2.0f, 1.5f,1.5f,
								0.3f, 0.2f, 0.1f);
	pointLightCount++;
	
	unsigned int spotLightCount = 0;
	//linterna
	spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
		0.0f, 2.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, -1.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		20.0f);
	spotLightCount++;

	//luz fija
	spotLights[1] = SpotLight(0.50196f, 0.0f, 0.50196f, //color
		0.0f, 2.0f,          //difusion
		0.0f, -1.945f, 3.6f,   //posicion
		0.0f, -1.0f, 0.0f,  //factor de atenuacion
		1.0f, 0.0f, 0.0f,
		30.0f);
	spotLightCount++;
	//luz de faro
	 //luz de helicóptero

	/*glm::vec3 posblackhawk = glm::vec3(2.0f, 0.0f, 0.0f);*/

	std::vector<std::string> skyboxFaces;
	/*skyboxFaces.push_back("Textures/Skybox/cupertin-lake_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake_ft.tga");*/

	skyboxFaces.push_back("Textures/Skybox/creacion_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/creacion_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/creacion_up.tga");
	skyboxFaces.push_back("Textures/Skybox/creacion_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/creacion_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/creacion_ft.tga");

	/*skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_rt.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_lf.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_dn.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_up.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_bk.tga");
	skyboxFaces.push_back("Textures/Skybox/cupertin-lake-night_ft.tga");*/


	skybox = Skybox(skyboxFaces);


	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 300.0f);
	
	movCoche = 0.0f;
	//movOffset = 0.5f;
	movOffset = 10.0f;
	movMuslo = 0.0f;
	movPie = 0.0f;
	dedos = 0.0f;


	sp.init(); //inicializar esfera
	sp.load();//enviar la esfera al shader
	cono.init();
	cilindro.init();

	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime; 
		lastTime = now;
		/*if(movCoche <= 360.0f)
			{
			movCoche += movOffset * deltaTime;
			}*/

	
			if (movMuslo <=1440.0f)
			{
				movMuslo += movOffset * deltaTime;
			}
		
			if (movPie <= 360.0f)
			{
				movPie += movOffset * deltaTime;
			}
		
			if (dedos <= 360.0f)
			{
				dedos += movOffset * deltaTime;
			}

		//Recibir eventos del usuario
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());
		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		skybox.DrawSkybox(camera.calculateViewMatrix(), projection);
		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		glm::vec3 lowerLight = camera.getCameraPosition();
		lowerLight.y -= 0.3f;
		spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);
		shaderList[0].SetSpotLights(spotLights, spotLightCount);
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);

		glm::mat4 model(1.0);


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
		//model = glm::scale(model, glm::vec3(30.0f, 1.0f,30.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//plano.UseTexture();
		plano1.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		//muslo
		model = glm::mat4(1.0);
		glm::mat4 modelaux(1.0);//matriz para almacenar datos
		modelaux = model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
		modelaux = model = glm::rotate(model, glm::radians(movCoche * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-4.0f, -1.75f, 1.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		pieljulien.UseTexture();
		//caja.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();

		model = modelaux;
		//model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(-3.5f, -1.75f, 1.0f));
		model = glm::rotate(model, glm::radians(-movMuslo * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
		//chamorro
		modelaux = model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pieljulien.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();
		//muñeca
		model = modelaux;
		model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-movPie* toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		//palma
		modelaux = model = glm::translate(model, glm::vec3(0.15f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pie.UseTexture();
		//caja.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();

		//Para dibujar los dedos debo de almacenar la posición del centro de la palma y utilizar una nueva matriz auxiliar
		glm::mat4 modeldedos(1.0);
		//modelaux se quedara fija en la palma
		modeldedos = modelaux;
		model = modelaux;
		//articulación palma-falange dedo índice
		model = glm::translate(model, glm::vec3(0.15f, 0.2f, 0.0f));
		model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		//falange dedo índice
		//model = glm::mat4(1.0);
		modeldedos = model = glm::translate(model, glm::vec3(0.125f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.1f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		pie.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();

		//articulacion falange-falangina
		model = modeldedos;
		model = glm::translate(model, glm::vec3(0.125f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		//falangina dedo índice
		//model = glm::mat4(1.0);
		modeldedos = model = glm::translate(model, glm::vec3(0.15f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.09f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		//articulacion falangina-falangeta
		model = modeldedos;
		model = glm::translate(model, glm::vec3(0.15f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
		//falangeta dedo índice
		//model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.1f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.08f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();
		//termina dedo índice
		//regresar las matrices a la posición del centro de la palma
		model = modelaux;
		modeldedos = modelaux;

		//medio
		 //articulación palma-falange dedo medio
		model = glm::translate(model, glm::vec3(0.15f, 0.07f, 0.0f));
		model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
		//falange dedo medio
		modeldedos = model = glm::translate(model, glm::vec3(0.175f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.35f, 0.1f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		//articulacion falange-falangina
		model = modeldedos;
		model = glm::translate(model, glm::vec3(0.175f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		//falangina dedo medio
		//model = glm::mat4(1.0);
		modeldedos = model = glm::translate(model, glm::vec3(0.15f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.09f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		//articulacion falangina-falangeta
		model = modeldedos;
		model = glm::translate(model, glm::vec3(0.15f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		//falangeta dedo medio
		//model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.1f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.08f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();
		//termina dedo medio
		//regresar las matrices a la posición del centro de la palma
		model = modelaux;
		modeldedos = modelaux;

		//
		//anular
		//
		//articulación palma-falange dedo anular
		model = glm::translate(model, glm::vec3(0.15f, -0.07f, 0.0f));
		model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		//falange dedo anular
		//model = glm::mat4(1.0);
		modeldedos = model = glm::translate(model, glm::vec3(0.15f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.1f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		//articulacion falange-falangina
		model = modeldedos;
		model = glm::translate(model, glm::vec3(0.15f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		//falangina dedo anular
		//model = glm::mat4(1.0);
		modeldedos = model = glm::translate(model, glm::vec3(0.15f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.09f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		//articulacion falangina-falangeta
		model = modeldedos;
		model = glm::translate(model, glm::vec3(0.15f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		//falangeta dedo anular
		//model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.1f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.08f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();
		//termina dedo anular
		//regresar las matrices a la posición del centro de la palma
		model = modelaux;
		modeldedos = modelaux;

		//
		//menique
		//
		//articulación palma-falange dedo menique
		model = glm::translate(model, glm::vec3(0.15f, -0.2f, 0.0f));
		//model = glm::rotate(model, glm::radians(mainWindow.getfalange_menique() * toRadians), glm::vec3(0.0f, 0.0f, 1.0f));

		//falange dedo menique
		//model = glm::mat4(1.0);
		modeldedos = model = glm::translate(model, glm::vec3(0.075f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.15f, 0.1f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		//articulacion falange-falangina
		model = modeldedos;
		model = glm::translate(model, glm::vec3(0.075f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		//falangina dedo menique
		//model = glm::mat4(1.0);
		modeldedos = model = glm::translate(model, glm::vec3(0.15f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.09f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();

		//articulacion falangina-falangeta
		model = modeldedos;
		model = glm::translate(model, glm::vec3(0.15f, 0.0f, 0.0f));
		model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		//falangeta dedo menique
		//model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.1f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.2f, 0.08f, 0.2f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		meshList[1]->RenderMesh();
		//termina dedo menique

		//--------------------------------------objetos--------------------------------------------------------------

		//caja animales
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.0f,-1.5f,-1.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		caja.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();

		//silla
		//respaldo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.5f, -1.55f, -2.8f));
		model = glm::scale(model, glm::vec3(0.4f, 0.8f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		trono.UseTexture();
		meshList[1]->RenderMesh();
		//asiento
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.5f, -1.9f, -2.65f));
		model = glm::scale(model, glm::vec3(0.3f, 0.1f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		trono1.UseTexture();
		meshList[1]->RenderMesh();
		//izquierdo
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.675f, -1.85f, -2.65f));
		model = glm::scale(model, glm::vec3(0.05f, 0.2f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		tronoIzq.UseTexture();
		meshList[1]->RenderMesh();
		//derecho
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.325f, -1.85f, -2.65f));
		model = glm::scale(model, glm::vec3(0.05f, 0.2f, 0.4f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		tronoDer.UseTexture();
		meshList[1]->RenderMesh();

		//lapida
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, -1.8f, 2.5f));
		model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		lapida.UseTexture();
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, -1.5f, 2.5f));
		model = glm::scale(model, glm::vec3(0.05f, 0.3f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cruz.UseTexture();
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.5f, -1.5f, 2.5f));
		model = glm::scale(model, glm::vec3(0.3f, 0.05f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cruz.UseTexture();
		meshList[1]->RenderMesh();

		//----------------------------------------molino--------------------------------------------
		//cilindro
		model = glm::mat4(1.0);
		glm::mat4 modelaux1(1.0);//matriz para almacenar datos
		modelaux1 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.75f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 1.59f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));//FALSE ES PARA QUE NO SEA TRANSPUESTA
		cemento.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cilindro.render();

		model = modelaux1;
		//model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.0f, -0.25f, 0.0f));
		//model = glm::rotate(model, glm::radians(-movMuslo * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
		//cono
		modelaux1 = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.5f, 1.0f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brickTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cono.render();
		

		model = modelaux1;
		model = glm::translate(model, glm::vec3(0.0f,-0.25f, 0.25f));
		//model = glm::rotate(model, glm::radians(-movPie * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		////cilindro
		modelaux1 = model = glm::translate(model, glm::vec3(0.0f,0.0f, -0.3f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(0.03f, 0.5f, 0.03f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cilindro.render();

		glm::mat4 modelaspas(1.0);
		//modelaux1 se quedara fija en el tubo
		modelaspas = modelaux1;
		model = modelaux1;

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.2f));
		model =modelaspas= glm::rotate(model, glm::radians(-movMuslo * toRadians), glm::vec3(0.0f, 0.0f, 1.0f));

		////aspa1
		modelaspas = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.2f));
		model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.01f, 1.2f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cilindro.render();

		////articulacion madera-tubo
		model = modelaspas;
		//model = glm::translate(model, glm::vec3(0.125f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		////madera
		////model = glm::mat4(1.0);
		modelaspas = model = glm::translate(model, glm::vec3(-0.65f, 0.0f, -0.01f));
		model = glm::scale(model, glm::vec3(0.7f, 0.25f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		madera.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();

		
		////regresar las matrices a la posición del centro del tubo
		model = modelaux1;
		modelaspas = modelaux1;
		model = modelaspas = glm::rotate(model, glm::radians(-movMuslo * toRadians), glm::vec3(0.0f, 0.0f, 1.0f));

		//aspas2
		modelaspas = model = glm::translate(model, glm::vec3(0.0f, 0.94f, 0.4f));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.01f, 1.2f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cilindro.render();

		////articulacion madera-tubo
		model = modelaspas;
		//model = glm::translate(model, glm::vec3(0.125f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		////madera
		////model = glm::mat4(1.0);
		modelaspas = model = glm::translate(model, glm::vec3(0.0f, -0.3f, -0.01f));
		model = glm::scale(model, glm::vec3(0.25f, 0.7f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		madera.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();

		
		////regresar las matrices a la posición del centro del tubo
		model = modelaux1;
		modelaspas = modelaux1;
		model = modelaspas = glm::rotate(model, glm::radians(-movMuslo * toRadians), glm::vec3(0.0f, 0.0f, 1.0f));

		//aspas3
		modelaspas = model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.4f));
		model = glm::rotate(model, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.01f, 1.2f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cilindro.render();

		////articulacion madera-tubo
		model = modelaspas;
		//model = glm::translate(model, glm::vec3(0.125f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		////madera
		////model = glm::mat4(1.0);
		modelaspas = model = glm::translate(model, glm::vec3(0.65f, 0.0f, -0.01f));
		model = glm::scale(model, glm::vec3(0.7f, 0.25f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		madera.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();
		
		////regresar las matrices a la posición del tubo
		model = modelaux1;
		modelaspas = modelaux1;
		model = modelaspas = glm::rotate(model, glm::radians(-movMuslo * toRadians), glm::vec3(0.0f, 0.0f, 1.0f));

		//aspas4
		modelaspas = model = glm::translate(model, glm::vec3(0.0f, -0.01f, 0.4f));
		//model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(0.01f, 1.2f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		cilindro.render();

		////articulacion madera-tubo
		model = modelaspas;
		//model = glm::translate(model, glm::vec3(0.125f, 0.0f, 0.0f));
		//model = glm::rotate(model, glm::radians(-dedos * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));

		////madera
		////model = glm::mat4(1.0);
		modelaspas = model = glm::translate(model, glm::vec3(0.0f, -0.6f, -0.01f));
		model = glm::scale(model, glm::vec3(0.25f, 0.7f, 0.01f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		madera.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();



		//jaula
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.7f, -1.965f, 2.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.05f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		meshList[2]->RenderMesh();
		
		////cilindro
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.0f, -1.4f, 1.7f));
		model = glm::scale(model, glm::vec3(0.05f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		cilindro.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.0f, -1.4f, 1.9f));
		model = glm::scale(model, glm::vec3(0.05f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		cilindro.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.0f, -1.4f, 2.1f));
		model = glm::scale(model, glm::vec3(0.05f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		cilindro.render();
		
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.0f, -1.4f, 2.3f));
		model = glm::scale(model, glm::vec3(0.05f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		cilindro.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.8f, -1.4f, 2.3f));
		model = glm::scale(model, glm::vec3(0.05f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		cilindro.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.6f, -1.4f, 2.3f));
		model = glm::scale(model, glm::vec3(0.05f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		cilindro.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.4f, -1.4f, 2.3f));
		model = glm::scale(model, glm::vec3(0.05f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		cilindro.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.4f, -1.4f, 2.1f));
		model = glm::scale(model, glm::vec3(0.05f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		cilindro.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.4f, -1.4f, 1.9f));
		model = glm::scale(model, glm::vec3(0.05f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		cilindro.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.4f, -1.4f, 1.7f));
		model = glm::scale(model, glm::vec3(0.05f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		cilindro.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.6f, -1.4f, 1.7f));
		model = glm::scale(model, glm::vec3(0.05f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		cilindro.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.8f, -1.4f, 1.7f));
		model = glm::scale(model, glm::vec3(0.05f, 0.7f, 0.05f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		cilindro.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.7f, -1.4f, 2.0f));
		model = glm::scale(model, glm::vec3(0.7f, 0.05f, 0.7f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		hierro.UseTexture();
		meshList[2]->RenderMesh();


		//flipper
		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		modelaux=model = glm::translate(model, glm::vec3(-1.15f, -1.945f, 3.6f));
		modelaux = model = glm::rotate(model, glm::radians(mainWindow.getflipperiz() * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(-0.65f, -1.945f, 3.6f));
		model = glm::translate(model, glm::vec3(0.5f, -00.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		marmol.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();


		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		modelaux = model = glm::translate(model, glm::vec3(1.15f, -1.945f, 3.6f));
		modelaux= model = glm::rotate(model, glm::radians(mainWindow.getflipperder() * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
		//model = glm::translate(model, glm::vec3(0.65f, -1.945f, 3.6f));
		model = glm::translate(model, glm::vec3(-0.5f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		marmol.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		model = glm::mat4(1.0);
		modelaux = glm::mat4(1.0);
		modelaux= model = glm::translate(model, glm::vec3(-3.0f, -1.945f, -2.0f));
		modelaux= model = glm::rotate(model, glm::radians(mainWindow.getflippersup() * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
		model = glm::translate(model, glm::vec3(0.5f, 0.0f, 0.0f));
		model = glm::scale(model, glm::vec3(1.0f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		marmol.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[3]->RenderMesh();

		

		//canica
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getd(), -1.9f, mainWindow.getw()));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		metal.UseTexture();
		sp.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.0f, -1.9f, -1.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		plata.UseTexture();
		sp.render();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.7f, -1.9f, 3.0f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		metal.UseTexture();
		sp.render();

		//elementos visuales
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, 3.75f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, 3.25f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		senal.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, 2.75f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, 2.25f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		elefan.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, 0.75f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, 0.25f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		senal.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, -0.25f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, -0.75f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		elefan.UseTexture();
		meshList[2]->RenderMesh();


		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, -1.25f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, -1.75f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		senal.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, -2.25f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, -2.75f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		elefan.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.2f, -1.75f, -3.25f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.8f, -1.75f, -3.25f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		senal.UseTexture();
		meshList[2]->RenderMesh();

		//rampa
		model = glm::mat4(1.0); 
		model = glm::translate(model, glm::vec3(2.55f, -1.75f, 2.058f));
		model = glm::scale(model, glm::vec3(1.0f, 0.5f, 0.1f));
		model = glm::rotate(model, glm::radians(90 * toRadians), glm::vec3(0.0f, 1.0f, 0.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		cuidado.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[4]->RenderMesh();

		//elementos visuales flipper superior
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.0f, -1.75f, -2.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		banos.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.3f, -1.75f, -2.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.6f, -1.75f, -2.0f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		banos1.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.875f, -1.75f, -2.0f));
		model = glm::scale(model, glm::vec3(0.25f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		//elementos visuales flipper iz
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.15f, -1.75f, 3.6f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.45f, -1.75f, 3.6f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		restaurante.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.75f, -1.75f, 3.6f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.05f, -1.75f, 3.6f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		botiquin.UseTexture();
		meshList[2]->RenderMesh();

		//elementos visuales flipper der
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.15f, -1.75f, 3.6f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.45f, -1.75f, 3.6f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		telefono.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.75f, -1.75f, 3.6f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.05f, -1.75f, 3.6f));
		model = glm::scale(model, glm::vec3(0.3f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		mascarilla.UseTexture();
		meshList[2]->RenderMesh();


		//elementos visuales barrera
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, 3.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, 3.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, 2.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, 2.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, 1.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, 1.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, 0.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, 0.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, -0.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, -0.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, -1.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, -1.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, -2.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, -2.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, -3.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.95f, -1.75f, -3.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();
		 //sup
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.65f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.15f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.65f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.15f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.65f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(1.15f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.65f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.15f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.35f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-0.85f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.35f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-1.85f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.35f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.85f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.5f, -1.75f, -3.95f));
		model = glm::scale(model, glm::vec3(0.8f, 0.5f, 0.1f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		
		//izq---------------------------------------------------------------
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, 3.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, 3.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, 2.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, 2.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, 1.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, 1.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, 0.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, 0.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, -0.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, -0.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, -1.375f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.75f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		//espacio

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, -2.625f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.75f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, -3.25f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		flecha.UseTexture();
		meshList[2]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-3.95f, -1.75f, -3.75f));
		model = glm::scale(model, glm::vec3(0.1f, 0.5f, 0.5f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		luz.UseTexture();
		meshList[2]->RenderMesh();

		//carga de modelos
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(movCoche, -1.4f, 0.0f));
		////model = glm::scale(model, glm::vec3(0.1f,0.1f, 0.1f));
		//model = glm::scale(model, glm::vec3(0.005f, 0.005f, 0.005f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, 90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//Kitt_M.RenderModel();

		//

		//model = glm::mat4(1.0);
		//posblackhawk = glm::vec3(2.0f, 2.0f, 0.0f);
		//model = glm::translate(model, posblackhawk);
		//model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//model = glm::rotate(model, -90* toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//Blackhawk_M.RenderModel();


		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, -2.5f, -0.1f));
		////model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//Camino_M.RenderModel();

		//----------------------------------------------------
		//spring
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(3.7f, -1.9f, 3.5f));
		model = glm::scale(model, glm::vec3(0.1f, 0.1f, mainWindow.getresorte()));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		spring.RenderModel();

		//--------------------------------------------------


		////trunk
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.0f, -1.66f, 0.5f));
		model = glm::scale(model, glm::vec3(0.07f, 0.07f, 0.07f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		trunk.RenderModel();

		////trampoline
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(2.2f, -1.95f, 3.15f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.3f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		trampoline.RenderModel();


		//tunel
		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(-2.5f, -2.0f, -0.5f));
		model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(1.0f, 0.0f, 0.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		//model = glm::rotate(model, -90 * toRadians, glm::vec3(0.0f, 0.0f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		tunel.RenderModel();

		////Agave ¿qué sucede si lo renderizan antes del coche y de la pista?
		//model = glm::mat4(1.0);
		//model = glm::translate(model, glm::vec3(0.0f, -1.7f, -2.0f));
		//model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
		//glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		////blending: transparencia o traslucidez
		//glEnable(GL_BLEND);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		//Tagave.UseTexture();
		//Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		//meshList[3]->RenderMesh();
		//glDisable(GL_BLEND);

			glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}