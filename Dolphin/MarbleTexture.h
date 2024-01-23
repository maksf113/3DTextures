#pragma once
#include "PerlinTexture.h"

class MarbleTexture : public PerlinTexture
{
private:
	double m_veinWidth;
	double m_veinFrequency;
	double m_distortionPower;
	glm::vec3 m_baseColor;
	glm::vec3 m_veinColor;
	void FillDataArray(GLubyte* data, double*** noise)
	{
		// fill byte array with RGBA
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
				{
					double xyzValue = double(i) / m_size + double(j) / m_size + double(k) / m_size
						+ m_distortionPower * Perlin(m_octaves, m_falloff, i, j, k, noise);

					double sineValue = abs(sin(xyzValue * 3.1415926 * m_veinFrequency));
					sineValue = pow(sineValue, m_veinWidth);

					float red = 255.0f * (sineValue * m_baseColor.r + (1.0 - sineValue) * m_veinColor.r);
					float green = 255.0f * (sineValue * m_baseColor.g + (1.0 - sineValue) * m_veinColor.g);
					float blue = 255.0f * (sineValue * m_baseColor.b + (1.0 - sineValue) * m_veinColor.b);

					data[0 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255);
					data[1 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(blue);
					data[2 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(green);
					data[3 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(red);
				}
	}
public:
	MarbleTexture(GLuint size, GLuint octaves, double falloff, double veinWidth, double veinFrequency,
		double distortionPower, glm::vec3 baseColor, glm::vec3 veinColor) :
		PerlinTexture(size, octaves, falloff), m_veinWidth(veinWidth), m_veinFrequency(veinFrequency),
		m_distortionPower(distortionPower), m_baseColor(baseColor), m_veinColor(veinColor)
	{
		double*** noise = new double** [m_size];
		for (int i = 0; i < m_size; i++)
		{
			noise[i] = new double* [m_size];
			for (int j = 0; j < m_size; j++)
				noise[i][j] = new double[m_size];
		}

		GenNoise(noise);
		GLubyte* data = new GLubyte[4 * m_size * m_size * m_size];
		FillDataArray(data, noise);
		GenTexture();
		LoadTexture(data);
		DeleteNoise(noise);
		delete[] data;
	}
};
