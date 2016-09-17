isEmpty(BOOST_ROOT) {
  BOOST_ROOT = $$PWD/core/include \
               $$PWD/config/include \
               $$PWD/assert/include \
               $$PWD/throw_exception/include \
               $$PWD/static_assert/include \
               $$PWD/type_traits/include \
               $$PWD/utility/include \
               $$PWD/move/include \
               $$PWD/optional/include \
               $$PWD/iostreams/include \
               $$PWD/preprocessor/include \
               $$PWD/mpl/include \
               $$PWD/range/include \
               $$PWD/iterator/include \
               $$PWD/detail/include \
               $$PWD/concept_check/include \
               $$PWD/integer/include
}
