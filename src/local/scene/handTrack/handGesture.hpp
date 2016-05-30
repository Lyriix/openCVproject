#ifndef _HAND_GESTURE_
#define _HAND_GESTURE_ 

#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>

//using namespace cv;
//using namespace std;

class HandGesture{
	public:
		HandGesture();
        std::vector<std::vector<cv::Point> > contours;
        std::vector<std::vector<int> >hullI;
        std::vector<std::vector<cv::Point> >hullP;
        std::vector<std::vector<cv::Vec4i> > defects;
        std::vector <cv::Point> fingerTips;
//		Rect rect;
        int cIdx;
		int frameNumber;
        int mostFrequentFingerNumber;
        int nrOfDefects;
        cv::Rect bRect;
        double bRect_width;
        double bRect_height;
        bool isHand;
        bool detectIfHand();
        void initVectors();
        void getFingerNumber(cv::Mat img);
        void eleminateDefects();
        void getFingerTips(cv::Mat img);
        void drawFingerTips(cv::Mat img);
	private:
//		string bool2string(bool tf);
//		int fontFace;
//		int prevNrFingerTips;
        void checkForOneFinger(cv::Mat img);
        float getAngle(cv::Point s,cv::Point f,cv::Point e);
        std::vector<int> fingerNumbers;
        void analyzeContours();
//		string intToString(int number);
        void computeFingerNumber();
//		void drawNewNumber(MyImage *m);
        void addNumberToImg(cv::Mat img);
        std::vector<int> numbers2Display;
        void addFingerNumberToVector();
        cv::Scalar numberColor;
        int nrNoFinger;
        float distanceP2P(cv::Point a,cv::Point b);
        void removeRedundantEndPoints(std::vector<cv::Vec4i> newDefects);
        void removeRedundantFingerTips();
};




#endif
