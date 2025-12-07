#include <iostream>
#include <opencv2/opencv.hpp>

static void printHelp() {
    std::cout << "Camera Calibration Tool\n"
              << "Usage: ./CameraCalibrator [options]\n\n"
              << "Options:\n"
              << "  -c, --camera <id>        ID of the camera to use (Default: 0)\n"
              << "  -b, --board <w> <h>      Board size: width and height (e.g., -b 9 6)\n"
              << "  -s, --square <val>       Physical square size in meters/mm (e.g., -s 0.025)\n"
              << "  -o, --out <fileNameWithExtension>	File in which settings are saved\n"
	      << "  -h, --help               Show this help message\n" 
              << std::endl;
}

int main(int argc, char** argv) {
    	std::cout << "hi" << std::endl;
	
	//default values
	cv::Size patternsize(9, 6);
	float squareSize = 0.024;
	std::size_t cameraIndex = 0;
	std::string outFileName = "calibratedParametrs.yml";

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
		else if ((a == "--out" || a == "-o") && i + 1 < argc) {
        		outFileName = argv[++i];
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

	std::vector<std::vector<cv::Point2f>> allCorners;
	std::vector<std::vector<cv::Point3f>> objectPoints;

	std::vector<cv::Point3f> obj;
    	for (int y = 0; y < patternsize.height; y++)
        	for (int x = 0; x < patternsize.width; x++)
            		obj.emplace_back(x * squareSize, y * squareSize, 0.0f);
	
	cv::Mat cameraMatrix, distCoeffs;
    	bool calibrated = false;

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
		
		cv::Mat display;
        	if (calibrated) {
			cv::Mat und;
			cv::undistort(frame, und, cameraMatrix, distCoeffs);
            		if (und.size() != frame.size()) cv::resize(und, und, frame.size());
			cv::hconcat(frame, und, display);
        	} else {
			cv::Mat placeholder = frame.clone();
			cv::putText(placeholder, "Undistorted (off)", cv::Point(10, 30),
                    	cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);	
			cv::putText(frame, "press <c> to capture frame if parren is found", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX,
               	 		0.8, cv::Scalar(0, 255, 0), 2);
			cv::putText(frame, "press <k> to calibrate camera", cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX,
               	 		0.8, cv::Scalar(0, 255, 0), 2); 

			cv::hconcat(frame, placeholder, display);
        	}


		int key = cv::waitKey(1);

		if (key == 'c') {
            		if (patternfound) {
                		allCorners.push_back(corners);
                		objectPoints.push_back(obj);
				std::cout << "captured\n";
            		} else {
				std::cout << "Chessboard not found — try again\n";
            		}
        	} else if (key == 'k') {
            		if (allCorners.size() < 5) {
				std::cout << "Need at least 5 frames. Currently: "<< allCorners.size() << std::endl;
            		} else {
				std::vector<cv::Mat> rvecs, tvecs;
				std::cout << "Running calibration...\n";
                		double rms = cv::calibrateCamera(objectPoints, allCorners,
                                             gray.size(), cameraMatrix,
                                             distCoeffs, rvecs, tvecs);
				std::cout << "RMS error = " << rms << std::endl;
				std::cout << "Camera matrix:\n" << cameraMatrix << std::endl;
				std::cout << "Distortion:\n" << distCoeffs << std::endl;
                		calibrated = true;
            		}
        	} else if(key == 's'){
			if(!calibrated) {
				std::cout << "Not calibrated yet, there is nothing to save";
			} else {
				cv::FileStorage fs(outFileName, cv::FileStorage::WRITE);
				fs << "camera_matrix" << cameraMatrix;
				fs << "distortion_coefficients" << distCoeffs;
				std::cout << "Settings are saved to " << outFileName << std::endl;
			}
		} 
		else if(key == 'q') break;

		cv::imshow("live", display);

	}

	return 0;
}
