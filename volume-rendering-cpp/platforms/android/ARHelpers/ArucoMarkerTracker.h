#ifndef ARUCO_MARKER_TRACKER_H
#define ARUCO_MARKER_TRACKER_H

#include <functional>
#include <mutex>
#include <opencv2/core.hpp>
class ArucoMarkerTracker {
public:
	ArucoMarkerTracker();
	void setImageSize(int width, int height);
	bool Update(const uint8_t* data);
private:
	int m_img_width, m_img_height;
    cv::Mat m_cameraMatrix, m_distCoeffs;
};
#endif

