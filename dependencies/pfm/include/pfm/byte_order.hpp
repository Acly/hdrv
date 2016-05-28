#ifndef PFM_BYTE_ORDER_HPP_INCLUDED
#define PFM_BYTE_ORDER_HPP_INCLUDED

#include <pfm/pfm.hpp>

namespace pfm {

#if defined(PFM_BIG_ENDIAN) || defined(PFM_LITTLE_ENDIAN)
#  error
#endif

#if (defined(__powerpc) || defined(__powerpc__) || defined(__POWERPC__) || defined(__ppc__) || defined(_M_PPC) || defined(__ARCH_PPC))
#  define PFM_BIG_ENDIAN
#elif (defined(i386) || defined(__i386__) || defined(__i386) || defined(_M_IX86) || defined(_X86_) || defined(__THW_INTEL__) || defined(__I86__) || defined(__INTEL__)) \
   || (defined(__amd64__) || defined(__amd64) || defined(__x86_64__) || defined(__x86_64) || defined(_M_X64))
#  define PFM_LITTLE_ENDIAN
#else
#  error
#endif

typedef int byte_order_type;
enum byte_order_enum
{
  little_endian_byte_order,
  big_endian_byte_order,
#if defined(PFM_BIG_ENDIAN)
  host_byte_order = big_endian_byte_order
#elif defined(PFM_LITTLE_ENDIAN)
  host_byte_order = little_endian_byte_order
#else
#  error
#endif
};

#undef PFM_BIG_ENDIAN
#undef PFM_LITTLE_ENDIAN

void swap_byte_order(float_type& value);

} // namespace pfm

inline void pfm::swap_byte_order(float_type& value)
{
  typedef unsigned char uint8_t;
  uint8_t* bytes = reinterpret_cast<uint8_t*>(&value);
  std::swap(bytes[0], bytes[3]);
  std::swap(bytes[1], bytes[2]);
}

#endif // PFM_BYTE_ORDER_HPP_INCLUDED
