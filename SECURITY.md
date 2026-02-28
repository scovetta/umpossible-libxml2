# Security Policy

## Supported Versions

| Version | Supported          |
| ------- | ------------------ |
| 2.16.x  | :white_check_mark: |
| 2.15.x  | :white_check_mark: |
| 2.14.x  | :white_check_mark: |
| < 2.14  | :x:                |

## Reporting a Vulnerability

**Do not report security vulnerabilities through public GitHub issues.**

To report a security vulnerability, please create a **confidential issue** with
the `security` label in this repository. We will review and respond on a best-effort
basis.

Please include the following in your report:

- A description of the vulnerability and its potential impact
- Steps to reproduce or a proof-of-concept
- The version of libxml2 affected
- Any suggested mitigations (optional)

## Security Update Policy

Security fixes are released as patch versions (e.g., 2.16.x). We recommend
always using the latest release.

## Scope

libxml2 is an open-source C library for parsing XML and HTML. It is widely used
to process both trusted and untrusted data. The library processes structured
documents and is a frequent target for:

- Memory safety issues (buffer overflows, use-after-free, heap corruption)
- Denial of service via malformed input (infinite loops, excessive memory usage)
- XML external entity (XXE) injection when entity expansion is not disabled
- Exponential entity expansion ("Billion Laughs" attack)

## Disclaimer

This is open-source software maintained by volunteers. Processing **untrusted
XML/HTML data** with libxml2 carries inherent risk. If security is critical to
your use case, consider disabling dangerous parser features such as entity
expansion (`XML_PARSE_NOENT`) and external entity loading, and keep the library
up to date.

Refer to the `xmlParserOption` flags for hardening options.
