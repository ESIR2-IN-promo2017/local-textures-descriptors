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
    cv::Scharr(L, d2Ldx2, CV_64F, 2, 0);
    cv::Scharr(L, d2Ldy2, CV_64F, 0, 2);
    cv::Scharr(L, d2Ldxdy, CV_64F, 1, 1);

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
