#ifndef _PATCH_MATCH_H
#define _PATCH_MATCH_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <cassert>
#include "ImageCorrespondance.h"

namespace PatchMatch{
	double getYFromRGB(cv::Mat image, int x, int y){
		int R = image.at<cv::Vec3b>(y, x)[2];
		int G = image.at<cv::Vec3b>(y, x)[1];
		int B = image.at<cv::Vec3b>(y, x)[0];

		return MIN(255,0.299*R+0.587*G+0.11*B);
	}
	//=======================================================================================================
	cv::Mat imageYFromRGB(cv::Mat image){
		
		cv::Mat out = cv::Mat::zeros(image.rows, image.cols,CV_64F);

		for(int x=0; x < out.cols; x++) {
			for(int y=0; y < out.rows; y++) {
				out.at<double>(y,x) = getYFromRGB(image, x, y);
			}
		}

		return out;
	}
	//=======================================================================================================
	void attributePixels(cv::Mat out, int outx, int outy, cv::Mat source, int sx, int sy){
		out.at<cv::Vec3b>(outy, outx) = source.at<cv::Vec3b>(sy,sx);
	}
	//=======================================================================================================
	double distance(cv::Mat source,cv::Mat target, int sx, int sy, int tx, int ty, int patchSize, double prevDist) {
		double dist = 0.0;

		//int halfSize = (int)(((double)patchSize)/2 + 0.5)-1;
		int halfSize = patchSize/2;

		if ((sx+halfSize > source.cols-1) || (sx-halfSize < 0) || 
			(sy+halfSize > source.rows-1) || (sy-halfSize < 0) ||
			(tx+halfSize > source.cols-1) || (tx-halfSize < 0) || 
			(ty+halfSize > source.rows-1) || (ty-halfSize < 0))
		{
			dist = HUGE_VAL;
		}
		else
		{

		 for(int i=-halfSize; i<=halfSize; i++){
			for(int j=-halfSize; j<=halfSize; j++){
				int xSource = i+sx;
				int ySource = j+sy;
				double Ysource = source.at<double>(ySource, xSource);
				
				int xTarget = i+tx;
				int yTarget = j+ty;
				double grayTarget = target.at<double>(yTarget, xTarget);

				double diff = Ysource - grayTarget;
				dist += diff*diff;
			}
		}
		}
		if(dist <= 1e-10)
			return 0.0;
		return dist;
	}
	//=======================================================================================================
	cv::Mat apply(cv::Mat source, cv::Mat target, int iterations, int patchSize) {
		assert(iterations > 0 && "Iterations must be a strictly positive integer\n");
		assert(patchSize >= 3 && (patchSize & 1) && "Patch size must be at least 3 and odd\n");

		// convert patch diameter to patch radius
		//patchSize = patchSize/2+0.5;

		cv::Mat out = cv::Mat::zeros(target.rows, target.cols,CV_8UC3);
    
		cv::Mat sourceY = imageYFromRGB(source);
		cv::Mat targetY = imageYFromRGB(target);

		ImageCorrespondance corres(target.cols, target.rows);

		// INITIALIZATION - uniform random assignment
		for(int x=patchSize/2; x < out.cols-patchSize/2; x++) {
			for(int y=patchSize/2; y < out.rows-patchSize/2; y++) {
				int dx = rand() % (source.cols - 1);
				int dy = rand() % (source.rows - 1);

				Vec * vector = &corres.vectors[x][y];
				vector->x = dx;
				vector->y = dy;
				vector->dist = distance(sourceY, targetY, dx, dy, x, y, patchSize, HUGE_VAL);
				
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
						
						double distLeft = distance(sourceY, targetY, left->x, left->y, x, y, patchSize, outPtr->dist);

						if (distLeft < outPtr->dist) {
							outPtr->x = left->x;
							outPtr->y = left->y;
							outPtr->dist = distLeft;

							attributePixels(out, x, y, source, outPtr->x, outPtr->y);

							nbChange++;
						}
				
						Vec * up = &corres.vectors[x][y-1];
						double distUp = distance(sourceY, targetY, up->x, up->y, x, y, patchSize, outPtr->dist);

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
						
						double distRight = distance(sourceY, targetY, right->x, right->y, x, y, patchSize, outPtr->dist);

						if (distRight < outPtr->dist) {
							outPtr->x = right->x;
							outPtr->y = right->y;
							outPtr->dist = distRight;

							attributePixels(out, x, y, source, outPtr->x, outPtr->y);

							nbChange++;
						}

						Vec * down = &corres.vectors[x][y+1];
						double distDown = distance(sourceY, targetY, down->x, down->y, x, y, patchSize, outPtr->dist);

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
						double dist = distance(sourceY, targetY, random->x, random->y, x, y, patchSize, outPtr->dist);
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

	
}


#endif