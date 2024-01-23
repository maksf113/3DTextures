#include "Mesh.h"

template<>
void Mesh<ClothSimVertex>::CreateGLState(std::vector<ClothSimVertex>& vertices, std::vector<GLuint>& indices)
{
	m_vao.Bind();
	VBO<ClothSimVertex> vbo(vertices);
	EBO ebo(indices);
	// link position, previous position, force and texUVs
	m_vao.linkAttrib(vbo, 0, 3, GL_FLOAT, sizeof(ClothSimVertex), (void*)0);
	m_vao.linkAttrib(vbo, 1, 3, GL_FLOAT, sizeof(ClothSimVertex), (void*)(3 * sizeof(float)));
	m_vao.linkAttrib(vbo, 2, 3, GL_FLOAT, sizeof(ClothSimVertex), (void*)(6 * sizeof(float)));
	m_vao.linkAttrib(vbo, 3, 2, GL_FLOAT, sizeof(ClothSimVertex), (void*)(9 * sizeof(float)));

	m_vao.Unbind();
	vbo.Unbind();
	ebo.Unbind();
}