#ifndef SPACE_HPP
#define SPACE_HPP

#include <lak/array.hpp>
#include <lak/memory.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct delta_transform
{
	glm::vec3 value        = glm::vec3(0.0);
	glm::vec3 velocity     = glm::vec3(0.0);
	glm::vec3 acceleration = glm::vec3(0.0);
};

struct reference_frame
{
	reference_frame *parent = nullptr;
	lak::array<lak::shared_ptr<reference_frame>> children;

	delta_transform translation;
	delta_transform rotation;
	delta_transform scale = {.value = glm::vec3(1.0)};

	~reference_frame();

	lak::shared_ptr<reference_frame> add_child();

	void erase(decltype(children)::const_iterator child);
	void erase(const reference_frame *child);

	void update(float delta);

	glm::mat4 get_parent() const;
	glm::mat4 get_transform() const;

	glm::vec3 total_translation() const;

	void view(float speed = 1.f);
};

#endif
