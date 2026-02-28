# Ecosystem: Haskell

## Identity

- ecosyste.ms registry: `hackage.haskell.org`
- Package URL scheme: `pkg:hackage/packagename`
- Package managers: Cabal (cabal-install) and Stack. Cabal is the standard; Stack adds curated snapshot resolvers on top of it.

## File patterns

- Manifests: `*.cabal`, `package.yaml` (hpack, generates .cabal), `cabal.project`
- Lockfile: `cabal.project.freeze` (Cabal), `stack.yaml.lock` (Stack)
- Config: `cabal.project`, `stack.yaml`, `.hlint.yaml`, `fourmolu.yaml`, `.ormolu`

## CODEOWNERS paths

```
# Dependencies
*.cabal @depbot
package.yaml @depbot
cabal.project @depbot
stack.yaml @depbot
cabal.project.freeze @depbot
stack.yaml.lock @depbot

# Releases
*.cabal @releasebot
package.yaml @releasebot
```

## CI

- Container image: `haskell:{version}` (includes GHC and Cabal)
- For Stack-based projects, install Stack in the container or use `fpco/stack-build:{version}`
- Version matrix: test against the GHC versions listed in the package's `tested-with` field, or check haskell.org/ghc for recent releases. Common to test GHC 9.4, 9.6, 9.8, 9.10.
- Haskell builds are slow. Use caching aggressively (`~/.cabal/store/`, `dist-newstyle/`).

## Commands

### Cabal projects

| Task | Command |
|------|---------|
| Install deps | `cabal update && cabal build --only-dependencies` |
| Run tests | `cabal test` |
| Lint | `hlint src/` |
| Format | `fourmolu -i src/**/*.hs` or `ormolu -i src/**/*.hs` |
| Build | `cabal build` |
| Build sdist | `cabal sdist` (creates source tarball for upload) |
| Publish | `cabal upload dist-newstyle/sdist/*.tar.gz` (to Hackage) |
| Audit | No built-in audit tool. Check Hackage advisories and haskell/security-advisories on GitHub. |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

### Stack projects

| Task | Command |
|------|---------|
| Install deps | `stack build --only-dependencies` |
| Run tests | `stack test` |
| Build | `stack build` |
| Build sdist | `stack sdist` |

## Smoke test

```sh
# Cabal
cabal update
cabal build
cabal test

# Stack
stack build
stack test
```

## Version bumping

Haskell uses the Package Versioning Policy (PVP), which is similar to semver but with four components: `A.B.C.D`. `A.B` is the major version (breaking changes), `C` is minor (new features), `D` is patch.

- `A.B` bump: breaking API changes, removed exports, changed type signatures
- `C` bump: new modules, new exports, deprecations
- `D` bump: bug fixes, documentation, internal changes

Some packages use semver instead of PVP. Follow whatever the package already uses.

## Package metadata

The `.cabal` file contains package metadata: name, version, synopsis, description, homepage, bug-reports, license, author, maintainer, category, tested-with. If the project uses `package.yaml` (hpack), the same fields are in YAML format and generate the `.cabal` file. When forking or mirroring, update the homepage and bug-reports fields to point to the correct repository URL.

## Notes

- Hackage requires a user account for uploading. New accounts need a package trustee to approve the first upload.
- The `tested-with` field in the cabal file is the standard way to declare which GHC versions are supported. CI should match this.
- Many Haskell projects use Nix for reproducible builds alongside or instead of Cabal/Stack. If you see `flake.nix`, `default.nix`, or `shell.nix`, the project likely has Nix-based build support.
- Upper bounds on dependencies are strongly encouraged in the Haskell ecosystem (unlike most others). The Hackage build matrix will test against new dependency versions and flag breakage.

## Dangerous patterns

- **`unsafePerformIO` / `unsafeCoerce`**: `unsafePerformIO` breaks referential transparency and can cause data races, reordering, or duplicated effects. `unsafeCoerce` bypasses the type system entirely. Both are valid in specific low-level library code but should never appear in application code.
- **`read` on untrusted input**: `read "not a number" :: Int` throws an exception. For external data, use `readMaybe` from `Text.Read` which returns `Maybe`.
  ```haskell
  -- Crashes on bad input
  let n = read userInput :: Int
  -- Safe
  case readMaybe userInput of
    Just n  -> process n
    Nothing -> handleError
  ```
- **`callCommand` / `callProcess` injection**: `callCommand ("grep " ++ userInput ++ " file")` passes input through a shell. Use `proc` or `callProcess` with separate arguments: `callProcess "grep" [userInput, "file"]`.
- **`Data.Binary.decode` memory DoS**: `decode` on untrusted input can allocate arbitrary amounts of memory (e.g. a crafted length prefix claiming billions of elements). Validate input size before decoding, or use incremental parsing with bounds checks.
- **Lazy I/O space leaks**: `readFile` returns a lazy `String`, but holding a reference to part of it can keep the entire file handle open and memory pinned. Use strict I/O (`Data.Text.IO.readFile`, `Data.ByteString.readFile`) for predictable resource usage.
- **Insecure defaults**: `http-client` does not use TLS by default (use `http-client-tls` with `tlsManagerSettings`), `tls` and `connection` packages default to verifying certificates but custom `ClientParams` can disable validation, `aeson` decodes numbers without bounds checking which can cause memory issues on extreme values.
