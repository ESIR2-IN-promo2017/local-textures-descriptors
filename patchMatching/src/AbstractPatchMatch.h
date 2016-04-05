#ifndef ABSTRACT_PATCH_MATCH_H
#define ABSTRACT_PATCH_MATCH_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>


struct Vec{
	int x;
	int y;
	double dist;
};

class ImageCorrespondance{
private:
	int m_rows;
	int m_columns;
public:
	Vec ** vectors;

public:
	ImageCorrespondance(int rows, int columns):
		m_rows(rows), m_columns(columns)
	{
		vectors = new Vec*[m_rows];

		for(int i=0; i<m_rows; i++)
			vectors[i] = new Vec[m_columns];
	}
	~ImageCorrespondance()
	{
		for ( int i=0 ; i < m_rows ; i++)
			delete[] vectors[i];

		delete[] vectors;
	}
};

class AbstractPatchMatch{
public:
	static void attributePixels(cv::Mat out, int outx, int outy, cv::Mat source, int sx, int sy);
};

#endif