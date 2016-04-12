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

#include "textureDescriptor.h"
#include "descriptor.h"

using namespace cv;
using namespace std;

Descriptor* des;
std::vector<std::vector<cv::Mat> > crp;
std::vector<Point2f> touch_image;

void on_mouse_object( int e, int x, int y, int d, void *ptr )
{
    if (e == EVENT_LBUTTONDOWN )
    {
        cout << "(" << x << " " << y  << ")" << endl;
        if(touch_image.size() < 1 )
        {
 
            touch_image.push_back(Point2f(float(x),float(y)));
        }
        else
        {
            touch_image.push_back(Point2f(float(x),float(y)));
            cout << " Calculating distance " <<endl;
            // Deactivate callback
            cv::setMouseCallback("Display window", NULL, NULL);

            TextureDescriptor signature1 = des->at(touch_image[0].y, touch_image[0].x);
            TextureDescriptor signature2 = des->at(touch_image[1].y, touch_image[1].x);
            touch_image.clear();

            cout << "Distance : " << signature1.distance(signature2) << endl;
        } 
    }
}

void descriptor_object(cv::Mat& image)
{
    Mat imageFloat;
    image.convertTo(imageFloat, CV_32FC3, 1.0/255.0, 0.0);

    unsigned int r = 10; //patch size
    cout << "1" << endl;
    Descriptor imageDescriptor(imageFloat, r);
    des = &imageDescriptor;
    cout << "2" << endl;
    TextureDescriptor signature1 = imageDescriptor.at(20,20);
    cout << "3" << endl;
    TextureDescriptor signature2 = imageDescriptor.at(20,21);
    cout << "4" << endl;

    std::cout << "Distance : " << signature1.distance(signature2) << std::endl;

    
    imshow("image", image);

    setMouseCallback("image",on_mouse_object, NULL );
    //  Press "Escape button" to exit
    while(1)
    {
        int key=cvWaitKey(10);
        if(key==27) break;
    }

    //Mat crp1, crp2;
    //Mat S1 = matDescriptorToVector(); // signature clic1
    //Mat S2 = matDescriptorToVector(); // signature clic2
    
    //cout << "distance :" << distanceColumnVector(S1, S2) << endl; 
}


void on_mouse_static( int e, int x, int y, int d, void *ptr )
{
    if (e == EVENT_LBUTTONDOWN )
    {
        cout << "(" << x << " " << y  << ")" << endl;
        if(touch_image.size() < 1 )
        {
 
            touch_image.push_back(Point2f(float(x),float(y)));
        }
        else
        {
            touch_image.push_back(Point2f(float(x),float(y)));
            cout << " Calculating distance " <<endl;
            // Deactivate callback
            cv::setMouseCallback("Display window", NULL, NULL);

            Mat S1 = crp[touch_image[0].y][touch_image[0].x];
            Mat S2 = crp[touch_image[1].y][touch_image[1].x];

            Mat Chol1, Chol2;
            Cholesky(S1, Chol1);
            Cholesky(S2, Chol2);

            Mat sign1, sign2;
            sign1 = matDescriptorToVector(Chol1); // signature clic1
            sign2 = matDescriptorToVector(Chol2); // signature clic2


            cout << "distance :" << distanceColumnVector(sign1, sign2) << endl;

            touch_image.clear();
        }
    }
}

void descriptor_static(cv::Mat& image)
{
    Mat imageFloat, imageLab;
    image.convertTo(imageFloat, CV_32FC3, 1.0/255.0, 0.0);
    cvtColor(imageFloat, imageLab, CV_BGR2Lab);

    unsigned int r = 10; //patch size

    std::array<cv::Mat, 8> vecteur(Z(imageLab));
    cv::Mat wr = Wr((int)r);

    /*for(unsigned int i=0; i<r; i++) {
        for(unsigned int j=0; j<r; j++) {
            wr.at<float>(i,j) = 1;
        }
    }*/

    float b = beta(wr);
    crp = Crp(vecteur, wr, b, r);

    std::cout << "Crp rows : " << crp.size() << std::endl;
    std::cout << "Crp cols : " << crp[0].size() << std::endl;



    imshow("image", image);

    setMouseCallback("image",on_mouse_static, NULL );
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


    //descriptor_object(image);
    descriptor_static(image);

    return 0;
}
