#ifndef PIC_PIXEL_HPP_INCLUDED
#define PIC_PIXEL_HPP_INCLUDED

#include <pic/pic.hpp>

namespace pic {

class pixel
{
public:
  typedef uint8_t value_type;
  typedef std::size_t size_type;
  pixel();
  pixel(value_type, value_type, value_type, value_type);
  explicit pixel(value_type);
  const value_type& operator[](size_type) const;
  value_type& operator[](size_type);
private:
  value_type rgbe_or_xyze_[4];
};

bool operator==(const pixel&, const pixel&);
bool operator!=(const pixel&, const pixel&);
template <typename Ch, typename Tr> std::basic_ostream<Ch,Tr>& operator<<(std::basic_ostream<Ch,Tr>&, const pixel&);
template <typename Ch, typename Tr> std::basic_istream<Ch,Tr>& operator>>(std::basic_istream<Ch,Tr>&, pixel&);

} // namespace pic

#include <cassert>

inline pic::pixel::pixel()
{
}

inline pic::pixel::pixel(value_type r_or_x, value_type g_or_y, value_type b_or_z, value_type e)
{
  rgbe_or_xyze_[0] = r_or_x;
  rgbe_or_xyze_[1] = g_or_y;
  rgbe_or_xyze_[2] = b_or_z;
  rgbe_or_xyze_[3] = e;
}

inline pic::pixel::pixel(value_type rgbe_or_xyze)
{
  rgbe_or_xyze_[0] = rgbe_or_xyze;
  rgbe_or_xyze_[1] = rgbe_or_xyze;
  rgbe_or_xyze_[2] = rgbe_or_xyze;
  rgbe_or_xyze_[3] = rgbe_or_xyze;
}

inline const pic::pixel::value_type& pic::pixel::operator[](size_type index) const
{
  assert(index < 4);
  return rgbe_or_xyze_[index];
}

inline pic::pixel::value_type& pic::pixel::operator[](size_type index)
{
  assert(index < 4);
  return rgbe_or_xyze_[index];
}

inline bool pic::operator==(const pixel& lhs, const pixel& rhs)
{
  return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]) && (lhs[2] == rhs[2]) && (lhs[3] == rhs[3]);
}

inline bool pic::operator!=(const pixel& lhs, const pixel& rhs)
{
  return !(lhs == rhs);
}

template <typename Ch, typename Tr>
inline std::basic_ostream<Ch,Tr>& pic::operator<<(std::basic_ostream<Ch,Tr>& ostream, const pixel& p)
{
  return ostream << p[0] << ' ' << p[1] << ' ' << p[2] <<  ' ' << p[3];
}

template <typename Ch, typename Tr>
inline std::basic_istream<Ch,Tr>& pic::operator>>(std::basic_istream<Ch,Tr>& istream, pixel& p)
{
  return istream >> p[0] >> p[1] >> p[2] >> p[3];
}

#endif // PIC_PIXEL_HPP_INCLUDED
