#ifndef PFM_PFM_HPP_INCLUDED
#define PFM_PFM_HPP_INCLUDED

#include <exception>
#include <stdexcept>
#include <string>

namespace pfm {

typedef int format_type;
enum format_enum { color_format, grayscale_format };

typedef float float_type;

typedef float_type grayscale_pixel;

class runtime_error : public std::runtime_error {
public:
  explicit runtime_error(const std::string& what);
};

} // namespace pfm

inline pfm::runtime_error::runtime_error(const std::string& what)
  : std::runtime_error(what)
{
}

#endif // PFM_PFM_HPP_INCLUDED
