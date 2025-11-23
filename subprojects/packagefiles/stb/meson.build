project(
	'stb',
	'c',
	license: 'MIT OR Unlicense',
	license_files: 'LICENSE',
)

comp = meson.get_compiler('c')

if comp.get_id() == 'msvc'
	c_args = [
	]
else
	c_args = [
		'-Wno-unknown-warning-option',
		'-Wno-incompatible-function-pointer-types',
	]
endif

add_project_arguments(c_args, language: ['c'])

incdir = include_directories('.')

stb_libraries = {
	'c_lexer': {
		'version': '0.12',
		'c_args': [
		],
	},
	'divide': {
		'version': '0.94',
		'c_args': [
			# C_INTEGER_DIVISION_TRUNCATES
			# C_INTEGER_DIVISION_FLOORS
		],
	},
	'dxt': {
		'version': '1.12',
		'c_args': [
		],
	},
	'easy_font': {
		'version': '1.1',
		'c_args': [
		],
	},
	'herringbone_wang_tile': {
		'version': '0.7',
		'c_args': [
			# STB_HBWANG_RAND()
			# STB_HBWANG_ASSERT(X)
			# STB_HBWANG_STATIC
			# STB_HBWANG_NO_REPITITION_REDUCTION
			# STB_HBWANG_MAX_X
			# STB_HBWANG_MAX_Y
		],
	},
	'hexwave': {
		'version': '0.5',
		'c_args': [
		],
	},
	'image_resize': {
		'version': '0.97',
		'c_args': [
			# STBIR_ASSERT(X)
			# STBIR_MALLOC
			# STBIR_FREE
			# STBIR_SATURATE_INT
			# STBIR_DEFAULT_FILTER_UPSAMPLE
			# STBIR_DEFAULT_FILTER_DOWNSAMPLE
			# STBIR_PROGRESS_REPORT
			# STBIR_MAX_CHANNELS
		],
	},
	'image_resize2': {
		'version': '2.17',
		'c_args': [
			# STBIR_ASSERT(X)
			# STBIR_MALLOC
			# STBIR_FREE
			# STBIR_NO_SIMD
			# STBIR_[NO_]SSE2
			# STBIR_[NO_]AVX
			# STBIR_[NO_]AVX2
			# STBIR_FP16C
			# STBIR_USE_FMA
			# STBIR_FLOAT_LOW_CLAMP
			# STBIR_FLOAT_HIGH_CLAMP
			# STBIR_DEFAULT_FILTER_UPSAMPLE
			# STBIR_DEFAULT_FILTER_DOWNSAMPLE
			# STBIR_CEILF
			# STBIR_FLOORF
		],
	},
	'image_write': {
		'version': '1.16',
		'c_args': [
			# STBIW_ASSERT(X)
			# STBIW_MALLOC
			# STBIW_REALLOC
			# STBIW_FREE
			# STBIW_MEMMOVE
			# STBIW_ZLIB_COMPRESS
			# STBIW_WINDOWS_UTF8
			# STBI_WRITE_NO_STDIO
		],
	},
	'image': {
		'version': '2.30',
		'c_args': [
			# STBI_ASSERT(X)
			# STBI_MALLOC
			# STBI_REALLOC
			# STBI_FREE
			# STBI_NO_STDIO
		],
	},
	'include': {
		'version': '0.02',
		'c_args': [
			# STB_INCLUDE_LINE_GLSL
			# STB_INCLUDE_LINE_NONE
		],
	},
	'perlin': {
		'version': '0.5',
		'c_args': [
		],
	},
	'rect_pack': {
		'version': '1.01',
		'c_args': [
			# STBRP_SORT
			# STBRP_ASSERT
		],
	},
	'sprintf': {
		'version': '1.10',
		'c_args': [
			# STB_SPRINTF_DECORATE
		],
	},
	'truetype': {
		'version': '1.26',
		'c_args': [
			# STBTT_RASTERIZER_VERSION
		],
	},
	'vorbis': {
		'version': '1.22',
		'c_args': [
			# STB_VORBIS_NO_PUSHDATA_API
			# STB_VORBIS_NO_PULLDATA_API
			# STB_VORBIS_NO_STDIO
			# STB_VORBIS_NO_INTEGER_CONVERSION
			# STB_VORBIS_NO_FAST_SCALED_FLOAT
			# STB_VORBIS_MAX_CHANNELS
			# STB_VORBIS_PUSHDATA_CRC_COUNT
			# STB_VORBIS_FAST_HUFFMAN_LENGTH
			# STB_VORBIS_FAST_BINARY_LENGTH
			# STB_VORBIS_FAST_HUFFMAN_INT
			# STB_VORBIS_NO_HUFFMAN_BINARY_SEARCH
			# STB_VORBIS_DIVIDES_IN_RESIDUE
			# STB_VORBIS_DIVIDES_IN_CODEBOOK
			# STB_VORBIS_DIVIDE_TABLE
			# STB_VORBIS_NO_INLINE_DECODE
			# STB_VORBIS_NO_DEFER_FLOOR
		],
	},
}

foreach library, info : stb_libraries
	lib = static_library(
		'stb_@0@-@1@'.format(library, info.get('version')),
		'stb_@0@.c'.format(library),
		info.get('c_args'),
		include_directories: incdir,
	)
	dep = declare_dependency(
		link_with: lib,
		include_directories: incdir,
		version: info.get('version'),
	)
	meson.override_dependency(
		'stb_@0@-@1@'.format(library, info.get('version')), dep)
	meson.override_dependency(
		'stb_@0@'.format(library), dep)
endforeach
