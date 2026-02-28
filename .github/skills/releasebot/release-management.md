# Release Management

Methodology for assessing release readiness and managing release processes.

## Release Readiness Checklist

### Code Quality
- [ ] All CI checks pass on the release branch
- [ ] No open critical/high-priority issues blocking release
- [ ] Test coverage has not decreased since last release
- [ ] No known regressions

### Documentation
- [ ] CHANGELOG updated with all changes since last release
- [ ] README reflects current state
- [ ] API documentation is current
- [ ] Migration guide (if breaking changes)

### Version
- [ ] Version bumped in all required locations
- [ ] Version follows semantic versioning
- [ ] Git tag matches version string

### Dependencies
- [ ] No dependencies with known CVEs
- [ ] No deprecated dependencies
- [ ] Lock file is committed and up to date

### Legal
- [ ] LICENSE file is present and current
- [ ] All new files have appropriate license headers
- [ ] No new license-incompatible dependencies

### Artifacts
- [ ] Release artifacts build successfully
- [ ] Artifacts are signed (if applicable)
- [ ] SBOM is generated (if applicable)

## Changelog Format

Follow [Keep a Changelog](https://keepachangelog.com/) format:

```markdown
# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/),
and this project adheres to [Semantic Versioning](https://semver.org/).

## [Unreleased]

### Added
- New feature description (#PR)

### Changed
- Changed behavior description (#PR)

### Deprecated
- Feature to be removed in future

### Removed
- Removed feature description

### Fixed
- Bug fix description (#issue)

### Security
- Security fix description (#issue)

## [1.2.3] - 2024-01-15

### Fixed
- Fix buffer overflow in inflate() (#42)

[Unreleased]: https://github.com/owner/repo/compare/v1.2.3...HEAD
[1.2.3]: https://github.com/owner/repo/compare/v1.2.2...v1.2.3
```

## Version Bumping

### Semantic Versioning Rules

| Change Type | Version Bump | Example |
|------------|-------------|---------|
| Breaking API change | Major (X.0.0) | Removing a public function |
| New feature (backwards compatible) | Minor (0.X.0) | Adding a new function |
| Bug fix (backwards compatible) | Patch (0.0.X) | Fixing incorrect behavior |
| Security fix | Patch (minimum) | Could be minor if new API added |

### Where to Bump Version

Check ecosystem files (`.github/ecosystems/`) for ecosystem-specific version locations:

| Ecosystem | Version Location |
|-----------|-----------------|
| C/C++ | Header file (`#define VERSION`), CMakeLists.txt, configure |
| Python | `pyproject.toml`, `setup.py`, `__version__` in `__init__.py` |
| npm | `package.json` |
| Ruby | `*.gemspec`, `lib/*/version.rb` |
| Rust | `Cargo.toml` |
| Go | Git tags (no version file) |
| Java | `pom.xml`, `build.gradle` |
| .NET | `*.csproj` |

## SBOM (Software Bill of Materials)

Generate SBOM for releases:

```bash
# CycloneDX format (widely supported)
# npm
npx @cyclonedx/cyclonedx-npm --output-file sbom.json

# Python
cyclonedx-py --format json -o sbom.json

# Go
cyclonedx-gomod mod -json -output sbom.json

# GitHub CLI
gh sbom
```

## Release Process Template

```markdown
## Release v[X.Y.Z] Checklist

### Pre-Release
- [ ] Create release branch: `release/vX.Y.Z`
- [ ] Bump version in all locations
- [ ] Update CHANGELOG.md
- [ ] Run full test suite
- [ ] Review open issues for blockers

### Release
- [ ] Merge release branch to main
- [ ] Create Git tag: `vX.Y.Z`
- [ ] Create GitHub Release with changelog
- [ ] Upload artifacts (if applicable)
- [ ] Publish to package registry (if applicable)

### Post-Release
- [ ] Verify package is available on registry
- [ ] Update version in main branch to dev version
- [ ] Announce release (if applicable)
- [ ] Close release milestone (if applicable)
```

## Finding Format

```markdown
### Release Finding: [Brief Title]

- **Type**: Version inconsistency / Missing changelog / Missing artifact / Blockers
- **Severity**: High (blocks release) / Medium (should fix) / Low (nice to have)
- **Details**: [What's wrong or missing]
- **Recommendation**: [Specific action to take]
```
