#include "EBO.h"

EBO::EBO(std::vector<GLuint>& indices) 
{
	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
}
EBO::EBO(GLuint* indices, GLsizeiptr size) 
{
	glGenBuffers(1, &m_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, indices, GL_STATIC_DRAW);
}
GLuint EBO::id() const
{
	return m_id;
}
void EBO::Bind() 
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_id);
}
void EBO::Unbind() 
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
void EBO::Delete()
{
	glDeleteBuffers(1, &m_id);
}