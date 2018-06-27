// Simplified Renderer application for GP course
// Code is similar to the one in lab 1 but all the graphics sections were refactored into the Graphics Class.
// Extra improvements:
// Reduced OpenGL version from 4.5 to 3.3 to allow it to render in older laptops.
// Added Shapes library for rendering cubes, spheres and vectors.
// Added examples of matrix multiplication on Update.
// Added resize screen and keyboard callbacks.
// 
// Update 2018/01 updated libraries and created project for VS2015.

// Suggestions or extra help please do email me S.Padilla@hw.ac.uk
//
// Note: Do not forget to link the libraries correctly and add the GLEW DLL in your debug/release folder.

#include <iostream>
#include <vector>
using namespace std;

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>


#include "graphics.h"
#include "shapes.h"
#include "ObjectMover.h"

// FUNCTIONS
void render(double currentTime);
void update(double currentTime);
void startup();
void onResizeCallback(GLFWwindow* window, int w, int h);
void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

// VARIABLES
bool		running = true;

Graphics	myGraphics;		// Runing all the graphics in this object

Cube		myCube;

const int numberParticles = 360;
ObjectMover particleEmitter[numberParticles];

float t = 0.001f;			// Global variable for animation
float explosionTimer = 0.0f;

bool mousePressed = false;
bool alreadyPressed = false;
double mousePosX = 0.0;
double mousePosY = 0.0;

int windowWidth;
int windowHeight;

int main()
{
	int errorGraphics = myGraphics.Init();		// Launch window and graphics context
	if (errorGraphics) return 0;				//Close if something went wrong...

	startup();									// Setup all necessary information for startup (aka. load texture, shaders, models, etc).

												// Mixed graphics and update functions - declared in main for simplicity.
	glfwSetWindowSizeCallback(myGraphics.window, onResizeCallback);			// Set callback for resize
	glfwSetKeyCallback(myGraphics.window, onKeyCallback);					// Set Callback for keys

																			// MAIN LOOP run until the window is closed
	do {
		double currentTime = glfwGetTime();		// retrieve timelapse
		glfwPollEvents();						// poll callbacks
		update(currentTime);					// update (physics, animation, structures, etc)
		render(currentTime);					// call render function.

		glfwSwapBuffers(myGraphics.window);		// swap buffers (avoid flickering and tearing)

		mousePressed = glfwGetMouseButton(myGraphics.window, GLFW_MOUSE_BUTTON_1) || mousePressed;

		if (mousePressed && !alreadyPressed) {
			glfwGetCursorPos(myGraphics.window, &mousePosX, &mousePosY );
			glfwGetWindowSize(myGraphics.window, &windowWidth, &windowHeight);
			mousePosX = (mousePosX / windowWidth -0.5);
			mousePosY = (mousePosY / windowHeight -0.5) * -1;
			alreadyPressed = true;
			explosionTimer = t;
		}


		running &= (glfwGetKey(myGraphics.window, GLFW_KEY_ESCAPE) == GLFW_RELEASE);	// exit if escape key pressed
		running &= (glfwWindowShouldClose(myGraphics.window) != GL_TRUE);
	} while (running);

	myGraphics.endProgram();			// Close and clean everything up...

	cout << "\nPress any key to continue...\n";
	cin.ignore(); cin.get(); // delay closing console to read debugging errors.

	return 0;
}

void startup() {

	// Calculate proj_matrix for the first time.
	myGraphics.aspect = (float)myGraphics.windowWidth / (float)myGraphics.windowHeight;
	myGraphics.proj_matrix = glm::perspective(glm::radians(50.0f), myGraphics.aspect, 0.1f, 1000.0f);

	// Load Geometry
	myCube.Load();

	for (int i = 0; i < numberParticles; i++) {
		Particle p;
		p.Load();

		glm::vec3 v = glm::vec3(cos(glm::radians((float)i)), sin(glm::radians((float) i)),0.0f);

		particleEmitter[i] = ObjectMover(p, v);
	}

	myGraphics.SetOptimisations();		// Cull and depth testing
}

void update(double currentTime) {
	if (mousePressed) {
		for (int i = 0; i < numberParticles; i++) {
			glm::mat4 mv_particle =
				glm::translate(glm::vec3((float) mousePosX, (float) mousePosY, -6.0f) + (t - explosionTimer ) * particleEmitter[i].v) *
				glm::rotate(glm::radians(i + 270.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
				glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)) *
				glm::mat4(1.0f);

			particleEmitter[i].particle.mv_matrix = mv_particle;
			particleEmitter[i].particle.proj_matrix = myGraphics.proj_matrix;
		}

	}
	// Calculate Cube movement ( T * R * S ) http://www.opengl-tutorial.org/beginners-tutorials/tutorial-3-matrices/
	glm::mat4 mv_matrix_cube =
		glm::translate(glm::vec3(200.0f, 0.0f, -6.0f)) *
		glm::rotate(glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)) *
		//glm::rotate(t, glm::vec3(1.0f, 0.0f, 0.0f)) *
		//glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)) *
		glm::mat4(1.0f);
	myCube.mv_matrix = mv_matrix_cube;
	myCube.proj_matrix = myGraphics.proj_matrix;

	t += 0.1f; // increment movement variable

}

void render(double currentTime) {
	// Clear viewport - start a new frame.
	myGraphics.ClearViewport();

	// Draw
	myCube.Draw();

	if (mousePressed && t < (explosionTimer + 1)) {
		for (int i = 0; i < numberParticles; i++) {
			particleEmitter[i].particle.Draw();
		}
	}
	else {
		mousePressed = false;
		alreadyPressed = false;
	}

}

void onResizeCallback(GLFWwindow* window, int w, int h) {	// call everytime the window is resized
	myGraphics.windowWidth = w;
	myGraphics.windowHeight = h;

	myGraphics.aspect = (float)w / (float)h;
	myGraphics.proj_matrix = glm::perspective(glm::radians(50.0f), myGraphics.aspect, 0.1f, 1000.0f);
}

void onKeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) { // called everytime a key is pressed
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);

	//if (key == GLFW_KEY_LEFT) angleY += 0.05f;
}