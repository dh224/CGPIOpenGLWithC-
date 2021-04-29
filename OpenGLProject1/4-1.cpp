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
//������display()������Ҫ�ı����Ŀռ䣬�����Ͳ���Ҫ��η���ռ��ˡ�
GLuint mvLoc, projLoc,vLoc ;
int width, height;
float aspect;
glm::mat4 pMat, vMat, mMat, mvMat,rMat,tMat;// r,tMat������ת ����
stack<glm::mat4> mvStack;

void setupVertices(void) { 
	//36�����㣬12�������Σ�����˷�����Զ���2*2*2��������
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
	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubePositions), cubePositions, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(pyramidPositions), pyramidPositions, GL_STATIC_DRAW);
}




void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("vertShader4-7.glsl", "fragShader4-1.glsl");
	cameraX = 0.0f; cameraY = 0.0f; cameraZ = 12.0f;
	cubeLocX = 0.0f; cubeLocY = -2.0f; cubeLocZ = 0.0f;
	pyrLocX = 0.0f; pyrLocY = 0.0f; pyrLocZ = 0.0f;

	setupVertices();
	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f); // 1.0472Ϊ60�ȵĻ���ֵ  �Ҷ��ˣ�����ʵ���Ƕ����� Near far ��

	
}


void display(GLFWwindow* window, double currentTime) {


	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);
	glEnable(GL_CULL_FACE);

	// ��ȡModelView�����project�����ͳһ����
	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");

	//vLoc = glGetUniformLocation(renderingProgram, "v_matrix");

	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mvStack.push(vMat);
	//����������
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));//�̶���������̫������λ��
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime,glm::vec3(1.0f,0.0f,0.0f));//ȷ������������ת

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LEQUAL);
	glFrontFace(GL_CCW);
	glDrawArrays(GL_TRIANGLES, 0, 18);
	mvStack.pop();//�Ƴ�����������ת
	//�����塪�����ǲ���
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)currentTime) * 4.0,0.0f, cos((float)currentTime) * 4.0));//ȷ������λ��
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 1.0f, 0.0f));//ȷ�����ǵ���ת
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	mvStack.pop();//�Ƴ����ǵ���ת
	//�����塪������
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin((float)currentTime) * 2.0, cos((float)currentTime) * 2.0));//ȷ������λ��
	mvStack.top() *= glm::rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0f, 0.0f, 1.0f));//ȷ���������ת
	mvStack.top() *= glm::scale(glm::mat4(1.0f),glm::vec3(0.25,0.25,0.25));//�������������
	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);
	glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLES, 0, 36);

	mvStack.pop();
	mvStack.pop();
	mvStack.pop();
	mvStack.pop();//���
	//mMat = glm::translate(glm::mat4(1.0f), glm::vec3(cubeLocX, cubeLocY, cubeLocZ));
	//mvMat = vMat * mMat;




	//tf = ((float)currentTime);
	//tfLoc = glGetUniformLocation(renderingProgram, "tf");
	//glUniform1f(tfLoc, (float)tf);


	////��VBO������������ɫ����
	//glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);//����0�����������Ϊ��Ծ
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);//����0�����Թ�����������
	//glEnableVertexAttribArray(0);//���õ�0����������

	//glEnable(GL_DEPTH_TEST);//����OpenGL���ã�����ģ��
	//glDepthFunc(GL_LEQUAL);
	//glDrawArrays(GL_TRIANGLES, 0, 36);	// 0, 36, 24  (or 100000)	

		// draw the pyramid using buffer #1
	//mMat = glm::translate(glm::mat4(1.0f), glm::vec3(pyrLocX, pyrLocY, pyrLocZ));
	//mvMat = vMat * mMat;


	//glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glEnableVertexAttribArray(0);


}
void window_reshape_callback(GLFWwindow * window,int newWidth,int newHeight) {
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