/*
 * GLUS - Modern OpenGL, OpenGL ES and OpenVG Utilities. Copyright (C) since 2010 Norbert Nopper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GL/glus.h"

static GLUSfloat glusPerlinGetInterpolatedValue(const GLUSfloat value0, const GLUSfloat value1, const GLUSfloat t)
{
	GLUSfloat ft;

	ft = (1.0f - cosf(t * GLUS_PI)) / 2.0f;

	return value0 * (1.0f - ft) + value1 * ft;
}

static GLUSfloat glusPerlinGetNoiseValue1D(GLUStgaimage* image, GLUSint x, const GLUSint amplitude, const GLUSint* data1D)
{
	if (!image || !data1D)
	{
		return 0.0f;
	}

	x = x % (GLUSint)image->width;

	if (x < 0)
	{
		x = (GLUSint)image->width + x;
	}

	return (GLUSfloat)(data1D[x] % amplitude);
}

static GLUSfloat glusPerlinGetNoiseValue2D(GLUStgaimage* image, GLUSint x, GLUSint y, const GLUSint amplitude, const GLUSint* data2D)
{
	if (!image || !data2D)
	{
		return 0.0f;
	}

	x = x % (GLUSint)image->width;
	y = y % (GLUSint)image->height;

	if (x < 0)
	{
		x = (GLUSint)image->width + x;
	}
	if (y < 0)
	{
		y = (GLUSint)image->height + y;
	}

	return (GLUSfloat)(data2D[x + y * (GLUSint)image->width] % amplitude);
}

static GLUSfloat glusPerlinGetNoiseValue3D(GLUStgaimage* image, GLUSint x, GLUSint y, GLUSint z, const GLUSint amplitude, const GLUSint* data3D)
{
	if (!image || !data3D)
	{
		return 0.0f;
	}

	x = x % (GLUSint)image->width;
	y = y % (GLUSint)image->height;
	z = z % (GLUSint)image->depth;

	if (x < 0)
	{
		x = (GLUSint)image->width + x;
	}
	if (y < 0)
	{
		y = (GLUSint)image->height + y;
	}
	if (z < 0)
	{
		z = (GLUSint)image->depth + z;
	}

	return (GLUSfloat)(data3D[x + y * (GLUSint)image->width + z * (GLUSint)image->height * (GLUSint)image->width] % amplitude);
}

GLUSboolean GLUSAPIENTRY glusPerlinCreateNoise1D(GLUStgaimage* image, const GLUSint width, const GLUSint seed, const GLUSfloat frequency, const GLUSfloat amplitude, const GLUSfloat persistence, const GLUSint octaves)
{
	GLUSint i;

	GLUSfloat* data;
	GLUSint* data1D;

	GLUSfloat frequencyFactor;
	GLUSfloat amplitudeFactor;

	if (!image)
	{
		return GLUS_FALSE;
	}

	if (persistence <= 0.0f || width < 1)
	{
		return GLUS_FALSE;
	}

	image->width = (GLUSushort)width;
	image->height = 1;
	image->depth = 1;
	image->format = GLUS_SINGLE_CHANNEL;
	image->data = glusMemoryMalloc(width * sizeof(GLUSubyte));

	if (!image->data)
	{
		glusImageDestroyTga(image);

		return GLUS_FALSE;
	}

	data = glusMemoryMalloc(width * sizeof(GLUSfloat));

	if (!data)
	{
		glusImageDestroyTga(image);

		return GLUS_FALSE;
	}

	for (i = 0; i < width; i++)
	{
		data[i] = 0.0f;
	}

	//

	data1D = glusMemoryMalloc(width * sizeof(GLUSint));

	if (!data1D)
	{
		glusImageDestroyTga(image);

		glusMemoryFree(data);

		return GLUS_FALSE;
	}

	srand(seed);
	for (i = 0; i < width; i++)
	{
		data1D[i] = rand();
	}

	//

	frequencyFactor = 1.0f;
	amplitudeFactor = 1.0f / persistence;

	for (i = 0; i < octaves; i++)
	{
		GLUSint x;

		GLUSint waveLenghtX;

		GLUSfloat currentFrequency = frequency * frequencyFactor;
		GLUSfloat currentAmplitude = amplitude / (1.0f * amplitudeFactor);

		if (currentFrequency <= 1.0f)
		{
			currentFrequency = 1.0f;
		}

		waveLenghtX = width / (GLUSint)currentFrequency;

		if (waveLenghtX < 1)
		{
			waveLenghtX = 1;
		}

		// Render per wavelength
		for (x = 0; x < width; x += waveLenghtX)
		{
			GLUSint xInner;

			GLUSfloat p[2];

			GLUSint xRandom = x / waveLenghtX;

			// Get random values from adjacent points
			p[0] = glusPerlinGetNoiseValue1D(image, xRandom, (GLUSint)currentAmplitude, data1D);
			p[1] = glusPerlinGetNoiseValue1D(image, xRandom + 1, (GLUSint)currentAmplitude, data1D);

			// Sample the points in the wavelength area
			for (xInner = 0; xInner < waveLenghtX && x + xInner < width; xInner++)
			{
				GLUSint index = x + xInner;

				data[index] += glusPerlinGetInterpolatedValue(p[0], p[1], (GLUSfloat)xInner / (GLUSfloat)waveLenghtX);
			}
		}

		frequencyFactor *= 2.0f;
		amplitudeFactor *= 1.0f / persistence;
	}

	glusMemoryFree(data1D);

	for (i = 0; i < width; i++)
	{
		image->data[i] = (GLUSubyte)data[i];
	}

	glusMemoryFree(data);

	return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusPerlinCreateNoise2D(GLUStgaimage* image, const GLUSint width, const GLUSint height, const GLUSint seed, const GLUSfloat frequency, const GLUSfloat amplitude, const GLUSfloat persistence, const GLUSint octaves)
{
	GLUSint i;

	GLUSfloat* data;
	GLUSint* data2D;

	GLUSfloat frequencyFactor;
	GLUSfloat amplitudeFactor;

	if (!image)
	{
		return GLUS_FALSE;
	}

	if (persistence <= 0.0f || width < 1 || height < 1)
	{
		return GLUS_FALSE;
	}

	image->width = (GLUSushort)width;
	image->height = (GLUSushort)height;
	image->depth = 1;
	image->format = GLUS_SINGLE_CHANNEL;
	image->data = glusMemoryMalloc(width * height * sizeof(GLUSubyte));

	if (!image->data)
	{
		glusImageDestroyTga(image);

		return GLUS_FALSE;
	}

	data = glusMemoryMalloc(width * height * sizeof(GLUSfloat));

	if (!data)
	{
		glusImageDestroyTga(image);

		return GLUS_FALSE;
	}

	for (i = 0; i < width * height; i++)
	{
		data[i] = 0.0f;
	}

	//

	data2D = glusMemoryMalloc(width * height * sizeof(GLUSint));

	if (!data2D)
	{
		glusImageDestroyTga(image);

		glusMemoryFree(data);

		return GLUS_FALSE;
	}

	srand(seed);
	for (i = 0; i < width * height; i++)
	{
		data2D[i] = rand();
	}

	//

	frequencyFactor = 1.0f;
	amplitudeFactor = 1.0f / persistence;

	for (i = 0; i < octaves; i++)
	{
		GLUSint x, y;

		GLUSint waveLenghtX;
		GLUSint waveLenghtY;

		GLUSfloat currentFrequency = frequency * frequencyFactor;
		GLUSfloat currentAmplitude = amplitude / (1.0f * amplitudeFactor);

		if (currentFrequency <= 1.0f)
		{
			currentFrequency = 1.0f;
		}

		waveLenghtX = width / (GLUSint)currentFrequency;
		waveLenghtY = height / (GLUSint)currentFrequency;

		if (waveLenghtX < 1)
		{
			waveLenghtX = 1;
		}
		if (waveLenghtY < 1)
		{
			waveLenghtY = 1;
		}

		// Render per wavelength
		for (y = 0; y < height; y += waveLenghtY)
		{
			for (x = 0; x < width; x += waveLenghtX)
			{
				GLUSint xInner, yInner;

				GLUSfloat p[4];

				GLUSint xRandom = x / waveLenghtX;
				GLUSint yRandom = y / waveLenghtY;

				// Get random values from adjacent points
				p[0] = glusPerlinGetNoiseValue2D(image, xRandom, yRandom, (GLUSint)currentAmplitude, data2D);
				p[1] = glusPerlinGetNoiseValue2D(image, xRandom + 1, yRandom, (GLUSint)currentAmplitude, data2D);
				p[2] = glusPerlinGetNoiseValue2D(image, xRandom, yRandom + 1, (GLUSint)currentAmplitude, data2D);
				p[3] = glusPerlinGetNoiseValue2D(image, xRandom + 1, yRandom + 1, (GLUSint)currentAmplitude, data2D);

				// Sample the points in the wavelength area
				for (yInner = 0; yInner < waveLenghtY && y + yInner < height; yInner++)
				{
					for (xInner = 0; xInner < waveLenghtX && x + xInner < width; xInner++)
					{
						GLUSint index = (y + yInner) * width + x + xInner;

						GLUSfloat x0 = glusPerlinGetInterpolatedValue(p[0], p[1], (GLUSfloat)xInner / (GLUSfloat)waveLenghtX);
						GLUSfloat x1 = glusPerlinGetInterpolatedValue(p[2], p[3], (GLUSfloat)xInner / (GLUSfloat)waveLenghtX);

						data[index] += glusPerlinGetInterpolatedValue(x0, x1, (GLUSfloat)yInner / (GLUSfloat)waveLenghtY);
					}
				}
			}
		}

		frequencyFactor *= 2.0f;
		amplitudeFactor *= 1.0f / persistence;
	}

	glusMemoryFree(data2D);

	for (i = 0; i < width * height; i++)
	{
		image->data[i] = (GLUSubyte)data[i];
	}

	glusMemoryFree(data);

	return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusPerlinCreateNoise3D(GLUStgaimage* image, const GLUSint width, const GLUSint height, const GLUSint depth, const GLUSint seed, const GLUSfloat frequency, const GLUSfloat amplitude, const GLUSfloat persistence, const GLUSint octaves)
{
	GLUSint i;

	GLUSfloat* data;
	GLUSint* data3D;

	GLUSfloat frequencyFactor;
	GLUSfloat amplitudeFactor;

	if (!image)
	{
		return GLUS_FALSE;
	}

	if (persistence <= 0.0f || width < 1 || height < 1 || depth < 1)
	{
		return GLUS_FALSE;
	}

	image->width = (GLUSushort)width;
	image->height = (GLUSushort)height;
	image->depth = (GLUSushort)depth;
	image->format = GLUS_SINGLE_CHANNEL;
	image->data = glusMemoryMalloc(width * height * depth * sizeof(GLUSubyte));

	if (!image->data)
	{
		glusImageDestroyTga(image);

		return GLUS_FALSE;
	}

	data = glusMemoryMalloc(width * height * depth * sizeof(GLUSfloat));

	if (!data)
	{
		glusImageDestroyTga(image);

		return GLUS_FALSE;
	}

	for (i = 0; i < width * height * depth; i++)
	{
		data[i] = 0.0f;
	}

	//

	data3D = glusMemoryMalloc(width * height * depth * sizeof(GLUSint));

	if (!data3D)
	{
		glusImageDestroyTga(image);

		glusMemoryFree(data);

		return GLUS_FALSE;
	}

	srand(seed);
	for (i = 0; i < width * height * depth; i++)
	{
		data3D[i] = rand();
	}

	//

	frequencyFactor = 1.0f;
	amplitudeFactor = 1.0f / persistence;

	for (i = 0; i < octaves; i++)
	{
		GLUSint x, y, z;

		GLUSint waveLenghtX;
		GLUSint waveLenghtY;
		GLUSint waveLenghtZ;

		GLUSfloat currentFrequency = frequency * frequencyFactor;
		GLUSfloat currentAmplitude = amplitude / (1.0f * amplitudeFactor);

		if (currentFrequency <= 1.0f)
		{
			currentFrequency = 1.0f;
		}

		waveLenghtX = width / (GLUSint)currentFrequency;
		waveLenghtY = height / (GLUSint)currentFrequency;
		waveLenghtZ = depth / (GLUSint)currentFrequency;

		if (waveLenghtX < 1)
		{
			waveLenghtX = 1;
		}
		if (waveLenghtY < 1)
		{
			waveLenghtY = 1;
		}
		if (waveLenghtZ < 1)
		{
			waveLenghtZ = 1;
		}

		// Render per wavelength
		for (z = 0; z < depth; z += waveLenghtZ)
		{
			for (y = 0; y < height; y += waveLenghtY)
			{
				for (x = 0; x < width; x += waveLenghtX)
				{
					GLUSint xInner, yInner, zInner;

					GLUSfloat p[8];

					GLUSint xRandom = x / waveLenghtX;
					GLUSint yRandom = y / waveLenghtY;
					GLUSint zRandom = z / waveLenghtZ;

					// Get random values from adjacent points
					p[0] = glusPerlinGetNoiseValue3D(image, xRandom, yRandom, zRandom, (GLUSint)currentAmplitude, data3D);
					p[1] = glusPerlinGetNoiseValue3D(image, xRandom + 1, yRandom, zRandom, (GLUSint)currentAmplitude, data3D);
					p[2] = glusPerlinGetNoiseValue3D(image, xRandom, yRandom + 1, zRandom, (GLUSint)currentAmplitude, data3D);
					p[3] = glusPerlinGetNoiseValue3D(image, xRandom + 1, yRandom + 1, zRandom, (GLUSint)currentAmplitude, data3D);

					p[4] = glusPerlinGetNoiseValue3D(image, xRandom, yRandom, zRandom + 1, (GLUSint)currentAmplitude, data3D);
					p[5] = glusPerlinGetNoiseValue3D(image, xRandom + 1, yRandom, zRandom + 1, (GLUSint)currentAmplitude, data3D);
					p[6] = glusPerlinGetNoiseValue3D(image, xRandom, yRandom + 1, zRandom + 1, (GLUSint)currentAmplitude, data3D);
					p[7] = glusPerlinGetNoiseValue3D(image, xRandom + 1, yRandom + 1, zRandom + 1, (GLUSint)currentAmplitude, data3D);

					// Sample the points in the wavelength area
					for (zInner = 0; zInner < waveLenghtZ && z + zInner < depth; zInner++)
					{
						for (yInner = 0; yInner < waveLenghtY && y + yInner < height; yInner++)
						{
							for (xInner = 0; xInner < waveLenghtX && x + xInner < width; xInner++)
							{
								GLUSint index = (z + zInner) * height * width + (y + yInner) * width + x + xInner;

								GLUSfloat x0 = glusPerlinGetInterpolatedValue(p[0], p[1], (GLUSfloat)xInner / (GLUSfloat)waveLenghtX);
								GLUSfloat x1 = glusPerlinGetInterpolatedValue(p[2], p[3], (GLUSfloat)xInner / (GLUSfloat)waveLenghtX);
								GLUSfloat x2 = glusPerlinGetInterpolatedValue(p[4], p[5], (GLUSfloat)xInner / (GLUSfloat)waveLenghtX);
								GLUSfloat x3 = glusPerlinGetInterpolatedValue(p[6], p[7], (GLUSfloat)xInner / (GLUSfloat)waveLenghtX);

								GLUSfloat y0 = glusPerlinGetInterpolatedValue(x0, x1, (GLUSfloat)yInner / (GLUSfloat)waveLenghtY);
								GLUSfloat y1 = glusPerlinGetInterpolatedValue(x2, x3, (GLUSfloat)yInner / (GLUSfloat)waveLenghtY);

								data[index] += glusPerlinGetInterpolatedValue(y0, y1, (GLUSfloat)zInner / (GLUSfloat)waveLenghtZ);
							}
						}
					}
				}
			}
		}

		frequencyFactor *= 2.0f;
		amplitudeFactor *= 1.0f / persistence;
	}

	glusMemoryFree(data3D);

	for (i = 0; i < width * height * depth; i++)
	{
		image->data[i] = (GLUSubyte)data[i];
	}

	glusMemoryFree(data);

	return GLUS_TRUE;
}
