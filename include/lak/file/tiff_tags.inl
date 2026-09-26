#include "lak/format_traits.hpp"
#include "lak/stdint.hpp"
#include "lak/type_pack.hpp"

#define LAK_FOREACH_TIFF_TAG_VERSION(MACRO, ...)                              \
	MACRO(TIFF_6, "TIFF 6", __VA_ARGS__)                                        \
	MACRO(TIFF_EP, "TIFF/EP", __VA_ARGS__)                                      \
	MACRO(EXIF, "Exif", __VA_ARGS__)                                            \
	MACRO(DNG_1_1_0_0, "DNG 1.1.0.0", __VA_ARGS__)                              \
	MACRO(DNG_1_2_0_0, "DNG 1.2.0.0", __VA_ARGS__)                              \
	MACRO(DNG_1_3_0_0, "DNG 1.3.0.0", __VA_ARGS__)                              \
	MACRO(DNG_1_4_0_0, "DNG 1.4.0.0", __VA_ARGS__)                              \
	MACRO(DNG_1_5_0_0, "DNG 1.5.0.0", __VA_ARGS__)                              \
	MACRO(DNG_1_6_0_0, "DNG 1.6.0.0", __VA_ARGS__)                              \
	MACRO(DNG_1_7_0_0, "DNG 1.7.0.0", __VA_ARGS__)                              \
	MACRO(DNG_1_7_1_0, "DNG 1.7.1.0", __VA_ARGS__)

namespace lak
{
	namespace tiff
	{
		enum struct tiff_version
		{
#define LAK_TIFF_TAG_VERSION(NAME, ...) NAME,
			LAK_FOREACH_TIFF_TAG_VERSION(LAK_TIFF_TAG_VERSION)
#undef LAK_TIFF_TAG_VERSION
		};
	}

	template<typename CHAR>
	struct format_traits<lak::tiff::tiff_version, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::tiff::tiff_version &version)
		{
			switch (version)
			{
#define LAK_TIFF_TAG_VERSION(NAME, STR, ...)                                  \
	case lak::tiff::tiff_version::NAME: return lak::strconv<CHAR>(STR ""_view);
				LAK_FOREACH_TIFF_TAG_VERSION(LAK_TIFF_TAG_VERSION)
#undef LAK_TIFF_TAG_VERSION
			}
			return {};
		}
	};
}

#define LAK_FOREACH_TIFF6_TAG_NAME(MACRO, ...)                                \
	MACRO(NewSubfileType, __VA_ARGS__)                                          \
	MACRO(SubfileType, __VA_ARGS__)                                             \
	MACRO(ImageWidth, __VA_ARGS__)                                              \
	MACRO(ImageLength, __VA_ARGS__)                                             \
	MACRO(BitsPerSample, __VA_ARGS__)                                           \
	MACRO(Compression, __VA_ARGS__)                                             \
	MACRO(PhotometricInterpretation, __VA_ARGS__)                               \
	MACRO(Thresholding, __VA_ARGS__)                                            \
	MACRO(CellWidth, __VA_ARGS__)                                               \
	MACRO(CellLength, __VA_ARGS__)                                              \
	MACRO(FillOrder, __VA_ARGS__)                                               \
	MACRO(DocumentName, __VA_ARGS__)                                            \
	MACRO(ImageDescription, __VA_ARGS__)                                        \
	MACRO(Make, __VA_ARGS__)                                                    \
	MACRO(Model, __VA_ARGS__)                                                   \
	MACRO(StripOffsets, __VA_ARGS__)                                            \
	MACRO(Orientation, __VA_ARGS__)                                             \
	MACRO(SamplesPerPixel, __VA_ARGS__)                                         \
	MACRO(RowsPerStrip, __VA_ARGS__)                                            \
	MACRO(StripByteCounts, __VA_ARGS__)                                         \
	MACRO(MinSampleValue, __VA_ARGS__)                                          \
	MACRO(MaxSampleValue, __VA_ARGS__)                                          \
	MACRO(XResolution, __VA_ARGS__)                                             \
	MACRO(YResolution, __VA_ARGS__)                                             \
	MACRO(PlanarConfiguration, __VA_ARGS__)                                     \
	MACRO(PageName, __VA_ARGS__)                                                \
	MACRO(XPosition, __VA_ARGS__)                                               \
	MACRO(YPosition, __VA_ARGS__)                                               \
	MACRO(FreeOffsets, __VA_ARGS__)                                             \
	MACRO(FreeByteCounts, __VA_ARGS__)                                          \
	MACRO(GrayResponseUnit, __VA_ARGS__)                                        \
	MACRO(GrayResponseCurve, __VA_ARGS__)                                       \
	MACRO(T4Options, __VA_ARGS__)                                               \
	MACRO(T6Options, __VA_ARGS__)                                               \
	MACRO(ResolutionUnit, __VA_ARGS__)                                          \
	MACRO(PageNumber, __VA_ARGS__)                                              \
	MACRO(TransferFunction, __VA_ARGS__)                                        \
	MACRO(Software, __VA_ARGS__)                                                \
	MACRO(DateTime, __VA_ARGS__)                                                \
	MACRO(Artist, __VA_ARGS__)                                                  \
	MACRO(HostComputer, __VA_ARGS__)                                            \
	MACRO(Predictor, __VA_ARGS__)                                               \
	MACRO(WhitePoint, __VA_ARGS__)                                              \
	MACRO(PrimaryChromaticities, __VA_ARGS__)                                   \
	MACRO(ColorMap, __VA_ARGS__)                                                \
	MACRO(HalftoneHints, __VA_ARGS__)                                           \
	MACRO(TileWidth, __VA_ARGS__)                                               \
	MACRO(TileLength, __VA_ARGS__)                                              \
	MACRO(TileOffsets, __VA_ARGS__)                                             \
	MACRO(TileByteCounts, __VA_ARGS__)                                          \
	MACRO(InkSet, __VA_ARGS__)                                                  \
	MACRO(InkNames, __VA_ARGS__)                                                \
	MACRO(NumberOfInks, __VA_ARGS__)                                            \
	MACRO(DotRange, __VA_ARGS__)                                                \
	MACRO(TargetPrinter, __VA_ARGS__)                                           \
	MACRO(ExtraSamples, __VA_ARGS__)                                            \
	MACRO(SampleFormat, __VA_ARGS__)                                            \
	MACRO(SMinSampleValue, __VA_ARGS__)                                         \
	MACRO(SMaxSampleValue, __VA_ARGS__)                                         \
	MACRO(TransferRange, __VA_ARGS__)                                           \
	MACRO(JPEGProc, __VA_ARGS__)                                                \
	MACRO(JPEGInterchangeFormat, __VA_ARGS__)                                   \
	MACRO(JPEGInterchangeFormatLength, __VA_ARGS__)                             \
	MACRO(JPEGRestartInterval, __VA_ARGS__)                                     \
	MACRO(JPEGLosslessPredictors, __VA_ARGS__)                                  \
	MACRO(JPEGPointTransforms, __VA_ARGS__)                                     \
	MACRO(JPEGQTables, __VA_ARGS__)                                             \
	MACRO(JPEGDCTables, __VA_ARGS__)                                            \
	MACRO(JPEGACTables, __VA_ARGS__)                                            \
	MACRO(YCbCrCoefficients, __VA_ARGS__)                                       \
	MACRO(YCbCrSubSampling, __VA_ARGS__)                                        \
	MACRO(YCbCrPositioning, __VA_ARGS__)                                        \
	MACRO(ReferenceBlackWhite, __VA_ARGS__)                                     \
	MACRO(Copyright, __VA_ARGS__)

#define LAK_FOREACH_TIFF_EP_TAG_NAME(MACRO, ...)                              \
	MACRO(SubIFDs, __VA_ARGS__)                                                 \
	MACRO(JPEGTables, __VA_ARGS__)                                              \
	/*MACRO(DCS620_Unknown1,__VA_ARGS__)*/                                      \
	MACRO(CFARepeatPatternDim, __VA_ARGS__)                                     \
	MACRO(CFAPattern, __VA_ARGS__)                                              \
	MACRO(BatteryLevel, __VA_ARGS__)                                            \
	/*MACRO(DCS620_Unknown2,__VA_ARGS__)*/                                      \
	MACRO(ExposureTime, __VA_ARGS__)                                            \
	MACRO(FNumber, __VA_ARGS__)                                                 \
	MACRO(IPTC_NAA, __VA_ARGS__)                                                \
	MACRO(InterColorProfile, __VA_ARGS__)                                       \
	MACRO(ExposureProgram, __VA_ARGS__)                                         \
	MACRO(SpectralSensitivity, __VA_ARGS__)                                     \
	MACRO(GPSInfo, __VA_ARGS__)                                                 \
	MACRO(ISOSpeedRatings, __VA_ARGS__)                                         \
	MACRO(OECF, __VA_ARGS__)                                                    \
	MACRO(Interlace, __VA_ARGS__)                                               \
	MACRO(TimeZoneOffset, __VA_ARGS__)                                          \
	MACRO(SelfTimerMode, __VA_ARGS__)                                           \
	MACRO(DateTimeOriginal, __VA_ARGS__)                                        \
	MACRO(CompressedBitsPerPixel, __VA_ARGS__)                                  \
	MACRO(ShutterSpeedValue, __VA_ARGS__)                                       \
	MACRO(ApertureValue, __VA_ARGS__)                                           \
	MACRO(BrightnessValue, __VA_ARGS__)                                         \
	MACRO(ExposureBiasValue, __VA_ARGS__)                                       \
	MACRO(MaxApertureValue, __VA_ARGS__)                                        \
	MACRO(SubjectDistance, __VA_ARGS__)                                         \
	MACRO(MeteringMode, __VA_ARGS__)                                            \
	MACRO(LightSource, __VA_ARGS__)                                             \
	MACRO(Flash, __VA_ARGS__)                                                   \
	MACRO(FocalLength, __VA_ARGS__)                                             \
	MACRO(FlashEnergy, __VA_ARGS__)                                             \
	MACRO(SpatialFrequencyResponse, __VA_ARGS__)                                \
	MACRO(Noise, __VA_ARGS__)                                                   \
	MACRO(FocalPlaneXResolution, __VA_ARGS__)                                   \
	MACRO(FocalPlaneYResolution, __VA_ARGS__)                                   \
	MACRO(FocalPlaneResolutionUnit, __VA_ARGS__)                                \
	MACRO(ImageNumber, __VA_ARGS__)                                             \
	MACRO(SecurityClassification, __VA_ARGS__)                                  \
	MACRO(ImageHistory, __VA_ARGS__)                                            \
	MACRO(SubjectLocation, __VA_ARGS__)                                         \
	MACRO(ExposureIndex, __VA_ARGS__)                                           \
	MACRO(TIFF_EPStandardID, __VA_ARGS__)                                       \
	MACRO(SensingMethod, __VA_ARGS__)

#define LAK_FOREACH_EXIF_TAG_NAME(MACRO, ...)                                 \
	MACRO(ApplicationNotes, __VA_ARGS__)                                        \
	MACRO(Model2, __VA_ARGS__)                                                  \
	MACRO(KodakIFD, __VA_ARGS__)                                                \
	MACRO(ExifVersion, __VA_ARGS__)                                             \
	MACRO(CreateDate, __VA_ARGS__)                                              \
	MACRO(ComponentsConfiguration, __VA_ARGS__)                                 \
	MACRO(FlashpixVersion, __VA_ARGS__)                                         \
	MACRO(ColorSpace, __VA_ARGS__)                                              \
	MACRO(ExifImageWidth, __VA_ARGS__)                                          \
	MACRO(ExifImageHeight, __VA_ARGS__)                                         \
	MACRO(RelatedSoundFile, __VA_ARGS__)                                        \
	MACRO(InteropOffset, __VA_ARGS__)                                           \
	MACRO(ExifFlashEnergy, __VA_ARGS__)                                         \
	MACRO(ExifExposureIndex, __VA_ARGS__)                                       \
	MACRO(ExifSensingMethod, __VA_ARGS__)                                       \
	MACRO(FileSource, __VA_ARGS__)                                              \
	MACRO(SceneType, __VA_ARGS__)                                               \
	MACRO(CustomRendered, __VA_ARGS__)                                          \
	MACRO(ExposureMode, __VA_ARGS__)                                            \
	MACRO(WhiteBalance, __VA_ARGS__)                                            \
	MACRO(DigitalZoomRatio, __VA_ARGS__)                                        \
	MACRO(FocalLengthIn35mmFormat, __VA_ARGS__)                                 \
	MACRO(SceneCaptureType, __VA_ARGS__)                                        \
	MACRO(GainControl, __VA_ARGS__)                                             \
	MACRO(Contrast, __VA_ARGS__)                                                \
	MACRO(Saturation, __VA_ARGS__)                                              \
	MACRO(Sharpness, __VA_ARGS__)                                               \
	MACRO(SubjectDistanceRange, __VA_ARGS__)                                    \
	MACRO(LensMake, __VA_ARGS__)                                                \
	MACRO(LensModel, __VA_ARGS__)                                               \
	MACRO(LensSerialNumber, __VA_ARGS__)                                        \
	MACRO(ExifOffset, __VA_ARGS__)

#define LAK_FOREACH_DNG_1_1_0_0_TAG_NAME(MACRO, ...)                          \
	MACRO(DNGVersion, __VA_ARGS__)                                              \
	MACRO(DNGBackwardVersion, __VA_ARGS__)                                      \
	MACRO(UniqueCameraModel, __VA_ARGS__)                                       \
	MACRO(LocalizedCameraModel, __VA_ARGS__)                                    \
	MACRO(CFAPlaneColor, __VA_ARGS__)                                           \
	MACRO(CFALayout, __VA_ARGS__)                                               \
	MACRO(LinearizationTable, __VA_ARGS__)                                      \
	MACRO(BlackLevelRepeatDim, __VA_ARGS__)                                     \
	MACRO(BlackLevel, __VA_ARGS__)                                              \
	MACRO(BlackLevelDeltaH, __VA_ARGS__)                                        \
	MACRO(BlackLevelDeltaV, __VA_ARGS__)                                        \
	MACRO(WhiteLevel, __VA_ARGS__)                                              \
	MACRO(DefaultScale, __VA_ARGS__)                                            \
	MACRO(DefaultCropOrigin, __VA_ARGS__)                                       \
	MACRO(DefaultCropSize, __VA_ARGS__)                                         \
	MACRO(ColorMatrix1, __VA_ARGS__)                                            \
	MACRO(ColorMatrix2, __VA_ARGS__)                                            \
	MACRO(CameraCalibration1, __VA_ARGS__)                                      \
	MACRO(CameraCalibration2, __VA_ARGS__)                                      \
	MACRO(ReductionMatrix1, __VA_ARGS__)                                        \
	MACRO(ReductionMatrix2, __VA_ARGS__)                                        \
	MACRO(AnalogBalance, __VA_ARGS__)                                           \
	MACRO(AsShotNeutral, __VA_ARGS__)                                           \
	MACRO(AsShotWhiteXY, __VA_ARGS__)                                           \
	MACRO(BaselineExposure, __VA_ARGS__)                                        \
	MACRO(BaselineNoise, __VA_ARGS__)                                           \
	MACRO(BaselineSharpness, __VA_ARGS__)                                       \
	MACRO(BayerGreenSplit, __VA_ARGS__)                                         \
	MACRO(LinearResponseLimit, __VA_ARGS__)                                     \
	MACRO(CameraSerialNumber, __VA_ARGS__)                                      \
	MACRO(LensInfo, __VA_ARGS__)                                                \
	MACRO(ChromaBlurRadius, __VA_ARGS__)                                        \
	MACRO(AntiAliasStrength, __VA_ARGS__)                                       \
	MACRO(ShadowScale, __VA_ARGS__)                                             \
	MACRO(DNGPrivateData, __VA_ARGS__)                                          \
	MACRO(MakerNoteSafety, __VA_ARGS__)                                         \
	MACRO(CalibrationIlluminant1, __VA_ARGS__)                                  \
	MACRO(CalibrationIlluminant2, __VA_ARGS__)                                  \
	MACRO(BestQualityScale, __VA_ARGS__)                                        \
	MACRO(RawDataUniqueID, __VA_ARGS__)                                         \
	MACRO(OriginalRawFileName, __VA_ARGS__)                                     \
	MACRO(OriginalRawFileData, __VA_ARGS__)                                     \
	MACRO(ActiveArea, __VA_ARGS__)                                              \
	MACRO(MaskedAreas, __VA_ARGS__)                                             \
	MACRO(AsShotICCProfile, __VA_ARGS__)                                        \
	MACRO(AsShotPreProfileMatrix, __VA_ARGS__)                                  \
	MACRO(CurrentICCProfile, __VA_ARGS__)                                       \
	MACRO(CurrentPreProfileMatrix, __VA_ARGS__)

#define LAK_FOREACH_DNG_1_2_0_0_TAG_NAME(MACRO, ...)                          \
	MACRO(ColorimetricReference, __VA_ARGS__)                                   \
	MACRO(CameraCalibrationSignature, __VA_ARGS__)                              \
	MACRO(ProfileCalibrationSignature, __VA_ARGS__)                             \
	MACRO(ExtraCameraProfiles, __VA_ARGS__)                                     \
	MACRO(AsShotProfileName, __VA_ARGS__)                                       \
	MACRO(NoiseReductionApplied, __VA_ARGS__)                                   \
	MACRO(ProfileName, __VA_ARGS__)                                             \
	MACRO(ProfileHueSatMapDims, __VA_ARGS__)                                    \
	MACRO(ProfileHueSatMapData1, __VA_ARGS__)                                   \
	MACRO(ProfileHueSatMapData2, __VA_ARGS__)                                   \
	MACRO(ProfileToneCurve, __VA_ARGS__)                                        \
	MACRO(ProfileEmbedPolicy, __VA_ARGS__)                                      \
	MACRO(ProfileCopyright, __VA_ARGS__)                                        \
	MACRO(ForwardMatrix1, __VA_ARGS__)                                          \
	MACRO(ForwardMatrix2, __VA_ARGS__)                                          \
	MACRO(PreviewApplicationName, __VA_ARGS__)                                  \
	MACRO(PreviewApplicationVersion, __VA_ARGS__)                               \
	MACRO(PreviewSettingName, __VA_ARGS__)                                      \
	MACRO(PreviewSettingDigest, __VA_ARGS__)                                    \
	MACRO(PreviewColorSpace, __VA_ARGS__)                                       \
	MACRO(PreviewDateTime, __VA_ARGS__)                                         \
	MACRO(RawImageDigest, __VA_ARGS__)                                          \
	MACRO(OriginalRawFileDigest, __VA_ARGS__)                                   \
	MACRO(SubTileBlockSize, __VA_ARGS__)                                        \
	MACRO(RowInterleaveFactor, __VA_ARGS__)                                     \
	MACRO(ProfileLookTableDims, __VA_ARGS__)                                    \
	MACRO(ProfileLookTableData, __VA_ARGS__)

#define LAK_FOREACH_DNG_1_3_0_0_TAG_NAME(MACRO, ...)                          \
	MACRO(OpcodeList1, __VA_ARGS__)                                             \
	MACRO(OpcodeList2, __VA_ARGS__)                                             \
	MACRO(OpcodeList3, __VA_ARGS__)                                             \
	MACRO(NoiseProfile, __VA_ARGS__)

#define LAK_FOREACH_DNG_1_4_0_0_TAG_NAME(MACRO, ...)                          \
	MACRO(OriginalDefaultFinalSize, __VA_ARGS__)                                \
	MACRO(OriginalBestQualityFinalSize, __VA_ARGS__)                            \
	MACRO(OriginalDefaultCropSize, __VA_ARGS__)                                 \
	MACRO(DefaultUserCrop, __VA_ARGS__)                                         \
	MACRO(ProfileHueSatMapEncoding, __VA_ARGS__)                                \
	MACRO(ProfileLookTableEncoding, __VA_ARGS__)                                \
	MACRO(BaselineExposureOffset, __VA_ARGS__)                                  \
	MACRO(DefaultBlackRender, __VA_ARGS__)                                      \
	MACRO(NewRawImageDigest, __VA_ARGS__)                                       \
	MACRO(RawToPreviewGain, __VA_ARGS__)

#define LAK_FOREACH_DNG_1_5_0_0_TAG_NAME(MACRO, ...)                          \
	MACRO(DepthFormat, __VA_ARGS__)                                             \
	MACRO(DepthNear, __VA_ARGS__)                                               \
	MACRO(DepthFar, __VA_ARGS__)                                                \
	MACRO(DepthUnits, __VA_ARGS__)                                              \
	MACRO(DepthMeasureType, __VA_ARGS__)                                        \
	MACRO(EnhanceParams, __VA_ARGS__)

#define LAK_FOREACH_DNG_1_6_0_0_TAG_NAME(MACRO, ...)                          \
	MACRO(ProfileGainTableMap, __VA_ARGS__)                                     \
	MACRO(SemanticName, __VA_ARGS__)                                            \
	MACRO(SemanticInstanceID, __VA_ARGS__)                                      \
	MACRO(CalibrationIlluminant3, __VA_ARGS__)                                  \
	MACRO(CameraCalibration3, __VA_ARGS__)                                      \
	MACRO(ColorMatrix3, __VA_ARGS__)                                            \
	MACRO(ForwardMatrix3, __VA_ARGS__)                                          \
	MACRO(IlluminantData1, __VA_ARGS__)                                         \
	MACRO(IlluminantData2, __VA_ARGS__)                                         \
	MACRO(IlluminantData3, __VA_ARGS__)                                         \
	MACRO(MaskSubArea, __VA_ARGS__)                                             \
	MACRO(ProfileHueSatMapData3, __VA_ARGS__)                                   \
	MACRO(ReductionMatrix3, __VA_ARGS__)                                        \
	MACRO(RGBTables, __VA_ARGS__)

#define LAK_FOREACH_DNG_1_7_0_0_TAG_NAME(MACRO, ...)                          \
	MACRO(ProfileGainTableMap2, __VA_ARGS__)                                    \
	MACRO(ImageSequenceInfo, __VA_ARGS__)                                       \
	MACRO(ImageStats, __VA_ARGS__)                                              \
	MACRO(ProfileDynamicRange, __VA_ARGS__)                                     \
	MACRO(ProfileGroupName, __VA_ARGS__)

#define LAK_FOREACH_DNG_1_7_1_0_TAG_NAME(MACRO, ...)                          \
	MACRO(ColumnInterleaveFactor, __VA_ARGS__)                                  \
	MACRO(JXLDistance, __VA_ARGS__)                                             \
	MACRO(JXLEffort, __VA_ARGS__)                                               \
	MACRO(JXLDecodeSpeed, __VA_ARGS__)

#define LAK_FOREACH_DNG_TAG_NAME(MACRO, ...)                                  \
	LAK_FOREACH_DNG_1_1_0_0_TAG_NAME(MACRO, __VA_ARGS__)                        \
	LAK_FOREACH_DNG_1_2_0_0_TAG_NAME(MACRO, __VA_ARGS__)                        \
	LAK_FOREACH_DNG_1_3_0_0_TAG_NAME(MACRO, __VA_ARGS__)                        \
	LAK_FOREACH_DNG_1_4_0_0_TAG_NAME(MACRO, __VA_ARGS__)                        \
	LAK_FOREACH_DNG_1_5_0_0_TAG_NAME(MACRO, __VA_ARGS__)                        \
	LAK_FOREACH_DNG_1_6_0_0_TAG_NAME(MACRO, __VA_ARGS__)                        \
	LAK_FOREACH_DNG_1_7_0_0_TAG_NAME(MACRO, __VA_ARGS__)                        \
	LAK_FOREACH_DNG_1_7_1_0_TAG_NAME(MACRO, __VA_ARGS__)

#define LAK_FOREACH_TIFF_TAG_NAME(MACRO, ...)                                 \
	LAK_FOREACH_TIFF6_TAG_NAME(MACRO, __VA_ARGS__)                              \
	LAK_FOREACH_TIFF_EP_TAG_NAME(MACRO, __VA_ARGS__)                            \
	LAK_FOREACH_EXIF_TAG_NAME(MACRO, __VA_ARGS__)                               \
	LAK_FOREACH_DNG_TAG_NAME(MACRO, __VA_ARGS__)

#define LAK_FOREACH_TIFF6_TAG_VALUE(MACRO, ...)                               \
	MACRO(0xFE, NewSubfileType, TIFF_6, __VA_ARGS__)                            \
	MACRO(0xFF, SubfileType, TIFF_6, __VA_ARGS__)                               \
	MACRO(0x100, ImageWidth, TIFF_6, __VA_ARGS__)                               \
	MACRO(0x101, ImageLength, TIFF_6, __VA_ARGS__)                              \
	MACRO(0x102, BitsPerSample, TIFF_6, __VA_ARGS__)                            \
	MACRO(0x103, Compression, TIFF_6, __VA_ARGS__)                              \
	MACRO(0x106, PhotometricInterpretation, TIFF_6, __VA_ARGS__)                \
	MACRO(0x107, Thresholding, TIFF_6, __VA_ARGS__)                             \
	MACRO(0x108, CellWidth, TIFF_6, __VA_ARGS__)                                \
	MACRO(0x109, CellLength, TIFF_6, __VA_ARGS__)                               \
	MACRO(0x10A, FillOrder, TIFF_6, __VA_ARGS__)                                \
	MACRO(0x10D, DocumentName, TIFF_6, __VA_ARGS__)                             \
	MACRO(0x10E, ImageDescription, TIFF_6, __VA_ARGS__)                         \
	MACRO(0x10F, Make, TIFF_6, __VA_ARGS__)                                     \
	MACRO(0x110, Model, TIFF_6, __VA_ARGS__)                                    \
	MACRO(0x111, StripOffsets, TIFF_6, __VA_ARGS__)                             \
	MACRO(0x112, Orientation, TIFF_6, __VA_ARGS__)                              \
	MACRO(0x115, SamplesPerPixel, TIFF_6, __VA_ARGS__)                          \
	MACRO(0x116, RowsPerStrip, TIFF_6, __VA_ARGS__)                             \
	MACRO(0x117, StripByteCounts, TIFF_6, __VA_ARGS__)                          \
	MACRO(0x118, MinSampleValue, TIFF_6, __VA_ARGS__)                           \
	MACRO(0x119, MaxSampleValue, TIFF_6, __VA_ARGS__)                           \
	MACRO(0x11A, XResolution, TIFF_6, __VA_ARGS__)                              \
	MACRO(0x11B, YResolution, TIFF_6, __VA_ARGS__)                              \
	MACRO(0x11C, PlanarConfiguration, TIFF_6, __VA_ARGS__)                      \
	MACRO(0x11D, PageName, TIFF_6, __VA_ARGS__)                                 \
	MACRO(0x11E, XPosition, TIFF_6, __VA_ARGS__)                                \
	MACRO(0x11F, YPosition, TIFF_6, __VA_ARGS__)                                \
	MACRO(0x120, FreeOffsets, TIFF_6, __VA_ARGS__)                              \
	MACRO(0x121, FreeByteCounts, TIFF_6, __VA_ARGS__)                           \
	MACRO(0x122, GrayResponseUnit, TIFF_6, __VA_ARGS__)                         \
	MACRO(0x123, GrayResponseCurve, TIFF_6, __VA_ARGS__)                        \
	MACRO(0x124, T4Options, TIFF_6, __VA_ARGS__)                                \
	MACRO(0x125, T6Options, TIFF_6, __VA_ARGS__)                                \
	MACRO(0x128, ResolutionUnit, TIFF_6, __VA_ARGS__)                           \
	MACRO(0x129, PageNumber, TIFF_6, __VA_ARGS__)                               \
	MACRO(0x12D, TransferFunction, TIFF_6, __VA_ARGS__)                         \
	MACRO(0x131, Software, TIFF_6, __VA_ARGS__)                                 \
	MACRO(0x132, DateTime, TIFF_6, __VA_ARGS__)                                 \
	MACRO(0x13B, Artist, TIFF_6, __VA_ARGS__)                                   \
	MACRO(0x13C, HostComputer, TIFF_6, __VA_ARGS__)                             \
	MACRO(0x13D, Predictor, TIFF_6, __VA_ARGS__)                                \
	MACRO(0x13E, WhitePoint, TIFF_6, __VA_ARGS__)                               \
	MACRO(0x13F, PrimaryChromaticities, TIFF_6, __VA_ARGS__)                    \
	MACRO(0x140, ColorMap, TIFF_6, __VA_ARGS__)                                 \
	MACRO(0x141, HalftoneHints, TIFF_6, __VA_ARGS__)                            \
	MACRO(0x142, TileWidth, TIFF_6, __VA_ARGS__)                                \
	MACRO(0x143, TileLength, TIFF_6, __VA_ARGS__)                               \
	MACRO(0x144, TileOffsets, TIFF_6, __VA_ARGS__)                              \
	MACRO(0x145, TileByteCounts, TIFF_6, __VA_ARGS__)                           \
	MACRO(0x14C, InkSet, TIFF_6, __VA_ARGS__)                                   \
	MACRO(0x14D, InkNames, TIFF_6, __VA_ARGS__)                                 \
	MACRO(0x14E, NumberOfInks, TIFF_6, __VA_ARGS__)                             \
	MACRO(0x150, DotRange, TIFF_6, __VA_ARGS__)                                 \
	MACRO(0x151, TargetPrinter, TIFF_6, __VA_ARGS__)                            \
	MACRO(0x152, ExtraSamples, TIFF_6, __VA_ARGS__)                             \
	MACRO(0x153, SampleFormat, TIFF_6, __VA_ARGS__)                             \
	MACRO(0x154, SMinSampleValue, TIFF_6, __VA_ARGS__)                          \
	MACRO(0x155, SMaxSampleValue, TIFF_6, __VA_ARGS__)                          \
	MACRO(0x156, TransferRange, TIFF_6, __VA_ARGS__)                            \
	MACRO(0x200, JPEGProc, TIFF_6, __VA_ARGS__)                                 \
	MACRO(0x201, JPEGInterchangeFormat, TIFF_6, __VA_ARGS__)                    \
	MACRO(0x202, JPEGInterchangeFormatLength, TIFF_6, __VA_ARGS__)              \
	MACRO(0x203, JPEGRestartInterval, TIFF_6, __VA_ARGS__)                      \
	MACRO(0x205, JPEGLosslessPredictors, TIFF_6, __VA_ARGS__)                   \
	MACRO(0x206, JPEGPointTransforms, TIFF_6, __VA_ARGS__)                      \
	MACRO(0x207, JPEGQTables, TIFF_6, __VA_ARGS__)                              \
	MACRO(0x208, JPEGDCTables, TIFF_6, __VA_ARGS__)                             \
	MACRO(0x209, JPEGACTables, TIFF_6, __VA_ARGS__)                             \
	MACRO(0x211, YCbCrCoefficients, TIFF_6, __VA_ARGS__)                        \
	MACRO(0x212, YCbCrSubSampling, TIFF_6, __VA_ARGS__)                         \
	MACRO(0x213, YCbCrPositioning, TIFF_6, __VA_ARGS__)                         \
	MACRO(0x214, ReferenceBlackWhite, TIFF_6, __VA_ARGS__)                      \
	MACRO(0x8298, Copyright, TIFF_6, __VA_ARGS__)

#define LAK_FOREACH_TIFF_EP_TAG_VALUE(MACRO, ...)                             \
	MACRO(0x14A, SubIFDs, TIFF_EP, __VA_ARGS__)                                 \
	MACRO(0x15B, JPEGTables, TIFF_EP, __VA_ARGS__)                              \
	MACRO(0x828D, CFARepeatPatternDim, TIFF_EP, __VA_ARGS__)                    \
	MACRO(0x828E, CFAPattern, TIFF_EP, __VA_ARGS__)                             \
	MACRO(0x828F, BatteryLevel, TIFF_EP, __VA_ARGS__)                           \
	MACRO(0x829A, ExposureTime, TIFF_EP, __VA_ARGS__)                           \
	MACRO(0x829D, FNumber, TIFF_EP, __VA_ARGS__)                                \
	MACRO(0x83BB, IPTC_NAA, TIFF_EP, __VA_ARGS__)                               \
	MACRO(0x8773, InterColorProfile, TIFF_EP, __VA_ARGS__)                      \
	MACRO(0x8822, ExposureProgram, TIFF_EP, __VA_ARGS__)                        \
	MACRO(0x8824, SpectralSensitivity, TIFF_EP, __VA_ARGS__)                    \
	MACRO(0x8825, GPSInfo, TIFF_EP, __VA_ARGS__)                                \
	MACRO(0x8827, ISOSpeedRatings, TIFF_EP, __VA_ARGS__)                        \
	MACRO(0x8828, OECF, TIFF_EP, __VA_ARGS__)                                   \
	MACRO(0x8829, Interlace, TIFF_EP, __VA_ARGS__)                              \
	MACRO(0x882A, TimeZoneOffset, TIFF_EP, __VA_ARGS__)                         \
	MACRO(0x882B, SelfTimerMode, TIFF_EP, __VA_ARGS__)                          \
	MACRO(0x9003, DateTimeOriginal, TIFF_EP, __VA_ARGS__)                       \
	MACRO(0x9102, CompressedBitsPerPixel, TIFF_EP, __VA_ARGS__)                 \
	MACRO(0x9201, ShutterSpeedValue, TIFF_EP, __VA_ARGS__)                      \
	MACRO(0x9202, ApertureValue, TIFF_EP, __VA_ARGS__)                          \
	MACRO(0x9203, BrightnessValue, TIFF_EP, __VA_ARGS__)                        \
	MACRO(0x9204, ExposureBiasValue, TIFF_EP, __VA_ARGS__)                      \
	MACRO(0x9205, MaxApertureValue, TIFF_EP, __VA_ARGS__)                       \
	MACRO(0x9206, SubjectDistance, TIFF_EP, __VA_ARGS__)                        \
	MACRO(0x9207, MeteringMode, TIFF_EP, __VA_ARGS__)                           \
	MACRO(0x9208, LightSource, TIFF_EP, __VA_ARGS__)                            \
	MACRO(0x9209, Flash, TIFF_EP, __VA_ARGS__)                                  \
	MACRO(0x920A, FocalLength, TIFF_EP, __VA_ARGS__)                            \
	MACRO(0x920B, FlashEnergy, TIFF_EP, __VA_ARGS__)                            \
	MACRO(0x920C, SpatialFrequencyResponse, TIFF_EP, __VA_ARGS__)               \
	MACRO(0x920D, Noise, TIFF_EP, __VA_ARGS__)                                  \
	MACRO(0x920E, FocalPlaneXResolution, TIFF_EP, __VA_ARGS__)                  \
	MACRO(0x920F, FocalPlaneYResolution, TIFF_EP, __VA_ARGS__)                  \
	MACRO(0x9210, FocalPlaneResolutionUnit, TIFF_EP, __VA_ARGS__)               \
	MACRO(0x9211, ImageNumber, TIFF_EP, __VA_ARGS__)                            \
	MACRO(0x9212, SecurityClassification, TIFF_EP, __VA_ARGS__)                 \
	MACRO(0x9213, ImageHistory, TIFF_EP, __VA_ARGS__)                           \
	MACRO(0x9214, SubjectLocation, TIFF_EP, __VA_ARGS__)                        \
	MACRO(0x9215, ExposureIndex, TIFF_EP, __VA_ARGS__)                          \
	MACRO(0x9216, TIFF_EPStandardID, TIFF_EP, __VA_ARGS__)                      \
	MACRO(0x9217, SensingMethod, TIFF_EP, __VA_ARGS__)

#define LAK_FOREACH_EXIF_TAG_VALUE(MACRO, ...)                                \
	MACRO(0x2BC, ApplicationNotes, EXIF, __VA_ARGS__)                           \
	MACRO(0x827D, Model2, EXIF, __VA_ARGS__)                                    \
	MACRO(0x8290, KodakIFD, EXIF, __VA_ARGS__)                                  \
	MACRO(0x9000, ExifVersion, EXIF, __VA_ARGS__)                               \
	MACRO(0x9004, DateTimeDigitized, EXIF, __VA_ARGS__)                         \
	MACRO(0x9101, ComponentsConfiguration, EXIF, __VA_ARGS__)                   \
	MACRO(0xA000, FlashpixVersion, EXIF, __VA_ARGS__)                           \
	MACRO(0xA001, ColorSpace, EXIF, __VA_ARGS__)                                \
	MACRO(0xA002, PixelXDimension, EXIF, __VA_ARGS__)                           \
	MACRO(0xA003, PixelYDimension, EXIF, __VA_ARGS__)                           \
	MACRO(0xA004, RelatedSoundFile, EXIF, __VA_ARGS__)                          \
	MACRO(0xA005, InteropIFD, EXIF, __VA_ARGS__)                                \
	MACRO(0xA20B, ExifFlashEnergy, EXIF, __VA_ARGS__)                           \
	MACRO(0xA215, ExifExposureIndex, EXIF, __VA_ARGS__)                         \
	MACRO(0xA217, ExifSensingMethod, EXIF, __VA_ARGS__)                         \
	MACRO(0xA300, FileSource, EXIF, __VA_ARGS__)                                \
	MACRO(0xA301, SceneType, EXIF, __VA_ARGS__)                                 \
	MACRO(0xA401, CustomRendered, EXIF, __VA_ARGS__)                            \
	MACRO(0xA402, ExposureMode, EXIF, __VA_ARGS__)                              \
	MACRO(0xA403, WhiteBalance, EXIF, __VA_ARGS__)                              \
	MACRO(0xA404, DigitalZoomRatio, EXIF, __VA_ARGS__)                          \
	MACRO(0xA405, FocalLengthIn35mmFormat, EXIF, __VA_ARGS__)                   \
	MACRO(0xA406, SceneCaptureType, EXIF, __VA_ARGS__)                          \
	MACRO(0xA407, GainControl, EXIF, __VA_ARGS__)                               \
	MACRO(0xA408, Contrast, EXIF, __VA_ARGS__)                                  \
	MACRO(0xA409, Saturation, EXIF, __VA_ARGS__)                                \
	MACRO(0xA40A, Sharpness, EXIF, __VA_ARGS__)                                 \
	MACRO(0xA40C, SubjectDistanceRange, EXIF, __VA_ARGS__)                      \
	MACRO(0xA433, LensMake, EXIF, __VA_ARGS__)                                  \
	MACRO(0xA434, LensModel, EXIF, __VA_ARGS__)                                 \
	MACRO(0xA435, LensSerialNumber, EXIF, __VA_ARGS__)                          \
	MACRO(0x8769, ExifOffset, EXIF, __VA_ARGS__)

#define LAK_FOREACH_DNG_TAG_VALUE(MACRO, ...)                                 \
	/* 1.1.0.0 */                                                               \
	MACRO(0xC612, DNGVersion, DNG_1_1_0_0, __VA_ARGS__)                         \
	MACRO(0xC613, DNGBackwardVersion, DNG_1_1_0_0, __VA_ARGS__)                 \
	MACRO(0xC614, UniqueCameraModel, DNG_1_1_0_0, __VA_ARGS__)                  \
	MACRO(0xC615, LocalizedCameraModel, DNG_1_1_0_0, __VA_ARGS__)               \
	MACRO(0xC616, CFAPlaneColor, DNG_1_1_0_0, __VA_ARGS__)                      \
	MACRO(0xC617, CFALayout, DNG_1_1_0_0, __VA_ARGS__)                          \
	MACRO(0xC618, LinearizationTable, DNG_1_1_0_0, __VA_ARGS__)                 \
	MACRO(0xC619, BlackLevelRepeatDim, DNG_1_1_0_0, __VA_ARGS__)                \
	MACRO(0xC61A, BlackLevel, DNG_1_1_0_0, __VA_ARGS__)                         \
	MACRO(0xC61B, BlackLevelDeltaH, DNG_1_1_0_0, __VA_ARGS__)                   \
	MACRO(0xC61C, BlackLevelDeltaV, DNG_1_1_0_0, __VA_ARGS__)                   \
	MACRO(0xC61D, WhiteLevel, DNG_1_1_0_0, __VA_ARGS__)                         \
	MACRO(0xC61E, DefaultScale, DNG_1_1_0_0, __VA_ARGS__)                       \
	MACRO(0xC61F, DefaultCropOrigin, DNG_1_1_0_0, __VA_ARGS__)                  \
	MACRO(0xC620, DefaultCropSize, DNG_1_1_0_0, __VA_ARGS__)                    \
	MACRO(0xC621, ColorMatrix1, DNG_1_1_0_0, __VA_ARGS__)                       \
	MACRO(0xC622, ColorMatrix2, DNG_1_1_0_0, __VA_ARGS__)                       \
	MACRO(0xC623, CameraCalibration1, DNG_1_1_0_0, __VA_ARGS__)                 \
	MACRO(0xC624, CameraCalibration2, DNG_1_1_0_0, __VA_ARGS__)                 \
	MACRO(0xC625, ReductionMatrix1, DNG_1_1_0_0, __VA_ARGS__)                   \
	MACRO(0xC626, ReductionMatrix2, DNG_1_1_0_0, __VA_ARGS__)                   \
	MACRO(0xC627, AnalogBalance, DNG_1_1_0_0, __VA_ARGS__)                      \
	MACRO(0xC628, AsShotNeutral, DNG_1_1_0_0, __VA_ARGS__)                      \
	MACRO(0xC629, AsShotWhiteXY, DNG_1_1_0_0, __VA_ARGS__)                      \
	MACRO(0xC62A, BaselineExposure, DNG_1_1_0_0, __VA_ARGS__)                   \
	MACRO(0xC62B, BaselineNoise, DNG_1_1_0_0, __VA_ARGS__)                      \
	MACRO(0xC62C, BaselineSharpness, DNG_1_1_0_0, __VA_ARGS__)                  \
	MACRO(0xC62D, BayerGreenSplit, DNG_1_1_0_0, __VA_ARGS__)                    \
	MACRO(0xC62E, LinearResponseLimit, DNG_1_1_0_0, __VA_ARGS__)                \
	MACRO(0xC62F, CameraSerialNumber, DNG_1_1_0_0, __VA_ARGS__)                 \
	MACRO(0xC630, LensInfo, DNG_1_1_0_0, __VA_ARGS__)                           \
	MACRO(0xC631, ChromaBlurRadius, DNG_1_1_0_0, __VA_ARGS__)                   \
	MACRO(0xC632, AntiAliasStrength, DNG_1_1_0_0, __VA_ARGS__)                  \
	MACRO(0xC633, ShadowScale, DNG_1_1_0_0, __VA_ARGS__)                        \
	MACRO(0xC634, DNGPrivateData, DNG_1_1_0_0, __VA_ARGS__)                     \
	MACRO(0xC635, MakerNoteSafety, DNG_1_1_0_0, __VA_ARGS__)                    \
	MACRO(0xC65A, CalibrationIlluminant1, DNG_1_1_0_0, __VA_ARGS__)             \
	MACRO(0xC65B, CalibrationIlluminant2, DNG_1_1_0_0, __VA_ARGS__)             \
	MACRO(0xC65C, BestQualityScale, DNG_1_1_0_0, __VA_ARGS__)                   \
	MACRO(0xC65D, RawDataUniqueID, DNG_1_1_0_0, __VA_ARGS__)                    \
	MACRO(0xC68B, OriginalRawFileName, DNG_1_1_0_0, __VA_ARGS__)                \
	MACRO(0xC68C, OriginalRawFileData, DNG_1_1_0_0, __VA_ARGS__)                \
	MACRO(0xC68D, ActiveArea, DNG_1_1_0_0, __VA_ARGS__)                         \
	MACRO(0xC68E, MaskedAreas, DNG_1_1_0_0, __VA_ARGS__)                        \
	MACRO(0xC68F, AsShotICCProfile, DNG_1_1_0_0, __VA_ARGS__)                   \
	MACRO(0xC690, AsShotPreProfileMatrix, DNG_1_1_0_0, __VA_ARGS__)             \
	MACRO(0xC691, CurrentICCProfile, DNG_1_1_0_0, __VA_ARGS__)                  \
	MACRO(0xC692, CurrentPreProfileMatrix, DNG_1_1_0_0, __VA_ARGS__)            \
	/* 1.2.0.0 */                                                               \
	MACRO(0xC6BF, ColorimetricReference, DNG_1_2_0_0, __VA_ARGS__)              \
	MACRO(0xC6F3, CameraCalibrationSignature, DNG_1_2_0_0, __VA_ARGS__)         \
	MACRO(0xC6F4, ProfileCalibrationSignature, DNG_1_2_0_0, __VA_ARGS__)        \
	MACRO(0xC6F5, ExtraCameraProfiles, DNG_1_2_0_0, __VA_ARGS__)                \
	MACRO(0xC6F6, AsShotProfileName, DNG_1_2_0_0, __VA_ARGS__)                  \
	MACRO(0xC6F7, NoiseReductionApplied, DNG_1_2_0_0, __VA_ARGS__)              \
	MACRO(0xC6F8, ProfileName, DNG_1_2_0_0, __VA_ARGS__)                        \
	MACRO(0xC6F9, ProfileHueSatMapDims, DNG_1_2_0_0, __VA_ARGS__)               \
	MACRO(0xC6FA, ProfileHueSatMapData1, DNG_1_2_0_0, __VA_ARGS__)              \
	MACRO(0xC6FB, ProfileHueSatMapData2, DNG_1_2_0_0, __VA_ARGS__)              \
	MACRO(0xC6FC, ProfileToneCurve, DNG_1_2_0_0, __VA_ARGS__)                   \
	MACRO(0xC6FD, ProfileEmbedPolicy, DNG_1_2_0_0, __VA_ARGS__)                 \
	MACRO(0xC6FE, ProfileCopyright, DNG_1_2_0_0, __VA_ARGS__)                   \
	MACRO(0xC714, ForwardMatrix1, DNG_1_2_0_0, __VA_ARGS__)                     \
	MACRO(0xC715, ForwardMatrix2, DNG_1_2_0_0, __VA_ARGS__)                     \
	MACRO(0xC716, PreviewApplicationName, DNG_1_2_0_0, __VA_ARGS__)             \
	MACRO(0xC717, PreviewApplicationVersion, DNG_1_2_0_0, __VA_ARGS__)          \
	MACRO(0xC718, PreviewSettingName, DNG_1_2_0_0, __VA_ARGS__)                 \
	MACRO(0xC719, PreviewSettingDigest, DNG_1_2_0_0, __VA_ARGS__)               \
	MACRO(0xC71A, PreviewColorSpace, DNG_1_2_0_0, __VA_ARGS__)                  \
	MACRO(0xC71B, PreviewDateTime, DNG_1_2_0_0, __VA_ARGS__)                    \
	MACRO(0xC71C, RawImageDigest, DNG_1_2_0_0, __VA_ARGS__)                     \
	MACRO(0xC71D, OriginalRawFileDigest, DNG_1_2_0_0, __VA_ARGS__)              \
	MACRO(0xC71E, SubTileBlockSize, DNG_1_2_0_0, __VA_ARGS__)                   \
	MACRO(0xC71F, RowInterleaveFactor, DNG_1_2_0_0, __VA_ARGS__)                \
	MACRO(0xC725, ProfileLookTableDims, DNG_1_2_0_0, __VA_ARGS__)               \
	MACRO(0xC726, ProfileLookTableData, DNG_1_2_0_0, __VA_ARGS__)               \
	/* 1.4.0.0 */                                                               \
	MACRO(0xC740, OpcodeList1, DNG_1_3_0_0, __VA_ARGS__)                        \
	MACRO(0xC741, OpcodeList2, DNG_1_3_0_0, __VA_ARGS__)                        \
	MACRO(0xC74E, OpcodeList3, DNG_1_3_0_0, __VA_ARGS__)                        \
	MACRO(0xC761, NoiseProfile, DNG_1_3_0_0, __VA_ARGS__)                       \
	/* 1.4.0.0 */                                                               \
	MACRO(0xC791, OriginalDefaultFinalSize, DNG_1_4_0_0, __VA_ARGS__)           \
	MACRO(0xC792, OriginalBestQualityFinalSize, DNG_1_4_0_0, __VA_ARGS__)       \
	MACRO(0xC793, OriginalDefaultCropSize, DNG_1_4_0_0, __VA_ARGS__)            \
	MACRO(0xC7B5, DefaultUserCrop, DNG_1_4_0_0, __VA_ARGS__)                    \
	MACRO(0xC7A3, ProfileHueSatMapEncoding, DNG_1_4_0_0, __VA_ARGS__)           \
	MACRO(0xC7A4, ProfileLookTableEncoding, DNG_1_4_0_0, __VA_ARGS__)           \
	MACRO(0xC7A5, BaselineExposureOffset, DNG_1_4_0_0, __VA_ARGS__)             \
	MACRO(0xC7A6, DefaultBlackRender, DNG_1_4_0_0, __VA_ARGS__)                 \
	MACRO(0xC7A7, NewRawImageDigest, DNG_1_4_0_0, __VA_ARGS__)                  \
	MACRO(0xC7A8, RawToPreviewGain, DNG_1_4_0_0, __VA_ARGS__)                   \
	/* 1.5.0.0 */                                                               \
	MACRO(0xC7E9, DepthFormat, DNG_1_5_0_0, __VA_ARGS__)                        \
	MACRO(0xC7EA, DepthNear, DNG_1_5_0_0, __VA_ARGS__)                          \
	MACRO(0xC7EB, DepthFar, DNG_1_5_0_0, __VA_ARGS__)                           \
	MACRO(0xC7EC, DepthUnits, DNG_1_5_0_0, __VA_ARGS__)                         \
	MACRO(0xC7ED, DepthMeasureType, DNG_1_5_0_0, __VA_ARGS__)                   \
	MACRO(0xC7EE, EnhanceParams, DNG_1_5_0_0, __VA_ARGS__)                      \
	/* 1.6.0.0 */                                                               \
	MACRO(0xCD2D, ProfileGainTableMap, DNG_1_6_0_0, __VA_ARGS__)                \
	MACRO(0xCD2E, SemanticName, DNG_1_6_0_0, __VA_ARGS__)                       \
	MACRO(0xCD30, SemanticInstanceID, DNG_1_6_0_0, __VA_ARGS__)                 \
	MACRO(0xCD31, CalibrationIlluminant3, DNG_1_6_0_0, __VA_ARGS__)             \
	MACRO(0xCD32, CameraCalibration3, DNG_1_6_0_0, __VA_ARGS__)                 \
	MACRO(0xCD33, ColorMatrix3, DNG_1_6_0_0, __VA_ARGS__)                       \
	MACRO(0xCD34, ForwardMatrix3, DNG_1_6_0_0, __VA_ARGS__)                     \
	MACRO(0xCD35, IlluminantData1, DNG_1_6_0_0, __VA_ARGS__)                    \
	MACRO(0xCD36, IlluminantData2, DNG_1_6_0_0, __VA_ARGS__)                    \
	MACRO(0xCD37, IlluminantData3, DNG_1_6_0_0, __VA_ARGS__)                    \
	MACRO(0xCD38, MaskSubArea, DNG_1_6_0_0, __VA_ARGS__)                        \
	MACRO(0xCD39, ProfileHueSatMapData3, DNG_1_6_0_0, __VA_ARGS__)              \
	MACRO(0xCD3A, ReductionMatrix3, DNG_1_6_0_0, __VA_ARGS__)                   \
	MACRO(0xCD3F, RGBTables, DNG_1_6_0_0, __VA_ARGS__)                          \
	/* 1.7.0.0 */                                                               \
	MACRO(0xCD40, ProfileGainTableMap2, DNG_1_7_0_0, __VA_ARGS__)               \
	MACRO(0xCD44, ImageSequenceInfo, DNG_1_7_0_0, __VA_ARGS__)                  \
	MACRO(0xCD46, ImageStats, DNG_1_7_0_0, __VA_ARGS__)                         \
	MACRO(0xCD47, ProfileDynamicRange, DNG_1_7_0_0, __VA_ARGS__)                \
	MACRO(0xCD48, ProfileGroupName, DNG_1_7_0_0, __VA_ARGS__)                   \
	/* 1.7.1.0 */                                                               \
	MACRO(0xCD43, ColumnInterleaveFactor, DNG_1_7_1_0, __VA_ARGS__)             \
	MACRO(0xCD49, JXLDistance, DNG_1_7_1_0, __VA_ARGS__)                        \
	MACRO(0xCD4A, JXLEffort, DNG_1_7_1_0, __VA_ARGS__)                          \
	MACRO(0xCD4B, JXLDecodeSpeed, DNG_1_7_1_0, __VA_ARGS__)

#define LAK_FOREACH_TIFF_TAG_VALUE(MACRO, ...)                                \
	LAK_FOREACH_TIFF6_TAG_VALUE(MACRO, __VA_ARGS__)                             \
	LAK_FOREACH_TIFF_EP_TAG_VALUE(MACRO, __VA_ARGS__)                           \
	LAK_FOREACH_EXIF_TAG_VALUE(MACRO, __VA_ARGS__)                              \
	LAK_FOREACH_DNG_TAG_VALUE(MACRO, __VA_ARGS__)

namespace lak
{
	namespace tiff
	{
		enum struct tag_name : uint16_t
		{
#define LAK_TIFF_TAG_NAME(VAL, NAME, ...) NAME = VAL,
			LAK_FOREACH_TIFF_TAG_VALUE(LAK_TIFF_TAG_NAME)
#undef LAK_TIFF_TAG_NAME
		};
	}

	template<typename CHAR>
	struct format_traits<lak::tiff::tag_name, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::tiff::tag_name &tag)
		{
			switch (tag)
			{
#define LAK_TIFF_TAG_NAME(VAL, NAME, ...)                                     \
	case lak::tiff::tag_name::NAME: return lak::strconv<CHAR>(#NAME ""_view);
				LAK_FOREACH_TIFF_TAG_VALUE(LAK_TIFF_TAG_NAME)
#undef LAK_TIFF_TAG_NAME
			}
			return lak::fmt<CHAR, "Unknown ({:#X})">(static_cast<uint16_t>(tag));
		}
	};
}

#define LAK_FOREACH_TIFF6_TAG_TYPES(MACRO, ...)                               \
	MACRO(NewSubfileType, uint32_t, 1U, __VA_ARGS__)                            \
	MACRO(SubfileType, uint16_t, 1U, __VA_ARGS__)                               \
	MACRO(ImageWidth, uint16_t, 1U, __VA_ARGS__)                                \
	MACRO(ImageWidth, uint32_t, 1U, __VA_ARGS__)                                \
	MACRO(ImageLength, uint16_t, 1U, __VA_ARGS__)                               \
	MACRO(ImageLength, uint32_t, 1U, __VA_ARGS__)                               \
	MACRO(BitsPerSample, uint16_t, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(Compression, uint16_t, 1U, __VA_ARGS__)                               \
	MACRO(PhotometricInterpretation, uint16_t, 1U, __VA_ARGS__)                 \
	MACRO(Thresholding, uint16_t, 1U, __VA_ARGS__)                              \
	MACRO(CellWidth, uint16_t, 1U, __VA_ARGS__)                                 \
	MACRO(CellLength, uint16_t, 1U, __VA_ARGS__)                                \
	MACRO(FillOrder, uint16_t, 1U, __VA_ARGS__)                                 \
	MACRO(DocumentName, char, lak::dynamic_extent, __VA_ARGS__)                 \
	MACRO(ImageDescription, char, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(Make, char, lak::dynamic_extent, __VA_ARGS__)                         \
	MACRO(Model, char, lak::dynamic_extent, __VA_ARGS__)                        \
	MACRO(StripOffsets, uint16_t, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(StripOffsets, uint32_t, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(Orientation, uint16_t, 1U, __VA_ARGS__)                               \
	MACRO(SamplesPerPixel, uint16_t, 1U, __VA_ARGS__)                           \
	MACRO(RowsPerStrip, uint16_t, 1U, __VA_ARGS__)                              \
	MACRO(RowsPerStrip, uint32_t, 1U, __VA_ARGS__)                              \
	MACRO(StripByteCounts, uint16_t, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(StripByteCounts, uint32_t, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(MinSampleValue, uint16_t, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(MaxSampleValue, uint16_t, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(XResolution, lak::tiff::urational, 1U, __VA_ARGS__)                   \
	MACRO(YResolution, lak::tiff::urational, 1U, __VA_ARGS__)                   \
	MACRO(PlanarConfiguration, uint16_t, 1U, __VA_ARGS__)                       \
	MACRO(PageName, char, lak::dynamic_extent, __VA_ARGS__)                     \
	MACRO(XPosition, lak::tiff::urational, lak::dynamic_extent, __VA_ARGS__)    \
	MACRO(YPosition, lak::tiff::urational, lak::dynamic_extent, __VA_ARGS__)    \
	MACRO(FreeOffsets, uint32_t, lak::dynamic_extent, __VA_ARGS__)              \
	MACRO(FreeByteCounts, uint32_t, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(GrayResponseUnit, uint16_t, 1U, __VA_ARGS__)                          \
	MACRO(GrayResponseCurve, uint16_t, lak::dynamic_extent, __VA_ARGS__)        \
	MACRO(T4Options, uint32_t, 1U, __VA_ARGS__)                                 \
	MACRO(T6Options, uint32_t, 1U, __VA_ARGS__)                                 \
	MACRO(ResolutionUnit, uint16_t, 1U, __VA_ARGS__)                            \
	MACRO(PageNumber, uint16_t, 2U, __VA_ARGS__)                                \
	MACRO(TransferFunction, uint16_t, lak::dynamic_extent, __VA_ARGS__)         \
	MACRO(Software, char, lak::dynamic_extent, __VA_ARGS__)                     \
	MACRO(DateTime, char, 20U, __VA_ARGS__)                                     \
	MACRO(Artist, char, lak::dynamic_extent, __VA_ARGS__)                       \
	MACRO(HostComputer, char, lak::dynamic_extent, __VA_ARGS__)                 \
	MACRO(Predictor, uint16_t, 1U, __VA_ARGS__)                                 \
	MACRO(WhitePoint, lak::tiff::urational, 2U, __VA_ARGS__)                    \
	MACRO(PrimaryChromaticities, lak::tiff::urational, 6U, __VA_ARGS__)         \
	MACRO(ColorMap, uint16_t, lak::dynamic_extent, __VA_ARGS__)                 \
	MACRO(HalftoneHints, uint16_t, 2U, __VA_ARGS__)                             \
	MACRO(TileWidth, uint16_t, 1U, __VA_ARGS__)                                 \
	MACRO(TileWidth, uint32_t, 1U, __VA_ARGS__)                                 \
	MACRO(TileLength, uint16_t, 1U, __VA_ARGS__)                                \
	MACRO(TileLength, uint32_t, 1U, __VA_ARGS__)                                \
	MACRO(TileOffsets, uint32_t, lak::dynamic_extent, __VA_ARGS__)              \
	MACRO(TileByteCounts, uint16_t, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(TileByteCounts, uint32_t, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(InkSet, uint16_t, 1U, __VA_ARGS__)                                    \
	MACRO(InkNames, char, lak::dynamic_extent, __VA_ARGS__)                     \
	MACRO(NumberOfInks, uint16_t, 1U, __VA_ARGS__)                              \
	MACRO(DotRange, uint8_t, lak::dynamic_extent, __VA_ARGS__)                  \
	MACRO(DotRange, uint16_t, lak::dynamic_extent, __VA_ARGS__)                 \
	MACRO(TargetPrinter, char, lak::dynamic_extent, __VA_ARGS__)                \
	MACRO(ExtraSamples, uint8_t, lak::dynamic_extent, __VA_ARGS__)              \
	MACRO(SampleFormat, uint16_t, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(SMinSampleValue, uint8_t, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(SMinSampleValue, char, lak::dynamic_extent, __VA_ARGS__)              \
	MACRO(SMinSampleValue, uint16_t, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(SMinSampleValue, uint32_t, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(                                                                      \
	  SMinSampleValue, lak::tiff::urational, lak::dynamic_extent, __VA_ARGS__)  \
	MACRO(SMinSampleValue, int8_t, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(SMinSampleValue, byte_t, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(SMinSampleValue, int16_t, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(SMinSampleValue, int32_t, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(                                                                      \
	  SMinSampleValue, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)   \
	MACRO(SMinSampleValue, f32_t, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(SMinSampleValue, f64_t, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(                                                                      \
	  SMinSampleValue, lak::tiff::_offset, lak::dynamic_extent, __VA_ARGS__)    \
	MACRO(SMaxSampleValue, uint8_t, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(SMaxSampleValue, char, lak::dynamic_extent, __VA_ARGS__)              \
	MACRO(SMaxSampleValue, uint16_t, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(SMaxSampleValue, uint32_t, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(                                                                      \
	  SMaxSampleValue, lak::tiff::urational, lak::dynamic_extent, __VA_ARGS__)  \
	MACRO(SMaxSampleValue, int8_t, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(SMaxSampleValue, byte_t, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(SMaxSampleValue, int16_t, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(SMaxSampleValue, int32_t, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(                                                                      \
	  SMaxSampleValue, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)   \
	MACRO(SMaxSampleValue, f32_t, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(SMaxSampleValue, f64_t, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(                                                                      \
	  SMaxSampleValue, lak::tiff::_offset, lak::dynamic_extent, __VA_ARGS__)    \
	MACRO(TransferRange, uint16_t, 6U, __VA_ARGS__)                             \
	MACRO(JPEGProc, uint16_t, 1U, __VA_ARGS__)                                  \
	MACRO(JPEGInterchangeFormat, uint32_t, 1U, __VA_ARGS__)                     \
	MACRO(JPEGInterchangeFormatLength, uint32_t, 1U, __VA_ARGS__)               \
	MACRO(JPEGRestartInterval, uint16_t, 1U, __VA_ARGS__)                       \
	MACRO(JPEGLosslessPredictors, uint16_t, lak::dynamic_extent, __VA_ARGS__)   \
	MACRO(JPEGPointTransforms, uint16_t, lak::dynamic_extent, __VA_ARGS__)      \
	MACRO(JPEGQTables, uint32_t, lak::dynamic_extent, __VA_ARGS__)              \
	MACRO(JPEGDCTables, uint32_t, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(JPEGACTables, uint32_t, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(YCbCrCoefficients, lak::tiff::urational, 3U, __VA_ARGS__)             \
	MACRO(YCbCrSubSampling, uint16_t, 2U, __VA_ARGS__)                          \
	MACRO(YCbCrPositioning, uint16_t, 1U, __VA_ARGS__)                          \
	MACRO(ReferenceBlackWhite, uint32_t, lak::dynamic_extent, __VA_ARGS__)      \
	MACRO(ReferenceBlackWhite,                                                  \
	      lak::tiff::urational,                                                 \
	      lak::dynamic_extent,                                                  \
	      __VA_ARGS__)                                                          \
	MACRO(Copyright, char, lak::dynamic_extent, __VA_ARGS__)

#define LAK_FOREACH_TIFF_EP_TAG_TYPES(MACRO, ...)                             \
	MACRO(SubIFDs, uint32_t, lak::dynamic_extent, __VA_ARGS__)                  \
	MACRO(JPEGTables, byte_t, lak::dynamic_extent, __VA_ARGS__)                 \
	MACRO(CFARepeatPatternDim, uint16_t, 2U, __VA_ARGS__)                       \
	MACRO(CFAPattern, uint8_t, lak::dynamic_extent, __VA_ARGS__)                \
	MACRO(BatteryLevel, lak::tiff::urational, 1U, __VA_ARGS__)                  \
	MACRO(BatteryLevel, char, lak::dynamic_extent, __VA_ARGS__)                 \
	MACRO(ExposureTime, lak::tiff::urational, 1U, __VA_ARGS__)                  \
	MACRO(ExposureTime, lak::tiff::urational, 2U, __VA_ARGS__)                  \
	MACRO(FNumber, lak::tiff::urational, 1U, __VA_ARGS__)                       \
	MACRO(FNumber, lak::tiff::urational, 2U, __VA_ARGS__)                       \
	MACRO(IPTC_NAA, uint32_t, lak::dynamic_extent, __VA_ARGS__)                 \
	MACRO(IPTC_NAA, char, lak::dynamic_extent, __VA_ARGS__)                     \
	MACRO(InterColorProfile, byte_t, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(ExposureProgram, uint16_t, 1U, __VA_ARGS__)                           \
	MACRO(SpectralSensitivity, char, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(GPSInfo, uint32_t, 1U, __VA_ARGS__)                                   \
	MACRO(ISOSpeedRatings, uint16_t, 1U, __VA_ARGS__)                           \
	MACRO(ISOSpeedRatings, uint16_t, 2U, __VA_ARGS__)                           \
	MACRO(ISOSpeedRatings, uint16_t, 3U, __VA_ARGS__)                           \
	MACRO(OECF, byte_t, lak::dynamic_extent, __VA_ARGS__)                       \
	MACRO(Interlace, uint16_t, 1U, __VA_ARGS__)                                 \
	MACRO(TimeZoneOffset, int16_t, 1U, __VA_ARGS__)                             \
	MACRO(TimeZoneOffset, int16_t, 2U, __VA_ARGS__)                             \
	MACRO(SelfTimerMode, uint16_t, 1U, __VA_ARGS__)                             \
	MACRO(DateTimeOriginal, char, 20U, __VA_ARGS__)                             \
	MACRO(CompressedBitsPerPixel, lak::tiff::urational, 1U, __VA_ARGS__)        \
	MACRO(ShutterSpeedValue, lak::tiff::urational, 1U, __VA_ARGS__)             \
	MACRO(ApertureValue, lak::tiff::urational, 1U, __VA_ARGS__)                 \
	MACRO(BrightnessValue, lak::tiff::rational, 1U, __VA_ARGS__)                \
	MACRO(BrightnessValue, lak::tiff::rational, 2U, __VA_ARGS__)                \
	MACRO(ExposureBiasValue, lak::tiff::rational, 1U, __VA_ARGS__)              \
	MACRO(ExposureBiasValue, lak::tiff::rational, 2U, __VA_ARGS__)              \
	MACRO(MaxApertureValue, lak::tiff::urational, 1U, __VA_ARGS__)              \
	MACRO(SubjectDistance, lak::tiff::rational, 1U, __VA_ARGS__)                \
	MACRO(SubjectDistance, lak::tiff::rational, 2U, __VA_ARGS__)                \
	MACRO(MeteringMode, uint16_t, 1U, __VA_ARGS__)                              \
	MACRO(LightSource, uint16_t, 1U, __VA_ARGS__)                               \
	MACRO(Flash, uint16_t, 1U, __VA_ARGS__)                                     \
	MACRO(FocalLength, lak::tiff::urational, 1U, __VA_ARGS__)                   \
	MACRO(FocalLength, lak::tiff::urational, 2U, __VA_ARGS__)                   \
	MACRO(FlashEnergy, lak::tiff::urational, 1U, __VA_ARGS__)                   \
	MACRO(FlashEnergy, lak::tiff::urational, 2U, __VA_ARGS__)                   \
	MACRO(SpatialFrequencyResponse, byte_t, lak::dynamic_extent, __VA_ARGS__)   \
	MACRO(Noise, byte_t, lak::dynamic_extent, __VA_ARGS__)                      \
	MACRO(FocalPlaneXResolution, lak::tiff::urational, 1U, __VA_ARGS__)         \
	MACRO(FocalPlaneYResolution, lak::tiff::urational, 1U, __VA_ARGS__)         \
	MACRO(FocalPlaneResolutionUnit, uint16_t, 1U, __VA_ARGS__)                  \
	MACRO(ImageNumber, uint32_t, 1U, __VA_ARGS__)                               \
	MACRO(SecurityClassification, char, lak::dynamic_extent, __VA_ARGS__)       \
	MACRO(ImageHistory, char, lak::dynamic_extent, __VA_ARGS__)                 \
	MACRO(SubjectLocation, uint16_t, 2U, __VA_ARGS__)                           \
	MACRO(SubjectLocation, uint16_t, 3U, __VA_ARGS__)                           \
	MACRO(SubjectLocation, uint16_t, 4U, __VA_ARGS__)                           \
	MACRO(ExposureIndex, lak::tiff::urational, 1U, __VA_ARGS__)                 \
	MACRO(ExposureIndex, lak::tiff::urational, 2U, __VA_ARGS__)                 \
	MACRO(TIFF_EPStandardID, uint8_t, 4U, __VA_ARGS__)                          \
	MACRO(SensingMethod, uint16_t, 1U, __VA_ARGS__)

#define LAK_FOREACH_EXIF_TAG_TYPES(MACRO, ...)                                \
	MACRO(Model2, char, lak::dynamic_extent, __VA_ARGS__)                       \
	MACRO(KodakIFD, uint32_t, 1U, __VA_ARGS__)                                  \
	MACRO(ExifVersion, byte_t, 4U, __VA_ARGS__)                                 \
	MACRO(DateTimeDigitized, char, 20U, __VA_ARGS__)                            \
	MACRO(ComponentsConfiguration, byte_t, 4U, __VA_ARGS__)                     \
	MACRO(FlashpixVersion, byte_t, 4U, __VA_ARGS__)                             \
	MACRO(ColorSpace, uint16_t, 1U, __VA_ARGS__)                                \
	MACRO(PixelXDimension, uint16_t, 1U, __VA_ARGS__)                           \
	MACRO(PixelXDimension, uint32_t, 1U, __VA_ARGS__)                           \
	MACRO(PixelYDimension, uint16_t, 1U, __VA_ARGS__)                           \
	MACRO(PixelYDimension, uint32_t, 1U, __VA_ARGS__)                           \
	MACRO(RelatedSoundFile, char, 13U, __VA_ARGS__)                             \
	MACRO(InteropIFD, uint32_t, 1U, __VA_ARGS__)                                \
	MACRO(ExifFlashEnergy, lak::tiff::urational, 1U, __VA_ARGS__)               \
	MACRO(ExifExposureIndex, lak::tiff::urational, 1U, __VA_ARGS__)             \
	MACRO(ExifSensingMethod, uint16_t, 1U, __VA_ARGS__)                         \
	MACRO(FileSource, byte_t, 1U, __VA_ARGS__)                                  \
	MACRO(SceneType, byte_t, 1U, __VA_ARGS__)                                   \
	MACRO(CustomRendered, uint16_t, 1U, __VA_ARGS__)                            \
	MACRO(ExposureMode, uint16_t, 1U, __VA_ARGS__)                              \
	MACRO(WhiteBalance, uint16_t, 1U, __VA_ARGS__)                              \
	MACRO(DigitalZoomRatio, lak::tiff::urational, 1U, __VA_ARGS__)              \
	MACRO(FocalLengthIn35mmFormat, uint16_t, 1U, __VA_ARGS__)                   \
	MACRO(SceneCaptureType, uint16_t, 1U, __VA_ARGS__)                          \
	MACRO(GainControl, lak::tiff::urational, 1U, __VA_ARGS__)                   \
	MACRO(Contrast, uint16_t, 1U, __VA_ARGS__)                                  \
	MACRO(Saturation, uint16_t, 1U, __VA_ARGS__)                                \
	MACRO(Sharpness, uint16_t, 1U, __VA_ARGS__)                                 \
	MACRO(SubjectDistanceRange, uint16_t, 1U, __VA_ARGS__)                      \
	MACRO(LensMake, char, lak::dynamic_extent, __VA_ARGS__)                     \
	MACRO(LensModel, char, lak::dynamic_extent, __VA_ARGS__)                    \
	MACRO(LensSerialNumber, char, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(ExifOffset, uint32_t, 1U, __VA_ARGS__)

#define LAK_FOREACH_DNG_TAG_TYPES(MACRO, ...)                                 \
	/* 1.1.0.0 */                                                               \
	MACRO(DNGVersion, uint8_t, 4U, __VA_ARGS__)                                 \
	MACRO(DNGBackwardVersion, uint8_t, 4U, __VA_ARGS__)                         \
	MACRO(UniqueCameraModel, char, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(LocalizedCameraModel, char, lak::dynamic_extent, __VA_ARGS__)         \
	MACRO(LocalizedCameraModel, uint8_t, lak::dynamic_extent, __VA_ARGS__)      \
	MACRO(CFAPlaneColor, uint8_t, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(CFALayout, uint16_t, 1U, __VA_ARGS__)                                 \
	MACRO(LinearizationTable, uint16_t, lak::dynamic_extent, __VA_ARGS__)       \
	MACRO(BlackLevelRepeatDim, uint16_t, 2U, __VA_ARGS__)                       \
	MACRO(BlackLevel, uint16_t, lak::dynamic_extent, __VA_ARGS__)               \
	MACRO(BlackLevel, uint32_t, lak::dynamic_extent, __VA_ARGS__)               \
	MACRO(BlackLevel, lak::tiff::urational, lak::dynamic_extent, __VA_ARGS__)   \
	MACRO(                                                                      \
	  BlackLevelDeltaH, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)  \
	MACRO(                                                                      \
	  BlackLevelDeltaV, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)  \
	MACRO(WhiteLevel, uint16_t, lak::dynamic_extent, __VA_ARGS__)               \
	MACRO(WhiteLevel, uint32_t, lak::dynamic_extent, __VA_ARGS__)               \
	MACRO(DefaultScale, lak::tiff::urational, 2U, __VA_ARGS__)                  \
	MACRO(DefaultCropOrigin, uint16_t, 2U, __VA_ARGS__)                         \
	MACRO(DefaultCropOrigin, uint32_t, 2U, __VA_ARGS__)                         \
	MACRO(DefaultCropOrigin, lak::tiff::urational, 2U, __VA_ARGS__)             \
	MACRO(DefaultCropSize, uint16_t, 2U, __VA_ARGS__)                           \
	MACRO(DefaultCropSize, uint32_t, 2U, __VA_ARGS__)                           \
	MACRO(DefaultCropSize, lak::tiff::urational, 2U, __VA_ARGS__)               \
	MACRO(ColorMatrix1, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)  \
	MACRO(ColorMatrix2, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)  \
	MACRO(CameraCalibration1,                                                   \
	      lak::tiff::rational,                                                  \
	      lak::dynamic_extent,                                                  \
	      __VA_ARGS__)                                                          \
	MACRO(CameraCalibration2,                                                   \
	      lak::tiff::rational,                                                  \
	      lak::dynamic_extent,                                                  \
	      __VA_ARGS__)                                                          \
	MACRO(                                                                      \
	  ReductionMatrix1, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)  \
	MACRO(                                                                      \
	  ReductionMatrix2, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)  \
	MACRO(                                                                      \
	  AnalogBalance, lak::tiff::urational, lak::dynamic_extent, __VA_ARGS__)    \
	MACRO(AsShotNeutral, uint16_t, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(                                                                      \
	  AsShotNeutral, lak::tiff::urational, lak::dynamic_extent, __VA_ARGS__)    \
	MACRO(AsShotWhiteXY, lak::tiff::urational, 2U, __VA_ARGS__)                 \
	MACRO(BaselineExposure, lak::tiff::urational, 1U, __VA_ARGS__)              \
	MACRO(BaselineNoise, lak::tiff::urational, 1U, __VA_ARGS__)                 \
	MACRO(BaselineSharpness, lak::tiff::urational, 1U, __VA_ARGS__)             \
	MACRO(BayerGreenSplit, uint32_t, 1U, __VA_ARGS__)                           \
	MACRO(LinearResponseLimit, lak::tiff::urational, 1U, __VA_ARGS__)           \
	MACRO(CameraSerialNumber, char, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(LensInfo, lak::tiff::urational, 4U, __VA_ARGS__)                      \
	MACRO(ChromaBlurRadius, lak::tiff::urational, 1U, __VA_ARGS__)              \
	MACRO(AntiAliasStrength, lak::tiff::urational, 1U, __VA_ARGS__)             \
	MACRO(ShadowScale, lak::tiff::urational, 1U, __VA_ARGS__)                   \
	MACRO(DNGPrivateData, uint8_t, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(MakerNoteSafety, uint16_t, 1U, __VA_ARGS__)                           \
	MACRO(CalibrationIlluminant1, uint16_t, 1U, __VA_ARGS__)                    \
	MACRO(CalibrationIlluminant2, uint16_t, 1U, __VA_ARGS__)                    \
	MACRO(BestQualityScale, lak::tiff::urational, 1U, __VA_ARGS__)              \
	MACRO(RawDataUniqueID, uint8_t, 16U, __VA_ARGS__)                           \
	MACRO(OriginalRawFileName, char, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(OriginalRawFileName, uint8_t, lak::dynamic_extent, __VA_ARGS__)       \
	MACRO(OriginalRawFileData, byte_t, lak::dynamic_extent, __VA_ARGS__)        \
	MACRO(ActiveArea, uint16_t, 4U, __VA_ARGS__)                                \
	MACRO(ActiveArea, uint32_t, 4U, __VA_ARGS__)                                \
	MACRO(MaskedAreas, uint16_t, lak::dynamic_extent, __VA_ARGS__)              \
	MACRO(MaskedAreas, uint32_t, lak::dynamic_extent, __VA_ARGS__)              \
	MACRO(AsShotICCProfile, byte_t, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(AsShotPreProfileMatrix,                                               \
	      lak::tiff::rational,                                                  \
	      lak::dynamic_extent,                                                  \
	      __VA_ARGS__)                                                          \
	MACRO(CurrentICCProfile, byte_t, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(CurrentPreProfileMatrix,                                              \
	      lak::tiff::rational,                                                  \
	      lak::dynamic_extent,                                                  \
	      __VA_ARGS__)                                                          \
	/* 1.2.0.0 */                                                               \
	MACRO(ColorimetricReference, uint16_t, 1U, __VA_ARGS__)                     \
	MACRO(CameraCalibrationSignature, char, lak::dynamic_extent, __VA_ARGS__)   \
	MACRO(                                                                      \
	  CameraCalibrationSignature, uint8_t, lak::dynamic_extent, __VA_ARGS__)    \
	MACRO(ProfileCalibrationSignature, char, lak::dynamic_extent, __VA_ARGS__)  \
	MACRO(                                                                      \
	  ProfileCalibrationSignature, uint8_t, lak::dynamic_extent, __VA_ARGS__)   \
	MACRO(ExtraCameraProfiles, uint32_t, lak::dynamic_extent, __VA_ARGS__)      \
	MACRO(AsShotProfileName, char, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(AsShotProfileName, uint8_t, lak::dynamic_extent, __VA_ARGS__)         \
	MACRO(NoiseReductionApplied, lak::tiff::urational, 1U, __VA_ARGS__)         \
	MACRO(ProfileName, char, lak::dynamic_extent, __VA_ARGS__)                  \
	MACRO(ProfileName, uint8_t, lak::dynamic_extent, __VA_ARGS__)               \
	MACRO(ProfileHueSatMapDims, uint32_t, 3U, __VA_ARGS__)                      \
	MACRO(ProfileHueSatMapData1, f32_t, lak::dynamic_extent, __VA_ARGS__)       \
	MACRO(ProfileHueSatMapData2, f32_t, lak::dynamic_extent, __VA_ARGS__)       \
	MACRO(ProfileToneCurve, f32_t, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(ProfileEmbedPolicy, uint32_t, 1U, __VA_ARGS__)                        \
	MACRO(ProfileCopyright, char, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(ProfileCopyright, uint8_t, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(                                                                      \
	  ForwardMatrix1, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)    \
	MACRO(                                                                      \
	  ForwardMatrix2, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)    \
	MACRO(PreviewApplicationName, char, lak::dynamic_extent, __VA_ARGS__)       \
	MACRO(PreviewApplicationName, uint8_t, lak::dynamic_extent, __VA_ARGS__)    \
	MACRO(PreviewApplicationVersion, char, lak::dynamic_extent, __VA_ARGS__)    \
	MACRO(PreviewApplicationVersion, uint8_t, lak::dynamic_extent, __VA_ARGS__) \
	MACRO(PreviewSettingName, char, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(PreviewSettingName, uint8_t, lak::dynamic_extent, __VA_ARGS__)        \
	MACRO(PreviewSettingDigest, uint8_t, 16U, __VA_ARGS__)                      \
	MACRO(PreviewColorSpace, uint32_t, 1U, __VA_ARGS__)                         \
	MACRO(PreviewDateTime, char, lak::dynamic_extent, __VA_ARGS__)              \
	MACRO(RawImageDigest, uint8_t, 16U, __VA_ARGS__)                            \
	MACRO(OriginalRawFileDigest, uint8_t, 16U, __VA_ARGS__)                     \
	MACRO(SubTileBlockSize, uint16_t, 2U, __VA_ARGS__)                          \
	MACRO(SubTileBlockSize, uint32_t, 2U, __VA_ARGS__)                          \
	MACRO(RowInterleaveFactor, uint16_t, 1U, __VA_ARGS__)                       \
	MACRO(RowInterleaveFactor, uint32_t, 1U, __VA_ARGS__)                       \
	MACRO(ProfileLookTableDims, uint32_t, 2U, __VA_ARGS__)                      \
	MACRO(ProfileLookTableData, f32_t, lak::dynamic_extent, __VA_ARGS__)        \
	/* 1.3.0.0 */                                                               \
	MACRO(OpcodeList1, byte_t, lak::dynamic_extent, __VA_ARGS__)                \
	MACRO(OpcodeList2, byte_t, lak::dynamic_extent, __VA_ARGS__)                \
	MACRO(OpcodeList3, byte_t, lak::dynamic_extent, __VA_ARGS__)                \
	MACRO(NoiseProfile, f64_t, lak::dynamic_extent, __VA_ARGS__)                \
	/* 1.4.0.0 */                                                               \
	MACRO(OriginalDefaultFinalSize, uint16_t, 2U, __VA_ARGS__)                  \
	MACRO(OriginalDefaultFinalSize, uint32_t, 2U, __VA_ARGS__)                  \
	MACRO(OriginalBestQualityFinalSize, uint16_t, 2U, __VA_ARGS__)              \
	MACRO(OriginalBestQualityFinalSize, uint32_t, 2U, __VA_ARGS__)              \
	MACRO(OriginalDefaultCropSize, uint16_t, 2U, __VA_ARGS__)                   \
	MACRO(OriginalDefaultCropSize, uint32_t, 2U, __VA_ARGS__)                   \
	MACRO(OriginalDefaultCropSize, lak::tiff::urational, 2U, __VA_ARGS__)       \
	MACRO(DefaultUserCrop, lak::tiff::urational, 4U, __VA_ARGS__)               \
	MACRO(ProfileHueSatMapEncoding, uint32_t, 1U, __VA_ARGS__)                  \
	MACRO(ProfileLookTableEncoding, uint32_t, 1U, __VA_ARGS__)                  \
	MACRO(BaselineExposureOffset, lak::tiff::urational, 1U, __VA_ARGS__)        \
	MACRO(DefaultBlackRender, uint32_t, 1U, __VA_ARGS__)                        \
	MACRO(NewRawImageDigest, uint8_t, 16U, __VA_ARGS__)                         \
	MACRO(RawToPreviewGain, f64_t, 1U, __VA_ARGS__)                             \
	/* 1.5.0.0 */                                                               \
	MACRO(DepthFormat, uint16_t, 1U, __VA_ARGS__)                               \
	MACRO(DepthNear, lak::tiff::urational, 1U, __VA_ARGS__)                     \
	MACRO(DepthFar, lak::tiff::urational, 1U, __VA_ARGS__)                      \
	MACRO(DepthUnits, uint16_t, 1U, __VA_ARGS__)                                \
	MACRO(DepthMeasureType, uint16_t, 1U, __VA_ARGS__)                          \
	MACRO(EnhanceParams, char, lak::dynamic_extent, __VA_ARGS__)                \
	/* 1.6.0.0 */                                                               \
	MACRO(ProfileGainTableMap, byte_t, lak::dynamic_extent, __VA_ARGS__)        \
	MACRO(SemanticName, char, lak::dynamic_extent, __VA_ARGS__)                 \
	MACRO(SemanticInstanceID, char, lak::dynamic_extent, __VA_ARGS__)           \
	MACRO(CalibrationIlluminant3, uint16_t, 1U, __VA_ARGS__)                    \
	MACRO(CameraCalibration3,                                                   \
	      lak::tiff::rational,                                                  \
	      lak::dynamic_extent,                                                  \
	      __VA_ARGS__)                                                          \
	MACRO(ColorMatrix3, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)  \
	MACRO(                                                                      \
	  ForwardMatrix3, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)    \
	MACRO(IlluminantData1, byte_t, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(IlluminantData2, byte_t, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(IlluminantData3, byte_t, lak::dynamic_extent, __VA_ARGS__)            \
	MACRO(MaskSubArea, uint32_t, 4U, __VA_ARGS__)                               \
	MACRO(ProfileHueSatMapData3, f32_t, lak::dynamic_extent, __VA_ARGS__)       \
	MACRO(                                                                      \
	  ReductionMatrix3, lak::tiff::rational, lak::dynamic_extent, __VA_ARGS__)  \
	MACRO(RGBTables, byte_t, lak::dynamic_extent, __VA_ARGS__)                  \
	/* 1.7.0.0 */                                                               \
	MACRO(ProfileGainTableMap2, byte_t, lak::dynamic_extent, __VA_ARGS__)       \
	MACRO(ImageSequenceInfo, byte_t, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(ImageStats, byte_t, lak::dynamic_extent, __VA_ARGS__)                 \
	MACRO(ProfileDynamicRange, byte_t, 8U, __VA_ARGS__)                         \
	MACRO(ProfileGroupName, char, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(ProfileGroupName, uint8_t, lak::dynamic_extent, __VA_ARGS__)          \
	/* 1.7.1.0 */                                                               \
	MACRO(ColumnInterleaveFactor, uint16_t, 1U, __VA_ARGS__)                    \
	MACRO(ColumnInterleaveFactor, uint32_t, 1U, __VA_ARGS__)                    \
	MACRO(JXLDistance, f32_t, 1U, __VA_ARGS__)                                  \
	MACRO(JXLEffort, uint32_t, 1U, __VA_ARGS__)                                 \
	MACRO(JXLDecodeSpeed, uint32_t, 1U, __VA_ARGS__)

#define LAK_FOREACH_TIFF_TAG_TYPES(MACRO, ...)                                \
	LAK_FOREACH_TIFF6_TAG_TYPES(MACRO, __VA_ARGS__)                             \
	LAK_FOREACH_TIFF_EP_TAG_TYPES(MACRO, __VA_ARGS__)                           \
	LAK_FOREACH_EXIF_TAG_TYPES(MACRO, __VA_ARGS__)                              \
	LAK_FOREACH_DNG_TAG_TYPES(MACRO, __VA_ARGS__)

#define LAK_FOREACH_STRING_TIFF_TAG(MACRO, ...)                               \
	/* TIFF 6 */                                                                \
	MACRO(DocumentName, __VA_ARGS__)                                            \
	MACRO(ImageDescription, __VA_ARGS__)                                        \
	MACRO(Make, __VA_ARGS__)                                                    \
	MACRO(Model, __VA_ARGS__)                                                   \
	MACRO(PageName, __VA_ARGS__)                                                \
	MACRO(Software, __VA_ARGS__)                                                \
	MACRO(Artist, __VA_ARGS__)                                                  \
	MACRO(HostComputer, __VA_ARGS__)                                            \
	MACRO(InkNames, __VA_ARGS__)                                                \
	MACRO(TargetPrinter, __VA_ARGS__)                                           \
	MACRO(SMinSampleValue, __VA_ARGS__)                                         \
	MACRO(SMaxSampleValue, __VA_ARGS__)                                         \
	MACRO(Copyright, __VA_ARGS__)                                               \
	/* TIFF EP */                                                               \
	MACRO(BatteryLevel, __VA_ARGS__)                                            \
	MACRO(IPTC_NAA, __VA_ARGS__)                                                \
	MACRO(SpectralSensitivity, __VA_ARGS__)                                     \
	MACRO(SecurityClassification, __VA_ARGS__)                                  \
	MACRO(ImageHistory, __VA_ARGS__)                                            \
	/* EXIF */                                                                  \
	MACRO(LensMake, __VA_ARGS__)                                                \
	MACRO(LensModel, __VA_ARGS__)                                               \
	MACRO(LensSerialNumber, __VA_ARGS__)                                        \
	/* DNG */                                                                   \
	MACRO(UniqueCameraModel, __VA_ARGS__)                                       \
	MACRO(LocalizedCameraModel, __VA_ARGS__)                                    \
	MACRO(CameraSerialNumber, __VA_ARGS__)                                      \
	MACRO(OriginalRawFileName, __VA_ARGS__)                                     \
	MACRO(CameraCalibrationSignature, __VA_ARGS__)                              \
	MACRO(ProfileCalibrationSignature, __VA_ARGS__)                             \
	MACRO(AsShotProfileName, __VA_ARGS__)                                       \
	MACRO(ProfileName, __VA_ARGS__)                                             \
	MACRO(ProfileCopyright, __VA_ARGS__)                                        \
	MACRO(PreviewApplicationName, __VA_ARGS__)                                  \
	MACRO(PreviewApplicationVersion, __VA_ARGS__)                               \
	MACRO(PreviewSettingName, __VA_ARGS__)                                      \
	MACRO(PreviewDateTime, __VA_ARGS__)                                         \
	MACRO(EnhanceParams, __VA_ARGS__)                                           \
	MACRO(SemanticName, __VA_ARGS__)                                            \
	MACRO(SemanticInstanceID, __VA_ARGS__)                                      \
	MACRO(ProfileGroupName, __VA_ARGS__)
