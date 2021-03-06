#ifndef PATCH_MATCH_DESCRIPTOR_H
#define PATCH_MATCH_DESCRIPTOR_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cassert>
#include <math.h>
#include <iostream>

#include "AbstractPatchMatch.h"
#include "../../descriptor.h"





class PatchMatchDescriptor : public AbstractPatchMatch{
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
	static double distanceDescriptor(const Descriptor& desSource, const Descriptor& desTarget, int sx, int sy, int tx, int ty);
};


#endif