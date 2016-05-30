#include <iostream>


#include "handTrack.hpp"

handTrack::handTrack(){}

void handTrack::init_Trackbars(){
    for(int i=0;i<NSAMPLES;i++){
        c_lower[i][0]=12;
        c_upper[i][0]=7;
        c_lower[i][1]=30;
        c_upper[i][1]=40;
        c_lower[i][2]=80;
        c_upper[i][2]=80;
    }
    cv::createTrackbar("lower1",name_trackbar,&c_lower[0][0],255);
    cv::createTrackbar("lower2",name_trackbar,&c_lower[0][1],255);
    cv::createTrackbar("lower3",name_trackbar,&c_lower[0][2],255);
    cv::createTrackbar("upper1",name_trackbar,&c_upper[0][0],255);
    cv::createTrackbar("upper2",name_trackbar,&c_upper[0][1],255);
    cv::createTrackbar("upper3",name_trackbar,&c_upper[0][2],255);
    cv::waitKey(1);
    //cv::destroyWindow(name_trackbar);


}

void handTrack::initHandTrack(cv::VideoCapture capture){
    capture >> img;
    //cut_Mat();
    //cv::namedWindow("detectHand",CV_WINDOW_KEEPRATIO);
    //cv::namedWindow("waitForPalmCover",CV_WINDOW_KEEPRATIO);
    //cv::namedWindow("average",CV_WINDOW_KEEPRATIO);
    cv::namedWindow(name_trackbar,CV_WINDOW_KEEPRATIO);
    waitForPalmCover(capture);
}


void handTrack::waitForPalmCover(cv::VideoCapture capture){
    int square_len=20;
    cv::flip(img,img,1);

    roi.push_back(My_ROI(cv::Point(img.cols/3, img.rows/6),cv::Point(img.cols/3+square_len,img.rows/6+square_len),img));
    roi.push_back(My_ROI(cv::Point(img.cols/4, img.rows/2),cv::Point(img.cols/4+square_len,img.rows/2+square_len),img));
    roi.push_back(My_ROI(cv::Point(img.cols/3, img.rows/1.5),cv::Point(img.cols/3+square_len,img.rows/1.5+square_len),img));
    roi.push_back(My_ROI(cv::Point(img.cols/2, img.rows/2),cv::Point(img.cols/2+square_len,img.rows/2+square_len),img));
    roi.push_back(My_ROI(cv::Point(img.cols/2.5, img.rows/2.5),cv::Point(img.cols/2.5+square_len,img.rows/2.5+square_len),img));
    roi.push_back(My_ROI(cv::Point(img.cols/2, img.rows/1.5),cv::Point(img.cols/2+square_len,img.rows/1.5+square_len),img));
    roi.push_back(My_ROI(cv::Point(img.cols/2.5, img.rows/1.8),cv::Point(img.cols/2.5+square_len,img.rows/1.8+square_len),img));

    for(int i =0;i<50;i++){
        capture >> img;
        //cut_Mat();
        cv::flip(img,img,1);
        for(int j=0;j<NSAMPLES;j++){
            roi[j].draw_rectangle(img);
        }

        imshow("waitForPalmCover", img);
        if(cv::waitKey(60) >= 0)
        {
            cv::destroyWindow("waitForPalmCover");
            cv::waitKey(1);
            break;
        }
        //cv::waitKey(30);
    }
    //cv::waitKey(1);
    cv::destroyWindow("waitForPalmCover");
}

void handTrack::average(cv::VideoCapture capture){
    capture >> img;
    //cut_Mat();
    cv::flip(img,img,1);

    for(int i=0;i<30;i++){
        capture >> img;
        //cut_Mat();
        cv::flip(img,img,1);
        //Changement de couleurs de src
        cvtColor(img,img,ORIGCOL2COL);

        for(int j=0;j<NSAMPLES;j++){
            //Donne la difference de couleurs entre l'image totale et les carrés de peau de la main
            getAvgColor(roi[j],avgColor[j]);
            roi[j].draw_rectangle(img);
        }

        cvtColor(img,img,COL2ORIGCOL);
        cv::imshow("average", img);
        if(cv::waitKey(1) >= 0)
        {
            cv::destroyWindow("average");
            cv::waitKey(1);
            break;
        }
    }
//cv::waitKey(1);
cv::destroyWindow("average");
}

void handTrack::getAvgColor(My_ROI roi, int avg[3]){
    cv::Mat r;
    roi.roi_ptr.copyTo(r);
    std::vector<int>hm;
    std::vector<int>sm;
    std::vector<int>lm;
    // generate vectors
    for(int i=2; i<r.rows-2; i++){
        for(int j=2; j<r.cols-2; j++){
            hm.push_back(r.data[r.channels()*(r.cols*i + j) + 0]) ;
            sm.push_back(r.data[r.channels()*(r.cols*i + j) + 1]) ;
            lm.push_back(r.data[r.channels()*(r.cols*i + j) + 2]) ;
        }
    }
    avg[0]=getMedian(hm);
    avg[1]=getMedian(sm);
    avg[2]=getMedian(lm);
}

int handTrack::getMedian(std::vector<int> val){
    int median;
    size_t size = val.size();
    std::sort(val.begin(), val.end());
    if (size  % 2 == 0)  {
        median = val[size / 2 - 1] ;
    } else{
        median = val[size / 2];
    }
    return median;
}

void handTrack::Detect_Hand(cv::VideoCapture capture){
    hg.frameNumber ++;

    capture >> img;
    //cut_Mat();

    cv::flip(img,img,1);
    //Sous echantillonnage  :
    cv::pyrDown(img,img_LR);
    cv::blur(img_LR,img_LR,cv::Size(3,3));
    //Changement de couleurs :
    cv::cvtColor(img_LR,img_LR,ORIGCOL2COL);
    //Passage de l'image en binaire
     produceBinaries();
    cv::cvtColor(img_LR,img_LR,COL2ORIGCOL);
    makeContours();
    hg.getFingerNumber(img);
    //Affichage de l'image
    //std::cout<<display_images<<std::endl;
    if(display_images == true)
    {
        cv::imshow("detectHand",img);
        //cv::waitKey(1);
    }
    if(no_display_images == true)
    {
        cv::destroyWindow("detectHand");
        no_display_images = false;
        display_images = false;
    }


}

void handTrack::produceBinaries(){
    cv::Scalar lowerBound;
    cv::Scalar upperBound;

    for(int i=0;i<NSAMPLES;i++){
        normalizeColors();
        lowerBound=cv::Scalar( avgColor[i][0] - c_lower[i][0] , avgColor[i][1] - c_lower[i][1], avgColor[i][2] - c_lower[i][2] );
        upperBound=cv::Scalar( avgColor[i][0] + c_upper[i][0] , avgColor[i][1] + c_upper[i][1], avgColor[i][2] + c_upper[i][2] );
        bwList.push_back(cv::Mat(img_LR.rows,img_LR.cols,CV_8U));
        cv::inRange(img_LR,lowerBound,upperBound,bwList[i]);
    }
    bwList[0].copyTo(bw);

    for(int i=1;i<NSAMPLES;i++){
        bw+=bwList[i];
    }
    cv::medianBlur(bw, bw,7);

}

void handTrack::normalizeColors(){
    // copy all boundries read from trackbar
    // to all of the different boundries
    for(int i=1;i<NSAMPLES;i++){
        for(int j=0;j<3;j++){
            c_lower[i][j]=c_lower[0][j];
            c_upper[i][j]=c_upper[0][j];
        }
    }
    // normalize all boundries so that
    // threshold is whithin 0-255
    for(int i=0;i<NSAMPLES;i++){
        if((avgColor[i][0]-c_lower[i][0]) <0){
            c_lower[i][0] = avgColor[i][0] ;
        }if((avgColor[i][1]-c_lower[i][1]) <0){
            c_lower[i][1] = avgColor[i][1] ;
        }if((avgColor[i][2]-c_lower[i][2]) <0){
            c_lower[i][2] = avgColor[i][2] ;
        }if((avgColor[i][0]+c_upper[i][0]) >255){
            c_upper[i][0] = 255-avgColor[i][0] ;
        }if((avgColor[i][1]+c_upper[i][1]) >255){
            c_upper[i][1] = 255-avgColor[i][1] ;
        }if((avgColor[i][2]+c_upper[i][2]) >255){
            c_upper[i][2] = 255-avgColor[i][2] ;
        }
    }
}


void handTrack::makeContours(){
    cv::Mat aBw;
    pyrUp(bw,bw);
    bw.copyTo(aBw);

    //detecte les contours d'une image binaire
    cv::findContours(aBw,hg.contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
    hg.initVectors();
    hg.cIdx=findBiggestContour();

    if(hg.cIdx!=-1){
        //cv::approxPolyDP( Mat(hg.contours[hg.cIdx]), hg.contours[hg.cIdx], 11, true );
        hg.bRect=cv::boundingRect(cv::Mat(hg.contours[hg.cIdx]));
        //trouve l'enveloppe convexe des contours
        cv::convexHull(cv::Mat(hg.contours[hg.cIdx]),hg.hullP[hg.cIdx],false,true);
        cv::convexHull(cv::Mat(hg.contours[hg.cIdx]),hg.hullI[hg.cIdx],false,false);
        //Approximation d'un polygone à partir des contours
        cv::approxPolyDP(cv::Mat(hg.hullP[hg.cIdx]), hg.hullP[hg.cIdx], 18, true );
        if(hg.contours[hg.cIdx].size()>3 ){
            //On cherche les erreurs et on les elimine
            cv::convexityDefects(hg.contours[hg.cIdx],hg.hullI[hg.cIdx],hg.defects[hg.cIdx]);
            hg.eleminateDefects();
        }
        bool isHand=hg.detectIfHand();
        //Si une main a ete détectée, on calcul le nombre de doigts et on dessine
        if(isHand){
            hg.getFingerTips(img);
            hg.drawFingerTips(img);
            myDrawContours();


            //		}
        }
    }
}

int handTrack::findBiggestContour(){
    int indexOfBiggestContour = -1;
    int sizeOfBiggestContour = 0;
    for (int i = 0; i < hg.contours.size(); i++){
        if(hg.contours[i].size() > sizeOfBiggestContour){
            sizeOfBiggestContour = hg.contours[i].size();
            indexOfBiggestContour = i;
        }
    }
    return indexOfBiggestContour;
}

void handTrack::myDrawContours(){
    cv::drawContours(img,hg.hullP,hg.cIdx,cv::Scalar(200,0,0),2, 8, std::vector<cv::Vec4i>(), 0, cv::Point());

    cv::rectangle(img,hg.bRect.tl(),hg.bRect.br(),cv::Scalar(0,0,200));
    std::vector<cv::Vec4i>::iterator d=hg.defects[hg.cIdx].begin();
    std::vector<cv::Mat> channels;
    cv::Mat result;
    for(int i=0;i<3;i++)
        channels.push_back(bw);
    cv::merge(channels,result);
    cv::drawContours(result,hg.hullP,hg.cIdx,cv::Scalar(0,0,250),10, 8, std::vector<cv::Vec4i>(), 0, cv::Point());


    while( d!=hg.defects[hg.cIdx].end() ) {
        cv::Vec4i& v=(*d);
        int startidx=v[0]; cv::Point ptStart(hg.contours[hg.cIdx][startidx] );
        int endidx=v[1]; cv::Point ptEnd(hg.contours[hg.cIdx][endidx] );
        int faridx=v[2]; cv::Point ptFar(hg.contours[hg.cIdx][faridx] );
        float depth = v[3] / 256;cv::circle(result,ptFar,9, cv::Scalar(0,205,0), 5 );
        d++;

    }

}

void handTrack::cut_Mat(){
    //cv::Mat image;
    //img.copyTo(image);
    //img = img(cv::Range::all(), cv::Range(Col_split_begin, Col_split_end));
    //image(cv::Range::all(), cv::Range(Col_split_begin, Col_split_end)).copyTo(img);
    int width = img.cols/2;
    int heigth = img.rows;
    img = cv::Mat(img,cv::Rect(Col_split_begin,Col_split_end,width,heigth));
}


cv::Mat handTrack::getimg(){
    return img;
}

HandGesture handTrack::gethg(){
    return hg;
}

void handTrack::setname_img(std::string name){
    name_img = name;
}
void handTrack::setname_trackbar(std::string name){
    name_trackbar = name;
}

void handTrack::setDisplayImages(bool valid)
{
    if(valid == true)
    this->display_images =valid;
    if(valid == false)
        this->no_display_images = true;
}
