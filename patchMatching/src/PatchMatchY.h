#ifndef PATCH_MATCH_Y_H
#define PATCH_MATCH_Y_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cassert>
#include <math.h>
#include <iostream>

#include "AbstractPatchMatch.h"
#include "../../descriptor.h"




class PatchMatchY : public AbstractPatchMatch{
public:
	static cv::Mat apply(cv::Mat source, cv::Mat target, int iterations, int patchSize);

private:
	/**
	 * Calcule la distance sur la composante Y entre 2 pixels
	 * @param source : image (canal Y) de l'image source
	 * @param target : image (canal Y) de l'image cible
	 * @param sx : coordonnée x dans l'image source
	 * @param sy : coordonnée y dans l'image source
	 * @param tx : coordonnée x dans l'image cible
	 * @param ty : coordonnée y dans l'image cible
	 * @param patchSize : taille du patch (largeur)
	 */
	static double distanceY(const cv::Mat& sourceY, const cv::Mat& targetY, int sx, int sy, int tx, int ty, int patchSize);
	
};


#endif