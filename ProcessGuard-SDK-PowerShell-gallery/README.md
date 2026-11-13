# ProcessGuard-SDK PowerShell SDK

PowerShell7.4+ Windows x64. `Install-Module ProcessGuardSDK -Scope CurrentUser`.

```powershell
$client=Connect-ProcessGuardSDKSDK
try { Invoke-ProcessGuardSDKSDK -Client $client -Operation status } finally { Disconnect-ProcessGuardSDKSDK -Client $client }
```

Use -Arguments with a hashtable matching [schema](docs/API.md). Native errors retain their Windows code through the inner exception. Matching driver installed separately.
