# Dependency Analysis

Methodology for auditing and managing project dependencies.

## Dependency Discovery

### Direct Dependencies

Identify dependency manifest files by ecosystem:

| Ecosystem | Manifest | Lock File |
|-----------|----------|-----------|
| Ruby | `Gemfile`, `*.gemspec` | `Gemfile.lock` |
| Go | `go.mod` | `go.sum` |
| npm | `package.json` | `package-lock.json`, `yarn.lock`, `pnpm-lock.yaml` |
| Python | `requirements.txt`, `pyproject.toml`, `setup.py`, `Pipfile` | `requirements.txt` (pinned), `Pipfile.lock`, `poetry.lock` |
| Rust | `Cargo.toml` | `Cargo.lock` |
| Java | `pom.xml`, `build.gradle` | N/A (use dependency:tree) |
| PHP | `composer.json` | `composer.lock` |
| .NET | `*.csproj`, `*.nuspec` | `packages.lock.json` |
| Perl | `cpanfile`, `Makefile.PL` | `cpanfile.snapshot` |
| Elixir | `mix.exs` | `mix.lock` |

### System Dependencies

Also check for:
- `Dockerfile` / `apt-get install` / `apk add` — OS-level deps
- `CMakeLists.txt` / `find_package()` — C/C++ library deps
- `configure` / `pkg-config` — Autotools deps
- `MODULE.bazel` / `WORKSPACE` — Bazel deps

## Maintenance Thresholds

Flag dependencies that meet these criteria:

| Signal | Threshold | Flag |
|--------|-----------|------|
| Last release | > 2 years ago | 🟡 Stale |
| Maintainers | = 1 | 🟡 Bus-factor risk |
| Open issues response | > 90 days median | 🟡 Unresponsive |
| Status | deprecated | 🔴 Deprecated |
| Status | archived | 🔴 Archived |
| Known CVEs | any unpatched | 🔴 Vulnerable |
| Downloads trend | declining > 50% YoY | 🟡 Declining |

Check using ecosyste.ms API:
```bash
curl -s "https://packages.ecosyste.ms/api/v1/registries/{registry}/packages/{name}" | \
  jq '{name, latest_release_published_at, maintainers_count, status}'
```

## Version Policy

### Semantic Versioning

| Range | Meaning | Risk |
|-------|---------|------|
| `^1.2.3` | >=1.2.3, <2.0.0 | Medium — allows minor updates |
| `~1.2.3` | >=1.2.3, <1.3.0 | Low — allows patches only |
| `1.2.3` | Exactly 1.2.3 | Lowest — pinned |
| `>=1.0` | Any version >=1.0 | High — allows major updates |
| `*` | Any version | Highest — completely unpinned |

### Recommendation

- **Production dependencies**: Use `~` (tilde) or pin exact versions
- **Development dependencies**: `^` (caret) is acceptable
- **Always have a lock file** committed to the repository

## Dependency Tree Analysis

Map the full dependency tree to identify:

1. **Diamond dependencies** — Same package required at different versions
2. **Deep chains** — Transitive dependency depth > 5 levels
3. **Abandoned transitives** — Your deps depend on abandoned packages
4. **License conflicts** — Transitive deps with incompatible licenses

## Structured Output Format

For each flagged dependency:

```json
{
  "name": "package-name",
  "current_version": "1.2.3",
  "latest_version": "2.0.1",
  "ecosystem": "npm",
  "flags": ["stale", "bus-factor"],
  "last_release": "2021-06-15",
  "maintainers": 1,
  "cves": [],
  "recommendation": "Consider alternatives or fork for maintenance",
  "alternatives": ["alternative-package"]
}
```

## Update Strategy

| Scenario | Action |
|----------|--------|
| Patch update available (1.2.3 → 1.2.4) | Create PR automatically |
| Minor update available (1.2.3 → 1.3.0) | Create PR with changelog |
| Major update available (1.2.3 → 2.0.0) | Create issue with migration notes |
| Dependency deprecated | Create issue with alternative suggestions |
| Security CVE in dependency | Create high-priority issue immediately |
