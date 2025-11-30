#include <iostream>
#include <opencv2/opencv.hpp>

static void printHelp() {
    std::cout << "Camera Calibration Tool\n"
              << "Usage: ./CameraCalibrator [options]\n\n"
              << "Options:\n"
              << "  -c, --camera <id>        ID of the camera to use (Default: 0)\n"
              << "  -b, --board <w> <h>      Board size: width and height (e.g., -b 9 6)\n"
              << "  -s, --square <val>       Physical square size in meters/mm (e.g., -s 0.025)\n"
              << "  -h, --help               Show this help message\n" 
              << std::endl;
}

int main(int argc, char** argv) {
    	std::cout << "hi" << std::endl;
	
	//default values
	cv::Size patternsize(9, 6);
	float squareSize = 0.024;
	std::size_t cameraIndex = 0;

 	for (int i = 1; i < argc; i++) {
    		std::string a = argv[i];

    		if ((a == "--camera" || a == "-c") && i + 1 < argc) {
        		cameraIndex = std::stoi(argv[++i]);
    		}	 
    		else if ((a == "--board" || a == "-b") && i + 2 < argc) {
        		patternsize.width = std::stoi(argv[++i]);
        		patternsize.height = std::stoi(argv[++i]);
    		} 
    		else if ((a == "--square" || a == "-s") && i + 1 < argc) {
        		squareSize = std::stof(argv[++i]);
    		} 
		else if((a == "--help" || a == "-h")) {
			printHelp();
			return 0;
		}
	}

	cv::VideoCapture cap;
	
	cap.open(cameraIndex);

    	if (!cap.isOpened()) {
		std::cerr << "ERROR: Could not open camera or video\n";
        	return -1;
    	}
	
	cv::Mat frame, gray;

	while(true){
		cap >> frame;
		if (frame.empty()) {
            		std::cerr << "ERROR: Blank frame captured\n";
            		break;
        	}
        		
		cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);


		std::vector<cv::Point2f> corners;

        	bool patternfound = cv::findChessboardCorners(gray, patternsize, corners, cv::CALIB_CB_ADAPTIVE_THRESH | cv::CALIB_CB_NORMALIZE_IMAGE | cv::CALIB_CB_FAST_CHECK);

        	if(patternfound) {

			cv::cornerSubPix(gray, corners, cv::Size(11, 11), cv::Size(-1, -1), cv::TermCriteria(cv::TermCriteria::EPS + cv::TermCriteria::COUNT, 30, 0.1));
			cv::drawChessboardCorners(frame, patternsize, cv::Mat(corners), patternfound);
        	}

		cv::imshow("live", frame);

		if(cv::waitKey(10) == 27) break;
	}

	return 0;
}
