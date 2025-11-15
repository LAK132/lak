#ifndef LAK_OPENVR_OPENVR_HPP
#define LAK_OPENVR_OPENVR_HPP

#include "lak/result.hpp"

#include <openvr.h>

#ifdef LAK_ENABLE_GLM
#	include <glm/vec2.hpp>
#	include <glm/vec3.hpp>
#	include <glm/vec4.hpp>
#	include <glm/mat3x3.hpp>
#	include <glm/mat3x4.hpp>
#	include <glm/mat4x4.hpp>
#endif

namespace lak
{
	namespace openvr
	{
		lak::error_code<vr::EVRInitError> init(vr::EVRApplicationType app_type,
		                                       const char *startup_info = nullptr);

		lak::error_code<vr::ETrackedPropertyError> as_result(
		  vr::ETrackedPropertyError err);
		lak::error_code<vr::EHDCPError> as_result(vr::EHDCPError err);
		lak::error_code<vr::EVRInputError> as_result(vr::EVRInputError err);
		lak::error_code<vr::EVRSpatialAnchorError> as_result(
		  vr::EVRSpatialAnchorError err);
		lak::error_code<vr::EVROverlayError> as_result(vr::EVROverlayError err);
		lak::error_code<vr::EVRFirmwareError> as_result(vr::EVRFirmwareError err);
		lak::error_code<vr::EVRNotificationError> as_result(
		  vr::EVRNotificationError err);
		lak::error_code<vr::EVRInitError> as_result(vr::EVRInitError err);
		lak::error_code<vr::EVRTrackedCameraError> as_result(
		  vr::EVRTrackedCameraError err);
		lak::error_code<vr::EVRApplicationError> as_result(
		  vr::EVRApplicationError err);
		lak::error_code<vr::EVRCompositorError> as_result(
		  vr::EVRCompositorError err);
		lak::error_code<vr::EVRRenderModelError> as_result(
		  vr::EVRRenderModelError err);
		lak::error_code<vr::EVRSettingsError> as_result(vr::EVRSettingsError err);
		lak::error_code<vr::EVRScreenshotError> as_result(
		  vr::EVRScreenshotError err);
		lak::error_code<vr::EIOBufferError> as_result(vr::EIOBufferError err);
		lak::error_code<vr::EVRDebugError> as_result(vr::EVRDebugError err);

#ifdef LAK_ENABLE_GLM
		glm::vec2 to_glm(const vr::HmdVector2_t &v);
		glm::vec3 to_glm(const vr::HmdVector3_t &v);
		glm::vec4 to_glm(const vr::HmdVector4_t &v);
		glm::mat3x3 to_glm(const vr::HmdMatrix33_t &m);
		glm::mat4x3 to_glm(const vr::HmdMatrix34_t &m);
		glm::mat4x4 to_glm(const vr::HmdMatrix44_t &m);

		vr::HmdVector2_t to_hmd(const glm::vec2 &v);
		vr::HmdVector3_t to_hmd(const glm::vec3 &v);
		vr::HmdVector4_t to_hmd(const glm::vec4 &v);
		vr::HmdMatrix33_t to_hmd(const glm::mat3x3 &m);
		vr::HmdMatrix34_t to_hmd(const glm::mat4x3 &m);
		vr::HmdMatrix44_t to_hmd(const glm::mat4x4 &m);
#endif
	}
}

#include "lak/openvr/openvr.inl"

#endif
