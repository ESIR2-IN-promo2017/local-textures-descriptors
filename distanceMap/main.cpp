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

void callback(int e, int x, int y, int d, void* data)
{
    std::vector<std::vector<cv::Mat> >* crp = (std::vector<std::vector<cv::Mat> >*)data;
    if (e == EVENT_LBUTTONDOWN ) {
        Mat output(crp->size(), (*crp)[0].size(), CV_8U);

        int iBase = y;
        int jBase = x;
        double seuilDistance = 150;

        Mat Chol1;
        Mat sign1;


        Cholesky((*crp)[iBase][jBase], Chol1);
        sign1 = matDescriptorToVector(Chol1); // signature clic1

        for(unsigned int i=0; i<crp->size(); i++) {
            for(unsigned int j=0; j<(*crp)[0].size(); j++) {
                Mat Chol2, sign2;
                Cholesky((*crp)[i][j], Chol2);
                sign2 = matDescriptorToVector(Chol2); // signature clic2
                //std::cout << "dist : " << distanceColumnVector(sign1, sign2) << std::endl;

                double dist = distanceColumnVector(sign1, sign2);
                if( dist < seuilDistance) {
                    output.at<uchar>(i, j) = 255;
                }
                else {
                    output.at<uchar>(i, j) = 0;
                }
            }
        }
        std::cout << "(" << x << ", " << y << ")" << std::endl;
        imshow("dest", output);
        imwrite("output.png", output);
    }
}
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




    //imwrite("output.png", output);
    std::cout << crp.size() << std::endl;
    std::cout << image.rows << std::endl;
    std::cout << crp[0].size() << std::endl;
    std::cout << image.cols << std::endl;


    imshow("image", image);
    imshow("dest", image);


    setMouseCallback("image",callback, &crp );
    //  Press "Escape button" to exit
    while(1)
    {
        int key=cvWaitKey(10);
        if(key==27) break;
    }
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

