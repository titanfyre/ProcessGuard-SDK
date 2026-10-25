# Validation and deployment boundaries

The release is a signed evaluation build. Offline validation exercises the pointer-free protocol, strict input lengths, reserved fields, UTF-16 boundaries and one million reproducible malformed-policy mutations per SDK. Release and AddressSanitizer results are recorded privately; public status reflects the tests actually completed.

The private driver builds with WDK, warnings-as-errors, PREfast and Control Flow Guard. This does not by itself validate a loaded driver. No guarantee of zero vulnerabilities, zero overhead or compatibility with every Windows build is made.

Windows 10 x64 VM: normal and targeted Driver Verifier passed 280 checks and 320 concurrent launches in each campaign; owner cleanup, invalid requests and unload verified. Baseline restored; Windows 11/HVCI qualification remains pending.

Qualification before production delivery must also cover Windows 11, Secure Boot, HVCI, coexistence with endpoint security, repeated PnP removal, power transitions, install rollback and customer-specific policy scope. Run in an isolated lab first. Never disable production security settings to run an unsigned preview.

The kernel device grants access only to SYSTEM and administrators. One session owns the policy. Wire records contain no pointers; inputs have exact sizes and bounded capacities. Decisions have no network dependency. A bounded passive-level lock protects policy updates and the event ring; there is no user-mode round trip. ProcessGuard resolves the canonical image with SeLocateProcessImageName outside that lock; this documented call allocates a temporary name buffer that is freed on every path. Display/export may lag or lose records under load; the counters expose drops.

Deleting or stopping the client removes its rules; in-flight callbacks may complete a decision already made. This owner-lifetime behavior is intentional and is not persistent endpoint enforcement. Audited operations are observations of a pre-operation callback, not proofs that all downstream work succeeded.

Both supplied setup executables embed the verified Microsoft-signed package, verify its catalog and INF/SYS membership, elevate for installation, report Windows errors and restart requirements, and uninstall only this product's exact package. The actual console installer was exercised in the signed-package VM campaigns below.

## Microsoft documentation

- [Process creation callbacks](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/nc-ntddk-pcreate_process_notify_routine_ex)
- [PS_CREATE_NOTIFY_INFO](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/ntddk/ns-ntddk-_ps_create_notify_info)

## Returned Microsoft-signed package validation

The returned 0.1.0.0 package was verified against the submitted image: unchanged INF and Authenticode-protected SYS content, trusted Microsoft catalog, and INF/SYS catalog membership. Both self-contained installers embed the exact verified files.

The actual console installer, live functionality and uninstall/unload passed on Windows 10 build 19045 x64 twice: normally and under standard Driver Verifier targeted at this product and the other two evaluation drivers. Guest test-signing was disabled; no lab certificate was used. 280 checks and 320 concurrent launches per campaign, including policy attribution, malformed requests, access control, overflow and owner cleanup. No new minidump was observed. Both campaigns restored the original VM and preserved its historical checkpoints. No driver was installed on the development host.

These are bounded VM tests, not a Windows 11, Secure Boot/HVCI, physical hardware, long-duration or customer-deployment qualification. Installer UI resources are byte-verified; the automated install/uninstall runs used the console setup.
