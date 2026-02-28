# Insecure Defaults Detection

Methodology for finding fail-open patterns and insecure default configurations.

## What Are Insecure Defaults?

Code that works correctly in the "happy path" but fails into an insecure state when errors occur or when configuration is missing.

## Common Patterns

### Fail-Open Authentication

```python
# BAD: Fails open if auth service is unreachable
def check_auth(token):
    try:
        return auth_service.verify(token)
    except ConnectionError:
        return True  # INSECURE: allows access when auth is down

# GOOD: Fails closed
def check_auth(token):
    try:
        return auth_service.verify(token)
    except ConnectionError:
        return False  # Deny access when auth is unavailable
```

### Missing Security Controls by Default

```yaml
# BAD: TLS disabled by default
settings:
  tls_enabled: false  # User must opt-in to security

# GOOD: TLS enabled by default
settings:
  tls_enabled: true  # User must explicitly opt-out (and should be warned)
```

### Permissive Error Handling

```c
// BAD: Continues on verification failure
int verify_signature(const unsigned char *sig, const unsigned char *data) {
    int result = crypto_verify(sig, data);
    if (result < 0) {
        log_warning("Verification failed, continuing anyway");
        return 0;  // INSECURE: treats failure as success
    }
    return result;
}
```

## Detection Checklist

Search for these patterns:

| Pattern | Search Terms | Risk |
|---------|-------------|------|
| Catch-all that returns success | `except:.*return True`, `catch.*return 0` | Auth bypass |
| Disabled-by-default security | `enabled.*false`, `secure.*false`, `verify.*false` | Missing protection |
| Commented-out security checks | `// TODO.*security`, `# FIXME.*auth` | Incomplete implementation |
| Debug/development mode leaking | `DEBUG.*True`, `development`, `insecure` | Info leak, auth bypass |
| Default credentials | `admin/admin`, `root/root`, `password` | Unauthorized access |
| Empty or no-op validation | `def validate.*pass`, `return true` in validators | Input bypass |
| Wildcard CORS | `Access-Control-Allow-Origin: *` | CSRF |
| Verbose error responses | `traceback`, `stack_trace`, `debug_info` in responses | Info leak |

## Configuration Files to Check

- `.env` / `.env.example` — Default environment values
- Docker Compose files — Port exposure, volume mounts
- CI/CD configs — Secret handling, permissions
- Application configs — Default ports, auth settings, TLS settings
- Web server configs — CORS, CSP, HSTS headers

## Severity Assessment

| Default State | Severity |
|--------------|----------|
| Auth disabled by default | Critical |
| TLS/encryption disabled by default | High |
| Verbose errors in production config | Medium |
| Permissive CORS by default | Medium |
| Debug logging enabled by default | Low-Medium |
| Missing security headers by default | Low |

## Reporting

For each insecure default found:

```markdown
### Insecure Default: [Brief Title]

- **File**: path/to/file, line N
- **Pattern**: [Fail-open / Missing control / Permissive default]
- **Current behavior**: [What happens now]
- **Secure behavior**: [What should happen]
- **Risk**: [What an attacker could exploit]
- **Fix**: [Specific change to make]
```
