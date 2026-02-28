# Dangerous Code Patterns

Cross-language vulnerability categories that security and code review agents should check for. This is a reference for all agents, not just securibot.

## Memory Safety

| Pattern | Languages | Risk |
|---------|-----------|------|
| Buffer overflow | C, C++ | Arbitrary code execution |
| Use-after-free | C, C++ | Arbitrary code execution |
| Double free | C, C++ | Heap corruption |
| Stack buffer overflow | C, C++ | Control flow hijack |
| Integer overflow leading to small allocation | C, C++, Rust (in unsafe) | Heap overflow |
| Off-by-one in loop bounds | All | Out-of-bounds access |
| Null pointer dereference | C, C++, Java | Crash / DoS |
| Uninitialized memory read | C, C++ | Information leak |

## Injection

| Pattern | Languages | Risk |
|---------|-----------|------|
| SQL injection | Any with SQL | Data breach |
| Command injection | Any with `exec`/`system`/`popen` | RCE |
| Path traversal | All | Arbitrary file access |
| Format string vulnerability | C (printf family) | Info leak / RCE |
| Template injection (SSTI) | Python, Ruby, JS | RCE |
| LDAP injection | Java, .NET | Auth bypass |
| XML External Entity (XXE) | Java, Python, PHP | SSRF / file read |
| Deserialization of untrusted data | Java, Python, PHP, Ruby, .NET | RCE |

## Cryptographic Weaknesses

| Pattern | Risk |
|---------|------|
| Hardcoded secrets/keys | Credential exposure |
| Weak hash (MD5, SHA1 for security) | Collision attacks |
| Weak cipher (DES, RC4, ECB mode) | Data exposure |
| Predictable random (rand(), Math.random() for security) | Guessable tokens |
| Missing certificate validation | MITM attacks |
| Timing side-channels in comparisons | Token bypass |
| Custom crypto implementation | All the above |

## Authentication & Authorization

| Pattern | Risk |
|---------|------|
| Missing authentication check | Unauthorized access |
| Broken authorization (IDOR) | Privilege escalation |
| Hardcoded credentials | Permanent backdoor |
| Weak session management | Session hijack |
| Missing CSRF protection | Unauthorized actions |
| JWT without signature verification | Auth bypass |

## Resource Management

| Pattern | Languages | Risk |
|---------|-----------|------|
| Unbounded allocation | All | DoS via OOM |
| Missing resource cleanup | C, C++, Java | Resource leak |
| Unchecked recursion depth | All | Stack overflow DoS |
| Missing timeout on network ops | All | Hung process / DoS |
| File descriptor leak | C, C++, Python, Go | Resource exhaustion |
| Unbounded loop on input | All | CPU DoS |

## Error Handling

| Pattern | Risk |
|---------|------|
| Swallowing exceptions silently | Hidden failures |
| Error messages with stack traces to user | Information leak |
| Catch-all without re-throw | Hidden security failures |
| Missing error check on security ops | Bypass |
| Logging sensitive data in errors | Information leak |

## Supply Chain

| Pattern | Risk |
|---------|------|
| Dependency from untrusted source | Malicious code |
| Unpinned dependency versions | Supply chain attack |
| Post-install scripts in dependencies | Arbitrary code at install time |
| Typosquatting risk (misspelled packages) | Malicious dependency |
| Abandoned dependency (no updates >2yr) | Unpatched vulnerabilities |

## Detection Tips

When reviewing code, search for these patterns:

### C/C++
```
strcpy, strcat, sprintf, gets     → Buffer overflow
malloc without size check          → Integer overflow
free() followed by use             → Use-after-free
printf(user_input)                 → Format string
system(), popen(), exec*()         → Command injection
```

### Python
```
eval(), exec()                     → Code injection
subprocess.shell=True              → Command injection
pickle.loads(untrusted)            → Deserialization RCE
os.path.join(base, user_input)     → Path traversal (if no validation)
yaml.load() without Loader         → Arbitrary code execution
```

### JavaScript
```
eval(), Function()                 → Code injection
innerHTML, document.write          → XSS
child_process.exec(user_input)     → Command injection
JSON.parse without try/catch       → DoS via malformed input
fs operations with user paths      → Path traversal
```

### Ruby
```
eval, send, public_send            → Code injection
system, backtick, %x               → Command injection
Marshal.load(untrusted)            → Deserialization RCE
ERB.new(user_input).result         → Template injection
```

### Go
```
exec.Command with user input       → Command injection
sql.Query with string concat       → SQL injection
http.Get without timeout           → Resource exhaustion
ioutil.ReadAll on untrusted input  → OOM DoS
```

### Rust
```
unsafe blocks                      → All memory safety bets off
.unwrap() on untrusted input       → Panic DoS
std::process::Command + user input → Command injection
transmute                          → Type confusion
```
