# Ecosystem: Rust

## Identity

- ecosyste.ms registry: `crates.io`
- Package URL scheme: `pkg:cargo/packagename`

## File patterns

- Manifests: `Cargo.toml`
- Lockfile: `Cargo.lock`
- Config: `.cargo/config.toml`, `rust-toolchain.toml`, `clippy.toml`

## CODEOWNERS paths

```
# Dependencies
Cargo.toml @depbot
Cargo.lock @depbot

# Releases
Cargo.toml @releasebot
```

## CI

- Container image: `rust:{version}`
- No lockfile removal needed for libraries (Cargo.lock is often gitignored). For applications, keep the lockfile.
- Version matrix: check endoflife.date/rust or use `stable`, `beta`, `nightly` channels
- Rust stable is usually sufficient. Add nightly only if the project uses nightly features.

## Commands

| Task | Command |
|------|---------|
| Install deps | `cargo fetch` |
| Run tests | `cargo test` |
| Lint | `cargo clippy -- -D warnings` |
| Format check | `cargo fmt -- --check` |
| Build | `cargo build --release` |
| Publish | `cargo publish` |
| Audit | `cargo audit` |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

## Smoke test

```sh
cargo build --release
cargo test
```

## Version bumping

Patch: bug fixes, performance improvements, dependency patches
Minor: new features, new public API, deprecations
Major: breaking changes, removed public API, minimum Rust edition or version bump

## Package metadata

`Cargo.toml` contains package metadata: name, version, description, homepage, repository, license. When forking or mirroring, update the homepage and repository fields to point to the correct repository URL.

## Dangerous patterns

- **`unsafe` blocks with undefined behavior**: dereferencing raw pointers, violating aliasing rules, or creating invalid references in `unsafe` causes UB that the compiler cannot catch. Minimize `unsafe` surface, document invariants, and test with Miri (`cargo +nightly miri test`).
- **`unwrap()` / `expect()` on untrusted input**: panics crash the thread (and often the process). Use `match`, `if let`, or `?` for any value derived from external input.
  ```rust
  // DoS if input is not valid UTF-8
  let name = std::str::from_utf8(bytes).unwrap();
  // Safe
  let name = std::str::from_utf8(bytes)?;
  ```
- **Silent integer overflow in release builds**: debug builds panic on overflow, but release builds wrap silently. Use `checked_add`, `saturating_add`, or `wrapping_add` when overflow matters.
- **`std::mem::transmute`**: reinterprets bits from one type to another without any checks. Incorrect use violates type invariants and causes UB. Prefer safe conversion traits (`From`, `TryFrom`, `as`).
- **`Command` with shell strings**: `Command::new("sh").arg("-c").arg(format!("grep {} file", user_input))` is injectable. Build commands with `Command::new("grep").arg(user_input).arg("file")`.
- **Unsound `Send` / `Sync` implementations**: manually implementing `Send` or `Sync` for types with interior pointers can cause data races. Only implement these when you can prove the type is actually thread-safe.
- **Insecure defaults**: `reqwest::Client` verifies TLS certificates by default, but `danger_accept_invalid_certs(true)` disables this. `rustls` is secure by default; manually configuring `webpki` wrong can skip validation. Integer casts with `as` silently truncate.
