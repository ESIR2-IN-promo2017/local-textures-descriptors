#include "textureDescriptor.h"

std::array<cv::Mat, 8> Z(cv::Mat const& img)
{
    std::array<cv::Mat, 8> z; //return array

    std::vector<cv::Mat> Lab;
    cv::split(img, Lab);

    cv::Mat L(Lab[0]), a(Lab[1]), b(Lab[2]);

    cv::Mat dLdx, dLdy, d2Ldx2, d2Ldy2, d2Ldxdy;

    cv::Scharr(L, dLdx, CV_64F, 1, 0);
    cv::Scharr(L, dLdy, CV_64F, 0, 1);
    cv::Scharr(dLdx, d2Ldx2, CV_64F, 1, 0);
    cv::Scharr(dLdy, d2Ldy2, CV_64F, 0, 1);
    // Triggers an assertion
    //cv::Scharr(L, d2Ldxdy, CV_64F, 1, 1);

    z[0] = abs(L);
    z[1] = abs(a);
    z[2] = abs(b);
    z[3] = abs(dLdx);
    z[4] = abs(dLdy);
    z[5] = abs(d2Ldx2);
    z[6] = abs(d2Ldy2);
    z[7] = abs(d2Ldxdy);

    return z;
}


cv::Mat Wr(int r) {
	int sizePatch = 2 * r + 1;
	cv::Mat Wr(sizePatch, sizePatch, CV_64F);
	double sigma_carre = pow(r / 3, 2);
	double Z = 0.0;
	for (int i = -r; i <= r; i++) {
		for (int j = -r; j <= r; j++) {
			double dist_pq = pow((double)sqrt(i*i + j*j), 2);
			double w = exp(-(dist_pq) / (2.0 * sigma_carre));
			Wr.at<double>(i + r, j + r) = w;
			Z += w;
		}
	}
	return Wr/Z;
}