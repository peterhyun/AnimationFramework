#ifndef CAMERA_H
#define CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <iostream>

#define CAM_SENSITIVITY 0.08f
#define CAM_SPEED 50.0f


class Camera
{
public:

	enum CamControl { FORWARD = 0, BACKWARD, LEFT, RIGHT };

	Camera();

	~Camera();

	glm::mat4 getViewMatrix() const
	{
		return glm::lookAt(_position, _position + _front, _up);
	}

	glm::vec3 position() const
	{
		return _position;
	}

	//void keyboardInput(CamControl control, float deltaTime);

	void mouseInput(float x, float y);

	void mouseScrollInput(float yoffset);

	float zoom() const { return _zoom; };

	void translate(float xoffset, float yoffset);

private:
	glm::vec3 _position;
	glm::vec3 _front;
	glm::vec3 _up;
	glm::vec3 _right;
	glm::vec3 _worldFront;
	glm::vec3 _worldUp;
	glm::vec3 _worldRight;
	glm::quat Orientation;
	float _zoom;

	void updateCameraVectors();
};

#endif
