#include "space.hpp"

#include "lak/span_manip.hpp"

#include <imgui.h>

#include <glm/ext/matrix_transform.hpp>

#include <numbers>

reference_frame::~reference_frame()
{
	for (auto &child : children) child->parent = nullptr;
}

lak::shared_ptr<reference_frame> reference_frame::add_child()
{
	return children.emplace_back(
	  lak::shared_ptr<reference_frame>::make(reference_frame{.parent = this}));
}

void reference_frame::erase(decltype(children)::const_iterator child)
{
	children.erase(child);
}

void reference_frame::erase(const reference_frame *child)
{
	erase(lak::find_if(children.begin(),
	                   children.end(),
	                   [&child](const auto &frame)
	                   { return frame.get() == child; }));
}

void reference_frame::update(float delta)
{
	rotation.velocity += rotation.acceleration * delta;
	rotation.value += rotation.velocity * delta;
	constexpr float tau = 2.f * std::numbers::pi_v<float>;
	rotation.value.x    = lak::fslack(-rotation.value.x, tau);
	rotation.value.y    = lak::fslack(-rotation.value.y, tau);
	rotation.value.z    = lak::fslack(-rotation.value.z, tau);

	translation.velocity += translation.acceleration * delta;
	translation.value += translation.velocity * delta;

	scale.velocity += scale.acceleration * delta;
	scale.value += scale.velocity * delta;
}

glm::mat4 reference_frame::get_parent() const
{
	return parent ? parent->get_transform() : glm::mat4(1.0f);
}

glm::mat4 reference_frame::get_transform() const
{
	auto trans = glm::translate(get_parent(), translation.value);
	trans      = glm::rotate(trans, rotation.value.z, glm::vec3(0, 0, 1));
	trans      = glm::rotate(trans, rotation.value.y, glm::vec3(0, 1, 0));
	trans      = glm::rotate(trans, rotation.value.x, glm::vec3(1, 0, 0));
	return glm::scale(trans, scale.value);
}

glm::vec3 reference_frame::total_translation() const
{
	auto trans = glm::translate(get_parent(), translation.value);
	return glm::vec3(trans[3]);
}

void reference_frame::view(float speed)
{
	if (ImGui::TreeNode("translation"))
	{
		ImGui::DragFloat3("value", &translation.value[0], speed);
		ImGui::DragFloat3("velocity", &translation.velocity[0], speed);
		ImGui::DragFloat3("acceleration", &translation.acceleration[0], speed);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("rotation"))
	{
		auto _speed = speed * glm::pi<float>() / 180.0f;
		ImGui::DragFloat3("value", &rotation.value[0], _speed);
		ImGui::DragFloat3("velocity", &rotation.velocity[0], _speed);
		ImGui::DragFloat3("acceleration", &rotation.acceleration[0], _speed);
		ImGui::TreePop();
	}

	if (ImGui::TreeNode("scale"))
	{
		auto _speed = speed * 0.1f;
		ImGui::DragFloat3("value", &scale.value[0], _speed);
		ImGui::DragFloat3("velocity", &scale.velocity[0], _speed);
		ImGui::DragFloat3("acceleration", &scale.acceleration[0], _speed);
		ImGui::TreePop();
	}
}
