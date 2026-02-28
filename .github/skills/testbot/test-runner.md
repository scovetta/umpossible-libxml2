# Test Runner

Methodology for executing and reporting on test suites across different ecosystems.

## Test Discovery

1. **Check ecosystem files** in `.github/ecosystems/` for the test command
2. **Scan for test infrastructure**:
   - Test directories: `test/`, `tests/`, `spec/`, `__tests__/`, `t/`
   - Test files: `*_test.*`, `test_*.*`, `*_spec.*`, `*.test.*`
   - Test configs: `jest.config.*`, `pytest.ini`, `.rspec`, `phpunit.xml`
   - CI test steps in workflow files

3. **Identify the test framework**:

| Ecosystem | Common Frameworks | Command |
|-----------|------------------|---------|
| C/C++ | CTest, Google Test, Check, custom | `make test`, `ctest`, `./test_runner` |
| Python | pytest, unittest, nose2 | `pytest`, `python -m pytest` |
| JavaScript | Jest, Mocha, Vitest | `npm test`, `npx jest` |
| Ruby | RSpec, Minitest | `bundle exec rspec`, `rake test` |
| Go | built-in | `go test ./...` |
| Rust | built-in | `cargo test` |
| Java | JUnit, TestNG | `mvn test`, `gradle test` |
| PHP | PHPUnit | `./vendor/bin/phpunit` |
| Elixir | ExUnit | `mix test` |

## Test Execution Report Format

After running tests, report in this structured format:

```markdown
## Test Execution Report

### Environment
- **Runner**: ubuntu-latest
- **Language version**: [detected version]
- **Test framework**: [framework name]
- **Command**: [exact command used]

### Results
- **Total tests**: N
- **Passed**: N ✅
- **Failed**: N ❌
- **Skipped**: N ⏭️
- **Duration**: Xs

### Failed Tests
| Test | File | Error |
|------|------|-------|
| test_name | path/to/test.py:42 | AssertionError: expected X got Y |

### Coverage (if available)
- **Lines**: X% (Y/Z)
- **Branches**: X% (Y/Z)
- **Uncovered files**: [list critical uncovered files]
```

## Test Health Assessment

Evaluate the test suite health:

| Metric | Good | Acceptable | Poor |
|--------|------|------------|------|
| Pass rate | 100% | >95% | <95% |
| Coverage (lines) | >80% | >60% | <60% |
| Coverage (branches) | >70% | >50% | <50% |
| Test execution time | <5min | <15min | >15min |
| Flaky test rate | 0% | <2% | >2% |
| Test-to-code ratio | >0.5 | >0.3 | <0.3 |

## CI Integration

When assessing CI test health:
1. Run tests in the same way CI does (check workflow files)
2. Note any test fixtures or data files required
3. Check for environment-specific test skips
4. Verify test isolation (tests shouldn't depend on execution order)
