#include "TREShapeGroup.h"
#include "TREVertexArray.h"
#include "TREVertexStore.h"
#include "TREModel.h"
#include <LDLib/Vector.h>

TREShapeGroup::TREShapeGroup(void)
	:m_vertexStore(NULL),
	m_indices(NULL),
	m_shapesPresent(0)
{
}

TREShapeGroup::TREShapeGroup(const TREShapeGroup &other)
	:TCObject(other),
	m_vertexStore((TREVertexStore *)TCObject::copy(other.m_vertexStore)),
	m_indices((TCULongArrayArray *)TCObject::copy(other.m_indices)),
	m_shapesPresent(other.m_shapesPresent)
{
}

TREShapeGroup::~TREShapeGroup(void)
{
}

void TREShapeGroup::dealloc(void)
{
	TCObject::release(m_vertexStore);
	TCObject::release(m_indices);
	TCObject::dealloc();
}

void TREShapeGroup::addShapeType(TREShapeType shapeType, int index)
{
	TCULongArray *newIntArray = new TCULongArray;

	if (!m_indices)
	{
		m_indices = new TCULongArrayArray;
	}
	m_indices->insertObject(newIntArray, index);
	newIntArray->release();
	m_shapesPresent |= shapeType;
}

TCULongArray *TREShapeGroup::getIndices(TREShapeType shapeType, bool create)
{
	TCULong index = getIndex(shapeType);

	if (!(m_shapesPresent & shapeType))
	{
		if (create)
		{
			addShapeType(shapeType, index);
		}
		else
		{
			return NULL;
		}
	}
	return m_indices->objectAtIndex(index);
}

TCULong TREShapeGroup::getIndex(TREShapeType shapeType)
{
	int bit;
	TCULong index = 0;

	for (bit = 1; bit != shapeType; bit = bit << 1)
	{
		if (m_shapesPresent & bit)
		{
			index++;
		}
	}
	return index;
}

/*
void TREShapeGroup::addConditionalLine(int index1, int index2, int index3,
									   int index4)
{
	addShape(TRESConditionalLine, index1, index2, index3, index4);
}

void TREShapeGroup::addBFCTriangle(int index1, int index2, int index3)
{
	addShape(TRESBFCTriangle, index1, index2, index3);
}

void TREShapeGroup::addBFCQuad(int index1, int index2, int index3, int index4)
{
	addShape(TRESBFCQuad, index1, index2, index3, index4);
}
*/

void TREShapeGroup::addShape(TREShapeType shapeType, int firstIndex, int count)
{
	TCULongArray *indices = getIndices(shapeType, true);
	int i;

	for (i = 0; i < count; i++)
	{
		indices->addValue(firstIndex + i);
	}
}

/*
void TREShapeGroup::addShape(TREShapeType shapeType, int index1, int index2)
{
	TCULongArray *indices = getIndices(shapeType, true);

	indices->addValue(index1);
	indices->addValue(index2);
}

void TREShapeGroup::addShape(TREShapeType shapeType, int index1, int index2,
							 int index3)
{
	TCULongArray *indices = getIndices(shapeType, true);

	indices->addValue(index1);
	indices->addValue(index2);
	indices->addValue(index3);
}

void TREShapeGroup::addShape(TREShapeType shapeType, int index1, int index2,
							 int index3, int index4)
{
	TCULongArray *indices = getIndices(shapeType, true);

	indices->addValue(index1);
	indices->addValue(index2);
	indices->addValue(index3);
	indices->addValue(index4);
}
*/

GLenum TREShapeGroup::modeForShapeType(TREShapeType shapeType)
{
	switch (shapeType)
	{
	case TRESLine:
		return GL_LINES;
		break;
	case TRESTriangle:
		return GL_TRIANGLES;
		break;
	case TRESQuad:
		return GL_QUADS;
		break;
	case TRESConditionalLine:
		return GL_LINES;
		break;
	case TRESBFCTriangle:
		return GL_TRIANGLES;
		break;
	case TRESBFCQuad:
		return GL_QUADS;
		break;
	case TRESTriangleStrip:
		return GL_TRIANGLE_STRIP;
		break;
	case TRESQuadStrip:
		return GL_QUAD_STRIP;
		break;
	case TRESTriangleFan:
		return GL_TRIANGLE_FAN;
		break;
	default:
		// We shouldn't ever get here.
		return GL_TRIANGLES;
		break;
	}
}

int TREShapeGroup::numPointsForShapeType(TREShapeType shapeType)
{
	switch (shapeType)
	{
	case TRESLine:
		return 2;
		break;
	case TRESTriangle:
		return 3;
		break;
	case TRESQuad:
		return 4;
		break;
	case TRESConditionalLine:
		return 4;
		break;
	case TRESBFCTriangle:
		return 3;
		break;
	case TRESBFCQuad:
		return 4;
		break;
	default:
		// Strips are variable size
		return 0;
		break;
	}
}

void TREShapeGroup::drawShapeType(TREShapeType shapeType)
{
	TCULongArray *indexArray = getIndices(shapeType);

	if (indexArray)
	{
		glDrawElements(modeForShapeType(shapeType), indexArray->getCount(),
			GL_UNSIGNED_INT, indexArray->getValues());
	}
}

void TREShapeGroup::drawStripShapeType(TREShapeType shapeType)
{
	TCULongArray *indexArray = getIndices(shapeType);

	if (indexArray)
	{
		TCULong *indices = indexArray->getValues();
		int totalIndices = indexArray->getCount();
		int stripCount = 0;
		int i;
		GLenum glMode = modeForShapeType(shapeType);

		for (i = 0; i < totalIndices; i += stripCount)
		{
			stripCount = indices[i];
			i++;
			glDrawElements(glMode, stripCount, GL_UNSIGNED_INT, indices + i);
		}
	}
}

void TREShapeGroup::draw(void)
{
	if (m_vertexStore)
	{
//		m_vertexStore->activate();
		drawShapeType(TRESTriangle);
		drawShapeType(TRESQuad);
		drawStripShapeType(TRESTriangleStrip);
		drawStripShapeType(TRESQuadStrip);
		drawStripShapeType(TRESTriangleFan);
	}
}

int TREShapeGroup::addLine(Vector *vertices)
{
	int index;

	m_vertexStore->setup();
	index = m_vertexStore->addVertices(vertices, 3);
	addShape(TRESLine, index, 2);
	return index;
}

int TREShapeGroup::addTriangle(Vector *vertices)
{
	int index;

	m_vertexStore->setup();
	index = m_vertexStore->addVertices(vertices, 3);
	addShape(TRESTriangle, index, 3);
	return index;
}

int TREShapeGroup::addQuad(Vector *vertices)
{
	int index;

	m_vertexStore->setup();
	index = m_vertexStore->addVertices(vertices, 4);
	addShape(TRESQuad, index, 4);
	return index;
}

int TREShapeGroup::addQuadStrip(Vector *vertices, Vector *normals, int count)
{
	return addStrip(TRESQuadStrip, vertices, normals, count);
}

int TREShapeGroup::addTriangleFan(Vector *vertices, Vector *normals, int count)
{
	return addStrip(TRESTriangleFan, vertices, normals, count);
}

int TREShapeGroup::addStrip(TREShapeType shapeType, Vector *vertices,
							Vector *normals, int count)
{
	int index;

	m_vertexStore->setup();
	index = m_vertexStore->addVertices(vertices, normals, count);
	addShape(shapeType, count, 1);
	addShape(shapeType, index, count);
	return index;
}

void TREShapeGroup::setVertexStore(TREVertexStore *vertexStore)
{
	vertexStore->retain();
	TCObject::release(m_vertexStore);
	m_vertexStore = vertexStore;
}

void TREShapeGroup::getMinMax(Vector& min, Vector& max, float* matrix)
{
	int bit;

	for (bit = 1; (TREShapeType)bit < TRESFirstStrip; bit = bit << 1)
	{
		getMinMax(getIndices((TREShapeType)bit), min, max, matrix);
	}
	for (; (TREShapeType)bit < TRESLast; bit = bit << 1)
	{
		getStripMinMax(getIndices((TREShapeType)bit), min, max, matrix);
	}
}

void TREShapeGroup::getMinMax(TCULongArray *indices, Vector& min, Vector& max,
							  float* matrix)
{
	if (indices)
	{
		int i;
		int count = indices->getCount();

		for (i = 0; i < count; i++)
		{
			getMinMax((*indices)[i], min, max, matrix);
		}
	}
}

void TREShapeGroup::getStripMinMax(TCULongArray *indices, Vector& min,
								   Vector& max, float* matrix)
{
	if (indices)
	{
		int i, j;
		int count = indices->getCount();
		int stripCount = 0;

		for (i = 0; i < count; i += stripCount)
		{
			stripCount = (*indices)[i];
			i++;
			for (j = 0; j < stripCount; j++)
			{
				getMinMax((*indices)[i + j], min, max, matrix);
			}
		}
	}
}

void TREShapeGroup::getMinMax(const TREVertex &vertex, Vector& min, Vector& max)
{
	Vector point = Vector(vertex.v[0], vertex.v[1], vertex.v[2]);

	if (point[0] < min[0])
	{
		min[0] = point[0];
	}
	if (point[1] < min[1])
	{
		min[1] = point[1];
	}
	if (point[2] < min[2])
	{
		min[2] = point[2];
	}
	if (point[0] > max[0])
	{
		max[0] = point[0];
	}
	if (point[1] > max[1])
	{
		max[1] = point[1];
	}
	if (point[2] > max[2])
	{
		max[2] = point[2];
	}
}

void TREShapeGroup::getMinMax(TCULong index, Vector& min, Vector& max,
							  float* matrix)
{
	TREVertex vertex = (*m_vertexStore->getVertices())[index];

	TREModel::transformVertex(vertex, matrix);
	getMinMax(vertex, min, max);
}
