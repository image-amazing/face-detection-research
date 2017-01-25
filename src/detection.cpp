#include "camera.h"
#include "detection.h"
#include <vector>


Face2D::Face2D():faceCenter(NO_FACE, NO_FACE), faceSize(NO_FACE) {}
Face2D::Face2D(cv::Point2d center, float size):faceCenter(center), faceSize(size) {}
Face2D::Face2D(float x, float y, float size):faceCenter(x, y), faceSize(size) {}

float Face2D::size() {
	return this->faceSize;
}
Face2D& Face2D::size(float size) {
	this->faceSize = size;
	return *this;
}

cv::Point2d Face2D::center() {
	return this->faceCenter;
}
Face2D& Face2D::center(cv::Point2d center) {
	this->faceCenter = center;
	return *this;
}
Face2D& Face2D::center(float x, float y) {
	this->faceCenter = cv::Point2d(x, y);
	return *this;
}

bool Face2D::operator !=(int NO_FACE) {
	return !(*this == NO_FACE);
}
bool Face2D::operator ==(int NO_FACE) {
	return this->faceSize == NO_FACE && this->faceCenter.x == NO_FACE && this->faceCenter.y == NO_FACE;
}

double Face2D::euclidDistanceSquared(const Face2D &otherFace) {
	double dx = faceCenter.x - otherFace.faceCenter.x;
	double dy = faceCenter.y - otherFace.faceCenter.y;
	return dx * dx + dy * dy;
}

Detection::Detection(const cv::CascadeClassifier &face):face_cascade(face) {}
Detection::Detection(std::string face_path):face_cascade(face_path) {}
Face2D Detection::detectFace(cv::Mat image) {
	std::vector<cv::Rect> faces;
	const float athird = 1.0f / 3.0f;
	face_cascade.detectMultiScale( image, faces, 1.2, 3, 0|CV_HAAR_SCALE_IMAGE, cv::Size(2, 2), cv::Size(athird * image.cols, athird * image.rows) );
	// face_cascade.detectMultiScale( image, faces, 1.4, 3, 0|CV_HAAR_SCALE_IMAGE, cv::Size(2, 2), cv::Size(150, 150) );

	double minDist = -1;
	Face2D minFace;
	for(auto face : faces) {
		Face2D f2d = createFace(face);//, image.cols, image.rows);
		double dist = f2d.euclidDistanceSquared(previousFace);
		if(minDist < 0 || minDist > dist) {
			minDist = dist;
			minFace = f2d;
		}
	}
	if(minDist >= 0) {
		previousFace = minFace;
	}

	return previousFace;
}
bool Detection::hasDetectedFace() {
	return previousFace != Face2D::NO_FACE;
}

Face2D Detection::createFace(cv::Rect faceRect, float xmax, float ymax) {
	cv::Point2d center( faceRect.x + faceRect.width*0.5, faceRect.y + faceRect.height*0.5 );
	float size = (faceRect.width * 0.5 + faceRect.height * 0.5) / 2;
	center.x = center.x * 1.0 / xmax - 0.5;
	center.y = center.y * 1.0 / ymax - 0.5;
	return createFace(center, size);
}
Face2D Detection::createFace(cv::Point2d center, float size) {
	return Face2D(center, size);
}
void detection_test(int argc, char **argv) {
	Camera cam(argc, argv);
	cv::CascadeClassifier face_cascade;
	face_cascade.load(FACE_SAMPLE_URL);
	Detection detection(face_cascade);
	cv::namedWindow("Detected Face",1);
	for(auto frame : cam) {
		Face2D faceDetected = detection.detectFace(frame);
		
		// If face found - Draw circles on the detected face
		if(faceDetected != Face2D::NO_FACE) 
			cv::ellipse( frame, faceDetected.center(), cv::Size( faceDetected.size(), faceDetected.size() ), 0, 0, 360, cv::Scalar( 255, 0, 255 ), 4, 8, 0 );
		imshow( "Detected Face", frame );
        if(cv::waitKey(30) >= 0) break;
	}
}


void detection_measure(int argc, char **argv) {
	int key;
	cv::String prefix;
	if(argc > 1) {
		prefix = argv[1];
	}
	Camera cam(argc, argv);
	cv::CascadeClassifier face_cascade;
	face_cascade.load(FACE_SAMPLE_URL);
	Detection detection(face_cascade);
	cv::namedWindow("Detected Face",1);
	for(auto frame : cam) {
		Face2D faceDetected = detection.detectFace(frame);
		
		if((key = cv::waitKey(30)) >= 0) {
			if(key == 'a' && faceDetected != Face2D::NO_FACE) {
				cv::Point fpos = faceDetected.center();
				std::string path = cv::format("%sface_%dx%d.jpeg", prefix.c_str(), fpos.x, fpos.y);
				std::cout << fpos.x << ", " << fpos.y << ", size=" << faceDetected.size() << " Saved to: " << path << "\n";
				imwrite(path, frame);
			}
			else break;
		}
		if(faceDetected != Face2D::NO_FACE) 
			cv::ellipse( frame, faceDetected.center(), cv::Size( faceDetected.size(), faceDetected.size() ), 0, 0, 360, cv::Scalar( 255, 0, 255 ), 4, 8, 0 );
		// If face found - Draw circles on the detected face
		imshow( "Detected Face", frame );
	}
}
