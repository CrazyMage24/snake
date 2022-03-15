#include<iostream>
#include <math.h>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <filesystem>
namespace fs = std::filesystem;

#include "Texture.h"
#include "Shader.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Vertices coordinates
GLfloat snake_vertices[] =
{ //     COORDINATES     /        COLORS      /       TexCoord  //
	-0.02f, -0.02f, 0.0f,     1.0f, 0.0f, 0.0f,	    0.0f, 0.0f, // Lower left corner
	-0.02f,  0.02f, 0.0f,     0.0f, 1.0f, 0.0f,	    0.0f, 1.0f, // Upper left corner
	 0.02f,  0.02f, 0.0f,     0.0f, 0.0f, 1.0f,	    1.0f, 1.0f, // Upper right corner
	 0.02f, -0.02f, 0.0f,     1.0f, 1.0f, 1.0f,	    1.0f, 0.0f  // Lower right corner
};

// Indices for vertices order
GLuint snake_indices[] =
{
	0, 2, 1, // Upper triangle
	0, 3, 2 // Lower triangle
};

unsigned int WIDTH = 800;
unsigned int HEIGHT = 800;

float TIMESPAN = 0.1f;

enum DIRECTION
{
	LEFT, RIGHT, UP, DOWN
};
DIRECTION direction = RIGHT;

struct Point
{
	int x;
	int y;
	Point(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
};

vector<Point> snake;
vector<VAO> vaos;
vector<VBO> vbos;
vector<EBO> ebos;
vector<Shader> shaders;
vector<GLuint> x_s;
vector<GLuint> y_s;
vector<Texture> textures;

int main()
{
	snake.push_back(Point(0, 0));
	snake.push_back(Point(-1, 0));
	snake.push_back(Point(-2, 0));
	// Initialize GLFW
	glfwInit();

	// Tell GLFW what version of OpenGL we are using 
	// In this case we are using OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Tell GLFW we are using the CORE profile
	// So that means we only have the modern functions
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a GLFWwindow object of 800 by 800 pixels, naming it "YoutubeOpenGL"
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Snake", NULL, NULL);
	// Error check if the window fails to create
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	// Introduce the window into the current context
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Load GLAD so it configures OpenGL
	gladLoadGL();
	// Specify the viewport of OpenGL in the Window
	// In this case the viewport goes from x = 0, y = 0, to x = 800, y = 800
	glViewport(0, 0, WIDTH, HEIGHT);

	// Generates Shader object using shaders default.vert and default.frag
	for (int i = 0; i < snake.size(); i++)
	{
		shaders.push_back(Shader("default.vert", "default.frag"));
		vaos.push_back(VAO());
		vaos[i].Bind();
		vbos.push_back(VBO(snake_vertices, sizeof(snake_vertices)));
		ebos.push_back(EBO(snake_indices, sizeof(snake_indices)));

		vaos[i].LinkAttrib(vbos[i], 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
		vaos[i].LinkAttrib(vbos[i], 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		vaos[i].LinkAttrib(vbos[i], 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		// Unbind all to prevent accidentally modifying them
		vaos[i].Unbind();
		vbos[i].Unbind();
		ebos[i].Unbind();

		x_s.push_back(glGetUniformLocation(shaders[i].ID, "x_"));
		y_s.push_back(glGetUniformLocation(shaders[i].ID, "y_"));

		std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
		std::string texPath = "\\snake\\";
		// Texture
		textures.push_back(Texture((parentDir + texPath + "git.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE));
		textures[i].texUnit(shaders[i], "tex0", 0);
	}

	glfwSetKeyCallback(window, key_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	double lastTime = glfwGetTime(), timer = lastTime;
	double deltaTime = 0, nowTime = 0;

	// Main while loop
	while (!glfwWindowShouldClose(window))
	{
		// - Measure time
		nowTime = glfwGetTime();
		deltaTime = nowTime - lastTime;

		// - Reset after one second
		if (deltaTime > TIMESPAN) 
		{
			deltaTime = 0;
			lastTime = glfwGetTime();

			for (int i = snake.size() - 1; i >= 1; i--)
			{
				snake[i].x = snake[i - 1].x;
				snake[i].y = snake[i - 1].y;
			}

			if (direction == RIGHT)
			{
				snake[0].x++;
				if (snake[0].x == 25)
				{
					snake[0].x = -25;
				}
			}
			else if (direction == LEFT)
			{
				snake[0].x--;
				if (snake[0].x == -25)
				{
					snake[0].x = 25;
				}
			}
			else if (direction == UP)
			{
				snake[0].y++;
				if (snake[0].y == 25)
				{
					snake[0].y = -25;
				}
			}
			else if (direction == DOWN)
			{
				snake[0].y--;
				if (snake[0].y == -25)
				{
					snake[0].y = 25;
				}
			}
			// Specify the color of the background
			glClearColor(0.17f, 0.23f, 0.67f, 1.0f);
			// Clean the back buffer and assign the new color to it
			glClear(GL_COLOR_BUFFER_BIT);
			// Tell OpenGL which Shader Program we want to use
			for (int i = 0; i < snake.size(); i++)
			{
				shaders[i].Activate();
				glUniform1i(x_s[i], snake[i].x);
				glUniform1i(y_s[i], snake[i].y);
				// Binds texture so that is appears in rendering
				textures[i].Bind();
				// Bind the VAO so OpenGL knows to use it
				vaos[i].Bind();

				// Draw primitives, number of indices, datatype of indices, index of indices
				glDrawElements(GL_TRIANGLES, 6000, GL_UNSIGNED_INT, 0);
			}
			// Swap the back buffer with the front buffer
			glfwSwapBuffers(window);
			// Take care of all GLFW events
			glfwPollEvents();

		}
	}

	// Delete all the objects we've created
	for (int i = 0; i < snake.size(); i++)
	{
		vaos[i].Delete();
		vbos[i].Delete();
		ebos[i].Delete();
		shaders[i].Delete();
		textures[i].Delete();
	}
	// Delete window before ending the program
	glfwDestroyWindow(window);
	// Terminate GLFW before ending the program
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) 
{
	// make sure the viewport matches the new window dimensions; note that width and
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) 
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		direction = RIGHT;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		direction = LEFT;
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		direction = UP;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		direction = DOWN;
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		snake.push_back(Point(snake[2].x, snake[2].y - 1));
		shaders.push_back(Shader("default.vert", "default.frag"));
		vaos.push_back(VAO());
		vaos[vaos.size()-1].Bind();
		vbos.push_back(VBO(snake_vertices, sizeof(snake_vertices)));
		ebos.push_back(EBO(snake_indices, sizeof(snake_indices)));

		vaos[vaos.size() - 1].LinkAttrib(vbos[vbos.size() - 1], 0, 3, GL_FLOAT, 8 * sizeof(float), (void*)0);
		vaos[vaos.size() - 1].LinkAttrib(vbos[vbos.size() - 1], 1, 3, GL_FLOAT, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		vaos[vaos.size() - 1].LinkAttrib(vbos[vbos.size() - 1], 2, 2, GL_FLOAT, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		// Unbind all to prevent accidentally modifying them
		vaos[vaos.size() - 1].Unbind();
		vbos[vbos.size() - 1].Unbind();
		ebos[ebos.size() - 1].Unbind();

		x_s.push_back(glGetUniformLocation(shaders[shaders.size() - 1].ID, "x_"));
		y_s.push_back(glGetUniformLocation(shaders[shaders.size() - 1].ID, "y_"));

		std::string parentDir = (fs::current_path().fs::path::parent_path()).string();
		std::string texPath = "\\snake\\";
		// Texture
		textures.push_back(Texture((parentDir + texPath + "git.png").c_str(), GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE));
		textures[textures.size() - 1].texUnit(shaders[shaders.size() - 1], "tex0", 0);
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) 
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) 
	{
		double xpos, ypos;
		//getting cursor position
		glfwGetCursorPos(window, &xpos, &ypos);
		std::cout << "Cursor Position at (" << xpos << " : " << ypos << "\n";
	}
}