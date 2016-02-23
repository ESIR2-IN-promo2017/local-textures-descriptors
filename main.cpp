#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <iostream>
#include <iomanip>
#include <limits>
#include <vector>
#include "textureDescriptor.h"

using namespace cv;
using namespace std;


void test_mat3x3();

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

    Mat imageFloat;
    image.convertTo(imageFloat, CV_32FC3, 1.0/255.0, 0.0);

    Mat imageLAB;
    cvtColor(imageFloat, imageLAB, CV_BGR2Lab);

    // Array of 8 matrix
	std::array<cv::Mat, 8> vecteur(Z(imageLAB));

    // Appel Crp
    unsigned int r = 1; //3
    Mat wr = Wr((int)r);

    for(unsigned int i=0;i<r;i++) {
        for(unsigned int j=0;j<r;j++) {
            wr.at<float>(i,j) = 1;
        }
    }
    float b = beta(wr);
    std::vector<std::vector<cv::Mat> > crp = Crp(vecteur, wr, b, r);


    std::cout << std::fixed << std::setprecision(2);
    // Afficher 1er descripteur
    int xTmp = 80;
    int yTmp = 80;
    cout << "Descripteur ("<<xTmp << "," << yTmp << ")" << endl;
    for(unsigned int i=0;i<8; i++) {
        for(unsigned int j=0; j<8; j++){
            cout << " " << crp[xTmp][yTmp].at<float>(i,j) << "\t";
        }
        cout << endl;
    }

    xTmp = 80;
    yTmp = 83;
    cout << "Descripteur ("<<xTmp << "," << yTmp << ")" << endl;
    for(unsigned int i=0;i<8; i++) {
        for(unsigned int j=0; j<8; j++){
            cout << " " << crp[xTmp][yTmp].at<float>(i,j) << "\t";
        }
        cout << endl;
    }

/*
    Mat test, test2;
    //crp[80][80].at<float>(2,2) /= (float)std::numeric_limits<float>::max();
    applyColorMap(crp[80][80], test, COLORMAP_COOL);
    namedWindow("tr", WINDOW_NORMAL);
    imshow("tr", test);

    applyColorMap(crp[80][83], test2, COLORMAP_COOL);
    namedWindow("tr2", WINDOW_NORMAL);
    imshow("tr2", test2);
    waitKey(0);
    //*/



    /*
	int indexPeriod = name.find_last_of('.');
	string nameOutFiles(name.substr(0, indexPeriod));
	nameOutFiles += "_0" + name.substr(indexPeriod);

	// Write 8 matrix
	for(int i=0; i<8; i++) {
		nameOutFiles[indexPeriod+1] = (i+'0');
		cv::imwrite(nameOutFiles, vecteur[i]);

	}*/




    //test_mat3x3();



    return 0;
}



void test_mat3x3()
{
        /* TEST MATRIUCE 3x3 */
    
    std::array<cv::Mat, 8> vecteur3x3;
    for(int i=0; i<8; i++) {
        vecteur3x3[i] = Mat(3, 3, CV_32F);
    }
    vecteur3x3[0].at<float>(0,0) = 0;
    vecteur3x3[0].at<float>(0,1) = 1;
    vecteur3x3[0].at<float>(0,2) = 2;
    vecteur3x3[0].at<float>(1,0) = 1;
    vecteur3x3[0].at<float>(1,1) = 2;
    vecteur3x3[0].at<float>(1,2) = 3;
    vecteur3x3[0].at<float>(2,0) = 2;
    vecteur3x3[0].at<float>(2,1) = 3;
    vecteur3x3[0].at<float>(2,2) = 4;

    vecteur3x3[1].at<float>(0,0) = vecteur3x3[2].at<float>(0,0) = 0;
    vecteur3x3[1].at<float>(0,1) = vecteur3x3[2].at<float>(0,1) = 0;
    vecteur3x3[1].at<float>(0,2) = vecteur3x3[2].at<float>(0,2) = 0;
    vecteur3x3[1].at<float>(1,0) = vecteur3x3[2].at<float>(1,0) = 0;
    vecteur3x3[1].at<float>(1,1) = vecteur3x3[2].at<float>(1,1) = 0;
    vecteur3x3[1].at<float>(1,2) = vecteur3x3[2].at<float>(1,2) = 0;
    vecteur3x3[1].at<float>(2,0) = vecteur3x3[2].at<float>(2,0) = 0;
    vecteur3x3[1].at<float>(2,1) = vecteur3x3[2].at<float>(2,1) = 0;
    vecteur3x3[1].at<float>(2,2) = vecteur3x3[2].at<float>(2,2) = 0;

    vecteur3x3[3].at<float>(0,0) = vecteur3x3[4].at<float>(0,0) = 1;
    vecteur3x3[3].at<float>(0,1) = vecteur3x3[4].at<float>(0,1) = 1;
    vecteur3x3[3].at<float>(0,2) = vecteur3x3[4].at<float>(0,2) = 1;
    vecteur3x3[3].at<float>(1,0) = vecteur3x3[4].at<float>(1,0) = 1;
    vecteur3x3[3].at<float>(1,1) = vecteur3x3[4].at<float>(1,1) = 1;
    vecteur3x3[3].at<float>(1,2) = vecteur3x3[4].at<float>(1,2) = 1;
    vecteur3x3[3].at<float>(2,0) = vecteur3x3[4].at<float>(2,0) = 1;
    vecteur3x3[3].at<float>(2,1) = vecteur3x3[4].at<float>(2,1) = 1;
    vecteur3x3[3].at<float>(2,2) = vecteur3x3[4].at<float>(2,2) = 1;

    vecteur3x3[5].at<float>(0,0) = vecteur3x3[6].at<float>(0,0) = vecteur3x3[7].at<float>(0,0) = 0;
    vecteur3x3[5].at<float>(0,1) = vecteur3x3[6].at<float>(0,1) = vecteur3x3[7].at<float>(0,1) = 0;
    vecteur3x3[5].at<float>(0,2) = vecteur3x3[6].at<float>(0,2) = vecteur3x3[7].at<float>(0,2) = 0;
    vecteur3x3[5].at<float>(1,0) = vecteur3x3[6].at<float>(1,0) = vecteur3x3[7].at<float>(1,0) = 0;
    vecteur3x3[5].at<float>(1,1) = vecteur3x3[6].at<float>(1,1) = vecteur3x3[7].at<float>(1,1) = 0;
    vecteur3x3[5].at<float>(1,2) = vecteur3x3[6].at<float>(1,2) = vecteur3x3[7].at<float>(1,2) = 0;
    vecteur3x3[5].at<float>(2,0) = vecteur3x3[6].at<float>(2,0) = vecteur3x3[7].at<float>(2,0) = 0;
    vecteur3x3[5].at<float>(2,1) = vecteur3x3[6].at<float>(2,1) = vecteur3x3[7].at<float>(2,1) = 0;
    vecteur3x3[5].at<float>(2,2) = vecteur3x3[6].at<float>(2,2) = vecteur3x3[7].at<float>(2,2) = 0;

    std::cout << std::fixed << std::setprecision(4);

    unsigned int r = 1;
    Mat wr = Wr((int)r);

    cout << "wr : " << endl;
    for(unsigned int i=0;i<3; i++) {
        for(unsigned int j=0; j<3; j++){
            cout << " " <<  wr.at<float>(i,j) << " ";
        }
        cout << endl;
    }

    float b = beta(wr);
    std::vector<std::vector<cv::Mat> > crp = Crp(vecteur3x3, wr, b, r);


    // Afficher 1er descripteur
    int xTmp = 0;
    int yTmp = 0;
    cout << "Descripteur ("<<xTmp << "," << yTmp << ")" << endl;
    for(unsigned int i=0;i<8; i++) {
        for(unsigned int j=0; j<8; j++){
            cout << " " <<  crp[xTmp][yTmp].at<float>(i,j) << " ";
        }
        cout << endl;
    }

    Mat test;
    applyColorMap(crp[0][0], test, COLORMAP_JET);
    namedWindow("tr", WINDOW_NORMAL);
    imshow("tr", test);
    waitKey(0);
}
