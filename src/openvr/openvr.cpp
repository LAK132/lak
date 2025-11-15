#include "lak/openvr/openvr.hpp"

lak::error_code<vr::EVRInitError> lak::openvr::init(
  vr::EVRApplicationType app_type, const char *startup_info)
{
	vr::EVRInitError init_error = vr::VRInitError_None;
	vr::VR_Init(&init_error, app_type, startup_info);
	return lak::openvr::as_result(init_error);
}

lak::error_code<vr::ETrackedPropertyError> lak::openvr::as_result(
  vr::ETrackedPropertyError err)
{
	if (err == vr::ETrackedPropertyError::TrackedProp_Success)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EHDCPError> lak::openvr::as_result(vr::EHDCPError err)
{
	if (err == vr::EHDCPError::HDCPError_None)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVRInputError> lak::openvr::as_result(
  vr::EVRInputError err)
{
	if (err == vr::EVRInputError::VRInputError_None)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVRSpatialAnchorError> lak::openvr::as_result(
  vr::EVRSpatialAnchorError err)
{
	if (err == vr::EVRSpatialAnchorError::VRSpatialAnchorError_Success)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVROverlayError> lak::openvr::as_result(
  vr::EVROverlayError err)
{
	if (err == vr::EVROverlayError::VROverlayError_None)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVRFirmwareError> lak::openvr::as_result(
  vr::EVRFirmwareError err)
{
	if (err == vr::EVRFirmwareError::VRFirmwareError_None)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVRNotificationError> lak::openvr::as_result(
  vr::EVRNotificationError err)
{
	if (err == vr::EVRNotificationError::VRNotificationError_OK)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVRInitError> lak::openvr::as_result(vr::EVRInitError err)
{
	if (err == vr::EVRInputError::VRInputError_None)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVRTrackedCameraError> lak::openvr::as_result(
  vr::EVRTrackedCameraError err)
{
	if (err == vr::EVRTrackedCameraError::VRTrackedCameraError_None)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVRApplicationError> lak::openvr::as_result(
  vr::EVRApplicationError err)
{
	if (err == vr::EVRApplicationError::VRApplicationError_None)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVRCompositorError> lak::openvr::as_result(
  vr::EVRCompositorError err)
{
	if (err == vr::EVRCompositorError::VRCompositorError_None)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVRRenderModelError> lak::openvr::as_result(
  vr::EVRRenderModelError err)
{
	if (err == vr::EVRRenderModelError::VRRenderModelError_None)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVRSettingsError> lak::openvr::as_result(
  vr::EVRSettingsError err)
{
	if (err == vr::EVRSettingsError::VRSettingsError_None)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVRScreenshotError> lak::openvr::as_result(
  vr::EVRScreenshotError err)
{
	if (err == vr::EVRScreenshotError::VRScreenshotError_None)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EIOBufferError> lak::openvr::as_result(
  vr::EIOBufferError err)
{
	if (err == vr::EIOBufferError::IOBuffer_Success)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

lak::error_code<vr::EVRDebugError> lak::openvr::as_result(
  vr::EVRDebugError err)
{
	if (err == vr::EVRDebugError::VRDebugError_Success)
		return lak::ok_t{};
	else
		return lak::err_t{err};
}

#ifdef LAK_ENABLE_GLM
glm::vec2 lak::openvr::to_glm(const vr::HmdVector2_t &v)
{
	glm::vec2 result;
	for (size_t i = 0; i < 2; ++i) result[i] = v.v[i];
	return result;
}

glm::vec3 lak::openvr::to_glm(const vr::HmdVector3_t &v)
{
	glm::vec3 result;
	for (size_t i = 0; i < 3; ++i) result[i] = v.v[i];
	return result;
}

glm::vec4 lak::openvr::to_glm(const vr::HmdVector4_t &v)
{
	glm::vec4 result;
	for (size_t i = 0; i < 4; ++i) result[i] = v.v[i];
	return result;
}

glm::mat3x3 lak::openvr::to_glm(const vr::HmdMatrix33_t &m)
{
	glm::mat3x3 result;
	for (size_t i = 0; i < 3; ++i)
		for (size_t j = 0; j < 3; ++j) result[i][j] = m.m[j][i];
	return result;
}

glm::mat4x3 lak::openvr::to_glm(const vr::HmdMatrix34_t &m)
{
	glm::mat4x3 result;
	for (size_t i = 0; i < 4; ++i)
		for (size_t j = 0; j < 3; ++j) result[i][j] = m.m[j][i];
	return result;
}

glm::mat4x4 lak::openvr::to_glm(const vr::HmdMatrix44_t &m)
{
	glm::mat4x4 result;
	for (size_t i = 0; i < 4; ++i)
		for (size_t j = 0; j < 4; ++j) result[i][j] = m.m[j][i];
	return result;
}

vr::HmdVector2_t lak::openvr::to_hmd(const glm::vec2 &v)
{
	vr::HmdVector2_t result;
	for (size_t i = 0; i < 2; ++i) result.v[i] = v[i];
	return result;
}

vr::HmdVector3_t lak::openvr::to_hmd(const glm::vec3 &v)
{
	vr::HmdVector3_t result;
	for (size_t i = 0; i < 3; ++i) result.v[i] = v[i];
	return result;
}

vr::HmdVector4_t lak::openvr::to_hmd(const glm::vec4 &v)
{
	vr::HmdVector4_t result;
	for (size_t i = 0; i < 4; ++i) result.v[i] = v[i];
	return result;
}

vr::HmdMatrix33_t lak::openvr::to_hmd(const glm::mat3x3 &m)
{
	vr::HmdMatrix33_t result;
	for (size_t i = 0; i < 3; ++i)
		for (size_t j = 0; j < 3; ++j) result.m[i][j] = m[j][i];
	return result;
}

vr::HmdMatrix34_t lak::openvr::to_hmd(const glm::mat4x3 &m)
{
	vr::HmdMatrix34_t result;
	for (size_t i = 0; i < 3; ++i)
		for (size_t j = 0; j < 4; ++j) result.m[i][j] = m[j][i];
	return result;
}

vr::HmdMatrix44_t lak::openvr::to_hmd(const glm::mat4x4 &m)
{
	vr::HmdMatrix44_t result;
	for (size_t i = 0; i < 4; ++i)
		for (size_t j = 0; j < 4; ++j) result.m[i][j] = m[j][i];
	return result;
}
#endif
