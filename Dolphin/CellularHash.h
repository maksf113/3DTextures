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

struct noiseStr
{
	double value;
	double dist1;
	double dist2;
	glm::vec3 color;
	int closestID;
	int secondClosestID;
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
	void CalculateNoise(noiseStr*** noise, const std::vector<point>& randomPoints)
	{
		float dist = 0.0;
		float dist2 = 0.0;
		float temp = 0.0;
		int pointIndex = 0;
		int secondIndex = 0;
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
				{
					dist = glm::length(glm::vec3(float(i) + 0.5f, float(j) + 0.5f, float(k) + 0.5f) - randomPoints[0].position);
					dist2 = glm::length(glm::vec3(float(i) + 0.5f, float(j) + 0.5f, float(k) + 0.5f) - randomPoints[0].position);
					pointIndex= 0;
					secondIndex = 0;
					for (int m = 1; m < m_numPoints; m++)
					{
						temp = glm::length(glm::vec3(float(i) + 0.5f, float(j) + 0.5f, float(k) + 0.5f) - randomPoints[m].position);
						if (temp < dist)
						{
							dist = temp; 
							pointIndex = m;
						}
						if (temp < dist2 && temp > dist)
						{
							dist2 = temp;
							secondIndex = m;
						}
					}
					
					noise[i][j][k].dist1 = dist;
					noise[i][j][k].dist2 = dist2;
					noise[i][j][k].value = dist2; // + dist2;
					double factor = dist / (dist + dist2);
					glm::vec3 col1 = randomPoints[pointIndex].color;
					col1 *= factor;
					glm::vec3 col2 = randomPoints[secondIndex].color;
					col2 *= (1.0 - factor);
					noise[i][j][k].color = col1 + col2;
					noise[i][j][k].closestID = pointIndex;
					noise[i][j][k].secondClosestID = secondIndex;
				}
	}
	void NormalizeNoise(noiseStr*** noise)
	{
		float min = noise[0][0][0].value;
		float max = noise[0][0][0].value;
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
				{
					if (min > noise[i][j][k].value)
						min = noise[i][j][k].value;
					if (max < noise[i][j][k].value)
						max = noise[i][j][k].value;
				}
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
					noise[i][j][k].value = (noise[i][j][k].value - min) / (max - min);
	}
	void GetMinMax(noiseStr*** noise, double& min, double& max)
	{
		//double noiseVal = noise[i][j][k] - int(noise[0][0][0]) % m_size;
		min = int(noise[0][0][0].value) % m_size;
		max = noise[0][0][0].value;
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
				{
					if (min > noise[i][j][k].value)
						min = noise[i][j][k].value;
					if (max < noise[i][j][k].value)
						max = noise[i][j][k].value;
				}
	}
	void DeleteNoise(noiseStr*** noise)
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
	virtual void FillDataArray(GLubyte* data, noiseStr*** noise)
	{
		// fill byte array with RGBA
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
				{
					data[0 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255);
					data[1 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255 * noise[i][j][k].value);// * noise[i][j][k].color.r);
					data[2 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255 * noise[i][j][k].value);// * noise[i][j][k].color.g);
					data[3 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255 * noise[i][j][k].value);// * noise[i][j][k].color.b);
				}																			   
	}
	void GenRandomPoint(point& point)
	{
		point.position.x = float(rand()) / float(RAND_MAX) * float(m_size);
		point.position.y = float(rand()) / float(RAND_MAX) * float(m_size);
		point.position.z = float(rand()) / float(RAND_MAX) * float(m_size);
		point.color.r = float(rand()) / float(RAND_MAX);
		point.color.g = float(rand()) / float(RAND_MAX);
		point.color.b = float(rand()) / float(RAND_MAX);
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

		noiseStr*** noise = new noiseStr** [m_size];
		for (int i = 0; i < m_size; i++)
		{
			noise[i] = new noiseStr* [m_size];
			for (int j = 0; j < m_size; j++)
				noise[i][j] = new noiseStr[m_size];
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