isEmpty(BOOST_ROOT) {
  BOOST_ROOT = $$PWD/core/include \
               $$PWD/config/include \
               $$PWD/assert/include \
               $$PWD/throw_exception/include \
               $$PWD/static_assert/include \
               $$PWD/type_traits/include \
               $$PWD/utility/include \
               $$PWD/move/include \
               $$PWD/optional/include
}
