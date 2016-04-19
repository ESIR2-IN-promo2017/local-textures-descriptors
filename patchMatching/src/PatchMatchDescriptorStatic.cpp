#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cassert>
#include <vector>
#include "PatchMatchDescriptorStatic.h"
#include "../../textureDescriptor.h"



double PatchMatchDescriptorStatic::distanceDescriptor(const std::vector<std::vector<cv::Mat> >& crp1, const std::vector<std::vector<cv::Mat> >& crp2, int sx, int sy, int tx, int ty, int cols, int rows) {
	double dist = 0.0;

	int halfSize = 0;

	if ((sx+halfSize > cols-1) || (sx-halfSize < 0) || 
		(sy+halfSize > rows-1) || (sy-halfSize < 0) ||
		(tx+halfSize > cols-1) || (tx-halfSize < 0) || 
		(ty+halfSize > rows-1) || (ty-halfSize < 0))
	{
		return HUGE_VAL;
	}

	//std::cout << "(" << sx << "," << sy << ") (" << tx << "," << ty << ")" << std::endl;
	//std::cout << crp1 << std::endl;
	cv::Mat mat1 = crp1[sy][sx];
    cv::Mat mat2 = crp2[ty][tx];

    cv::Mat chol1, chol2;
    Cholesky(mat1, chol1);
    Cholesky(mat2, chol2);

    cv::Mat sign1 = matDescriptorToVector(chol1);
	cv::Mat sign2 = matDescriptorToVector(chol2);

    dist = distanceColumnVector(sign1, sign2);
    //std::cout << "dist : " << dist << "\t" << "(" << sx << "," << sy << ") (" << tx << "," << ty << ")" << std::endl;

	if(dist <= 1e-10)
		dist = 0.0;

	return dist;
}

cv::Mat PatchMatchDescriptorStatic::apply(cv::Mat source, cv::Mat target, int iterations, int patchSize) {
	assert(iterations > 0 && "Iterations must be a strictly positive integer\n");
	assert(patchSize >= 3 && (patchSize & 1) && "Patch size must be at least 3 and odd\n");

	// convert patch diameter to patch radius
	//patchSize = patchSize/2+0.5;

	// Creation des 2 descripteurs
	// Les calculs se font a ce moment
	/*std::cout << "patchSize : " << patchSize << std::endl;
	std::cout << "1" << std::endl;
	std::cout << "1.5" << std::endl;
	std::cout << "2" << std::endl;

	Descriptor desSource(source, patchSize/2);
	Descriptor desTarget(target, patchSize/2);
	*/
	cv::Mat imageFloat1, imageLAB1;
	cv::Mat imageFloat2, imageLAB2;
    source.convertTo(imageFloat1, CV_32FC3, 1.0/255.0, 0.0);
    cvtColor(imageFloat1, imageLAB1, CV_BGR2Lab);
    target.convertTo(imageFloat2, CV_32FC3, 1.0/255.0, 0.0);
    cvtColor(imageFloat2, imageLAB2, CV_BGR2Lab);
    // Array of 8 matrix
	std::array<cv::Mat, 8> vecteur1(Z(imageLAB1));
	std::array<cv::Mat, 8> vecteur2(Z(imageLAB2));



    cv::Mat wr = Wr(patchSize/2);

    for(unsigned int i=0;i<patchSize/2;i++) {
        for(unsigned int j=0;j<patchSize/2;j++) {
            wr.at<float>(i,j) = 1;
        }
    }
    std::vector<std::vector<cv::Mat> > crp1;
    std::vector<std::vector<cv::Mat> > crp2;
    crp1 = Crp(vecteur1, wr, beta(wr), patchSize/2);
    crp2 = Crp(vecteur2, wr, beta(wr), patchSize/2);
	
    std::cout << "(" << crp1[0].size() << "," << crp1.size() << ")" << std::endl;
    std::cout << "(" << crp2[0].size() << "," << crp2.size() << ")" << std::endl;



	cv::Mat out = cv::Mat::zeros(target.rows, target.cols,CV_8UC3);



	ImageCorrespondance corres(target.cols, target.rows);

	// INITIALIZATION - uniform random assignment
	for(int x=patchSize/2; x < out.cols-patchSize/2; x++) {
		for(int y=patchSize/2; y < out.rows-patchSize/2; y++) {
			int dx = rand() % (source.cols - 1);
			int dy = rand() % (source.rows - 1);

			Vec * vector = &corres.vectors[x][y];
			vector->x = dx;
			vector->y = dy;

			vector->dist = distanceDescriptor(crp1, crp2, dx, dy, x, y, source.cols, source.rows);

			if (vector->dist == HUGE_VAL) {vector->x = 0; vector->y = 0; dx=dy=0;}
			attributePixels(out, x, y, source, dx, dy);
		}
	}

	bool forwardSearch = true;

	Vec * outPtr;
	for (int i=0; i < iterations; i++) {
		std::cout << "Iteration n " << i+1 << " / " << iterations << std::endl;

		// PROPAGATION
		int nbChange = 0;
		if(forwardSearch) {
			// Forward propagation - compare left, center and up
			for(int x = patchSize/2; x < out.cols-patchSize/2; x++) {
				for(int y = patchSize/2; y < out.rows-patchSize/2; y++) {
					outPtr = &corres.vectors[x][y];

					Vec * left = &corres.vectors[x-1][y];

					//std::cout << "left->x : " << left->x << std::endl;
					double distLeft = distanceDescriptor(crp1, crp2, left->x, left->y, x, y, source.cols, source.rows);

					if (distLeft < outPtr->dist) {
						outPtr->x = left->x;
						outPtr->y = left->y;
						outPtr->dist = distLeft;

						attributePixels(out, x, y, source, outPtr->x, outPtr->y);

						nbChange++;
					}

					Vec * up = &corres.vectors[x][y-1];
					//std::cout << "up->x : " << up->x << std::endl;
					double distUp = distanceDescriptor(crp1, crp2, up->x, up->y, x, y, source.cols, source.rows);

					if (distUp < outPtr->dist) {
						outPtr->x = up->x;
						outPtr->y = up->y;
						outPtr->dist = distUp;

						attributePixels(out, x, y, source, outPtr->x, outPtr->y);

						nbChange++;
					}
				}

				imshow("patch match", out);
				cv::waitKey(1);
				
			}


			std::cout<<nbChange<<std::endl;
		}
		else {
			// Backward propagation - compare right, center and down
			for(int x = out.cols-patchSize/2; x >= patchSize/2; x--) {
				for(int y = out.rows-patchSize/2; y >= patchSize/2; y--) {	
					outPtr = &corres.vectors[x][y];

					Vec * right = &corres.vectors[x+1][y];

					double distRight = distanceDescriptor(crp1, crp2, right->x, right->y, x, y, source.cols, source.rows);

					if (distRight < outPtr->dist) {
						outPtr->x = right->x;
						outPtr->y = right->y;
						outPtr->dist = distRight;

						attributePixels(out, x, y, source, outPtr->x, outPtr->y);

						nbChange++;
					}

					Vec * down = &corres.vectors[x][y+1];
					double distDown = distanceDescriptor(crp1, crp2, down->x, down->y, x, y, source.cols, source.rows);

					if (distDown < outPtr->dist) {
						outPtr->x = down->x;
						outPtr->y = down->y;
						outPtr->dist = distDown;

						attributePixels(out, x, y, source, outPtr->x, outPtr->y);

						nbChange++;
					}
				}

				imshow("patch match", out);
				cv::waitKey(1);
			}

			std::cout<<nbChange<<std::endl;
		}

		forwardSearch = !forwardSearch;

		for(int x = patchSize/2; x < target.cols-patchSize/2; x++) {
			for(int y = patchSize/2; y < target.rows-patchSize/2; y++) {

				int radius = source.rows > source.cols ? source.rows : source.cols;

				// search an exponentially smaller window each iteration
				while (radius > 1) {
					// Search around current offset vector (distance-weighted)

					// clamp the search window to the image
					outPtr = &corres.vectors[x][y];
					int minX = (int)outPtr->x - radius;
					int maxX = ((int)(outPtr->x + radius)) + 1;
					int minY = (int)outPtr->y - radius;
					int maxY = ((int)(outPtr->y + radius)) + 1;
					if (minX < 0) minX = patchSize/2;
					if (maxX > source.cols) maxX = source.cols-patchSize/2;
					if (minY < 0) minY = patchSize/2;
					if (maxY > source.rows) maxY = source.rows-patchSize/2;

					int randX = rand() % (maxX - minX) + minX;
					int randY = rand() % (maxY - minY) + minY;

					Vec * random = &corres.vectors[randX][randY];
					double dist = distanceDescriptor(crp1, crp2, random->x, random->y, x, y, source.cols, source.rows);
					if (dist < outPtr->dist) {
						outPtr->x = random->x;
						outPtr->y = random->y;
						outPtr->dist = dist;

						attributePixels(out, x, y, source, outPtr->x, outPtr->y);
					}

					radius >>= 1;
				}
			}

			//imshow("patch match", out);
			//cv::waitKey(1);
		}
	}
	cv::waitKey();
	return out;
}