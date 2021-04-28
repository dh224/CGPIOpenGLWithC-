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
using namespace std;

#define numVAOs 1
#define numVBOs 2



Utils util = Utils();
float cameraX, cameraY, cameraZ;
float cubeLocX, cubeLocY, cubeLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

//分配在display()中所需要的变量的空间，这样就不需要多次分配空间了。
GLuint mvLoc, projLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat,rMat,tMat;// r,tMat用于旋转 动画


void setupVertices(void) { //36个顶点，12个三角形，组成了放置在远点的2*2*2的立方体
	float vertexPositions[108] = {
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

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexPositions), vertexPositions, GL_STATIC_DRAW);
}




void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("verShader4-1.glsl", "fragShader4-1.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 8.0f;
	cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;
	setupVertices();
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);


	// 获取ModelView矩阵和project矩阵的统一变量
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;

	//旋转动画所需的
	tMat = glm::translate(glm::mat4(1.0f), glm::vec3(sin(0.35f * currentTime) * 2, cos(0.52f * currentTime) * 2.0f, sin(0.7f * currentTime) * 2.0f));
	//构建project矩阵
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472为60度的弧度值
	//构建ModelView矩阵
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));
	rMat = glm::rotate(glm::mat4(1.0f), 1.75f * (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));
	rMat = glm::rotate(rMat,1.75f * (float)currentTime, glm::vec3(1.0f,0.0f,0.0f));
	rMat = glm::rotate(rMat, 1.75f * (float)currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
	mMat = tMat * rMat;
	mvMat = vMat * mMat;

	//将project矩阵和MV矩阵赋值
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	//将VBO关联到顶点着色器中
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);//将第0个缓冲区标记为活跃
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);//将第0个属性关联到缓冲区
	glEnableVertexAttribArray(0);//启用第0个顶点属性

	glEnable(GL_DEPTH_TEST);//调整OpenGL设置，绘制模型
	glDepthFunc(GL_LEQUAL);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}


int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 4 - program 1", NULL, NULL);
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