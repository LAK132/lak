#include <iostream>
#include <optional>
#include <string>
#include <vector>

static const std::string windows_intel_runner = "windows-2025-vs2026";
static const std::string windows_arm_runner   = "windows-11-vs2026-arm";
static const std::string ubuntu_intel_runner  = "ubuntu-26.04";
static const std::string ubuntu_arm_runner    = "ubuntu-26.04-arm";
static const std::string macos_intel_runner   = "macos-26-intel";
static const std::string macos_arm_runner     = "macos-26";

struct os_t
{
	std::string runner;
	std::string system;
	std::string arch;

	inline friend std::ostream &operator<<(std::ostream &strm, const os_t &os)
	{
		strm << "os:{";
		strm << "runner:\"" << os.runner << "\",";
		strm << "system:\"" << os.system << "\",";
		strm << "arch:\"" << os.arch << "\"";
		strm << "},";
		return strm;
	}
};

enum struct build_type_t
{
	plain,
	debug,
	debug_optimised,
	release,
	minsize,
};

inline std::string build_type_string(build_type_t bt)
{
	using namespace std::literals::string_literals;

	switch (bt)
	{
		case build_type_t::plain:           return "plain"s;
		case build_type_t::debug:           return "debug"s;
		case build_type_t::debug_optimised: return "debugoptimized"s;
		case build_type_t::release:         return "release"s;
		case build_type_t::minsize:         return "minsize"s;

		default: std::cerr << "invalid build type\n"; std::abort();
	}
}

struct setup_t
{
	std::string setup_options;
	build_type_t build_type = build_type_t::debug;
	std::string compile_options;
};

struct target_t
{
	std::vector<setup_t> setups;
	std::string target;
	std::string run;
};

inline std::vector<target_t> operator+(std::vector<target_t> a,
                                       std::vector<target_t> b)
{
	std::vector<target_t> result;
	result.reserve(a.size() + b.size());
	for (auto &t : a) result.push_back(std::move(t));
	for (auto &t : b) result.push_back(std::move(t));
	return result;
}

struct matrix_entry_t
{
	os_t os;
	std::vector<target_t> targets;

	inline friend std::ostream &operator<<(std::ostream &strm,
	                                       const matrix_entry_t &e)
	{
		size_t i = e.targets.size();
		for (const auto &t : e.targets)
		{
			--i;
			i += t.setups.size();
			for (const auto &s : t.setups)
			{
				strm << "{";
				strm << e.os;
				strm << "setup_options:\"" << s.setup_options << "\",";
				strm << "compile_options:\"" << s.compile_options << "\",";
				strm << "build_type:\"" << build_type_string(s.build_type) << "\",";
				strm << "target:\"" << t.target << "\",";
				strm << "run:\"" << t.run << "\"";
				strm << "}";
				if (--i != 0) strm << ",";
			}
		}
		return strm;
	}
};

inline std::string quote(const std::string &str)
{
	using namespace std::literals::string_literals;
	return "\\\""s + str + "\\\""s;
}

inline std::string meson_option(const std::string &option,
                                const std::vector<std::string> &args)
{
	using namespace std::literals::string_literals;

	std::string result = "-D"s + option + "="s;
	if (!args.empty())
	{
		result += args[0];
		for (size_t i = 1U; i < args.size(); ++i) result += ","s + args[i];
	}
	if (args.size() > 1U) result = quote(result);
	result += " "s;
	return result;
}

inline void print_matrix(const std::vector<matrix_entry_t> &matrix)
{
	std::cout << "matrix=[";
	size_t i = matrix.size();
	for (const auto &e : matrix)
	{
		std::cout << e;
		if (--i != 0) std::cout << ",";
		std::cout << "";
	}
	std::cout << "]\n";
}
