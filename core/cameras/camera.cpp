#include "camera.h"

FocusCamera::FocusCamera(const ysl::Point3f& position, ysl::Vector3f up, const ysl::Point3f& center):
	m_position(position),
	m_front(center - position),
	m_worldUp(up),
	m_movementSpeed(SPEED),
	m_mouseSensitivity(SENSITIVITY),
	m_center(center),
	m_zoom(ZOOM)
{
	m_right = ysl::Vector3f::Cross(m_front, m_worldUp);
	m_up = ysl::Vector3f::Cross(m_right, m_front);
	//updateCameraVectors(QVector3D(0,1,0),QVector3D(0,0,0),0);
}

void FocusCamera::setPosition(const ysl::Point3f & pos)
{
	m_position = pos;
	m_front = (m_center - m_position).Normalized();
	m_right = ysl::Vector3f::Cross(m_front, m_worldUp).Normalized();
	m_up = ysl::Vector3f::Cross(m_right, m_front).Normalized();
}

void FocusCamera::setUpDirection(const ysl::Vector3f & up)
{
	m_worldUp = up;
	m_front = (m_center - m_position).Normalized();
	m_right = ysl::Vector3f::Cross(m_front, m_worldUp).Normalized();
	m_up = ysl::Vector3f::Cross(m_right, m_front).Normalized();
}

void FocusCamera::setCenter(const ysl::Point3f& center)
{
	m_center = center;
	m_front = (m_center - m_position).Normalized();
	m_right = ysl::Vector3f::Cross(m_front, m_worldUp).Normalized();
	m_up = ysl::Vector3f::Cross(m_right, m_front).Normalized();
}

void FocusCamera::movement(const ysl::Vector3f& direction, float deltaTime)
{
	const auto velocity = m_movementSpeed * direction * deltaTime;
	m_position += velocity;
}

void FocusCamera::rotation(float xoffset, float yoffset)
{
	xoffset *= m_mouseSensitivity;
	yoffset *= m_mouseSensitivity;
	const auto theta = 4.0 * (std::fabs(xoffset) + std::fabs(yoffset));
	const auto v = ((m_right * xoffset) + (m_up * yoffset));
	const auto axis = ysl::Vector3f::Cross(v, -m_front).Normalized();
	updateCameraVectors(axis, theta);
}

void FocusCamera::processMouseScroll(float yoffset)
{
	if (m_zoom >= 1.0f && m_zoom <= 45.0f)
		m_zoom -= yoffset;
	if (m_zoom <= 1.0f)
		m_zoom = 1.0f;
	if (m_zoom >= 45.0f)
		m_zoom = 45.0f;
}

void FocusCamera::updateCameraVectors(const ysl::Vector3f& axis, double theta)
{
	ysl::Transform rotation;
	rotation.SetRotate(axis, theta);
	ysl::Transform translation;
	translation.SetTranslate(-m_center.x, -m_center.y, -m_center.z);
	m_position = translation.Inversed() * (rotation * (translation * m_position));
	m_front = (rotation * m_front.Normalized());
	m_up = (rotation * m_up.Normalized());
	m_right = ysl::Vector3f::Cross(m_front, m_up);
	m_up = ysl::Vector3f::Cross(m_right, m_front);
	m_front.Normalize();
	m_right.Normalize();
	m_up.Normalize();
}
