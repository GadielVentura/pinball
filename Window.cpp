#include<stdio.h>
#include<Windows.h>
#include "Window.h"
//float flipperiz = 0.0f,
//flipperder = 0.0f,
//flippersup = 0.0f;
Window::Window()
{
	width = 800;
	height = 600;
	flipperiz = 0.0f;
	flipperder = 0.0f;
	flippersup = 0.0f;
	w;
	d;
	resorte;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
Window::Window(GLint windowWidth, GLint windowHeight)
{
	width = windowWidth;
	height = windowHeight;
	muevex = 2.0f;
	for (size_t i = 0; i < 1024; i++)
	{
		keys[i] = 0;
	}
}
int Window::Initialise()
{
	//Inicialización de GLFW
	if (!glfwInit())
	{
		printf("Falló inicializar GLFW");
		glfwTerminate();
		return 1;
	}
	//Asignando variables de GLFW y propiedades de ventana
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	//para solo usar el core profile de OpenGL y no tener retrocompatibilidad
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	//CREAR VENTANA
	mainWindow = glfwCreateWindow(width, height, "Primer ventana", NULL, NULL);

	if (!mainWindow)
	{
		printf("Fallo en crearse la ventana con GLFW");
		glfwTerminate();
		return 1;
	}
	//Obtener tamaño de Buffer
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//asignar el contexto
	glfwMakeContextCurrent(mainWindow);

	//MANEJAR TECLADO y MOUSE
	createCallbacks();


	//permitir nuevas extensiones
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		printf("Falló inicialización de GLEW");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	glEnable(GL_DEPTH_TEST); //HABILITAR BUFFER DE PROFUNDIDAD
							 // Asignar valores de la ventana y coordenadas
							 
							 //Asignar Viewport
	glViewport(0, 0, bufferWidth, bufferHeight);
	//Callback para detectar que se está usando la ventana
	glfwSetWindowUserPointer(mainWindow, this);
}

void Window::createCallbacks()
{
	glfwSetKeyCallback(mainWindow, ManejaTeclado);
	glfwSetCursorPosCallback(mainWindow, ManejaMouse);
	glfwSetMouseButtonCallback(mainWindow, mouse_button_callback); 
}
GLfloat Window::getXChange()
{
	GLfloat theChange = xChange;
	xChange = 0.0f;
	return theChange;
}

GLfloat Window::getYChange()
{
	GLfloat theChange = yChange;
	yChange = 0.0f;
	return theChange;
}




void Window::ManejaTeclado(GLFWwindow* window, int key, int code, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key == GLFW_KEY_Y)
	{
		theWindow-> muevex += 1.0;
	}
	if (key == GLFW_KEY_U)
	{
		theWindow-> muevex -= 1.0;
	}

	if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
	{
		if (theWindow->flippersup == 0.0f || theWindow->flippersup == 1320)
		{
			theWindow->flippersup += 1320.0f;
		}
		if(theWindow->flippersup == 2640.0f)
		{
			theWindow->flippersup -= 2640.0f;
		}
	}

	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
	{
		if (theWindow->flipperiz == 0.0f || theWindow->flipperiz == 1320)
		{
			theWindow->flipperiz += 1320.0f;
		}
		if (theWindow->flipperiz == 2640.0f)
		{
			theWindow->flipperiz -= 2640.0f;
		}
	}
	
	if ((key == GLFW_KEY_M)) //&& theWindow->flipperder < 1320.0
	{
		if (theWindow->flipperder == 0.0f || theWindow->flipperder == -1320)
		{
			theWindow->flipperder -= 1320.0f;
		}
		if (theWindow->flipperder == -2640.0f)
		{
			theWindow->flipperder += 2640.0f;
		}
	}

	if (key == GLFW_KEY_W)
	{
		theWindow->w += 0.2;
	}
	if (key == GLFW_KEY_S)
	{
		theWindow->w -= 0.2;
	}


	if (key == GLFW_KEY_D)
	{
		theWindow->d += 0.2;
	}
	if (key == GLFW_KEY_A)
	{
		theWindow->d -= 0.2;
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
		{
			theWindow->keys[key] = true;
			printf("se presiono la tecla %d'\n", key);
		}
		else if (action == GLFW_RELEASE)
		{
			theWindow->keys[key] = false;
			printf("se solto la tecla %d'\n", key);
		}
	}
}

void Window::ManejaMouse(GLFWwindow* window, double xPos, double yPos)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	if (theWindow->mouseFirstMoved)
	{
		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		theWindow->mouseFirstMoved = false;
	}

	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->yChange = theWindow->lastY - yPos;

	theWindow->lastX = xPos;
	theWindow->lastY = yPos;
}

void Window::mouse_button_callback(GLFWwindow* window, int button, int action, int mode)
{
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
	
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{


		//Sleep(2000);
		if (theWindow->resorte == 0.1f || theWindow->resorte==0.05f)
		{

			theWindow->resorte -= 0.05f;
		}
		if (theWindow->resorte == 0.0f)
		{
		
		theWindow->resorte += 0.1f;
		}

	}
}

Window::~Window()
{
	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}
