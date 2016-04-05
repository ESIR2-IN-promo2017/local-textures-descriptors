#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream> 
#include "PatchMatchLab.h"
#include "PatchMatchDescriptor.h"



int main(int argc,char **argv){ 
	cv::Mat source, target;
	
	source = cv::imread( "../data/sonne1.png");
	target = cv::imread( "../data/sonne2.png");

	// Distance Y
	PatchMatchLab pY;
	cv::Mat patchMatchY = pY.apply(source,target, 5, 5);
	imwrite( "../result/patchMatch_Y.png", patchMatchY );

	// Distance Lab
	PatchMatchLab pLab;
	cv::Mat patchMatchLab = pLab.apply(source,target, 5, 5);
	imwrite( "../result/patchMatch_Lab.png", patchMatchLab );



	// Distance entre descripteurs
	PatchMatchDescriptor pDescripteurs;
	cv::Mat patchMatchDescripteurs = pDescripteurs.apply(source,target, 5, 5);
	imwrite( "../result/patchMatch_descripteurs.png", patchMatchDescripteurs);

	return 0; 
}