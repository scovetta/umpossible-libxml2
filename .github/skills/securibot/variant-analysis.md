# Variant Analysis

When a vulnerability is found, search for similar patterns across the codebase.

## Process

### 1. Root Cause Framing

Don't just fix the symptom — understand the pattern:

| Surface Bug | Root Cause Frame |
|------------|-----------------|
| Buffer overflow in `parse_header()` | "All functions that copy user input to fixed buffers" |
| Missing NULL check in `get_config()` | "All functions that dereference return values without NULL checks" |
| SQL injection in `user_search()` | "All database queries built with string concatenation" |
| Hardcoded API key in `client.py` | "All files containing string literals matching credential patterns" |

### 2. Search Expansion

Once you have the root cause frame, search broadly:

```bash
# Example: Find all unbounded string copies in C code
grep -rn 'strcpy\|strcat\|sprintf' --include='*.c' --include='*.h' .

# Example: Find all exec/system calls
grep -rn 'system(\|popen(\|exec(' --include='*.c' --include='*.py' --include='*.rb' .

# Example: Find potential credential patterns
grep -rn 'password\|secret\|api_key\|token' --include='*.py' --include='*.js' --include='*.rb' . | grep -v test | grep -v '\.md'
```

### 3. Classification

For each variant found, classify:

- **True positive**: Same vulnerability pattern, exploitable
- **Mitigated**: Same pattern but with compensating controls (bounds check nearby, input validation upstream)
- **False positive**: Similar syntax but not actually vulnerable (e.g., `strcpy` with compile-time-constant source)

### 4. Reporting

Report variants grouped by root cause:

```markdown
## Variant Analysis: [Root Cause]

### Original Finding
- File: `src/parser.c:148`
- Pattern: Unbounded `strcpy` of user input

### Variants Found

| # | File | Line | Status | Notes |
|---|------|------|--------|-------|
| 1 | src/handler.c | 92 | Vulnerable | Same pattern, no bounds check |
| 2 | src/config.c | 55 | Mitigated | Input pre-validated at line 42 |
| 3 | src/util.c | 201 | False positive | Source is compile-time constant |

### Recommendation
[Describe the systematic fix that addresses all variants]
```

## Automation Tips

### Regex Patterns for Common Vulnerabilities

```bash
# C buffer overflows
'strcpy\s*\(|strcat\s*\(|sprintf\s*\(|gets\s*\('

# Format string vulnerabilities
'printf\s*\(\s*[^"]\|fprintf\s*\([^,]+,\s*[^"]'

# Command injection (multi-language)
'system\s*\(|popen\s*\(|exec\s*\(|spawn\s*\(|child_process'

# Deserialization
'pickle\.loads?\s*\(|Marshal\.load\s*\(|yaml\.load\s*\(|ObjectInputStream'

# Hardcoded secrets
'(?i)(password|secret|api.?key|token)\s*[=:]\s*["\x27][^"\x27]{8,}'
```

### False Positive Reduction

Before reporting a variant:
1. Check if there's input validation upstream in the call chain
2. Check if the data source is actually untrusted
3. Check if there's a compensating control (e.g., sandboxing)
4. Check if the code is in test files (lower severity)
