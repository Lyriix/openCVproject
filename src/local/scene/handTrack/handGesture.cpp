#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <stdio.h>

#include "handGesture.hpp"

HandGesture::HandGesture(){
    frameNumber=0;
//	nrNoFinger=0;
//	fontFace = FONT_HERSHEY_PLAIN;
}

void HandGesture::initVectors(){
    hullI=std::vector<std::vector<int> >(contours.size());
    hullP=std::vector<std::vector<cv::Point> >(contours.size());
    defects=std::vector<std::vector<cv::Vec4i> > (contours.size());
}

void HandGesture::eleminateDefects(){
    int tolerance =  bRect_height/5;
    float angleTol=95;
    std::vector<cv::Vec4i> newDefects;
    int startidx, endidx, faridx;
    std::vector<cv::Vec4i>::iterator d=defects[cIdx].begin();
    while( d!=defects[cIdx].end() ) {
        cv::Vec4i& v=(*d);
        startidx=v[0]; cv::Point ptStart(contours[cIdx][startidx] );
        endidx=v[1]; cv::Point ptEnd(contours[cIdx][endidx] );
        faridx=v[2]; cv::Point ptFar(contours[cIdx][faridx] );
        if(distanceP2P(ptStart, ptFar) > tolerance && distanceP2P(ptEnd, ptFar) > tolerance && getAngle(ptStart, ptFar, ptEnd  ) < angleTol ){
            if( ptEnd.y > (bRect.y + bRect.height -bRect.height/4 ) ){
            }else if( ptStart.y > (bRect.y + bRect.height -bRect.height/4 ) ){
            }else {
                newDefects.push_back(v);
            }
        }
        d++;
    }
    nrOfDefects=newDefects.size();
    defects[cIdx].swap(newDefects);
    removeRedundantEndPoints(defects[cIdx]);
}

float HandGesture::distanceP2P(cv::Point a, cv::Point b){
    float d= sqrt(fabs( pow(a.x-b.x,2) + pow(a.y-b.y,2) )) ;
    return d;
}

float HandGesture::getAngle(cv::Point s, cv::Point f, cv::Point e){
    float l1 = distanceP2P(f,s);
    float l2 = distanceP2P(f,e);
    float dot=(s.x-f.x)*(e.x-f.x) + (s.y-f.y)*(e.y-f.y);
    float angle = acos(dot/(l1*l2));
    angle=angle*180/M_PI;
    return angle;
}

// remove fingertips that are too close to
// eachother
void HandGesture::removeRedundantFingerTips(){
    std::vector<cv::Point> newFingers;
    for(int i=0;i<fingerTips.size();i++){
        for(int j=i;j<fingerTips.size();j++){
            if(distanceP2P(fingerTips[i],fingerTips[j])<10 && i!=j){
            }else{
                newFingers.push_back(fingerTips[i]);
                break;
            }
        }
    }
    fingerTips.swap(newFingers);
}

// remove endpoint of convexity defects if they are at the same fingertip
void HandGesture::removeRedundantEndPoints(std::vector<cv::Vec4i> newDefects){
    cv::Vec4i temp;
    float avgX, avgY;
    float tolerance=bRect_width/6;
    int startidx, endidx, faridx;
    int startidx2, endidx2;
    for(int i=0;i<newDefects.size();i++){
        for(int j=i;j<newDefects.size();j++){
            startidx=newDefects[i][0]; cv::Point ptStart(contours[cIdx][startidx] );
            endidx=newDefects[i][1]; cv::Point ptEnd(contours[cIdx][endidx] );
            startidx2=newDefects[j][0]; cv::Point ptStart2(contours[cIdx][startidx2] );
            endidx2=newDefects[j][1]; cv::Point ptEnd2(contours[cIdx][endidx2] );
            if(distanceP2P(ptStart,ptEnd2) < tolerance ){
                contours[cIdx][startidx]=ptEnd2;
                break;
            }if(distanceP2P(ptEnd,ptStart2) < tolerance ){
                contours[cIdx][startidx2]=ptEnd;
            }
        }
    }
}

bool HandGesture::detectIfHand(){
    analyzeContours();
    double h = bRect_height;
    double w = bRect_width;
    isHand=true;
    if(fingerTips.size() > 5 ){
        isHand=false;
    }else if(h==0 || w == 0){
        isHand=false;
    }else if(h/w > 4 || w/h >4){
        isHand=false;
    }else if(bRect.x<20){
        isHand=false;
    }
    return isHand;
}

void HandGesture::analyzeContours(){
    bRect_height=bRect.height;
    bRect_width=bRect.width;
}

void HandGesture::getFingerTips(cv::Mat img){
    fingerTips.clear();
    int i=0;
    std::vector<cv::Vec4i>::iterator d=defects[cIdx].begin();
    while( d!=defects[cIdx].end() ) {
        cv::Vec4i& v=(*d);
        int startidx=v[0]; cv::Point ptStart(contours[cIdx][startidx] );
        int endidx=v[1]; cv::Point ptEnd(contours[cIdx][endidx] );
        int faridx=v[2]; cv::Point ptFar(contours[cIdx][faridx] );
        if(i==0){
            fingerTips.push_back(ptStart);
            i++;
        }
        fingerTips.push_back(ptEnd);
        d++;
        i++;

    }
    if(fingerTips.size()==0){
        checkForOneFinger(img);
    }
}

void HandGesture::checkForOneFinger(cv::Mat img){
    int yTol=bRect.height/6;
    cv::Point highestP;
    highestP.y=img.rows;
    std::vector<cv::Point>::iterator d=contours[cIdx].begin();
    while( d!=contours[cIdx].end() ) {
        cv::Point v=(*d);
        if(v.y<highestP.y){
            highestP=v;
        }
        d++;
    }int n=0;
    d=hullP[cIdx].begin();
    while( d!=hullP[cIdx].end() ) {
        cv::Point v=(*d);

        if(v.y<highestP.y+yTol && v.y!=highestP.y && v.x!=highestP.x){
            n++;
        }
        d++;
    }if(n==0){
        fingerTips.push_back(highestP);
    }
}


void HandGesture::drawFingerTips(cv::Mat img){
    cv::Point p;
    int k=0;
    for(int i=0;i<fingerTips.size();i++){
        p=fingerTips[i];
        cv::circle(img,p,5,cv::Scalar(100,255,100), 4 );
     }
}

// calculate most frequent numbers of fingers
// over 20 frames
void HandGesture::getFingerNumber(cv::Mat img){
    removeRedundantFingerTips();
    if(bRect.height > img.rows/2 && nrNoFinger>12 && isHand ){
        numberColor=cv::Scalar(0,200,0);
        addFingerNumberToVector();
        if(frameNumber>12){
            nrNoFinger=0;
            frameNumber=0;
            computeFingerNumber();
            numbers2Display.push_back(mostFrequentFingerNumber);
            fingerNumbers.clear();
        }else{
            frameNumber++;
        }
    }else{
        nrNoFinger++;
        numberColor=cv::Scalar(200,200,200);
    }
    //addNumberToImg(img);
}

void HandGesture::addFingerNumberToVector(){
    int i=fingerTips.size();
    fingerNumbers.push_back(i);
}

void HandGesture::computeFingerNumber(){
    std::sort(fingerNumbers.begin(), fingerNumbers.end());
    int frequentNr;
    int thisNumberFreq=1;
    int highestFreq=1;
    frequentNr=fingerNumbers[0];
    for(int i=1;i<fingerNumbers.size(); i++){
        if(fingerNumbers[i-1]!=fingerNumbers[i]){
            if(thisNumberFreq>highestFreq){
                frequentNr=fingerNumbers[i-1];
                highestFreq=thisNumberFreq;
            }
            thisNumberFreq=0;
        }
        thisNumberFreq++;
    }
    if(thisNumberFreq>highestFreq){
        frequentNr=fingerNumbers[fingerNumbers.size()-1];
    }
    mostFrequentFingerNumber=frequentNr;
}

// add the calculated number of fingers to image m->src
void HandGesture::addNumberToImg(cv::Mat img){
    int xPos=10;
    int yPos=10;
    int offset=30;
    for(int i=0;i<numbers2Display.size();i++){
        cv::rectangle(img,cv::Point(xPos,yPos),cv::Point(xPos+offset,yPos+offset),numberColor, 2);
        xPos+=40;
        if(xPos>(img.cols-img.cols/3.2)){
            yPos+=40;
            xPos=10;
        }
    }
}
