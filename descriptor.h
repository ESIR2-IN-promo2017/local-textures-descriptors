/**
 * @file descriptor.h
 * @brief 
 * @author Amaury Louarn
 * @version 1.0
 * @date 2016-03-08
 */

#include <iostream>
#include <vector>

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class TextureDescriptor
{
    friend class Descriptor;

    public:
        double distance(TextureDescriptor const& rhs) const;

    protected:
        TextureDescriptor();
        TextureDescriptor(std::vector<cv::Mat> const& attribVector, unsigned int i, unsigned int j);

    private:
        cv::Mat m_descriptor;
};

class Descriptor
{
    public:
        Descriptor(cv::Mat const& img);
        ~Descriptor();

        TextureDescriptor at(unsigned int i, unsigned int j) const;

    private:
        void calculPonderations();
        void calculVecteurAttributs();

    private:
        unsigned int const m_patch_size = 1;

        cv::Mat m_img;

        cv::Mat m_ponderations;
        float m_beta;

        std::vector<cv::Mat> m_attribVector;

        TextureDescriptor* m_descriptors;
};
