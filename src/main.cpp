#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    	std::cout << "hi" << std::endl;
	
	int cameraIndex = 0;
	cv::VideoCapture cap;
	
	cap.open(cameraIndex);

    	if (!cap.isOpened()) {
		std::cerr << "ERROR: Could not open camera or video\n";
        	return -1;
    	}
	
	cv::Mat frame;

	while(true){
		cap >> frame;
		
		if (frame.empty()) {
            		std::cerr << "ERROR: Blank frame captured\n";
            		break;
        	}
        	
		cv::imshow("live", frame);

		if(cv::waitKey(10) == 27) break;
	}

	return 0;
}
