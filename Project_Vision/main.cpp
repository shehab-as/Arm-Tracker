////
////  main.cpp
////  Project_Vision
////
////  Created by Shehab Mohamed on 12/5/16.
////  Copyright © 2016 Shehab Mohamed. All rights reserved.
////
#include <iostream>
#include <string>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/video/video.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace std;
using namespace cv;

void RGB_Range(Mat&);
void HSV_Range(Mat&);
void Approx_White_Pixels(Mat&);
bool WHITEBOY(Mat& img, int x, int y);

void detectAndDisplay(Mat frame);


string loulou_cascade = "/Users/shehabmohamed/Desktop/C++ Projects/Project_Vision/build/Debug/haarcascade_frontalface_alt.xml";
CascadeClassifier Face;
int main()
{
    VideoCapture cap(0);
    if( !Face.load( loulou_cascade ) ){ printf("--(!)Error loading\n"); return -1; };
    if(!cap.isOpened())
        cout<<"Webcam is not opened."<<endl;
    
    // 640x480
    cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
    
    BackgroundSubtractorMOG2 MOG;
    
    Mat firstM;
    Mat frame, frame_HSV, frame_YCRCB, frame_RGB;
    Mat Skin_Mask_YC, Skin_Mask_RGB;

    while(true)
    {
        cap.read(frame);
        
        //Loulou's waste of time.
        
//        if (background_frame>0)
//        {
//            if(firstM.empty())
//            {
//                cap.read(firstM);
//                cvtColor(firstM, firstM, CV_RGB2YCrCb);
//            }
//            background_frame--;
//            continue;
//        }
//        else
//            cap.read(frame);
//            cvtColor(frame, frame, CV_RGB2YCrCb);
//        
//        vector<Mat> bg, fg;
//        split(firstM, bg);
//        split(frame, fg);
//        Mat distance;
//        distance.create(frame.rows, frame.cols, CV_32F);
//        distance = Scalar(0,0,0);
//        for( int i =0; i< fg.size() ; i++)
//        {
//            Mat temp = abs(fg[i] - bg[i]);
//            temp.convertTo(temp, CV_32F);
//            
//            distance += temp;
//        }
//        Mat mask;
//        threshold(distance, mask, 35, 255, THRESH_BINARY);
//        
//        Mat kernel5x5 = getStructuringElement(MORPH_RECT, Size(5,5));
//        morphologyEx(mask, mask, MORPH_OPEN, kernel5x5);

        
        //Loulou's paper
//        auto lower_YCRCB = Scalar(0, 150, 100);
//        auto upper_YCRCB = Scalar(255, 200, 150);
        
        //From Gesture example (Perfect so far).
        auto lower_YCRCB = Scalar(60, 140, 90);
        auto upper_YCRCB = Scalar(250, 210, 130);
        
        auto lower_RGB = Scalar();
        auto upper_RGB = Scalar();
        
        //Not correct segmentation.
//        auto lower_YCRCB = Scalar(0, 133, 77);
//        auto upper_YCRCB = Scalar(255, 173, 127);
        
        //My guess
//        auto lower_YCRCB = Scalar(0, 140, 80);
//        auto upper_YCRCB = Scalar(255, 180, 130);
////////////////////////////////////////////////////////////////////
        cvtColor(frame, frame_YCRCB, CV_BGR2YCrCb);
        cvtColor(frame, frame_RGB, CV_BGR2RGB);

        vector<Mat> Contours;
        vector<Vec4i> Hierarchy;
        //Mat Contours;
        
        
        inRange(frame_YCRCB, lower_YCRCB, upper_YCRCB, Skin_Mask_YC);
        inRange(frame_RGB, lower_RGB, upper_RGB, Skin_Mask_RGB);
        
        medianBlur(Skin_Mask_YC, Skin_Mask_YC, 5);
        Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2 * 5 + 1, 2 * 5 + 1), Point(5, 5));
        dilate(Skin_Mask_YC, Skin_Mask_YC, element);
        //medianBlur(Skin_Mask_RGB, Skin_Mask_RGB, 5);
        //RGB_Range(frame_RGB);
        
        //Finding Contours Code Block for YCRCB
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        findContours(Skin_Mask_YC, Contours, Hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
        vector<Mat> Better_Contours;
        
        for(int i=0; i<Contours.size(); i++)
        {
            auto area = contourArea(Contours[i]);
            if(area > 1200 && area < 15000)
            {
//                auto ratio = Contours[i].rows/area;
//                cout<<"Ratio: "<<ratio<<endl;

                Better_Contours.push_back(Contours[i]);
                //putText(frame,  to_string(i) + "   ratio " + to_string( Contours[i].rows/ area), Point(frame.cols/2, frame.rows/2), FONT_HERSHEY_COMPLEX_SMALL, 10, Scalar(255,0,0), 1, CV_AA );
                
                drawContours(frame, Contours, i, Scalar(0,255,0), 3);
            }
        }
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

        vector<vector<Point>> hull(Better_Contours.size());
        
        for(int i=0; i<Better_Contours.size(); i++)
            convexHull(Mat(Better_Contours[i]), hull[i], false);

        
        for(int i=0; i<Better_Contours.size(); i++)
        {
            auto area2 = contourArea(hull[i]);
            auto ratio = Contours[i].rows/area2;
            cout<<"Ratio: "<<ratio<<endl;
            //if(ratio <= 0.001)
            drawContours( frame, hull, i, Scalar(0,0,255), 3, 8, vector<Vec4i>() );

        }
        
        
        //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        imshow("Frame", frame);
        //imshow("YCRCB", frame_YCRCB);
        imshow("Skin Mask (YCRCB)", Skin_Mask_YC);
        imshow("Skin Mask (RGB)", Skin_Mask_RGB);
        //imshow("Skin Mask (RGB)", Skin_Mask_RGB);
    }
    //////////////////////////////////////////
    
    return 0;
}

void RGB_Range(Mat& frame)
{
    for(int i=0; i<frame.rows; i++)
    {
        for(int j=0; j<frame.cols; j++)
        {
            int R = frame.at<Vec3b>(i, j)[0];
            int G = frame.at<Vec3b>(i, j)[1];
            int B = frame.at<Vec3b>(i, j)[2];

            bool e1 = (R>95) && (G>40) && (B>20) && ((max(R,max(G,B)) - min(R, min(G,B)))>15) && (abs(R-G)>15) && (R>G) && (R>B);
            bool e2 = (R>220) && (G>210) && (B>170) && (abs(R-G)<=15) && (R>B) && (G>B);
            
            if(e1 || e2)
            {
                frame.at<Vec3b>(i, j)[0] = 255;
                frame.at<Vec3b>(i, j)[1] = 255;
                frame.at<Vec3b>(i, j)[2] = 255;
            }
        }
    }
}

bool WHITEBOY(Mat& img, int x, int y)
{
    int white_pixels_count = 0;
    for(int i=x-3; i<=x+3; i++)
    {
        for(int j=y-3; y<=y+3; y++)
        {
            if(img.at<uchar>(i, j) == 255)
                white_pixels_count++;
        }
    }
    if(white_pixels_count>=25)
        return true;
    
    return false;
}

void Approx_White_Pixels(Mat& img)
{
    for(int i=0; i<img.rows; i++)
    {
        for(int j=0; j<img.cols; j++)
        {
            if(WHITEBOY(img, i, j) == true)
                img.at<uchar>(i, j) = 255;
        }
    }
}

