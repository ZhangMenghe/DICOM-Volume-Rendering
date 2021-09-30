#include "ArucoMarkerTracker.h"

#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/calib3d.hpp>//Rodrigues
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <vrController.h>

ArucoMarkerTracker::ArucoMarkerTracker(){
//    m_cameraMatrix = (cv::Mat1d(3, 3) << 492.07322693, 0, 311.530553, 0, 492.19478694, 242.44158817, 0, 0, 1);
//    m_distCoeffs = (cv::Mat1d(1, 5) << 3.23547400e-01,-1.47500321e+00,-1.40266246e-04,-1.34290186e-03,8.58477151e-01);
    //samsung after rotation
    m_cameraMatrix = (cv::Mat1d(3, 3) << 492.1944152, 0, 236.55853355, 0, 492.07290321, 311.53035564, 0, 0, 1);
    m_distCoeffs = (cv::Mat1d(1, 5) << 3.23544718e-01, -1.47495319e+00, -1.34309892e-03,  1.40473255e-04,8.58269404e-01);
}
void ArucoMarkerTracker::setImageSize(int width, int height){
	m_img_width = width;
	m_img_height= height;
}

glm::quat getQuaternion(cv::Vec3d& rodrigues1x3){
	double Q[4];
	cv::Mat R;
	cv::Rodrigues(rodrigues1x3, R);
	double trace = R.at<double>(0,0) + R.at<double>(1,1) + R.at<double>(2,2);

	if (trace > 0.0)
	{
		double s = sqrt(trace + 1.0);
		Q[3] = (s * 0.5);
		s = 0.5 / s;
		Q[0] = ((R.at<double>(2,1) - R.at<double>(1,2)) * s);
		Q[1] = ((R.at<double>(0,2) - R.at<double>(2,0)) * s);
		Q[2] = ((R.at<double>(1,0) - R.at<double>(0,1)) * s);
	}

	else
	{
		int i = R.at<double>(0,0) < R.at<double>(1,1) ? (R.at<double>(1,1) < R.at<double>(2,2) ? 2 : 1) : (R.at<double>(0,0) < R.at<double>(2,2) ? 2 : 0);
		int j = (i + 1) % 3;
		int k = (i + 2) % 3;

		double s = sqrt(R.at<double>(i, i) - R.at<double>(j,j) - R.at<double>(k,k) + 1.0);
		Q[i] = s * 0.5;
		s = 0.5 / s;

		Q[3] = (R.at<double>(k,j) - R.at<double>(j,k)) * s;
		Q[j] = (R.at<double>(j,i) + R.at<double>(i,j)) * s;
		Q[k] = (R.at<double>(k,i) + R.at<double>(i,k)) * s;
	}
	return glm::quat(Q[3], Q[0], Q[1], Q[2]);
}
bool ArucoMarkerTracker::Update(const uint8_t* data){
    cv::Mat gray_frame = cv::Mat(m_img_height, m_img_width, CV_8U, (void*)data);
	cv::rotate(gray_frame, gray_frame, cv::ROTATE_90_CLOCKWISE);

	//try estimate marker
	static cv::Ptr<cv::aruco::Dictionary> dictionary = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_6X6_250);

	std::vector<int> ids;
	std::vector<std::vector<cv::Point2f>> corners;
	cv::aruco::detectMarkers(gray_frame, dictionary, corners, ids);
	if (ids.empty()) return false;

	// if at least one marker detected
	std::vector<cv::Vec3d> rvecs, tvecs;
	cv::aruco::estimatePoseSingleMarkers(corners, 0.16, m_cameraMatrix, m_distCoeffs, rvecs, tvecs);

	glm::quat q = getQuaternion(rvecs[0]);
	q.y = -q.y;q.z = -q.z;
	glm::mat4 rotmat = glm::toMat4(q);

	auto tvec = tvecs[0];
	glm::mat4 Marker2Camera =
			glm::translate(glm::mat4(1.0), glm::vec3(tvec[0], -tvec[1], -tvec[2]))
			* rotmat
			* glm::scale(glm::mat4(1.0), glm::vec3(0.16f));
	vrController::instance()->setSpaceMatrix(Marker2Camera);
	Manager::camera->setViewMat(glm::mat4(1.0f));
	return true;
}