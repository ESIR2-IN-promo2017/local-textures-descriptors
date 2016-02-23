#pragma once

#include <iostream>
#include <array>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>

//TODO: gestion des bords

/*
 * types de gestion des bords (cf doc openCV) :
 *
 *> Various border types, image boundaries are denoted with '|'
 *
 *> BORDER_REPLICATE:     aaaaaa|abcdefgh|hhhhhhh
 *> BORDER_REFLECT:       fedcba|abcdefgh|hgfedcb
 *> BORDER_REFLECT_101:   gfedcb|abcdefgh|gfedcba
 *> BORDER_WRAP:          cdefgh|abcdefgh|abcdefg
 *> BORDER_CONSTANT:      iiiiii|abcdefgh|iiiiiii  with some specified 'i'
 */

/**
 * @brief Calculates upper triangular matrix S, where A is a symmetrical matrix A=S*S'
 *
 * @param A: la matrice à décomposer
 * @param S: une matrice dans laquelle insérer le résultat
 */
static void Cholesky(cv::Mat const& A, cv::Mat & S);

/**
 * @brief calcule le vecteur de paramètres pour l'image
 *
 * @param img: l'image à traiter
 * @pre: img est une image en Lab (voir cv::cvtColor)
 *
 * @return le vecteur [ L a b |dL/dx| |dL/dy| |d²L/dx²| |d²L/dy²| |d²L/dxdy| ] pour toute l'image
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

/**
 * @brief calcule le facteur de normalisation associé à la matrice de coefficients
 *
 * @param Wr: la matrice de coefficients du patch
 *
 * @return 1/(somme des coefficients de Wr)
 */
float beta(cv::Mat const& Wr);

/**
 * @brief recupere le vecteur de paramètre z(q) pour un pixel
 *
 * @param Z: le vecteur de paramètres de toute l'image (cf Z(cv::Mat))
 * @param i: ligne du pixel dans l'image
 * @param j: colonne du pixel dans l'image
 *
 * @return: le vecteur [ L(p) a(p) b(p) |dL(p)/dx| |dL(p)/dy| |d²L(p)/dx²| |d²L(p)/dy²| |d²L(p)/dxdy| ] pour le pixel p = (i,j)
 */
inline cv::Mat Zq(std::array<cv::Mat, 8> const& Z, unsigned int const i, unsigned int const j);

/**
 * @brief calcule le vecteur moyen du vecteur de paramètres pour un patch
 *
 * @param Z: le vecteur de paramètres de toute l'image
 * @param Wr: matrice des poids dans le patch
 * @param beta: coefficient de normalisation de Wr (1/somme des coeffs de Wr)
 * @param r: taille du patch (le patch sera de taille 2*r+1)
 * @param i: ligne du pixel au centre du patch
 * @param j: colonne du pixel au centre du patch
 *
 * @return: un vecteur moyennant tous les vecteurs z(q) du patch
 */
inline cv::Mat MUr(std::array<cv::Mat, 8> const& Z, cv::Mat const& Wr, float const& beta, unsigned int const r, unsigned int const i, unsigned int const j);

/**
 * @brief calcule les matrices de covariances de toute l'image
 *
 * @param Z: vecteur de paramètres de toute l'image
 * @param Wr: matrice des poids dans un patch
 * @param beta: coefficient de normalisation de Wr (1/somme des coeffs de Wr)
 * @param r: taille du patch (le patch sera de taille 2*r+1)
 *
 * @return un tableau contenant les matrices de covariance de Z(p) de chaque pixel
 */
std::vector<std::vector<cv::Mat> > Crp(std::array<cv::Mat, 8> const& Z, cv::Mat const& Wr, float const& beta, unsigned int r);
