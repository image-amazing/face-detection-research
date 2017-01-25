#include "prediction.h"


#include <opencv/cv.h>
#include <glob.h>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>
// #include <cmath>
#include "detection.h"

#include <GL/glew.h>

#include "Timer.h"


Face3D::Face3D():faceCenter(NO_FACE,NO_FACE,NO_FACE), faceSize(NO_FACE) {}
Face3D::Face3D(float x, float y, float z, float size)
		:faceCenter(x, y, z), faceSize(size) {}
Face3D::Face3D(cv::Point3 center, float size):faceCenter(center), faceSize(size) {}

bool Face3D::operator !=(int NO_FACE) {
	return !(*this == NO_FACE);
}
bool Face3D::operator ==(int) {
	return this->faceSize == NO_FACE && this->faceCenter.x == NO_FACE && this->faceCenter.y == NO_FACE && this->faceCenter.z == NO_FACE;
}

float Face3D::size() {
	return this->faceSize;
}
Face3D& Face3D::size(float size) {
	this->faceSize = size;
	return *this;
}

cv::Point3 Face3D::center() {
	return faceCenter;
}
Face3D& Face3D::center(cv::Point3 newCenter) {
	faceCenter = newCenter;
	return *this;
}
Face3D& Face3D::center(float x, float y, float z) {
	faceCenter.x = x;
	faceCenter.y = y;
	faceCenter.z = z;
	return *this;
}
double Face3D::euclidDistanceSquared(const Face3D &otherFace) {
	double dx = faceCenter.x - otherFace.faceCenter.x;
	double dy = faceCenter.y - otherFace.faceCenter.y;
	double dz = faceCenter.z - otherFace.faceCenter.z;
	return dx * dx + dy * dy + dz * dz;
}


Prediction::Prediction(Camera &cam):camera(&cam) {}

cv::Mat Prediction::generateProjectionMatrix(Camera *cam) {
	if(cam == 0) cam = this->camera;
	if(cam == 0) return cv::Mat( 4, 4, CV_8UC1, cv::Scalar(23) );
	focalLength = 3.6 / 1000;
	pixelLength = 1.4 / 1000000; // 10 ^-6
	widthM = cam->width();
	heightM = cam->height();
	return cv::Mat(4, 4, CV_8UC1, cv::Scalar(23) );
}
cv::Point3 Prediction::unprojectCameraSpaceCoordinate(const cv::Point &coordinate, double distance) {
	double x = (coordinate.x - widthM  / 2) * (distance / focalLength);
	double y = (coordinate.y - heightM / 2) * (distance / focalLength);
	return cv::Point3(x, y, distance);
}
Prediction &Prediction::setCameraProjectionMatrix(const cv::Mat &proj) {
	cameraViewTransform = proj;
	return *this;
}
Prediction &Prediction::setWorldMatrix(const cv::Mat &world) {
	cameraWorldTransform = world;
	return *this;
}
cv::Point3 Prediction::getWorldSpaceCoordinate(cv::Point3 point) {
	return point;//cv::Point3(0,0,0);
}

void glutInitPred(int argc, char **argv) {
    // glutInit(&argc, argv);
    // glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
    gluLookAt(	0.0f, // Eye vector
			 	0.0f,
			 	1.0f,
			 	0.0f, // center vector
			 	0.0f,
			 	0.0f,
			 	0.0f, // Up vector
			 	1.0f,
			 	0.0f);
}

float xKomp = 1, yKomp = 1, zKomp = 1;
Face3D predict3DCoordinate(Face2D face, Prediction prediction) {
	// Unproject faceDetected.center() into camMat furstum: face3DCoord = Face3D(x, y, 0)
	cv::Point3 predictionCoordinate = prediction.unprojectCameraSpaceCoordinate(face.center(), face.size());

	// GLint viewport[4]; //var to hold the viewport info
	// GLdouble modelview[16]; //var to hold the modelview info
	// GLdouble projection[16]; //var to hold the projection matrix info
	// GLfloat winX, winY, winZ; //variables to hold screen x,y,z coordinates
	// GLdouble worldX, worldY, worldZ; //variables to hold world x,y,z coordinates

	// glGetDoublev( GL_MODELVIEW_MATRIX, modelview ); //get the modelview info
	// glGetDoublev( GL_PROJECTION_MATRIX, projection ); //get the projection matrix info
	// glGetIntegerv( GL_VIEWPORT, viewport ); //get the viewport info
 	// std::cout << "Projection: ";
 	// for(auto a : projection)
 	// 	std::cout << a << "; ";
 	// std::cout << std::endl
	//get the world coordinates from the screen coordinates
	// gluUnProject( predictionCoordinate.x, predictionCoordinate.y, predictionCoordinate.z, 
	// 			modelview, projection, viewport, &worldX, &worldY, &worldZ);
	// std::cout << worldX << ", " << worldY << ", " << worldZ << "\n";
	// Map faceDetected.size() into the z coordinate from the camera and store into face3DCoord.z
	predictionCoordinate.x *= xKomp;
	predictionCoordinate.y *= yKomp;
	predictionCoordinate.z = zKomp / predictionCoordinate.z;
	// translate face3DCoord to world coordinates
	predictionCoordinate = prediction.getWorldSpaceCoordinate(predictionCoordinate);

	return Face3D(predictionCoordinate, face.size());
}

void measureMe(Camera &cam, std::string file) {
}
void prediction_test(int argc, char **argv) {
    glutInitPred(argc, argv);
	Camera cam(argc, argv);
	// Generate camera matrix camMat
	Prediction prediction(cam);
	prediction.setCameraProjectionMatrix(prediction.generateProjectionMatrix());

	cv::CascadeClassifier face_cascade;
	face_cascade.load(FACE_SAMPLE_URL);
	Detection detection(face_cascade);

	// Open a named window to render the camera image in
	cv::namedWindow("Predicted Face",1);
	for(auto frame : cam) {
		Face2D faceDetected = detection.detectFace(frame);
		if(faceDetected != Face2D::NO_FACE) {
			Face3D face3DCoord = predict3DCoordinate(faceDetected, prediction);
			cv::Point3 center = face3DCoord.center();
			std::cout << "Prediction: " << center.x << ", " << center.y << ", " << center.z << "\n";

			cv::ellipse( frame, faceDetected.center(), cv::Size( faceDetected.size(), faceDetected.size() ), 0, 0, 360, cv::Scalar( 255, 0, 255 ), 4, 8, 0 );
		}
		
		// Update the image of the window buffer
		imshow( "Predicted Face", frame );
		// Update the window display
		if(cv::waitKey(30) >= 0) break;
	}
}

void prediction_performance(int argc, char **argv) {

   glutInitPred(argc, argv);
	Camera cam(argc, argv);

	// Generate camera matrix camMat
	Prediction prediction(cam);
	prediction.setCameraProjectionMatrix(prediction.generateProjectionMatrix());

	cv::CascadeClassifier face_cascade;
	face_cascade.load(FACE_SAMPLE_URL);
	Detection detection(face_cascade);
		
	Timer timer;
	bool withAlg = false, startWithAlg = withAlg;
	const int fpsCount = 200, resCount = 6 * 2;
	double fpss[fpsCount];
	int currentResolution = 0, currentFPS = 0;
	// Open a named window to render the camera image in
	// cv::namedWindow("Predicted Face",1);
	std::ostream &save = std::cout;
	for(auto frame : cam) {
		if(withAlg) {
			Face2D faceDetected = detection.detectFace(frame);
			if(faceDetected != Face2D::NO_FACE) {
				Face3D face3DCoord = predict3DCoordinate(faceDetected, prediction);
				//cv::Point3 center = face3DCoord.center();

				// cv::ellipse( frame, faceDetected.center(), cv::Size( faceDetected.size(), faceDetected.size() ), 0, 0, 360, cv::Scalar( 255, 0, 255 ), 4, 8, 0 );
			}
		}
		double time = timer.elapsed();
		fpss[currentFPS] = 1.0 / time;
		currentFPS += 1;
		if(currentFPS >= fpsCount) {
			double mean = 0, stdFPS = 0;
			for(int i = 0; i < fpsCount; i++) 
				mean += fpss[i];
			mean = mean / fpsCount;
			for(int i = 0; i < fpsCount; i++) 
				stdFPS += (fpss[i] - mean) * (fpss[i] - mean);

			stdFPS = sqrt(stdFPS/fpsCount);
			save << mean << ", " << stdFPS;
			currentFPS = 0;
			withAlg = !withAlg;
			if(startWithAlg == withAlg) {
				save << "\n";
				break;
			}
			else save << ", ";
		}
		timer.reset();
	}
	// Update the image of the window buffer
	// imshow( "Predicted Face", frame );
	// Update the window display
	// if(cv::waitKey(30) >= 0) break;
	//save.close();
}
// double abs(double a) {return a < 0 ? -a: a;}
void prediction_measure(int argc, char **argv) {
    glutInitPred(argc, argv);
	std::ofstream clear("./results/prediction_measure.txt");
	clear.close();
	Camera cam(argc, argv);
	std::string face_eval_folder = "./face_imgs";
	// Generate camera matrix camMat
	Prediction prediction(cam);
	prediction.setCameraProjectionMatrix(prediction.generateProjectionMatrix());

	cv::CascadeClassifier face_cascade;
	face_cascade.load(FACE_SAMPLE_URL);
	Detection detection(face_cascade);

	glob_t glob_face_positions;
	glob((face_eval_folder + "/*").c_str(), GLOB_TILDE, NULL, &glob_face_positions);
	for(unsigned int i=0; i<glob_face_positions.gl_pathc; i++) {
		glob_t glob_face_position;
		std::string face_path = glob_face_positions.gl_pathv[i];
		std::string real_pos_str = face_path.substr(face_path.find_last_of("_")+1);
		std::stringstream ss(real_pos_str);
		double x, y, z;
		char d;
		ss >> x >> d >> y >> d >> z;
		// std::cout << face_path << "(" << real_pos_str << ")" << " => Real: " << x << ", " << y << ", " << z << std::endl;
		face_path += "/*";
		glob(face_path.c_str(), GLOB_TILDE, NULL, &glob_face_position);
		const int fileCount = glob_face_position.gl_pathc;
		std::vector<cv::Point3d> errors(fileCount, cv::Point3d(0,0,0));
		cv::Point3d errorMin(1000,1000,1000);
		cv::Point3d errorMax(0,0,0);
		cv::Point3d errorAvg(0,0,0);
		cv::Point3d errorSTD(0,0,0);
		for(unsigned int j=0; j< fileCount; j++) {
			// std::cout << glob_face_position.gl_pathv[j] << std::endl;
			bool doCompensate = (j == 0 && face_path.find(face_eval_folder + "/0") == 0);
			cv::Mat faceImg = cv::imread(glob_face_position.gl_pathv[j], CV_LOAD_IMAGE_COLOR);
			Face2D faceDetected = detection.detectFace(faceImg);
			Face3D face3D = predict3DCoordinate(faceDetected, prediction);
			cv::Point3 center = face3D.center();
			if(doCompensate) {
				zKomp = z * 1.0 / center.z;
				yKomp = y * 1.0 / center.y;
				xKomp = x * 1.0 / center.x;
			}
			else {
				// std::cout << "Face2D: " << faceDetected.center().x << ", " << faceDetected.center().y << 
				// 			" => Predicted: " << center.x << ", " << center.y << ", " << center.z << "\n";
				errors[j].x = abs(x - center.x), 
				errors[j].y = abs(y - center.y), 
				errors[j].z = abs(z - center.z);
				errorMin.x = (errors[j].x < errorMin.x ? errors[j].x : errorMin.x);
				errorMin.y = (errors[j].y < errorMin.y ? errors[j].y : errorMin.y);
				errorMin.z = (errors[j].z < errorMin.z ? errors[j].z : errorMin.z);
				errorMax.x = (errors[j].x > errorMin.x ? errors[j].x : errorMin.x);
				errorMax.y = (errors[j].y > errorMin.y ? errors[j].y : errorMin.y);
				errorMax.z = (errors[j].z > errorMin.z ? errors[j].z : errorMin.z);
				errorAvg.x += errors[j].x;
				errorAvg.y += errors[j].y;
				errorAvg.z += errors[j].z;
			}
		}
		if(fileCount > 0) {
			errorAvg.x = errorAvg.x / fileCount;
			errorAvg.y = errorAvg.y / fileCount;
			errorAvg.z = errorAvg.z / fileCount;
			for(unsigned int j = 0; j < errors.size(); j++) {
				errorSTD.x += pow(errorAvg.x - errors[j].x, 2);
				errorSTD.y += pow(errorAvg.y - errors[j].y, 2);
				errorSTD.z += pow(errorAvg.z - errors[j].z, 2);
			}
			errorSTD.x = sqrt(errorSTD.x / fileCount);
			errorSTD.y = sqrt(errorSTD.y / fileCount);
			errorSTD.z = sqrt(errorSTD.z / fileCount);
			std::ofstream save("./results/prediction_measure.txt", std::ofstream::app);
			save << "Path: " << face_path << "\n";
			save << "Mean error: " 
					<< errorAvg.x << ", " 
					<< errorAvg.y << ", " 
					<< errorAvg.z << "\n";
			save << "Error standard diviation: " 
					<< errorSTD.x << ", " 
					<< errorSTD.y << ", " 
					<< errorSTD.z << "\n";
			save << "Error Min: " 
					<< errorMin.x << ", " 
					<< errorMin.y << ", " 
					<< errorMin.z << "\n";
			save << "Error Max: " 
					<< errorMax.x << ", " 
					<< errorMax.y << ", " 
					<< errorMax.z << "\n";
			save.close();
		}
		else std::cout << "No images found in folder =(\n";
	}
	return;
}
