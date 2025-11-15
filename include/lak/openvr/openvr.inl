#include "lak/openvr/openvr.hpp"

#include <ostream>

namespace lak
{
	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::ETrackedPropertyError err)
	{
		switch (err)
		{
			case vr::ETrackedPropertyError::TrackedProp_Success:
				return strm << "TrackedProp_Success";
			case vr::ETrackedPropertyError::TrackedProp_WrongDataType:
				return strm << "TrackedProp_WrongDataType";
			case vr::ETrackedPropertyError::TrackedProp_WrongDeviceClass:
				return strm << "TrackedProp_WrongDeviceClass";
			case vr::ETrackedPropertyError::TrackedProp_BufferTooSmall:
				return strm << "TrackedProp_BufferTooSmall";
			case vr::ETrackedPropertyError::TrackedProp_UnknownProperty:
				return strm << "TrackedProp_UnknownProperty";
			case vr::ETrackedPropertyError::TrackedProp_InvalidDevice:
				return strm << "TrackedProp_InvalidDevice";
			case vr::ETrackedPropertyError::TrackedProp_CouldNotContactServer:
				return strm << "TrackedProp_CouldNotContactServer";
			case vr::ETrackedPropertyError::TrackedProp_ValueNotProvidedByDevice:
				return strm << "TrackedProp_ValueNotProvidedByDevice";
			case vr::ETrackedPropertyError::TrackedProp_StringExceedsMaximumLength:
				return strm << "TrackedProp_StringExceedsMaximumLength";
			case vr::ETrackedPropertyError::TrackedProp_NotYetAvailable:
				return strm << "TrackedProp_NotYetAvailable";
			case vr::ETrackedPropertyError::TrackedProp_PermissionDenied:
				return strm << "TrackedProp_PermissionDenied";
			case vr::ETrackedPropertyError::TrackedProp_InvalidOperation:
				return strm << "TrackedProp_InvalidOperation";
			case vr::ETrackedPropertyError::TrackedProp_CannotWriteToWildcards:
				return strm << "TrackedProp_CannotWriteToWildcards";
			case vr::ETrackedPropertyError::TrackedProp_IPCReadFailure:
				return strm << "TrackedProp_IPCReadFailure";
			case vr::ETrackedPropertyError::TrackedProp_OutOfMemory:
				return strm << "TrackedProp_OutOfMemory";
			case vr::ETrackedPropertyError::TrackedProp_InvalidContainer:
				return strm << "TrackedProp_InvalidContainer";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EHDCPError err)
	{
		switch (err)
		{
			case vr::EHDCPError::HDCPError_None:
				return strm << "HDCPError_None";
			case vr::EHDCPError::HDCPError_LinkLost:
				return strm << "HDCPError_LinkLost";
			case vr::EHDCPError::HDCPError_Tampered:
				return strm << "HDCPError_Tampered";
			case vr::EHDCPError::HDCPError_DeviceRevoked:
				return strm << "HDCPError_DeviceRevoked";
			case vr::EHDCPError::HDCPError_Unknown:
				return strm << "HDCPError_Unknown";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVRInputError err)
	{
		switch (err)
		{
			case vr::EVRInputError::VRInputError_None:
				return strm << "VRInputError_None";
			case vr::EVRInputError::VRInputError_NameNotFound:
				return strm << "VRInputError_NameNotFound";
			case vr::EVRInputError::VRInputError_WrongType:
				return strm << "VRInputError_WrongType";
			case vr::EVRInputError::VRInputError_InvalidHandle:
				return strm << "VRInputError_InvalidHandle";
			case vr::EVRInputError::VRInputError_InvalidParam:
				return strm << "VRInputError_InvalidParam";
			case vr::EVRInputError::VRInputError_NoSteam:
				return strm << "VRInputError_NoSteam";
			case vr::EVRInputError::VRInputError_MaxCapacityReached:
				return strm << "VRInputError_MaxCapacityReached";
			case vr::EVRInputError::VRInputError_IPCError:
				return strm << "VRInputError_IPCError";
			case vr::EVRInputError::VRInputError_NoActiveActionSet:
				return strm << "VRInputError_NoActiveActionSet";
			case vr::EVRInputError::VRInputError_InvalidDevice:
				return strm << "VRInputError_InvalidDevice";
			case vr::EVRInputError::VRInputError_InvalidSkeleton:
				return strm << "VRInputError_InvalidSkeleton";
			case vr::EVRInputError::VRInputError_InvalidBoneCount:
				return strm << "VRInputError_InvalidBoneCount";
			case vr::EVRInputError::VRInputError_InvalidCompressedData:
				return strm << "VRInputError_InvalidCompressedData";
			case vr::EVRInputError::VRInputError_NoData:
				return strm << "VRInputError_NoData";
			case vr::EVRInputError::VRInputError_BufferTooSmall:
				return strm << "VRInputError_BufferTooSmall";
			case vr::EVRInputError::VRInputError_MismatchedActionManifest:
				return strm << "VRInputError_MismatchedActionManifest";
			case vr::EVRInputError::VRInputError_MissingSkeletonData:
				return strm << "VRInputError_MissingSkeletonData";
			case vr::EVRInputError::VRInputError_InvalidBoneIndex:
				return strm << "VRInputError_InvalidBoneIndex";
			case vr::EVRInputError::VRInputError_InvalidPriority:
				return strm << "VRInputError_InvalidPriority";
			case vr::EVRInputError::VRInputError_PermissionDenied:
				return strm << "VRInputError_PermissionDenied";
			case vr::EVRInputError::VRInputError_InvalidRenderModel:
				return strm << "VRInputError_InvalidRenderModel";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVRSpatialAnchorError err)
	{
		switch (err)
		{
			case vr::EVRSpatialAnchorError::VRSpatialAnchorError_Success:
				return strm << "VRSpatialAnchorError_Success";
			case vr::EVRSpatialAnchorError::VRSpatialAnchorError_Internal:
				return strm << "VRSpatialAnchorError_Internal";
			case vr::EVRSpatialAnchorError::VRSpatialAnchorError_UnknownHandle:
				return strm << "VRSpatialAnchorError_UnknownHandle";
			case vr::EVRSpatialAnchorError::VRSpatialAnchorError_ArrayTooSmall:
				return strm << "VRSpatialAnchorError_ArrayTooSmall";
			case vr::EVRSpatialAnchorError::
			  VRSpatialAnchorError_InvalidDescriptorChar:
				return strm << "VRSpatialAnchorError_InvalidDescriptorChar";
			case vr::EVRSpatialAnchorError::VRSpatialAnchorError_NotYetAvailable:
				return strm << "VRSpatialAnchorError_NotYetAvailable";
			case vr::EVRSpatialAnchorError::
			  VRSpatialAnchorError_NotAvailableInThisUniverse:
				return strm << "VRSpatialAnchorError_NotAvailableInThisUniverse";
			case vr::EVRSpatialAnchorError::
			  VRSpatialAnchorError_PermanentlyUnavailable:
				return strm << "VRSpatialAnchorError_PermanentlyUnavailable";
			case vr::EVRSpatialAnchorError::VRSpatialAnchorError_WrongDriver:
				return strm << "VRSpatialAnchorError_WrongDriver";
			case vr::EVRSpatialAnchorError::VRSpatialAnchorError_DescriptorTooLong:
				return strm << "VRSpatialAnchorError_DescriptorTooLong";
			case vr::EVRSpatialAnchorError::VRSpatialAnchorError_Unknown:
				return strm << "VRSpatialAnchorError_Unknown";
			case vr::EVRSpatialAnchorError::VRSpatialAnchorError_NoRoomCalibration:
				return strm << "VRSpatialAnchorError_NoRoomCalibration";
			case vr::EVRSpatialAnchorError::VRSpatialAnchorError_InvalidArgument:
				return strm << "VRSpatialAnchorError_InvalidArgument";
			case vr::EVRSpatialAnchorError::VRSpatialAnchorError_UnknownDriver:
				return strm << "VRSpatialAnchorError_UnknownDriver";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVROverlayError err)
	{
		switch (err)
		{
			case vr::EVROverlayError::VROverlayError_None:
				return strm << "VROverlayError_None";
			case vr::EVROverlayError::VROverlayError_UnknownOverlay:
				return strm << "VROverlayError_UnknownOverlay";
			case vr::EVROverlayError::VROverlayError_InvalidHandle:
				return strm << "VROverlayError_InvalidHandle";
			case vr::EVROverlayError::VROverlayError_PermissionDenied:
				return strm << "VROverlayError_PermissionDenied";
			case vr::EVROverlayError::VROverlayError_OverlayLimitExceeded:
				return strm << "VROverlayError_OverlayLimitExceeded";
			case vr::EVROverlayError::VROverlayError_WrongVisibilityType:
				return strm << "VROverlayError_WrongVisibilityType";
			case vr::EVROverlayError::VROverlayError_KeyTooLong:
				return strm << "VROverlayError_KeyTooLong";
			case vr::EVROverlayError::VROverlayError_NameTooLong:
				return strm << "VROverlayError_NameTooLong";
			case vr::EVROverlayError::VROverlayError_KeyInUse:
				return strm << "VROverlayError_KeyInUse";
			case vr::EVROverlayError::VROverlayError_WrongTransformType:
				return strm << "VROverlayError_WrongTransformType";
			case vr::EVROverlayError::VROverlayError_InvalidTrackedDevice:
				return strm << "VROverlayError_InvalidTrackedDevice";
			case vr::EVROverlayError::VROverlayError_InvalidParameter:
				return strm << "VROverlayError_InvalidParameter";
			case vr::EVROverlayError::VROverlayError_ThumbnailCantBeDestroyed:
				return strm << "VROverlayError_ThumbnailCantBeDestroyed";
			case vr::EVROverlayError::VROverlayError_ArrayTooSmall:
				return strm << "VROverlayError_ArrayTooSmall";
			case vr::EVROverlayError::VROverlayError_RequestFailed:
				return strm << "VROverlayError_RequestFailed";
			case vr::EVROverlayError::VROverlayError_InvalidTexture:
				return strm << "VROverlayError_InvalidTexture";
			case vr::EVROverlayError::VROverlayError_UnableToLoadFile:
				return strm << "VROverlayError_UnableToLoadFile";
			case vr::EVROverlayError::VROverlayError_KeyboardAlreadyInUse:
				return strm << "VROverlayError_KeyboardAlreadyInUse";
			case vr::EVROverlayError::VROverlayError_NoNeighbor:
				return strm << "VROverlayError_NoNeighbor";
			case vr::EVROverlayError::VROverlayError_TooManyMaskPrimitives:
				return strm << "VROverlayError_TooManyMaskPrimitives";
			case vr::EVROverlayError::VROverlayError_BadMaskPrimitive:
				return strm << "VROverlayError_BadMaskPrimitive";
			case vr::EVROverlayError::VROverlayError_TextureAlreadyLocked:
				return strm << "VROverlayError_TextureAlreadyLocked";
			case vr::EVROverlayError::VROverlayError_TextureLockCapacityReached:
				return strm << "VROverlayError_TextureLockCapacityReached";
			case vr::EVROverlayError::VROverlayError_TextureNotLocked:
				return strm << "VROverlayError_TextureNotLocked";
			case vr::EVROverlayError::VROverlayError_TimedOut:
				return strm << "VROverlayError_TimedOut";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVRFirmwareError err)
	{
		switch (err)
		{
			case vr::EVRFirmwareError::VRFirmwareError_None:
				return strm << "VRFirmwareError_None";
			case vr::EVRFirmwareError::VRFirmwareError_Success:
				return strm << "VRFirmwareError_Success";
			case vr::EVRFirmwareError::VRFirmwareError_Fail:
				return strm << "VRFirmwareError_Fail";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVRNotificationError err)
	{
		switch (err)
		{
			case vr::EVRNotificationError::VRNotificationError_OK:
				return strm << "VRNotificationError_OK";
			case vr::EVRNotificationError::VRNotificationError_InvalidNotificationId:
				return strm << "VRNotificationError_InvalidNotificationId";
			case vr::EVRNotificationError::VRNotificationError_NotificationQueueFull:
				return strm << "VRNotificationError_NotificationQueueFull";
			case vr::EVRNotificationError::VRNotificationError_InvalidOverlayHandle:
				return strm << "VRNotificationError_InvalidOverlayHandle";
			case vr::EVRNotificationError::
			  VRNotificationError_SystemWithUserValueAlreadyExists:
				return strm << "VRNotificationError_SystemWithUserValueAlreadyExists";
			case vr::EVRNotificationError::VRNotificationError_ServiceUnavailable:
				return strm << "VRNotificationError_ServiceUnavailable";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVRInitError err)
	{
		return strm << vr::VR_GetVRInitErrorAsSymbol(err) << "("
		            << vr::VR_GetVRInitErrorAsEnglishDescription(err) << ")";
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVRTrackedCameraError err)
	{
		switch (err)
		{
			case vr::EVRTrackedCameraError::VRTrackedCameraError_None:
				return strm << "VRTrackedCameraError_None";
			case vr::EVRTrackedCameraError::VRTrackedCameraError_OperationFailed:
				return strm << "VRTrackedCameraError_OperationFailed";
			case vr::EVRTrackedCameraError::VRTrackedCameraError_InvalidHandle:
				return strm << "VRTrackedCameraError_InvalidHandle";
			case vr::EVRTrackedCameraError::
			  VRTrackedCameraError_InvalidFrameHeaderVersion:
				return strm << "VRTrackedCameraError_InvalidFrameHeaderVersion";
			case vr::EVRTrackedCameraError::VRTrackedCameraError_OutOfHandles:
				return strm << "VRTrackedCameraError_OutOfHandles";
			case vr::EVRTrackedCameraError::VRTrackedCameraError_IPCFailure:
				return strm << "VRTrackedCameraError_IPCFailure";
			case vr::EVRTrackedCameraError::
			  VRTrackedCameraError_NotSupportedForThisDevice:
				return strm << "VRTrackedCameraError_NotSupportedForThisDevice";
			case vr::EVRTrackedCameraError::VRTrackedCameraError_SharedMemoryFailure:
				return strm << "VRTrackedCameraError_SharedMemoryFailure";
			case vr::EVRTrackedCameraError::
			  VRTrackedCameraError_FrameBufferingFailure:
				return strm << "VRTrackedCameraError_FrameBufferingFailure";
			case vr::EVRTrackedCameraError::VRTrackedCameraError_StreamSetupFailure:
				return strm << "VRTrackedCameraError_StreamSetupFailure";
			case vr::EVRTrackedCameraError::VRTrackedCameraError_InvalidGLTextureId:
				return strm << "VRTrackedCameraError_InvalidGLTextureId";
			case vr::EVRTrackedCameraError::
			  VRTrackedCameraError_InvalidSharedTextureHandle:
				return strm << "VRTrackedCameraError_InvalidSharedTextureHandle";
			case vr::EVRTrackedCameraError::
			  VRTrackedCameraError_FailedToGetGLTextureId:
				return strm << "VRTrackedCameraError_FailedToGetGLTextureId";
			case vr::EVRTrackedCameraError::
			  VRTrackedCameraError_SharedTextureFailure:
				return strm << "VRTrackedCameraError_SharedTextureFailure";
			case vr::EVRTrackedCameraError::VRTrackedCameraError_NoFrameAvailable:
				return strm << "VRTrackedCameraError_NoFrameAvailable";
			case vr::EVRTrackedCameraError::VRTrackedCameraError_InvalidArgument:
				return strm << "VRTrackedCameraError_InvalidArgument";
			case vr::EVRTrackedCameraError::
			  VRTrackedCameraError_InvalidFrameBufferSize:
				return strm << "VRTrackedCameraError_InvalidFrameBufferSize";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVRApplicationError err)
	{
		switch (err)
		{
			case vr::EVRApplicationError::VRApplicationError_None:
				return strm << "VRApplicationError_None";
			case vr::EVRApplicationError::VRApplicationError_AppKeyAlreadyExists:
				return strm << "VRApplicationError_AppKeyAlreadyExists";
			case vr::EVRApplicationError::VRApplicationError_NoManifest:
				return strm << "VRApplicationError_NoManifest";
			case vr::EVRApplicationError::VRApplicationError_NoApplication:
				return strm << "VRApplicationError_NoApplication";
			case vr::EVRApplicationError::VRApplicationError_InvalidIndex:
				return strm << "VRApplicationError_InvalidIndex";
			case vr::EVRApplicationError::VRApplicationError_UnknownApplication:
				return strm << "VRApplicationError_UnknownApplication";
			case vr::EVRApplicationError::VRApplicationError_IPCFailed:
				return strm << "VRApplicationError_IPCFailed";
			case vr::EVRApplicationError::
			  VRApplicationError_ApplicationAlreadyRunning:
				return strm << "VRApplicationError_ApplicationAlreadyRunning";
			case vr::EVRApplicationError::VRApplicationError_InvalidManifest:
				return strm << "VRApplicationError_InvalidManifest";
			case vr::EVRApplicationError::VRApplicationError_InvalidApplication:
				return strm << "VRApplicationError_InvalidApplication";
			case vr::EVRApplicationError::VRApplicationError_LaunchFailed:
				return strm << "VRApplicationError_LaunchFailed";
			case vr::EVRApplicationError::
			  VRApplicationError_ApplicationAlreadyStarting:
				return strm << "VRApplicationError_ApplicationAlreadyStarting";
			case vr::EVRApplicationError::VRApplicationError_LaunchInProgress:
				return strm << "VRApplicationError_LaunchInProgress";
			case vr::EVRApplicationError::VRApplicationError_OldApplicationQuitting:
				return strm << "VRApplicationError_OldApplicationQuitting";
			case vr::EVRApplicationError::VRApplicationError_TransitionAborted:
				return strm << "VRApplicationError_TransitionAborted";
			case vr::EVRApplicationError::VRApplicationError_IsTemplate:
				return strm << "VRApplicationError_IsTemplate";
			case vr::EVRApplicationError::VRApplicationError_SteamVRIsExiting:
				return strm << "VRApplicationError_SteamVRIsExiting";
			case vr::EVRApplicationError::VRApplicationError_BufferTooSmall:
				return strm << "VRApplicationError_BufferTooSmall";
			case vr::EVRApplicationError::VRApplicationError_PropertyNotSet:
				return strm << "VRApplicationError_PropertyNotSet";
			case vr::EVRApplicationError::VRApplicationError_UnknownProperty:
				return strm << "VRApplicationError_UnknownProperty";
			case vr::EVRApplicationError::VRApplicationError_InvalidParameter:
				return strm << "VRApplicationError_InvalidParameter";
			case vr::EVRApplicationError::VRApplicationError_NotImplemented:
				return strm << "VRApplicationError_NotImplemented";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVRCompositorError err)
	{
		switch (err)
		{
			case vr::EVRCompositorError::VRCompositorError_None:
				return strm << "VRCompositorError_None";
			case vr::EVRCompositorError::VRCompositorError_RequestFailed:
				return strm << "VRCompositorError_RequestFailed";
			case vr::EVRCompositorError::VRCompositorError_IncompatibleVersion:
				return strm << "VRCompositorError_IncompatibleVersion";
			case vr::EVRCompositorError::VRCompositorError_DoNotHaveFocus:
				return strm << "VRCompositorError_DoNotHaveFocus";
			case vr::EVRCompositorError::VRCompositorError_InvalidTexture:
				return strm << "VRCompositorError_InvalidTexture";
			case vr::EVRCompositorError::VRCompositorError_IsNotSceneApplication:
				return strm << "VRCompositorError_IsNotSceneApplication";
			case vr::EVRCompositorError::VRCompositorError_TextureIsOnWrongDevice:
				return strm << "VRCompositorError_TextureIsOnWrongDevice";
			case vr::EVRCompositorError::
			  VRCompositorError_TextureUsesUnsupportedFormat:
				return strm << "VRCompositorError_TextureUsesUnsupportedFormat";
			case vr::EVRCompositorError::
			  VRCompositorError_SharedTexturesNotSupported:
				return strm << "VRCompositorError_SharedTexturesNotSupported";
			case vr::EVRCompositorError::VRCompositorError_IndexOutOfRange:
				return strm << "VRCompositorError_IndexOutOfRange";
			case vr::EVRCompositorError::VRCompositorError_AlreadySubmitted:
				return strm << "VRCompositorError_AlreadySubmitted";
			case vr::EVRCompositorError::VRCompositorError_InvalidBounds:
				return strm << "VRCompositorError_InvalidBounds";
			case vr::EVRCompositorError::VRCompositorError_AlreadySet:
				return strm << "VRCompositorError_AlreadySet";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVRRenderModelError err)
	{
		switch (err)
		{
			case vr::EVRRenderModelError::VRRenderModelError_None:
				return strm << "VRRenderModelError_None";
			case vr::EVRRenderModelError::VRRenderModelError_Loading:
				return strm << "VRRenderModelError_Loading";
			case vr::EVRRenderModelError::VRRenderModelError_NotSupported:
				return strm << "VRRenderModelError_NotSupported";
			case vr::EVRRenderModelError::VRRenderModelError_InvalidArg:
				return strm << "VRRenderModelError_InvalidArg";
			case vr::EVRRenderModelError::VRRenderModelError_InvalidModel:
				return strm << "VRRenderModelError_InvalidModel";
			case vr::EVRRenderModelError::VRRenderModelError_NoShapes:
				return strm << "VRRenderModelError_NoShapes";
			case vr::EVRRenderModelError::VRRenderModelError_MultipleShapes:
				return strm << "VRRenderModelError_MultipleShapes";
			case vr::EVRRenderModelError::VRRenderModelError_TooManyVertices:
				return strm << "VRRenderModelError_TooManyVertices";
			case vr::EVRRenderModelError::VRRenderModelError_MultipleTextures:
				return strm << "VRRenderModelError_MultipleTextures";
			case vr::EVRRenderModelError::VRRenderModelError_BufferTooSmall:
				return strm << "VRRenderModelError_BufferTooSmall";
			case vr::EVRRenderModelError::VRRenderModelError_NotEnoughNormals:
				return strm << "VRRenderModelError_NotEnoughNormals";
			case vr::EVRRenderModelError::VRRenderModelError_NotEnoughTexCoords:
				return strm << "VRRenderModelError_NotEnoughTexCoords";
			case vr::EVRRenderModelError::VRRenderModelError_InvalidTexture:
				return strm << "VRRenderModelError_InvalidTexture";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVRSettingsError err)
	{
		switch (err)
		{
			case vr::EVRSettingsError::VRSettingsError_None:
				return strm << "VRSettingsError_None";
			case vr::EVRSettingsError::VRSettingsError_IPCFailed:
				return strm << "VRSettingsError_IPCFailed";
			case vr::EVRSettingsError::VRSettingsError_WriteFailed:
				return strm << "VRSettingsError_WriteFailed";
			case vr::EVRSettingsError::VRSettingsError_ReadFailed:
				return strm << "VRSettingsError_ReadFailed";
			case vr::EVRSettingsError::VRSettingsError_JsonParseFailed:
				return strm << "VRSettingsError_JsonParseFailed";
			case vr::EVRSettingsError::VRSettingsError_UnsetSettingHasNoDefault:
				return strm << "VRSettingsError_UnsetSettingHasNoDefault";
			case vr::EVRSettingsError::VRSettingsError_AccessDenied:
				return strm << "VRSettingsError_AccessDenied";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVRScreenshotError err)
	{
		switch (err)
		{
			case vr::EVRScreenshotError::VRScreenshotError_None:
				return strm << "VRScreenshotError_None";
			case vr::EVRScreenshotError::VRScreenshotError_RequestFailed:
				return strm << "VRScreenshotError_RequestFailed";
			case vr::EVRScreenshotError::VRScreenshotError_IncompatibleVersion:
				return strm << "VRScreenshotError_IncompatibleVersion";
			case vr::EVRScreenshotError::VRScreenshotError_NotFound:
				return strm << "VRScreenshotError_NotFound";
			case vr::EVRScreenshotError::VRScreenshotError_BufferTooSmall:
				return strm << "VRScreenshotError_BufferTooSmall";
			case vr::EVRScreenshotError::
			  VRScreenshotError_ScreenshotAlreadyInProgress:
				return strm << "VRScreenshotError_ScreenshotAlreadyInProgress";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EIOBufferError err)
	{
		switch (err)
		{
			case vr::EIOBufferError::IOBuffer_Success:
				return strm << "IOBuffer_Success";
			case vr::EIOBufferError::IOBuffer_OperationFailed:
				return strm << "IOBuffer_OperationFailed";
			case vr::EIOBufferError::IOBuffer_InvalidHandle:
				return strm << "IOBuffer_InvalidHandle";
			case vr::EIOBufferError::IOBuffer_InvalidArgument:
				return strm << "IOBuffer_InvalidArgument";
			case vr::EIOBufferError::IOBuffer_PathExists:
				return strm << "IOBuffer_PathExists";
			case vr::EIOBufferError::IOBuffer_PathDoesNotExist:
				return strm << "IOBuffer_PathDoesNotExist";
			case vr::EIOBufferError::IOBuffer_Permission:
				return strm << "IOBuffer_Permission";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}

	template<typename CHAR>
	std::basic_ostream<CHAR> &operator<<(std::basic_ostream<CHAR> &strm,
	                                     vr::EVRDebugError err)
	{
		switch (err)
		{
			case vr::EVRDebugError::VRDebugError_Success:
				return strm << "VRDebugError_Success";
			case vr::EVRDebugError::VRDebugError_BadParameter:
				return strm << "VRDebugError_BadParameter";
			default:
				return strm << lak::strconv<CHAR>(
				         lak::streamify(static_cast<uintmax_t>(err)));
		}
	}
}
