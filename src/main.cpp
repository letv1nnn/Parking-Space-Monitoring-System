#include <opencv2/objdetect.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>

using namespace cv;

void detect_face(Mat& img, CascadeClassifier& cascade, CascadeClassifier& nested_cascade, double scale);
std::string cascade_name, nested_cascade_name;

int main(int argc, char** argv) {
	std::cout << "Running Face Recognition System..." << std::endl;

	VideoCapture capture;
	Mat frame, image;

	CascadeClassifier cascade, nested_cascade;
	double scale = 1.0;

	cascade_name = "./haarcascade_frontalcatface.xml";
	nested_cascade_name = "./haarcascade_eye_tree_eyeglasses.xml";

	std::cout << "Cascade path: " << cascade_name << std::endl;
	std::cout << "Nested cascade path: " << nested_cascade_name << std::endl;

	if (!cascade.load(cascade_name.c_str())) {
		std::cerr << "ERROR: Could not load cascade classifier." << std::endl;
		return -1;
	}

	if (!nested_cascade.load(nested_cascade_name.c_str())) {
		std::cerr << "ERROR: Could not load nested cascade classifier." << std::endl;
		return -1;
	}

	// capturing video from default camera
	capture.open("C:\\Users\\letv1\\Downloads\\video_face_test.mp4");

	if (!capture.isOpened()) {
		std::cerr << "ERRPR: Could not open video capture." << std::endl;
		return -1;
	}

	std::cout << "Face Detection System is running..." << std::endl;
	while (true) {
		capture >> frame;
		if (frame.empty()) {
			std::cerr << "ERROR: Could not read frame from camera." << std::endl;
			break;
		}
		Mat frame1 = frame.clone();

		detect_face(frame1, cascade, nested_cascade, scale);

		char c = (char)waitKey(10);
		if (c == 27 || c == 'q' || c == 'Q') {
			std::cout << "Exiting..." << std::endl;
			break;
		}
	}

	return 0;
}

void detect_face(Mat& img, CascadeClassifier& cascade, CascadeClassifier& nested_cascade, double scale) {
	std::vector<Rect> faces, faces2;
	Mat gray, small_img;

	cvtColor(img, gray, COLOR_BGR2GRAY);
	double fx = 1 / scale;

	resize(gray, small_img, Size(), fx, fx, INTER_LINEAR);
	equalizeHist(small_img, small_img);

	cascade.detectMultiScale(small_img, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	for (size_t i = 0; i < faces.size(); i++) {
		Rect r = faces[i];
		Mat small_img_roi;
		std::vector<Rect> nested_faces;
		Point center;
		Scalar color = Scalar(255, 0, 0);
		int radius;

		double aspect_ratio = (double)r.width / r.height;
		if (0.75 < aspect_ratio && aspect_ratio < 1.3) {
			center.x = cvRound((r.x + r.width * 0.5) * scale);
			center.y = cvRound((r.y + r.height * 0.5) * scale);
			radius = cvRound((r.width + r.height) * 0.25 * scale);

			circle(img, center, radius, color, 3, LINE_AA);
		}
		else {
			rectangle(img, Point(cvRound(r.x * scale), cvRound(r.y * scale)),
				Point(cvRound((r.x + r.width - 1) * scale),
					cvRound((r.y + r.height - 1) * scale)), color, 3, 8, 0);
		}
		if (nested_cascade.empty())
			continue;
		small_img_roi = small_img(r);
	}

	imshow("Face Detection", img);
}

