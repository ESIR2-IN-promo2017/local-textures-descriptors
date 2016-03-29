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
    unsigned int ii = (unsigned int) std::max(0l, std::min(i, (long) mat.rows));
    unsigned int jj = (unsigned int) std::max(0l, std::min(j, (long) mat.cols));

    return mat.at<float>(ii,jj);
}

TextureDescriptor::TextureDescriptor()
{
}

TextureDescriptor::TextureDescriptor(std::vector<cv::Mat> const& attribVector, unsigned int i, unsigned int j, cv::Mat const& ponderations, unsigned int r):
    m_descriptor(this->calculateSize(attribVector.size()), 1, CV_32F)
{
    double beta = 1/sum(ponderations)[0];
    cv::Mat mu = calculMoyenne(attribVector, i, j, ponderations, beta, r);

    cv::Mat crp = cv::Mat::zeros(8,8, CV_32F);

    for(unsigned int qi = 0; qi < 2*r+1; ++qi)
        for(unsigned int qj = 0; qj < 2*r+1; ++qj)
        {
            cv::Mat z = extractAttribVector(attribVector, i + qi - r, j + qj - r) - mu;
            crp += (z * z.t()) * (ponderations.at<float>(qi, qj));
        }

    crp *= beta;

    cv::Mat choleskyMatrix;
    this->Cholesky(crp, choleskyMatrix);

    extractDescriptorFromCholesky(choleskyMatrix);
}

double TextureDescriptor::distance(TextureDescriptor const& rhs) const
{
    //TODO
    float sum = 0.0;

    if(this->m_descriptor.rows != rhs.m_descriptor.rows)
        return 0.0;

    for(int i=0; i < this->m_descriptor.rows; i++) {
        float tmp = this->m_descriptor.at<float>(i, 0) - rhs.m_descriptor.at<float>(i, 0);
        sum += tmp * tmp;
    }
    return std::sqrt(sum);
}

cv::Mat TextureDescriptor::extractAttribVector(std::vector<cv::Mat> const& attribVector, unsigned int i, unsigned int j)
{
    cv::Mat retAttribVector;
    for(unsigned int it = 0; it < attribVector.size(); ++it)
        retAttribVector.at<float>(it,1) = getPixel(attribVector[it], i, j);

    return retAttribVector;
}

cv::Mat TextureDescriptor::calculMoyenne(std::vector<cv::Mat> const& attribVector, unsigned int i, unsigned int j, cv::Mat ponderations, double beta, unsigned int r)
{
    cv::Mat mu;
    for(unsigned int ii = 0; ii < 2*r+1; ++ii)
        for(unsigned int jj = 0; jj < 2*r+1; ++jj)
            mu += ponderations.at<float>(ii,jj) * extractAttribVector(attribVector, i + ii - r, j + jj - r);

    return mu * beta;
}

void TextureDescriptor::Cholesky(cv::Mat const& A, cv::Mat & S)
{
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
}

void TextureDescriptor::extractDescriptorFromCholesky(cv::Mat const& choleskyMatrix)
{
    unsigned int position = 0;
    for(int col = 0; col < choleskyMatrix.cols; ++col)
        for(int row = col; row < choleskyMatrix.rows; ++row)
            m_descriptor.at<float>(1, position++) = choleskyMatrix.at<float>(col, row);
}

unsigned int TextureDescriptor::calculateSize(unsigned int size)
{
    unsigned int length = 0;
    for(unsigned int i = size; i > 0; --i)
        length += i;
    return length;
}

/*----DESCRIPTOR----*/

Descriptor::Descriptor(cv::Mat const& img, unsigned int r):
    m_patch_size(r),
    m_w(img.cols),
    m_h(img.rows)
{
    cvtColor(img, m_img, CV_BGR2Lab);
    calculPonderations();
    calculVecteurAttributs();
    for(int i = 0; i < m_img.rows; ++i)
        for(int j = 0; j < m_img.cols; ++j)
        {
            m_descriptors[i*m_img.cols + j] = new TextureDescriptor(m_attribVector, i, j, m_ponderations, m_patch_size);
        }
}

Descriptor::~Descriptor()
{
    for(auto descriptor: m_descriptors)
        delete descriptor;
}

TextureDescriptor Descriptor::at(unsigned int i, unsigned int j) const
{
    i %= m_h;
    j %= m_w;
    return *m_descriptors[i*m_w + j];
}

void Descriptor::calculPonderations()
{
    m_ponderations = cv::Mat(m_patch_size, m_patch_size, CV_32F);

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
}

void Descriptor::calculVecteurAttributs()
{
    std::vector<cv::Mat> Lab;
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
}
