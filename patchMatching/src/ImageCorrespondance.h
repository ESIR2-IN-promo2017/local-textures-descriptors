#ifndef _IMAGE_CORRESPONDANCE_H
#define _IMAGE_CORRESPONDANCE_H

#include <math.h>
#include <iostream>
#include "PatchMatch.h"

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
	ImageCorrespondance(int rows, int columns):m_rows(rows), m_columns(columns){
		vectors = new Vec*[m_rows];

		for(int i=0; i<m_rows; i++)
			vectors[i] = new Vec[m_columns];
	}
	//=======================================================================================================
	~ImageCorrespondance(){
		for ( int i=0 ; i < m_rows ; i++) 
			delete vectors[i]; 

		delete vectors; 
	}
};

#endif
