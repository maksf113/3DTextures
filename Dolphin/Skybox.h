#pragma once
#include "Mesh.h"

class Skybox
{
private:
	GLuint m_vao;
	GLuint m_vbo;
	GLuint m_ebo;
	GLuint m_cubemapTexture;
	GLuint m_textureUnit;
	float Vertices[24] =
	{
		//   Coordinates
		-1.0f, -1.0f,  1.0f,//        7--------6
		 1.0f, -1.0f,  1.0f,//       /|       /|
		 1.0f, -1.0f, -1.0f,//      4--------5 |
		-1.0f, -1.0f, -1.0f,//      | |      | |
		-1.0f,  1.0f,  1.0f,//      | 3------|-2
		 1.0f,  1.0f,  1.0f,//      |/       |/
		 1.0f,  1.0f, -1.0f,//      0--------1
		-1.0f,  1.0f, -1.0f
	};

	unsigned int Indices[36] =
	{
		// Right
		1, 2, 6,
		6, 5, 1,
		// Left
		0, 4, 7,
		7, 3, 0,
		// Top
		4, 5, 6,
		6, 7, 4,
		// Bottom
		0, 3, 2,
		2, 1, 0,
		// Back
		0, 1, 5,
		5, 4, 0,
		// Front
		3, 7, 6,
		6, 2, 3
	};
public:
	Skybox(const char* path, GLuint slot) : m_textureUnit(slot)
	{
		glGenVertexArrays(1, &m_vao);
		glGenBuffers(1, &m_vbo);
		glGenBuffers(1, &m_ebo);
		glBindVertexArray(m_vao);
		glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), &Vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), &Indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		std::string facesCubemap[6] =
		{
			std::string(path) + "/right1.png",
			std::string(path) + "/left1.png",
			std::string(path) + "/top1.png",
			std::string(path) + "/bottom1.png",
			std::string(path) + "/front1.png",
			std::string(path) + "/back1.png"
		};

		glGenTextures(1, &m_cubemapTexture);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

		for (unsigned int i = 0; i < 6; i++)
		{
			int Width, Height, numColCh;
			stbi_set_flip_vertically_on_load(false);
			unsigned char* data = stbi_load(facesCubemap[i].c_str(), &Width, &Height, &numColCh, 0);
			GLenum format;
			if (numColCh == 1)
				format = GL_RED;
			else if (numColCh == 3)
				format = GL_RGB;
			else if (numColCh == 4)
				format = GL_RGBA;

			if (data)
				glTexImage2D
				(
					GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0,
					GL_RGB,
					Width,
					Height,
					0,
					format,
					GL_UNSIGNED_BYTE,
					data
				);
			else
				std::cout << "Failed to load image: " << facesCubemap[i] << std::endl;
			stbi_image_free(data);
		}
	}

	void Draw(const ShaderProgram& shader, const Camera& camera)
	{
		shader.Activate();
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		view = glm::mat4(glm::mat3(camera.GetView()));
		projection = camera.GetProjection();
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setInt("skybox", m_textureUnit);
		glActiveTexture(GL_TEXTURE0 + m_textureUnit);
		glDisable(GL_DEPTH_TEST);
		glBindVertexArray(m_vao);
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubemapTexture);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
		glEnable(GL_DEPTH_TEST);
	}

	const GLuint GetTexID() const { return m_cubemapTexture; }
	const GLuint GetTexUnit() const { return m_textureUnit; }
};
