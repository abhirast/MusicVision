/*
 * imgpract.cpp
 *
 *  Created on: May 25, 2015
 *      Author: ameya
 */
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;




int main()
{
/*
    Mat image = imread("../malibu.jpeg",1);   // Read the file
    Mat image2,image3;
    std::vector<cv::Mat> pyramids;
    Mat struct_ele=getStructuringElement(MORPH_CROSS,Size(3,3));
    blur(image,image2, Size(5,5),Point(-1,-1),BORDER_REPLICATE);
    buildPyramid(image,pyramids,5);
    image3=pyramids[2];
    dilate(image,image3,struct_ele);
    erode(image,image3,struct_ele);
    Mat kernel=Mat::ones(5,5,CV_32FC1);
    kernel.at<double>(1,1)=0;
    kernel.at<double>(0,0)=0.51;
    kernel=kernel/(float)2;
    filter2D(image,image3,-1,kernel);
    medianBlur(image,image3,15);
    cvtColor(image,image3,CV_BGR2GRAY);
    kernel=Mat(5,5,CV_8U,cv::Scalar(1));

    morphologyEx(image3,image3,MORPH_CLOSE,kernel);
    VideoCapture cp(0);
    if(!cp.isOpened())
    {
    	return -1;
    }
    Mat edges;
    double propval;
    int a=50;
    int b;
    namedWindow("mywindow",1);
	createTrackbar("newtrack","mywindow",&a,100);
    for(;;)
    {

    	cp>>edges;
    	imshow("mywindow",edges);
    	//propval=cp.get(CV_CAP_PROP_BRIGHTNESS);
    	//printf("\n%d",a);
    	if(waitKey(30)>0)
    		break;
    }
    namedWindow( "Display window", WINDOW_AUTOSIZE );// Create a window for display.
    Mat prev=imread("../first.png",0);
    Mat prev_conv;
    prev.convertTo(prev_conv,8);
    Mat next=imread("../second.png",0);
    Mat next_conv;
    next.convertTo(next_conv,8);

    calcOpticalFlowFarneback(prev_conv,next_conv,image3, 0.5, 3, 15, 3, 5, 1.2, 0);


    imshow( "Display window", image3);                   // Show our image inside it.
*/
	VideoCapture cp2(0);

	 int iLowH = 44;
	 int iHighH = 74;

	  int iLowS = 72;
	 int iHighS = 152;

	  int iLowV = 41;
	 int iHighV = 91;
	 namedWindow("myVideo",CV_WINDOW_AUTOSIZE);
	  //Create trackbars in "Control" window
	 cvCreateTrackbar("LowH", "myVideo", &iLowH, 179); //Hue (0 - 179)
	 cvCreateTrackbar("HighH", "myVideo", &iHighH, 179);

	  cvCreateTrackbar("LowS", "myVideo", &iLowS, 255); //Saturation (0 - 255)
	 cvCreateTrackbar("HighS", "myVideo", &iHighS, 255);

	  cvCreateTrackbar("LowV", "myVideo", &iLowV, 255); //Value (0 - 255)
	 cvCreateTrackbar("HighV", "myVideo", &iHighV, 255);

	Mat input_image, hsv_tx_image, color_thresh_image,blurred_image,opened_image,single_channel_gray;

	int width=cp2.get(CV_CAP_PROP_FRAME_WIDTH);
	int height=cp2.get(CV_CAP_PROP_FRAME_HEIGHT);
	Size frameSize(static_cast<int>(width),static_cast<int>(height));
	VideoWriter vw("output.avi",CV_FOURCC('P','I','M','1'),30,frameSize,1);
	int morph_size=3;
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );

	for(;;)
	{

		cp2.read(input_image);
		vw.write(input_image);
		cvtColor(input_image,hsv_tx_image,COLOR_BGR2HSV);
		inRange(hsv_tx_image,Scalar(iLowH, iLowS, iLowV),Scalar(iHighH, iHighS, iHighV),color_thresh_image);
		GaussianBlur(color_thresh_image,blurred_image,Size(3,3),5,5,BORDER_DEFAULT);
		morphologyEx(blurred_image,opened_image,MORPH_OPEN, kernel);
		imshow("myVideo",opened_image);

		if(waitKey(5)>0)
		{
			break;
		}
	}
    waitKey(0);                                          // Wait for a keystroke in the window
    return 0;
}

