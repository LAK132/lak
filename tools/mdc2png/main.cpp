#include <lak/file/mdc.hpp>
#include <lak/system/file.hpp>

#include <stb_image_write.h>

#include <iostream>

int main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: mdc2png <input> <output>\n";
		return EXIT_FAILURE;
	}

	lak::debugger.live_output_enabled = true;
	lak::debugger.live_errors_only    = true;

	lak::fs::path source = argv[1];
	lak::fs::path dest   = argv[2];

	std::cerr << "Loading " << source << "\n";

	auto file      = lak::read_file(source).UNWRAP();
	auto raw       = lak::from_bytes<lak::mdc::mdc>(file).UNWRAP().first;
	auto processed = (lak::image3_t)raw;

	std::cerr << "Converting to PNG\n";

	if (!stbi_write_png_to_func(
	      +[](void *dest, void *data, int size)
	      {
		      std::cerr << "Saving to " << dest << "\n";
		      if (!lak::save_file(*(lak::fs::path *)dest,
		                          lak::span<const byte_t>(lak::span(data, size))))
		      {
			      std::cerr << "Failed to save file " << *(lak::fs::path *)dest;
			      ABORT();
		      }
	      },
	      (void *)&dest,
	      int(processed.size().x),
	      int(processed.size().y),
	      3,
	      processed.data(),
	      int(processed.contig_size_bytes() / processed.size().y)))
	{
		std::cerr << "Failed to convert to png\n";
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
