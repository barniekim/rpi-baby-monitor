
// OpenCV includes
#include <opencv2/opencv.hpp>

#include <ctime>

using namespace cv;
using namespace std;

inline int detect_motion(const Mat &motion, Mat &result, Mat &result_cropped, 
				int x_start, int x_stop, int y_start, int y_stop, 
				int max_deviation, Scalar &color){
	Scalar mean, stddev;
	meanStdDev(motion, mean, stddev);

	if(stddev[0] < max_deviation){
		int number_of_changes = 0;
		int min_x = motion.cols, max_x = 0;
		int min_y = motion.rows, max_y = 0;
		
		for(int j=y_start; j<y_stop; j+=2){
			for( int i=x_start; i<x_stop; i+=2){
				if(static_cast<int>(motion.at<uchar>(j, i)) == 255){
					number_of_changes++;
					if(min_x > i) min_x = i;
					if(min_x < i) max_x = i;
					if(min_y > i) min_y = j;
					if(min_y < i) max_y = j;
				}
			}
		}
		
		if(number_of_changes){
			if(min_x-10 > 0) min_x -= 10;
			if(min_y-10 > 0) min_y -= 10;
			if(min_x+10 < result.cols-1) min_x += 10;
			if(min_y+10 < result.rows-1) min_y += 10;

			Point x(min_x, min_y);
			Point y(max_x, max_y);
			Rect rect(x, y);
			Mat cropped = result(rect);
			cropped.copyTo(result_cropped);
			rectangle(result, rect, color, 2);
		}
		return number_of_changes;
	}
	return 0;	
}

int main (int argc, char **argv){


	/** open a videocapture object w/ default device */
	VideoCapture cap;
	cap.open(0);
	if(!cap.isOpened()){
		cerr << "ERR: VideoCapture couldn't be opened" << endl;
		return -1;
	}
	cap.set(CV_CAP_PROP_FRAME_WIDTH,  640);
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);

	/** FPS check */
	int frame_counter = 0;
	int tick = 0;
	int fps = 0;
	std::time_t time_begin 	= std::time(0);
	std::time_t time_now 	= time_begin;

	/** do video capture */
	cout << "start capturing image.." << endl;

	Mat result, result_cropped;
	Size result_size = Size(500, 375);

	Mat prev_frame, curr_frame, next_frame;
	cap.read(prev_frame);
	cap.read(curr_frame);
	cap.read(next_frame);

	cvtColor(prev_frame, prev_frame, CV_BGR2GRAY);
	cvtColor(curr_frame, curr_frame, CV_BGR2GRAY);
	cvtColor(next_frame, next_frame, CV_BGR2GRAY);


	Mat d1, d2, motion;
	int number_of_changes=0;
	int number_of_sequence=0;
	Scalar mean_;
	Scalar COLOR_GREEN(0, 255, 0); // GREEN

	int x_start = 0, x_stop = curr_frame.cols-1;
	int y_start = 0, y_stop = curr_frame.rows-1;

	int there_is_motion = 5;

	int max_deviation = 20;

	Mat kernel_ero = getStructuringElement(MORPH_RECT, Size(2, 2));

	while(true){

		// get a frame
		prev_frame = curr_frame;
		curr_frame = next_frame;
		cap.read(next_frame);
		
		result = next_frame;
		cvtColor(next_frame, next_frame, CV_RGB2GRAY);

		absdiff(prev_frame, next_frame, d1);
		absdiff(next_frame, curr_frame, d2);
		bitwise_and(d1, d2, motion);
		threshold(motion, motion, 35, 255, CV_THRESH_BINARY);
		erode(motion, motion, kernel_ero);

		number_of_changes = detect_motion(motion, result, result_cropped, x_start, x_stop, y_start, y_stop, max_deviation, COLOR_GREEN);

		printf("# of changes: %d\n", number_of_changes);

		/*
		 * write texts on the frame: fps, resolution
		int text_pos_y = 0;

		text_pos_y += 30;
		putText(result, 
			format("Average FPS: %d", fps), 
			Point(30, text_pos_y), 
			FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255));

		text_pos_y += 30;
		putText(result, 
			format("Size: %d x %d", result.size().width, result.size().height), 
			Point(30, text_pos_y), 
			FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0,0,255));
		*/

		// show frames
		imshow("result", result);	

		if(waitKey(1) >= 0){ break; }
	}

	return 0;
}
