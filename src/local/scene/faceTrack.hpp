#ifndef FACETRACK_HPP
#define FACETRACK_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>

class faceTrack
{
public:
    /** \brief Constructor */
    faceTrack();
    /** \brief Detection face Function */
    IplImage* detectFaces(IplImage *img_src);

private:
    IplImage *img_dest;
    CvHaarClassifierCascade *cascade; //Classificateur
    CvMemStorage *storage; //Zone mémoire utilisé comme buffer pour la détection des visages
    int key;

};

#endif // FACETRACK_HPP
