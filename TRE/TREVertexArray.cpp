#include "TREVertexArray.h"

#include <string.h>

TREVertexArray::TREVertexArray(unsigned int allocated)
	:m_vertices(NULL),
	m_count(0),
	m_allocated(allocated)
{
	if (m_allocated)
	{
		m_vertices = new TREVertex[allocated];
	}
}

TREVertexArray::TREVertexArray(const TREVertexArray &other)
	:m_vertices(NULL),
	m_count(other.m_count),
	m_allocated(other.m_count)
{
	if (m_allocated)
	{
		m_vertices = new TREVertex[m_allocated];
		memcpy(m_vertices, other.m_vertices, m_count * sizeof(TREVertex));
	}
}

TREVertexArray::~TREVertexArray(void)
{
}

void TREVertexArray::dealloc(void)
{
	delete m_vertices;
	TCObject::dealloc();
}

TCObject *TREVertexArray::copy(void)
{
	return new TREVertexArray(*this);
}

bool TREVertexArray::addVertex(const TREVertex &vertex)
{
	return insertVertex(vertex, m_count);
}

bool TREVertexArray::setCapacity(unsigned int newCapacity)
{
	if (newCapacity >= m_count)
	{
		TREVertex *newVertices = new TREVertex[newCapacity];

		m_allocated = newCapacity;
		if (m_count)
		{
			memcpy(m_vertices, newVertices, m_count * sizeof(TREVertex));
		}
		delete m_vertices;
		m_vertices = newVertices;
		return true;
	}
	else
	{
		return false;
	}
}

bool TREVertexArray::insertVertex(const TREVertex &vertex, unsigned int index)
{
	bool expanded = false;
	TREVertex *oldVertices = NULL;

	if (index > m_count)
	{
		return false;
	}
	if (m_count + 1 > m_allocated)
	{
		TREVertex* newVertices;

		expanded = true;
		if (m_allocated)
		{
			m_allocated *= 2;
		}
		else
		{
			m_allocated = 4;
		}
		newVertices = new TREVertex[m_allocated];
		if (index)
		{
			memcpy(newVertices, m_vertices, index * sizeof(TREVertex));
		}
		if (index < m_count)
		{
			memcpy(newVertices + index + 1, m_vertices + index,
				(m_count - index) * sizeof(TREVertex));
		}
		// We can't delete m_vertices, because the incoming vertex could
		// potentially be a const ref to one of the members of the old array.
		// Save the pointer for deletion later.
		oldVertices = m_vertices;
		m_vertices = newVertices;
	}
	if (!expanded && index < m_count)
	{
		memmove(m_vertices + index + 1, m_vertices + index,
			(m_count - index) * sizeof(TREVertex));
	}
	m_vertices[index] = vertex;
	m_count++;
	if (oldVertices)
	{
		delete oldVertices;
	}
	return true;
}

bool TREVertexArray::replaceVertex(const TREVertex &vertex, unsigned int index)
{
	if (index < m_count)
	{
		m_vertices[index] = vertex;
		return true;
	}
	else
	{
		return false;
	}
}

void TREVertexArray::removeAll(void)
{
	while (m_count)
	{
		removeVertex(m_count - 1);
	}
}

bool TREVertexArray::removeVertex(int index)
{
	if (index >= 0 && (unsigned)index < m_count)
	{
		m_count--;
		if ((unsigned)index < m_count)
		{
			memmove(m_vertices + index, m_vertices + index + 1,
				(m_count - index) * sizeof(TREVertex));
		}
		return true;
	}
	else
	{
		return false;
	}
}

TREVertex &TREVertexArray::vertexAtIndex(unsigned int index) const
{
	return m_vertices[index];
}

/*
void TCArray::sortUsingFunction(TCArraySortFunction function)
{
	qsort(items, count, sizeof (void*), function);
}
*/

const TREVertex &TREVertexArray::operator[](unsigned int index) const
{
	return m_vertices[index];
}
