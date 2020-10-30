#ifndef LAK_INTRIN_HPP
#define LAK_INTRIN_HPP

#include "lak/compiler.hpp"

#if defined(LAK_COMPILER_GNUC)
#  include <cpuid.h>
#  include <immintrin.h>
#elif defined(LAK_COMPILER_MSVC)
#  include <immintrin.h>
#else
#  error "Compiler not supported"
#endif

namespace lak
{
  void cpuid(unsigned int index, unsigned int info[4]);

  struct instruction_set
  {
    bool MMX;
    bool SSE;
    bool SSE2;
    bool SSE3;
    bool SSSE3;
    bool SSE41;
    bool SSE42;
    bool AVX;
    bool AVX2;
    bool AVX512F;
    bool AVX512PF;
    bool AVX512ER;
    bool AVX512CD;

    static instruction_set get();
  };
}

#endif
