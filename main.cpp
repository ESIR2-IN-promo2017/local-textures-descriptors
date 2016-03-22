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

    Mat imageLAB;
    cvtColor(imageFloat, imageLAB, CV_BGR2Lab);

    // Array of 8 matrix
	std::array<cv::Mat, 8> vecteur(Z(imageLAB));

    // Appel Crp
    unsigned int r = 10; 
    Mat wr = Wr((int)r);

    for(unsigned int i=0;i<r;i++) {
        for(unsigned int j=0;j<r;j++) {
            wr.at<float>(i,j) = 1;
        }
    }
    float b = beta(wr);
    crp = Crp(vecteur, wr, b, r);

    //std::cout << "crp[66][100] = " << std::endl;
    //std::cout << crp[66][100] << std::endl;

    /*
    int cptErr=0;
    Mat choleskyMatrixAll;
    for(unsigned int i=2*r; i<rows-2*r; i++){
      for(unsigned int j=2*r; j<cols-2*r; j++){
        cptErr+=Cholesky(crp[i][j], choleskyMatrixAll);
      }
    }
    std::cout<< "nombre de patchs à problème: "<< cptErr<< std::endl;
    */

    cv::Mat mat = matDescriptorToVector(crp[20][20]);
    cv::Mat mat2 = matDescriptorToVector(crp[20][21]);

    std::cout << distanceColumnVector(mat, mat2) << std::endl;

    
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

    /*
    std::cout << "cholesky[66][100] = " << std::endl;
    std::cout << choleskyMatrix1 << std::endl;

    std::cout << "crp[75][71] = " << std::endl;
    std::cout << crp[75][71] << std::endl;

    Mat choleskyMatrix2;
    Cholesky(crp[75][71], choleskyMatrix2);

    std::cout << "cholesky[75][71] = " << std::endl;
    std::cout << choleskyMatrix2 << std::endl;


    show_descriptor(choleskyMatrix1, "[66][100]");
    show_descriptor(choleskyMatrix2, "[75][71]");


    waitKey(0);
    */
    /*
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
    //*/

    /*
    xTmp = 80;
    yTmp = 83;
    cout << "Descripteur ("<<xTmp << "," << yTmp << ")" << endl;
    for(unsigned int i=0;i<8; i++) {
        for(unsigned int j=0; j<8; j++){
            cout << " " << crp[xTmp][yTmp].at<float>(i,j) << "\t";
        }
        cout << endl;
    }
    //*/


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

    unsigned int r = 10;
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
