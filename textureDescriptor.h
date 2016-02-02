#pragma once

#include <iostream>
#include <array>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
 * @brief calcule le vecteur de paramètres pour l'image
 *
 * @param img: l'image à traiter
 * @pre: img est une image en Lab (voir cv::cvtColor)
 *
 * @return le vecteur [ L(p) a(p) b(p) |dL(p)/dx| |dL(p)/dy| |d²L(p)/dx²| |d²L(p)/dy²| |d²L(p)/dxdy| ]
 */
std::array<cv::Mat, 8> Z(cv::Mat const& img);


/**
* @brief calcule la matrice des distances du patch
*
* @param r: taille du patch 2*r+1
*
* @return matrice Wr(p,q)=exp(-(||p-q||²/2*sigma²))
*/
cv::Mat Wr(int r);