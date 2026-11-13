# ProcessGuard-SDK Ruby SDK

`gem install processguard-sdk`

Windows x64 Ruby3.1+.

```ruby
require "processguard_sdk"
ProcessGuardSDKSDK::Client.open { |client| p client.call("status") }
```

[API and operation schema](docs/API.md). Matching driver installed separately.
