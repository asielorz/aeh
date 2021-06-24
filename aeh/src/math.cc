#ifdef AEH_WITH_GLM

#include "math.hh"
#include "debug/assert.hh"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <algorithm>

namespace aeh::math
{

	glm::vec3 xyz(const glm::vec4 & v) noexcept
	{
		return { v.x, v.y, v.z };
	}

	float length_sq(const glm::vec3 & vector) noexcept
	{
		return glm::dot(vector, vector);
	}

	float dist_sq(const glm::vec3 & a, const glm::vec3 & b) noexcept
	{
		return length_sq(b - a);
	}

	bool float_equal(const glm::vec2 & a, const glm::vec2 & b, float epsilon) noexcept
	{
		return float_equal(a.x, b.x, epsilon) && float_equal(a.y, b.y, epsilon);
	}

	bool float_equal(const glm::vec3 & a, const glm::vec3 & b, float epsilon) noexcept
	{
		return
			float_equal(a.x, b.x, epsilon) &&
			float_equal(a.y, b.y, epsilon) &&
			float_equal(a.z, b.z, epsilon);
	}

	bool float_equal(const glm::quat & q1, const glm::quat & q2, float epsilon) noexcept
	{
		return float_equal(q1.x, q2.x, epsilon) && float_equal(q1.y, q2.y, epsilon) && float_equal(q1.z, q2.z, epsilon) && float_equal(q1.w, q2.w, epsilon);
	}

	bool float_zero(const glm::vec2 & vector, float epsilon) noexcept
	{
		return float_equal(vector, glm::vec2{}, epsilon);
	}

	bool float_zero(const glm::vec3 & vector, float epsilon) noexcept
	{
		return float_equal(vector, glm::vec3{}, epsilon);
	}

	bool float_zero(const glm::quat & quaternion, float epsilon) noexcept
	{
		return float_equal(quaternion, glm::quat{}, epsilon);
	}

	glm::vec3 to_radians(const glm::vec3 & euler_angles_in_degrees) noexcept
	{
		return {
			glm::radians(euler_angles_in_degrees.x),
			glm::radians(euler_angles_in_degrees.y),
			glm::radians(euler_angles_in_degrees.z)
		};
	}

	glm::vec3 to_degrees(const glm::vec3 & euler_angles_in_radians) noexcept
	{
		return {
			glm::degrees(euler_angles_in_radians.x),
			glm::degrees(euler_angles_in_radians.y),
			glm::degrees(euler_angles_in_radians.z)
		};
	}

	glm::quat rotate_x(const glm::quat & rotation, float angle) noexcept
	{
		return axis_angle(rotation, angle, { 1, 0, 0 });
	}

	glm::quat rotate_y(const glm::quat & rotation, float angle) noexcept
	{
		return axis_angle(rotation, angle, { 0, 1, 0 });
	}

	glm::quat rotate_z(const glm::quat & rotation, float angle) noexcept
	{
		return axis_angle(rotation, angle, { 0, 0, 1 });
	}

	glm::quat axis_angle(const glm::quat & rotation, float angle, const glm::vec3 axis) noexcept
	{
		return glm::rotate(rotation, angle, axis);
	}

	glm::vec3 rotate_around(const glm::vec3 & point, const glm::vec3 & center, const glm::quat & rot) noexcept
	{
		const glm::vec3 pt_local = point - center;
		return glm::mat3_cast(rot) * pt_local + center;
	}

	glm::vec3 rotate_around(const glm::vec3 & point, const glm::vec3 & center, const glm::vec3 & axis, float angle) noexcept
	{
		return rotate_around(point, center, glm::rotate(glm::quat{}, angle, axis));
	}

	glm::vec3 transform_point(const glm::vec3 & point, const glm::mat4 & transformation) noexcept
	{
		return glm::vec3(transformation * glm::vec4(point, 1));
	}

	glm::vec3 transform_vector(const glm::vec3 & point, const glm::mat4 & transformation) noexcept
	{
		return glm::vec3(transformation * glm::vec4(point, 0));
	}

	glm::quat look_at(const glm::vec3 & from, const glm::vec3 & to, const glm::vec3 & up) noexcept
	{
		assert(!float_equal(from, to));
		const auto forward = glm::normalize(to - from);
		const auto left = glm::cross(up, forward);

		// The rotation matrix using the 3 vectors from the object's local basis
		// NOTE: glm's matrices are column major
		glm::mat3 rotation_matrix;
		rotation_matrix[0][0] = left.x;		rotation_matrix[0][1] = left.y;		rotation_matrix[0][2] = left.z;
		rotation_matrix[1][0] = up.x;		rotation_matrix[1][1] = up.y;		rotation_matrix[1][2] = up.z;
		rotation_matrix[2][0] = forward.x;	rotation_matrix[2][1] = forward.y;	rotation_matrix[2][2] = forward.z;

		return glm::quat_cast(rotation_matrix);
	}

	bool is_normalized(const glm::vec2 & vector, float epsilon) noexcept
	{
		return float_equal(glm::length(vector), 1.0f, epsilon);
	}

	bool is_normalized(const glm::vec3 & vector, float epsilon) noexcept
	{
		return float_equal(glm::length(vector), 1.0f, epsilon);
	}

	bool is_normalized(const glm::vec4 & vector, float epsilon) noexcept
	{
		return float_equal(glm::length(vector), 1.0f, epsilon);
	}

	bool are_perpendicular(const glm::vec2 & a, const glm::vec2 & b, float epsilon) noexcept
	{
		return math::float_zero(glm::dot(a, b), epsilon);
	}

	bool are_perpendicular(const glm::vec3 & a, const glm::vec3 & b, float epsilon) noexcept
	{
		return math::float_zero(glm::dot(a, b), epsilon);
	}

	bool are_perpendicular(const glm::vec4 & a, const glm::vec4 & b, float epsilon) noexcept
	{
		return math::float_zero(glm::dot(a, b), epsilon);
	}

	glm::vec3 any_perpendicular(const glm::vec3 & v) noexcept
	{
		const float abs_vals[] = { std::abs(v.x), std::abs(v.y), std::abs(v.z) };
		const auto min = std::min_element(abs_vals, abs_vals + 3) - abs_vals;

		if (min == 0)
			return math::matrix::rotate_x(half_pi) * v;
		else if (min == 1)
			return math::matrix::rotate_y(half_pi) * v;
		else
			return math::matrix::rotate_z(half_pi) * v;
	}

	float position_relative_to_plane(const glm::vec3 & point, const glm::vec3 & plane_point, const glm::vec3 & plane_normal)
	{
		const float d = glm::dot(plane_point, plane_normal);
		return glm::dot(point, plane_normal) - d;
	}

	int inc_circular(int i, int size) noexcept
	{
		++i;
		if (i >= size)
			i = 0;
		return i;
	}

	int dec_circular(int i, int size) noexcept
	{
		--i;
		if (i < 0)
			i = size - 1;
		return i;
	}

	int div_round_up(int dividend, int divisor) noexcept
	{
		auto const [result, remainder] = std::div(dividend, divisor);
		return result + (remainder > 0);
	}

	glm::vec3 forward(SphereCoords c) noexcept
	{
		return {
			cos(c.longitude) * sin(c.latitude),
			cos(c.latitude),
			sin(c.longitude) * sin(c.latitude)
		};
	}

	glm::vec3 up(SphereCoords c) noexcept
	{
		c.latitude -= half_pi;
		return forward(c);
	}

	glm::vec3 lateral(SphereCoords c) noexcept
	{
		return glm::cross(up(c), forward(c));
	}

	SphereCoords spherical_coordinates(const glm::vec3 & direction) noexcept
	{
		// Precondition test
		debug_assert(is_normalized(direction));

		const float latitude = acos(direction.y);
		const float longitude = acos(direction.x / sin(latitude));

		// Postcondition test
		debug_assert(float_equal(sin(longitude) * sin(latitude), direction.z));

		return { latitude, longitude };
	}

	bool float_equal(const SphereCoords & c1, const SphereCoords & c2, float epsilon) noexcept
	{
		return 
			float_equal(c1.latitude, c2.latitude, epsilon) &&
			float_equal(c1.longitude, c2.longitude, epsilon);
	}

	bool float_zero(const SphereCoords & c, float epsilon) noexcept
	{
		return float_zero(c.latitude, epsilon) && float_zero(c.longitude, epsilon);
	}

	namespace matrix
	{

		glm::mat4 translate(const glm::vec3 & pos) noexcept
		{
			return glm::mat4(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, 1, 0,
				pos.x, pos.y, pos.z, 1
			);
		}

		glm::mat4 translate(float x, float y, float z) noexcept
		{
			return translate({ x, y, z });
		}

		glm::mat4 scale(const glm::vec3 & size) noexcept
		{
			return glm::mat4(
				size.x,      0,      0, 0,
				     0, size.y,      0, 0,
				     0,      0, size.z, 0,
				     0,      0,      0, 1
			);
		}

		glm::mat4 scale(float x, float y, float z) noexcept
		{
			return scale({x, y, z});
		}

		glm::mat4 uniform_scale(float s) noexcept
		{
			return scale(s, s, s);
		}

		glm::mat3 scale_mat3(const glm::vec3 & size) noexcept
		{
			return glm::mat3(
				size.x,      0,      0,
				     0, size.y,      0,
				     0,      0, size.z
			);
		}

		glm::mat3 scale_mat3(float x, float y, float z) noexcept
		{
			return scale_mat3({x, y, z});
		}

		glm::mat3 uniform_scale_mat3(float s) noexcept
		{
			return scale_mat3(s, s, s);
		}

		glm::mat4 rotate(const glm::quat & rot) noexcept
		{
			return glm::mat4_cast(rot);
		}

		glm::mat3 rotate(const glm::vec3 & eye, const glm::vec3 & center, const glm::vec3 & up) noexcept
		{
			const glm::vec3 f(glm::normalize(center - eye));
			const glm::vec3 s(glm::normalize(cross(f, up)));
			const glm::vec3 u(glm::cross(s, f));

			return { s, u, f };
		}

		glm::mat3 rotate(const glm::vec3 & forward, const glm::vec3 & up) noexcept
		{
			debug_assert(is_normalized(forward));
			debug_assert(is_normalized(up));
			debug_assert(are_perpendicular(forward, up));

			const glm::vec3 lateral = glm::cross(up, forward);
			return { lateral, up, forward };
		}

		glm::mat3 rotate(SphereCoords c) noexcept
		{
			const glm::vec3 f = forward(c);
			const glm::vec3 u = up(c);
			const glm::vec3 l = glm::cross(u, f);

			return { l, u, f };
		}

		glm::mat3 rotate_x(float x) noexcept
		{
			return {
				1.0f,    0.0f,   0.0f,
				0.0f,  cos(x), sin(x),
				0.0f, -sin(x), cos(x)
			};
		}

		glm::mat3 rotate_y(float x) noexcept
		{
			return {
				cos(x), 0.0f, -sin(x),
				  0.0f, 1.0f,    0.0f,
				sin(x), 0.0f,  cos(x)
			};
		}

		glm::mat3 rotate_z(float x) noexcept
		{
			return {
				 cos(x), sin(x), 0.0f,
				-sin(x), cos(x), 0.0f,
				   0.0f,   0.0f, 1.0f
			};
		}

		glm::mat3 axis_angle(const glm::vec3 & axis, float angle) noexcept
		{
			const glm::quat q = glm::angleAxis(angle, axis);
			return glm::mat3_cast(q);
		}

		glm::mat4 model(const glm::vec3 & pos, const glm::vec3 & size) noexcept
		{
			return translate(pos) * scale(size);
		}

		glm::mat4 model(const glm::vec3 & pos, const glm::vec3 & size, const glm::quat & rot) noexcept
		{
			return model_no_scale(pos, rot) * scale(size);
		}

		glm::mat4 model_no_scale(const glm::vec3 & pos, const glm::quat & rot) noexcept
		{
			return translate(pos) * rotate(rot);
		}

		glm::mat4 model_billboard(const glm::vec3 & pos, float z_rot, const glm::vec2 & size, const glm::mat3 & cam_rot) noexcept
		{
			return
				math::matrix::translate(pos) *
				glm::mat4(cam_rot *
				math::matrix::rotate_z(z_rot) *
				math::matrix::scale_mat3(size.x, size.y, 1));
		}

		glm::mat4 inv_model(const glm::vec3 & pos, const glm::vec3 & size) noexcept
		{
			return scale({ 1 / size.x, 1 / size.y, 1 / size.z }) * translate(-pos);
		}

		glm::mat4 inv_model(const glm::vec3 & pos, const glm::vec3 & size, const glm::quat & rot) noexcept
		{
			return scale({ 1 / size.x, 1 / size.y, 1 / size.z }) * rotate(glm::inverse(rot)) * translate(-pos);
		}

		glm::mat4 inv_model_no_scale(const glm::vec3 & pos, const glm::quat & rot) noexcept
		{
			return rotate(glm::inverse(rot)) * translate(-pos);
		}

		glm::mat4 shadow(const glm::vec3 & light_pos, const glm::vec3 & light_dir, float angle, float radius) noexcept
		{
			const glm::mat4 bias = {
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.5f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f
			};

			const glm::mat4 light_cam = glm::lookAt(light_pos, light_pos + light_dir, glm::vec3(0, 1, 0));
			const glm::mat4 light_proj = glm::perspective(2 * angle, 1.0f, 0.1f, radius);
			return bias * light_proj * light_cam;
		}

	} // namespace matrix
} // namespace math

#endif
