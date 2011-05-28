#include "LD3dsExporter.h"
#include <LDLoader/LDLMainModel.h>
#include <LDLoader/LDLModelLine.h>
#include <LDLoader/LDLTriangleLine.h>
#include <LDLoader/LDLQuadLine.h>
#include <LDLoader/LDLPalette.h>
#include <lib3ds.h>

#if defined WIN32 && defined(_MSC_VER) && _MSC_VER >= 1400 && defined(_DEBUG)
#define new DEBUG_CLIENTBLOCK
#endif

LD3dsExporter::LD3dsExporter(void):
LDExporter("3dsExporter/")
{
	loadSettings();
}

LD3dsExporter::~LD3dsExporter(void)
{
}

void LD3dsExporter::initSettings(void) const
{
	addSetting(LDExporterSetting(ls(_UC("3dsSeams")), m_seams,
		udKey("Seams").c_str()));
	m_settings.back().setGroupSize(1);
	addSetting(LDExporterSetting(ls(_UC("3dsSeamWidth")), m_seamWidth,
		udKey("SeamWidth").c_str()));
}

void LD3dsExporter::dealloc(void)
{
	LDExporter::dealloc();
}

ucstring LD3dsExporter::getTypeDescription(void) const
{
	return ls(_UC("3dsTypeDescription"));
}

void LD3dsExporter::loadSettings(void)
{
	LDExporter::loadSettings();
	m_seams = boolForKey("Seams", true);
	m_seamWidth = floatForKey("SeamWidth", 0.5);
	//m_includeCamera = boolForKey("IncludeCamera", true);
}

int LD3dsExporter::getMaterial(int colorNumber)
{
	IntIntMap::iterator it = m_colorNumbers.find(colorNumber);

	if (it == m_colorNumbers.end())
	{
		int material = (int)m_colorNumbers.size();
		Lib3dsMaterial *mat = lib3ds_material_new((std::string("ldraw_") +
			ltostr(colorNumber)).c_str());
        lib3ds_file_insert_material(m_file, mat, -1);
		int r, g, b, a;
		LDLPalette *pPalette = m_topModel->getMainModel()->getPalette();
		pPalette->getRGBA(colorNumber, r, g, b, a);
		LDLColorInfo colorInfo = pPalette->getAnyColorInfo(colorNumber);

        mat->diffuse[0] = r / 255.0f;
        mat->diffuse[1] = g / 255.0f;
        mat->diffuse[2] = b / 255.0f;
		mat->transparency = 1.0f - a / 255.0f;
		mat->two_sided = 1;
		if (colorInfo.rubber)
		{
			mat->specular[0] = mat->specular[1] = mat->specular[2] = 0.05f;
		}
		mat->shading = LIB3DS_SHADING_PHONG;
		m_colorNumbers[colorNumber] = material;
		return material;
	}
	else
	{
		return it->second;
	}
}

void LD3dsExporter::writeTriangle(
	Lib3dsMesh *pMesh,
	const TCVector *points,
	int i0,
	int i1,
	int i2,
	int colorNumber,
	const TCFloat *matrix)
{
	int ix[3];
	int voffset = pMesh->nvertices;
	int foffset = pMesh->nfaces;

	ix[0] = i0;
	ix[1] = i1;
	ix[2] = i2;
	lib3ds_mesh_resize_vertices(pMesh, voffset + 3, 0, 0);
	lib3ds_mesh_resize_faces(pMesh, foffset + 1);
	for (int i = 0; i < 3; i++)
	{
		TCVector vector = points[ix[i]];

		vector = vector.transformPoint(matrix);
		pMesh->vertices[voffset + i][0] = vector[0];
		pMesh->vertices[voffset + i][1] = vector[1];
		pMesh->vertices[voffset + i][2] = vector[2];
		pMesh->faces[foffset].index[i] = (unsigned short)(voffset + i);
		pMesh->faces[foffset].material = getMaterial(colorNumber);
	}
}

bool LD3dsExporter::shouldFlipWinding(
	bool bfc,
	bool invert,
	LDLShapeLine *pShapeLine)
{
	if (bfc)
	{
		if (invert)
		{
			return pShapeLine->getBFCWindingCCW();
		}
		else
		{
			return !pShapeLine->getBFCWindingCCW();
		}
	}
	else
	{
		return invert;
	}
}

void LD3dsExporter::writeShapeLine(
	Lib3dsMesh *pMesh,
	LDLShapeLine *pShapeLine,
	const TCFloat *matrix,
	int colorNumber,
	bool bfc,
	bool invert)
{
	if (pShapeLine->getColorNumber() != 16)
	{
		colorNumber = pShapeLine->getColorNumber();
	}
	if (shouldFlipWinding(bfc, invert, pShapeLine))
	{
		writeTriangle(pMesh, pShapeLine->getPoints(), 2, 1, 0, colorNumber,
			matrix);
	}
	else
	{
		writeTriangle(pMesh, pShapeLine->getPoints(), 0, 1, 2, colorNumber,
			matrix);
	}
	if (pShapeLine->getNumPoints() > 3)
	{
		if (shouldFlipWinding(bfc, invert, pShapeLine))
		{
			writeTriangle(pMesh, pShapeLine->getPoints(), 3, 2, 0, colorNumber,
				matrix);
		}
		else
		{
			writeTriangle(pMesh, pShapeLine->getPoints(), 0, 2, 3, colorNumber,
				matrix);
		}
	}
}

std::string LD3dsExporter::getMeshName(LDLModel *model)
{
	std::string name;
	StringIntMap::iterator it;
	int count;

	if (model != NULL)
	{
		char *filename = filenameFromPath(model->getFilename());
		size_t dotSpot;

		name = filename;
		dotSpot = name.rfind('.');
		delete filename;
		if (dotSpot < name.size())
		{
			name = name.substr(0, dotSpot);
		}
	}
	else
	{
		name = "no_name";
	}
	it = m_names.find(name);
	if (it == m_names.end())
	{
		m_names[name] = 1;
		count = 1;
	}
	else
	{
		count = ++it->second;
	}
	name += " ";
	name += ltostr(count);
	return name;
}

void LD3dsExporter::doExport(
	LDLModel *pModel,
	Lib3dsNode *pParentNode,
	const TCFloat *matrix,
	int colorNumber,
	bool inPart,
	bool bfc,
	bool invert)
{
	LDLFileLineArray *pFileLines = pModel->getFileLines();

	if (pFileLines != NULL)
	{
		BFCState newBfcState = pModel->getBFCState();
		int count = pModel->getActiveLineCount();
		std::string meshName;
		Lib3dsMesh *pMesh;
		Lib3dsNode *pChildNode = NULL;
		Lib3dsMeshInstanceNode *pInst;
		bool linesInvert = invert;

		if (TCVector::determinant(matrix) < 0.0f)
		{
			linesInvert = !linesInvert;
		}
		bfc = (bfc && newBfcState == BFCOnState) ||
			newBfcState == BFCForcedOnState;
		meshName = getMeshName(pModel);
		pMesh = lib3ds_mesh_new(meshName.c_str());
		lib3ds_file_insert_mesh(m_file, pMesh, -1);
		pInst = lib3ds_node_new_mesh_instance(pMesh,
			(meshName + "n").c_str(), NULL, NULL, NULL);
		lib3ds_file_append_node(m_file, (Lib3dsNode *)pInst, pParentNode);
		pChildNode = (Lib3dsNode *)pInst;
		for (int i = 0; i < count; i++)
		{
			LDLFileLine *pFileLine = (*pFileLines)[i];
			if (!pFileLine->isValid())
			{
				continue;
			}
			switch (pFileLine->getLineType())
			{
			case LDLLineTypeTriangle:
			case LDLLineTypeQuad:
				writeShapeLine(pMesh, (LDLShapeLine *)pFileLine, matrix,
					colorNumber, bfc, linesInvert);
				break;
			case LDLLineTypeModel:
				{
					LDLModelLine *pModelLine = (LDLModelLine *)pFileLine;
					LDLModel *pOtherModel = pModelLine->getModel(true);
					if (pOtherModel != NULL)
					{
						TCFloat newMatrix[16];
						int otherColorNumber = pModelLine->getColorNumber();
						bool otherInPart = inPart;
						bool otherInvert = invert;
						
						if (pModelLine->getBFCInvert())
						{
							otherInvert = !otherInvert;
						}
						if (otherColorNumber == 16)
						{
							otherColorNumber = colorNumber;
						}
						TCVector::multMatrix(matrix, pModelLine->getMatrix(),
							newMatrix);
						if (!inPart && pOtherModel->isPart() && m_seams)
						{
							TCVector min, max;
							TCFloat seamMatrix[16];
							TCFloat tempMatrix[16];

							pOtherModel->getBoundingBox(min, max);
							TCVector::calcScaleMatrix(m_seamWidth, seamMatrix,
								min, max);
							TCVector::multMatrix(newMatrix, seamMatrix,
								tempMatrix);
							memcpy(newMatrix, tempMatrix, sizeof(newMatrix));
							otherInPart = true;
						}
						doExport(pOtherModel, pChildNode, newMatrix,
							otherColorNumber, otherInPart, bfc, otherInvert);
					}
				}
				break;
			default:
				// Get rid of warning
				break;
			}
		}
	}
}

int LD3dsExporter::doExport(LDLModel *pTopModel)
{
	int retVal = 1;
	TCFloat matrix[16];

	TCVector::initIdentityMatrix(matrix);
	matrix[5] = 0.0;
	matrix[6] = -1.0;
	matrix[9] = 1.0;
	matrix[10] = 0.0;
	m_topModel = pTopModel;
    m_file = lib3ds_file_new();
	m_names.clear();
	doExport(pTopModel, NULL, matrix, 7, false, true, false);
	//if (m_includeCamera)
	//{
	//	Lib3dsCamera *pCamera = lib3ds_camera_new("Default");
	//	Lib3dsCameraNode *pCameraNode;
	//	TCVector cameraLoc(m_camera.getPosition().transformPoint(matrix));

	//	pCamera->position[0] = cameraLoc[0];
	//	pCamera->position[1] = cameraLoc[1];
	//	pCamera->position[2] = cameraLoc[2];
	//	pCamera->fov = m_fov;
	//	pCameraNode = lib3ds_node_new_camera(pCamera);
	//	lib3ds_file_append_node(m_file, (Lib3dsNode *)pCameraNode, NULL);
	//}
	if (!lib3ds_file_save(m_file, m_filename.c_str()))
	{
		retVal = 0;
	}
	lib3ds_file_free(m_file);
	return retVal;
}
