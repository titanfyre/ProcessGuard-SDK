# SDK publication status

Current source layout and registry coverage. A GitHub download is distinct from publication on a central registry.

| Target | Status | Package / documentation |
| --- | --- | --- |
| CPP/vcpkg | published_custom_registry | processguard-sdk |
| CPP/conan | published_custom_source_index | [processguard-sdk](https://github.com/TitanCSA/sdk-packages/tree/089bc73095aa8eda89bacbe10d0b9e58394b0207/conan-index) [Source](packaging/native/) |
| CPP/nuget | account_email_confirmed_requires_matching_trusted_publishing_policy | ProcessGuard-SDK.Native |
| NET/nuget | built_verified_requires_trusted_publishing_policy | ProcessGuardSDK.SDK [Source](ProcessGuard-SDK-NET-nuget/) |
| Python/pypi | built_fixture_and_production_verified | processguard-sdk [Source](ProcessGuard-SDK-Python-pypi/) |
| Python/conda-forge | custom_channel_install_verified_requires_conda_forge_review | [processguard-sdk](https://raw.githubusercontent.com/TitanCSA/sdk-packages/089bc73095aa8eda89bacbe10d0b9e58394b0207/conda-channel) [Source](packaging/conda/) |
| Rust/cargo | published_install_verified | [processguard-sdk](https://crates.io/crates/processguard-sdk) [Source](ProcessGuard-SDK-Rust-cargo/) |
| JavaScript/npm | awaiting_browser_publication_authentication | [processguard-sdk](https://www.npmjs.com/package/processguard-sdk) [Source](ProcessGuard-SDK-JavaScript-npm/) |
| Java/maven | published_install_verified | [processguard-sdk](https://central.sonatype.com/artifact/io.github.titancsa/processguard-sdk/1.0.0) [Source](ProcessGuard-SDK-Java-maven/) |
| Java/jitpack | published_install_verified | [processguard-sdk](https://jitpack.io/#titanfyre/ProcessGuard-SDK/java-sdk-v1.0.0) [Source](ProcessGuard-SDK-Java-maven/) |
| Go/modules | published_install_verified | [processguard-sdk](https://pkg.go.dev/github.com/titanfyre/ProcessGuard-SDK/ProcessGuard-SDK-Go-modules@v1.0.0) [Source](ProcessGuard-SDK-Go-modules/) |
| Go/pkg-go-dev | indexed_verified | [Documentation](https://pkg.go.dev/github.com/titanfyre/ProcessGuard-SDK/ProcessGuard-SDK-Go-modules@v1.0.0) |
| PowerShell/gallery | published_install_verified | [ProcessGuardSDK](https://www.powershellgallery.com/packages/ProcessGuardSDK/1.0.0) [Source](ProcessGuard-SDK-PowerShell-gallery/) |
| Dart/pub-dev | built_fixture_and_production_verified | processguard_sdk [Source](ProcessGuard-SDK-Dart-pub-dev/) |
| Ruby/rubygems | published_install_verified | [processguard-sdk](https://rubygems.org/gems/processguard-sdk) [Source](ProcessGuard-SDK-Ruby-rubygems/) |
| PHP/packagist | composer_source_and_tag_published | [titancsa/processguard-sdk](https://packagist.org/packages/titancsa/processguard-sdk) |

Native SDK source and CMake/Conan/vcpkg/NuGet recipes share [`packaging/native`](packaging/native/LAYOUT.md). Previous release archives and tags remain unchanged.

Application and driver prerequisites and original licensing remain those documented by each SDK. No driver installer is bundled with these source packages.
