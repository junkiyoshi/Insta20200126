#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(30);
	ofSetWindowTitle("openframeworks");

	ofBackground(255);
	ofSetColor(255);
	ofEnableDepthTest();

	this->cap_size = cv::Size(1280, 720);

	this->cap.open(0);
	this->cap.set(cv::CAP_PROP_FPS, 30);
	this->cap.set(cv::CAP_PROP_FRAME_WIDTH, this->cap_size.width);
	this->cap.set(cv::CAP_PROP_FRAME_HEIGHT, this->cap_size.height);

	this->face_cascade.load("D:\\opencv_4.1.2\\build\\install\\etc\\haarcascades\\haarcascade_frontalface_alt2.xml");
}

//--------------------------------------------------------------
void ofApp::update() {

	this->cap >> this->frame;
	cv::flip(this->frame, this->frame, 1);
	cv::cvtColor(this->frame, this->frame, cv::COLOR_BGR2RGB);

	cv::Mat gray_frame, small_frame;
	cv::cvtColor(this->frame, gray_frame, cv::COLOR_RGB2GRAY);
	cv::resize(gray_frame, small_frame, cv::Size(this->cap_size.width * 0.5, this->cap_size.height * 0.5));

	vector<cv::Rect> faces;
	this->face_cascade.detectMultiScale(small_frame, faces);

	int face_count = 0;
	glm::vec2 face_location, face_size;
	float max_area = 0.f;
	for (cv::Rect r : faces) {

		auto tmp_face_location = glm::vec2(r.x * 2.f, r.y * 2.f);
		auto tmp_face_size = glm::vec2(r.width * 2, r.height * 2);
		auto area = r.width * r.height;
		if (area > max_area) {

			face_location = tmp_face_location;
			face_size = tmp_face_size;
		}
	}

	this->mesh.clear();

	int span = 1;
	for (int x = 0; x < this->frame.cols; x += span) {

		float param_y = 0;
		if (face_location.x < x && x < face_location.x + face_size.x) {

			param_y = ofMap(ofNoise((x / 20) * 20, ofGetFrameNum() * 0.05), 0, 1, -100, 100);
		}

		for (int y = 0; y < this->frame.rows; y += span) {

			glm::vec3 vertex = glm::vec3(x, y + param_y, 0) - glm::vec3(this->cap_size.width * 0.5, this->cap_size.height * 0.5, 0);
			cv::Vec3b tmp = this->frame.at<cv::Vec3b>(y, x);
			auto color = ofColor(tmp[0], tmp[1], tmp[2]);

			this->mesh.addVertex(vertex);
			this->mesh.addColor(ofColor(0.299 * color.r + 0.587 * color.g + 0.114 * color.b));
		}
	}

	int cols = this->frame.rows / span;
	int rows = this->frame.cols / span;

	for (int col = 1; col < cols; col++) {

		for (int row = 1; row < rows; row++) {

			this->mesh.addIndex(col + row * cols);
			this->mesh.addIndex((col - 1) + (row - 1) * cols);
			this->mesh.addIndex((col - 1) + row * cols);

			this->mesh.addIndex(col + row * cols);
			this->mesh.addIndex((col - 1) + (row - 1) * cols);
			this->mesh.addIndex(col + (row - 1) * cols);
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {

	this->cam.begin();
	ofRotateX(180);

	this->mesh.draw();

	this->cam.end();
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}