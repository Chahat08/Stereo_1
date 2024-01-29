#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <random>
#include <vector>

#include "Shader.h"
#include "VertexData.h"
#include "Constants.h"
#include "Camera.h"

int SCREEN_WIDTH = 800;
int SCREEN_HEIGHT = 600;

int CURSOR_XPOS = INT_MIN;
int CURSOR_YPOS = INT_MIN;

int NUM_CUBES = 16;

void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
	if (CURSOR_XPOS == INT_MIN) {
		// set initial values
		CURSOR_XPOS = xpos;
		CURSOR_YPOS = ypos;
	}

	if (CURSOR_XPOS != xpos || CURSOR_YPOS != ypos)
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GLFW_TRUE);
	}
}

void createModelMatrices(Shader& shader, std::vector<glm::vec3> positions, std::vector<glm::vec3> axes) {
	for (int i = 0; i < NUM_CUBES; ++i) {
		glm::mat4 model(1.0f);
		model = glm::translate(model, positions[i]);
		model = glm::rotate(model, (float)glfwGetTime(), axes[i]);

		shader.setUniformMatrix4float("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 12*6);
	}
}

void createViewMatrixLeft(Shader& shader) {
	const float radius = 10.0f;
	float camX = sin(glfwGetTime()) * radius;
	float camZ = cos(glfwGetTime()) * radius;
	glm::mat4 view(1.0f);


	view = glm::lookAt(
		glm::vec3(camX, 0.0f, camZ),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	shader.setUniformMatrix4float("view", view);
}

void createViewMatrixRight(Shader& shader) {
	const float radius = 10.0f;
	float camX = sin(glfwGetTime()) * radius;
	float camZ = cos(glfwGetTime()) * radius;
	glm::mat4 view(1.0f);


	view = glm::lookAt(
		glm::vec3(camX + 1.0f, 0.0f, camZ),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	shader.setUniformMatrix4float("view", view);
}

void viewAndProjectionMatrices(Shader& shader, Camera& camera, bool leftEye) {
	float left, right, top, bottom, near = 0.1, far = 10000;
	float aspectRatio, radians, width_near, near_divided_by_focal_length;
	float fov;
	
	/* Clip to avoid extreme stereo */
	near = camera.focalLength / 5;
	aspectRatio = SCREEN_WIDTH / SCREEN_HEIGHT;
	radians = glm::radians(camera.aperture);
	width_near = near * tan(radians);
	near_divided_by_focal_length = near / camera.focalLength;
	fov = 2 * atan(width_near / near);

	//glDrawBuffer(GL_BACK_LEFT);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glDrawBuffer(GL_BACK_RIGHT);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 r(glm::normalize((glm::cross(camera.viewDirection, camera.upDirection))));

	r.x *= camera.eyeSeparation / 2.0;
	r.y *= camera.eyeSeparation / 2.0;
	r.z *= camera.eyeSeparation / 2.0;

	glm::mat4 projection(1.0f);
	glm::mat4 view(1.0f);

	if (!leftEye) {
		left = -aspectRatio * width_near - 0.5 * camera.eyeSeparation * near_divided_by_focal_length;
		right = aspectRatio * width_near - 0.5 * camera.eyeSeparation * near_divided_by_focal_length;
		top = width_near;
		bottom = -width_near;

		// PROJECTION MATRIX for the right eye
		projection = glm::frustum(left, right, bottom, top, near, far);

		// VIEW MATRIX for the right eye
		view = glm::lookAt(
			glm::vec3(camera.position.x + r.x, camera.position.y + r.y, camera.position.z + r.z), // cam pos
			glm::vec3(camera.position.x + r.x + camera.viewDirection.x,
				camera.position.y + r.y + camera.viewDirection.y,
				camera.position.z + r.z + camera.viewDirection.z), // cam target
			camera.upDirection // up
		);

		//glDrawBuffer(GL_BACK_RIGHT);
		//glDrawArrays(GL_TRIANGLES, 0, 12 * 6);
	}
	else {
		left = -aspectRatio * width_near + 0.5 * camera.eyeSeparation * near_divided_by_focal_length;
		right = aspectRatio * width_near + 0.5 * camera.eyeSeparation * near_divided_by_focal_length;
		top = width_near;
		bottom = -width_near;

		// PROJECTION MATRIX for the left eye
		projection = glm::frustum(left, right, bottom, top, near, far);

		// VIEW MATRIX for the left eye
		view = glm::lookAt(
			glm::vec3(camera.position.x - r.x, camera.position.y - r.y, camera.position.z - r.z), // cam pos
			glm::vec3(camera.position.x - r.x + camera.viewDirection.x,
				camera.position.y - r.y + camera.viewDirection.y,
				camera.position.z - r.z + camera.viewDirection.z), // cam target
			camera.upDirection // up
		);

		//glDrawBuffer(GL_BACK_LEFT);
		//glDrawArrays(GL_TRIANGLES, 0, 12 * 6);
	}

	shader.setUniformMatrix4float("projection", projection);
	shader.setUniformMatrix4float("view", view);
}

void viewAndProjectionMatrices(Shader& shader, Camera& camera, float offset=0.5f) {
	glDrawBuffer(GL_BACK_LEFT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawBuffer(GL_BACK_RIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const float radius = 10.0f;
	float camX = sin(glfwGetTime()) * radius;
	float camZ = cos(glfwGetTime()) * radius;
	glm::mat4 view(1.0f);

	// create view and projection matrices left

	view = glm::lookAt(
		glm::vec3(camX+offset, 0.0f, camZ),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	shader.setUniformMatrix4float("view", view);

	glDrawBuffer(GL_BACK_RIGHT);
	glDrawArrays(GL_TRIANGLES, 0, 12 * 6);

	// create view and projection matrices right


	view = glm::lookAt(
		glm::vec3(camX-offset, 0.0f, camZ),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f)
	);
	shader.setUniformMatrix4float("view", view);

	glDrawBuffer(GL_BACK_RIGHT);
	glDrawArrays(GL_TRIANGLES, 0, 12 * 6);
}

int main() {
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_STEREO, GLFW_TRUE);

	//glfwWindowHint(GLFW_DECORATED, NULL); // to remove border and titlebar

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	SCREEN_WIDTH = mode->width;
	SCREEN_HEIGHT = mode->height;

	GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Title", monitor, NULL);
	//GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Stereo window hopefully", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		return -1;
	}

	glfwMakeContextCurrent(window);
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN); // hide the cursor


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	//glfwSetCursorPosCallback(window, cursor_position_callback);
	glfwSetKeyCallback(window, key_callback);

	const GLubyte* vendor = glGetString(GL_VENDOR);
	const GLubyte* renderer = glGetString(GL_RENDERER);
	std::cout << "vendor: " << vendor << std::endl;
	std::cout << "renderer: " << renderer << std::endl;

	// creating shaders
	Shader shader("VertexShaderSource.vert", "FragmentShaderSource.frag", true);

	// creating and sending vertex data
	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositionsAndColors), &vertexPositionsAndColors, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glEnable(GL_DEPTH_TEST);

	std::random_device rd;
	std::mt19937 engine(rd());
	std::uniform_real_distribution<> distribution_xy(-5.0, 5.0);
	std::uniform_real_distribution<> distribution_z(-5.0, 5.0);
	std::uniform_real_distribution<> distribution_axes(0, 2 * PI);

	std::vector<glm::vec3> positions, axes;

	for (int i = 0; i < NUM_CUBES; ++i) {
		positions.push_back(glm::vec3(distribution_xy(engine), distribution_xy(engine), distribution_z(engine)));
		axes.push_back(glm::vec3(distribution_axes(engine), distribution_axes(engine), distribution_axes(engine)));
	}

	glm::mat4 projection(1.0f);
	projection = glm::perspective(
		glm::radians(45.0f),
		(float)SCREEN_WIDTH / (float)SCREEN_HEIGHT,
		0.1f,
		100.0f
	);
	shader.setUniformMatrix4float("projection", projection);

	Camera camera(
		glm::vec3(0.0f, 0.0f, 3.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		70.0,
		50.0,
		70.0/20.0
	);

	int i = 0;
	while (!glfwWindowShouldClose(window)) {
		processInput(window);

		glClearColor(0.0f, 0.027f, 0.212f, 0.1f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.use();
		glBindVertexArray(VAO);
		if(i++%2) createViewMatrixLeft(shader);
		else createViewMatrixRight(shader);
		shader.setUniformMatrix4float("projection", projection);
		createModelMatrices(shader, positions, axes);
		//viewAndProjectionMatrices(shader, camera, 0.5);
		//if (i++ % 2) viewAndProjectionMatrices(shader, camera, true);
		//else viewAndProjectionMatrices(shader, camera, false);
		//glDrawArrays(GL_TRIANGLES, 0, 12 * 6);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();

	return 0;
}