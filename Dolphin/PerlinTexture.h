#pragma once
#include <gl/glew.h>
#include <GLFW/glfw3.h>
#include <random>

class PerlinTexture
{
private:
	GLuint m_id;
protected:
	GLuint m_size;
	GLuint m_octaves;
	double m_falloff;
	double RandomDouble(double min, double max)
	{
		std::random_device rd;  // Random device used to obtain a seed
		std::mt19937 gen(rd());  // Mersenne Twister engine seeded with rd()
		std::uniform_real_distribution<double> dis(min, max);

		return dis(gen);
	}
	void GenNoise(double*** noise)
	{	
		for (int i = 0; i < m_size; i++)
			for (int j = 0; j < m_size; j++)
				for (int k = 0; k < m_size; k++)
					noise[i][j][k] = (double)rand() / (double)(RAND_MAX + 1.0);
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

	double SmoothNoise(double zoom, double x1, double y1, double z1, double*** noise)
	{
		// percentage from current block to the previous one
		double fractX = x1 - int(x1);
		double fractY = y1 - int(y1);
		double fractZ = z1 - int(z1);

		// indices of neighbouring values with wrapping
		double x2 = x1 - 1.0;
		if (x2 < 0.0)
			x2 = round(m_size / zoom) - 1.0;
		double y2 = y1 - 1.0;
		if (y2 < 0.0)
			y2 = round(m_size / zoom) - 1.0;
		double z2 = z1 - 1.0;
		if (z2 < 0.0)
			z2 = round(m_size / zoom) - 1.0;

		// linear interpolation
		double value = 0.0;
		value += fractX * fractY * fractZ * noise[int(x1)][int(y1)][int(z1)];
		value += fractX * fractY * (1.0 - fractZ) * noise[int(x1)][int(y1)][int(z2)];
		value += fractX * (1.0 - fractY) * fractZ * noise[int(x1)][int(y2)][int(z1)];
		value += fractX * (1.0 - fractY) * (1.0 - fractZ) * noise[int(x1)][int(y2)][int(z2)];

		value += (1.0 - fractX) * fractY * fractZ * noise[int(x2)][int(y1)][int(z1)];
		value += (1.0 - fractX) * fractY * (1.0 - fractZ) * noise[int(x2)][int(y1)][int(z2)];
		value += (1.0 - fractX) * (1.0 - fractY) * fractZ * noise[int(x2)][int(y2)][int(z1)];
		value += (1.0 - fractX) * (1.0 - fractY) * (1.0 - fractZ) * noise[int(x2)][int(y2)][int(z2)];

		return value;
	}

	double Perlin(int octaves, double falloff, double x, double y, double z, double*** noise)
	{
		double sum = 0.0;
		double zoom = pow(2.0, octaves - 1);
		double fact = 1.0;
		while (zoom >= 1.0)
		{
			sum += SmoothNoise(zoom, x / zoom, y / zoom, z / zoom, noise) * fact;
			zoom /= 2.0;
			fact *= falloff;
		}
		sum /= (1.0 - fact * falloff) / (1.0 - falloff);
		return sum;
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
					data[1 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255 * Perlin(m_octaves, m_falloff, i, j, k, noise));
					data[2 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255 * Perlin(m_octaves, m_falloff, i, j, k, noise));
					data[3 + k * 4 + j * m_size * 4 + i * m_size * m_size * 4] = GLubyte(255 * Perlin(m_octaves, m_falloff, i, j, k, noise));
				}
	}
public:
	PerlinTexture() {}
	PerlinTexture(GLuint size, GLuint octaves, double falloff) : m_size(size), m_octaves(octaves), m_falloff(falloff)
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
	void Bind()
	{
		glBindTexture(GL_TEXTURE_3D, m_id);
	}
	virtual ~PerlinTexture()
	{
		glDeleteTextures(1, &m_id);
	}
};

double Logistic(double x, double k)
{
	return (1.0 / pow(2.718, -k * x));
}
