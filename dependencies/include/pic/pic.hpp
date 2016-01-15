#ifndef PIC_PIC_HPP_INCLUDED
#define PIC_PIC_HPP_INCLUDED

#include <exception>
#include <stdexcept>
#include <string>

namespace pic {

typedef int format_type;
enum format { _32_bit_rle_rgbe, _32_bit_rle_xyze };

typedef int resolution_string_type;
enum resolution_string { neg_y_pos_x, neg_y_neg_x, pos_y_neg_x, pos_y_pos_x, pos_x_pos_y, neg_x_pos_y, neg_x_neg_y, pos_x_neg_y };

typedef unsigned char uint8_t;

class runtime_error : public std::runtime_error {
public:
  explicit runtime_error(const std::string& what);
};

template <typename FloatType> void rgbe_or_xyze_to_rgb_or_xyz(uint8_t rgbe_r_or_xyze_x, uint8_t rgbe_g_or_xyze_y, uint8_t rgbe_b_or_xyze_z, uint8_t rgbe_e_or_xyze_e, FloatType& rgb_r_or_xyz_x, FloatType& rgb_g_or_xyz_y, FloatType& rgb_b_or_xyz_z);
template <typename FloatType> void rgbe_to_rgb(uint8_t rgbe_r, uint8_t rgbe_g, uint8_t rgbe_b, uint8_t rgbe_e, FloatType& rgb_r, FloatType& rgb_g, FloatType& rgb_b);
template <typename FloatType> void xyze_to_xyz(uint8_t xyze_x, uint8_t xyze_y, uint8_t xyze_z, uint8_t xyze_e, FloatType& xyz_x, FloatType& xyz_y, FloatType& xyz_z);
template <typename FloatType> void rgb_or_xyz_to_rgbe_or_xyze(FloatType rgb_r_or_xyz_x, FloatType rgb_g_or_xyz_y, FloatType rgb_b_or_xyz_z, uint8_t& rgbe_r_or_xyze_x, uint8_t& rgbe_g_or_xyze_y, uint8_t& rgbe_b_or_xyze_z, uint8_t& rgbe_e_or_xyze_e);
template <typename FloatType> void rgb_to_rgbe(FloatType rgb_r, FloatType rgb_g, FloatType rgb_b, uint8_t& rgbe_r, uint8_t& rgbe_g, uint8_t& rgbe_b, uint8_t& rgbe_e);
template <typename FloatType> void xyz_to_xyze(FloatType xyz_x, FloatType xyz_y, FloatType xyz_z, uint8_t& xyze_x, uint8_t& xyze_y, uint8_t& xyze_z, uint8_t& xyze_e);

} // namespace pic

inline pic::runtime_error::runtime_error(const std::string& what)
  : std::runtime_error(what)
{
}

#include <cmath>

template <typename FloatType>
inline void pic::rgbe_or_xyze_to_rgb_or_xyz(uint8_t rgbe_r_or_xyze_x, uint8_t rgbe_g_or_xyze_y, uint8_t rgbe_b_or_xyze_z, uint8_t rgbe_e_or_xyze_e, FloatType& rgb_r_or_xyz_x, FloatType& rgb_g_or_xyz_y, FloatType& rgb_b_or_xyz_z)
{
  if (rgbe_e_or_xyze_e != 0) {
    FloatType f = std::ldexp(FloatType(1.0), int(rgbe_e_or_xyze_e) - (128 + 8));
    rgb_r_or_xyz_x = (rgbe_r_or_xyze_x + FloatType(0.5)) * f;
    rgb_g_or_xyz_y = (rgbe_g_or_xyze_y + FloatType(0.5)) * f;
    rgb_b_or_xyz_z = (rgbe_b_or_xyze_z + FloatType(0.5)) * f;
  }
  else {
    rgb_r_or_xyz_x = rgb_g_or_xyz_y = rgb_b_or_xyz_z = FloatType(0.0);
  }
}

template <typename FloatType>
inline void pic::rgb_or_xyz_to_rgbe_or_xyze(FloatType rgb_r_or_xyz_x, FloatType rgb_g_or_xyz_y, FloatType rgb_b_or_xyz_z, uint8_t& rgbe_r_or_xyze_x, uint8_t& rgbe_g_or_xyze_y, uint8_t& rgbe_b_or_xyze_z, uint8_t& rgbe_e_or_xyze_e)
{
  FloatType d = std::max(rgb_r_or_xyz_x, std::max(rgb_g_or_xyz_y, rgb_b_or_xyz_z));
  if (d <= FloatType(1e-32)) {
    rgbe_r_or_xyze_x = rgbe_g_or_xyze_y = rgbe_b_or_xyze_z = 0;
    rgbe_e_or_xyze_e = 0;
  }
  else {
    int e;
    d = frexp(d, &e) * FloatType(255.9999) / d;
    rgbe_r_or_xyze_x = rgb_r_or_xyz_x > FloatType(0.0) ? rgb_r_or_xyz_x * d : 0;
    rgbe_g_or_xyze_y = rgb_g_or_xyz_y > FloatType(0.0) ? rgb_g_or_xyz_y * d : 0;
    rgbe_b_or_xyze_z = rgb_b_or_xyz_z > FloatType(0.0) ? rgb_b_or_xyz_z * d : 0;
    rgbe_e_or_xyze_e = e + 128;
  }
}

template <typename FloatType>
inline void pic::rgbe_to_rgb(uint8_t rgbe_r, uint8_t rgbe_g, uint8_t rgbe_b, uint8_t rgbe_e, FloatType& rgb_r, FloatType& rgb_g, FloatType& rgb_b)
{
  return rgbe_or_xyze_to_rgb_or_xyz(rgbe_r, rgbe_g, rgbe_b, rgbe_e, rgb_r, rgb_g, rgb_b);
}

template <typename FloatType>
inline void pic::rgb_to_rgbe(FloatType rgb_r, FloatType rgb_g, FloatType rgb_b, uint8_t& rgbe_r, uint8_t& rgbe_g, uint8_t& rgbe_b, uint8_t& rgbe_e)
{
  return rgb_or_xyz_to_rgbe_or_xyze(rgb_r, rgb_g, rgb_b, rgbe_r, rgbe_g, rgbe_b, rgbe_e);
}

template <typename FloatType>
inline void pic::xyze_to_xyz(uint8_t xyze_x, uint8_t xyze_y, uint8_t xyze_z, uint8_t xyze_e, FloatType& xyz_x, FloatType& xyz_y, FloatType& xyz_z)
{
  return rgbe_or_xyze_to_rgb_or_xyz(xyze_x, xyze_y, xyze_z, xyze_e, xyz_x, xyz_y, xyz_z);
}

template <typename FloatType>
inline void pic::xyz_to_xyze(FloatType xyz_x, FloatType xyz_y, FloatType xyz_z, uint8_t& xyze_x, uint8_t& xyze_y, uint8_t& xyze_z, uint8_t& xyze_e)
{
  return rgb_or_xyz_to_rgbe_or_xyze(xyz_x, xyz_y, xyz_z, xyze_x, xyze_y, xyze_z, xyze_e);
}

#endif // PIC_PIC_HPP_INCLUDED
