#ifndef CPPURSES_PAINTER_PALETTE_BASIC8_HPP
#define CPPURSES_PAINTER_PALETTE_BASIC8_HPP
#include <array>
#include <cstdint>

#include <cppurses/painter/color.hpp>

namespace cppurses::basic8 {

constexpr auto Black  = Color{0};
constexpr auto Maroon = Color{1};
constexpr auto Green  = Color{2};
constexpr auto Olive  = Color{3};
constexpr auto Navy   = Color{4};
constexpr auto Purple = Color{5};
constexpr auto Teal   = Color{6};
constexpr auto Silver = Color{7};

inline auto const palette = ANSI_palette{{
    /* clang-format off */
    {Black,   ANSI{0}},
    {Maroon,  ANSI{1}},
    {Green,   ANSI{2}},
    {Olive,   ANSI{3}},
    {Navy,    ANSI{4}},
    {Purple,  ANSI{5}},
    {Teal,    ANSI{6}},
    {Silver,  ANSI{7}},
    /* clang-format on */
}};
}  // namespace cppurses::basic8
#endif  // CPPURSES_PAINTER_PALETTE_BASIC8_HPP