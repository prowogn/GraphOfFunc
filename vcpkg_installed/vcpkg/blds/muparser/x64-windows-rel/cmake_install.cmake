# Install script for directory: C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/pkgs/muparser_x64-windows")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "OFF")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY OPTIONAL FILES "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/x64-windows-rel/muparser.lib")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "RuntimeLibraries" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE SHARED_LIBRARY FILES "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/x64-windows-rel/muparser.dll")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParser.h"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParserBase.h"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParserBytecode.h"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParserCallback.h"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParserDLL.h"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParserDef.h"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParserError.h"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParserFixes.h"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParserInt.h"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParserTemplateMagic.h"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParserTest.h"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParserToken.h"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/src/7945d81364-4645a3f5ac.clean/include/muParserTokenReader.h"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/muparser/muparser-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/muparser/muparser-targets.cmake"
         "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/x64-windows-rel/CMakeFiles/Export/f4bad9bb078ad8062c3d1d8d4dc8f96d/muparser-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/muparser/muparser-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/muparser/muparser-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/muparser" TYPE FILE FILES "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/x64-windows-rel/CMakeFiles/Export/f4bad9bb078ad8062c3d1d8d4dc8f96d/muparser-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/muparser" TYPE FILE FILES "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/x64-windows-rel/CMakeFiles/Export/f4bad9bb078ad8062c3d1d8d4dc8f96d/muparser-targets-release.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Development" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/muparser" TYPE FILE FILES
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/x64-windows-rel/muparserConfig.cmake"
    "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/x64-windows-rel/muparserConfigVersion.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" TYPE FILE FILES "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/x64-windows-rel/muparser.pc")
endif()

if(CMAKE_INSTALL_COMPONENT)
  set(CMAKE_INSTALL_MANIFEST "install_manifest_${CMAKE_INSTALL_COMPONENT}.txt")
else()
  set(CMAKE_INSTALL_MANIFEST "install_manifest.txt")
endif()

string(REPLACE ";" "\n" CMAKE_INSTALL_MANIFEST_CONTENT
       "${CMAKE_INSTALL_MANIFEST_FILES}")
file(WRITE "C:/Users/vniki/OneDrive/Рабочий стол/GraphOfFunc/vcpkg_installed/vcpkg/blds/muparser/x64-windows-rel/${CMAKE_INSTALL_MANIFEST}"
     "${CMAKE_INSTALL_MANIFEST_CONTENT}")
