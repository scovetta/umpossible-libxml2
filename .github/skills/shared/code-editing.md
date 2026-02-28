# Code Editing

Core code editing capabilities and constraints for all agents. When an agent edits code, these rules apply universally.

## Principles

1. **Minimal diffs** — Change only what is necessary. Don't reformat, restyle, or refactor adjacent code.
2. **One concern per change** — Each commit addresses exactly one issue.
3. **Preserve intent** — Understand what the existing code does before changing it. Read surrounding context.
4. **Test your assumption** — If you're not sure what code does, trace it rather than guess.

## Before Editing

1. **Read the file** — Understand the full context, not just the target lines.
2. **Check for related files** — A change in a header may require changes in implementations.
3. **Understand the build** — Know how the project builds so your edit won't break compilation.
4. **Check tests** — Know which tests cover the code you're changing.

## Edit Checklist

Before committing any code change, verify:

- [ ] The change compiles/parses without errors
- [ ] The change is consistent with the project's existing style
- [ ] New functions/methods have documentation comments
- [ ] Error handling follows the project's conventions
- [ ] No debug/temporary code left in (print statements, TODO hacks)
- [ ] No unintended whitespace or formatting changes
- [ ] The change doesn't break the public API contract (or is flagged as breaking)

## Language-Specific Considerations

### C/C++
- Check header guards / `#pragma once`
- Verify `#include` additions are necessary and ordered correctly
- Watch for undefined behavior (signed overflow, null deref, buffer overrun)
- Match `malloc`/`free`, `new`/`delete` pairs

### Python
- Follow PEP 8 style
- Use type hints for new functions
- Handle exceptions specifically (no bare `except:`)
- Check import ordering (stdlib, third-party, local)

### JavaScript/TypeScript
- Use `const` by default, `let` when mutation is needed
- Handle async/await properly — no unhandled promises
- Check for `null`/`undefined` edge cases
- Follow the project's module system (ESM vs CJS)

### Rust
- Handle `Result` and `Option` properly — avoid `unwrap()` in library code
- Run `cargo clippy` guidance mentally
- Follow ownership rules — prefer borrowing over cloning

### Go
- Handle all errors — no `_` for error returns
- Follow Go naming conventions (exported vs unexported)
- Run `go vet` guidance mentally

### Ruby
- Follow the project's style (check for `.rubocop.yml`)
- Use `frozen_string_literal: true` in new files
- Handle exceptions with specific classes

## What NOT to Edit

- **Generated files** — Don't edit files that are auto-generated (check for "DO NOT EDIT" headers)
- **Vendor/node_modules** — Never edit third-party vendored code
- **Lock files** — Don't manually edit lock files (package-lock.json, Gemfile.lock, etc.)
- **Minified files** — Don't edit minified/bundled output files
- **Binary files** — Don't attempt to edit binary files
