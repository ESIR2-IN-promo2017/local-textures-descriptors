#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cassert>
#include <vector>
#include "AbstractPatchMatch.h"


void AbstractPatchMatch::attributePixels(cv::Mat out, int outx, int outy, cv::Mat source, int sx, int sy){
	out.at<cv::Vec3b>(outy, outx) = source.at<cv::Vec3b>(sy,sx);
}

