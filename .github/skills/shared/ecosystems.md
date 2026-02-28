# Ecosystem Lookup

Reference for using the ecosyste.ms API and the local `.github/ecosystems/` directory to get ecosystem-specific guidance.

## Local Ecosystem Files

Check `.github/ecosystems/` for per-ecosystem guidance files:

```
.github/ecosystems/
├── ruby.md
├── go.md
├── npm.md
├── python.md
├── rust.md
├── java.md
├── php.md
├── nuget.md
├── perl.md
├── swift.md
├── elixir.md
├── dart.md
└── haskell.md
```

Each file contains:
- **Identity**: File patterns and CODEOWNERS paths for detection
- **CI section**: Build/test commands, container images, version matrix guidance
- **Commands table**: Install, test, lint, format, build, publish commands
- **Smoke test**: Minimal verification after changes
- **Version bumping**: Where version numbers live and how to update them
- **Package metadata**: Required fields, registry URLs
- **Dangerous patterns**: Ecosystem-specific security anti-patterns

## Ecosystem Detection

Detect the repository's ecosystem by checking for marker files:

| Ecosystem | Marker Files |
|-----------|-------------|
| Ruby | `Gemfile`, `*.gemspec`, `.ruby-version` |
| Go | `go.mod`, `go.sum` |
| npm | `package.json`, `package-lock.json`, `yarn.lock` |
| Python | `setup.py`, `pyproject.toml`, `requirements.txt`, `Pipfile` |
| Rust | `Cargo.toml`, `Cargo.lock` |
| Java | `pom.xml`, `build.gradle`, `build.gradle.kts` |
| PHP | `composer.json`, `composer.lock` |
| NuGet/.NET | `*.csproj`, `*.sln`, `packages.config`, `*.nuspec` |
| Perl | `Makefile.PL`, `dist.ini`, `cpanfile` |
| Swift | `Package.swift`, `*.xcodeproj` |
| Elixir | `mix.exs`, `mix.lock` |
| Dart | `pubspec.yaml`, `pubspec.lock` |
| Haskell | `*.cabal`, `stack.yaml`, `cabal.project` |
| C/C++ | `CMakeLists.txt`, `Makefile`, `configure`, `*.c`, `*.h` |

A repository may use multiple ecosystems. Check all marker files.

## ecosyste.ms API

The [ecosyste.ms](https://ecosyste.ms) API provides package metadata, maintenance status, and dependency information.

### Package Lookup

```bash
# Get package info
curl -s "https://packages.ecosyste.ms/api/v1/registries/{registry}/packages/{name}" | jq .

# Registries: rubygems.org, npmjs.org, pypi.org, crates.io, repo1.maven.org, packagist.org, nuget.org, metacpan.org, pub.dev, hex.pm, hackage.haskell.org
```

### Useful Fields

```json
{
  "name": "package-name",
  "latest_release_number": "1.2.3",
  "latest_release_published_at": "2024-01-15T...",
  "licenses": "MIT",
  "repository_url": "https://github.com/owner/repo",
  "status": "active",
  "downloads": 1234567,
  "dependent_packages_count": 456,
  "maintainers_count": 3
}
```

### Maintenance Thresholds

Use these thresholds to assess dependency health:

| Signal | Threshold | Action |
|--------|-----------|--------|
| Last release | >2 years ago | Flag as stale |
| Maintainers | = 1 | Flag bus-factor risk |
| Open issues response time | >90 days median | Flag as unresponsive |
| Status | deprecated/archived | Flag for replacement |
| Downloads trend | Declining >50% YoY | Monitor for abandonment |

### Dependency Tree

```bash
# Get dependencies for a package
curl -s "https://packages.ecosyste.ms/api/v1/registries/{registry}/packages/{name}/versions/{version}" | jq '.dependencies'
```

## When to Use

- **Depbot**: Uses ecosyste.ms API for dependency health assessment
- **Buildbot**: Uses ecosystem files for CI command reference
- **Testbot**: Uses ecosystem files for test command reference
- **Securibot**: Uses ecosystem files for ecosystem-specific vulnerability patterns
- **Licensebot**: Uses ecosyste.ms API for dependency license lookup
- **Docbot**: Uses ecosystem files for metadata field requirements
- **Perfbot**: Uses ecosystem files for benchmark command reference
