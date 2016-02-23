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

void test_cholesky();
void test_mat3x3();

int main( int argc, char** argv )
{
    /*if( argc != 2)
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



    // Appel Crp
    unsigned int r = 3; //3
    Mat wr = Wr((int)r);

    for(int i=0;i<r;i++) {
        for(int j=0;j<r;j++) {
            wr.at<double>(i,j) = 1;
        }
    }
    double b = beta(wr);
    std::vector<std::vector<cv::Mat> > crp = Crp(vecteur, wr, b, r);


    std::cout << std::fixed << std::setprecision(2);
    // Afficher 1er descripteur
    int xTmp = 80;
    int yTmp = 80;
    cout << "Descripteur ("<<xTmp << "," << yTmp << ")" << endl;
    for(unsigned int i=0;i<8; i++) {
        for(unsigned int j=0; j<8; j++){
            cout << " " << crp[xTmp][yTmp].at<double>(i,j) << "\t";
        }
        cout << endl;
    }

    xTmp = 80;
    yTmp = 81;
    cout << "Descripteur ("<<xTmp << "," << yTmp << ")" << endl;
    for(unsigned int i=0;i<8; i++) {
        for(unsigned int j=0; j<8; j++){
            cout << " " << crp[xTmp][yTmp].at<double>(i,j) << "\t";
        }
        cout << endl;
    }


    Mat test1_1, test1_2;
    Mat test2_1, test2_2;

    crp[80][80].convertTo(test1_1, CV_8U);
    crp[80][81].convertTo(test1_2, CV_8U);
    
    applyColorMap(test1_1, test2_1, COLORMAP_JET);
    namedWindow("tr", WINDOW_NORMAL);
    imshow("tr", test2_1);

    applyColorMap(test1_2, test2_2, COLORMAP_JET);
    namedWindow("tr2", WINDOW_NORMAL);
    imshow("tr2", test2_2);
    waitKey(0);
    */


    /*
	int indexPeriod = name.find_last_of('.');
	string nameOutFiles(name.substr(0, indexPeriod));
	nameOutFiles += "_0" + name.substr(indexPeriod);

	// Write 8 matrix
	for(int i=0; i<8; i++) {
		nameOutFiles[indexPeriod+1] = (i+'0');
		cv::imwrite(nameOutFiles, vecteur[i]);

	}*/



    test_cholesky();

    //test_mat3x3();


    return 0;
}

int func(int n) {
    switch(n){
        case 0: return 1;
        case 1: return 5;
        case 2: return 14;
        case 3: return 15;
        default: return 0;
    }
}

void test_cholesky()
{
    int size = 4;
    Mat mat(4,4, CV_32F);
    Mat matCho(4,4, CV_32F);

    for(int k=0; k<4; k++) {
        for(int i=k; i<4; i++) {
            for(int j=k; j<4; j++) {
                mat.at<float>(i,j) = func(k);
            }
        }
    }



    Cholesky(mat, matCho);

    cout << "Mat : " << endl;
    for(int i=0;i<size; i++) {
        for(int j=0; j<size; j++){
            cout << " " << mat.at<float>(i,j) << "\t";
        }
        cout << endl;
    }


    cout << "MatCholesky : " << endl;
    for(int i=0;i<size; i++) {
        for(int j=0; j<size; j++){
            cout << " " << matCho.at<float>(i,j) << "\t";
        }
        cout << endl;
    }

}


void test_mat3x3()
{
        /* TEST MATRIUCE 3x3 */
    
    std::array<cv::Mat, 8> vecteur3x3;
    for(int i=0; i<8; i++) {
        vecteur3x3[i] = Mat(3, 3, CV_64F);
    }
    vecteur3x3[0].at<double>(0,0) = 0;
    vecteur3x3[0].at<double>(0,1) = 1;
    vecteur3x3[0].at<double>(0,2) = 2;
    vecteur3x3[0].at<double>(1,0) = 1;
    vecteur3x3[0].at<double>(1,1) = 2;
    vecteur3x3[0].at<double>(1,2) = 3;
    vecteur3x3[0].at<double>(2,0) = 2;
    vecteur3x3[0].at<double>(2,1) = 3;
    vecteur3x3[0].at<double>(2,2) = 4;

    vecteur3x3[1].at<double>(0,0) = vecteur3x3[2].at<double>(0,0) = 0;
    vecteur3x3[1].at<double>(0,1) = vecteur3x3[2].at<double>(0,1) = 0;
    vecteur3x3[1].at<double>(0,2) = vecteur3x3[2].at<double>(0,2) = 0;
    vecteur3x3[1].at<double>(1,0) = vecteur3x3[2].at<double>(1,0) = 0;
    vecteur3x3[1].at<double>(1,1) = vecteur3x3[2].at<double>(1,1) = 0;
    vecteur3x3[1].at<double>(1,2) = vecteur3x3[2].at<double>(1,2) = 0;
    vecteur3x3[1].at<double>(2,0) = vecteur3x3[2].at<double>(2,0) = 0;
    vecteur3x3[1].at<double>(2,1) = vecteur3x3[2].at<double>(2,1) = 0;
    vecteur3x3[1].at<double>(2,2) = vecteur3x3[2].at<double>(2,2) = 0;

    vecteur3x3[3].at<double>(0,0) = vecteur3x3[4].at<double>(0,0) = 1;
    vecteur3x3[3].at<double>(0,1) = vecteur3x3[4].at<double>(0,1) = 1;
    vecteur3x3[3].at<double>(0,2) = vecteur3x3[4].at<double>(0,2) = 1;
    vecteur3x3[3].at<double>(1,0) = vecteur3x3[4].at<double>(1,0) = 1;
    vecteur3x3[3].at<double>(1,1) = vecteur3x3[4].at<double>(1,1) = 1;
    vecteur3x3[3].at<double>(1,2) = vecteur3x3[4].at<double>(1,2) = 1;
    vecteur3x3[3].at<double>(2,0) = vecteur3x3[4].at<double>(2,0) = 1;
    vecteur3x3[3].at<double>(2,1) = vecteur3x3[4].at<double>(2,1) = 1;
    vecteur3x3[3].at<double>(2,2) = vecteur3x3[4].at<double>(2,2) = 1;

    vecteur3x3[5].at<double>(0,0) = vecteur3x3[6].at<double>(0,0) = vecteur3x3[7].at<double>(0,0) = 0;
    vecteur3x3[5].at<double>(0,1) = vecteur3x3[6].at<double>(0,1) = vecteur3x3[7].at<double>(0,1) = 0;
    vecteur3x3[5].at<double>(0,2) = vecteur3x3[6].at<double>(0,2) = vecteur3x3[7].at<double>(0,2) = 0;
    vecteur3x3[5].at<double>(1,0) = vecteur3x3[6].at<double>(1,0) = vecteur3x3[7].at<double>(1,0) = 0;
    vecteur3x3[5].at<double>(1,1) = vecteur3x3[6].at<double>(1,1) = vecteur3x3[7].at<double>(1,1) = 0;
    vecteur3x3[5].at<double>(1,2) = vecteur3x3[6].at<double>(1,2) = vecteur3x3[7].at<double>(1,2) = 0;
    vecteur3x3[5].at<double>(2,0) = vecteur3x3[6].at<double>(2,0) = vecteur3x3[7].at<double>(2,0) = 0;
    vecteur3x3[5].at<double>(2,1) = vecteur3x3[6].at<double>(2,1) = vecteur3x3[7].at<double>(2,1) = 0;
    vecteur3x3[5].at<double>(2,2) = vecteur3x3[6].at<double>(2,2) = vecteur3x3[7].at<double>(2,2) = 0;

    std::cout << std::fixed << std::setprecision(4);

    unsigned int r = 1;
    Mat wr = Wr((int)r);

    cout << "wr : " << endl;
    for(unsigned int i=0;i<3; i++) {
        for(unsigned int j=0; j<3; j++){
            cout << " " <<  wr.at<double>(i,j) << " ";
        }
        cout << endl;
    }

    double b = beta(wr);
    std::vector<std::vector<cv::Mat> > crp = Crp(vecteur3x3, wr, b, r);


    // Afficher 1er descripteur
    int xTmp = 0;
    int yTmp = 0;
    cout << "Descripteur ("<<xTmp << "," << yTmp << ")" << endl;
    for(unsigned int i=0;i<8; i++) {
        for(unsigned int j=0; j<8; j++){
            cout << " " <<  crp[xTmp][yTmp].at<double>(i,j) << " ";
        }
        cout << endl;
    }

    Mat test;
    applyColorMap(crp[0][0], test, COLORMAP_JET);
    namedWindow("tr", WINDOW_NORMAL);
    imshow("tr", test);
    waitKey(0);
}