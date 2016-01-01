// Include standard headers
#include <stdio.h>
#include <stdlib.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <common/shader.hpp>

int main(void)
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow(1024, 768, "Tutorial 02 - Red triangle", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

	//DO I NEED THIS??
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader");

	GLuint uiVBO[2];
	float fTriangle[9]; // Data to render triangle (3 vertices, each has 3 floats)
	float fQuad[12]; // Data to render quad using triangle strips (4 vertices, each has 3 floats)

	// Setup triangle vertices
	fTriangle[0] = -0.4f; fTriangle[1] = 0.1f; fTriangle[2] = 0.0f;
	fTriangle[3] = 0.4f; fTriangle[4] = 0.1f; fTriangle[5] = 0.0f;
	fTriangle[6] = 0.0f; fTriangle[7] = 0.7f; fTriangle[8] = 0.0f;

	// Setup quad vertices

	fQuad[0] = -0.2f; fQuad[1] = -0.1f; fQuad[2] = 0.0f;
	fQuad[3] = -0.2f; fQuad[4] = -0.6f; fQuad[5] = 0.0f;
	fQuad[6] = 0.2f; fQuad[7] = -0.1f; fQuad[8] = 0.0f;
	fQuad[9] = 0.2f; fQuad[10] = -0.6f; fQuad[11] = 0.0f;

	// Now we create two VBOs
	glGenBuffers(2, uiVBO);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fTriangle), fTriangle, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(fQuad), fQuad, GL_STATIC_DRAW);

	do {

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		// Triangle render
		glBindBuffer(GL_ARRAY_BUFFER, uiVBO[0]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Quad render using triangle strip
		glBindBuffer(GL_ARRAY_BUFFER, uiVBO[1]);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO
	glDeleteBuffers(2, uiVBO);
	glDeleteProgram(programID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}