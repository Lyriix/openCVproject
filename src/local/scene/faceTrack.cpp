#include "faceTrack.hpp"


faceTrack::faceTrack()
{
 //Initialisation du classificateur
    cascade = ( CvHaarClassifierCascade*)cvLoad("haarcascade_frontalface_alt.xml",0,0,0);
    storage = cvCreateMemStorage(0);

    //zcvReleaseHaarClassifierCascade(&cascade);
    //cvReleaseMemStorage(&storage);
}
/*
IplImage*faceTrack::detectFaces(IplImage *img_src)
{

    int i;


    CvSeq *faces = cvHaarDetectObjects(
                img_src,
                cascade,
                storage,
                1.1,
                3,
                0,
                cvSize(40,40));

    for(i=0;i<(faces ?faces->total : 0); i++)
    {
        CvRect *r = (CvRect*) cvGetSeqElem(faces,i);
        cvRectangle(img_src,cvPoint(r->x,r->y),cvPoint(r->x + r->width,r->y + r->height),CV_RGB(255,0,0),1,8,0);
        img_dest = cvCreateImage(cvSize(r->width,r->height),img_src->depth,img_src->nChannels);  //créer l'image à la taille du rectangle
        cvSetImageROI(img_src,*r);//définit la région d'interet de l'image source autoue du rectangle
        cvCopy(img_src,img_dest,0); //copier le rectangle sélectionné de img_src dans img_dest
        cvResetImageROI(img_src);

    }




     return img_dest;
}
*/
