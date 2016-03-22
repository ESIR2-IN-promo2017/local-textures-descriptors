#ifndef _PATCH_MATCH_H
#define _PATCH_MATCH_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <cassert>
#include "ImageCorrespondance.h"

namespace PatchMatch{
	//=======================================================================================================
	void attributePixels(cv::Mat out, int outx, int outy, cv::Mat source, int sx, int sy);
	//=======================================================================================================
	double distanceLab(std::vector<cv::Mat> source, std::vector<cv::Mat> target, int sx, int sy, int tx, int ty, int patchSize, double prevDist);
	//=======================================================================================================
	cv::Mat apply(cv::Mat source, cv::Mat target, int iterations, int patchSize);
}


#endif