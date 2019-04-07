#include <iostream>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

#include "Mesh.h"
#include "Shader.h"
#include "Camera.h"
#include <ctime>
// Function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

// Window dimensions
GLuint screenWidth = 800, screenHeight = 600;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void Do_Movement();


Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
bool keys[1024];
GLfloat lastX = 400, lastY = 300;
bool firstMouse = true;
Mesh pmesh;
GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;
bool notpressed = true;


// The MAIN function, from here we start the application and run the game loop
int main()
{
    std::cout << "Starting HW2 context, OpenGL 4.5" << std::endl;
    // Init GLFW
    glfwInit();
    // Set all the required options for GLFW
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "HW2", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);


    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }    

    // Define the viewport dimensions
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);  
    glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);


	//Mesh pmesh;
	pmesh.loadOffFile(".\\models\\sphere.off");
	pmesh.setupMesh();
	Shader shader(".\\Vetex.vert", ".\\Fragment.frag");
    // Game loop
    while (!glfwWindowShouldClose(window))
    {

		GLfloat currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

        // Check if any events have been activiated (key pressed, mouse moved etc.) and call corresponding response functions
        glfwPollEvents();

        // Render
        // Clear the colorbuffer
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		Do_Movement();
		shader.Use();

		glm::mat4 view;
		view = camera.GetViewMatrix();
		glm::mat4 projection;
		projection = glm::perspective(camera.Zoom, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);
		// Get the uniform locations
		GLint modelLoc = glGetUniformLocation(shader.Program, "model");
		GLint viewLoc = glGetUniformLocation(shader.Program, "view");
		GLint projLoc = glGetUniformLocation(shader.Program, "projection");
		// Pass the matrices to the shader
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
		glm::mat4 model;
		model = glm::translate(model, vec3(0.0f));
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

		pmesh.drawMesh();

        // Swap the screen buffers
        glfwSwapBuffers(window);
    }

    // Terminate GLFW, clearing any resources allocated by GLFW.
    glfwTerminate();
    return 0;
}

void Do_Movement()
{
	// Camera controls
	if (keys[GLFW_KEY_W])
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (keys[GLFW_KEY_S])
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (keys[GLFW_KEY_A])
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (keys[GLFW_KEY_D])
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (keys[GLFW_KEY_RIGHT])
		camera.ProcessKeyboard(RIGHTA, deltaTime);
	if (keys[GLFW_KEY_LEFT])
		camera.ProcessKeyboard(LEFTA, deltaTime);
	if (keys[GLFW_KEY_UP])
		camera.ProcessKeyboard(UPA, deltaTime);
	if (keys[GLFW_KEY_DOWN])
		camera.ProcessKeyboard(DOWNA, deltaTime);
	if (keys[GLFW_KEY_Q] && pmesh.faces.size()) {
		//pmesh.wireframe = !pmesh.wireframe;
		/*notpressed = true;
		srand((unsigned int)time(0));
		unsigned int rannum = rand() % pmesh.faces.size();
		pmesh.edgeCollapse(pmesh.faces[rannum]->vertexindices[0], pmesh.faces[rannum]->vertexindices[1]);
		//pmesh.edgeCollapse(pmesh.vertices[11], pmesh.vertices[23]);
		//pmesh.setupMesh();*/
		if (pmesh.pairs.size() != 0) {
			Pair pair = *(pmesh.pairs.begin());
			pmesh.pairs.erase(pmesh.pairs.begin());
			pmesh.edgeCollapse(pair.to, pair.from);
		}
	}
	if (keys[GLFW_KEY_F]) {
		pmesh.flatshading = !pmesh.flatshading;
		pmesh.setupMesh();
	}
	if (keys[GLFW_KEY_E]) {
		cout << "bruh" << endl;
		if (pmesh.splits.size() != 0) {
			pmesh.vertexSplit(pmesh.splits.back());
			pmesh.splits.pop_back();
		}
	}
}

// Is called whenever a key is pressed/released via GLFW
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	//cout << key << endl;
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;  // Reversed since y-coordinates go from bottom to left

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}


void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}