#define LAK_FOREACH_TIFF6_TAG_VALUE(MACRO, ...)                               \
	MACRO(0xFE, NewSubfileType, __VA_ARGS__)                                    \
	MACRO(0xFF, SubfileType, __VA_ARGS__)                                       \
	MACRO(0x100, ImageWidth, __VA_ARGS__)                                       \
	MACRO(0x101, ImageLength, __VA_ARGS__)                                      \
	MACRO(0x102, BitsPerSample, __VA_ARGS__)                                    \
	MACRO(0x103, Compression, __VA_ARGS__)                                      \
	MACRO(0x106, PhotometricInterpretation, __VA_ARGS__)                        \
	MACRO(0x107, Thresholding, __VA_ARGS__)                                     \
	MACRO(0x108, CellWidth, __VA_ARGS__)                                        \
	MACRO(0x109, CellLength, __VA_ARGS__)                                       \
	MACRO(0x10A, FillOrder, __VA_ARGS__)                                        \
	MACRO(0x10D, DocumentName, __VA_ARGS__)                                     \
	MACRO(0x10E, ImageDescription, __VA_ARGS__)                                 \
	MACRO(0x10F, Make, __VA_ARGS__)                                             \
	MACRO(0x110, Model, __VA_ARGS__)                                            \
	MACRO(0x111, StripOffsets, __VA_ARGS__)                                     \
	MACRO(0x112, Orientation, __VA_ARGS__)                                      \
	MACRO(0x115, SamplesPerPixel, __VA_ARGS__)                                  \
	MACRO(0x116, RowsPerStrip, __VA_ARGS__)                                     \
	MACRO(0x117, StripByteCounts, __VA_ARGS__)                                  \
	MACRO(0x118, MinSampleValue, __VA_ARGS__)                                   \
	MACRO(0x119, MaxSampleValue, __VA_ARGS__)                                   \
	MACRO(0x11A, XResolution, __VA_ARGS__)                                      \
	MACRO(0x11B, YResolution, __VA_ARGS__)                                      \
	MACRO(0x11C, PlanarConfiguration, __VA_ARGS__)                              \
	MACRO(0x11D, PageName, __VA_ARGS__)                                         \
	MACRO(0x11E, XPosition, __VA_ARGS__)                                        \
	MACRO(0x11F, YPosition, __VA_ARGS__)                                        \
	MACRO(0x120, FreeOffsets, __VA_ARGS__)                                      \
	MACRO(0x121, FreeByteCounts, __VA_ARGS__)                                   \
	MACRO(0x122, GrayResponseUnit, __VA_ARGS__)                                 \
	MACRO(0x123, GrayResponseCurve, __VA_ARGS__)                                \
	MACRO(0x124, T4Options, __VA_ARGS__)                                        \
	MACRO(0x125, T6Options, __VA_ARGS__)                                        \
	MACRO(0x128, ResolutionUnit, __VA_ARGS__)                                   \
	MACRO(0x129, PageNumber, __VA_ARGS__)                                       \
	MACRO(0x12D, TransferFunction, __VA_ARGS__)                                 \
	MACRO(0x131, Software, __VA_ARGS__)                                         \
	MACRO(0x132, DateTime, __VA_ARGS__)                                         \
	MACRO(0x13B, Artist, __VA_ARGS__)                                           \
	MACRO(0x13C, HostComputer, __VA_ARGS__)                                     \
	MACRO(0x13D, Predictor, __VA_ARGS__)                                        \
	MACRO(0x13E, WhitePoint, __VA_ARGS__)                                       \
	MACRO(0x13F, PrimaryChomaticities, __VA_ARGS__)                             \
	MACRO(0x140, ColorMap, __VA_ARGS__)                                         \
	MACRO(0x141, HalftoneHints, __VA_ARGS__)                                    \
	MACRO(0x142, TileWidth, __VA_ARGS__)                                        \
	MACRO(0x143, TileLength, __VA_ARGS__)                                       \
	MACRO(0x144, TileOffsets, __VA_ARGS__)                                      \
	MACRO(0x145, TileByteCounts, __VA_ARGS__)                                   \
	MACRO(0x14C, InkSet, __VA_ARGS__)                                           \
	MACRO(0x14D, InkNames, __VA_ARGS__)                                         \
	MACRO(0x14E, NumberOfInks, __VA_ARGS__)                                     \
	MACRO(0x150, DotRange, __VA_ARGS__)                                         \
	MACRO(0x151, TargetPrinter, __VA_ARGS__)                                    \
	MACRO(0x152, ExtraSamples, __VA_ARGS__)                                     \
	MACRO(0x153, SampleFormat, __VA_ARGS__)                                     \
	MACRO(0x154, SMinSampleValue, __VA_ARGS__)                                  \
	MACRO(0x155, SMaxSampleValue, __VA_ARGS__)                                  \
	MACRO(0x156, TransferRange, __VA_ARGS__)                                    \
	MACRO(0x200, JPEGProc, __VA_ARGS__)                                         \
	MACRO(0x201, JPEGInterchangeFormat, __VA_ARGS__)                            \
	MACRO(0x202, JPEGInterchangeFormatLength, __VA_ARGS__)                      \
	MACRO(0x203, JPEGRestartInterval, __VA_ARGS__)                              \
	MACRO(0x205, JPEGLosslessPredictors, __VA_ARGS__)                           \
	MACRO(0x206, JPEGPointTransforms, __VA_ARGS__)                              \
	MACRO(0x207, JPEGQTables, __VA_ARGS__)                                      \
	MACRO(0x208, JPEGDCTables, __VA_ARGS__)                                     \
	MACRO(0x209, JPEGACTables, __VA_ARGS__)                                     \
	MACRO(0x211, YCbCrCoefficients, __VA_ARGS__)                                \
	MACRO(0x212, YCbCrSubSampling, __VA_ARGS__)                                 \
	MACRO(0x213, YCbCrPositioning, __VA_ARGS__)                                 \
	MACRO(0x214, ReferenceBlackWhite, __VA_ARGS__)                              \
	MACRO(0x8298, Copyright, __VA_ARGS__)

#define LAK_FOREACH_TIFF_EP_TAG_VALUE(MACRO, ...)                             \
	MACRO(0x14A, SubIFDs, __VA_ARGS__)                                          \
	MACRO(0x15B, JPEGTables, __VA_ARGS__)                                       \
	MACRO(0x828D, CFARepeatPatternDim, __VA_ARGS__)                             \
	MACRO(0x828E, CFAPattern, __VA_ARGS__)                                      \
	MACRO(0x828F, BatteryLevel, __VA_ARGS__)                                    \
	MACRO(0x829A, ExposureTime, __VA_ARGS__)                                    \
	MACRO(0x829D, FNumber, __VA_ARGS__)                                         \
	MACRO(0x83BB, IPTC_NAA, __VA_ARGS__)                                        \
	MACRO(0x8773, InterColorProfile, __VA_ARGS__)                               \
	MACRO(0x8822, ExposureProgram, __VA_ARGS__)                                 \
	MACRO(0x8824, SpectralSensitivity, __VA_ARGS__)                             \
	MACRO(0x8825, GPSInfo, __VA_ARGS__)                                         \
	MACRO(0x8827, ISOSpeedRatings, __VA_ARGS__)                                 \
	MACRO(0x8828, OECF, __VA_ARGS__)                                            \
	MACRO(0x8829, Interlace, __VA_ARGS__)                                       \
	MACRO(0x882A, TimeZoneOffset, __VA_ARGS__)                                  \
	MACRO(0x882B, SelfTimerMode, __VA_ARGS__)                                   \
	MACRO(0x9003, DateTimeOriginal, __VA_ARGS__)                                \
	MACRO(0x9102, CompressedBitsPerPixel, __VA_ARGS__)                          \
	MACRO(0x9201, ShutterSpeedValue, __VA_ARGS__)                               \
	MACRO(0x9202, ApertureValue, __VA_ARGS__)                                   \
	MACRO(0x9203, BrightnessValue, __VA_ARGS__)                                 \
	MACRO(0x9204, ExposureBiasValue, __VA_ARGS__)                               \
	MACRO(0x9205, MaxApertureValue, __VA_ARGS__)                                \
	MACRO(0x9206, SubjectDistance, __VA_ARGS__)                                 \
	MACRO(0x9207, MeteringMode, __VA_ARGS__)                                    \
	MACRO(0x9208, LightSource, __VA_ARGS__)                                     \
	MACRO(0x9209, Flash, __VA_ARGS__)                                           \
	MACRO(0x920A, FocalLength, __VA_ARGS__)                                     \
	MACRO(0x920B, FlashEnergy, __VA_ARGS__)                                     \
	MACRO(0x920C, SpatialFrequencyResponse, __VA_ARGS__)                        \
	MACRO(0x920D, Noise, __VA_ARGS__)                                           \
	MACRO(0x920E, FocalPlaneXResolution, __VA_ARGS__)                           \
	MACRO(0x920F, FocalPlaneYResolution, __VA_ARGS__)                           \
	MACRO(0x9210, FocalPlaneResolutionUnit, __VA_ARGS__)                        \
	MACRO(0x9211, ImageNumber, __VA_ARGS__)                                     \
	MACRO(0x9212, SecurityClassification, __VA_ARGS__)                          \
	MACRO(0x9213, ImageHistory, __VA_ARGS__)                                    \
	MACRO(0x9214, SubjectLocation, __VA_ARGS__)                                 \
	MACRO(0x9215, ExposureIndex, __VA_ARGS__)                                   \
	MACRO(0x9216, TIFF_EPStandardID, __VA_ARGS__)                               \
	MACRO(0x9217, SensingMethod, __VA_ARGS__)

#define LAK_FOREACH_EXIF_TAG_VALUE(MACRO, ...)                                \
	MACRO(0x2BC, ApplicationNotes, __VA_ARGS__)                                 \
	MACRO(0xA433, LensMake, __VA_ARGS__)                                        \
	MACRO(0xA434, LensModel, __VA_ARGS__)                                       \
	MACRO(0xA435, LensSerialNumber, __VA_ARGS__)                                \
	MACRO(0x8769, ExifOffset, __VA_ARGS__)

#define LAK_FOREACH_DNG_TAG_VALUE(MACRO, ...)                                 \
	/* 1.1.0.0 */                                                               \
	MACRO(0xC612, DNGVersion, __VA_ARGS__)                                      \
	MACRO(0xC613, DNGBackwardVersion, __VA_ARGS__)                              \
	MACRO(0xC614, UniqueCameraModel, __VA_ARGS__)                               \
	MACRO(0xC615, LocalizedCameraModel, __VA_ARGS__)                            \
	MACRO(0xC616, CFAPlaneColor, __VA_ARGS__)                                   \
	MACRO(0xC617, CFALayout, __VA_ARGS__)                                       \
	MACRO(0xC618, LinearizationTable, __VA_ARGS__)                              \
	MACRO(0xC619, BlackLevelRepeatDim, __VA_ARGS__)                             \
	MACRO(0xC61A, BlackLevel, __VA_ARGS__)                                      \
	MACRO(0xC61B, BlackLevelDeltaH, __VA_ARGS__)                                \
	MACRO(0xC61C, BlackLevelDeltaV, __VA_ARGS__)                                \
	MACRO(0xC61D, WhiteLevel, __VA_ARGS__)                                      \
	MACRO(0xC61E, DefaultScale, __VA_ARGS__)                                    \
	MACRO(0xC61F, DefaultCropOrigin, __VA_ARGS__)                               \
	MACRO(0xC620, DefaultCropSize, __VA_ARGS__)                                 \
	MACRO(0xC621, ColorMatrix1, __VA_ARGS__)                                    \
	MACRO(0xC622, ColorMatrix2, __VA_ARGS__)                                    \
	MACRO(0xC623, CameraCalibration1, __VA_ARGS__)                              \
	MACRO(0xC624, CameraCalibration2, __VA_ARGS__)                              \
	MACRO(0xC625, ReductionMatrix1, __VA_ARGS__)                                \
	MACRO(0xC626, ReductionMatrix2, __VA_ARGS__)                                \
	MACRO(0xC627, AnalogBalance, __VA_ARGS__)                                   \
	MACRO(0xC628, AsShotNeutral, __VA_ARGS__)                                   \
	MACRO(0xC629, AsShotWhiteXY, __VA_ARGS__)                                   \
	MACRO(0xC62A, BaselineExposure, __VA_ARGS__)                                \
	MACRO(0xC62B, BaselineNoise, __VA_ARGS__)                                   \
	MACRO(0xC62C, BaselineSharpness, __VA_ARGS__)                               \
	MACRO(0xC62D, BayerGreenSplit, __VA_ARGS__)                                 \
	MACRO(0xC62E, LinearResponseLimit, __VA_ARGS__)                             \
	MACRO(0xC62F, CameraSerialNumber, __VA_ARGS__)                              \
	MACRO(0xC630, LensInfo, __VA_ARGS__)                                        \
	MACRO(0xC631, ChromaBlurRadius, __VA_ARGS__)                                \
	MACRO(0xC632, AntiAliasStrength, __VA_ARGS__)                               \
	MACRO(0xC633, ShadowScale, __VA_ARGS__)                                     \
	MACRO(0xC634, DNGPrivateData, __VA_ARGS__)                                  \
	MACRO(0xC635, MakerNoteSafety, __VA_ARGS__)                                 \
	MACRO(0xC65A, CalibrationIlluminant1, __VA_ARGS__)                          \
	MACRO(0xC65B, CalibrationIlluminant2, __VA_ARGS__)                          \
	MACRO(0xC65C, BestQualityScale, __VA_ARGS__)                                \
	MACRO(0xC65D, RawDataUniqueID, __VA_ARGS__)                                 \
	MACRO(0xC68B, OriginalRawFileName, __VA_ARGS__)                             \
	MACRO(0xC68C, OriginalRawFileData, __VA_ARGS__)                             \
	MACRO(0xC68D, ActiveArea, __VA_ARGS__)                                      \
	MACRO(0xC68E, MaskedAreas, __VA_ARGS__)                                     \
	MACRO(0xC68F, AsShotICCProfile, __VA_ARGS__)                                \
	MACRO(0xC690, AsShotPreProfileMatrix, __VA_ARGS__)                          \
	MACRO(0xC691, CurrentICCProfile, __VA_ARGS__)                               \
	MACRO(0xC692, CurrentPreProfileMatrix, __VA_ARGS__)                         \
	/* 1.2.0.0 */                                                               \
	MACRO(0xC6BF, ColorimetricReference, __VA_ARGS__)                           \
	MACRO(0xC6F3, CameraCalibrationSignature, __VA_ARGS__)                      \
	MACRO(0xC6F4, ProfileCalibrationSignature, __VA_ARGS__)                     \
	MACRO(0xC6F5, ExtraCameraProfiles, __VA_ARGS__)                             \
	MACRO(0xC6F6, AsShotProfileName, __VA_ARGS__)                               \
	MACRO(0xC6F7, NoiseReductionApplied, __VA_ARGS__)                           \
	MACRO(0xC6F8, ProfileName, __VA_ARGS__)                                     \
	MACRO(0xC6F9, ProfileHueSatMapDims, __VA_ARGS__)                            \
	MACRO(0xC6FA, ProfileHueSatMapData1, __VA_ARGS__)                           \
	MACRO(0xC6FB, ProfileHueSatMapData2, __VA_ARGS__)                           \
	MACRO(0xC6FC, ProfileToneCurve, __VA_ARGS__)                                \
	MACRO(0xC6FD, ProfileEmbedPolicy, __VA_ARGS__)                              \
	MACRO(0xC6FE, ProfileCopyright, __VA_ARGS__)                                \
	MACRO(0xC714, ForwardMatrix1, __VA_ARGS__)                                  \
	MACRO(0xC715, ForwardMatrix2, __VA_ARGS__)                                  \
	MACRO(0xC716, PreviewApplicationName, __VA_ARGS__)                          \
	MACRO(0xC717, PreviewApplicationVersion, __VA_ARGS__)                       \
	MACRO(0xC718, PreviewSettingName, __VA_ARGS__)                              \
	MACRO(0xC719, PreviewSettingDigest, __VA_ARGS__)                            \
	MACRO(0xC71A, PreviewColorSpace, __VA_ARGS__)                               \
	MACRO(0xC71B, PreviewDateTime, __VA_ARGS__)                                 \
	MACRO(0xC71C, RawImageDigest, __VA_ARGS__)                                  \
	MACRO(0xC71D, OriginalRawFileDigest, __VA_ARGS__)                           \
	MACRO(0xC71E, SubTileBlockSize, __VA_ARGS__)                                \
	MACRO(0xC71F, RowInterleaveFactor, __VA_ARGS__)                             \
	MACRO(0xC725, ProfileLookTableDims, __VA_ARGS__)                            \
	MACRO(0xC726, ProfileLookTableData, __VA_ARGS__)                            \
	MACRO(0xC740, OpcodeList1, __VA_ARGS__)                                     \
	MACRO(0xC741, OpcodeList2, __VA_ARGS__)                                     \
	MACRO(0xC74E, OpcodeList3, __VA_ARGS__)                                     \
	MACRO(0xC761, NoiseProfile, __VA_ARGS__)                                    \
	/* 1.4.0.0 */                                                               \
	MACRO(0xC791, OriginalDefaultFinalSize, __VA_ARGS__)                        \
	MACRO(0xC792, OriginalBestQualityFinalSize, __VA_ARGS__)                    \
	MACRO(0xC793, OriginalDefaultCropSize, __VA_ARGS__)                         \
	MACRO(0xC7B5, DefaultUserCrop, __VA_ARGS__)                                 \
	MACRO(0xC7A3, ProfileHueSatMapEncoding, __VA_ARGS__)                        \
	MACRO(0xC7A4, ProfileLookTableEncoding, __VA_ARGS__)                        \
	MACRO(0xC7A5, BaselineExposureOffset, __VA_ARGS__)                          \
	MACRO(0xC7A6, DefaultBlackRender, __VA_ARGS__)                              \
	MACRO(0xC7A7, NewRawImageDigest, __VA_ARGS__)                               \
	MACRO(0xC7A8, RawToPreviewGain, __VA_ARGS__)                                \
	/* 1.5.0.0 */                                                               \
	MACRO(0xC7E9, DepthFormat, __VA_ARGS__)                                     \
	MACRO(0xC7EA, DepthNear, __VA_ARGS__)                                       \
	MACRO(0xC7EB, DepthFar, __VA_ARGS__)                                        \
	MACRO(0xC7EC, DepthUnits, __VA_ARGS__)                                      \
	MACRO(0xC7ED, DepthMeasureType, __VA_ARGS__)                                \
	MACRO(0xC7EE, EnhanceParams, __VA_ARGS__)                                   \
	/* 1.6.0.0 */                                                               \
	MACRO(0xCD2D, ProfileGainTableMap, __VA_ARGS__)                             \
	MACRO(0xCD2E, SemanticName, __VA_ARGS__)                                    \
	MACRO(0xCD30, SemanticInstanceID, __VA_ARGS__)                              \
	MACRO(0xCD31, CalibrationIlluminant3, __VA_ARGS__)                          \
	MACRO(0xCD32, CameraCalibration3, __VA_ARGS__)                              \
	MACRO(0xCD33, ColorMatrix3, __VA_ARGS__)                                    \
	MACRO(0xCD34, ForwardMatrix3, __VA_ARGS__)                                  \
	MACRO(0xCD35, IlluminantData1, __VA_ARGS__)                                 \
	MACRO(0xCD36, IlluminantData2, __VA_ARGS__)                                 \
	MACRO(0xCD37, IlluminantData3, __VA_ARGS__)                                 \
	MACRO(0xCD38, MaskSubArea, __VA_ARGS__)                                     \
	MACRO(0xCD39, ProfileHueSatMapData3, __VA_ARGS__)                           \
	MACRO(0xCD3A, ReductionMatrix3, __VA_ARGS__)                                \
	MACRO(0xCD3F, RGBTables, __VA_ARGS__)                                       \
	/* 1.7.0.0 */                                                               \
	MACRO(0xCD40, ProfileGainTableMap2, __VA_ARGS__)                            \
	MACRO(0xCD44, ImageSequenceInfo, __VA_ARGS__)                               \
	MACRO(0xCD46, ImageStats, __VA_ARGS__)                                      \
	MACRO(0xCD47, ProfileDynamicRange, __VA_ARGS__)                             \
	MACRO(0xCD48, ProfileGroupName, __VA_ARGS__)                                \
	/* 1.7.1.0 */                                                               \
	MACRO(0xCD43, ColumnInterleaveFactor, __VA_ARGS__)                          \
	MACRO(0xCD49, JXLDistance, __VA_ARGS__)                                     \
	MACRO(0xCD4A, JXLEffort, __VA_ARGS__)                                       \
	MACRO(0xCD4B, JXLDecodeSpeed, __VA_ARGS__)

#define LAK_FOREACH_TIFF_TAG_VALUE(MACRO, ...)                                \
	LAK_FOREACH_TIFF6_TAG_VALUE(MACRO, __VA_ARGS__)                             \
	LAK_FOREACH_TIFF_EP_TAG_VALUE(MACRO, __VA_ARGS__)                           \
	LAK_FOREACH_EXIF_TAG_VALUE(MACRO, __VA_ARGS__)                              \
	LAK_FOREACH_DNG_TAG_VALUE(MACRO, __VA_ARGS__)

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
	MACRO(PrimaryChomaticities, lak::tiff::urational, 6U, __VA_ARGS__)          \
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
	MACRO(LensMake, char, lak::dynamic_extent, __VA_ARGS__)                     \
	MACRO(LensModel, char, lak::dynamic_extent, __VA_ARGS__)                    \
	MACRO(LensSerialNumber, char, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(ExifOffset, uint32_t, 1U, __VA_ARGS__)

#define LAK_FOREACH_DNG_TAG_TYPES(MACRO, ...)                                 \
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
	MACRO(OpcodeList1, byte_t, lak::dynamic_extent, __VA_ARGS__)                \
	MACRO(OpcodeList2, byte_t, lak::dynamic_extent, __VA_ARGS__)                \
	MACRO(OpcodeList3, byte_t, lak::dynamic_extent, __VA_ARGS__)                \
	MACRO(NoiseProfile, f64_t, lak::dynamic_extent, __VA_ARGS__)                \
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
	MACRO(DepthFormat, uint16_t, 1U, __VA_ARGS__)                               \
	MACRO(DepthNear, lak::tiff::urational, 1U, __VA_ARGS__)                     \
	MACRO(DepthFar, lak::tiff::urational, 1U, __VA_ARGS__)                      \
	MACRO(DepthUnits, uint16_t, 1U, __VA_ARGS__)                                \
	MACRO(DepthMeasureType, uint16_t, 1U, __VA_ARGS__)                          \
	MACRO(EnhanceParams, char, lak::dynamic_extent, __VA_ARGS__)                \
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
	MACRO(ProfileGainTableMap2, byte_t, lak::dynamic_extent, __VA_ARGS__)       \
	MACRO(ImageSequenceInfo, byte_t, lak::dynamic_extent, __VA_ARGS__)          \
	MACRO(ImageStats, byte_t, lak::dynamic_extent, __VA_ARGS__)                 \
	MACRO(ProfileDynamicRange, byte_t, 8U, __VA_ARGS__)                         \
	MACRO(ProfileGroupName, char, lak::dynamic_extent, __VA_ARGS__)             \
	MACRO(ProfileGroupName, uint8_t, lak::dynamic_extent, __VA_ARGS__)          \
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
