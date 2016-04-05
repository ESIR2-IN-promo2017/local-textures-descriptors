#ifndef PATCH_MATCH_LAB_H
#define PATCH_MATCH_LAB_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cassert>
#include <math.h>
#include <iostream>

#include "AbstractPatchMatch.h"
#include "../../descriptor.h"




class PatchMatchLab{
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
	
};


#endif