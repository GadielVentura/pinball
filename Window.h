#pragma once
#include<stdio.h>
#include<glew.h>
#include<glfw3.h>

class Window
{
public:
	Window();
	Window(GLint windowWidth, GLint windowHeight);
	int Initialise();
	GLfloat getBufferWidth() { return bufferWidth; }
	GLfloat getBufferHeight() { return bufferHeight; }
	GLfloat getXChange();
	GLfloat getYChange();
	GLfloat getmuevex() { return muevex; }
	GLfloat getflipperiz() { return flipperiz; }
	GLfloat getflipperder() { return flipperder; }
	GLfloat getflippersup() { return flippersup; }
	GLfloat getw() { return w; }
	GLfloat getd() { return d; }
	GLfloat getluz() { return luz; }
	GLfloat getluz1() { return luz1; }
	GLfloat getluz2() { return luz2; }
	GLfloat getresorte() { return resorte; }
	GLfloat getavCaz() { return avCaz; }
	GLfloat getavCax() { return avCax; }
	bool getShouldClose() {
		return  glfwWindowShouldClose(mainWindow);}
	bool* getsKeys() { return keys; }
	void swapBuffers() { return glfwSwapBuffers(mainWindow); }
	
	~Window();
private: 
	GLFWwindow *mainWindow;
	GLint width, height;
	bool keys[1024];
	GLint bufferWidth, bufferHeight;
	void createCallbacks();
	GLfloat lastX;
	GLfloat lastY;
	GLfloat xChange;
	GLfloat yChange;
	GLfloat muevex;
	GLfloat flipperiz=0.0f, flipperder=0.0f, flippersup=0.0f;
	GLfloat w=-1.0f, d=1.0f;
	GLfloat resorte=0.1f;
	GLfloat luz = 0.0f, luz1 = 0.0f, luz2 = 1.0f;
	GLfloat avCaz = 3.0f;
	GLfloat avCax = 0.0f;
	bool mouseFirstMoved;
	static void ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode);
	static void ManejaMouse(GLFWwindow* window, double xPos, double yPos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);

};

