#ifndef DETECTION_H
#define DETECTION_H 
#include <opencv/cv.h>
#define FACE_SAMPLE_URL "./face_detection/haarcascades/haarcascade_frontalface_alt2.xml"

class Face2D {
	cv::Point2d faceCenter;
	float faceSize;
public:
	static const int NO_FACE = -1;
	Face2D();
	Face2D(float x, float y, float size);
	Face2D(cv::Point2d center, float size);

	bool operator !=(int);
	bool operator ==(int);

	float size();
	Face2D& size(float size);

	cv::Point2d center();
	Face2D& center(cv::Point2d);
	Face2D& center(float x, float y);
	double euclidDistanceSquared(const Face2D &otherFace);
private:
};

class Detection {
	cv::CascadeClassifier face_cascade;
	Face2D previousFace;
public:
	Detection(const cv::CascadeClassifier &face);
	Detection(std::string face_path);
	Face2D detectFace(cv::Mat image);
	bool hasDetectedFace();
	Face2D createFace(cv::Rect faceRect, float xmax = 1, float ymax = 1);
	Face2D createFace(cv::Point2d center, float size);
private:
};
void detection_test(int argc, char **argv);
void detection_measure(int argc, char **argv);

#endif
