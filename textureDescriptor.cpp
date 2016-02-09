//#include "textureDescriptor.h"

#include <iostream>
#include <array>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

template<typename T>
T max(T const& a, T const& b)
{
    return (a > b)? a : b;
}

static void Cholesky( const cv::Mat& A, cv::Mat& S )
{
    CV_Assert(A.type() == CV_32F);
    int dim = A.rows;
    S.create(dim, dim, CV_32F);

    for(int i = 0; i < dim; i++ )
    {
        for(int j = 0; j < i; j++ )
            S.at<float>(i,j) = 0.f;

        float sum = 0.f;
        for(int k = 0; k < i; k++ )
        {
            float val = S.at<float>(k,i);
            sum += val*val;
        }

        S.at<float>(i,i) = sqrt(max(A.at<float>(i,i) - sum, 0.f));
        float ival = 1.f/S.at<float>(i, i);

        for(int j = i + 1; j < dim; j++ )
        {
            sum = 0;
            for(int k = 0; k < i; k++ )
                sum += S.at<float>(k, i) * S.at<float>(k, j);

            S.at<float>(i, j) = (A.at<float>(i, j) - sum)*ival;
        }
    }
    transpose(S,S);
}

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
    double sigma_carre = pow((double)r / 3.0, 2);
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

double beta(cv::Mat const& Wr)
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
        for(unsigned int qj = 0; qj < 2*r+1; ++qj)
        {
            //qi in [0, 2r+1[ (therefore qi in patch width)
            //so qi-r in [-r, r]
            mu += Wr.at<double>(qi, qj)*Zq(Z, i + qi-r, j + qj-r);
        }

    mu *= beta;

    return mu;
}

std::vector<std::vector<cv::Mat> > Crp(std::array<cv::Mat, 8> const& Z, cv::Mat const& Wr, double const& beta, unsigned int r)
{
    std::cout << "Z : " << Z[0].at<double>(0,0) << std::endl;
    std::cout << "Wr : " << Wr.at<double>(0,0) << std::endl;
    std::cout << "beta : " << beta << std::endl;
    std::cout << "r : " << r << std::endl;

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

            //std::cout << Crp.at<double>(0, 0) << std::endl;

            Crp *= beta;

            Crp_vector_ligne.push_back(Crp);
        }
        Crp_vector.push_back(Crp_vector_ligne);
    }

    return Crp_vector;
}
