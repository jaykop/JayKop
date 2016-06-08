#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include "GLApplication.h"
#include "Application.h"

static const GLfloat m_vertex_buffer_data[] =
{ -.5f, -.5f, 0.f,
-.5f, .5f, 0.f,
.5f, .5f, 0.f,
.5f, -.5f, 0.f
};

GLApplication::GLApplication(void)
{
	//Set projection matrix
	m_hdc = 0;
	m_hglrc = 0;
	m_info.m_width = 0;
	m_info.m_height = 0;
	m_info.m_zNear = 0.1f;
	m_info.m_zFar = 100.f;
	m_info.m_fovy = 45.f;
}

GLApplication::~GLApplication(void)
{

}

void GLApplication::Resize(int width, int height)
{
	//Resize screen size

	//Save the data
	m_info.m_width = static_cast<float>(width);
	m_info.m_height = static_cast<float>(height);

	//Prevent the case height = 0
	if (m_info.m_height <= 0) m_info.m_height = 1;

	//Get ratio
	float aspectRatio = (m_info.m_width / m_info.m_height);

	//Temporary stuff
	glViewport(0, 0, GLsizei(m_info.m_width), GLsizei(m_info.m_height));
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspectRatio, 0.1f, 100.f);
	//glOrtho(-m_width / 2.f, m_width / 2.f, -m_height / 2.f, m_height / 2.f, 0.1f, 100.f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GLApplication::SysShutdown(void)
{
	//release current device context
	wglMakeCurrent(m_hdc, NULL);

	//delete rendering context
	wglDeleteContext(m_hglrc);

	//make sure the window will be destroyed
	PostQuitMessage(0);
}

void GLApplication::SetGLFormat(void)
{
	//number of available format
	m_indexPixelFormat = 0;
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
		32,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		16,
		0, 0, 0, 0, 0, 0, 0
	};

	//Choose the closest pixel format abailable
	m_indexPixelFormat = ChoosePixelFormat(m_hdc, &pfd);
	
	//Set the pixel format for the provided window DC
	SetPixelFormat(m_hdc, m_indexPixelFormat, &pfd);
}

void GLApplication::CheckGL(HWND& window)
{
	//Get wnd DC
	m_hdc = GetDC(window);
	if (!m_hdc)
	{
		MessageBox(window, "Failed to get the wnd dc",
			"Device Context Error", MB_OK);

		SysShutdown();
	}

	//Select pixel format desc
	SetGLFormat();

	//Create the OpenGL rendering context
	m_hglrc = wglCreateContext(m_hdc);
	if (!m_hglrc)
	{
		MessageBox(window, "Failed to Create the OpenGL Rendering Context",
			"OpenGL Rendering Context Error", MB_OK);

		SysShutdown();
	}

	//Make the OpenGL Rendering context current rc
	if (!wglMakeCurrent(m_hdc, m_hglrc))
	{
		MessageBox(window, "Failed to make OpenGL Rendering Context current",
			"OpenGL Rendering Context Error", MB_OK);

		SysShutdown();
	}
}

bool GLApplication::InitGL(void)
{
	//InitGL
	glShadeModel(GL_SMOOTH);
	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthFunc(GL_LEQUAL);

	return true;
}

void GLApplication::OpenGLInit(HWND& window, int width, int height)
{
	CheckGL(window);

	//InitGL
	if (InitGL())
	{
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK)
		{
			MessageBox(window, "Failed to initialize GLEW",
				"OpenGL GLEW Error", MB_OK);
		}
	}

	Resize(width, height);

	//The VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	//This will be our vertex buffer
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertex_buffer_data), m_vertex_buffer_data, GL_STATIC_DRAW);

	//Call shader
	m_shader.LoadShaders("shader.vertex", "shader.fragment");

	//Use shader
	glUseProgram(m_shader.m_programID);

}

HDC GLApplication::GetHDC(void) const
{
	return m_hdc;
}

ProjectionInfo GLApplication::GetProjectionInfo(void) const
{
	return m_info;
}

Shader GLApplication::GetShader(void) const
{
	return m_shader;
}

GLuint GLApplication::GetMatrixID(void) const
{
	return m_matrixID;
}

GLuint GLApplication::GetVertexBuffer(void) const
{
	return m_vertexBuffer;
}