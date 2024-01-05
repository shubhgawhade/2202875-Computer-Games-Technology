#include "pch.h"
#include "Camera.h"

//camera for our app simple directX application

Camera::Camera()
{
	//initalise values. 
	//Orientation and Position are how we control the camera. 
	m_orientation.x = -90.0f;	//rotation around x - pitch
	m_orientation.y = 0.0f;		//rotation around y - yaw
	m_orientation.z = 0.0f;		//rotation around z - roll	//we tend to not use roll a lot in first person

	//camera position in space.
	m_position.x = 0.0f;		 
	m_position.y = 0.0f;
	m_position.z = 0.0f;

	//Look target point
	m_lookat.x = 0.0f;		
	m_lookat.y = 0.0f;
	m_lookat.z = 0.0f;

	//forward/look direction
	m_forward.x = 0.0f;		
	m_forward.y = 0.0f;
	m_forward.z = 0.0f;

	m_right.x = 0.0f;
	m_right.y = 0.0f;
	m_right.z = 0.0f;
	
	// Speed variables
	m_movespeed = 0.05f;
	m_boostspeed = 2.5f;
	m_camRotRate = 0.5f;

	//active = false;
	//force update with initial values to generate other camera data correctly for first update. 
	Update();
}


Camera::~Camera()
{
}

void Camera::Update()
{
	if (active)
	{
		m_forward.x = sin(DirectX::XMConvertToRadians(m_orientation.y));
		m_forward.y = sin(DirectX::XMConvertToRadians(m_orientation.z));
		m_forward.z = cos(DirectX::XMConvertToRadians(m_orientation.y));
		m_forward.Normalize();

		//create right vector from look Direction
		m_forward.Cross(DirectX::SimpleMath::Vector3::UnitY, m_right);

		//update lookat point
		m_lookat = m_position + m_forward;

		//apply camera vectors and create camera matrix
		m_cameraMatrix = (DirectX::SimpleMath::Matrix::CreateLookAt(m_position, m_lookat, DirectX::SimpleMath::Vector3::UnitY));
	}
}

DirectX::SimpleMath::Matrix Camera::getCameraMatrix()
{
	return m_cameraMatrix;
}

void Camera::setPosition(DirectX::SimpleMath::Vector3 newPosition)
{
	m_position = newPosition;
}

void Camera::setSmoothPosition(DirectX::SimpleMath::Vector3 newPosition)
{
	// Smooth movement using Lerp
	m_position = XMVectorLerp(m_position, newPosition, 0.01f);
}


DirectX::SimpleMath::Vector3 Camera::getPosition()
{
	return m_position;
}

DirectX::SimpleMath::Vector3 Camera::getForward()
{
	return m_forward;
}

DirectX::SimpleMath::Vector3 Camera::getRight()
{
	return m_right;
}

void Camera::setRotation(DirectX::SimpleMath::Vector3 newRotation)
{
	m_orientation = newRotation;
}

void Camera::setSmoothRotation(DirectX::SimpleMath::Vector3 newRotation)
{
	// Smooth rotation using Slerp
	m_orientation = XMQuaternionSlerp(m_orientation, newRotation,0.3f);
}


DirectX::SimpleMath::Vector3 Camera::getRotation()
{
	return m_orientation;
}

float Camera::getMoveSpeed()
{
	return m_movespeed;
}

float Camera::getBoostSpeed()
{
	return m_boostspeed;
}

float Camera::getRotationSpeed()
{
	return m_camRotRate;
}
