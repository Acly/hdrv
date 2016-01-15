#ifndef PFM_COLOR_PIXEL_HPP_INCLUDED
#define PFM_COLOR_PIXEL_HPP_INCLUDED

#include <pfm/pfm.hpp>

namespace pfm {

class color_pixel
{
public:
  typedef float_type value_type;
  typedef std::size_t size_type;
  color_pixel();
  color_pixel(value_type, value_type, value_type);
  explicit color_pixel(value_type);
  const value_type& operator[](size_type) const;
  value_type& operator[](size_type);
private:
  value_type rgb_[3];
};

bool operator==(const color_pixel&, const color_pixel&);
bool operator!=(const color_pixel&, const color_pixel&);
template <typename Ch, typename Tr> std::basic_ostream<Ch,Tr>& operator<<(std::basic_ostream<Ch,Tr>&, const color_pixel&);
template <typename Ch, typename Tr> std::basic_istream<Ch,Tr>& operator>>(std::basic_istream<Ch,Tr>&, color_pixel&);

} // namespace pfm

#include <cassert>

inline pfm::color_pixel::color_pixel()
{
}

inline pfm::color_pixel::color_pixel(value_type r, value_type g, value_type b)
{
  rgb_[0] = r;
  rgb_[1] = g;
  rgb_[2] = b;
}

inline pfm::color_pixel::color_pixel(value_type rgb)
{
  rgb_[0] = rgb;
  rgb_[1] = rgb;
  rgb_[2] = rgb;
}

inline const pfm::color_pixel::value_type& pfm::color_pixel::operator[](size_type index) const
{
  assert(index < 3);
  return rgb_[index];
}

inline pfm::color_pixel::value_type& pfm::color_pixel::operator[](size_type index)
{
  assert(index < 3);
  return rgb_[index];
}

inline bool pfm::operator==(const color_pixel& lhs, const color_pixel& rhs)
{
  return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && (lhs[2] == rhs[2]);
}

inline bool pfm::operator!=(const color_pixel& lhs, const color_pixel& rhs)
{
  return !(lhs == rhs);
}

template <typename Ch, typename Tr>
inline std::basic_ostream<Ch,Tr>& pfm::operator<<(std::basic_ostream<Ch,Tr>& ostream, const color_pixel& p)
{
  return ostream << p[0] << ' ' << p[1] << ' ' << p[2];
}

template <typename Ch, typename Tr>
inline std::basic_istream<Ch,Tr>& pfm::operator>>(std::basic_istream<Ch,Tr>& istream, color_pixel& p)
{
  return istream >> p[0] >> p[1] >> p[2];
}

#endif // PFM_COLOR_PIXEL_HPP_INCLUDED
