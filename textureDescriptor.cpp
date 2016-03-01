//#include "textureDescriptor.h"

#include <iostream>
#include <iomanip>
#include <opencv2/highgui/highgui.hpp>
#include <array>
#include <cmath>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

template<typename T>
T max(T const& a, T const& b)
{
    return (a > b)? a : b;
}

void Cholesky(cv::Mat const& A, cv::Mat & S)
{
    CV_Assert(A.type() == CV_32F);
    int dim = A.rows;
    S.create(dim, dim, CV_32F);

    cv::Mat E, M; 
    cv::eigen(A, E, M);

    //std::cout << "val propre A" << E << std::endl;

    cv::Mat Ebis= cv::Mat::diag(E);

    for(int i=0; i<8; i++){
      for(int j=0; j<8; j++){
	if(Ebis.at<double>(i,j)<0.0){
	  Ebis.at<double>(i,j)=0.0;
	}
      }
    }

    cv::Mat Abis= M.t()*Ebis*M;
    /*
    cv::eigen(Abis, E,M);
    std::cout << "val propre Abis" << E << std::endl;
    cv::imshow("A", A);
    cv::imshow("Abis", Abis);
    std::cout << "A :"<< std::endl<< A<<std::endl;
    std::cout << "A bis :"<< std::endl<< Abis<<std::endl;
    */
    for(int i = 0; i < dim; i++ ){
      for(int j = 0; j < i; j++ )
	S.at<float>(i,j) = 0.f;
      
      float sum = 0.f;
      for(int k = 0; k < i; k++ )
	{
	  float val = S.at<float>(k,i);
	  sum += val*val;
	}
      
      S.at<float>(i,i) = sqrt(max(Abis.at<float>(i,i) - sum, 0.f));
      float ival = 1.f/S.at<float>(i, i);
      
      for(int j = i + 1; j < dim; j++ )
	{
	  sum = 0;
	  for(int k = 0; k < i; k++ )
	    sum += S.at<float>(k, i) * S.at<float>(k, j);
	  
	  S.at<float>(i, j) = (Abis.at<float>(i, j) - sum)*ival;
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

    cv::Sobel(L, dLdx, CV_32F, 1, 0, CV_SCHARR);
    cv::Sobel(L, dLdy, CV_32F, 0, 1, CV_SCHARR);
    cv::Sobel(L, d2Ldx2, CV_32F, 2, 0);
    cv::Sobel(L, d2Ldy2, CV_32F, 0, 2);
    cv::Sobel(L, d2Ldxdy, CV_32F, 1, 1);

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
    cv::Mat Wr(sizePatch, sizePatch, CV_32F);
    float sigma_carre = pow((float)r / 3.0, 2);
    if(r == 0)
        sigma_carre = 1;
    float Z = 0.0;
    for (int i = -r; i <= r; i++) {
        for (int j = -r; j <= r; j++) {
            float dist_pq = pow((float)sqrt(i*i + j*j), 2);
            float w = exp(-(dist_pq) / (2.0 * sigma_carre));
            Wr.at<float>(i + r, j + r) = w;
            Z += w;
        }
    }
    return Wr/Z;
}

float beta(cv::Mat const& Wr)
{
    //sum(Wr) is Scalar, and sum(Wr)[0] is the real part
    return 1/(float)sum(Wr)[0];
}

inline cv::Mat Zq(std::array<cv::Mat, 8> const& Z,
        unsigned int const i,
        unsigned int const j)
{
    cv::Mat Zq(8,1, CV_32F);
    for(unsigned int ii = 0; ii < 8; ++ii)
        Zq.at<float>(ii) = Z[ii].at<float>(i,j);

    return Zq;
}

inline cv::Mat MUr(std::array<cv::Mat, 8> const& Z,
        cv::Mat const& Wr,
        float const& beta,
        unsigned int const r,
        unsigned int const i,
        unsigned int const j)
{
    cv::Mat mu(8,1, CV_32F);

    //mu = beta * sum[q in N] ( Wr(p,q) * z(q) )

    for(unsigned int qi = 0; qi < 2*r+1; ++qi)
        for(unsigned int qj = 0; qj < 2*r+1; ++qj)
        {
            //qi in [0, 2r+1[ (therefore qi in patch width)
            //so qi-r in [-r, r]
            mu += /*Wr.at<float>(qi, qj)* */ Zq(Z, i + qi-r, j + qj-r);
        }

    //mu *= beta;
    mu /= (2*r+1) * (2*r+1);

    return mu;
}

std::vector<std::vector<cv::Mat> > Crp(std::array<cv::Mat, 8> const& Z, cv::Mat const& Wr, float const& beta, unsigned int r)
{
    std::cout << "Z : " << Z[0].at<float>(0,0) << std::endl;
    std::cout << "Wr : " << Wr.at<float>(0,0) << std::endl;
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
            //std::cout << mu << std::endl;

            cv::Mat Crp = cv::Mat::zeros(8,8, CV_32F);

            for(unsigned int qi = 0; qi < 2*r+1; ++qi)
                for(unsigned int qj = 0; qj < 2*r+1; ++qj)
                {
                    cv::Mat zq = Zq(Z, i + qi-r, j + qj-r) - mu;
                    //std::cout << "Zq [" << qi << "," << qj << "]" << std::endl;
                    //std::cout << zq << std::endl;
                    Crp += (zq*zq.t())*(Wr.at<float>(qi,qj));
                }

            //std::cout << Crp.at<float>(0, 0) << std::endl;

            Crp *= beta;

            Crp_vector_ligne.push_back(Crp);
        }
        Crp_vector.push_back(Crp_vector_ligne);
    }

    return Crp_vector;
}
