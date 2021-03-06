#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <cassert>
#include <vector>
#include "PatchMatchY.h"



double PatchMatchY::distanceY(const cv::Mat& sourceY, const cv::Mat& targetY, int sx, int sy, int tx, int ty, int patchSize) {
		double dist = 0.0;

		//int halfSize = (int)(((double)patchSize)/2 + 0.5)-1;
		int halfSize = patchSize/2;

		if ((sx+halfSize > sourceY.cols-1) || (sx-halfSize < 0) || 
			(sy+halfSize > sourceY.rows-1) || (sy-halfSize < 0) ||
			(tx+halfSize > sourceY.cols-1) || (tx-halfSize < 0) || 
			(ty+halfSize > sourceY.rows-1) || (ty-halfSize < 0))
		{
			dist = HUGE_VAL;
		}
		else
		{
			for(int i=-halfSize; i<=halfSize; i++){
				for(int j=-halfSize; j<=halfSize; j++){
					int xSource = i+sx;
					int ySource = j+sy;
					double YSource = sourceY.at<double>(ySource, xSource);

					int xTarget = i+tx;
					int yTarget = j+ty;
					double YTarget = targetY.at<double>(yTarget, xTarget);

					double diffY = YSource - YTarget;
					dist += diffY*diffY;
				}
			}
		}
		if(dist <= 1e-10)
			return 0.0;
		return dist;
	}



cv::Mat PatchMatchY::apply(cv::Mat source, cv::Mat target, int iterations, int patchSize) {
	assert(iterations > 0 && "Iterations must be a strictly positive integer\n");
	assert(patchSize >= 3 && (patchSize & 1) && "Patch size must be at least 3 and odd\n");

	// convert patch diameter to patch radius
	//patchSize = patchSize/2+0.5;


	cv::Mat out = cv::Mat::zeros(target.rows, target.cols,CV_8UC3);

	// Conversion en Lab, 64F, 3 canaux séparés
	cv::Mat source2, sourceLab;
	cv::Mat target2, targetLab;
	std::vector<cv::Mat> sourceChannels(3), targetChannels(3);
	cv::cvtColor(source, source2, CV_BGR2Lab);
	cv::cvtColor(target, target2, CV_BGR2Lab);
	source2.convertTo(sourceLab, CV_64F);
	target2.convertTo(targetLab, CV_64F);
	cv::split(sourceLab, sourceChannels);
	cv::split(targetLab, targetChannels);



	ImageCorrespondance corres(target.cols, target.rows);

	// INITIALIZATION - uniform random assignment
	for(int x=patchSize/2; x < out.cols-patchSize/2; x++) {
		for(int y=patchSize/2; y < out.rows-patchSize/2; y++) {
			int dx = rand() % (source.cols - 1);
			int dy = rand() % (source.rows - 1);

			Vec * vector = &corres.vectors[x][y];
			vector->x = dx;
			vector->y = dy;
			vector->dist = distanceY(sourceChannels[0], targetChannels[0], dx, dy, x, y, patchSize);

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

					double distLeft = distanceY(sourceChannels[0], targetChannels[0], left->x, left->y, x, y, patchSize);

					if (distLeft < outPtr->dist) {
						outPtr->x = left->x;
						outPtr->y = left->y;
						outPtr->dist = distLeft;

						attributePixels(out, x, y, source, outPtr->x, outPtr->y);

						nbChange++;
					}

					Vec * up = &corres.vectors[x][y-1];
					double distUp =  distanceY(sourceChannels[0], targetChannels[0], up->x, up->y, x, y, patchSize);

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

					double distRight = distanceY(sourceChannels[0], targetChannels[0], right->x, right->y, x, y, patchSize);

					if (distRight < outPtr->dist) {
						outPtr->x = right->x;
						outPtr->y = right->y;
						outPtr->dist = distRight;

						attributePixels(out, x, y, source, outPtr->x, outPtr->y);

						nbChange++;
					}

					Vec * down = &corres.vectors[x][y+1];
					double distDown = distanceY(sourceChannels[0], targetChannels[0], down->x, down->y, x, y, patchSize);

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
					double dist = distanceY(sourceChannels[0], targetChannels[0], random->x, random->y, x, y, patchSize);
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