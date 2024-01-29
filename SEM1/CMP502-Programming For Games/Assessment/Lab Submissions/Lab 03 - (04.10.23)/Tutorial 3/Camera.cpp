#include "pch.h"
#include "Camera.h"

//camera for our app simple directX application. While it performs some basic functionality its incomplete. 
//

Camera::Camera()
{
	//initalise values. 
	m_orientation.x = 0.0f;		//rotation around x - pitch
	m_orientation.y = 0.0f;		//rotation around y - yaw
	m_orientation.z = 0.0f;		//rotation around z - roll	//we tend to not use roll a lot in first person

	m_lookat.x = 0.0f;
	m_lookat.y = 0.0f;
	m_lookat.z = 0.0f;

	//force update with initial values to generate other camera data correctly for first update. 
	Update();
}


Camera::~Camera()
{
}

void Camera::Update()
{
	//rotation in yaw - using the paramateric equation of a circle
	m_forward.x = sin((m_orientation.y)*3.1415f / 180.0f);
	m_forward.z = cos((m_orientation.y)*3.1415f / 180.0f);
	m_forward.Normalize();

	//create right vector from look Direction
	m_forward.Cross(DirectX::SimpleMath::Vector3::UnitY, m_right);

	//update lookat point
	m_lookat = m_position + m_forward;

	//apply camera vectors
	m_cameraMatrix = (DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_forward, DirectX::SimpleMath::Vector3::UnitY));

}

DirectX::SimpleMath::Matrix Camera::getCameraMatrix()
{
	return m_cameraMatrix;
}

void Camera::setPosition(DirectX::SimpleMath::Vector3 newPosition)
{
}

DirectX::SimpleMath::Vector3 Camera::getPosition()
{
	return m_position;
}

void Camera::setRotation(DirectX::SimpleMath::Vector3 newRotation)
{
}

DirectX::SimpleMath::Vector3 Camera::getRotation()
{
//	DirectX::SimpleMath::Vector3 rotations(m_pitch, m_yaw, m_roll);
	return m_orientation;
}
