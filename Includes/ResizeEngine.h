#pragma once
#include "Filters.h"
#include "ImageFile.h"

class CWeightsTable
{
	typedef struct 
	{
		double *Weights;		
		int Left, Right;		
	} sContribution;

private:
	sContribution *m_WeightTable;
	DWORD m_WindowSize;
	DWORD m_LineLength;

public:
	CWeightsTable(CGenericFilter *pFilter, DWORD uDstSize, DWORD uSrcSize);
	~CWeightsTable();

	double getWeight(int dst_pos, int src_pos) 
	{
			return m_WeightTable[dst_pos].Weights[src_pos];
	}
	int getLeftBoundary(int dst_pos) 
	{
			return m_WeightTable[dst_pos].Left;
	}
	int getRightBoundary(int dst_pos) 
	{
			return m_WeightTable[dst_pos].Right;
	}
};

class CResizableImage : public CImageFile
{
	CGenericFilter *m_pFilter;
	RGBQUAD *m_pResImg;
	CWeightsTable *m_pWeights;

public:
	CResizableImage() { m_pFilter = NULL; }
	virtual ~CResizableImage() {}

	void SetFilter(CGenericFilter *pFilter) { m_pFilter = pFilter; }

	// Scale an image to the desired dimensions
	void Resample(unsigned dst_width, unsigned dst_height);

private:
	void ScaleRow(unsigned int dst_width, unsigned int /*dst_height*/, unsigned int row);
	void ScaleCol(unsigned int dst_width, unsigned int dst_height, unsigned int col);

	// Performs horizontal image filtering
	void HorizontalFilter(unsigned int dst_width, unsigned int dst_height);

	// Performs vertical image filtering
	void VerticalFilter(unsigned int dst_width, unsigned int dst_height);
};

