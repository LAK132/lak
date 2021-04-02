#ifndef LAK_UWUIFY_HPP
#define LAK_UWUIFY_HPP

#include "lak/span.hpp"
#include "lak/string.hpp"

namespace lak
{
  template<typename CHAR>
  lak::string<CHAR> uwuify(lak::string<CHAR> str)
  {
    for (auto it = str.begin(); it != str.end(); ++it)
    {
      switch (*it)
      {
        case CHAR('r'): [[fallthrough]];
        case CHAR('l'): *it = CHAR('w'); break;

        case CHAR('R'): [[fallthrough]];
        case CHAR('L'): *it = CHAR('W'); break;

        case CHAR('o'):
          it = str.insert(str.insert(it + 1, CHAR('w')) + 1, CHAR('o'));
          break;
        case CHAR('O'):
          it = str.insert(str.insert(it + 1, CHAR('W')) + 1, CHAR('O'));
          break;

        case CHAR('u'):
          it = str.insert(str.insert(it + 1, CHAR('w')) + 1, CHAR('u'));
          break;
        case CHAR('U'):
          it = str.insert(str.insert(it + 1, CHAR('W')) + 1, CHAR('U'));
          break;

        case CHAR('t'):
          if (it + 1 != str.end() &&
              (*(it + 1) == CHAR('h') || *(it + 1) == CHAR('H')))
            it = str.insert(str.erase(it, it + 2), CHAR('d'));
          break;
        case CHAR('T'):
          if (it + 1 != str.end() &&
              (*(it + 1) == CHAR('h') || *(it + 1) == CHAR('H')))
            it = str.insert(str.erase(it, it + 2), CHAR('D'));
          break;

        case CHAR('h'): [[fallthrough]];
        case CHAR('H'):
          if (it + 1 != str.end())
          {
            if (++it; *it == CHAR('e'))
              it = str.insert(it, CHAR('y')) + 1;
            else if (*it == CHAR('E'))
              it = str.insert(it, CHAR('Y')) + 1;
          }
          break;

        case CHAR('n'): [[fallthrough]];
        case CHAR('N'):
          if (it + 1 != str.end()) switch (*++it)
            {
              case CHAR('a'): [[fallthrough]];
              case CHAR('e'): [[fallthrough]];
              case CHAR('i'): [[fallthrough]];
              case CHAR('o'): [[fallthrough]];
              case CHAR('u'): it = str.insert(it, CHAR('y')) + 1; break;

              case CHAR('A'): [[fallthrough]];
              case CHAR('E'): [[fallthrough]];
              case CHAR('I'): [[fallthrough]];
              case CHAR('O'): [[fallthrough]];
              case CHAR('U'): it = str.insert(it, CHAR('Y')) + 1; break;
            }
          break;
      }
    }
    return str;
  }

  template<typename CHAR>
  lak::string<CHAR> uwuify(lak::span<const CHAR> str)
  {
    return lak::uwuify(str.to_string());
  }
}

#endif