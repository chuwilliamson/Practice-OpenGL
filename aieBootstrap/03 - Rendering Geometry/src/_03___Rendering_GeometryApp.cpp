#include "_03___Rendering_GeometryApp.h"
#include "Gizmos.h"
#include "Input.h"
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include "gl_core_4_4.h"

using glm::vec3;
using glm::vec4;
using glm::mat4;
using aie::Gizmos;

_03___Rendering_GeometryApp::_03___Rendering_GeometryApp() {

}

_03___Rendering_GeometryApp::~_03___Rendering_GeometryApp() {

}

bool _03___Rendering_GeometryApp::startup() {

	setBackgroundColour(0, 0, 0);	

	assert(generateGrid(5, 5) == true);
	generateShader();
	// create simple camera transforms
	m_viewMatrix = glm::lookAt(vec3(10), vec3(0), vec3(0, 1, 0));
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, 16.0f / 9.0f, 0.1f, 1000.0f);

	return true;
}

void _03___Rendering_GeometryApp::shutdown() {

	Gizmos::destroy();
}

void _03___Rendering_GeometryApp::update(float deltaTime) 
{
	// quit if we press escape
	aie::Input* input = aie::Input::getInstance();

	if (input->isKeyDown(aie::INPUT_KEY_ESCAPE))
		quit();

	// update perspective based on screen size
	m_projectionMatrix = glm::perspective(glm::pi<float>() * 0.25f, getWindowWidth() / (float)getWindowHeight(), 0.1f, 1000.0f);
	m_projectionViewMatrix = m_projectionMatrix * m_viewMatrix;
}

void _03___Rendering_GeometryApp::draw() {

	// wipe the screen to the background colour
	clearScreen();
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glUseProgram(m_programID);
	unsigned int mvpUniform = glGetUniformLocation(m_programID, "mvp");
	
	glUniformMatrix4fv(mvpUniform, 1, false, glm::value_ptr(m_projectionViewMatrix));
	glBindVertexArray(m_VAO);

	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

}

bool _03___Rendering_GeometryApp::generateGrid(unsigned int rows, unsigned int cols)
{

	Vertex* aoVertices = new Vertex[rows * cols];
	Vertex* triangleVertices = new Vertex[3];
	triangleVertices[0] = { vec4(0), vec4(1,1,1,1) };
	triangleVertices[1] = { vec4(0,10,0,0), vec4(1,1,1,1) };
	triangleVertices[2] = { vec4(10,0,0,0), vec4(1,0,0,1) };

	// comments on post vs pre increment;
	// ++i; Fetch i, increment it, and return it
	// i++; Fetch i, copy it, increment i, return copy
	for (unsigned int r = 0; r < rows; ++r) {
		for (unsigned int c = 0; c < cols; ++c) {
			unsigned int currentIndex = r * cols + c;
			aoVertices[currentIndex] =
			{
				vec4((float)c, 0, (float)r, 1),
				vec4((float)c*(float)c, 0, (float)r*(float)r, 1)
			};
		}
	}
	// CREATE BUFFER OBJECTS
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IBO);

	// CREATE VERTEX ARRAYS
	glGenVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);	
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(Vertex), triangleVertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);//position
	glEnableVertexAttribArray(1);//colour
	// How is the data layout in memory?
	// |-----position----|----colour----|
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)sizeof(vec4));
	

	unsigned int* triangleIndices = new unsigned int[3];
	triangleIndices[0] = 0;
	triangleIndices[1] = 1;
	triangleIndices[2] = 2;

	glGenBuffers(1, &m_IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(unsigned int), triangleIndices, GL_STATIC_DRAW);

	// UNBIND
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	return true;
}

bool _03___Rendering_GeometryApp::generateShader()
{
	const char* vsSource = "#version 410\n \
							layout(location=0) in vec4 position; \
							layout(location=1) in vec4 colour; \
							out vec4 vColour; \
							uniform mat4 mvp; \
							void main() { vColour = colour; gl_Position = mvp * position;}";

	const char* fsSource = "#version 410\n \
							in vec4 vColour; \
							out vec4 fragColor; \
							void main() {fragColor = vColour;}";
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
