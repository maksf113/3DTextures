#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <vector>
#include <random>
#include <iostream>

struct point
{
	glm::vec3 position;
	glm::vec3 color;
};

class CellularTexture
{
private:
	GLuint m_id;
protected:
	GLuint m_size;
	GLuint m_numPoints;
	GLuint m_cubeSize;
	double RandomDouble(double min, double max)
	{
		std::random_device rd;  // Random device used to obtain a seed
		std::mt19937 gen(rd());  // Mersenne Twister engine seeded with rd()
		std::uniform_real_distribution<double> dis(min, max);

		return dis(gen);
	}
	void CalculateNoise(double*** noise, const std::vector<point>& randomPoints)
	{
		float dist = 0.0;
		float temp = 0.0;
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
				{
					dist = glm::length(glm::vec3(float(i) + 0.5f, float(j) + 0.5f, float(k) + 0.5f) - randomPoints[0].position);
					for (int m = 1; m < m_numPoints; m++)
					{
						temp = glm::length(glm::vec3(float(i) + 0.5f, float(j) + 0.5f, float(k) + 0.5f) - randomPoints[m].position);
						if (temp < dist)
							dist = temp; // + m_size * (randomPoints[m].color.r + m_size * (randomPoints[m].color.g + m_size * randomPoints[m].color.b));
					}
					noise[i][j][k] = dist;
					//std::cout << dist << std::endl;
				}
	}
	void NormalizeNoise(double*** noise)
	{
		float min = noise[0][0][0];
		float max = noise[0][0][0];
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
				{
					if (min > noise[i][j][k])
						min = noise[i][j][k];
					if (max < noise[i][j][k])
						max = noise[i][j][k];
				}
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
					noise[i][j][k] = (noise[i][j][k] - min) / (max - min);
	}
	void GetMinMax(double*** noise, double& min, double& max)
	{
		//double noiseVal = noise[i][j][k] - int(noise[0][0][0]) % m_size;
		min = int(noise[0][0][0]) % m_size;
		max = noise[0][0][0];
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
				{
					if (min > noise[i][j][k])
						min = noise[i][j][k];
					if (max < noise[i][j][k])
						max = noise[i][j][k];
				}
	}
	void DeleteNoise(double*** noise)
	{
		for (int i = 0; i < m_size; i++)
		{
			for (int j = 0; j < m_size; j++)
				delete[] noise[i][j];
			delete[] noise[i];
		}
		delete[] noise;
	}
	void GenTexture()
	{
		glGenTextures(1, &m_id);
		glBindTexture(GL_TEXTURE_3D, m_id);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
		glTexStorage3D(GL_TEXTURE_3D, 1, GL_RGBA8, m_size, m_size, m_size);
	}
	void LoadTexture(GLubyte* data)
	{
		glBindTexture(GL_TEXTURE_3D, m_id);
		glTexSubImage3D(GL_TEXTURE_3D, 0,
			0, 0, 0, m_size, m_size, m_size,
			GL_RGBA, GL_UNSIGNED_INT_8_8_8_8, data);
	}
	virtual void FillDataArray(GLubyte* data, double*** noise)
	{
		// fill byte array with RGBA
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
				{
					data[0 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255);
					data[1 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255 * noise[i][j][k]);
					data[2 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255 * noise[i][j][k]);
					data[3 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255 * noise[i][j][k]);
				}
	}
	void GenRandomPoint(point& point)
	{
		point.position.x = float(rand()) / float(RAND_MAX) * float(m_size);
		point.position.y = float(rand()) / float(RAND_MAX) * float(m_size);
		point.position.z = float(rand()) / float(RAND_MAX) * float(m_size);
		point.color.x = float(rand()) / float(RAND_MAX) * float(m_size);
		point.color.y = float(rand()) / float(RAND_MAX) * float(m_size);
		point.color.z = float(rand()) / float(RAND_MAX) * float(m_size);
	}
public:
	CellularTexture() {}
	~CellularTexture()
	{
		glDeleteTextures(1, &m_id);
	}
	CellularTexture(GLuint size, GLuint numPoints) : m_size(size), m_numPoints(numPoints)
	{
		std::vector<point> randomPoints;
		randomPoints.resize(m_numPoints);
		for (int i = 0; i < m_numPoints; i++)
			 GenRandomPoint(randomPoints[i]);

		double*** noise = new double** [m_size];
		for (int i = 0; i < m_size; i++)
		{
			noise[i] = new double* [m_size];
			for (int j = 0; j < m_size; j++)
				noise[i][j] = new double[m_size];
		}
		CalculateNoise(noise, randomPoints);
		NormalizeNoise(noise);
		GLubyte* data = new GLubyte[4 * m_size * m_size * m_size];
		FillDataArray(data, noise);
		GenTexture();
		LoadTexture(data);
		DeleteNoise(noise);
		delete[] data;
	}
	void Bind()
	{
		glBindTexture(GL_TEXTURE_3D, m_id);
	}
};