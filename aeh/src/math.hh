#pragma once

#ifdef AEH_WITH_GLM

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/constants.hpp>
#include "glm/gtc/quaternion.hpp"
#include <type_traits>

constexpr auto pi = glm::pi<float>();
constexpr auto half_pi = glm::half_pi<float>();
constexpr auto two_pi = glm::two_pi<float>();

namespace aeh::math
{
	constexpr float default_epsilon = 0.0001f;

	constexpr glm::vec3 x_axis = { 1.0f, 0.0f, 0.0f };
	constexpr glm::vec3 y_axis = { 0.0f, 1.0f, 0.0f };
	constexpr glm::vec3 z_axis = { 0.0f, 0.0f, 1.0f };

	//! Returns the first three coordinates of a 4 dimensional vector
	[[nodiscard]] glm::vec3 xyz(const glm::vec4 & v) noexcept;

	[[nodiscard]] float length_sq(const glm::vec3 & vector) noexcept;
	[[nodiscard]] float dist_sq(const glm::vec3 & a, const glm::vec3 & b) noexcept;

	template <typename Real1, typename Real2, typename = std::enable_if_t<std::is_floating_point<Real1>::value && std::is_floating_point<Real2>::value>>
	[[nodiscard]] constexpr bool float_equal(Real1 left, Real2 right, float epsilon = math::default_epsilon) noexcept;

	template <typename Real, typename = std::enable_if_t<std::is_floating_point<Real>::value>>
	[[nodiscard]] constexpr bool float_zero(Real value, float epsilon = math::default_epsilon) noexcept;

	// Float equal overloads
	[[nodiscard]] bool float_equal(const glm::vec2 & a, const glm::vec2 & b, float epsilon = math::default_epsilon) noexcept;
	[[nodiscard]] bool float_equal(const glm::vec3 & a, const glm::vec3 & b, float epsilon = math::default_epsilon) noexcept;
	[[nodiscard]] bool float_equal(const glm::quat & q1, const glm::quat & q2, float epsilon = math::default_epsilon) noexcept;

	[[nodiscard]] bool float_zero(const glm::vec2 & vector, float epsilon = math::default_epsilon) noexcept;
	[[nodiscard]] bool float_zero(const glm::vec3 & vector, float epsilon = math::default_epsilon) noexcept;
	[[nodiscard]] bool float_zero(const glm::quat & quaternion, float epsilon = math::default_epsilon) noexcept;

	[[nodiscard]] glm::vec3 to_radians(const glm::vec3 & euler_angles_in_degrees) noexcept;
	[[nodiscard]] glm::vec3 to_degrees(const glm::vec3 & euler_angles_in_radians) noexcept;

	[[nodiscard]] glm::quat rotate_x(const glm::quat & rotation, float angle) noexcept;
	[[nodiscard]] glm::quat rotate_y(const glm::quat & rotation, float angle) noexcept;
	[[nodiscard]] glm::quat rotate_z(const glm::quat & rotation, float angle) noexcept;
	[[nodiscard]] glm::quat axis_angle(const glm::quat & rotation, float angle, const glm::vec3 axis) noexcept;
	[[nodiscard]] glm::vec3 rotate_around(const glm::vec3 & point, const glm::vec3 & center, const glm::quat & rot) noexcept;
	[[nodiscard]] glm::vec3 rotate_around(const glm::vec3 & point, const glm::vec3 & center, const glm::vec3 & axis, float angle) noexcept;

	[[nodiscard]] glm::vec3 transform_point(const glm::vec3 & point, const glm::mat4 & transformation) noexcept;
	[[nodiscard]] glm::vec3 transform_vector(const glm::vec3 & point, const glm::mat4 & transformation) noexcept;

	//! Returns a quaternion representing the rotation for
	//! an object in position 'from' to be looking at
	//! another object in position 'to', by respecting the 'up' vector
	[[nodiscard]] glm::quat look_at(const glm::vec3 & from, const glm::vec3 & to, const glm::vec3 & up) noexcept;

	[[nodiscard]] bool is_normalized(const glm::vec2 & vector, float epsilon = math::default_epsilon) noexcept;
	[[nodiscard]] bool is_normalized(const glm::vec3 & vector, float epsilon = math::default_epsilon) noexcept;
	[[nodiscard]] bool is_normalized(const glm::vec4 & vector, float epsilon = math::default_epsilon) noexcept;

	[[nodiscard]] bool are_perpendicular(const glm::vec2 & a, const glm::vec2 & b, float epsilon = math::default_epsilon) noexcept;
	[[nodiscard]] bool are_perpendicular(const glm::vec3 & a, const glm::vec3 & b, float epsilon = math::default_epsilon) noexcept;
	[[nodiscard]] bool are_perpendicular(const glm::vec4 & a, const glm::vec4 & b, float epsilon = math::default_epsilon) noexcept;

	//! Returns a vector perpendicular to v and with length equal to length(v).
	[[nodiscard]] glm::vec3 any_perpendicular(const glm::vec3 & v) noexcept;

	//! Returns negative if below, zero if inside and positive if above.
	[[nodiscard]] float position_relative_to_plane(const glm::vec3 & point, const glm::vec3 & plane_point, const glm::vec3 & plane_normal);

	//! True if val is in the range [min, max].
	template <typename T>
	[[nodiscard]] bool is_in_range(const T & val, const T & min, const T & max) noexcept;

	//! Linear interpolation between start and end at time t.
	template <typename T>
	[[nodiscard]] T lerp(const T & start, const T & end, float t) noexcept;

	//! Given the start and end of a lerp and a value x, return the t such that lerp(start, end, t) == x.
	template <std::floating_point T>
	[[nodiscard]] T inverse_lerp(const T & start, const T & end, const T & x) noexcept;

	//! Compute the position in a bezier curve of 2, 3 or 4 points at t.
	template <typename T, std::floating_point F> [[nodiscard]] constexpr T bezier(T const & a, T const & b, F t) noexcept;
	template <typename T, std::floating_point F> [[nodiscard]] constexpr float bezier(T const & a, T const & b, T const & c, F t) noexcept;
	template <typename T, std::floating_point F> [[nodiscard]] constexpr float bezier(T const & a, T const & b, T const & c, T const & d, F t) noexcept;

	template <typename Point>
	[[nodiscard]] Point reflect_point(const Point& point, const Point& ref) noexcept;

	//! Increments an index in a circular array so that it wraps around when it goes out of bounds.
	[[nodiscard]] int inc_circular(int i, int size) noexcept;
	[[nodiscard]] int dec_circular(int i, int size) noexcept;

	//! Returns dividend / divisor. If remainder is not 0, rounds up instead of down.
	[[nodiscard]] int div_round_up(int dividend, int divisor) noexcept;

	//! Steer value towards target by step.
	[[nodiscard]] float steer_towards(float value, float target, float step) noexcept;

	//! Makes x be a multiple of divisor, always truncating the value downwards.
	float truncate_to_multiple_of(float x, float divisor);

	//! Represents a 3D rotation in spherical coordinates.
	//! Zenith is vector (0, 1, 0) and azimuth is vector (1, 0, 0).
	//! Latitude is the angle between the forward vector and the zenith, in the range [-pi, pi].
	//! Longitude is the angle between the projection of the forward vector in the XZ plane and the azimuth, in the range [0, 2 * pi].
	//! From the azimuth, an increase in the longitude represents a rotation towards the positive z.
	struct SphereCoords
	{
		static constexpr glm::vec3 zenith = {0, 1, 0};
		static constexpr glm::vec3 azimuth = {1, 0, 0};

		float latitude;
		float longitude;
	};

	//! Returns the forward vector for the given orientation.
	[[nodiscard]] glm::vec3 forward(SphereCoords c) noexcept;
	//! Returns the up vector for the given orientation.
	[[nodiscard]] glm::vec3 up(SphereCoords c) noexcept;
	//! Returns the lateral vector for the given orientation.
	[[nodiscard]] glm::vec3 lateral(SphereCoords c) noexcept;
	//! Returns the spherical coordinates associated to the given vector.
	//! This is the inverse function of forward(SphereCoords).
	[[nodiscard]] SphereCoords spherical_coordinates(const glm::vec3 & direction) noexcept;

	//! True if c1 and c2 are almost equal (difference is less than epsilon).
	[[nodiscard]] bool float_equal(const SphereCoords & c1, const SphereCoords & c2, float epsilon = math::default_epsilon) noexcept;
	//! True if latitude and longitude are almost zero (in the range [-epsilon, epsilon]).
	[[nodiscard]] bool float_zero(const SphereCoords & c, float epsilon = math::default_epsilon) noexcept;

	namespace matrix
	{

		[[nodiscard]] glm::mat4 translate(const glm::vec3 & pos) noexcept;
		[[nodiscard]] glm::mat4 translate(float x, float y, float z) noexcept;
		[[nodiscard]] glm::mat4 scale(const glm::vec3 & size) noexcept;
		[[nodiscard]] glm::mat4 scale(float x, float y, float z) noexcept;
		[[nodiscard]] glm::mat4 uniform_scale(float s) noexcept;
		[[nodiscard]] glm::mat3 scale_mat3(const glm::vec3 & size) noexcept;
		[[nodiscard]] glm::mat3 scale_mat3(float x, float y, float z) noexcept;
		[[nodiscard]] glm::mat3 uniform_scale_mat3(float s) noexcept;
		[[nodiscard]] glm::mat4 rotate(const glm::quat & rot) noexcept;
		//! Vector may not be normalized or perpendicular. Will do internal work to ensure correctness the same way glm::lookAt does.
		[[nodiscard]] glm::mat3 rotate(const glm::vec3 & eye, const glm::vec3 & center, const glm::vec3 & up) noexcept;
		//! Vectors must be normalized and perpendicular.
		[[nodiscard]] glm::mat3 rotate(const glm::vec3 & forward, const glm::vec3 & up) noexcept;
		[[nodiscard]] glm::mat3 rotate(SphereCoords c) noexcept;
		// Angle in radians
		[[nodiscard]] glm::mat3 rotate_x(float angle) noexcept;
		[[nodiscard]] glm::mat3 rotate_y(float angle) noexcept;
		[[nodiscard]] glm::mat3 rotate_z(float angle) noexcept;
		[[nodiscard]] glm::mat3 axis_angle(const glm::vec3 & axis, float angle) noexcept;

		[[nodiscard]] glm::mat4 model(const glm::vec3 & pos, const glm::vec3 & size) noexcept;
		[[nodiscard]] glm::mat4 model(const glm::vec3 & pos, const glm::vec3 & size, const glm::quat & rot) noexcept;
		[[nodiscard]] glm::mat4 model_no_scale(const glm::vec3 & pos, const glm::quat & rot) noexcept;
		[[nodiscard]] glm::mat4 model_billboard(const glm::vec3 & pos, float z_rot, const glm::vec2 & size, const glm::mat3 & cam_rot) noexcept;

		[[nodiscard]] glm::mat4 inv_model(const glm::vec3 & pos, const glm::vec3 & size) noexcept;
		[[nodiscard]] glm::mat4 inv_model(const glm::vec3 & pos, const glm::vec3 & size, const glm::quat & rot) noexcept;
		[[nodiscard]] glm::mat4 inv_model_no_scale(const glm::vec3 & pos, const glm::quat & rot) noexcept;

		[[nodiscard]] glm::mat4 shadow(const glm::vec3 & light_pos, const glm::vec3 & light_dir, float angle, float radius) noexcept;

	} // namespace matrix
} // namespace math

namespace aeh::math
{
	template <typename Real1, typename Real2, typename>
	constexpr bool float_equal(Real1 left, Real2 right, float epsilon) noexcept
	{
		using common_type = std::common_type_t<Real1, Real2>;

		return std::abs(static_cast<common_type>(left) - static_cast<common_type>(right)) < static_cast<common_type>(epsilon);
	}

	template <typename Real, typename>
	constexpr bool float_zero(Real value, float epsilon) noexcept
	{
		return float_equal(value, static_cast<Real>(0.0f), epsilon);
	}

	template <typename T>
	bool is_in_range(const T & val, const T & min, const T & max) noexcept
	{
		return (val >= min) && (val <= max);
	}

	template <typename T> requires std::integral<T> || std::random_access_iterator<T>
	T circular_clamp(T val, const T & min, const T & max) noexcept
	{
		const auto interval = (max - min) + 1;

		while (val > max)
			val -= interval;
		while (val < min)
			val += interval;

		return val;
	}

	template <std::floating_point T>
	T circular_clamp(T val, const T & min, const T & max) noexcept
	{
		const T interval = max - min;

		while (val > max)
			val -= interval;
		while (val < min)
			val += interval;

		return val;
	}

	template <typename T>
	T lerp(const T & start, const T & end, float t) noexcept
	{
		return start + (end - start) * t;
	}

	template <std::floating_point T>
	T inverse_lerp(const T & start, const T & end, const T & x) noexcept
	{
		return (x - start) / (end - start);
	}

	template <typename T, std::floating_point F>
	[[nodiscard]] constexpr T bezier(T const & a, T const & b, F t) noexcept
	{
		return a + (b - a) * t;
	}

	template <typename T, std::floating_point F>
	[[nodiscard]] constexpr float bezier(T const & a, T const & b, T const & c, F t) noexcept
	{
		return bezier(bezier(a, b, t), bezier(b, c, t), t);
	}

	template <typename T, std::floating_point F>
	[[nodiscard]] constexpr float bezier(T const & a, T const & b, T const & c, T const & d, F t) noexcept
	{
		return bezier(bezier(a, b, c, t), bezier(b, c, d, t), t);
	}

	template <typename Point>
	Point reflect_point(const Point & point, const Point & ref) noexcept
	{
		const Point v = ref - point;
		return ref + v;
	}

} // namespace math

#endif
