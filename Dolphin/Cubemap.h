#pragma once
#include "FBO.h"
#include "Camera.h"

class Cubemap
{
private:
	FBO m_fbo;
	GLenum m_texType;
	glm::vec3 m_position;
	Camera m_camera;
public:
	Cubemap(GLenum TexType, GLuint size, glm::vec3 Position) : m_position(Position), m_texType(TexType)
	{
		m_fbo.GenFramebuffer();
		m_fbo.SetWidth(size);
		m_fbo.SetHeight(size);
		m_fbo.GenTexture();
		if (!m_fbo.AttachCubemapFaces(m_texType))
			std::cerr << "Cubemap error: faces not attached" << std::endl;
	}
	void GenRBO()
	{
		m_fbo.GenRBO();
	}
	void DisableColor()
	{
		m_fbo.DisableColor();
	}
	void SetFaceToDraw(int faceIndex)
	{
		SwitchToFace(faceIndex);
		m_camera.SetPos(m_position);
		m_camera.updateMatrix(90.0f, 0.2f, 15.0f);
		m_fbo.Bind();
		m_fbo.Viewport();
		glFramebufferTexture2D(GL_FRAMEBUFFER, m_texType,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + (GLenum)faceIndex, m_fbo.GetTexID(), 0);
	}
	void SwitchToFace(int faceIndex)
	{
		switch (faceIndex)
		{
		case 0:
			m_camera.SetDir(glm::vec3(1.0f, 0.0f, 0.0f));
			m_camera.SetUp(glm::vec3(0.0f, -1.0f, 0.0f));
			break;
		case 1:
			m_camera.SetDir(glm::vec3(-1.0f, 0.0f, 0.0f));
			m_camera.SetUp(glm::vec3(0.0f, -1.0f, 0.0f));
			break;
		case 2:
			m_camera.SetDir(glm::vec3(0.0f, 1.0f, 0.0f));
			m_camera.SetUp(glm::vec3(0.0f, 0.0f, 1.0f));
			break;
		case 3:
			m_camera.SetDir(glm::vec3(0.0f, -1.0f, 0.0f));
			m_camera.SetUp(glm::vec3(0.0f, 0.0f, -1.0f));
			break;
		case 4:
			m_camera.SetDir(glm::vec3(0.0f, 0.0f, 1.0f));
			m_camera.SetUp(glm::vec3(0.0f, -1.0f, 0.0f));
			break;
		case 5:
			m_camera.SetDir(glm::vec3(0.0f, 0.0f, -1.0f));
			m_camera.SetUp(glm::vec3(0.0f, -1.0f, 0.0f));
			break;
		}
	}
	GLuint GetTexID() { return m_fbo.GetTexID(); }
	void Bind() { m_fbo.Bind(); }
	const Camera& GetCamera() const { return m_camera; }
};
