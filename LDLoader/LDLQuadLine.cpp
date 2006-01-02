#include "LDLQuadLine.h"
#include "LDLTriangleLine.h"
#include "LDLMainModel.h"
#include <TCFoundation/TCMacros.h>
#include <TCFoundation/TCLocalStrings.h>

LDLQuadLine::LDLQuadLine(LDLModel *parentModel, const char *line,
						 int lineNumber, const char *originalLine)
	:LDLShapeLine(parentModel, line, lineNumber, originalLine),
	m_colinearIndex(-1),
	m_matchingIndex(-1)
{
}

LDLQuadLine::LDLQuadLine(const LDLQuadLine &other)
	:LDLShapeLine(other),
	m_colinearIndex(-1),
	m_matchingIndex(other.m_matchingIndex)
{
}

bool LDLQuadLine::parse(void)
{
	float x1, y1, z1;
	float x2, y2, z2;
	float x3, y3, z3;
	float x4, y4, z4;
	int lineType;

	if (sscanf(m_line, "%d %li %f %f %f %f %f %f %f %f %f %f %f %f", &lineType,
		&m_colorNumber, &x1, &y1, &z1, &x2, &y2, &z2, &x3, &y3, &z3,
		&x4, &y4, &z4) == 14)
	{
		m_points = new TCVector[4];
		m_points[0] = TCVector(x1, y1, z1);
		m_points[1] = TCVector(x2, y2, z2);
		m_points[2] = TCVector(x3, y3, z3);
		m_points[3] = TCVector(x4, y4, z4);
		if (!getMainModel()->getSkipValidation())
		{
			// Note that we don't care what the second matching index is,
			// because we only need to throw out one of the two points, so don't
			// bother to even read it.
			if (getMatchingPoints(&m_matchingIndex))
			{
				m_valid = false;
			}
			else
			{
				swapPointsIfNeeded();
				checkForColinearPoints();
			}
		}
		return true;
	}
	else
	{
		m_valid = false;
		setError(LDLEParse, TCLocalStrings::get("LDLQuadLineParse"));
		return false;
	}
}

TCObject *LDLQuadLine::copy(void)
{
	return new LDLQuadLine(*this);
}

void LDLQuadLine::swapPointsIfNeeded(void)
{
	if (swapNeeded(0, 1, 2, 3))
	{
		// The original order is wrong; we'll try to fix it to produce a
		// standard convex quad.
		if (!swapNeeded(0, 1, 3, 2))
		{
			reportBadVertexOrder(0, 1, 3, 2);
			swapPoints(2, 3);
		}
		else if (!swapNeeded(0, 2, 1, 3))
		{
			reportBadVertexOrder(0, 2, 1, 3);
			swapPoints(1, 2);
		}
		else if (!swapNeeded(0, 2, 3, 1))
		{
			reportBadVertexOrder(0, 2, 3, 1);
			rotPoints(1, 2, 3);
		}
		else if (!swapNeeded(0, 3, 1, 2))
		{
			reportBadVertexOrder(0, 3, 1, 2);
			rotPoints(1, 3, 2);
		}
		else if (!swapNeeded(0, 3, 2, 1))
		{
			reportBadVertexOrder(0, 3, 2, 1);
			swapPoints(1, 3);
		}
		else
		{
			// None of the possible point orders resulted in an acceptable quad,
			// so this quad must be concave or non-planar.
			m_valid = false;
		}
		m_actionFlags.bfcClip = false;
	}
}

void LDLQuadLine::swapPoints(int index1, int index2)
{
	TCVector tmp = m_points[index1];

	m_points[index1] = m_points[index2];
	m_points[index2] = tmp;
}

void LDLQuadLine::rotPoints(int index1, int index2, int index3)
{
	TCVector tmp = m_points[index1];

	m_points[index1] = m_points[index2];
	m_points[index2] = m_points[index3];
	m_points[index3] = tmp;
}

bool LDLQuadLine::swapNeeded(int index1, int index2, int index3, int index4)
{
	TCVector &p1 = m_points[index1];
	TCVector &p2 = m_points[index2];
	TCVector &p3 = m_points[index3];
	TCVector &p4 = m_points[index4];
	TCVector normal1 = (p1 - p4) * (p1 - p2);
	TCVector normal2 = (p2 - p1) * (p2 - p3);
	TCVector normal3 = (p3 - p2) * (p3 - p4);
	TCVector normal4 = (p4 - p3) * (p4 - p1);
	TCFloat length1 = normal1.length();
	TCFloat length2 = normal2.length();
	TCFloat length3 = normal3.length();
	TCFloat length4 = normal4.length();
	TCFloat dotProduct;
	bool nonFlat = false;

	if (fEq(length1, 0.0) || fEq(length2, 0.0) ||
		fEq(length3, 0.0) || fEq(length4, 0.0))
	{
		return 0;
	}
	normal1 /= length1;
	normal2 /= length2;
	normal3 /= length3;
	normal4 /= length4;
	if ((dotProduct = normal1.dot(normal2)) <= 0.0)
	{
		return true;
	}
	if (dotProduct <= 0.9)
	{
		nonFlat = true;
	}
	if ((dotProduct = normal1.dot(normal3)) <= 0.0)
	{
		return true;
	}
	if (dotProduct <= 0.9)
	{
		nonFlat = true;
	}
	if ((dotProduct = normal1.dot(normal4)) <= 0.0)
	{
		return true;
	}
	if (dotProduct <= 0.9)
	{
		nonFlat = true;
	}
	if ((dotProduct = normal2.dot(normal3)) <= 0.0)
	{
		return true;
	}
	if (dotProduct <= 0.9)
	{
		nonFlat = true;
	}
	if ((dotProduct = normal2.dot(normal4)) <= 0.0)
	{
		return true;
	}
	if (dotProduct <= 0.9)
	{
		nonFlat = true;
	}
	if ((dotProduct = normal3.dot(normal4)) <= 0.0)
	{
		return true;
	}
	if (dotProduct <= 0.9)
	{
		nonFlat = true;
	}
	if (nonFlat)
	{
		setWarning(LDLENonFlatQuad,
			TCLocalStrings::get("LDLQuadLineNonFlatError"));
	}
	return false;
}

void LDLQuadLine::checkForColinearPoints(void)
{
	m_colinearIndex = getColinearIndex();
	if (m_colinearIndex >= 0)
	{
		m_valid = false;
	}
}

int LDLQuadLine::getColinearIndex(void)
{
	TCVector &p1 = m_points[0];
	TCVector &p2 = m_points[1];
	TCVector &p3 = m_points[2];
	TCVector &p4 = m_points[3];
	TCVector normal = (p1 - p4) * (p1 - p2);

	if (normal.lengthSquared() == 0.0f)
	{
		int index = middleIndex(p1, p2, p4);

		switch (index)
		{
		case 0:
			return 0;
		case 1:
			return 1;
		case 2:
			return 3;
		default:
			return 0;
		}
	}
	normal = (p2 - p1) * (p2 - p3);
	if (normal.lengthSquared() == 0.0f)
	{
		return middleIndex(p1, p2, p3);
	}
	normal = (p3 - p2) * (p3 - p4);
	if (normal.lengthSquared() == 0.0f)
	{
		return middleIndex(p2, p3, p4) + 1;
	}
	normal = (p4 - p3) * (p4 - p1);
	if (normal.lengthSquared() == 0.0f)
	{
		int index = middleIndex(p1, p3, p4);

		switch (index)
		{
		case 0:
			return 0;
		case 1:
			return 2;
		case 2:
			return 3;
		default:
			return 0;
		}
	}
	return -1;
}

LDLFileLineArray *LDLQuadLine::getReplacementLines(void)
{
	if (m_valid)
	{
		return NULL;
	}
	else
	{
		if (m_matchingIndex >= 0)
		{
			return removeMatchingPoint();
		}
		if (m_colinearIndex >= 0)
		{
			return removeColinearPoint();
		}
		else
		{
			return splitConcaveQuad();
		}
	}
}

LDLFileLineArray *LDLQuadLine::removePoint(int index)
{
	LDLFileLineArray *fileLineArray = NULL;
	LDLTriangleLine *triangleLine = NULL;
	TCVector &p1 = m_points[0];
	TCVector &p2 = m_points[1];
	TCVector &p3 = m_points[2];
	TCVector &p4 = m_points[3];

	switch (index)
	{
	case 0:
		triangleLine = newTriangleLine(p2, p3, p4);
		break;
	case 1:
		triangleLine = newTriangleLine(p1, p3, p4);
		break;
	case 2:
		triangleLine = newTriangleLine(p1, p2, p4);
		break;
	case 3:
		triangleLine = newTriangleLine(p1, p2, p3);
		break;
	default:
		break;
	}
	if (triangleLine)
	{
		fileLineArray = new LDLFileLineArray(1);
		fileLineArray->addObject(triangleLine);
		triangleLine->release();
	}
	return fileLineArray;
}

LDLFileLineArray *LDLQuadLine::removeMatchingPoint(void)
{
	LDLFileLineArray *fileLineArray = removePoint(m_matchingIndex);

	if (fileLineArray)
	{
		char pointBuf[64] = "";

		m_points[m_matchingIndex].print(pointBuf);
		setWarning(LDLEMatchingPoints,
			TCLocalStrings::get("LDLQuadLineIdentical"), m_matchingIndex + 1,
			pointBuf);
	}
	else
	{
		setError(LDLEGeneral, TCLocalStrings::get("LDLQuadLineIdenticalError"));
	}
	return fileLineArray;
}

LDLFileLineArray *LDLQuadLine::removeColinearPoint(void)
{
	LDLFileLineArray *fileLineArray = removePoint(m_colinearIndex);

	if (fileLineArray)
	{
		char pointBuf[64] = "";

		m_points[m_colinearIndex].print(pointBuf);
		setWarning(LDLEColinear, TCLocalStrings::get("LDLQuadLineCoLinear"),
			m_colinearIndex + 1, pointBuf);
	}
	else
	{
		setError(LDLEGeneral, TCLocalStrings::get("LDLQuadLineCoLinearError"));
	}
	return fileLineArray;
}

LDLFileLineArray *LDLQuadLine::splitConcaveQuad(void)
{
	LDLFileLineArray *fileLineArray = splitConcaveQuad(0, 1, 2, 3);

	if (!fileLineArray)
	{
		fileLineArray = splitConcaveQuad(0, 1, 3, 2);
	}
	if (!fileLineArray)
	{
		fileLineArray = splitConcaveQuad(0, 2, 1, 3);
	}
	if (!fileLineArray)
	{
		fileLineArray = splitConcaveQuad(0, 2, 3, 1);
	}
	if (!fileLineArray)
	{
		fileLineArray = splitConcaveQuad(0, 3, 1, 2);
	}
	if (!fileLineArray)
	{
		fileLineArray = splitConcaveQuad(0, 3, 2, 1);
	}
	if (!fileLineArray)
	{
		// All split attempts failed, the quad must not be flat.
		setError(LDLEConcaveQuad,
			TCLocalStrings::get("LDLQuadLineConcaveError"));
	}
	return fileLineArray;
}

LDLFileLineArray *LDLQuadLine::splitConcaveQuad(int index1, int index2,
												int index3, int index4)
{
	TCVector &p1 = m_points[index1];
	TCVector &p2 = m_points[index2];
	TCVector &p3 = m_points[index3];
	TCVector &p4 = m_points[index4];
	TCVector normal1 = (p1 - p4) * (p1 - p2);
	TCVector normal2 = (p2 - p1) * (p2 - p3);
	TCVector normal3 = (p3 - p2) * (p3 - p4);
	TCVector normal4 = (p4 - p3) * (p4 - p1);
	TCFloat length1 = normal1.length();
	TCFloat length2 = normal2.length();
	TCFloat length3 = normal3.length();
	TCFloat length4 = normal4.length();
	TCFloat dotProduct;
	LDLTriangleLine *triangle1 = NULL;
	LDLTriangleLine *triangle2 = NULL;
	LDLFileLineArray *fileLineArray = NULL;

	if (fEq(length1, 0.0) || fEq(length2, 0.0) ||
		fEq(length3, 0.0) || fEq(length4, 0.0))
	{
		// Ack!
		return NULL;
	}
	normal1 /= length1;
	normal2 /= length2;
	normal3 /= length3;
	normal4 /= length4;
	if ((dotProduct = normal1.dot(normal2)) <= 0.0)
	{
		triangle1 = newTriangleLine(p1, p2, p4);
		triangle2 = newTriangleLine(p2, p3, p4);
		reportQuadSplit(dotProduct < -0.9, p1, p2, p3, p4, p1, p2, p4, p2, p3,
			p4);
	}
	else if ((dotProduct = normal2.dot(normal3)) <= 0.0)
	{
		triangle1 = newTriangleLine(p1, p2, p3);
		triangle2 = newTriangleLine(p1, p3, p4);
		reportQuadSplit(dotProduct < -0.9, p1, p2, p3, p4, p1, p2, p3, p1, p3,
			p4);
	}
	else if ((dotProduct = normal3.dot(normal4)) <= 0.0)
	{
		triangle1 = newTriangleLine(p1, p2, p4);
		triangle2 = newTriangleLine(p2, p3, p4);
		reportQuadSplit(dotProduct < -0.9, p1, p2, p3, p4, p1, p2, p4, p2, p3,
			p4);
	}
	else if ((dotProduct = normal4.dot(normal1)) <= 0.0)
	{
		triangle1 = newTriangleLine(p1, p2, p3);
		triangle2 = newTriangleLine(p1, p3, p4);
		reportQuadSplit(dotProduct < -0.9, p1, p2, p3, p4, p1, p2, p3, p1, p3,
			p4);
	}
	if (triangle1)
	{
		fileLineArray = new LDLFileLineArray(2);
		fileLineArray->addObject(triangle1);
		fileLineArray->addObject(triangle2);
		triangle1->release();
		triangle2->release();
	}
	return fileLineArray;
}

void LDLQuadLine::reportBadVertexOrder(int index1, int index2, int index3,
									   int index4)
{
	char oldBuf[4][64];
	char newBuf[4][64];
	int indices[4];
	int i;

	indices[0] = index1;
	indices[1] = index2;
	indices[2] = index3;
	indices[3] = index4;
	for (i = 0; i < 4; i++)
	{
		m_points[i].print(oldBuf[i]);
		m_points[indices[i]].print(newBuf[i]);
	}
	if (m_actionFlags.bfcClip)
	{
		setError(LDLEVertexOrder,
			TCLocalStrings::get("LDLQuadLineBfcBadVertSeq"), oldBuf[0],
			oldBuf[1], oldBuf[2], oldBuf[3], newBuf[0], newBuf[1], newBuf[2],
			newBuf[3]);
	}
	else
	{
		setWarning(LDLEVertexOrder,
			TCLocalStrings::get("LDLQuadLineBadVertSeq"), oldBuf[0], oldBuf[1],
			oldBuf[2], oldBuf[3], newBuf[0], newBuf[1], newBuf[2], newBuf[3]);
	}
}

void LDLQuadLine::reportQuadSplit(bool flat, const TCVector& q1,
								  const TCVector& q2, const TCVector& q3,
								  const TCVector& q4, const TCVector& t1,
								  const TCVector& t2, const TCVector& t3,
								  const TCVector& t4, const TCVector& t5,
								  const TCVector& t6)
{
	char q1Buf[64], q2Buf[64], q3Buf[64], q4Buf[64];
	char t1Buf[64], t2Buf[64], t3Buf[64], t4Buf[64], t5Buf[64], t6Buf[64];
	const char *errorTypeString = TCLocalStrings::get("LDLQuadLineConcave");
	LDLErrorType errorType = LDLEConcaveQuad;

	if (!flat)
	{
		errorTypeString = TCLocalStrings::get("LDLQuadLineNonFlat");
		errorType = LDLENonFlatQuad;
	}
	q1.print(q1Buf);
	q2.print(q2Buf);
	q3.print(q3Buf);
	q4.print(q4Buf);
	t1.print(t1Buf);
	t2.print(t2Buf);
	t3.print(t3Buf);
	t4.print(t4Buf);
	t5.print(t5Buf);
	t6.print(t6Buf);
	setWarning(errorType, TCLocalStrings::get("LDLQuadLineSpit"),
		errorTypeString, q1Buf, q2Buf, q3Buf, q4Buf, t1Buf, t2Buf, t3Buf, t4Buf,
		t5Buf, t6Buf);
}

LDLTriangleLine *LDLQuadLine::newTriangleLine(const TCVector &p1,
											  const TCVector &p2,
											  const TCVector &p3)
{
	char pointBuf1[64] = "";
	char pointBuf2[64] = "";
	char pointBuf3[64] = "";
	char newLine[1024];

	p1.print(pointBuf1, 8);
	p2.print(pointBuf2, 8);
	p3.print(pointBuf3, 8);
	sprintf(newLine, "3 %ld %s %s %s", m_colorNumber, pointBuf1, pointBuf2,
		pointBuf3);
	return new LDLTriangleLine(m_parentModel, newLine, m_lineNumber, m_line);
}
