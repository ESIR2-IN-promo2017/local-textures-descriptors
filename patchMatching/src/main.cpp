#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <iostream> 
#include "PatchMatchY.h"
#include "PatchMatchLab.h"
#include "PatchMatchDescriptor.h"
#include "PatchMatchDescriptorStatic.h"



int main(int argc,char **argv){ 
	cv::Mat source, target;

	source = cv::imread( "../data/sonne1.png");
	target = cv::imread( "../data/sonne2.png");

	cv::Mat patchMatchY, patchMatchLab, patchMatchDescripteurs, patchMatchDescripteursStatic;

	int input=0;

	while(input<5)
	{
		std::cout << "\n-------------------------------------" << std::endl;
		std::cout << "1 - Distance Y" << std::endl;
		std::cout << "2 - Distance Lab" << std::endl;
		std::cout << "3 - Distance entre descripteurs" << std::endl;
		std::cout << "4 - Distance entre descripteurs static (non objet)" << std::endl;
		std::cout << "5 - Quitter"<< std::endl;
		std::cout << "-------------------------------------" << std::endl;

		std::cin >> input;

		switch(input)
		{
			case 1:
				// Distance Y
				patchMatchY = PatchMatchY::apply(source,target, 5, 5);
				imwrite( "../result/patchMatch_Y.png", patchMatchY );
				break;
			case 2:
				// Distance Lab
				patchMatchLab = PatchMatchLab::apply(source,target, 5, 5);
				imwrite( "../result/patchMatch_Lab.png", patchMatchLab );
				break;
			case 3:
				// Distance entre descripteurs
				patchMatchDescripteurs = PatchMatchDescriptor::apply(source,target, 5, 5);
				imwrite( "../result/patchMatch_descripteurs.png", patchMatchDescripteurs);
				break;
			case 4:
				// Distance entre descripteurs static (non objet)
				patchMatchDescripteursStatic = PatchMatchDescriptorStatic::apply(source,target, 5, 5);
				imwrite( "../result/patchMatch_descripteurs_static.png", patchMatchDescripteursStatic);
				break;
			default :
				break;
		}
	}
	return 0;
}
