from picamera import PiCamera

from time import sleep

class ImageAnalysis(object):
    def __init__(self):
        self.file = open('tmp.txt', 'w')
    def write(self, data):
        print "One time called write"
        self.file.write(data)

from picamera import PiCamera
camera = PiCamera()

def initPiCamera(width=160, height=160, callback=None):
	global camera
	from picamera.array import PiRGBArray 
	import io

	camera.resolution = (width, height)
	camera.framerate = 32
	camera.rotation  = 90
	camera.hflip = True

	rawCapture = PiRGBArray(camera)
	stream = io.BytesIO()
	for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
		# stream.truncate()
		# stream.seek(0)
		rawCapture.truncate(0)
		# sleep(1)
		if callback == None:
			yield frame.array
		elif not callback(frame.array):
			break

if __name__ == '__main__':
	camera.start_preview()
	for image in initPiCamera(320*3, 240*3):
		pass
	camera.stop_preview()
