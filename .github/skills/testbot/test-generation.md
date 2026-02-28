# Test Generation

Methodology for generating new tests to improve coverage and catch regressions.

## Principles

1. **Evidence-based** — Only generate tests for code you've read and understood
2. **Behavior-focused** — Test what the code does, not how it does it
3. **Edge-case driven** — Focus on boundaries, error paths, and corner cases
4. **Minimal** — Each test should verify exactly one behavior
5. **Independent** — Tests must not depend on each other or execution order

## Test Generation Process

### Step 1: Identify Gaps

Analyze existing tests to find what's NOT covered:

1. **Examine public API** — List all public functions/methods/endpoints
2. **Check existing tests** — Map which API functions are already tested
3. **Find the gap** — Functions with no tests are the highest priority
4. **Check error paths** — Even tested functions may lack error-path tests
5. **Check boundaries** — Integer limits, empty inputs, NULL/nil/undefined

### Step 2: Categorize Test Types

| Priority | Category | Example |
|----------|----------|---------|
| 1 (highest) | Crash/security regression | Test that OOM/overflow is handled |
| 2 | Core functionality | Test that main API works correctly |
| 3 | Error handling | Test that errors return correct codes |
| 4 | Edge cases | Test boundary values, empty input |
| 5 | Integration | Test component interaction |
| 6 (lowest) | Performance regression | Test that latency stays within bounds |

### Step 3: Write Tests

Follow the project's existing test style:

```
# Match existing conventions:
- Same test framework
- Same file naming pattern
- Same assertion style
- Same test organization (describe/it, test classes, etc.)
- Same fixture/setup patterns
```

### Step 4: Verify

Before submitting test PRs:
- [ ] New tests pass
- [ ] Existing tests still pass
- [ ] Tests are deterministic (no flakiness)
- [ ] Tests don't depend on network/time/random (or mock them)
- [ ] Test names are descriptive

## Test Patterns

### Happy Path
```
Given valid input → verify correct output
```

### Error Path
```
Given invalid input → verify error handling (correct error code, no crash, no leak)
```

### Boundary Values
```
Given min/max/zero/empty/null → verify graceful handling
```

### State Transitions
```
Given state A → perform action → verify state B
```

### Regression Test
```
Given the specific input that triggered bug #N → verify the bug is fixed
```

## What NOT to Generate

- Tests that duplicate existing coverage
- Tests that test language/framework behavior (not your code)
- Tests that are tightly coupled to implementation details
- Tests that require specific environment (network, file system) without mocking
- Tests that take more than a few seconds to run (unless explicitly benchmarks)
- Tests for generated/vendored code

## PR Format for Test Additions

```markdown
## Test Improvement: [Area]

### Gap Identified
[What was missing from the test suite]

### Tests Added
- `test_function_name_with_empty_input` — Verifies handling of empty input
- `test_function_name_overflow` — Verifies integer overflow is caught

### Coverage Impact
- Before: X% line coverage in `file.c`
- After: Y% line coverage in `file.c`

### Evidence
[How the gap was identified — e.g., coverage report, API audit, crash report]
```
