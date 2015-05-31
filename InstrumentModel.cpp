#include <opencv2/core/core.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "music.h"


using namespace std;
using namespace cv;

InstrumentModel::InstrumentModel(string fileName) {
	string line;
	ifstream templ(fileName.c_str());
	if (!templ.is_open()) {
		cout << "Unable to open template\n";
	}
	else {
		getline(templ, line);
		rows = atoi(line.c_str());
		getline(templ, line);
		cols = atoi(line.c_str());
		vector<int> row(cols);
		int val;
		for (int i = 0; i < rows; i++) {
			row.clear();
			getline(templ, line);
			stringstream ss(line);
			for (int j = 0; j < cols; j++) {
				ss >> val;
				row.push_back(val);
				if (val == 1) {
					calib_points.push_back(Point2f(i, j));
				}
			}
			values.push_back(row);
		}
		templ.close();
	}
}

void InstrumentModel::toImage(Mat &m) {
	m = Mat::zeros(rows, cols, CV_8UC1);
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			if (values[i][j] > 0) {
				m.at<uchar>(i, j) = 255;
			}
		}
	}
}

double InstrumentModel::getFreq(int x, int y) {
    return 0.0;
}

double InstrumentModel::getIntensity(int x, int y) {
    return 0.0;
}
