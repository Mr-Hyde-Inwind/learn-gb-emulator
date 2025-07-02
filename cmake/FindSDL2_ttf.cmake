# cmake/FindSDL2_ttf.cmake
find_path(SDL2_TTF_INCLUDE_DIR SDL_ttf.h PATH_SUFFIXES SDL2)
find_library(SDL2_TTF_LIBRARY SDL2_ttf)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SDL2_ttf
  REQUIRED_VARS SDL2_TTF_INCLUDE_DIR SDL2_TTF_LIBRARY
)

if(SDL2_TTF_FOUND)
  set(SDL2_ttf_INCLUDE_DIRS ${SDL2_TTF_INCLUDE_DIR})
  set(SDL2_ttf_LIBRARIES ${SDL2_TTF_LIBRARY})
endif()