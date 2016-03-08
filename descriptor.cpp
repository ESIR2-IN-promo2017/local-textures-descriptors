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
}

Descriptor::Descriptor(cv::Mat const& img)
{
    m_descriptors = new TextureDescriptor[m_img.rows * m_img.cols];

    calculPonderations();

    for(unsigned int i = 0; i < img.rows; ++i)
        for(unsigned int j = 0; j < img.cols; ++j)
        {
        }
}

TextureDescriptor Descriptor::at(unsigned int i, unsigned int j) const
{
    return m_descriptors[i*m_img.cols + j];
}
