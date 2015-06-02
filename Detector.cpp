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

const int BUFFER_SIZE = 5;

Detector::Detector(InstrumentModel &imodel, VideoCapture &cp) 
: buff_size(BUFFER_SIZE) {
    this->imodel = &imodel;
    this->cp = &cp;
    duration = 0;
}

bool Detector::init() {
    Mat image, gr_image;
    do {
        cp->read(image);
        cvtColor(image, gr_image, COLOR_BGR2GRAY);
        find_calib_locs(gr_image, calib_markings);
        if (showCalib) {
            // show the detected points
            for (int i = 0; i < calib_markings.size(); i++) {
     	  	   Point x((int) calib_markings[i].x, (int) calib_markings[i].y);
                rectangle(image, x - Point(5, 5), x + Point(5, 5), 
            	   Scalar( 0, 255, 255 ), -1, 8 );
     	    }
     	    imshow("detected points", image);
            waitKey(0);
        }
    } while (calib_markings.size() != imodel->calib_points.size());
    // sort the calib markings
    std::sort(calib_markings.begin(), calib_markings.end(), 
        [](const cv::Point2f &a, const cv::Point2f &b){ return a.y > b.y; });
    std::sort(calib_markings.begin(), calib_markings.begin() + 14, 
        [](const cv::Point2f &a, const cv::Point2f &b){ return a.x > b.x; });
    std::sort(calib_markings.begin() + 14, calib_markings.begin() + 16, 
        [](const cv::Point2f &a, const cv::Point2f &b){ return a.x > b.x; });
    std::sort(calib_markings.begin() + 16, calib_markings.begin() + 18, 
        [](const cv::Point2f &a, const cv::Point2f &b){ return a.x > b.x; });
    std::sort(calib_markings.begin() + 18, calib_markings.end(), 
        [](const cv::Point2f &a, const cv::Point2f &b){ return a.x > b.x; });
    
    homog = findHomography(calib_markings, imodel->calib_points, CV_RANSAC);
    if (showCalib) {
        // show the reconstructed points according to homography
        vector<Point2f> predicted;
        perspectiveTransform(calib_markings, predicted, homog);
        Mat tempIm;
        imodel->toImage(tempIm);
        for (int i = 0; i < predicted.size(); i++) {
            // cout << calib_markings[i] << endl;
            Point x(round(predicted[i].x), round(predicted[i].y));
            tempIm.at<uchar>(x.x, x.y) = 128;
        }
        namedWindow("reconstructed points", 0);
        imshow("reconstructed points", tempIm);
    }
 	for (int i = 0; i < buff_size; i++) {
 		buffer.push_back(Point2f(20.0, 20.0));
 	}
    return false;
}
        
bool Detector::next(MusicParams &params) {
    bool found = false;
    Mat image;
    if (!cp->read(image)) return false;
    
    Point2f loc = findPen(image);
    addToBuffer(loc);
    Point2f newloc = buffer[buffer.size()/2];
    // cout << newloc << endl;
    if (showTracking) {
    	namedWindow("temp", 0);
    	Point x((int) newloc.x, (int) newloc.y);
        rectangle(image, x - Point(5, 5), x + Point(5, 5), 
        			Scalar( 0, 255, 255 ), -1, 8);
        imshow("temp", image);
    }
    
    // find if the center is a maxima
    if (isLocalMax()) {
    	// found a local maxima indicating hit
    	duration = 0;
    	// find the location
    	vector<Point2f> predicted;
    	vector<Point2f> input;
    	input.push_back(newloc);
    	perspectiveTransform(input, predicted, homog);
    	int predx = round(predicted[0].x);
    	int predy = round(predicted[0].y);
    	if (showTracking) {
    		Mat tempIm;
    		if (predx >= 0 && predy >= 0) {
                imodel->toImage(tempIm);
    			tempIm.at<uchar>(predx, predy) = 128;
    			namedWindow("board", 0);
    			imshow("board", tempIm);
    			//waitKey(5000);
    		} else {
    			// cout << "Bad point" << endl;
    		}
    	}
        if (doScoring && predx >= 0 && predy >= 0) {
            cout << predicted[0].x << " " << predicted[0].y << endl;
        }
    	// set the MusicParams by reading from imodel depending on position
    	params.note = imodel->getNote(predx, predy);
    	params.intensity = imodel->getIntensity(predx, predy);
    	previous = params;
    } 
    else if (duration <= 0) {
    	params.note = -1;
    	params.intensity = -1;
    } 
    else {
    	params = previous;
    	duration--;
    }
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
	int iLowH = 39, iHighH =  80;
	int iLowS = 57, iHighS = 157;
	int iLowV =  0, iHighV = 255;
	
	Mat hsv_tx_image, color_thresh_image, bin_image;

	cvtColor(input_image, hsv_tx_image, COLOR_BGR2HSV);
	inRange(hsv_tx_image, Scalar(iLowH, iLowS, iLowV), 
				Scalar(iHighH, iHighS, iHighV), color_thresh_image);
    
    color_thresh_image.convertTo(bin_image, CV_8UC1);
    vector<vector<Point> > blobs;
    find_connected_components(bin_image, blobs);

    // look for connected component of size greater than 150
    int index = -1;
    for (int i = 0; i < blobs.size(); i++) {
    	if (blobs[i].size() > 150) index = i;
    }
    Point2f pt;
    if (index < 0){
    	pt.x = -1;
    	pt.y = -1;	
    } else {
    	int xpos = 0, ypos = 0;
    	for (int i = 0; i < blobs[index].size(); i++) {
    		xpos += blobs[index][i].x;
    		ypos = max(ypos, blobs[index][i].y);
   	 	}
   		pt.x = 1.0*xpos/blobs[index].size();
   		pt.y =  ypos;
    }
    // cout << pt.x << " " << pt.y << endl;
    return pt;
}

bool Detector::isLocalMax() {
	int count = 0;
    int mid = buffer.size()/2;
	for (int i = 0; i < mid; i++) {
		if (buffer[i].y >= buffer[mid].y) count++;
	}
    for (int i = mid; i < buffer.size(); i++) {
        if (buffer[i].y > buffer[mid].y) count++;   
    }
    // if (count == 0) {
        // cout << endl << endl;
        // for (int i = 0; i < buffer.size(); i++) cout << buffer[i] << "\t";
        // cout << endl << endl;
    // }
	return count < 1;
}

void Detector::addToBuffer(Point2f position) {
	buffer.pop_front();
	if (position.x < 0) {
		// do smoothing based on previous values
		Point2f pos = 2 * buffer[buffer.size() - 1]  - buffer[buffer.size() - 2];
		buffer.push_back(pos);
	} else {
		buffer.push_back(position);
	}
}