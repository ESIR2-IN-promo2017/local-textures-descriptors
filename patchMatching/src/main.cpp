#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream> 
#include "PatchMatch.h"



int main(int argc,char **argv){ 
	cv::Mat source, target;
	
	source = cv::imread( "../data/sonne1.png");
	target = cv::imread( "../data/sonne2.png");


	cv::Mat patchMatch = PatchMatch::apply(source,target, 5, 5);
	imwrite( "../result/patchMATCH.png", patchMatch );

	return 0; 
}