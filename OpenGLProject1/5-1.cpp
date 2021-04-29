#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Utils.h"
#include <stack>
using namespace std;

#define numVAOs 1
#define numVBOs 2



Utils util = Utils();
float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ, pyrLocX, pyrLocY, pyrLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
GLuint tfLoc;
float tf; //time factor
//分配在display()中所需要的变量的空间，这样就不需要多次分配空间了。
GLuint mvLoc, projLoc, vLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat, rMat, tMat;// r,tMat用于旋转 动画
stack<glm::mat4> mvStack;

GLuint brickTexture;
void setupVertices(void) {
	//36个顶点，12个三角形，组成了放置在远点的2*2*2的立方体
	float cubePositions[108] = {
		-1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, -1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, 1.0f, -1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f, 1.0f,  1.0f, -1.0f,
		1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, 1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f, -1.0f,  1.0f, -1.0f, -1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f, -1.0f
	};
	float pyramidPositions[54] =
	{ -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,    //front
		1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,    //right
		1.0f, -1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,  //back
		-1.0f, -1.0f, -1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,  //left
		-1.0f, -1.0f, -1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, //LF
		1.0f, -1.0f, 1.0f, -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f  //RR
	};
	float textureCoordinates[36] =
	{ 0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 0.0f, 0.5f, 1.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoordinates), textureCoordinates, GL_STATIC_DRAW);
}




void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("vertShader5-1.glsl", "fragShader5-1.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 6.0f;
	cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;
	pyrLocX = 0.0f; pyrLocY = 0.0f; pyrLocZ = 0.0f;

	setupVertices();
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472为60度的弧度值  我懂了，这其实就是定义了 Near far 面

	brickTexture = Utils::loadTexture("brick1.jpg");


}


void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));

	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(pyrLocX, pyrLocY, pyrLocZ));

	mMat = glm::rotate(mMat, -0.45f, glm::vec3(1.0f, 0.0f, 0.0f));
	mMat = glm::rotate(mMat, 0.61f, glm::vec3(0.0f, 1.0f, 0.0f));
	mMat = glm::rotate(mMat, 0.00f, glm::vec3(0.0f, 0.0f, 1.0f));

	mvMat = vMat * mMat;

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brickTexture);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, 18);
	//mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));
	//mvMat = vMat * mMat;




	//tf = ((float)currentTime);
	//tfLoc = glGetUniformLocation(renderingProgram, "tf");
	//glUniform1f(tfLoc, (float)tf);


	////将VBO关联到顶点着色器中
	//glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);//将第0个缓冲区标记为活跃
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);//将第0个属性关联到缓冲区
	//glEnableVertexAttribArray(0);//启用第0个顶点属性

	//glEnable(GL_DEPTH_TEST);//调整OpenGL设置，绘制模型
	//glDepthFunc(GL_LEQUAL);
	//glDrawArrays(GL_TRIANGLES, 0, 36);	// 0, 36, 24  (or 100000)	

		// draw the pyramid using buffer #1
	//mMat = glm::translate(glm::mat4(1.0f), glm::vec3(pyrLocX, pyrLocY, pyrLocZ));
	//mvMat = vMat * mMat;


	//glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(0);


}
void window_reshape_callback(GLFWwindow* window, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);

	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); //
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 4 - program 1", NULL, NULL);
	glfwSetWindowSizeCallback(window, window_reshape_callback);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;//也许是显卡驱动的问题，需要开启GLEW的实验功能才能完全加载其特性，不然会报错

	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}