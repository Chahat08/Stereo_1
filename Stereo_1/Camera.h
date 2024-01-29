#pragma once

#include <glm/glm.hpp>

class Camera {
public:
	glm::vec3 position;
	glm::vec3 viewDirection;
	glm::vec3 upDirection;
	float focalLength; /*focal length about view direction*/
	float aperture; /*camera aperture*/
	float eyeSeparation; /* eye separation */

	Camera() :
		position(NULL), viewDirection(NULL), upDirection(NULL), aperture(0.0f), focalLength(0.0f), eyeSeparation(0.0f) {};

	Camera(glm::vec3 pos, glm::vec3 viewDir, glm::vec3 upDir, float focalLen, float aper, float eyeSep) :
		position(pos), viewDirection(viewDir), upDirection(upDir), focalLength(focalLen), aperture(aper), eyeSeparation(eyeSep) {};
};
