#ifndef CAMERA_H
#define CAMERA_H 

#include <raspicam/raspicam_cv.h>
#include <string>
#include <vector>



class Camera {
	typedef raspicam::RaspiCam_Cv PiCamera;
	struct Iterator {
		cv::Mat frame;
		PiCamera *cam;
		Iterator();
		Iterator(PiCamera &cam);
		cv::Mat operator*();
		bool operator!=(Iterator &);
		bool operator==(Camera::Iterator &);
		Iterator& operator++();
	};
	PiCamera piCamera;
public:
	Camera();
	Camera(int argc, char **argv);
	~Camera();
	Iterator begin();
	Iterator end();
	cv::Mat captureOne();
	void processCameraParams(int argc, char **argv);
	void setCameraParam(int param, int value);
	int width();
	int height();
private:
	friend void camera_test(int, char **);
	float getParamVal(std::string param, int argc, char **argv, float defvalue=-1) const;
};

void camera_test(int, char **);
#endif
