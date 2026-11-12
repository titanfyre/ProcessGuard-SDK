cmake_minimum_required(VERSION 3.24)
project(ProcessGuardSDK VERSION 0.1.2 LANGUAGES C CXX)
if(NOT WIN32 OR NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
  message(FATAL_ERROR "This SDK requires Windows x64. No driver is installed by this build.")
endif()
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)
add_library(TitanProcessGuardSDK STATIC
  "sdk/SDK/ProcessGuard.cpp"
)
add_library(ProcessGuardSDK::sdk ALIAS TitanProcessGuardSDK)
set_target_properties(TitanProcessGuardSDK PROPERTIES EXPORT_NAME sdk)
target_compile_features(TitanProcessGuardSDK PUBLIC cxx_std_17)
target_compile_definitions(TitanProcessGuardSDK PUBLIC )
target_compile_definitions(TitanProcessGuardSDK PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN _WIN32_WINNT=0x0A00)
if(MSVC)
  target_compile_options(TitanProcessGuardSDK PRIVATE /utf-8 /EHsc /W4)
endif()
target_include_directories(TitanProcessGuardSDK PUBLIC
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk>"
  "$<INSTALL_INTERFACE:include/>"
  "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/sdk/SDK>"
  "$<INSTALL_INTERFACE:include/SDK>"
)
target_link_libraries(TitanProcessGuardSDK PUBLIC advapi32 comctl32 comdlg32 crypt32 gdi32 newdev ole32 setupapi shell32 user32 wintrust)
install(TARGETS TitanProcessGuardSDK EXPORT ProcessGuardSDKTargets ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
install(DIRECTORY sdk/ DESTINATION include FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.inl")
install(FILES LICENSE DESTINATION share/processguard-sdk)
install(EXPORT ProcessGuardSDKTargets NAMESPACE ProcessGuardSDK:: DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ProcessGuardSDK)
configure_file("ProcessGuardSDKConfig.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/ProcessGuardSDKConfig.cmake" @ONLY)
write_basic_package_version_file("${CMAKE_CURRENT_BINARY_DIR}/ProcessGuardSDKConfigVersion.cmake" VERSION 0.1.2 COMPATIBILITY SameMajorVersion)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/ProcessGuardSDKConfig.cmake" "${CMAKE_CURRENT_BINARY_DIR}/ProcessGuardSDKConfigVersion.cmake" DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/ProcessGuardSDK)
