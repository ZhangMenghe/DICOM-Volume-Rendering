#include "ArucoMarkerTracker.h"

#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/calib3d.hpp>//Rodrigues
//#include <glm/gtx/rotate_vector.hpp>
#include <vrController.h>
//#include <glm/gtx/component_wise.hpp>
#include <glm/gtc/type_ptr.hpp>
ArucoMarkerTracker::ArucoMarkerTracker(){
//    m_cameraMatrix = (cv::Mat1d(3, 3) << 492.07322693, 0, 311.530553, 0, 492.19478694, 242.44158817, 0, 0, 1);
//    m_distCoeffs = (cv::Mat1d(1, 5) << 3.23547400e-01,-1.47500321e+00,-1.40266246e-04,-1.34290186e-03,8.58477151e-01);
    m_cameraMatrix = (cv::Mat1d(3, 3) << 492.1944152, 0, 236.55853355, 0, 492.07290321, 311.53035564, 0, 0, 1);
    m_distCoeffs = (cv::Mat1d(1, 5) << 3.23544718e-01, -1.47495319e+00, -1.34309892e-03,  1.40473255e-04,8.58269404e-01);
}
void ArucoMarkerTracker::setImageSize(int width, int height){
	m_img_width = width;
	m_img_height= height;
}
static glm::vec3 ToDirectionVectorGL(cv::Vec3d& rodrigues1x3) noexcept
{
	cv::Mat rotation3x3;
	cv::Rodrigues(rodrigues1x3, rotation3x3);

	// direction OUT of the screen in CV coordinate system, because we care
	// about objects facing towards us - you can change this to anything
	// OpenCV coordsys: +X is Right on the screen, +Y is Down on the screen,
	//                  +Z is INTO the screen
	cv::Vec3d axis{ 0, 0, -1 };
	cv::Mat direction = rotation3x3 * cv::Mat(axis, false);

	// normalize to a unit vector
	double dirX = direction.at<double>(0);
	double dirY = direction.at<double>(1);
	double dirZ = direction.at<double>(2);
	double len = sqrt(dirX*dirX + dirY*dirY + dirZ*dirZ);
	dirX /= len;
	dirY /= len;
	dirZ /= len;
	// Convert from OpenCV to OpenGL 3D coordinate system
	return glm::vec3(float(dirX), float(-dirY), float(dirZ) );
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
	cv::aruco::estimatePoseSingleMarkers(corners, 0.16, m_cameraMatrix, m_distCoeffs, m_rvecs, m_tvecs);
	
	cv::Mat R;
//	Rodrigues(m_rvecs[0], R);

	glm::mat4 rot_mat(1.0f);

//	for (int row = 0; row < 3; row++) {
//		for (int col = 0; col < 3; col++) {
//			rot_mat[col][row] = (float)R.at<float>(row, col);
//		}
//	}

//	auto test_vec = rot_mat * glm::vec4(.0,.0,-1.0,.0);
//	float len = test_vec.length();
	glm::vec3 a = glm::vec3(0,0,-1);
	glm::vec3 b = -ToDirectionVectorGL(m_rvecs[0]); // in my case (1, 0, 0)
	glm::vec3 v = glm::cross(b, a);
	float angle = acos(glm::dot(b, a) / (glm::length(b) * glm::length(a)));
	glm::mat4 rotmat = glm::rotate(angle, v);

	LOGE("===ori: %f, %f, %f", b.x, b.y, b.z);


	auto tvec = m_tvecs[0];

	glm::mat4 model_mat =
			glm::translate(glm::mat4(1.0), glm::vec3(tvec[0], -tvec[1], -tvec[2]))
			*rotmat;
			//*rot_mat;

	//opencv coordinates -> opengl coord, (x,y,z)->(x, -y, -z)
	//glm is column major
//	float* pSource = (float*)glm::value_ptr(model_mat);
//	for (int i = 0; i < 16; i++)pSource[i] *= m_inverse_[i];


//	model_mat = model_mat * rot_mat;

	//Rotate the original image 90 degree clockwise(x,y,z)->(y, -x, z)
//	float tmp = model_mat[3][0];
//	model_mat[3][0] = model_mat[3][1];  model_mat[3][1] = -tmp;
//	for (int row = 0; row < 3; row++) {
//		for (int col = 0; col < 3; col++) {
//			model_mat[row][col] = (float)R.at<float>(row, col);
//		}
//	}



//	cv::Mat viewMatrixf = cv::Mat::zeros(4, 4, CV_32F);
//
//	for (unsigned int row = 0; row < 3; ++row)
//	{
//		for (unsigned int col = 0; col < 3; ++col)
//		{
//			viewMatrixf.at<float>(row, col) = (float)R.at<double>(row, col);
//		}
//		viewMatrixf.at<float>(row, 3) = (float)tvec[row];// *0.1f;
//	}
//	viewMatrixf.at<float>(3, 3) = 1.0f;
//
//	//反转Y、Z轴
//	cv::Mat cvToGl = cv::Mat::zeros(4, 4, CV_32F);
//	cvToGl.at<float>(0, 0) = 1.0f;
//	cvToGl.at<float>(1, 1) = -1.0f; // Invert the y axis
//	cvToGl.at<float>(2, 2) = -1.0f; // invert the z axis
//	cvToGl.at<float>(3, 3) = 1.0f;
//	viewMatrixf = cvToGl * viewMatrixf;
////	cv::transpose(viewMatrixf, viewMatrixf);
//
//	glm::mat4 test_mat = glm::make_mat4(viewMatrixf.data);
//	float ttmp = test_mat[3][0];
//	test_mat[3][0] = test_mat[3][1];  test_mat[3][1] = -ttmp;

	vrController::instance()->setPosition(model_mat);
	return true;
}