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
GLuint tfLoc;
float tf; //time factor
//������display()������Ҫ�ı����Ŀռ䣬�����Ͳ���Ҫ��η���ռ��ˡ�
GLuint mvLoc, projLoc,vLoc;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat,rMat,tMat;// r,tMat������ת ����


void setupVertices(void) { //36�����㣬12�������Σ�����˷�����Զ���2*2*2��������
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
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 420.0f;
	cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;
	glfwGetFramebufferSize(window, &width, &height);

	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472Ϊ60�ȵĻ���ֵ

	setupVertices();
}

void display(GLFWwindow* window, double currentTime) {
	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);


	// ��ȡModelView�����project�����ͳһ����
	//mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	vLoc = glGetUniformLocation(renderingProgram, "v_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));




	tf = ((float)currentTime);
	tfLoc = glGetUniformLocation(renderingProgram, "tf");
	glUniform1f(tfLoc, (float)tf);


	glUniformMatrix4fv(vLoc, 1, GL_FALSE, glm::value_ptr(vMat));
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	//��VBO������������ɫ����
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);//����0�����������Ϊ��Ծ
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);//����0�����Թ�����������
	glEnableVertexAttribArray(0);//���õ�0����������

	glEnable(GL_DEPTH_TEST);//����OpenGL���ã�����ģ��
	glDepthFunc(GL_LEQUAL);
	glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 100000);	// 0, 36, 24  (or 100000)	
}


int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Chapter 4 - program 1", NULL, NULL);
	glfwMakeContextCurrent(window);
	glewExperimental = GL_TRUE;//Ҳ�����Կ����������⣬��Ҫ����GLEW��ʵ�鹦�ܲ�����ȫ���������ԣ���Ȼ�ᱨ��

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