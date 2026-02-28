# Security Code Review

Methodology for reviewing code changes (PRs) from a security perspective.

## Review Scope

Every PR that touches code should be reviewed for:

1. **Input validation** — Is all external input validated before use?
2. **Memory safety** — Are allocations, bounds, and lifetimes correct?
3. **Error handling** — Do error paths handle security-sensitive state correctly?
4. **Authentication/Authorization** — Are access controls maintained?
5. **Data flow** — Can untrusted data reach sensitive sinks without sanitization?

## Taint Analysis (Manual)

Trace data flow from sources to sinks:

**Sources** (untrusted input):
- Network data (HTTP requests, socket reads)
- File contents (uploaded files, config files from disk)
- Environment variables
- Command-line arguments
- Database query results (if DB is shared)
- IPC/message queue data

**Sinks** (security-sensitive operations):
- Memory operations (memcpy, buffer writes)
- SQL queries
- Shell commands (system, exec, popen)
- File system operations (open, write, delete)
- Crypto operations (key derivation, signing)
- Network output (responses, redirects)
- Deserialization functions

If untrusted data reaches a sink without validation/sanitization, flag it.

## Review Checklist

### For C/C++ code:
- [ ] All buffer operations have bounds checks
- [ ] Integer arithmetic checked for overflow before allocation
- [ ] All `malloc`/`calloc` results checked for NULL
- [ ] No `strcpy`, `strcat`, `sprintf` — use bounded variants
- [ ] `free()` followed by NULL assignment
- [ ] No format strings from untrusted input

### For all languages:
- [ ] No hardcoded credentials or API keys
- [ ] Cryptographic operations use well-known libraries
- [ ] Error messages don't leak internal state
- [ ] File operations validate paths (no traversal)
- [ ] New dependencies are from trusted sources
- [ ] No `eval()` or equivalent with untrusted input

## Severity Classification

| Severity | Criteria | Example |
|----------|----------|---------|
| Critical | RCE, auth bypass, data breach | Buffer overflow in parser, SQL injection |
| High | Privilege escalation, significant data leak | IDOR, path traversal to sensitive files |
| Medium | DoS, limited data leak, missing security control | Unbounded allocation, verbose error messages |
| Low | Defense-in-depth improvement, minor hardening | Missing security header, weak hash for non-security use |

## Reporting Format

For each finding in a PR review:

```
**[SEVERITY]** Brief description

- **Location**: `file.c`, lines X-Y
- **Issue**: Describe the vulnerability
- **Impact**: What could an attacker do?
- **Suggestion**: How to fix it
- **CWE**: CWE-XXX (if applicable)
```
