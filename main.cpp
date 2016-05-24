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
    cout << "Creating Descriptor..." << flush;
    Descriptor imageDescriptor(imageFloat, r);
    des = &imageDescriptor;
    cout << "done" << endl << "signature1...";
    TextureDescriptor signature1 = imageDescriptor.at(50,50);
    cout << "done" << endl << "signature2...";
    TextureDescriptor signature2 = imageDescriptor.at(50,51);
    cout << "done" << endl << "distance..." << endl;
    cout << "Distance: " << signature1.distance(signature2) << endl;

#ifndef DEBUG 
    imshow("image", image);

    setMouseCallback("image",on_mouse_object, NULL );
    //  Press "Escape button" to exit
    while(1)
    {
        int key=cvWaitKey(10);
        if(key==27) break;
    }
#endif

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

    std::array<cv::Mat, TAILLE> vecteur(Z(imageLab));
    cv::Mat wr = Wr((int)r);


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

void qudrillage_image(Mat& image);

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

    int input=0;
    while(input<5)
    {
        std::cout << "\n-------------------------------------" << std::endl;
        std::cout << "1 - Descriptor object" << std::endl;
        std::cout << "2 - Descriptor static" << std::endl;
        std::cout << "3 - Quadrillage" << std::endl;
        std::cout << "5 - Quitter"<< std::endl;
        std::cout << "-------------------------------------" << std::endl;

        std::cin >> input;

        switch(input)
        {
            case 1:
                descriptor_object(image);
                break;
            case 2:
                descriptor_static(image);
                break;
            case 3:
                qudrillage_image(image);
                break;
            default :
                break;
        }
    }

    return 0;
}


void qudrillage_image(Mat& image)
{
    Mat imageFloat, imageLab;
    image.convertTo(imageFloat, CV_32FC3, 1.0/255.0, 0.0);
    cvtColor(imageFloat, imageLab, CV_BGR2Lab);

    unsigned int r = 10; //patch size

    std::array<cv::Mat, TAILLE> vecteur(Z(imageLab));
    cv::Mat wr = Wr((int)r);
    float b = beta(wr);
    crp = Crp(vecteur, wr, b, r);



    unsigned int divI = 2;
    unsigned int divJ = 5;
    std::vector<std::vector<Mat> > tabSigns;

    int hCase = image.rows/divI;
    int wCase = image.cols/divJ;

    for(unsigned int i=0; i<divI; i++) {
        std::vector<Mat> line;
        for(unsigned int j=0; j<divJ; j++) {

            int pixelI = hCase*i + (hCase/2);
            int pixelJ = wCase*j + (wCase/2);

            Mat chol, sign;
            Cholesky(crp[pixelI][pixelJ], chol);
            sign = matDescriptorToVector(chol); // signature clic1
            line.push_back(sign.clone());
        }
        tabSigns.push_back(line);
    }

    std::cout << "Distances horizontales : " << std::endl;
    std::vector<std::vector<double> > tabDistancesHori;
    for(unsigned int i=0; i<divI; i++) {
        std::vector<double> line;
        for(unsigned int j=0; j<divJ-1; j++) {
            double distance = distanceColumnVector(tabSigns[i][j], tabSigns[i][j+1]);
            line.push_back(distance);
            std::cout << (int)distance << ", ";
        }
        tabDistancesHori.push_back(line);
        std::cout << std::endl;
    }


    std::cout << "Distances verticales : " << std::endl;
    std::vector<std::vector<double> > tabDistancesVert;
    for(unsigned int i=0; i<divI-1; i++) {
        std::vector<double> line;
        for(unsigned int j=0; j<divJ; j++) {
            double distance = distanceColumnVector(tabSigns[i][j], tabSigns[i+1][j]);
            line.push_back(distance);
            std::cout << (int)distance << ", ";
        }
        tabDistancesVert.push_back(line);
        std::cout << std::endl;
    }
}
