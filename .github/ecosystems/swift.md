# Ecosystem: Swift

## Identity

- ecosyste.ms registry: `swiftpackageindex.com`
- Package URL scheme: `pkg:swift/github.com/owner/repo`
- Package manager: Swift Package Manager (SwiftPM), built into Xcode and the Swift toolchain

## File patterns

- Manifests: `Package.swift`
- Lockfile: `Package.resolved`
- Config: `.swift-version`, `.swiftlint.yml`, `.swiftformat`

## CODEOWNERS paths

```
# Dependencies
Package.swift @depbot
Package.resolved @depbot

# Releases
Package.swift @releasebot
```

## CI

- Container image: `swift:{version}` (Linux builds) or use macOS runners for Apple platform targets
- No lockfile removal needed in matrix builds — `Package.resolved` pins exact versions but `swift build` respects the version range in `Package.swift`
- Version matrix: check endoflife.date/swift for supported versions. Swift 5.9+ is the current baseline for most packages.
- Linux and macOS can behave differently (Foundation, Dispatch) — test on both when the package supports both platforms

## Commands

| Task | Command |
|------|---------|
| Resolve deps | `swift package resolve` |
| Run tests | `swift test` |
| Lint | `swiftlint` |
| Format | `swift-format --recursive .` or `swiftformat .` |
| Build | `swift build -c release` |
| Publish | Tag and push (SwiftPM resolves packages directly from git repos, no registry upload needed) |
| Audit | No built-in audit tool. Check Swift Package Index and GitHub advisories. |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

## Smoke test

```sh
swift build
swift test
```

## Version bumping

Swift packages use git tags for versioning (e.g. `1.2.3`, no `v` prefix by convention though both work). `Package.swift` declares supported Swift tools versions and platform requirements.

Patch: bug fixes, performance improvements
Minor: new features, new public API, deprecations
Major: breaking changes, removed public API, minimum Swift version bump, platform support changes

## Package metadata

`Package.swift` contains the package name, products (libraries and executables), dependencies, targets, and platform requirements. There's no separate metadata file for homepage or license — those live in the git repo and are indexed by Swift Package Index. When forking or mirroring, update any hardcoded URLs in README and documentation to point to the correct repository URL.

## Notes

- SwiftPM resolves packages directly from git repositories. There is no central upload registry like rubygems or npm. Swift Package Index indexes public repos but doesn't host packages.
- CocoaPods (`.podspec`, `Podfile`) is an older dependency manager still used in some iOS projects. If the package has a podspec, it targets CocoaPods users. The ecosyste.ms registry for CocoaPods is `cocoapods.org`.
- Packages that support both Linux and Apple platforms sometimes have `#if canImport(Darwin)` or `#if os(Linux)` conditional compilation blocks. CI should test both.

## Dangerous patterns

- **Force unwrap (`!`) on untrusted input**: `dict["key"]!` or `Int(userString)!` crashes the process with a fatal error if the value is nil. Use `guard let`, `if let`, or nil-coalescing (`??`) for any value from external sources.
  ```swift
  // DoS if key is missing
  let value = json["token"]!
  // Safe
  guard let value = json["token"] else { return }
  ```
- **`NSKeyedUnarchiver` without secure coding**: `unarchiveObject(with:)` deserializes arbitrary classes and is deprecated. Use `unarchivedObject(ofClass:from:)` with `requiresSecureCoding = true` to restrict which classes can be instantiated.
- **`Process` / `NSTask` shell injection**: `Process()` with `/bin/sh -c` and interpolated user input is injectable. Set `executableURL` and pass arguments as an array via `arguments`.
  ```swift
  // Vulnerable
  let process = Process()
  process.executableURL = URL(fileURLWithPath: "/bin/sh")
  process.arguments = ["-c", "grep \(userInput) file.txt"]
  // Safe
  process.executableURL = URL(fileURLWithPath: "/usr/bin/grep")
  process.arguments = [userInput, "file.txt"]
  ```
- **App Transport Security (ATS) disabled**: setting `NSAllowsArbitraryLoads = true` in `Info.plist` disables TLS requirements for all network connections. Prefer per-domain exceptions with `NSExceptionDomains` if plain HTTP is needed for specific hosts.
- **`URLSession` certificate bypass**: implementing `urlSession(_:didReceive:completionHandler:)` to always call `completionHandler(.useCredential, ...)` disables certificate validation. Only bypass for local development, never in production builds.
- **Insecure defaults**: `URLSession` validates certificates by default (don't override the delegate to weaken this), `UserDefaults` is not encrypted (don't store secrets there, use Keychain), `String(contentsOf: url)` performs synchronous network I/O on the calling thread which can freeze the UI and has no timeout control.
