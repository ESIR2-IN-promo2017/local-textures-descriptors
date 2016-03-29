#ifndef PATCH_MATCH_H
#define PATCH_MATCH_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cassert>
#include <math.h>
#include <iostream>

#include "../../descriptor.h"


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


class PatchMatch{
public:
	cv::Mat apply(cv::Mat source, cv::Mat target, int iterations, int patchSize);

private:
	void attributePixels(cv::Mat out, int outx, int outy, cv::Mat source, int sx, int sy);
	/**
	 * Calcule la distance Lab entre 2 pixels
	 * @param source : tableau de 3 images (L,a,b) de l'image source
	 * @param target : tableau de 3 images (L,a,b) de l'image cible
	 * @param sx : coordonnée x dans l'image source
	 * @param sy : coordonnée y dans l'image source
	 * @param tx : coordonnée x dans l'image cible
	 * @param ty : coordonnée y dans l'image cible
	 * @param patchSize : taille du patch (largeur)
	 */
	double distanceLab(const std::vector<cv::Mat>& source, const std::vector<cv::Mat>& target, int sx, int sy, int tx, int ty, int patchSize);
	/**
	 * Calcule la distance entre les 2 descripteurs des 2 pixels
	 * @param desSource : descripteur de l'image source
	 * @param desTarget : descripteur de l'image cible
	 * @param sx : coordonnée x dans l'image source
	 * @param sy : coordonnée y dans l'image source
	 * @param tx : coordonnée x dans l'image cible
	 * @param ty : coordonnée y dans l'image cible
	 */
	double distanceDescriptor(const Descriptor& desSource, const Descriptor& desTarget, int sx, int sy, int tx, int ty);
};


#endif