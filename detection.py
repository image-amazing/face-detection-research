import numpy as np
import cv2


class Face2D(object):
    def __init__(self, face, right_eye, left_eye):
        self._face = face
        self._reye = right_eye
        self._leye = left_eye
    def getFacePosition(self):
        return self.face

class ImageAnalysis(object):
    def __init__(self, cascades):
        self.face_cascade = cascades["face_cascade"]
        self.eye_cascade = cascades["eye_cascade"]
    def write(self, data):
        print "One time called write"
        self.file.write(data)
    def processFaceDetection(self, img):
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        faces = self.face_cascade.detectMultiScale(gray, scaleFactor=1.4, minSize=(2, 2), maxSize=(150, 150))
        if len(faces) > 0:
            return Face2D((faces[0][0] + faces[0][2] / 2.0, faces[0][1] + faces[0][3] / 2.0), None, None)
        return None
        # for (x,y,w,h) in faces:
        #     print x, y, w, h
        #     cv2.rectangle(img,(x,y),(x+w,y+h),(255,0,0),2)
            # off = 30
            # roi_gray = gray[y-off:y+h+off*2, x-off:x+w+off*2]
            # roi_color = img[y-off:y+h+off*2, x-off:x+w+off*2]
            # eyes = self.eye_cascade.detectMultiScale(roi_gray)
            # for (ex,ey,ew,eh) in eyes:
            #     cv2.rectangle(roi_color,(ex,ey),(ex+ew,ey+eh),(0,255,0),2)
        #cv2.imshow('img',img)

if __name__ == '__main__':
    # multiple cascades: https://github.com/Itseez/opencv/tree/master/data/haarcascades

    #https://github.com/Itseez/opencv/blob/master/data/haarcascades/haarcascade_frontalface_default.xml
    face_cascade = cv2.CascadeClassifier('face_detection/haarcascades/haarcascade_frontalface_alt2.xml')
    #https://github.com/Itseez/opencv/blob/master/data/haarcascades/haarcascade_eye.xml
    eye_cascade = cv2.CascadeClassifier('face_detection/haarcascades/haarcascade_eye.xml')
    #print eye_cascade, face_cascade
    #cap = cv2.VideoCapture(0)
    imgAnalysis = ImageAnalysis({ "face_cascade": face_cascade, "eye_cascade": eye_cascade })

    import camera

    import time

    #Works awesome for frntal-face-default
    # for image in camera.initPiCamera(320, 320):

    for image in camera.initPiCamera(320, 320):
        # print "new frame"
        start = time.time()
        #image = cv2.resize(image, (500, 500))
        print imgAnalysis.processFaceDetection(image)
        end = time.time()
        # print "Frame took ", end - start, " seconds to complete"
        cv2.imshow('img', image)
        k = cv2.waitKey(1) & 0xff
        if k == 27:
            break
    #camera.stop_preview()
    cv2.destroyAllWindows()
