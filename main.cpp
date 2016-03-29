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


std::vector<Point2f> touch_image;
std::vector<std::vector<cv::Mat> > crp;

void on_mouse( int e, int x, int y, int d, void *ptr )
{
    if (e == EVENT_LBUTTONDOWN )
    {
        if(touch_image.size() < 1 )
        {
 
	    touch_image.push_back(Point2f(float(x),float(y)));
            cout << x << " " << y << endl;
        }
        else
        {
	  touch_image.push_back(Point2f(float(x),float(y)));
	  cout << x << " " << y << endl;
            cout << " Calculating distance " <<endl;
            // Deactivate callback
            cv::setMouseCallback("Display window", NULL, NULL);
	    Mat S1= crp[touch_image[0].y][touch_image[0].x];
	    Mat S2= crp[touch_image[1].y][touch_image[1].x];
	    touch_image.clear();
	    
	    Mat Chol1, Chol2;
	    Cholesky(S1, Chol1);
	    Cholesky(S2, Chol2);

	    
	    Mat Sign1 = matDescriptorToVector(Chol1);
	    Mat Sign2 = matDescriptorToVector(Chol2);

	    cout << Sign1 << endl;
	    cout << endl << Sign2 << endl;
	    //cout << norm(Sign1, Sign2) << endl;
	    

	    cout << distanceColumnVector(Sign1, Sign2) << endl;
        } 
    }
}


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
    int rows = image.rows;
    int cols = image.cols;

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    Mat imageFloat;
    image.convertTo(imageFloat, CV_32FC3, 1.0/255.0, 0.0);

    unsigned int r = 10; //patch size
    Descriptor imageDescriptor(imageFloat, r);
    TextureDescriptor signature1 = imageDescriptor.at(20,20);
    TextureDescriptor signature2 = imageDescriptor.at(20,21);

    std::cout << signature1.distance(signature2) << std::endl;

    
    imshow("image", image);

    setMouseCallback("image",on_mouse, NULL );
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

    return 0;
}
