#ifndef _IMAGE_CORRESPONDANCE_H
#define _IMAGE_CORRESPONDANCE_H

#include <math.h>
#include <iostream>

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
	ImageCorrespondance(int rows, int columns);
	~ImageCorrespondance();
};

#endif
