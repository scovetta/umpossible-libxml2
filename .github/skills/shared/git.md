# Git Workflow

Standard git practices for all agents that create branches and commits.

## Branch Naming

```
{agent-name}/{short-description}
```

Examples:
- `securibot/fix-buffer-overflow-inflate`
- `testbot/add-edge-case-tests`
- `buildbot/fix-cmake-version`
- `depbot/update-openssl-3.2`
- `docbot/update-api-docs`
- `perfbot/optimize-hot-loop`
- `licensebot/add-spdx-headers`
- `releasebot/update-changelog`

## Commit Messages

Use conventional commit format:

```
type(scope): short description

Longer description if needed.

Refs: #issue-number
```

Types: `fix`, `feat`, `docs`, `test`, `build`, `ci`, `chore`, `perf`, `refactor`

Examples:
```
fix(security): validate input length before buffer copy

The inflate() function did not check input bounds, allowing
a heap buffer overflow when processing crafted streams.

Refs: #42
```

```
test(coverage): add tests for error handling paths

Adds tests for edge cases in decompression error recovery
that were identified by testbot analysis.

Refs: #38
```

## Rebase Policy

- **Always rebase** onto the default branch before creating a PR
- **Never force-push** to shared branches
- When conflicts arise, flag for human resolution rather than guessing

```bash
git fetch origin
git rebase origin/main
# If conflicts: abort and report
git rebase --abort
```

## CI Skip

When a commit should skip CI (documentation-only, metadata-only):

```bash
git commit -m "docs: update README [skip ci]"
```

Use `[skip ci]` only for:
- Pure documentation changes (.md files, comments)
- Metadata-only changes (.gitignore, .editorconfig)
- License header additions (no code logic changes)

**Never** skip CI for:
- Code changes (even "trivial" ones)
- Build system changes
- Dependency updates
- Configuration changes

## Working with the Repository

```bash
# Always start from a clean state
git checkout main
git pull origin main

# Create a feature branch
git checkout -b {agent-name}/{description}

# Stage and commit
git add -A
git commit -m "type(scope): description"

# Push
git push origin {agent-name}/{description}
```

## Safety Rules

- **Never commit to `main` directly** — always use branches + PRs
- **Never commit secrets**, API keys, or tokens
- **Never commit generated files** unless they're tracked by the project
- **Always check `.gitignore`** before adding files
- **Verify the diff** before committing — ensure only intended changes are included
- **One logical change per commit** — don't mix unrelated changes
