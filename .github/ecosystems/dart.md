# Ecosystem: Dart

## Identity

- ecosyste.ms registry: `pub.dev`
- Package URL scheme: `pkg:pub/packagename`
- Package manager: pub (built into the Dart SDK and Flutter)

## File patterns

- Manifests: `pubspec.yaml`
- Lockfile: `pubspec.lock`
- Config: `analysis_options.yaml`, `dart_test.yaml`

## CODEOWNERS paths

```
# Dependencies
pubspec.yaml @depbot
pubspec.lock @depbot

# Releases
pubspec.yaml @releasebot
```

## CI

- Container image: `dart:{version}` (for pure Dart) or `ghcr.io/cirruslabs/flutter:{version}` (for Flutter packages)
- Remove `pubspec.lock` before install in matrix builds to test fresh resolution
- Version matrix: Dart follows a rapid release cadence. Check dart.dev/get-dart for current stable/beta channels. Flutter packages should also test against Flutter stable.

## Commands

| Task | Command |
|------|---------|
| Install deps | `dart pub get` |
| Run tests | `dart test` |
| Lint | `dart analyze` |
| Format check | `dart format --set-exit-if-changed .` |
| Build (AOT) | `dart compile exe bin/main.dart` (for executables) |
| Dry run publish | `dart pub publish --dry-run` |
| Publish | `dart pub publish` |
| Audit | No built-in audit tool. Check pub.dev advisories and GitHub advisories. |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

### Flutter packages

| Task | Command |
|------|---------|
| Install deps | `flutter pub get` |
| Run tests | `flutter test` |
| Lint | `flutter analyze` |
| Build | `flutter build` (platform-specific) |

## Smoke test

```sh
dart pub get
dart analyze
dart test
```

## Version bumping

Version is set in `pubspec.yaml` in the `version` field. Dart uses semver.

Patch: bug fixes, performance improvements, dependency patches
Minor: new features, new public API, deprecations
Major: breaking changes, removed public API, minimum Dart SDK version bump

## Package metadata

`pubspec.yaml` contains package metadata: name, version, description, homepage, repository, issue_tracker, environment (SDK constraints), dependencies. When forking or mirroring, update the homepage, repository, and issue_tracker fields to point to the correct repository URL.

## Notes

- pub.dev requires a Google account for publishing.
- Dart has a scoring system on pub.dev (pub points) that checks documentation, platform support, code health, and maintenance. The `dart pub publish --dry-run` command catches many issues before upload.
- Flutter packages often have platform-specific native code (iOS, Android, web). CI for these needs platform SDKs, which makes container-based CI harder. Pure Dart packages are straightforward.
- The `dart fix` command can auto-apply recommended code migrations when the SDK version is bumped.

## Dangerous patterns

- **`Process.run` with shell strings**: `Process.run('sh', ['-c', 'grep $userInput file'])` passes input through a shell. Use `Process.run('grep', [userInput, 'file'])` with `runInShell: false` (the default).
  ```dart
  // Vulnerable
  Process.run('sh', ['-c', 'convert ${userFile} output.png']);
  // Safe
  Process.run('convert', [userFile, 'output.png']);
  ```
- **`HttpClient` certificate bypass**: setting `badCertificateCallback: (cert, host, port) => true` disables TLS certificate validation. Never ship this in production. Use it only in tests or local development behind a flag.
- **SQL interpolation in sqflite**: `db.rawQuery("SELECT * FROM items WHERE name = '$input'")` is injectable. Use parameterized queries: `db.rawQuery("SELECT * FROM items WHERE name = ?", [input])`.
- **JSON decode DoS on large input**: `jsonDecode(untrustedString)` on a very large or deeply nested payload can exhaust memory. Validate input size and nesting depth before parsing, or stream-parse with `JsonDecoder` and limit consumption.
- **`dart:mirrors` in AOT builds**: reflection via `dart:mirrors` is unavailable in AOT-compiled code (Flutter release builds). Code that depends on it will fail silently or crash at runtime.
- **Insecure defaults**: `HttpClient` validates certificates by default (don't weaken with `badCertificateCallback`), `webview_flutter` has JavaScript disabled by default (enabling it without input sanitization opens XSS), `SharedPreferences` stores data unencrypted on disk (don't store tokens or secrets there, use `flutter_secure_storage`).
