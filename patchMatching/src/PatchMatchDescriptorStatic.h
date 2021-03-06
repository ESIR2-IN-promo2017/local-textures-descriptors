#ifndef PATCH_MATCH_DESCRIPTOR_STATIC_H
#define PATCH_MATCH_DESCRIPTOR_STATIC_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cassert>
#include <math.h>
#include <iostream>
#include "AbstractPatchMatch.h"
//const int TAILLE = 12;




class PatchMatchDescriptorStatic : public AbstractPatchMatch{
public:
	static cv::Mat apply(cv::Mat source, cv::Mat target, int iterations, int patchSize);

private:

	/**
	 * Calcule la distance entre les 2 descripteurs des 2 pixels
	 * @param desSource : descripteur de l'image source
	 * @param desTarget : descripteur de l'image cible
	 * @param sx : coordonnée x dans l'image source
	 * @param sy : coordonnée y dans l'image source
	 * @param tx : coordonnée x dans l'image cible
	 * @param ty : coordonnée y dans l'image cible
	 */
	static double distanceDescriptor(const std::vector<std::vector<cv::Mat> >& crp1, const std::vector<std::vector<cv::Mat> >& crp2, int sx, int sy, int tx, int ty, int, int, int patchSize);

	static double distanceY(const cv::Mat& sourceY, const cv::Mat& targetY, int sx, int sy, int tx, int ty, int patchSize);
};


#endif
