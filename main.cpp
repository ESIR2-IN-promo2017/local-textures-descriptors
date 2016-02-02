#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "textureDescriptor.h"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
    if( argc != 2)
    {
        //incorrect arguments : help message
        cout <<" Usage: " << argv[0] << " <image>" << endl; //argv[0] contains the name of the program
        return -1;
    }

    string name(argv[1]);

    Mat image;
    image = imread(name, CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }


    Mat imageLAB;
    cvtColor(image, imageLAB, CV_BGR2Lab);

    // Array of 8 matrix
	std::array<cv::Mat, 8> vecteur(Z(imageLAB));



	int indexPeriod = name.find_last_of('.');
	string nameOutFiles(name.substr(0, indexPeriod));
	nameOutFiles += "_0" + name.substr(indexPeriod);

	// Write 8 matrix
	for(int i=0; i<8; i++) {
		nameOutFiles[indexPeriod+1] = (i+'0');
		cv::imwrite(nameOutFiles, vecteur[i]);

	}

    return 0;
}
