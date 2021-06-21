#include "lak/file.hpp"

#include "lak/strconv.hpp"

template<typename TO>
inline lak::string<TO> lak::strconv(const fs::path &path)
{
  return lak::strconv<TO>(path.native());
}
