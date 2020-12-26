/*
Semestre 2021-1
Pr�ctica  Iluminaci�n 1
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

#include "Window.h"
#include "Mesh.h"
#include "Shader_light.h"
#include "Camera.h"
#include "Texture.h"
//para iluminaci�n
#include "CommonValues.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "Material.h"

const float toRadians = 3.14159265f / 180.0f;

Window mainWindow;
std::vector<Mesh*> meshList;
std::vector<Shader> shaderList;
Camera camera;

Texture brickTexture;
Texture dirtTexture;
Texture plainTexture;
Texture dadoTexture;
Texture paisajes;
Texture d8;
//materiales
Material Material_brillante;
Material Material_opaco;
//luz direccional
DirectionalLight mainLight;
//para declarar varias luces de tipo pointlight
PointLight pointLights[MAX_POINT_LIGHTS];

GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

// Vertex Shader
static const char* vShader = "Shaders/shader_light.vert";

// Fragment Shader
static const char* fShader = "Shaders/shader_light.frag";
//c�lculo del promedio de las normales para sombreado de Phong
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

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);

	Mesh *obj1 = new Mesh();
	obj1->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj1);

	Mesh *obj2 = new Mesh();
	obj2->CreateMesh(vertices, indices, 32, 12);
	meshList.push_back(obj2);

	Mesh *obj3 = new Mesh();
	obj3->CreateMesh(floorVertices, floorIndices, 32, 6);
	meshList.push_back(obj3);
}

void CrearCubo()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		4, 5, 6,
		6, 7, 4,
		// back
		8, 9, 10,
		10, 11, 8,

		// left
		12, 13, 14,
		14, 15, 12,
		// bottom
		16, 17, 18,
		18, 19, 16,
		// top
		20, 21, 22,
		22, 23, 20,
	};
	//Ejercicio 1: reemplazar con sus dados de 6 caras texturizados, agregar normales en el eje X la normal va inversa, en el caso del eje z igual, en el caso del eje y va directo
	GLfloat cubo_vertices[] = {
		// front
		//x		y		z		S		T			NX		NY		NZ
		-0.5f, -0.5f,  0.5f,	0.25f,  0.33f,		0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		0.5f,	0.33f,		0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		0.5f,	0.66f,		0.0f,	0.0f,	-1.0f,
		-0.5f,  0.5f,  0.5f,	0.25f,	0.66f,		0.0f,	0.0f,	-1.0f,
		// right
		//x		y		z		S		T
		0.5f, -0.5f,  0.5f,	    0.5f,  0.33f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.75f,	0.33f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  -0.5f,	0.75f,	0.66f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,	    0.5f,	0.66f,		-1.0f,	0.0f,	0.0f,
		// back
		-0.5f, -0.5f, -0.5f,	1.0f,	0.33f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.75f,	0.33f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.75f,	0.66f,		0.0f,	0.0f,	1.0f,
		-0.5f,  0.5f, -0.5f,	1.0f,	0.66f,		0.0f,	0.0f,	1.0f,

		// left
		//x		y		z		S		T
		-0.5f, -0.5f,  -0.5f,	0.0f,	0.33f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f,  0.5f,	0.25f,  0.33f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  0.5f,	0.25f,	0.66f,		1.0f,	0.0f,	0.0f,
		-0.5f,  0.5f,  -0.5f,	0.0f,	0.66f,		1.0f,	0.0f,	0.0f,

		// bottom
		//x		y		z		S		T
		-0.5f, -0.5f,  0.5f,	0.25f,	0.0f,		0.0f,	-1.0f,	0.0f,//y negativo
		0.5f,  -0.5f,  0.5f,	0.5f,  0.0f,		0.0f,	-1.0f,	0.0f,
		 0.5f,  -0.5f,  -0.5f,	0.5f,	0.33f,		0.0f,	-1.0f,	0.0f,
		-0.5f, -0.5f,  -0.5f,	0.25f,	0.33f,		0.0f,	-1.0f,	0.0f,

		//UP
		 //x		y		z		S		T
		 -0.5f, 0.5f,  0.5f,	0.25f,	0.66f,		0.0f,	1.0f,	0.0f, //y positivo
		 0.5f,  0.5f,  0.5f,	0.5f,  0.66f,		0.0f,	1.0f,	0.0f,
		  0.5f, 0.5f,  -0.5f,	0.5f,	1.0f,		0.0f,	1.0f,	0.0f,
		 -0.5f, 0.5f,  -0.5f,	0.25f,	1.0f,		0.0f,	1.0f,	0.0f,

	};
	Mesh *cubo = new Mesh();
	cubo->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo);

}
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
		-0.5f, -0.5f,  0.5f,	0.51f,	0.34f,		0.0f,	0.0f,	-1.0f,
		0.5f, -0.5f,  0.5f,		0.74f,	0.34f,		0.0f,	0.0f,	-1.0f,
		0.5f,  0.5f,  0.5f,		0.74f,	0.66f,		0.0f,	0.0f,	-1.0f,//2
		-0.5f,  0.5f,  0.5f,	0.51f,	0.66f,		0.0f,	0.0f,	-1.0f,
		//derecho
		0.5f, -0.5f,  0.5f,		0.74f,	0.34f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f,  0.5f,		0.74f,	0.66f,		-1.0f,	0.0f,	0.0f,//5
		0.5f, -0.5f, -0.5f,		1.0f,	0.34f,		-1.0f,	0.0f,	0.0f,
		0.5f,  0.5f, -0.5f,		1.0f,	0.66f,		-1.0f,	0.0f,	0.0f,//7
		//izquierdo
		-0.5f, -0.5f,  0.5f,	0.51f,	0.34f,		1.0f,	0.0f,	0.0f,//8
		-0.5f,  0.5f,  0.5f,	0.51f,	0.66f,		1.0f,	0.0f,	0.0f,
		-0.5f, -0.5f, -0.5f,	0.25f,	0.34f,		1.0f,	0.0f,	0.0f,
		-0.5f, 0.5f, -0.5f,		0.25f,	0.66f,		1.0f,	0.0f,	0.0f,//11
		// back
		-0.5f, -0.5f, -0.5f,	0.25f,	0.34f,		0.0f,	0.0f,	1.0f,//12
		-0.5f, 0.5f, -0.5f,		0.25f,	0.66f,		0.0f,	0.0f,	1.0f,
		0.5f, -0.5f, -0.5f,		0.0f,	0.34f,		0.0f,	0.0f,	1.0f,
		0.5f,  0.5f, -0.5f,		0.0f,	0.66f,		0.0f,	0.0f,	1.0f,//15
		//arriba
		0.5f,  0.5f,  0.5f,		0.74f,	0.66f,		0.0f,	1.0f,	0.0f,//16
		-0.5f,  0.5f,  0.5f,	0.51f,	0.66f,		0.0f,	1.0f,	0.0f,
		-0.5f, 0.5f, -0.5f,		0.51f,	1.0f,		0.0f,	1.0f,	0.0f,//18	
		0.5f,  0.5f, -0.5f,		0.74f,	1.0f,		0.0f,	1.0f,	0.0f,
		//abajo
		-0.5f, -0.5f,  0.5f,	0.51f,	0.34f,		0.0f,	-1.0f,	0.0f,//20
		0.5f, -0.5f,  0.5f,		0.74f,	0.34f,		0.0f,	-1.0f,	0.0f,
		0.5f, -0.5f, -0.5f,		0.74f,	0.0f,		0.0f,	-1.0f,	0.0f,//22
		-0.5f, -0.5f, -0.5f,	0.51f,	0.0f,		0.0f,	-1.0f,	0.0f,



	};
	Mesh *cubo1 = new Mesh();
	cubo1->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubo1);

}

void CrearCubod8()
{
	unsigned int cubo_indices[] = {
		// front
		0, 1, 2,
		//abajo
		3,4,5,
		//derecha
		6,7,8,
		//izquierda
		11,10,9,
		//inferior
		//frontal
		14,13,12,
		//atras
		16,17,15,
		//derecha
		18,19,20,
		23,22,21,
	};

	GLfloat cubo_vertices[] = {

		//frontal
		//x		y		z		u		v
		-0.5f, -0.5f,  0.5f,	0.0f,	0.33f,		0.0f,	0.0f,	-1.0f,//0
		0.5f, -0.5f,  0.5f,		0.29f,	0.33f,		0.0f,	0.0f,	-1.0f,
		0.0f,  0.5f,  0.0f,		0.16f,  0.66f,		0.0f,	0.0f,	-1.0f,//2
		//atras
		0.5f, -0.5f, -0.5f,		0.56f,	0.33f,		0.0f,	0.0f,	1.0f,//3
		-0.5f, -0.5f, -0.5f,	0.84f,	0.33f,		0.0f,	0.0f,	1.0f,
		0.0f,  0.5f,  0.0f,		0.72f,  0.66f,		0.0f,	0.0f,	1.0f,//5
		//derecha
		0.5f, -0.5f,  0.5f,		0.29f,	0.33f,		-1.0f,	0.0f,	0.0f,//6
		0.5f, -0.5f, -0.5f,		0.56f,	0.33f,		-1.0f,	0.0f,	0.0f,
		0.0f,  0.5f,  0.0f,		0.44f,  0.66f,		-1.0f,	0.0f,	0.0f,//8
		//izquierda
		-0.5f, -0.5f,  0.5f,	1.0f,	0.66f,		1.0f,	0.0f,	0.0f,//9
		-0.5f, -0.5f, -0.5f,	0.72f,	0.66f,		1.0f,	0.0f,	0.0f,
		0.0f,  0.5f,  0.0f,		0.84f,  1.0f,		1.0f,	0.0f,	0.0f,//11

		//abajo
		//frontal
		-0.5f, -0.5f,  0.5f,	0.16f,	0.66f,		0.0f,	0.0f,	-1.0f,//12
		0.5f, -0.5f,  0.5f,		0.44f,	0.66f,		0.0f,	0.0f,	-1.0f,
		0.0f,  -1.5f,  0.0f,	0.29f,  0.33f,		0.0f,	0.0f,	-1.0f,//14
		//atras 
		-0.5f, -0.5f, -0.5f,	1.0f,	0.66f,		0.0f,	0.0f,	1.0f,//15
		0.5f, -0.5f,  -0.5f,	0.72f,	0.66f,		0.0f,	0.0f,	1.0f,
		0.0f,  -1.5f,  0.0f,	0.84f,  0.33f,		0.0f,	0.0f,	1.0f,//17
		//derecha
		0.5f, -0.5f,  0.5f,		0.44f,	0.66f,		-1.0f,	0.0f,	0.0f,
		0.5f, -0.5f,  -0.5f,	0.72f,	0.66f,		-1.0f,	0.0f,	0.0f,
		0.0f,  -1.5f,  0.0f,	0.56f,  0.33f,		-1.0f,	0.0f,	0.0f,//20
		//izquierda
		-0.5f, -0.5f,  0.5f,	0.28f,	0.33f,		1.0f,	0.0f,	0.0f,//21
		-0.5f, -0.5f, -0.5f,	0.0f,	0.33f,		1.0f,	0.0f,	0.0f,
		0.0f,  -1.5f,  0.0f,	0.16f,  0.0f,		1.0f,	0.0f,	0.0f,//23
	};
	Mesh *cubod8 = new Mesh();
	cubod8->CreateMesh(cubo_vertices, cubo_indices, 192, 36);
	meshList.push_back(cubod8);

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
	CrearCubo();
	CrearCubo1();
	CrearCubod8();
	CreateShaders();

	camera = Camera(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), -60.0f, 0.0f, 5.0f, 0.5f);

	brickTexture = Texture("Textures/brick.png");
	brickTexture.LoadTexture();
	dirtTexture = Texture("Textures/dirt.png");
	dirtTexture.LoadTexture();
	plainTexture = Texture("Textures/plain.png");
	plainTexture.LoadTexture();
	dadoTexture = Texture("Textures/dado.tga");
	dadoTexture.LoadTexture();
	paisajes = Texture("Textures/paisajes.tga");
	paisajes.LoadTexture();
	d8 = Texture("Textures/d8.tga");
	d8.LoadTexture();

	Material_brillante = Material(4.0f, 256);
	Material_opaco = Material(0.3f, 4);
	//luz direccional, s�lo 1 y siempre debe de existir
	mainLight = DirectionalLight(1.0f, 1.0f, 1.0f, 
								0.3f, 0.3f,
								0.0f, 0.0f, -1.0f);
//contador de luces puntuales
	unsigned int pointLightCount = 0;
	//Declaraci�n de primer luz puntual
	pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
								0.0f, 1.0f,
								0.0f, 0.3f, 0.0f,
								0.3f, 0.2f, 0.1f);
	pointLightCount++;

	pointLights[1] = PointLight(1.0f, 0.0f, 0.0f,
		0.0f, 1.0f,
		0.0f,0.1f,0.0f,
		0.3f, 0.0f, 0.1f);
	pointLightCount++;
	//Ejercicio 2: agregar otra luz puntual

	/*Ejercicio 3: mover las dos luces de forma independiente con teclado y tomar 3 capturas de pantalla:
	1.- Dado iluminado con luz 1
	2.- Dado iluminado con luz 2
	3.- Dado iluminado con luz 1 y 2
	*/

	GLuint uniformProjection = 0, uniformModel = 0, uniformView = 0, uniformEyePosition = 0,
		uniformSpecularIntensity = 0, uniformShininess = 0;
	glm::mat4 projection = glm::perspective(45.0f, (GLfloat)mainWindow.getBufferWidth() / mainWindow.getBufferHeight(), 0.1f, 100.0f);
	
	//Loop mientras no se cierra la ventana
	while (!mainWindow.getShouldClose())
	{
		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime; 
		lastTime = now;

		//Recibir eventos del usuario
		glfwPollEvents();

		camera.keyControl(mainWindow.getsKeys(), deltaTime);
		camera.mouseControl(mainWindow.getXChange(), mainWindow.getYChange());

		// Clear the window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0].UseShader();
		uniformModel = shaderList[0].GetModelLocation();
		uniformProjection = shaderList[0].GetProjectionLocation();
		uniformView = shaderList[0].GetViewLocation();
		uniformEyePosition = shaderList[0].GetEyePositionLocation();
		uniformSpecularIntensity = shaderList[0].GetSpecularIntensityLocation();
		uniformShininess = shaderList[0].GetShininessLocation();

		pointLights[1] = PointLight(1.0f, 0.0f, 0.0f,
			0.0f, 1.0f,
			mainWindow.getluzx(), mainWindow.getluzy(), mainWindow.getluzz(),
			0.3f, 0.0f, 0.1f);

		shaderList[0].SetDirectionalLight(&mainLight);
		shaderList[0].SetPointLights(pointLights, pointLightCount);

		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.calculateViewMatrix()));
		glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);


		glm::mat4 model(1.0);

		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -2.5f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		brickTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[0]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, 4.0f, -2.5f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		dirtTexture.UseTexture();
		Material_opaco.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[1]->RenderMesh();

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(0.0f, -2.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(0.4f, 0.4f, 1.0f));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		plainTexture.UseTexture();
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		meshList[2]->RenderMesh();
		
		//instanciar su dado
	/*	model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getmuevex(), mainWindow.getMovY(), mainWindow.getMovZ()));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		paisajes.UseTexture();
		meshList[4]->RenderMesh();*/

		model = glm::mat4(1.0);
		model = glm::translate(model, glm::vec3(mainWindow.getmuevex(), mainWindow.getMovY(), mainWindow.getMovZ()));
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		Material_brillante.UseMaterial(uniformSpecularIntensity, uniformShininess);
		d8.UseTexture();
		meshList[5]->RenderMesh();

		glUseProgram(0);

		mainWindow.swapBuffers();
	}

	return 0;
}