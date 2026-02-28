# CI Maintenance

Methodology for assessing and maintaining CI pipeline health.

## CI Assessment

### Phase 1: Pipeline Discovery

1. Check for CI configuration files:
   - `.github/workflows/*.yml` — GitHub Actions
   - `Makefile` / `CMakeLists.txt` — Build system
   - `Dockerfile` / `docker-compose.yml` — Container builds
   - `Jenkinsfile`, `.travis.yml`, `.circleci/config.yml` — Other CI systems
   - `BUILD.bazel`, `MODULE.bazel` — Bazel

2. Map the build matrix:
   - Which platforms are tested? (Linux, macOS, Windows)
   - Which compiler/runtime versions?
   - Which build configurations? (debug, release, sanitizers)

### Phase 2: Build Health Check

| Check | What to Verify |
|-------|---------------|
| Build success | Does the project build cleanly? |
| Warning count | Are there compiler/linter warnings? |
| Build time | Is the build reasonably fast? |
| Reproducibility | Same input → same output? |
| Dependencies | Are build deps properly declared? |
| Caching | Is CI caching effective? |

### Phase 3: Workflow Efficiency

GitHub Actions specific checks:

- **Runner selection**: Using appropriate runners (ubuntu-latest vs specific versions)
- **Caching**: `actions/cache` for dependencies, build artifacts
- **Matrix strategy**: `fail-fast` setting, `max-parallel` if needed
- **Timeouts**: `timeout-minutes` set to prevent hung jobs
- **Concurrency**: `concurrency` groups to cancel redundant runs
- **Artifacts**: Upload test results, coverage reports

## Build System Best Practices

### Makefile
```makefile
# Essential targets
all: build
build:
test: build
clean:
install: build
.PHONY: all build test clean install
```

### CMake
```cmake
# Version range for compatibility
cmake_minimum_required(VERSION 3.14...3.28)

# Standard options
option(BUILD_TESTING "Build tests" ON)
option(BUILD_SHARED_LIBS "Build shared libraries" ON)

# Install with proper export
include(GNUInstallDirs)
install(TARGETS mylib EXPORT mylib-targets)
```

### GitHub Actions Workflow Structure
```yaml
name: CI
on:
  push:
    branches: [main]
  pull_request:
    branches: [main]

concurrency:
  group: ${{ github.workflow }}-${{ github.ref }}
  cancel-in-progress: true

jobs:
  build:
    strategy:
      matrix:
        os: [ubuntu-latest, macos-latest, windows-latest]
    runs-on: ${{ matrix.os }}
    timeout-minutes: 30
    steps:
      - uses: actions/checkout@v4
      - name: Build
        run: make build
      - name: Test
        run: make test
```

## Findings Format

```markdown
## CI Assessment - [DATE]

### Pipeline Status
- **Workflows found**: N
- **Build systems**: [Make, CMake, etc.]
- **Platforms**: [Linux, macOS, Windows]
- **Language versions**: [versions tested]

### Issues Found

| Priority | Issue | Impact | Recommendation |
|----------|-------|--------|----------------|
| High | No CI caching | Slow builds | Add actions/cache for deps |
| Medium | Missing Windows tests | Platform gaps | Add windows-latest to matrix |
| Low | No timeout set | Hung job risk | Add timeout-minutes: 30 |

### Recommendations
1. [Prioritized list of improvements]
```

## Cross-Platform Considerations

| Issue | Linux | macOS | Windows |
|-------|-------|-------|---------|
| Line endings | LF | LF | CRLF (check .gitattributes) |
| Path separator | `/` | `/` | `\` (use `/` in CMake/Make) |
| Shared lib ext | `.so` | `.dylib` | `.dll` |
| Static lib ext | `.a` | `.a` | `.lib` |
| Shell | bash | bash/zsh | PowerShell/cmd |
| Package manager | apt | brew | choco/winget |
