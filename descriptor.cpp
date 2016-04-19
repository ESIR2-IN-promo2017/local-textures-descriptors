/**
 * @file descriptor.cpp
 * @brief 
 * @author Amaury Louarn
 * @version 1.0
 * @date 2016-03-08
 */

#include "descriptor.h"

float getPixel(cv::Mat const& mat, long i, long j)
{
#ifdef DEBUG
    std::cout << "--> getPixel(mat=" << &mat << ", i=" << i << ", j=" << j << ")" << std::endl;
#endif
    unsigned int ii = (unsigned int) std::max(0l, std::min(i, (long) mat.rows));
    unsigned int jj = (unsigned int) std::max(0l, std::min(j, (long) mat.cols));

    return mat.at<float>(ii,jj);
#ifdef DEBUG
    std::cout << "<-- getPixel()" << std::endl;
#endif
}

TextureDescriptor::TextureDescriptor()
{
#ifdef DEBUG
    std::cout << "--> TextureDescriptor::TextureDescriptor()" << std::endl;
#endif
#ifdef DEBUG
    std::cout << "<-- TextureDescriptor::TextureDescriptor()" << std::endl;
#endif
}

TextureDescriptor::TextureDescriptor(std::vector<cv::Mat> const& attribVector, unsigned int i, unsigned int j, cv::Mat const& ponderations, unsigned int r):
    m_descriptor(this->calculateSize(attribVector.size()), 1, CV_32F)
{
#ifdef DEBUG
    std::cout << "--> TextureDescriptor::TextureDescriptor(attribVector=" << &attribVector << ", i=" << i << ", j=" << j << ", ponderations=" << &ponderations << ", r=" << r << ")" << std::endl;
#endif
    double beta = 1/sum(ponderations)[0];
    cv::Mat mu = calculMoyenne(attribVector, i, j, ponderations, beta, r);

    cv::Mat crp = cv::Mat::zeros(attribVector.size(), attribVector.size(), CV_32F);

    for(long qi = 0; qi < 2*r+1; ++qi)
        for(long qj = 0; qj < 2*r+1; ++qj)
        {
            cv::Mat z = extractAttribVector(attribVector, i + qi - r, j + qj - r) - mu;
            crp += (z * z.t()) * (ponderations.at<float>(qi, qj));
        }

    crp *= beta;

    std::cout << "ok" << std::endl;
    cv::Mat choleskyMatrix;
    this->Cholesky(crp, choleskyMatrix);

    extractDescriptorFromCholesky(choleskyMatrix);
#ifdef DEBUG
    std::cout << "<-- TextureDescriptor::TextureDescriptor()" << std::endl;
#endif
}

double TextureDescriptor::distance(TextureDescriptor const& rhs) const
{
#ifdef DEBUG
    std::cout << "--> TextureDescriptor::distance(this=" << this << ", rhs=" << &rhs << ")" << std::endl;
#endif
    //TODO
    float sum = 0.0;

    if(this->m_descriptor.rows != rhs.m_descriptor.rows)
        return 0.0;

    for(int i=0; i < this->m_descriptor.rows; i++) {
        float tmp = this->m_descriptor.at<float>(i, 0) - rhs.m_descriptor.at<float>(i, 0);
        sum += tmp * tmp;
    }
#ifdef DEBUG
    std::cout << "<-- TextureDescriptor::distance()" << std::endl;
#endif
    return std::sqrt(sum);
}

cv::Mat TextureDescriptor::extractAttribVector(std::vector<cv::Mat> const& attribVector, long i, long j)
{
#ifdef DEBUG
    std::cout << "--> TextureDescriptor::extractAttribVector(attribVector=" << &attribVector << ", i=" << i << ", j=" << j << ")" << std::endl;
#endif
    cv::Mat retAttribVector(attribVector.size(), 1, CV_32F);
    for(unsigned int it = 0; it < attribVector.size(); ++it)
        retAttribVector.at<float>(it,1) = getPixel(attribVector[it], i, j);

#ifdef DEBUG
    std::cout << "<-- TextureDescriptor::extractAttribVector()" << std::endl;
#endif
    return retAttribVector;
}

cv::Mat TextureDescriptor::calculMoyenne(std::vector<cv::Mat> const& attribVector, unsigned int i, unsigned int j, cv::Mat ponderations, double beta, unsigned int r)
{
#ifdef DEBUG
    std::cout << "--> TextureDescriptor::calculMoyenne(attribVector=" << &attribVector << ", i=" << i << ", j=" << j << ", ponderations=" << &ponderations << ", beta=" << beta << ", r=" << r << ")" << std::endl;
#endif
    cv::Mat mu(attribVector.size(), 1, CV_32F);
    for(unsigned int ii = 0; ii < 2*r+1; ++ii)
        for(unsigned int jj = 0; jj < 2*r+1; ++jj)
            mu += ponderations.at<float>(ii,jj) * extractAttribVector(attribVector, (long) i + ii - r, (long) j + jj - r);

#ifdef DEBUG
    std::cout << "<-- TextureDescriptor::calculMoyenne()" << std::endl;
#endif
    return mu * beta;
}

void TextureDescriptor::Cholesky(cv::Mat const& A, cv::Mat & S)
{
#ifdef DEBUG
    std::cout << "--> TextureDescriptor::Cholesky(A=" << &A << ", S=" << &S << ")" << std::endl;
#endif
    CV_Assert(A.type() == CV_32F);
    int dim = A.rows;
    S.create(dim, dim, CV_32F);

    cv::Mat E, M; 
    cv::eigen(A, E, M);

    cv::Mat Ebis= cv::Mat::diag(E);

    //assert that every eigenvalue is > 0
    for(int i=0; i<8; i++)
        for(int j=0; j<8; j++)
            if(Ebis.at<float>(i,j)<0.0)
                Ebis.at<float>(i,j)=0.0;

    cv::Mat Abis= M.t()*Ebis*M;
    for(int i = 0; i < dim; i++ ){
        for(int j = 0; j < i; j++ )
            S.at<float>(i,j) = 0.f;

        float sum = 0.f;
        for(int k = 0; k < i; k++ )
        {
            float val = S.at<float>(k,i);
            sum += val*val;
        }

        S.at<float>(i,i) = std::sqrt(std::max(Abis.at<float>(i,i) - sum, 0.f));
        float ival = 1.f/S.at<float>(i, i);

        for(int j = i + 1; j < dim; j++)
        {
            sum = 0;
            for(int k = 0; k < i; k++ )
                sum += S.at<float>(k, i) * S.at<float>(k, j);

            S.at<float>(i, j) = (Abis.at<float>(i, j) - sum)*ival;
        }
    }
    transpose(S,S);
#ifdef DEBUG
    std::cout << "<-- TextureDescriptor::Cholesky()" << std::endl;
#endif
}

void TextureDescriptor::extractDescriptorFromCholesky(cv::Mat const& choleskyMatrix)
{
#ifdef DEBUG
    std::cout << "--> TextureDescriptor::extractDescriptorFromCholesky(choleskyMatrix=" << &choleskyMatrix << ")" << std::endl;
#endif
    unsigned int position = 0;
    for(int col = 0; col < choleskyMatrix.cols; ++col)
        for(int row = col; row < choleskyMatrix.rows; ++row)
            m_descriptor.at<float>(1, position++) = choleskyMatrix.at<float>(col, row);
#ifdef DEBUG
    std::cout << "<-- TextureDescriptor::extractDescriptorFromCholesky()" << std::endl;
#endif
}

unsigned int TextureDescriptor::calculateSize(unsigned int size)
{
#ifdef DEBUG
    std::cout << "--> TextureDescriptor::calculateSize(size=" << size << ")" << std::endl;
#endif
    unsigned int length = 0;
    for(unsigned int i = size; i > 0; --i)
        length += i;
#ifdef DEBUG
    std::cout << "<-- TextureDescriptor::calculateSize()" << std::endl;
#endif
    return length;
}

/*----DESCRIPTOR----*/

Descriptor::Descriptor(cv::Mat const& img, unsigned int r):
    m_patch_size(r),
    m_w(img.cols),
    m_h(img.rows),
    m_attribVector(),
    m_descriptors()
{
#ifdef DEBUG
    std::cout << "--> Descriptor::Descriptor(img=" << &img << ", r=" << r <<")" << std::endl;
#endif
    cvtColor(img, m_img, CV_BGR2Lab);
    calculPonderations();
    calculVecteurAttributs();
    for(unsigned int i = 0; i < (unsigned int) m_img.rows; ++i)
        for(unsigned int j = 0; j < (unsigned int) m_img.cols; ++j)
        {
            m_descriptors.push_back(new TextureDescriptor(m_attribVector, i, j, m_ponderations, m_patch_size));
        }
#ifdef DEBUG
    std::cout << "<-- Descriptor::Descriptor()" << std::endl;
#endif
}

Descriptor::~Descriptor()
{
#ifdef DEBUG
    std::cout << "--> Descriptor::~Descriptor()" << std::endl;
#endif
    for(auto descriptor: m_descriptors)
        delete descriptor;
#ifdef DEBUG
    std::cout << "<-- Descriptor::~Descriptor()" << std::endl;
#endif
}

TextureDescriptor Descriptor::at(unsigned int i, unsigned int j) const
{
#ifdef DEBUG
    std::cout << "--> Descriptor::at(i=" << i << ", j=" << j << ")" << std::endl;
#endif
    i %= m_h;
    j %= m_w;
#ifdef DEBUG
    std::cout << "<-- Descriptor::at()" << std::endl;
#endif
    return *m_descriptors[i*m_w + j];
}

void Descriptor::calculPonderations()
{
#ifdef DEBUG
    std::cout << "--> Descriptor::calculPonderations()" << std::endl;
#endif
    m_ponderations = cv::Mat(2*m_patch_size+1, 2*m_patch_size+1, CV_32FC1);

    float sigma_carre = std::pow((float) m_patch_size / 3.0, 2);

    if(m_patch_size == 0)
        sigma_carre = 1;

    float Z = 0.0;
    for (int i = -m_patch_size; i <= (long) m_patch_size; i++)
        for (int j = -m_patch_size; j <= (long) m_patch_size; j++)
        {
            float dist_pq = (float) std::sqrt(i*i + j*j);
            float w = exp(-(dist_pq) / (2.0 * sigma_carre));
            m_ponderations.at<float>(i + m_patch_size, j + m_patch_size) = w;
            Z += w;
        }

    m_ponderations /= Z;
    m_beta = 1/Z;
#ifdef DEBUG
    std::cout << "<-- Descriptor::calculPonderations()" << std::endl;
#endif
}

void Descriptor::calculVecteurAttributs()
{
#ifdef DEBUG
    std::cout << "--> Descriptor::calculVecteurAttributs()" << std::endl;
#endif
    cv::Mat Lab[3];
    for(unsigned int channel = 0; channel < 3; ++channel)
        Lab[channel] = cv::Mat(m_img.rows, m_img.cols, CV_32F);
    cv::split(m_img, Lab);

    cv::Mat dLdx, dLdy, d2Ldx2, d2Ldy2, d2Ldxdy;

    cv::Sobel(Lab[0], dLdx, CV_32F, 1, 0, CV_SCHARR);
    cv::Sobel(Lab[0], dLdy, CV_32F, 0, 1, CV_SCHARR);
    cv::Sobel(Lab[0], d2Ldx2, CV_32F, 2, 0);
    cv::Sobel(Lab[0], d2Ldy2, CV_32F, 0, 2);
    cv::Sobel(Lab[0], d2Ldxdy, CV_32F, 1, 1);

    m_attribVector.push_back(abs(Lab[0]));
    m_attribVector.push_back(abs(Lab[1]));
    m_attribVector.push_back(abs(Lab[2]));
    m_attribVector.push_back(abs(dLdx));
    m_attribVector.push_back(abs(dLdy));
    m_attribVector.push_back(abs(d2Ldx2));
    m_attribVector.push_back(abs(d2Ldy2));
    m_attribVector.push_back(abs(d2Ldxdy));

#ifdef DEBUG
    std::cout << "<-- Descriptor::calculVecteurAttributs()" << std::endl;
#endif
}
