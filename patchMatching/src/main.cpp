#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream> 
#include "PatchMatchY.h"
#include "PatchMatchLab.h"
#include "PatchMatchDescriptor.h"



int main(int argc,char **argv){ 
	cv::Mat source, target;
	
	source = cv::imread( "../data/sonne1.png");
	target = cv::imread( "../data/sonne2.png");

	// Distance Y
	cv::Mat patchMatchY = PatchMatchY::apply(source,target, 5, 5);
	imwrite( "../result/patchMatch_Y.png", patchMatchY );

	// Distance Lab
	cv::Mat patchMatchLab = PatchMatchLab::apply(source,target, 5, 5);
	imwrite( "../result/patchMatch_Lab.png", patchMatchLab );



	// Distance entre descripteurs
	cv::Mat patchMatchDescripteurs = PatchMatchDescriptor::apply(source,target, 5, 5);
	imwrite( "../result/patchMatch_descripteurs.png", patchMatchDescripteurs);

	return 0; 
}