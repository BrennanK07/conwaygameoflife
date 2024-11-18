#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <shader.h>
#include <iostream>

#include "simulation.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
unsigned int genTexture(unsigned char* pixels, Shader* shaderProgram);

int screenWidth = 1000;
int screenHeight = 1000;

unsigned char* pixels;

int main() {
	//Create window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, ".", NULL, NULL);

	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);

	//Initialize GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Set the viewport of the window and callback for resize
	glViewport(0, 0, screenWidth, screenHeight);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//Rendering setup
	float vertices[] = {
		-1.0,  1.0, 0.0, 0.0f, 1.0f, //top left
		-1.0, -1.0, 0.0, 0.0f, 0.0f, //bottom left
		 1.0,  1.0, 0.0, 1.0f, 1.0f, //top right
		 1.0, -1.0, 0.0, 1.0f, 0.0f //bottom right
	};

	int indices[] = {
		0, 1, 2,
		1, 2, 3
	};

	unsigned int VBO, VAO, EBO;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// Position attribute
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // 3 floats for position
	glEnableVertexAttribArray(0);

	// Texture coordinate attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // 2 floats for texture coords
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);


	//Shader
	Shader* defaultShader = new Shader("vert.glsl", "frag.glsl");
	defaultShader->use();

	glActiveTexture(GL_TEXTURE0); // Activate texture unit 0
	glBindTexture(GL_TEXTURE_2D, genTexture(updateGameState(), defaultShader));
	glUniform1i(glGetUniformLocation(defaultShader->ID, "screenTexture"), 0); // Bind the texture to the sampler

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	//Debug game logic
	initPixelArray();
	debug();
	pixels = updateGameState();

	unsigned int texture = genTexture(pixels, defaultShader);

	//Render loop
	while (!glfwWindowShouldClose(window)) {
		//Input
		processInput(window);

		//Rendering
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glBindTexture(GL_TEXTURE_2D, texture);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screenWidth, screenHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels);

		//defaultShader->use();
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		//Check and call events then swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	if (glfwGetKey(window, GLFW_KEY_F2) == GLFW_PRESS) {
		initPixelArray();
		pixels = updateGameState();
	}

	if (glfwGetKey(window, GLFW_KEY_F3) == GLFW_PRESS) {
		processPixels();
		pixels = updateGameState();
	}
}

unsigned int genTexture(unsigned char* pixels, Shader* shaderProgram) {
	// Create texture
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);

	glUseProgram(shaderProgram->ID);
	glUniform1i(glGetUniformLocation(shaderProgram->ID, "screenTexture"), 0);

	return texture;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);

	screenWidth = width;
	screenHeight = height;

	initPixelArray();
	debug();
}