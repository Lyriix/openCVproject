#ifndef HANDTRACK_HPP
#define HANDTRACK_HPP

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>

#include "roi.hpp"
#include "handGesture.hpp"

#define ORIGCOL2COL CV_BGR2HLS
#define COL2ORIGCOL CV_HLS2BGR
#define NSAMPLES 7

class handTrack
{
public:
    /** Constructor */
    handTrack();
    /** Init handtrack */
    void initHandTrack(cv::VideoCapture capture);
    /** Permet de dessiner les rectangles d'etalonnage */
    void waitForPalmCover(cv::VideoCapture capture);
    /** Recuperation des couleurs */
    void average(cv::VideoCapture capture);
    /** Donne la difference de couleurs */
    void getAvgColor(My_ROI roi, int avg[3]);
    /** Median */
    int getMedian(std::vector<int> val);
    /** Passage en binaire */
    void produceBinaries();
    /** Normalisation des couleurs */
    void normalizeColors();
    /** Creation des contours */
    void makeContours();
    /** Detection des plus groscontours */
    int findBiggestContour();
    /** geg */
    void myDrawContours();
    /** Detection main */
    void Detect_Hand(cv::VideoCapture capture);
    /** Initialisation trackbars */
    void init_Trackbars();
    void cut_Mat();

    /** Set boolean to display or no images */
    void setDisplayImages(bool valid);



    /** Getter */
    cv::Mat getimg();
    HandGesture gethg();
    void setname_img(std::string name);
    void setname_trackbar(std::string name);

    int Col_split_begin;
    int Col_split_end;


private:
    cv::Mat img;
    cv::Mat img_LR;
    My_ROI roi1, roi2,roi3,roi4,roi5,roi6;
    std::vector <My_ROI> roi;
    int avgColor[NSAMPLES][3];
    HandGesture hg;
    int c_lower[NSAMPLES][3];
    int c_upper[NSAMPLES][3];
    cv::Mat bw;
    std::vector<cv::Mat> bwList;
    std::string name_img;
    std::string name_trackbar;

    /** Display track images */
    bool display_images;
    bool no_display_images;

};

#endif // HANDTRACK_HPP
