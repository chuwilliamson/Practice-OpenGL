#pragma once

#include "Application.h"
#include <glm/mat4x4.hpp>
#include <glm/vec2.hpp>

class _03___Rendering_GeometryApp : public aie::Application 
{

public:
	_03___Rendering_GeometryApp();
	virtual ~_03___Rendering_GeometryApp();

	virtual bool startup();
	virtual void shutdown();

	virtual void update(float deltaTime);
	virtual void draw();
	

protected:		
	// camera transforms
	glm::mat4	m_viewMatrix;
	glm::mat4	m_projectionMatrix;
	glm::mat4	m_projectionViewMatrix;
	glm::vec4	m_clearColour;
	glm::vec2   m_mousePos;
	bool        m_renderBox;
	

	struct Vertex
	{
		glm::vec4 position;
		glm::vec4 colour;
	};

	unsigned int m_programID;
	unsigned int m_VAO;
	unsigned int m_VBO;
	unsigned int m_IBO;
	bool generateGrid(unsigned int rows, unsigned int cols);
	bool generateShader();
};