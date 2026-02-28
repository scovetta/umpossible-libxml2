# Documentation Generation

Methodology for generating and maintaining project documentation.

## Documentation Audit

### Phase 1: Inventory

Check for documentation in these locations:

| Document | Expected Location | Purpose |
|----------|------------------|---------|
| README | `README.md` (root) | Project overview, quickstart |
| CHANGELOG | `CHANGELOG.md` or `ChangeLog` | Version history |
| CONTRIBUTING | `CONTRIBUTING.md` | How to contribute |
| LICENSE | `LICENSE` or `COPYING` | Legal terms |
| API docs | `doc/`, `docs/`, inline | API reference |
| Examples | `examples/`, `doc/examples/` | Usage examples |
| Architecture | `doc/architecture.md` | System design |
| Security policy | `SECURITY.md` | Vulnerability reporting |
| Code of Conduct | `CODE_OF_CONDUCT.md` | Community standards |

### Phase 2: Quality Checks

For each documentation file found, verify:

1. **Accuracy** — Does it match the current code?
   - [ ] API signatures in docs match actual code
   - [ ] Installation instructions work with current versions
   - [ ] Examples compile/run successfully
   - [ ] Configuration options are current

2. **Completeness** — Does it cover what it should?
   - [ ] All public API functions/classes are documented
   - [ ] Error handling is documented
   - [ ] Common use cases have examples
   - [ ] Edge cases and limitations are noted

3. **Freshness** — When was it last updated?
   - [ ] No references to deprecated features
   - [ ] Version numbers are current
   - [ ] URLs are not broken (404)
   - [ ] Screenshots/diagrams reflect current UI/architecture

## Documentation Verification Checklist

Six automated checks to run on every scan:

### 1. README Health
- Has title and description
- Has installation/setup instructions
- Has usage examples
- Has license reference
- Has contributing reference
- Badge links work (CI status, coverage, version)

### 2. API Documentation Coverage
- Count public API symbols (functions, classes, types)
- Count documented symbols
- Report coverage percentage
- List undocumented public symbols

### 3. Example Validity
- Do example files compile/parse?
- Do they reference current API?
- Are imports/includes correct?

### 4. Link Validation
- Internal links (cross-references between docs)
- External links (check for 404s if network available)
- Anchor links within documents

### 5. Changelog Completeness
- Does the changelog cover the latest release?
- Does it follow a consistent format (keepachangelog recommended)?
- Are all version numbers present?

### 6. Metadata Consistency
- Version numbers match across files (README, package manifest, changelog)
- Project name is consistent
- URLs are consistent

## CI Skip Guidance

Documentation-only PRs can skip CI:
- Pure `.md` file changes
- Comment-only changes in source files
- `.github/` configuration changes that don't affect builds

Add `[skip ci]` to commit message for these changes.

## Finding Format

```markdown
## Documentation Audit - [DATE]

### Summary
- Documents found: N
- Documents missing: [list]
- API doc coverage: X%
- Broken links: N

### Issues

| Priority | File | Issue | Recommendation |
|----------|------|-------|----------------|
| High | README.md | Missing installation instructions | Add quickstart section |
| Medium | API docs | 5 undocumented public functions | Generate doc stubs |
| Low | CHANGELOG.md | Last entry is 3 versions old | Update changelog |
```
