# Metadata Maintenance

Keeping package/project metadata accurate and consistent.

## Metadata Locations

| Field | Where It Lives |
|-------|---------------|
| Version | Package manifest, changelog, README badge, source code constants |
| Name | Package manifest, README title, LICENSE |
| Description | Package manifest, README first paragraph, repository description |
| License | LICENSE file, package manifest SPDX field, source headers |
| Homepage/URL | Package manifest, README, repository settings |
| Repository URL | Package manifest, README |
| Author/Maintainer | Package manifest, CONTRIBUTORS, git log |
| Keywords/Topics | Package manifest, repository topics |

## Consistency Rules

### Version Numbers
All version references must match:
```bash
# Check for version string patterns
grep -rn 'version.*=\|VERSION.*=\|"version"' --include='*.json' --include='*.toml' --include='*.yaml' --include='*.yml' --include='*.h' --include='*.py' --include='*.rb' --include='*.gemspec' .
```

Flag any version mismatches between:
- Package manifest (package.json, Cargo.toml, *.gemspec, etc.)
- Changelog header
- Source code version constants
- README badges

### URLs
All URLs must:
- Point to the correct repository (not a fork)
- Use HTTPS (not HTTP)
- Not be broken (404)
- Be consistent (same base URL everywhere)

### License
- LICENSE file must exist
- SPDX identifier in package manifest must match LICENSE file content
- Source file headers (if present) must reference the correct license

## Ecosystem-Specific Metadata

### npm (package.json)
Required: `name`, `version`, `description`, `license`, `repository`
Recommended: `keywords`, `author`, `engines`, `main`/`exports`

### PyPI (pyproject.toml / setup.py)
Required: `name`, `version`, `description`, `license`
Recommended: `classifiers`, `python_requires`, `keywords`, `urls`

### RubyGems (*.gemspec)
Required: `name`, `version`, `summary`, `license`
Recommended: `description`, `homepage`, `metadata`

### Cargo (Cargo.toml)
Required: `name`, `version`, `edition`
For publishing: `license`, `description`, `repository`

### Go (go.mod)
Required: `module` path, `go` version
Convention: Tagged releases match `go.mod` module path

## Finding Format

```markdown
### Metadata Issue: [Brief Description]

- **Type**: Version mismatch / Missing field / Broken URL / License inconsistency
- **Files affected**: [list of files]
- **Current state**: [what's wrong]
- **Expected state**: [what it should be]
- **Fix**: [specific change needed]
```

## Automated Checks

Run these checks on every scan:

1. **Version consistency** — All version references match
2. **License presence** — LICENSE file exists and matches manifest
3. **URL health** — All metadata URLs are accessible
4. **Required fields** — All required metadata fields are present
5. **Repository URL** — Points to the correct repository
