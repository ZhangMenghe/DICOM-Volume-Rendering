#ifndef ARUCO_MARKER_TRACKER_H
#define ARUCO_MARKER_TRACKER_H

#include <functional>
#include <mutex>
#include <opencv2/core.hpp>
class ArucoMarkerTracker {
public:
	ArucoMarkerTracker();
//    void getLatestFrame(const char*& data, int& size) { data = (const char*)m_texture_data; size = m_tex_size; }
//    bool GetFirstTransformation(cv::Vec3d& rvec, cv::Vec3d& tvec);
	void setImageSize(int width, int height);
	bool Update(const uint8_t* data);
private:
	int m_img_width, m_img_height;
    cv::Mat m_cameraMatrix, m_distCoeffs;
    const float m_inverse_[16] = {
                   1.0, -1.0, -1.0, 1.0,
                   1.0,-1.0,-1.0,1.0,
                   1.0,-1.0,-1.0,1.0,
                   1.0, -1.0, -1.0, 1.0 };
    std::vector<cv::Vec3d> m_rvecs, m_tvecs;
};
#endif

