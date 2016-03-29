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
        TextureDescriptor(std::vector<cv::Mat> const& attribVector, unsigned int i, unsigned int j, cv::Mat const& ponderations, unsigned int r);

        cv::Mat extractAttribVector(std::vector<cv::Mat> const& attribVector, unsigned int i, unsigned int j);
        cv::Mat calculMoyenne(std::vector<cv::Mat> const& attribVector, unsigned int i, unsigned int j, cv::Mat ponderations, double beta, unsigned int r);
        void Cholesky(cv::Mat const& A, cv::Mat & S);
        void extractDescriptorFromCholesky(cv::Mat const& choleskyMatrix);

        unsigned int calculateSize(unsigned int size);

    private:
        cv::Mat m_descriptor;
        double m_beta;
};

class Descriptor
{
    public:
        /**
         * @brief constructs the Descriptor
         *
         * @param img: image to descript
         * @param r: radius of the patch (the patch will be of size 2*r+1)
         */
        Descriptor(cv::Mat const& img, unsigned int r);
        /**
         * @brief Descriptor destructor
         */
        ~Descriptor();

        /**
         * @brief gets a TextureDescriptor
         *
         * @param i: row number
         * @param j: column number
         *
         * @return the TextureDescriptor corresponding to the patch around (i,j)
         */
        TextureDescriptor at(unsigned int i, unsigned int j) const;

    private:
        void calculPonderations();
        void calculVecteurAttributs();

    private:
        unsigned int const m_patch_size;
        unsigned int m_w;
        unsigned int m_h;

        cv::Mat m_img;

        cv::Mat m_ponderations;
        float m_beta;

        std::vector<cv::Mat> m_attribVector;


        TextureDescriptor* m_descriptors;
};
