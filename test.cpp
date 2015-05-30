#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <algorithm>

using namespace cv;
using namespace std;

void find_connected_components(Mat &binary, vector<vector<Point> > &blobs);

int main() {
    Mat image;
    image = imread("data/template1.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    cout << image.rows << " " << image.cols << endl;
    
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
    
    namedWindow( "Display window", 0);
    imshow("Display window", page);
    waitKey(0);
    return 0;
}

void find_connected_components(Mat &binary, vector<vector<Point> > &blobs) {
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
                    blob.push_back(cv::Point(j,i));
                }
            }
            blobs.push_back(blob);
            label_count++;
        }
    }
}

