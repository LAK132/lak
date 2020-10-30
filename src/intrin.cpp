#include "lak/intrin.hpp"

void lak::cpuid(unsigned int index, unsigned int info[4])
{
#if defined(LAK_COMPILER_GNUC)
  __get_cpuid(index, info + 0, info + 1, info + 2, info + 3);
#elif defined(LAK_COMPILER_MSVC)
  __cpuid(reinterpret_cast<int *>(info), index);
#else
#  error "Compiler not supported"
#endif
}

// https://docs.microsoft.com/en-us/cpp/intrinsics/cpuid-cpuidex?view=msvc-160

lak::instruction_set lak::instruction_set::get()
{
  unsigned int cpu_info[4] = {};
  lak::cpuid(0, cpu_info);
  unsigned int max_level = cpu_info[0];

  lak::instruction_set result = {};

  if (max_level >= 1)
  {
    lak::cpuid(1, cpu_info);
    result.MMX   = (cpu_info[3] >> 23) & 1U;
    result.SSE   = (cpu_info[3] >> 25) & 1U;
    result.SSE2  = (cpu_info[3] >> 26) & 1U;
    result.SSE3  = (cpu_info[2] >> 0) & 1U;
    result.SSSE3 = (cpu_info[2] >> 9) & 1U;
    result.SSE41 = (cpu_info[2] >> 19) & 1U;
    result.SSE42 = (cpu_info[2] >> 20) & 1U;
    result.AVX   = (cpu_info[2] >> 28) & 1U;
  }

  if (max_level >= 7)
  {
    lak::cpuid(7, cpu_info);
    result.AVX2     = (cpu_info[1] >> 5) & 1U;
    result.AVX512F  = (cpu_info[1] >> 16) & 1U;
    result.AVX512PF = (cpu_info[1] >> 26) & 1U;
    result.AVX512ER = (cpu_info[1] >> 27) & 1U;
    result.AVX512CD = (cpu_info[1] >> 28) & 1U;
  }

  return result;
}