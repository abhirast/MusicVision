#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <algorithm>
#include <deque>
#include "music.h"

using namespace cv;
using namespace std;


Detector::Detector(InstrumentModel &imodel, VideoCapture &cp) : buff_size(11)
{
    this->imodel = &imodel;
    this->cp = &cp;
    duration = 0;
}

bool Detector::init() {
    Mat image, gr_image;
    cp->read(image);
    cvtColor(image, gr_image, COLOR_BGR2GRAY);
    vector<Point2f> calib_markings;
    find_calib_locs(gr_image, calib_markings);
    homog = findHomography(calib_markings, imodel->calib_points, CV_RANSAC);
 	for (int i = 0; i < buff_size; i++) {
 		buffer.push_back(0.0);
 	}
    return false;
}
        
bool Detector::next(MusicParams &params) {
    bool found = false;
    Mat image;
    if (!cp->read(image)) return false;
    Point2f loc = findPen(image);
    buffer.pop_front();
    buffer.push_back(loc.y);
    // find if the center is a minima
    bool isMax =  isLocalMax(); 
    if (!isMax) {
    	if (duration <= 0) {
    		params.frequency = -1;
    		return true;
    	} else {
    		params = previous;
    		duration--;
    		return true;
    	}
    }
    // found a local minima
    duration = 10;
    // find the location
    vector<Point2f> predicted;
    vector<Point2f> input;
    input.push_back(loc);
    perspectiveTransform(input, predicted, homog);
    cout << predicted[0] << endl;
    // set the MusicParams by reading from imodel depending on position
    params.frequency = 200;
    params.intensity = 1.0;
    previous = params;
    return true;
}

void Detector::close() {
}

void Detector::find_calib_locs(Mat &image, vector<Point2f> &locs) {
	// perform graythresh
    Mat bin_image;
    threshold (image, bin_image, 0, 255, THRESH_BINARY | THRESH_OTSU);
    // find the largest connected component
    vector<vector<Point> > blobs;
    find_connected_components(bin_image, blobs);
    vector<int> sizes(blobs.size());
    int index = -1, mx = -1;
    for (int i = 0; i < blobs.size(); i++) {
        if ((int) blobs[i].size() > mx) {
            index = i;
            mx = blobs[i].size();
        }
    }
    Mat page = Mat::zeros(bin_image.rows, bin_image.cols, CV_8U);
    for (int i = 0; i < blobs[index].size(); i++) {
        page.at<uchar>(blobs[index][i]) = 255;
    }
    // find the small connected components in the negative of the page
    page = 255 - page;
    // Create a structuring element
    int erosion_size = 2;  
    Mat res;
    Mat element = getStructuringElement(MORPH_CROSS,
          Size(2 * erosion_size + 1, 2 * erosion_size + 1),
          Point(0, 0) );
 
    // Apply erosion or dilation on the image
    erode(page, res, element);
    imshow("debug", res);
    waitKey(0);
    // find_connected_components(page, blobs);
    find_connected_components(res, blobs);
    
    locs.clear();
    for (int i = 0; i < blobs.size(); i++) {
        if (blobs[i].size() > 15 && blobs[i].size() < 1000) {
            locs.push_back(find_mean(blobs[i]));
        }
    }

    // for (int i = 0; i  < locs.size(); i++) {
    //     cout << locs[i] << endl;
    // }
    // Mat x = Mat::zeros(image.rows, image.cols, CV_8U);
    // for (int i = 0; i < locs.size(); i++) {
    //     x.at<uchar>(locs[i]) = 255;
    // }
    // Mat y;
    // dilate(x, y, element);
    // namedWindow( "Display window", 0);
    // imshow("Display window", image);
    // namedWindow( "Display window2", 0);
    // imshow("Display window2", y);
    // waitKey(0);
}

/////////////////////////////////////////////
//		Private methods
/////////////////////////////////////////////


void Detector::find_connected_components(Mat &binary, 
                        vector<vector<Point> > &blobs) {
	blobs.clear();
    Mat labels;
    binary.convertTo(labels, CV_32FC1);
    labels /= 255;
    int label_count = 2; // starts at 2 because 0,1 are used already
    
    for(int y=0; y < binary.rows; y++) {
        for(int x=0; x < binary.cols; x++) {
            if(labels.at<float>(y,x) != 1) {
                continue;
            }
 
            Rect rect;
            floodFill(labels, Point(x,y), Scalar(label_count), 
                                    &rect, Scalar(0), Scalar(0), 4);
 
            vector<Point>  blob;
 
            for(int i=rect.y; i < (rect.y+rect.height); i++) {
                for(int j=rect.x; j < (rect.x+rect.width); j++) {
                    if(labels.at<float>(i, j) != label_count) {
                        continue;
                    }
                    blob.push_back(Point(j,i));
                }
            }
            blobs.push_back(blob);
            label_count++;
        }
    }
}

Point Detector::find_mean(vector<Point> blob) {
	int a = 0, b = 0;
    for (int i = 0; i < blob.size(); i++) {
        a += blob[i].x;
        b += blob[i].y;
    }
    return Point2f(floor(1.0*a/blob.size()), floor(1.0*b/blob.size()));
}


Point2f Detector::findPen(Mat &input_image) {
	
	int iLowH = 44;
	int iHighH = 74;

	int iLowS = 72;
	int iHighS = 152;

	int iLowV = 41;
	int iHighV = 91;
	
	Mat hsv_tx_image, color_thresh_image,blurred_image,opened_image, dila_open;

	int width = input_image.cols;
	int height = input_image.rows;
	Size frameSize(static_cast<int>(width),static_cast<int>(height));
	
	int morph_size=3;
	int morph_dilate_size=5;
	Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size( 2*morph_size + 1, 
						2*morph_size+1 ), Point( morph_size, morph_size ) );
	Mat dilate_kernel=getStructuringElement(MORPH_ELLIPSE, 
				Size( 2*morph_dilate_size+ 1, 2*morph_dilate_size+1 ), 
				Point( morph_dilate_size, morph_dilate_size) );
	
	cvtColor(input_image,hsv_tx_image,COLOR_BGR2HSV);
	inRange(hsv_tx_image,Scalar(iLowH, iLowS, iLowV),Scalar(iHighH, iHighS, iHighV),color_thresh_image);
	GaussianBlur(color_thresh_image,blurred_image,Size(3,3),5,5,BORDER_DEFAULT);
	morphologyEx(blurred_image,opened_image,MORPH_OPEN, kernel);
	dilate(opened_image,dila_open,dilate_kernel);
	
    
    Mat bin_image;
    dila_open.convertTo(bin_image, CV_8UC1);
    imshow("myVideo",bin_image);
    int xpos = 0, ypos = 0, count = 0;
    bool found = false;
    for (int i = 0; i < bin_image.rows; i++) {
    	for (int j = 0; j < bin_image.cols; j++) {
    		if (bin_image.at<uchar>(i, j) > 0) {
    			xpos += i;
    			ypos = max(ypos, j);
    			count++;
    			found = true;
    		}
    	} 
    }
    if (!found) return Point2f(-1, -1);
    // cout << xpos/count << " " << ypos << " " << count <<endl;
    return Point2f(1.0*xpos/count, ypos);
}

bool Detector::isLocalMax() {
	int count = 0;
	for (int i = 0; i < buff_size; i++) {
		if (buffer[i] > buffer[buff_size/2]) count++;
	}
	return count < 3;
}