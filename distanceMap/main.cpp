#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/contrib/contrib.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <iostream>
#include <iomanip>
#include <limits>
#include <numeric>
#include <vector>

#include "../textureDescriptor.h"

using namespace cv;
const int TAILLE = 8;

void show_distance(cv::Mat& image)
{
    Mat imageFloat, imageLab;
    cvtColor(image, imageLab, CV_BGR2Lab);
    imageLab.convertTo(imageFloat, CV_32FC3, 1.0, 0.0);


    unsigned int r = 10; //patch size

    std::array<cv::Mat, TAILLE> vecteur(Z(imageFloat));
    cv::Mat wr = Wr((int)r);
    float b = beta(wr);
    std::vector<std::vector<cv::Mat> > crp = Crp(vecteur, wr, b, r);



    Mat output(image.rows, image.cols, CV_8U);

    int iBase = 40;
    int jBase = 70;
    double seuil = 200;

    Mat Chol1(image.rows, image.cols, CV_8U);
    Mat sign1(image.rows, image.cols, CV_8U);


    Cholesky(crp[iBase][jBase], Chol1);
    sign1 = matDescriptorToVector(Chol1); // signature clic1


    for(int i=0; i<image.rows; i++) {
        for(int j=0; j<image.cols; j++) {
            Mat Chol2, sign2;
            Cholesky(crp[i][j], Chol2);
            sign2 = matDescriptorToVector(Chol2); // signature clic2
            //std::cout << j << " : " << distanceColumnVector(sign1, sign2) << std::endl;
            
            double dist = distanceColumnVector(sign1, sign2);
            if( dist < seuil) {
                output.at<uchar>(i, j) = 255;
            }
            else {
                output.at<uchar>(i, j) = 0;
            }
        }
    }

    imwrite("output.png", output);
    std::cout<< "file saved" << std::endl;


}


int main( int argc, char** argv )
{
    if( argc != 2)
    {
        //incorrect arguments : help message
        std::cout <<" Usage: " << argv[0] << " <image>" << std::endl; //argv[0] contains the name of the program
        return -1;
    }

    string name(argv[1]);

    Mat image;
    image = imread(name, CV_LOAD_IMAGE_COLOR);   // Read the file

    if(! image.data )                              // Check for invalid input
    {
        std::cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    show_distance(image);

    return 0;
}

