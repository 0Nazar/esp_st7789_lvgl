# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "C:/Users/Welcome/ESP32/v5.4/esp-idf/components/bootloader/subproject")
  file(MAKE_DIRECTORY "C:/Users/Welcome/ESP32/v5.4/esp-idf/components/bootloader/subproject")
endif()
file(MAKE_DIRECTORY
  "C:/Users/Welcome/esp_proj/esp_st7789_lvgl/bootloader"
  "C:/Users/Welcome/esp_proj/esp_st7789_lvgl/bootloader-prefix"
  "C:/Users/Welcome/esp_proj/esp_st7789_lvgl/bootloader-prefix/tmp"
  "C:/Users/Welcome/esp_proj/esp_st7789_lvgl/bootloader-prefix/src/bootloader-stamp"
  "C:/Users/Welcome/esp_proj/esp_st7789_lvgl/bootloader-prefix/src"
  "C:/Users/Welcome/esp_proj/esp_st7789_lvgl/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/Welcome/esp_proj/esp_st7789_lvgl/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/Welcome/esp_proj/esp_st7789_lvgl/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
