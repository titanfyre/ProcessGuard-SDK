# SDK publication status

Current source layout and registry coverage. A GitHub download is distinct from publication on a central registry.

| Target | Status | Package / documentation |
| --- | --- | --- |
| CPP/vcpkg | published_custom_registry | processguard-sdk |
| CPP/conan | public_recipe_builds_locally |  [Source](packaging/native/) |
| CPP/nuget | paused_pending_owner_email_confirmation | ProcessGuard-SDK.Native |
| NET/nuget | built_verified_publication_on_hold_by_owner | ProcessGuardSDK.SDK [Source](ProcessGuard-SDK-NET-nuget/) |
| Python/pypi | built_fixture_and_production_verified | processguard-sdk [Source](ProcessGuard-SDK-Python-pypi/) |
| Python/conda-forge | built_tested_requires_source_publication_and_official_review | processguard-sdk [Source](packaging/conda/) |
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
