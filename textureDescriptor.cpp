//#include "textureDescriptor.h"

#include <iostream>
#include <array>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

std::array<cv::Mat, 8> Z(cv::Mat const& img)
{
    std::array<cv::Mat, 8> z; //return array

    std::vector<cv::Mat> Lab;
    cv::split(img, Lab);

    cv::Mat L(Lab[0]), a(Lab[1]), b(Lab[2]);

    cv::Mat dLdx, dLdy, d2Ldx2, d2Ldy2, d2Ldxdy;

    cv::Sobel(L, dLdx, CV_64F, 1, 0, CV_SCHARR);
    cv::Sobel(L, dLdy, CV_64F, 0, 1, CV_SCHARR);
    cv::Sobel(L, d2Ldx2, CV_64F, 2, 0);
    cv::Sobel(L, d2Ldy2, CV_64F, 0, 2);
    // Triggers an assertion
    cv::Sobel(L, d2Ldxdy, CV_64F, 1, 1);

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

cv::Mat Wr(int r)
{
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

inline double beta(cv::Mat const& Wr)
{
    //sum(Wr) is Scalar, and sum(Wr)[0] is the real part
    return 1/(double)sum(Wr)[0];
}

inline cv::Mat Zq(std::array<cv::Mat, 8> const& Z,
        unsigned int const i,
        unsigned int const j)
{
    cv::Mat Zq(8,1, CV_64F);
    for(unsigned int ii = 0; ii < 8; ++ii)
        Zq.at<double>(ii) = Z[ii].at<double>(i,j);

    return Zq;
}

inline cv::Mat MUr(std::array<cv::Mat, 8> const& Z,
        cv::Mat const& Wr,
        double const& beta,
        unsigned int const r,
        unsigned int const i,
        unsigned int const j)
{
    cv::Mat mu(8,1, CV_64F);

    //mu = beta * sum[q in N] ( Wr(p,q) * z(q) )

    for(unsigned int qi = 0; qi < 2*r+1; ++qi)
        for(unsigned int qj = 0; qj < 2*r+1; ++qi)
        {
            //qi in [0, 2r+1[ (therefore qi in patch width)
            //so qi-r in [-r, r]
            mu += Wr.at<double>(qi, qj)*Zq(Z, i + qi-r, j + qj-r);
        }

    mu *= beta;

    return mu;
}

std::vector<std::vector<cv::Mat> > Crp(std::array<cv::Mat, 8> const& Z, cv::Mat const& Wr, double beta, unsigned int r)
{
    cv::Size imgSize = Z[0].size();

    std::vector<std::vector<cv::Mat> > Crp_vector;

    //TODO: edge gestion
    for(unsigned int i = r; i < imgSize.height - r; ++i)
    {
        std::vector<cv::Mat> Crp_vector_ligne;
        for(unsigned int j = r; j < imgSize.width - r; ++j)
        {
            cv::Mat mu = MUr(Z, Wr, beta, r, i, j);

            cv::Mat Crp = cv::Mat::zeros(8,8, CV_64F);

            for(unsigned int qi = 0; qi < 2*r+1; ++qi)
                for(unsigned int qj = 0; qj < 2*r+1; ++qj)
                {
                    cv::Mat zq = Zq(Z, i + qi-r, j + qj-r);
                    Crp += (zq*zq.t())*(Wr.at<double>(qi,qj));
                }

            Crp *= beta;

            Crp_vector_ligne.push_back(Crp);
        }
        Crp_vector.push_back(Crp_vector_ligne);
    }

    return Crp_vector;
}
