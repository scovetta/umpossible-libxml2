# License Compliance

Methodology for auditing license compliance, REUSE conformance, and SPDX correctness.

## License Audit Process

### Phase 1: Project License

1. **Check for LICENSE file** in root directory
2. **Identify the license** — Match against known SPDX identifiers
3. **Verify consistency** — LICENSE file content matches SPDX ID in metadata
4. **Check for multiple licenses** (dual licensing, different licenses for different components)

### Phase 2: Source File Headers

Check if source files have (or should have) license headers:

```bash
# Find files without license headers
find . -name '*.c' -o -name '*.h' -o -name '*.py' -o -name '*.js' -o -name '*.rb' -o -name '*.go' -o -name '*.rs' | \
  xargs grep -L 'license\|License\|LICENSE\|SPDX-License-Identifier\|Copyright'
```

SPDX header format:
```
// SPDX-License-Identifier: MIT
// Copyright (c) 2024 Author Name
```

### Phase 3: REUSE Compliance

[REUSE](https://reuse.software) specification requires:
1. Every file has a license header or a `.license` companion file
2. All referenced licenses are in `LICENSES/` directory
3. Consistent SPDX identifiers throughout

Check with:
```bash
# If reuse tool is available
reuse lint

# Manual check
find . -type f ! -path './.git/*' ! -path './LICENSES/*' | while read f; do
  if ! grep -q 'SPDX-License-Identifier' "$f" && ! [ -f "$f.license" ]; then
    echo "MISSING: $f"
  fi
done
```

### Phase 4: Dependency Licenses

Check that dependency licenses are compatible with the project license:

```bash
# npm
npx license-checker --json

# pip
pip-licenses --format=json

# Go
go-licenses csv ./...

# Cargo
cargo license --json

# Bundler
bundle exec license_finder
```

## License Compatibility Matrix

| Project License | Compatible Deps | Incompatible Deps |
|----------------|----------------|-------------------|
| MIT | MIT, BSD, ISC, Apache-2.0, Unlicense | GPL (if not willing to relicense) |
| Apache-2.0 | MIT, BSD, ISC, Apache-2.0, Unlicense | GPL-2.0 (only), AGPL |
| GPL-2.0 | MIT, BSD, ISC, LGPL-2.1, GPL-2.0 | Apache-2.0 (GPL-2.0-only), GPL-3.0 |
| GPL-3.0 | MIT, BSD, ISC, Apache-2.0, LGPL, GPL-2.0+, GPL-3.0 | GPL-2.0-only |
| LGPL-2.1 | MIT, BSD, ISC, LGPL-2.1 | GPL (for non-LGPL components) |
| BSD-2-Clause | MIT, BSD, ISC, Unlicense | Copyleft (if linking) |
| MPL-2.0 | MIT, BSD, ISC, Apache-2.0 | File-level copyleft applies |
| AGPL-3.0 | All of GPL-3.0 compatible | Everything if SaaS use |

### Rules

1. **Copyleft in dependencies** — If a dependency is GPL, the whole project may need to be GPL
2. **LGPL exception** — LGPL deps can be used in non-GPL projects if dynamically linked
3. **Apache-2.0 + GPL-2.0 conflict** — Apache-2.0 has patent clauses incompatible with GPL-2.0-only
4. **No license = All rights reserved** — Dependencies without a license cannot be used

## SPDX Identifiers

Common SPDX license identifiers:

| SPDX ID | Full Name |
|---------|-----------|
| `MIT` | MIT License |
| `Apache-2.0` | Apache License 2.0 |
| `GPL-2.0-only` | GNU GPL v2 only |
| `GPL-2.0-or-later` | GNU GPL v2 or later |
| `GPL-3.0-only` | GNU GPL v3 only |
| `GPL-3.0-or-later` | GNU GPL v3 or later |
| `LGPL-2.1-only` | GNU LGPL v2.1 only |
| `BSD-2-Clause` | BSD 2-Clause |
| `BSD-3-Clause` | BSD 3-Clause |
| `ISC` | ISC License |
| `MPL-2.0` | Mozilla Public License 2.0 |
| `AGPL-3.0-only` | GNU AGPL v3 |
| `Unlicense` | The Unlicense |
| `Zlib` | zlib License |

## Finding Format

```markdown
### License Finding: [Brief Title]

- **Type**: Missing header / Incompatible dependency / License mismatch / REUSE violation
- **Severity**: High (incompatible) / Medium (missing) / Low (inconsistent)
- **Files affected**: [list or count]
- **Details**: [What's wrong]
- **Recommendation**: [Specific fix]
```

## Automatable Fixes

These can be safely automated via PR:
- Adding SPDX headers to source files
- Creating `LICENSES/` directory with license texts
- Adding `.license` companion files for non-source files
- Updating SPDX identifier in package manifest to match LICENSE

These require human decision:
- Choosing between dual-license options
- Replacing an incompatibly-licensed dependency
- Changing the project's license
