vcpkg_download_distfile(ARCHIVE
    URLS "https://github.com/titanfyre/ProcessGuard-SDK/releases/download/sdk-v0.1.2/processguard-sdk-sdk-0.1.2.tar.gz"
    FILENAME "processguard-sdk-sdk-0.1.2.tar.gz"
    SHA512 f4e00ab084c79b212a56e9dd798affae645a7bebc007225849a881ae667d31d07d3c42a3ec81dc0b41d346576c82b1d0dbe59c0768414c2f7ff56e2d4bef02c5
)
vcpkg_extract_source_archive(SOURCE_PATH ARCHIVE "${ARCHIVE}" NO_REMOVE_ONE_LEVEL)
vcpkg_cmake_configure(SOURCE_PATH "${SOURCE_PATH}")
vcpkg_cmake_install()
vcpkg_cmake_config_fixup(PACKAGE_NAME ProcessGuardSDK CONFIG_PATH lib/cmake/ProcessGuardSDK)
file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug/include" "${CURRENT_PACKAGES_DIR}/debug/share")
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")
