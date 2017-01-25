#include <iostream>
#include <string>
#include "camera.h"


Camera::Iterator::Iterator(Camera::PiCamera &cam):cam(&cam) {}	
Camera::Iterator::Iterator():cam(0) {}		
cv::Mat Camera::Iterator::operator*() {
	return frame;
}
bool Camera::Iterator::operator==(Camera::Iterator &other) {
	return (this->cam == other.cam && (this->cam == 0 || this->cam->isOpened() == other.cam->isOpened()) );
}
bool Camera::Iterator::operator!=(Camera::Iterator &other) {
	return !this->operator==(other);//*other != frame;
}
Camera::Iterator& Camera::Iterator::operator++() {
	if(this->cam == 0 || !this->cam->isOpened())
		return *this;

	this->cam->grab();
	this->cam->retrieve(frame);
	return *this;
}

Camera::Camera():Camera(0, 0) {}
Camera::Camera(int argc, char **argv) {
	this->processCameraParams(argc, argv);
	if (!this->piCamera.open()) {
		std::cerr << "Error opening the camera" << std::endl;
		throw 1;
	}
	std::cout <<"Open camera...\n";
}
Camera::~Camera() {
	if(this->piCamera.isOpened())
		this->piCamera.release();
}


Camera::Iterator Camera::begin() {
	if(this->piCamera.isOpened())
		return ++Camera::Iterator(this->piCamera);
	return Camera::Iterator();
}
Camera::Iterator Camera::end() {
	return Camera::Iterator();
}
cv::Mat Camera::captureOne() {
	this->piCamera.grab();
	cv::Mat frame;
	this->piCamera.retrieve(frame);
	return frame;
}
void Camera::setCameraParam(int param, int value) {
	this->piCamera.set( param, value );
}

void Camera::processCameraParams(int argc, char **argv) {
	this->setCameraParam( CV_CAP_PROP_FRAME_WIDTH, 	getParamVal( "-w", argc, argv, 320 ) );
	this->setCameraParam( CV_CAP_PROP_FRAME_HEIGHT, 	getParamVal( "-h", argc, argv, 320 ) );

	this->setCameraParam( CV_CAP_PROP_CONVERT_RGB , 0 );

	this->setCameraParam( CV_CAP_PROP_BRIGHTNESS, 	getParamVal( "-br", argc, argv, 50 ) );
	this->setCameraParam( CV_CAP_PROP_CONTRAST , 		getParamVal( "-co", argc, argv, 50 ) );
	this->setCameraParam( CV_CAP_PROP_SATURATION, 	getParamVal( "-sa", argc, argv, 50 ) );
	this->setCameraParam( CV_CAP_PROP_GAIN, 			getParamVal( "-g", argc, argv, 50 ) );
	this->setCameraParam( CV_CAP_PROP_FPS, 			getParamVal( "-fps", argc, argv,  0 ) );
	this->setCameraParam( CV_CAP_PROP_FORMAT, CV_8UC1 );

}
int Camera::width() {
	return this->piCamera.get( CV_CAP_PROP_FRAME_WIDTH );
}
int Camera::height() {
	return this->piCamera.get( CV_CAP_PROP_FRAME_HEIGHT );
}

float Camera::getParamVal(std::string param, int argc, char **argv, float defvalue) const {
	int idx=-1;
	for(int i=0; i<argc && idx==-1; i++)
		if(std::string( argv[i] ) == param) idx=i;
	if(idx == -1) return defvalue;
	else return atof( argv[idx+1] );
}

void camera_test(int argc, char **argv) {

	Camera cam(argc, argv);
	cv::Mat edges;
	cv::namedWindow("edges",1);
	for(auto frame : cam) {
		cv::imshow("edges", frame);
		if(cv::waitKey(30) >= 0) break;
	}
}
