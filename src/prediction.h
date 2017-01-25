#ifndef PREDICTION_H
#define PREDICTION_H 

#include <opencv/cv.h>
#include "camera.h"

namespace cv {
	typedef Point3_<double> Point3;
};

class Face3D {
	cv::Point3 faceCenter;
	float faceSize;
public:
	static const int NO_FACE = -1;
	Face3D();
	Face3D(float x, float y, float z, float size);
	Face3D(cv::Point3 center, float size);

	bool operator !=(int);
	bool operator ==(int);

	float size();
	Face3D& size(float size);

	cv::Point3 center();
	Face3D& center(cv::Point3);
	Face3D& center(float x, float y, float z);
	double euclidDistanceSquared(const Face3D &otherFace);
private:
};

class Prediction {
	Camera *camera;
	cv::Mat cameraViewTransform;
	cv::Mat cameraWorldTransform;
	double heightM, widthM, focalLength, pixelLength;
public:
	Prediction(Camera &cam);
	cv::Mat generateProjectionMatrix(Camera *cam = 0);
	cv::Point3 unprojectCameraSpaceCoordinate(const cv::Point &, double distance=1);
	Prediction &setCameraProjectionMatrix(const cv::Mat &);
	Prediction &setWorldMatrix(const cv::Mat &);
	cv::Point3 getWorldSpaceCoordinate(cv::Point3);
};


void prediction_test(int argc, char **argv);
void prediction_measure(int argc, char **argv);
void prediction_performance(int argc, char **argv);


#endif
