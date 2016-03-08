/**
 * @file descriptor.cpp
 * @brief 
 * @author Amaury Louarn
 * @version 1.0
 * @date 2016-03-08
 */

#include "descriptor.h"

TextureDescriptor::TextureDescriptor()
{
    //TODO
}

TextureDescriptor::TextureDescriptor(float L, float a, float b, float dLdx, float dLdy, float d2Ldx2, float d2Ldy2, float d2Ldxdy)
{
    //TODO
}

double TextureDescriptor::distance(TextureDescriptor const& rhs) const
{
    //TODO
    return 0;
}

Descriptor::Descriptor(cv::Mat const& img)
{
    m_descriptors = new TextureDescriptor[m_img.rows * m_img.cols];

    cvtColor(img, m_img, CV_BGR2Lab);
    calculPonderations();
    calculVecteurAttributs();

}

TextureDescriptor Descriptor::at(unsigned int i, unsigned int j) const
{
    return m_descriptors[i*m_img.cols + j];
}

float getPixel(cv::Mat const& mat, long i, long j)
{
    unsigned int ii = (unsigned int) std::max(0l, std::min(i, (long) mat.rows));
    unsigned int jj = (unsigned int) std::max(0l, std::min(j, (long) mat.cols));

    return mat.at<float>(ii,jj);
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
