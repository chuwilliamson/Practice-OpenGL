#include "_03___Rendering_GeometryApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "gl_core_4_4.h"
#include <stdio.h>
#include <errno.h>

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

_03___Rendering_GeometryApp::_03___Rendering_GeometryApp()
{
}

_03___Rendering_GeometryApp::~_03___Rendering_GeometryApp()
{
}

bool _03___Rendering_GeometryApp::startup()
{
	setBackgroundColour(0.25f, 0.25f, 0.25f);

	assert(generateGrid(5, 5) == true);
	generateShader();
	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);

	return true;
}

void _03___Rendering_GeometryApp::shutdown()
{
	Gizmos::destroy();
}
float totalTime = 0;
void _03___Rendering_GeometryApp::update(float deltaTime)
{
	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	// update perspective based on screen size
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.0f);
	m_projectionViewMatrix = m_projectionMatrix * m_viewMatrix;
	totalTime += deltaTime;
}

void _03___Rendering_GeometryApp::draw()
{
	// wipe the screen to the background colour
	clearScreen();


	glUseProgram(m_programID);
	unsigned int mvpUniform = glGetUniformLocation(m_programID, "mvp");
	unsigned int timeUniform = glGetUniformLocation(m_programID, "time");

	glUniformMatrix4fv(mvpUniform, 1, false, glm::value_ptr(m_projectionViewMatrix));
	glUniform1f(timeUniform, totalTime);
	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

bool _03___Rendering_GeometryApp::generateGrid(unsigned int rows, unsigned int cols)
{
	Vertex* vertices = new Vertex[4];
	vertices[0] = { vec4(-5, 0, 0, 1), vec4(1, 0, 0, 1) };
	vertices[1] = { vec4(5, 0, 0, 1), vec4(0, 1, 0, 1) };
	vertices[2] = { vec4(-5, 5, 0, 1), vec4(0, 0, 1, 1) };
	vertices[3] = { vec4(5, 5, 0, 1), vec4(1, 1, 1, 1) };

	unsigned int indices[6] = { 0, 1, 2, 2, 1, 3 };
	// comments on post vs pre increment;
	// ++i; Fetch i, increment it, and return it
	// i++; Fetch i, copy it, increment i, return copy

	// CREATE VERTEX ARRAYS
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// CREATE BUFFER OBJECTS
	glGenBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), vertices, GL_STATIC_DRAW);

	// How is the data layout in memory?
	// |-----position----|----colour----|
	glEnableVertexAttribArray(0);//position	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(1);//colour
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// UNBIND
	glBindVertexArray(0);
	return true;
}

bool GetShader(const char* filename, char* &buffer)
{	
	

	FILE* fp;

	//uses the current working directory which is in $(SolutionDir)bin
	
	fopen_s(&fp, filename, "r");
	if (fp == nullptr)
		perror("Error opening file");
	//put the position to the end
	fseek(fp, 0, SEEK_END);
	//get the size of the file
	size_t size = ftell(fp);	
	buffer = new char[size + 1];
	//rewind to beginning 
	fseek(fp, 0, SEEK_SET);
	
	size_t numitems = fread(buffer, sizeof(char), size + 1, fp);	
	buffer[numitems] = '\0';
	
	//no idea why this won't terminate
	//reading to buffer with a memory size of size + 1
	//number of items is the size of the stream in bytes which is the number of characters
	//char is 1 byte
	
	//size_t numitems = fread_s(buffer, size + 1, 1, size, fp);
	//buffer[numitems] = '\0';
	//aie code is wrong?
	fclose(fp);	
	
	
	return true;
}

bool _03___Rendering_GeometryApp::generateShader()
{
	char* vsSource;
	char* fsSource;
	GetShader("shaders/vertex.vert", vsSource);
	GetShader("shaders/fragment.frag", fsSource);


	int success = GL_FALSE;
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vertexShader, 1, (const char**)&vsSource, 0);
	glCompileShader(vertexShader);
	glShaderSource(fragmentShader, 1, (const char**)&fsSource, 0);
	glCompileShader(fragmentShader);

	m_programID = glCreateProgram();
	glAttachShader(m_programID, vertexShader);
	glAttachShader(m_programID, fragmentShader);
	glLinkProgram(m_programID);

	glGetProgramiv(m_programID, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		int infoLogLength = 0;
		glGetProgramiv(m_programID, GL_INFO_LOG_LENGTH, &infoLogLength);
		char* infoLog = new char[infoLogLength];

		glGetProgramInfoLog(m_programID, infoLogLength, 0, infoLog);
		printf("Error: Failed to link shader program! \n");
		printf("%s\n", infoLog);
		delete[] infoLog;
	}

	glDeleteShader(fragmentShader);
	glDeleteShader(vertexShader);


	return true;
}
