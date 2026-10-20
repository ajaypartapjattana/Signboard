#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

inline constexpr glm::vec3 CAMERA_FW( 0.0f,  0.0f, -1.0f);
inline constexpr glm::vec3 CAMERA_UP( 0.0f,  1.0f,  0.0f);
inline constexpr glm::vec3 CAMERA_RT( 1.0f,  0.0f,  0.0f);

class Camera {
public:
	struct Orientation {
		glm::vec3 position;
		glm::quat rotation;
	};

protected:
	Orientation orientation;

public:
	Camera() noexcept
		: orientation{ glm::vec3(0.0f), glm::quat(1.0f, 0.0f, 0.0f, 0.0f) }
	{}

	explicit Camera(const Orientation& orientation) noexcept
		: orientation(orientation)
	{
		this->orientation.rotation = glm::normalize(this->orientation.rotation);
	}

	void moveW(const glm::vec3& delta) noexcept {
		orientation.position += delta;
	}

	void moveL(const glm::vec3& delta) noexcept {
		orientation.position += orientation.rotation * delta;
	}

	void moveUniformW(const glm::vec3& dir, float distance) noexcept {
		const float length2 = glm::dot(dir, dir);

		if (length2 > 0.0f)
			orientation.position += dir * (distance * glm::inversesqrt(length2));
	}

	void moveUniformL(const glm::vec3& dir, float distance) noexcept {
		const float length2 = glm::dot(dir, dir);

		if (length2 > 0.0f)
			orientation.position += orientation.rotation * (dir * (distance * glm::inversesqrt(length2)));
	}

	void rotateW(const glm::vec3& axis, float angle) noexcept {
		glm::quat delta = glm::angleAxis(angle, glm::normalize(axis));
		orientation.rotation = glm::normalize(delta * orientation.rotation);
	}

	void rotateL(const glm::vec3& axis, float angle) noexcept {
		glm::quat delta = glm::angleAxis(angle, glm::normalize(axis));
		orientation.rotation = glm::normalize(orientation.rotation * delta);
	}

	void moveTo(const glm::vec3& position) noexcept {
		orientation.position = position;
	}

	void lookAt(const glm::vec3& target) noexcept {
		glm::vec3 dir = target - orientation.position;

		if (glm::dot(dir, dir) == 0.0f)
			return;

		orientation.rotation = glm::quatLookAt(glm::normalize(dir), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 forward() const noexcept {
		return orientation.rotation * CAMERA_FW;
	}

	glm::vec3 up() const noexcept {
		return orientation.rotation * CAMERA_UP;
	}

	glm::vec3 right() const noexcept {
		return orientation.rotation * CAMERA_RT;
	}

	glm::mat4 view() const noexcept {
		return glm::mat4_cast(glm::conjugate(orientation.rotation)) * glm::translate(glm::mat4(1.0f), -orientation.position);
	}
};

class PerspectiveCamera : public Camera {
public:
	struct Projection {
		float fov, aspect, near, far;
	};

private:
	Projection projection;

public:
	PerspectiveCamera() noexcept
		: Camera()
		, projection{ 45.0f, 1.0f, 0.1f, 1000.0f }
	{}

	explicit PerspectiveCamera(const Orientation& orientation, const Projection& projection) noexcept
		: Camera(orientation)
		, projection(projection)
	{}

	void focus(float delta) noexcept {
		projection.fov += delta;
	}

	void focusAt(float fov) noexcept {
		projection.fov = fov;
	}

	void aspect(float aspect) noexcept {
		projection.aspect = aspect;
	}

	void clip(float near, float far) noexcept {
		projection.near = near;
		projection.far = far;
	}

	glm::mat4 project() const noexcept {
		return glm::perspectiveRH_ZO(glm::radians(projection.fov), projection.aspect, projection.near, projection.far);
	}
};

class OrthographicCamera : public Camera {
public:
	struct Projection {
		float left, right, bottom, top, near, far;
	};

private:
	Projection projection;

public:
	OrthographicCamera() noexcept
		: Camera()
		, projection{ 0.5f, 0.5f, 0.5f, 0.5f, 0.1f, 1000.0f }
	{}

	explicit OrthographicCamera(const Orientation& orientation, const Projection& projection) noexcept
		: Camera(orientation)
		, projection(projection)
	{}

	void bound(float left, float right, float bottom, float top) noexcept {
		projection.left = left;
		projection.right = right;
		projection.bottom = bottom;
		projection.top = top;
	}

	void clip(float near, float far) noexcept {
		projection.near = near;
		projection.far = far;
	}

	void widthAspect(float width, float aspect) noexcept {
		const float halfWidth = width * 0.5f;
		const float halfHeight = halfWidth / aspect;

		projection.left = -halfWidth;
		projection.right = halfWidth;
		projection.bottom = -halfHeight;
		projection.top = halfHeight;
	}

	void heightAspect(float height, float aspect) noexcept {
		const float halfHeight = height * 0.5f;
		const float halfWidth = halfHeight * aspect;

		projection.left = -halfWidth;
		projection.right = halfWidth;
		projection.bottom = -halfHeight;
		projection.top = halfHeight;
	}

	glm::mat4 project() const noexcept {
		glm::mat4 result = glm::orthoRH_ZO(projection.left, projection.right, projection.bottom, projection.top, projection.near, projection.far);
		result[1][1] *= -1.0f;

		return result;
	}
};
