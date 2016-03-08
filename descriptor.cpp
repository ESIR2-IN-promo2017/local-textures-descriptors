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

    calculPonderations();

    for(unsigned int i = 0; i < (unsigned int) img.rows; ++i)
        for(unsigned int j = 0; j < (unsigned int) img.cols; ++j)
        {
        }
}

TextureDescriptor Descriptor::at(unsigned int i, unsigned int j) const
{
    return m_descriptors[i*m_img.cols + j];
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
}
