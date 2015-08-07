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

static GLUSvoid glusShapeInitf(GLUSshape* shape)
{
    if (!shape)
    {
        return;
    }

    memset(shape, 0, sizeof(GLUSshape));

    shape->mode = GLUS_TRIANGLES;
}

static GLUSboolean glusShapeCheckf(GLUSshape* shape)
{
    if (!shape)
    {
        return GLUS_FALSE;
    }

    return shape->vertices && shape->normals && shape->tangents && shape->texCoords && shape->indices;
}

static GLUSboolean glusShapeFinalizef(GLUSshape* shape)
{
    GLUSuint i;

    // vertex, normal, tangent, bitangent, texCoords
    GLUSuint stride = 4 + 3 + 3 + 3 + 2;

    if (!shape)
    {
        return GLUS_FALSE;
    }

    // Add bitangents
    shape->bitangents = (GLUSfloat*) glusMemoryMalloc(3 * shape->numberVertices * sizeof(GLUSfloat));

    if (!shape->bitangents)
    {
        return GLUS_FALSE;
    }

    for (i = 0; i < shape->numberVertices; i++)
    {
        glusVector3Crossf(&(shape->bitangents[i * 3]), &(shape->normals[i * 3]), &(shape->tangents[i * 3]));
    }

    //

    shape->allAttributes = (GLUSfloat*) glusMemoryMalloc(stride * shape->numberVertices * sizeof(GLUSfloat));

    if (!shape->allAttributes)
    {
        return GLUS_FALSE;
    }

    for (i = 0; i < shape->numberVertices; i++)
    {
        shape->allAttributes[i * stride + 0] = shape->vertices[i * 4 + 0];
        shape->allAttributes[i * stride + 1] = shape->vertices[i * 4 + 1];
        shape->allAttributes[i * stride + 2] = shape->vertices[i * 4 + 2];
        shape->allAttributes[i * stride + 3] = shape->vertices[i * 4 + 3];

        shape->allAttributes[i * stride + 4] = shape->normals[i * 3 + 0];
        shape->allAttributes[i * stride + 5] = shape->normals[i * 3 + 1];
        shape->allAttributes[i * stride + 6] = shape->normals[i * 3 + 2];

        shape->allAttributes[i * stride + 7] = shape->tangents[i * 3 + 0];
        shape->allAttributes[i * stride + 8] = shape->tangents[i * 3 + 1];
        shape->allAttributes[i * stride + 9] = shape->tangents[i * 3 + 2];

        shape->allAttributes[i * stride + 10] = shape->bitangents[i * 3 + 0];
        shape->allAttributes[i * stride + 11] = shape->bitangents[i * 3 + 1];
        shape->allAttributes[i * stride + 12] = shape->bitangents[i * 3 + 2];

        shape->allAttributes[i * stride + 13] = shape->texCoords[i * 2 + 0];
        shape->allAttributes[i * stride + 14] = shape->texCoords[i * 2 + 1];
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusShapeCreatePlanef(GLUSshape* shape, const GLUSfloat halfExtend)
{
    GLUSuint i;

    GLUSuint numberVertices = 4;
    GLUSuint numberIndices = 6;

    GLUSfloat xy_vertices[] = { -1.0f, -1.0f, 0.0f, +1.0f, +1.0f, -1.0f, 0.0f, +1.0f, -1.0f, +1.0f, 0.0f, +1.0f, +1.0f, +1.0f, 0.0f, +1.0f };

    GLUSfloat xy_normals[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };

    GLUSfloat xy_tangents[] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

    GLUSfloat xy_texCoords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

    GLUSindex xy_indices[] = { 0, 1, 2, 1, 3, 2 };

    if (!shape)
    {
        return GLUS_FALSE;
    }
    glusShapeInitf(shape);

    shape->numberVertices = numberVertices;
    shape->numberIndices = numberIndices;

    shape->vertices = (GLUSfloat*) glusMemoryMalloc(4 * numberVertices * sizeof(GLUSfloat));
    shape->normals = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->tangents = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->texCoords = (GLUSfloat*) glusMemoryMalloc(2 * numberVertices * sizeof(GLUSfloat));
    shape->indices = (GLUSindex*) glusMemoryMalloc(numberIndices * sizeof(GLUSindex));

    if (!glusShapeCheckf(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    memcpy(shape->vertices, xy_vertices, sizeof(xy_vertices));
    for (i = 0; i < numberVertices; i++)
    {
        shape->vertices[i * 4 + 0] *= halfExtend;
        shape->vertices[i * 4 + 1] *= halfExtend;
    }

    memcpy(shape->normals, xy_normals, sizeof(xy_normals));

    memcpy(shape->tangents, xy_tangents, sizeof(xy_tangents));

    memcpy(shape->texCoords, xy_texCoords, sizeof(xy_texCoords));

    memcpy(shape->indices, xy_indices, sizeof(xy_indices));

    if (!glusShapeFinalizef(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusShapeCreateRectangularPlanef(GLUSshape* shape, const GLUSfloat horizontalExtend, const GLUSfloat verticalExtend)
{
    GLUSuint i;

    GLUSuint numberVertices = 4;
    GLUSuint numberIndices = 6;

    GLUSfloat xy_vertices[] = { -1.0f, -1.0f, 0.0f, +1.0f, +1.0f, -1.0f, 0.0f, +1.0f, -1.0f, +1.0f, 0.0f, +1.0f, +1.0f, +1.0f, 0.0f, +1.0f };

    GLUSfloat xy_normals[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f };

    GLUSfloat xy_tangents[] = { 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f };

    GLUSfloat xy_texCoords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

    GLUSindex xy_indices[] = { 0, 1, 2, 1, 3, 2 };

    if (!shape)
    {
        return GLUS_FALSE;
    }
    glusShapeInitf(shape);

    shape->numberVertices = numberVertices;
    shape->numberIndices = numberIndices;

    shape->vertices = (GLUSfloat*) glusMemoryMalloc(4 * numberVertices * sizeof(GLUSfloat));
    shape->normals = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->tangents = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->texCoords = (GLUSfloat*) glusMemoryMalloc(2 * numberVertices * sizeof(GLUSfloat));
    shape->indices = (GLUSindex*) glusMemoryMalloc(numberIndices * sizeof(GLUSindex));

    if (!glusShapeCheckf(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    memcpy(shape->vertices, xy_vertices, sizeof(xy_vertices));
    for (i = 0; i < numberVertices; i++)
    {
        shape->vertices[i * 4 + 0] *= horizontalExtend;
        shape->vertices[i * 4 + 1] *= verticalExtend;
    }

    memcpy(shape->normals, xy_normals, sizeof(xy_normals));

    memcpy(shape->tangents, xy_tangents, sizeof(xy_tangents));

    memcpy(shape->texCoords, xy_texCoords, sizeof(xy_texCoords));

    memcpy(shape->indices, xy_indices, sizeof(xy_indices));

    if (!glusShapeFinalizef(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusShapeCreateRectangularGridPlanef(GLUSshape* shape, const GLUSfloat horizontalExtend, const GLUSfloat verticalExtend, const GLUSuint rows, const GLUSuint columns, const GLUSboolean triangleStrip)
{
    GLUSuint i, currentRow, currentColumn;

    GLUSuint numberVertices = (rows + 1) * (columns + 1);
    GLUSuint numberIndices;

    GLUSfloat x, y, s, t;

    if (triangleStrip)
    {
    	numberIndices = rows * 2 * (columns + 1);
    }
    else
    {
    	numberIndices = rows * 6 * columns;
    }

    if (rows < 1 || columns < 1 || numberVertices > GLUS_MAX_VERTICES || numberIndices > GLUS_MAX_INDICES)
    {
        return GLUS_FALSE;
    }

    if (!shape)
    {
        return GLUS_FALSE;
    }
    glusShapeInitf(shape);

    if (triangleStrip)
    {
        shape->mode = GLUS_TRIANGLE_STRIP;
    }
    else
    {
        shape->mode = GLUS_TRIANGLES;
    }

    shape->numberVertices = numberVertices;
    shape->numberIndices = numberIndices;

    shape->vertices = (GLUSfloat*) glusMemoryMalloc(4 * numberVertices * sizeof(GLUSfloat));
    shape->normals = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->tangents = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->texCoords = (GLUSfloat*) glusMemoryMalloc(2 * numberVertices * sizeof(GLUSfloat));
    shape->indices = (GLUSindex*) glusMemoryMalloc(numberIndices * sizeof(GLUSindex));

    if (!glusShapeCheckf(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    for (i = 0; i < numberVertices; i++)
    {
        x = (GLUSfloat) (i % (columns + 1)) / (GLUSfloat) columns;
        y = 1.0f - (GLUSfloat) (i / (columns + 1)) / (GLUSfloat) rows;

        s = x;
        t = y;

        shape->vertices[i * 4 + 0] = horizontalExtend * (x - 0.5f);
        shape->vertices[i * 4 + 1] = verticalExtend * (y - 0.5f);
        shape->vertices[i * 4 + 2] = 0.0f;
        shape->vertices[i * 4 + 3] = 1.0f;

        shape->normals[i * 3 + 0] = 0.0f;
        shape->normals[i * 3 + 1] = 0.0f;
        shape->normals[i * 3 + 2] = 1.0f;

        shape->tangents[i * 3 + 0] = 1.0f;
        shape->tangents[i * 3 + 1] = 0.0f;
        shape->tangents[i * 3 + 2] = 0.0f;

        shape->texCoords[i * 2 + 0] = s;
        shape->texCoords[i * 2 + 1] = t;
    }

    if (triangleStrip)
    {
		for (i = 0; i < rows * (columns + 1); i++)
		{
			currentColumn = i % (columns + 1);
			currentRow = i / (columns + 1);

			if (currentRow == 0)
			{
				// Left to right, top to bottom
				shape->indices[i * 2] = currentColumn + currentRow * (columns + 1);
				shape->indices[i * 2 + 1] = currentColumn + (currentRow + 1) * (columns + 1);
			}
			else
			{
				// Right to left, bottom to up
				shape->indices[i * 2] = (columns - currentColumn) + (currentRow + 1) * (columns + 1);
				shape->indices[i * 2 + 1] = (columns - currentColumn) + currentRow * (columns + 1);
			}
		}
    }
    else
    {
    	for (i = 0; i < rows * columns; i++)
    	{
			currentColumn = i % columns;
			currentRow = i / columns;

			shape->indices[i * 6 + 0] = currentColumn + currentRow * (columns + 1);
    	    shape->indices[i * 6 + 1] = currentColumn + (currentRow + 1) * (columns + 1);
    	    shape->indices[i * 6 + 2] = (currentColumn + 1) + (currentRow + 1) * (columns + 1);

    	    shape->indices[i * 6 + 3] = (currentColumn + 1) + (currentRow + 1) * (columns + 1);
    	    shape->indices[i * 6 + 4] = (currentColumn + 1) + currentRow * (columns + 1);
    	    shape->indices[i * 6 + 5] = currentColumn + currentRow * (columns + 1);
    	}
    }

    if (!glusShapeFinalizef(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusShapeCreateDiscf(GLUSshape* shape, const GLUSfloat radius, const GLUSuint numberSectors)
{
    GLUSuint i;

    GLUSuint numberVertices = numberSectors + 2;
    GLUSuint numberIndices = numberSectors * 3;

    GLUSfloat angleStep = (2.0f * GLUS_PI) / ((GLUSfloat) numberSectors);

    GLUSuint indexIndices;
    GLUSuint indexCounter;

    GLUSuint vertexCounter = 0;

    if (numberSectors < 3 || numberVertices > GLUS_MAX_VERTICES || numberIndices > GLUS_MAX_INDICES)
    {
        return GLUS_FALSE;
    }

    if (!shape)
    {
        return GLUS_FALSE;
    }
    glusShapeInitf(shape);

    shape->numberVertices = numberVertices;
    shape->numberIndices = numberIndices;

    shape->vertices = (GLUSfloat*) glusMemoryMalloc(4 * numberVertices * sizeof(GLUSfloat));
    shape->normals = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->tangents = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->texCoords = (GLUSfloat*) glusMemoryMalloc(2 * numberVertices * sizeof(GLUSfloat));
    shape->indices = (GLUSindex*) glusMemoryMalloc(numberIndices * sizeof(GLUSindex));

    if (!glusShapeCheckf(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    // Center
    shape->vertices[vertexCounter * 4 + 0] = 0.0f;
    shape->vertices[vertexCounter * 4 + 1] = 0.0f;
    shape->vertices[vertexCounter * 4 + 2] = 0.0f;
    shape->vertices[vertexCounter * 4 + 3] = 1.0f;

    shape->normals[vertexCounter * 3 + 0] = 0.0f;
    shape->normals[vertexCounter * 3 + 1] = 0.0f;
    shape->normals[vertexCounter * 3 + 2] = 1.0f;

    shape->tangents[vertexCounter * 3 + 0] = 1.0f;
    shape->tangents[vertexCounter * 3 + 1] = 0.0f;
    shape->tangents[vertexCounter * 3 + 2] = 0.0f;

    shape->texCoords[vertexCounter * 2 + 0] = 0.5f;
    shape->texCoords[vertexCounter * 2 + 1] = 0.5f;

    vertexCounter++;

    for (i = 0; i < numberSectors + 1; i++)
    {
    	GLUSfloat currentAngle = angleStep * (GLUSfloat)i;

		shape->vertices[vertexCounter * 4 + 0] = cosf(currentAngle) * radius;
		shape->vertices[vertexCounter * 4 + 1] = sinf(currentAngle) * radius;
		shape->vertices[vertexCounter * 4 + 2] = 0.0f;
		shape->vertices[vertexCounter * 4 + 3] = 1.0f;

		shape->normals[vertexCounter * 3 + 0] = 0.0f;
		shape->normals[vertexCounter * 3 + 1] = 0.0f;
		shape->normals[vertexCounter * 3 + 2] = 1.0f;

		shape->tangents[vertexCounter * 3 + 0] = 1.0f;
		shape->tangents[vertexCounter * 3 + 1] = 0.0f;
		shape->tangents[vertexCounter * 3 + 2] = 0.0f;

		shape->texCoords[vertexCounter * 2 + 0] = 0.5f * cosf(currentAngle) * 0.5f;
		shape->texCoords[vertexCounter * 2 + 1] = 0.5f * sinf(currentAngle) * 0.5f;

		vertexCounter++;
    }

    indexIndices = 0;

    // Bottom
    indexCounter = 1;

    for (i = 0; i < numberSectors; i++)
    {
    	shape->indices[indexIndices++] = 0;
        shape->indices[indexIndices++] = indexCounter;
        shape->indices[indexIndices++] = indexCounter + 1;

        indexCounter++;
    }

    if (!glusShapeFinalizef(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

	return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusShapeCreateCubef(GLUSshape* shape, const GLUSfloat halfExtend)
{
    GLUSuint i;

    GLUSuint numberVertices = 24;
    GLUSuint numberIndices = 36;

    GLUSfloat cubeVertices[] = { -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f,

    -1.0f, +1.0f, -1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f,

    -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, -1.0f, -1.0f, +1.0f,

    -1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f,

    -1.0f, -1.0f, -1.0f, +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f, -1.0f, +1.0f,

    +1.0f, -1.0f, -1.0f, +1.0f, +1.0f, -1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, +1.0f, -1.0f, +1.0f  };

    GLUSfloat cubeNormals[] = {  0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f,

    0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f,

    0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f,

    0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f,

    -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

    +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f };

    GLUSfloat cubeTangents[] = { +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f,

    +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f,

    -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

    +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f,

    0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f, 0.0f, 0.0f, +1.0f,

    0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, -1.0f };

    GLUSfloat cubeTexCoords[] =
            { 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

    0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,

    1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

    0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f,

    1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f };

    GLUSindex cubeIndices[] = { 0, 2, 1, 0, 3, 2, 4, 5, 6, 4, 6, 7, 8, 9, 10, 8, 10, 11, 12, 15, 14, 12, 14, 13, 16, 17, 18, 16, 18, 19, 20, 23, 22, 20, 22, 21 };

    if (!shape)
    {
        return GLUS_FALSE;
    }
    glusShapeInitf(shape);

    shape->numberVertices = numberVertices;
    shape->numberIndices = numberIndices;

    shape->vertices = (GLUSfloat*) glusMemoryMalloc(4 * numberVertices * sizeof(GLUSfloat));
    shape->normals = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->tangents = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->texCoords = (GLUSfloat*) glusMemoryMalloc(2 * numberVertices * sizeof(GLUSfloat));
    shape->indices = (GLUSindex*) glusMemoryMalloc(numberIndices * sizeof(GLUSindex));

    if (!glusShapeCheckf(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    memcpy(shape->vertices, cubeVertices, sizeof(cubeVertices));
    for (i = 0; i < numberVertices; i++)
    {
        shape->vertices[i * 4 + 0] *= halfExtend;
        shape->vertices[i * 4 + 1] *= halfExtend;
        shape->vertices[i * 4 + 2] *= halfExtend;
    }

    memcpy(shape->normals, cubeNormals, sizeof(cubeNormals));

    memcpy(shape->tangents, cubeTangents, sizeof(cubeTangents));

    memcpy(shape->texCoords, cubeTexCoords, sizeof(cubeTexCoords));

    memcpy(shape->indices, cubeIndices, sizeof(cubeIndices));

    if (!glusShapeFinalizef(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusShapeCreateSpheref(GLUSshape* shape, const GLUSfloat radius, const GLUSuint numberSlices)
{
    GLUSuint i, j;

    GLUSuint numberParallels = numberSlices / 2;
    GLUSuint numberVertices = (numberParallels + 1) * (numberSlices + 1);
    GLUSuint numberIndices = numberParallels * numberSlices * 6;

    GLUSfloat angleStep = (2.0f * GLUS_PI) / ((GLUSfloat) numberSlices);

    GLUSuint indexIndices;

    // used later to help us calculating tangents vectors
    GLUSfloat helpVector[3] = { 1.0f, 0.0f, 0.0f };
    GLUSfloat helpQuaternion[4];
    GLUSfloat helpMatrix[16];

    if (numberSlices < 3 || numberVertices > GLUS_MAX_VERTICES || numberIndices > GLUS_MAX_INDICES)
    {
        return GLUS_FALSE;
    }

    if (!shape)
    {
        return GLUS_FALSE;
    }
    glusShapeInitf(shape);

    shape->numberVertices = numberVertices;
    shape->numberIndices = numberIndices;

    shape->vertices = (GLUSfloat*) glusMemoryMalloc(4 * numberVertices * sizeof(GLUSfloat));
    shape->normals = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->tangents = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->texCoords = (GLUSfloat*) glusMemoryMalloc(2 * numberVertices * sizeof(GLUSfloat));
    shape->indices = (GLUSindex*) glusMemoryMalloc(numberIndices * sizeof(GLUSindex));

    if (!glusShapeCheckf(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    for (i = 0; i < numberParallels + 1; i++)
    {
        for (j = 0; j < numberSlices + 1; j++)
        {
            GLUSuint vertexIndex = (i * (numberSlices + 1) + j) * 4;
            GLUSuint normalIndex = (i * (numberSlices + 1) + j) * 3;
            GLUSuint tangentIndex = (i * (numberSlices + 1) + j) * 3;
            GLUSuint texCoordsIndex = (i * (numberSlices + 1) + j) * 2;

            shape->vertices[vertexIndex + 0] = radius * sinf(angleStep * (GLUSfloat) i) * sinf(angleStep * (GLUSfloat) j);
            shape->vertices[vertexIndex + 1] = radius * cosf(angleStep * (GLUSfloat) i);
            shape->vertices[vertexIndex + 2] = radius * sinf(angleStep * (GLUSfloat) i) * cosf(angleStep * (GLUSfloat) j);
            shape->vertices[vertexIndex + 3] = 1.0f;

            shape->normals[normalIndex + 0] = shape->vertices[vertexIndex + 0] / radius;
            shape->normals[normalIndex + 1] = shape->vertices[vertexIndex + 1] / radius;
            shape->normals[normalIndex + 2] = shape->vertices[vertexIndex + 2] / radius;

            shape->texCoords[texCoordsIndex + 0] = (GLUSfloat) j / (GLUSfloat) numberSlices;
            shape->texCoords[texCoordsIndex + 1] = 1.0f - (GLUSfloat) i / (GLUSfloat) numberParallels;

            // use quaternion to get the tangent vector
            glusQuaternionRotateRyf(helpQuaternion, 360.0f * shape->texCoords[texCoordsIndex + 0]);
            glusQuaternionGetMatrix4x4f(helpMatrix, helpQuaternion);

            glusMatrix4x4MultiplyVector3f(&shape->tangents[tangentIndex], helpMatrix, helpVector);
        }
    }

    indexIndices = 0;
    for (i = 0; i < numberParallels; i++)
    {
        for (j = 0; j < numberSlices; j++)
        {
            shape->indices[indexIndices++] = i * (numberSlices + 1) + j;
            shape->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + j;
            shape->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + (j + 1);

            shape->indices[indexIndices++] = i * (numberSlices + 1) + j;
            shape->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + (j + 1);
            shape->indices[indexIndices++] = i * (numberSlices + 1) + (j + 1);
        }
    }

    if (!glusShapeFinalizef(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

	return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusShapeCreateDomef(GLUSshape* shape, const GLUSfloat radius, const GLUSuint numberSlices)
{
    GLUSuint i, j;

    GLUSuint numberParallels = numberSlices / 4;
    GLUSuint numberVertices = (numberParallels + 1) * (numberSlices + 1);
    GLUSuint numberIndices = numberParallels * numberSlices * 6;

    GLUSfloat angleStep = (2.0f * GLUS_PI) / ((GLUSfloat) numberSlices);

    GLUSuint indexIndices;

    // used later to help us calculating tangents vectors
    GLUSfloat helpVector[3] = { 1.0f, 0.0f, 0.0f };
    GLUSfloat helpQuaternion[4];
    GLUSfloat helpMatrix[16];

    if (numberSlices < 3 || numberVertices > GLUS_MAX_VERTICES || numberIndices > GLUS_MAX_INDICES)
    {
        return GLUS_FALSE;
    }

    if (!shape)
    {
        return GLUS_FALSE;
    }
    glusShapeInitf(shape);

    shape->numberVertices = numberVertices;
    shape->numberIndices = numberIndices;

    shape->vertices = (GLUSfloat*) glusMemoryMalloc(4 * numberVertices * sizeof(GLUSfloat));
    shape->normals = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->tangents = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->texCoords = (GLUSfloat*) glusMemoryMalloc(2 * numberVertices * sizeof(GLUSfloat));
    shape->indices = (GLUSindex*) glusMemoryMalloc(numberIndices * sizeof(GLUSindex));

    if (!glusShapeCheckf(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    for (i = 0; i < numberParallels + 1; i++)
    {
        for (j = 0; j < numberSlices + 1; j++)
        {
            GLUSuint vertexIndex = (i * (numberSlices + 1) + j) * 4;
            GLUSuint normalIndex = (i * (numberSlices + 1) + j) * 3;
            GLUSuint tangentIndex = (i * (numberSlices + 1) + j) * 3;
            GLUSuint texCoordsIndex = (i * (numberSlices + 1) + j) * 2;

            shape->vertices[vertexIndex + 0] = radius * sinf(angleStep * (GLUSfloat) i) * sinf(angleStep * (GLUSfloat) j);
            shape->vertices[vertexIndex + 1] = radius * cosf(angleStep * (GLUSfloat) i);
            shape->vertices[vertexIndex + 2] = radius * sinf(angleStep * (GLUSfloat) i) * cosf(angleStep * (GLUSfloat) j);
            shape->vertices[vertexIndex + 3] = 1.0f;

            shape->normals[normalIndex + 0] = shape->vertices[vertexIndex + 0] / radius;
            shape->normals[normalIndex + 1] = shape->vertices[vertexIndex + 1] / radius;
            shape->normals[normalIndex + 2] = shape->vertices[vertexIndex + 2] / radius;

            shape->texCoords[texCoordsIndex + 0] = (GLUSfloat) j / (GLUSfloat) numberSlices;
            shape->texCoords[texCoordsIndex + 1] = 1.0f - (GLUSfloat) i / (GLUSfloat) numberParallels;

            // use quaternion to get the tangent vector
            glusQuaternionRotateRyf(helpQuaternion, 360.0f * shape->texCoords[texCoordsIndex + 0]);
            glusQuaternionGetMatrix4x4f(helpMatrix, helpQuaternion);

            glusMatrix4x4MultiplyVector3f(&shape->tangents[tangentIndex], helpMatrix, helpVector);
        }
    }

    indexIndices = 0;
    for (i = 0; i < numberParallels; i++)
    {
        for (j = 0; j < numberSlices; j++)
        {
            shape->indices[indexIndices++] = i * (numberSlices + 1) + j;
            shape->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + j;
            shape->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + (j + 1);

            shape->indices[indexIndices++] = i * (numberSlices + 1) + j;
            shape->indices[indexIndices++] = (i + 1) * (numberSlices + 1) + (j + 1);
            shape->indices[indexIndices++] = i * (numberSlices + 1) + (j + 1);
        }
    }

    if (!glusShapeFinalizef(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

	return GLUS_TRUE;
}

/*
 * @author Pablo Alonso-Villaverde Roza
 * @author Norbert Nopper
 */
GLUSboolean GLUSAPIENTRY glusShapeCreateTorusf(GLUSshape* shape, const GLUSfloat innerRadius, const GLUSfloat outerRadius, const GLUSuint numberSlices, const GLUSuint numberStacks)
{
    // s, t = parametric values of the equations, in the range [0,1]
    GLUSfloat s = 0;
    GLUSfloat t = 0;

    // sIncr, tIncr are increment values aplied to s and t on each loop iteration to generate the torus
    GLUSfloat sIncr;
    GLUSfloat tIncr;

    // to store precomputed sin and cos values
    GLUSfloat cos2PIs, sin2PIs, cos2PIt, sin2PIt;

    GLUSuint numberVertices;
    GLUSuint numberIndices;

    // used later to help us calculating tangents vectors
    GLUSfloat helpVector[3] = { 0.0f, 1.0f, 0.0f };
    GLUSfloat helpQuaternion[4];
    GLUSfloat helpMatrix[16];

    // indices for each type of buffer (of vertices, indices, normals...)
    GLUSuint indexVertices, indexIndices, indexNormals, indexTangents, indexTexCoords;

    // loop counters
    GLUSuint sideCount, faceCount;

    // used to generate the indices
    GLUSuint v0, v1, v2, v3;

    GLUSfloat torusRadius = (outerRadius - innerRadius) / 2.0f;
    GLUSfloat centerRadius = outerRadius - torusRadius;

    numberVertices = (numberStacks + 1) * (numberSlices + 1);
    numberIndices = numberStacks * numberSlices * 2 * 3; // 2 triangles per face * 3 indices per triangle

    if (numberSlices < 3 || numberStacks < 3 || numberVertices > GLUS_MAX_VERTICES || numberIndices > GLUS_MAX_INDICES)
    {
        return GLUS_FALSE;
    }

    if (!shape)
    {
        return GLUS_FALSE;
    }
    glusShapeInitf(shape);

    shape->numberVertices = numberVertices;
    shape->numberIndices = numberIndices;

    shape->vertices = (GLUSfloat*) glusMemoryMalloc(4 * numberVertices * sizeof(GLUSfloat));
    shape->normals = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->tangents = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->texCoords = (GLUSfloat*) glusMemoryMalloc(2 * numberVertices * sizeof(GLUSfloat));
    shape->indices = (GLUSindex*) glusMemoryMalloc(numberIndices * sizeof(GLUSindex));

    if (!glusShapeCheckf(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    sIncr = 1.0f / (GLUSfloat) numberSlices;
    tIncr = 1.0f / (GLUSfloat) numberStacks;

    // generate vertices and its attributes
    for (sideCount = 0; sideCount <= numberSlices; ++sideCount, s += sIncr)
    {
        // precompute some values
        cos2PIs = (GLUSfloat) cosf(2.0f * GLUS_PI * s);
        sin2PIs = (GLUSfloat) sinf(2.0f * GLUS_PI * s);

        t = 0.0f;
        for (faceCount = 0; faceCount <= numberStacks; ++faceCount, t += tIncr)
        {
            // precompute some values
            cos2PIt = (GLUSfloat) cosf(2.0f * GLUS_PI * t);
            sin2PIt = (GLUSfloat) sinf(2.0f * GLUS_PI * t);

            // generate vertex and stores it in the right position
            indexVertices = ((sideCount * (numberStacks + 1)) + faceCount) * 4;
            shape->vertices[indexVertices + 0] = (centerRadius + torusRadius * cos2PIt) * cos2PIs;
            shape->vertices[indexVertices + 1] = (centerRadius + torusRadius * cos2PIt) * sin2PIs;
            shape->vertices[indexVertices + 2] = torusRadius * sin2PIt;
            shape->vertices[indexVertices + 3] = 1.0f;

            // generate normal and stores it in the right position
            // NOTE: cos (2PIx) = cos (x) and sin (2PIx) = sin (x) so, we can use this formula
            //       normal = {cos(2PIs)cos(2PIt) , sin(2PIs)cos(2PIt) ,sin(2PIt)}
            indexNormals = ((sideCount * (numberStacks + 1)) + faceCount) * 3;
            shape->normals[indexNormals + 0] = cos2PIs * cos2PIt;
            shape->normals[indexNormals + 1] = sin2PIs * cos2PIt;
            shape->normals[indexNormals + 2] = sin2PIt;

            // generate texture coordinates and stores it in the right position
            indexTexCoords = ((sideCount * (numberStacks + 1)) + faceCount) * 2;
            shape->texCoords[indexTexCoords + 0] = s;
            shape->texCoords[indexTexCoords + 1] = t;

            // use quaternion to get the tangent vector
            glusQuaternionRotateRzf(helpQuaternion, 360.0f * s);
            glusQuaternionGetMatrix4x4f(helpMatrix, helpQuaternion);

            indexTangents = ((sideCount * (numberStacks + 1)) + faceCount) * 3;

            glusMatrix4x4MultiplyVector3f(&shape->tangents[indexTangents], helpMatrix, helpVector);
        }
    }

    // generate indices
    indexIndices = 0;
    for (sideCount = 0; sideCount < numberSlices; ++sideCount)
    {
        for (faceCount = 0; faceCount < numberStacks; ++faceCount)
        {
            // get the number of the vertices for a face of the torus. They must be < numVertices
            v0 = ((sideCount * (numberStacks + 1)) + faceCount);
            v1 = (((sideCount + 1) * (numberStacks + 1)) + faceCount);
            v2 = (((sideCount + 1) * (numberStacks + 1)) + (faceCount + 1));
            v3 = ((sideCount * (numberStacks + 1)) + (faceCount + 1));

            // first triangle of the face, counter clock wise winding
            shape->indices[indexIndices++] = v0;
            shape->indices[indexIndices++] = v1;
            shape->indices[indexIndices++] = v2;

            // second triangle of the face, counter clock wise winding
            shape->indices[indexIndices++] = v0;
            shape->indices[indexIndices++] = v2;
            shape->indices[indexIndices++] = v3;
        }
    }

    if (!glusShapeFinalizef(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusShapeCreateCylinderf(GLUSshape* shape, const GLUSfloat halfExtend, const GLUSfloat radius, const GLUSuint numberSlices)
{
    GLUSuint i, j;

    GLUSuint numberVertices = (numberSlices + 2) * 2 + (numberSlices + 1) * 2;
    GLUSuint numberIndices = numberSlices * 3 * 2 + numberSlices * 6;

    GLUSfloat angleStep = (2.0f * GLUS_PI) / ((GLUSfloat) numberSlices);

    GLUSuint indexIndices;
    GLUSuint centerIndex;
    GLUSuint indexCounter;

    GLUSuint vertexCounter = 0;

    if (numberSlices < 3 || numberVertices > GLUS_MAX_VERTICES || numberIndices > GLUS_MAX_INDICES)
    {
        return GLUS_FALSE;
    }

    if (!shape)
    {
        return GLUS_FALSE;
    }
    glusShapeInitf(shape);

    shape->numberVertices = numberVertices;
    shape->numberIndices = numberIndices;

    shape->vertices = (GLUSfloat*) glusMemoryMalloc(4 * numberVertices * sizeof(GLUSfloat));
    shape->normals = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->tangents = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->texCoords = (GLUSfloat*) glusMemoryMalloc(2 * numberVertices * sizeof(GLUSfloat));
    shape->indices = (GLUSindex*) glusMemoryMalloc(numberIndices * sizeof(GLUSindex));

    if (!glusShapeCheckf(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    // Center bottom
    shape->vertices[vertexCounter * 4 + 0] = 0.0f;
    shape->vertices[vertexCounter * 4 + 1] = -halfExtend;
    shape->vertices[vertexCounter * 4 + 2] = 0.0f;
    shape->vertices[vertexCounter * 4 + 3] = 1.0f;

    shape->normals[vertexCounter * 3 + 0] = 0.0f;
    shape->normals[vertexCounter * 3 + 1] = -1.0f;
    shape->normals[vertexCounter * 3 + 2] = 0.0f;

    shape->tangents[vertexCounter * 3 + 0] = 0.0f;
    shape->tangents[vertexCounter * 3 + 1] = 0.0f;
    shape->tangents[vertexCounter * 3 + 2] = 1.0f;

    shape->texCoords[vertexCounter * 2 + 0] = 0.0f;
    shape->texCoords[vertexCounter * 2 + 1] = 0.0f;

    vertexCounter++;

    // Bottom
    for (i = 0; i < numberSlices + 1; i++)
    {
    	GLUSfloat currentAngle = angleStep * (GLUSfloat)i;

		shape->vertices[vertexCounter * 4 + 0] = cosf(currentAngle) * radius;
		shape->vertices[vertexCounter * 4 + 1] = -halfExtend;
		shape->vertices[vertexCounter * 4 + 2] = -sinf(currentAngle) * radius;
		shape->vertices[vertexCounter * 4 + 3] = 1.0f;

		shape->normals[vertexCounter * 3 + 0] = 0.0f;
		shape->normals[vertexCounter * 3 + 1] = -1.0f;
		shape->normals[vertexCounter * 3 + 2] = 0.0f;

		shape->tangents[vertexCounter * 3 + 0] = sinf(currentAngle);
		shape->tangents[vertexCounter * 3 + 1] = 0.0f;
		shape->tangents[vertexCounter * 3 + 2] = cosf(currentAngle);

		shape->texCoords[vertexCounter * 2 + 0] = 0.0f;
		shape->texCoords[vertexCounter * 2 + 1] = 0.0f;

		vertexCounter++;
    }

    // Center top
    shape->vertices[vertexCounter * 4 + 0] = 0.0f;
    shape->vertices[vertexCounter * 4 + 1] = halfExtend;
    shape->vertices[vertexCounter * 4 + 2] = 0.0f;
    shape->vertices[vertexCounter * 4 + 3] = 1.0f;

    shape->normals[vertexCounter * 3 + 0] = 0.0f;
    shape->normals[vertexCounter * 3 + 1] = 1.0f;
    shape->normals[vertexCounter * 3 + 2] = 0.0f;

    shape->tangents[vertexCounter * 3 + 0] = 0.0f;
    shape->tangents[vertexCounter * 3 + 1] = 0.0f;
    shape->tangents[vertexCounter * 3 + 2] = -1.0f;

    shape->texCoords[vertexCounter * 2 + 0] = 1.0f;
    shape->texCoords[vertexCounter * 2 + 1] = 1.0f;

    vertexCounter++;

    // Top
    for (i = 0; i < numberSlices + 1; i++)
    {
    	GLUSfloat currentAngle = angleStep * (GLUSfloat)i;

		shape->vertices[vertexCounter * 4 + 0] = cosf(currentAngle) * radius;
		shape->vertices[vertexCounter * 4 + 1] = halfExtend;
		shape->vertices[vertexCounter * 4 + 2] = -sinf(currentAngle) * radius;
		shape->vertices[vertexCounter * 4 + 3] = 1.0f;

		shape->normals[vertexCounter * 3 + 0] = 0.0f;
		shape->normals[vertexCounter * 3 + 1] = 1.0f;
		shape->normals[vertexCounter * 3 + 2] = 0.0f;

		shape->tangents[vertexCounter * 3 + 0] = -sinf(currentAngle);
		shape->tangents[vertexCounter * 3 + 1] = 0.0f;
		shape->tangents[vertexCounter * 3 + 2] = -cosf(currentAngle);

		shape->texCoords[vertexCounter * 2 + 0] = 1.0f;
		shape->texCoords[vertexCounter * 2 + 1] = 1.0f;

		vertexCounter++;
    }

    for (i = 0; i < numberSlices + 1; i++)
    {
		GLUSfloat currentAngle = angleStep * (GLUSfloat)i;

		GLUSfloat sign = -1.0f;

		for (j = 0; j < 2; j++)
        {
			shape->vertices[vertexCounter * 4 + 0] = cosf(currentAngle) * radius;
			shape->vertices[vertexCounter * 4 + 1] = halfExtend * sign;
			shape->vertices[vertexCounter * 4 + 2] = -sinf(currentAngle) * radius;
			shape->vertices[vertexCounter * 4 + 3] = 1.0f;

			shape->normals[vertexCounter * 3 + 0] = cosf(currentAngle);
			shape->normals[vertexCounter * 3 + 1] = 0.0f;
			shape->normals[vertexCounter * 3 + 2] = -sinf(currentAngle);

			shape->tangents[vertexCounter * 3 + 0] = -sinf(currentAngle);
			shape->tangents[vertexCounter * 3 + 1] = 0.0f;
			shape->tangents[vertexCounter * 3 + 2] = -cosf(currentAngle);

			shape->texCoords[vertexCounter * 2 + 0] = (GLUSfloat)i / (GLUSfloat)numberSlices;
			shape->texCoords[vertexCounter * 2 + 1] = (sign + 1.0f) / 2.0f;

			vertexCounter++;

			sign = 1.0f;
        }
    }

    indexIndices = 0;

    // Bottom
    centerIndex = 0;
    indexCounter = 1;

    for (i = 0; i < numberSlices; i++)
    {
    	shape->indices[indexIndices++] = centerIndex;
        shape->indices[indexIndices++] = indexCounter + 1;
        shape->indices[indexIndices++] = indexCounter;

        indexCounter++;
    }
    indexCounter++;

    // Top
    centerIndex = indexCounter;
    indexCounter++;

    for (i = 0; i < numberSlices; i++)
    {
    	shape->indices[indexIndices++] = centerIndex;
        shape->indices[indexIndices++] = indexCounter;
        shape->indices[indexIndices++] = indexCounter + 1;

        indexCounter++;
    }
    indexCounter++;

    // Sides
    for (i = 0; i < numberSlices; i++)
    {
    	shape->indices[indexIndices++] = indexCounter;
        shape->indices[indexIndices++] = indexCounter + 2;
        shape->indices[indexIndices++] = indexCounter + 1;

    	shape->indices[indexIndices++] = indexCounter + 2;
        shape->indices[indexIndices++] = indexCounter + 3;
        shape->indices[indexIndices++] = indexCounter + 1;

        indexCounter += 2;
    }

    if (!glusShapeFinalizef(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

	return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusShapeCreateConef(GLUSshape* shape, const GLUSfloat halfExtend, const GLUSfloat radius, const GLUSuint numberSlices, const GLUSuint numberStacks)
{
    GLUSuint i, j;

    GLUSuint numberVertices = (numberSlices + 2) + (numberSlices + 1) * (numberStacks + 1);
    GLUSuint numberIndices = numberSlices * 3 + numberSlices * 6 * numberStacks;

    GLUSfloat angleStep = (2.0f * GLUS_PI) / ((GLUSfloat) numberSlices);

    GLUSuint indexIndices;
    GLUSuint centerIndex;
    GLUSuint indexCounter;

    GLUSuint vertexCounter = 0;

    GLUSfloat h = 2.0f * halfExtend;
    GLUSfloat r = radius;
    GLUSfloat l = sqrtf(h*h + r*r);

    if (numberSlices < 3 || numberStacks < 1 || numberVertices > GLUS_MAX_VERTICES || numberIndices > GLUS_MAX_INDICES)
    {
        return GLUS_FALSE;
    }

    if (!shape)
    {
        return GLUS_FALSE;
    }
    glusShapeInitf(shape);

    shape->numberVertices = numberVertices;
    shape->numberIndices = numberIndices;

    shape->vertices = (GLUSfloat*) glusMemoryMalloc(4 * numberVertices * sizeof(GLUSfloat));
    shape->normals = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->tangents = (GLUSfloat*) glusMemoryMalloc(3 * numberVertices * sizeof(GLUSfloat));
    shape->texCoords = (GLUSfloat*) glusMemoryMalloc(2 * numberVertices * sizeof(GLUSfloat));
    shape->indices = (GLUSindex*) glusMemoryMalloc(numberIndices * sizeof(GLUSindex));

    if (!glusShapeCheckf(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

    // Center bottom
    shape->vertices[vertexCounter * 4 + 0] = 0.0f;
    shape->vertices[vertexCounter * 4 + 1] = -halfExtend;
    shape->vertices[vertexCounter * 4 + 2] = 0.0f;
    shape->vertices[vertexCounter * 4 + 3] = 1.0f;

    shape->normals[vertexCounter * 3 + 0] = 0.0f;
    shape->normals[vertexCounter * 3 + 1] = -1.0f;
    shape->normals[vertexCounter * 3 + 2] = 0.0f;

    shape->tangents[vertexCounter * 3 + 0] = 0.0f;
    shape->tangents[vertexCounter * 3 + 1] = 0.0f;
    shape->tangents[vertexCounter * 3 + 2] = 1.0f;

    shape->texCoords[vertexCounter * 2 + 0] = 0.0f;
    shape->texCoords[vertexCounter * 2 + 1] = 0.0f;

    vertexCounter++;

    // Bottom
    for (i = 0; i < numberSlices + 1; i++)
    {
    	GLUSfloat currentAngle = angleStep * (GLUSfloat)i;

		shape->vertices[vertexCounter * 4 + 0] = cosf(currentAngle) * radius;
		shape->vertices[vertexCounter * 4 + 1] = -halfExtend;
		shape->vertices[vertexCounter * 4 + 2] = -sinf(currentAngle) * radius;
		shape->vertices[vertexCounter * 4 + 3] = 1.0f;

		shape->normals[vertexCounter * 3 + 0] = 0.0f;
		shape->normals[vertexCounter * 3 + 1] = -1.0f;
		shape->normals[vertexCounter * 3 + 2] = 0.0f;

		shape->tangents[vertexCounter * 3 + 0] = sinf(currentAngle);
		shape->tangents[vertexCounter * 3 + 1] = 0.0f;
		shape->tangents[vertexCounter * 3 + 2] = cosf(currentAngle);

		shape->texCoords[vertexCounter * 2 + 0] = 0.0f;
		shape->texCoords[vertexCounter * 2 + 1] = 0.0f;

		vertexCounter++;
    }

	for (j = 0; j < numberStacks + 1; j++)
    {
		GLUSfloat level = (GLUSfloat)j / (GLUSfloat)numberStacks;

		for (i = 0; i < numberSlices + 1; i++)
		{
			GLUSfloat currentAngle = angleStep * (GLUSfloat)i;

			shape->vertices[vertexCounter * 4 + 0] = cosf(currentAngle) * radius * (1.0f - level);
			shape->vertices[vertexCounter * 4 + 1] = -halfExtend + 2.0f * halfExtend * level;
			shape->vertices[vertexCounter * 4 + 2] = -sinf(currentAngle) * radius * (1.0f - level);
			shape->vertices[vertexCounter * 4 + 3] = 1.0f;

			shape->normals[vertexCounter * 3 + 0] = h / l * cosf(currentAngle);
			shape->normals[vertexCounter * 3 + 1] = r / l;
			shape->normals[vertexCounter * 3 + 2] = h / l * -sinf(currentAngle);

			shape->tangents[vertexCounter * 3 + 0] = -sinf(currentAngle);
			shape->tangents[vertexCounter * 3 + 1] = 0.0f;
			shape->tangents[vertexCounter * 3 + 2] = -cosf(currentAngle);

			shape->texCoords[vertexCounter * 2 + 0] = (GLUSfloat)i / (GLUSfloat)numberSlices;
			shape->texCoords[vertexCounter * 2 + 1] = level;

			vertexCounter++;
        }
    }

    indexIndices = 0;

    // Bottom
    centerIndex = 0;
    indexCounter = 1;

    for (i = 0; i < numberSlices; i++)
    {
    	shape->indices[indexIndices++] = centerIndex;
        shape->indices[indexIndices++] = indexCounter + 1;
        shape->indices[indexIndices++] = indexCounter;

        indexCounter++;
    }
    indexCounter++;

    // Sides
	for (j = 0; j < numberStacks; j++)
	{
		for (i = 0; i < numberSlices; i++)
		{
			shape->indices[indexIndices++] = indexCounter;
			shape->indices[indexIndices++] = indexCounter + 1;
			shape->indices[indexIndices++] = indexCounter + numberSlices + 1;

			shape->indices[indexIndices++] = indexCounter + 1;
			shape->indices[indexIndices++] = indexCounter + numberSlices + 2;
			shape->indices[indexIndices++] = indexCounter + numberSlices + 1;

	        indexCounter++;
    	}
		indexCounter++;
    }

    if (!glusShapeFinalizef(shape))
    {
        glusShapeDestroyf(shape);

        return GLUS_FALSE;
    }

	return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusShapeCalculateTangentBitangentf(GLUSshape* shape)
{
    GLUSuint i;

    if (!shape || !shape->vertices || !shape->texCoords || shape->mode != GLUS_TRIANGLES)
    {
        return GLUS_FALSE;
    }

    // Allocate memory if needed
    if (!shape->tangents)
    {
    	shape->tangents = (GLUSfloat*) glusMemoryMalloc(3 * shape->numberVertices * sizeof(GLUSfloat));

        if (!shape->tangents)
        {
            return GLUS_FALSE;
        }
    }

    if (!shape->bitangents)
    {
    	shape->bitangents = (GLUSfloat*) glusMemoryMalloc(3 * shape->numberVertices * sizeof(GLUSfloat));

        if (!shape->bitangents)
        {
            return GLUS_FALSE;
        }
    }

    // Reset all tangents to 0.0f
    for (i = 0; i < shape->numberVertices; i++)
    {
    	shape->tangents[i * 3] = 0.0f;
    	shape->tangents[i * 3 + 1] = 0.0f;
    	shape->tangents[i * 3 + 2] = 0.0f;

    	shape->bitangents[i * 3] = 0.0f;
    	shape->bitangents[i * 3 + 1] = 0.0f;
    	shape->bitangents[i * 3 + 2] = 0.0f;
    }

    if (shape->numberIndices > 0)
    {
    	float s1, t1, s2, t2;
    	float Q1[4];
    	float Q2[4];
    	float tangent[3];
    	float bitangent[3];
    	float scalar;

    	for (i = 0; i < shape->numberIndices; i += 3)
    	{
    		s1 = shape->texCoords[2*shape->indices[i+1]] - shape->texCoords[2*shape->indices[i]];
    		t1 = shape->texCoords[2*shape->indices[i+1]+1] - shape->texCoords[2*shape->indices[i]+1];
    		s2 = shape->texCoords[2*shape->indices[i+2]] - shape->texCoords[2*shape->indices[i]];
    		t2 = shape->texCoords[2*shape->indices[i+2]+1] - shape->texCoords[2*shape->indices[i]+1];

    		scalar = 1.0f / (s1*t2-s2*t1);

    		glusPoint4SubtractPoint4f(Q1, &shape->vertices[4*shape->indices[i+1]], &shape->vertices[4*shape->indices[i]]);
    		Q1[3] = 1.0f;
    		glusPoint4SubtractPoint4f(Q2, &shape->vertices[4*shape->indices[i+2]], &shape->vertices[4*shape->indices[i]]);
    		Q2[3] = 1.0f;

    		tangent[0] = scalar * (t2 * Q1[0] - t1 * Q2[0]);
    		tangent[1] = scalar * (t2 * Q1[1] - t1 * Q2[1]);
    		tangent[2] = scalar * (t2 * Q1[2] - t1 * Q2[2]);

    		bitangent[0] = scalar * (-s2 * Q1[0] + s1 * Q2[0]);
    		bitangent[1] = scalar * (-s2 * Q1[1] + s1 * Q2[1]);
    		bitangent[2] = scalar * (-s2 * Q1[2] + s1 * Q2[2]);

    		glusVector3Normalizef(tangent);

    		glusVector3Normalizef(bitangent);

        	shape->tangents[3 * shape->indices[i]] += tangent[0];
        	shape->tangents[3 * shape->indices[i] + 1] += tangent[1];
        	shape->tangents[3 * shape->indices[i] + 2] += tangent[2];

        	shape->tangents[3 * shape->indices[i+1]] += tangent[0];
        	shape->tangents[3 * shape->indices[i+1] + 1] += tangent[1];
        	shape->tangents[3 * shape->indices[i+1] + 2] += tangent[2];

        	shape->tangents[3 * shape->indices[i+2]] += tangent[0];
        	shape->tangents[3 * shape->indices[i+2] + 1] += tangent[1];
        	shape->tangents[3 * shape->indices[i+2] + 2] += tangent[2];


        	shape->bitangents[3 * shape->indices[i]] += bitangent[0];
        	shape->bitangents[3 * shape->indices[i] + 1] += bitangent[1];
        	shape->bitangents[3 * shape->indices[i] + 2] += bitangent[2];

        	shape->bitangents[3 * shape->indices[i+1]] += bitangent[0];
        	shape->bitangents[3 * shape->indices[i+1] + 1] += bitangent[1];
        	shape->bitangents[3 * shape->indices[i+1] + 2] += bitangent[2];

        	shape->bitangents[3 * shape->indices[i+2]] += bitangent[0];
        	shape->bitangents[3 * shape->indices[i+2] + 1] += bitangent[1];
        	shape->bitangents[3 * shape->indices[i+2] + 2] += bitangent[2];
    	}
    }
    else
    {
    	float s1, t1, s2, t2;
    	float Q1[4];
    	float Q2[4];
    	float tangent[3];
    	float bitangent[3];
    	float scalar;

    	for (i = 0; i < shape->numberVertices; i += 3)
    	{
    		s1 = shape->texCoords[2*(i+1)] - shape->texCoords[2*i];
    		t1 = shape->texCoords[2*(i+1)+1] - shape->texCoords[2*i+1];
    		s2 = shape->texCoords[2*(i+2)] - shape->texCoords[2*i];
    		t2 = shape->texCoords[2*(i+2)+1] - shape->texCoords[2*i+1];

    		scalar = 1.0f / (s1*t2-s2*t1);

    		glusPoint4SubtractPoint4f(Q1, &shape->vertices[4*(i+1)], &shape->vertices[4*i]);
    		Q1[3] = 1.0f;
    		glusPoint4SubtractPoint4f(Q2, &shape->vertices[4*(i+2)], &shape->vertices[4*i]);
    		Q2[3] = 1.0f;

    		tangent[0] = scalar * (t2 * Q1[0] - t1 * Q2[0]);
    		tangent[1] = scalar * (t2 * Q1[1] - t1 * Q2[1]);
    		tangent[2] = scalar * (t2 * Q1[2] - t1 * Q2[2]);

    		bitangent[0] = scalar * (-s2 * Q1[0] + s1 * Q2[0]);
    		bitangent[1] = scalar * (-s2 * Q1[1] + s1 * Q2[1]);
    		bitangent[2] = scalar * (-s2 * Q1[2] + s1 * Q2[2]);

    		glusVector3Normalizef(tangent);

    		glusVector3Normalizef(bitangent);

        	shape->tangents[3 * i] += tangent[0];
        	shape->tangents[3 * i + 1] += tangent[1];
        	shape->tangents[3 * i + 2] += tangent[2];

        	shape->tangents[3 * (i+1)] += tangent[0];
        	shape->tangents[3 * (i+1) + 1] += tangent[1];
        	shape->tangents[3 * (i+1) + 2] += tangent[2];

        	shape->tangents[3 * (i+2)] += tangent[0];
        	shape->tangents[3 * (i+2) + 1] += tangent[1];
        	shape->tangents[3 * (i+2) + 2] += tangent[2];


        	shape->bitangents[3 * i] += bitangent[0];
        	shape->bitangents[3 * i + 1] += bitangent[1];
        	shape->bitangents[3 * i + 2] += bitangent[2];

        	shape->bitangents[3 * (i+1)] += bitangent[0];
        	shape->bitangents[3 * (i+1) + 1] += bitangent[1];
        	shape->bitangents[3 * (i+1) + 2] += bitangent[2];

        	shape->bitangents[3 * (i+2)] += bitangent[0];
        	shape->bitangents[3 * (i+2) + 1] += bitangent[1];
        	shape->bitangents[3 * (i+2) + 2] += bitangent[2];
    	}
    }

    // Normalize, as several triangles have added a vector
    for (i = 0; i < shape->numberVertices; i++)
    {
		glusVector3Normalizef(&(shape->tangents[i * 3]));
		glusVector3Normalizef(&(shape->bitangents[i * 3]));
    }

    return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusShapeCopyf(GLUSshape* shape, const GLUSshape* source)
{
	GLUSuint stride = 4 + 3 + 3 + 3 + 2;

	if (!shape || !source)
	{
		return GLUS_FALSE;
	}

    glusShapeInitf(shape);

    shape->numberVertices = source->numberVertices;
    shape->numberIndices = source->numberIndices;
    shape->mode = source->mode;

    if (source->vertices)
    {
    	shape->vertices = (GLUSfloat*) glusMemoryMalloc(4 * source->numberVertices * sizeof(GLUSfloat));
        if (!shape->vertices)
        {
        	glusShapeDestroyf(shape);

            return GLUS_FALSE;
        }
        memcpy(shape->vertices, source->vertices, 4 * source->numberVertices * sizeof(GLUSfloat));
    }
    if (source->normals)
    {
    	shape->normals = (GLUSfloat*) glusMemoryMalloc(3 * source->numberVertices * sizeof(GLUSfloat));
        if (!shape->normals)
        {
        	glusShapeDestroyf(shape);

            return GLUS_FALSE;
        }
        memcpy(shape->normals, source->normals, 3 * source->numberVertices * sizeof(GLUSfloat));
    }
    if (source->tangents)
    {
    	shape->tangents = (GLUSfloat*) glusMemoryMalloc(3 * source->numberVertices * sizeof(GLUSfloat));
        if (!shape->tangents)
        {
        	glusShapeDestroyf(shape);

            return GLUS_FALSE;
        }
        memcpy(shape->tangents, source->tangents, 3 * source->numberVertices * sizeof(GLUSfloat));
    }
    if (source->bitangents)
    {
    	shape->bitangents = (GLUSfloat*) glusMemoryMalloc(3 * source->numberVertices * sizeof(GLUSfloat));
        if (!shape->bitangents)
        {
        	glusShapeDestroyf(shape);

            return GLUS_FALSE;
        }
        memcpy(shape->bitangents, source->bitangents, 3 * source->numberVertices * sizeof(GLUSfloat));
    }
    if (source->texCoords)
    {
    	shape->texCoords = (GLUSfloat*) glusMemoryMalloc(2 * source->numberVertices * sizeof(GLUSfloat));
        if (!shape->texCoords)
        {
        	glusShapeDestroyf(shape);

            return GLUS_FALSE;
        }
        memcpy(shape->texCoords, source->texCoords, 2 * source->numberVertices * sizeof(GLUSfloat));
    }
    if (source->allAttributes)
    {
    	shape->allAttributes = (GLUSfloat*) glusMemoryMalloc(stride * source->numberVertices * sizeof(GLUSfloat));
        if (!shape->allAttributes)
        {
        	glusShapeDestroyf(shape);

            return GLUS_FALSE;
        }
        memcpy(shape->allAttributes, source->allAttributes, stride * source->numberVertices * sizeof(GLUSfloat));
    }
    if (source->indices)
    {
    	shape->indices = (GLUSindex*) glusMemoryMalloc(source->numberIndices * sizeof(GLUSindex));
        if (!shape->indices)
        {
        	glusShapeDestroyf(shape);

            return GLUS_FALSE;
        }
        memcpy(shape->indices, source->indices, source->numberIndices * sizeof(GLUSindex));
    }

    return GLUS_TRUE;
}

GLUSvoid GLUSAPIENTRY glusShapeDestroyf(GLUSshape* shape)
{
    if (!shape)
    {
        return;
    }

    if (shape->vertices)
    {
        glusMemoryFree(shape->vertices);

        shape->vertices = 0;
    }

    if (shape->normals)
    {
        glusMemoryFree(shape->normals);

        shape->normals = 0;
    }

    if (shape->tangents)
    {
        glusMemoryFree(shape->tangents);

        shape->tangents = 0;
    }

    if (shape->bitangents)
    {
        glusMemoryFree(shape->bitangents);

        shape->bitangents = 0;
    }

    if (shape->texCoords)
    {
        glusMemoryFree(shape->texCoords);

        shape->texCoords = 0;
    }

    if (shape->allAttributes)
    {
        glusMemoryFree(shape->allAttributes);

        shape->allAttributes = 0;
    }

    if (shape->indices)
    {
        glusMemoryFree(shape->indices);

        shape->indices = 0;
    }

    shape->numberVertices = 0;
    shape->numberIndices = 0;
    shape->mode = 0;
}
