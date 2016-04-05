#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cassert>
#include <vector>
#include "PatchMatchDescriptor.h"


void PatchMatchDescriptor::attributePixels(cv::Mat out, int outx, int outy, cv::Mat source, int sx, int sy){
	out.at<cv::Vec3b>(outy, outx) = source.at<cv::Vec3b>(sy,sx);
}


double PatchMatchDescriptor::distanceDescriptor(const Descriptor& desSource, const Descriptor& desTarget, int sx, int sy, int tx, int ty) {
	double dist = 0.0;
	TextureDescriptor textSource( desSource.at(sy, sx));
	TextureDescriptor textTarget( desTarget.at(ty, tx));

	dist = textSource.distance(textTarget);

	if(dist <= 1e-10)
		dist = 0.0;

	return dist;
}

cv::Mat PatchMatchDescriptor::apply(cv::Mat source, cv::Mat target, int iterations, int patchSize) {
	assert(iterations > 0 && "Iterations must be a strictly positive integer\n");
	assert(patchSize >= 3 && (patchSize & 1) && "Patch size must be at least 3 and odd\n");

	// convert patch diameter to patch radius
	//patchSize = patchSize/2+0.5;

	// Creation des 2 descripteurs
	// Les calculs se font a ce moment
	std::cout << "patchSize : " << patchSize << std::endl;
	std::cout << "1" << std::endl;
	Descriptor desSource(source, patchSize/2);
	std::cout << "1.5" << std::endl;
	Descriptor desTarget(target, patchSize/2);
	std::cout << "2" << std::endl;

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
			vector->dist = distanceDescriptor(desSource, desTarget, dx, dy, x, y);

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

					double distLeft = distanceDescriptor(desSource, desTarget, left->x, left->y, x, y);

					if (distLeft < outPtr->dist) {
						outPtr->x = left->x;
						outPtr->y = left->y;
						outPtr->dist = distLeft;

						attributePixels(out, x, y, source, outPtr->x, outPtr->y);

						nbChange++;
					}

					Vec * up = &corres.vectors[x][y-1];
					double distUp = distanceDescriptor(desSource, desTarget, up->x, up->y, x, y);

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

					double distRight = distanceDescriptor(desSource, desTarget, right->x, right->y, x, y);

					if (distRight < outPtr->dist) {
						outPtr->x = right->x;
						outPtr->y = right->y;
						outPtr->dist = distRight;

						attributePixels(out, x, y, source, outPtr->x, outPtr->y);

						nbChange++;
					}

					Vec * down = &corres.vectors[x][y+1];
					double distDown = distanceDescriptor(desSource, desTarget, down->x, down->y, x, y);

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
					double dist = distanceDescriptor(desSource, desTarget, random->x, random->y, x, y);
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