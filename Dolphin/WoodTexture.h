#pragma once
#include "PerlinTexture.h"

class WoodTexture : public PerlinTexture
{
private:
	double m_ringWidth;
	double m_ringFrequency;
	double m_distortionPower;
	glm::vec3 m_lightColor;
	glm::vec3 m_darkColor;
	void FillDataArray(GLubyte* data, double*** noise)
	{
		// fill byte array with RGBA
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
				{
					double xValue = (double(i) - m_size / 2.0) / double(m_size);
					double yValue = (double(j) - m_size / 2.0) / double(m_size);
					double distanceFromZ = sqrt(xValue * xValue + yValue * yValue) 
						+ m_distortionPower * Perlin(m_octaves, m_falloff, i, j, k, noise);

					double sineValue = abs(sin(distanceFromZ * 3.1415926 * m_ringFrequency));
					sineValue = pow(sineValue, m_ringWidth);

					float red = 255.0f * (sineValue * m_lightColor.r + (1.0 - sineValue) * m_darkColor.r);
					float green = 255.0f * (sineValue * m_lightColor.g + (1.0 - sineValue) * m_darkColor.g);
					float blue = 255.0f * (sineValue * m_lightColor.b + (1.0 - sineValue) * m_darkColor.b);

					data[0 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255);
					data[1 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(blue);
					data[2 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(green);
					data[3 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(red);
				}
	}
public:
	WoodTexture(GLuint size, GLuint octaves, double falloff, double ringWidth, double ringFrequency,
		double distortionPower, glm::vec3 lightColor, glm::vec3 darkColor) :
		PerlinTexture(size, octaves, falloff), m_ringWidth(ringWidth), m_ringFrequency(ringFrequency),
		m_distortionPower(distortionPower), m_lightColor(lightColor), m_darkColor(darkColor)
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

