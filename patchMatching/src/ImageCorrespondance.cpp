#include "ImageCorrespondance.h"
#include <math.h>
#include <iostream>


ImageCorrespondance::ImageCorrespondance(int rows, int columns):
	m_rows(rows), m_columns(columns)
{
	vectors = new Vec*[m_rows];

	for(int i=0; i<m_rows; i++)
		vectors[i] = new Vec[m_columns];
}
ImageCorrespondance::~ImageCorrespondance()
{
	for ( int i=0 ; i < m_rows ; i++) 
		delete vectors[i]; 

	delete vectors; 
}
