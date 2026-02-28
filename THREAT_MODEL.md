# Threat Model: libxml2 2.16.0 Attack Surface Assessment

## Attack Surface

- **XML parsing (parser.c)**: Accepts arbitrary XML byte streams from untrusted sources; primary attack surface for DoS, memory corruption, and entity expansion. Risk: Critical.
- **HTML parsing (HTMLparser.c)**: Lenient HTML5-style parser accepting malformed input; additional parsing paths increase attack surface. Risk: High.
- **XPath evaluation (xpath.c)**: Evaluates XPath expressions against parsed document trees; attacker-controlled documents can force expensive O(n²) comparisons in string value comparison functions. Risk: High.
- **DTD/Schema validation (valid.c, xmlschemas.c, relaxng.c)**: External DTD/schema resources can be loaded if `XML_PARSE_DTDLOAD` is enabled; potential SSRF when resolving external references. Risk: Medium (disabled by default).
- **XML catalog resolution (catalog.c)**: Resolves public/system identifiers via catalog files; trusted input assumed, but catalog files from untrusted sources could redirect entity loads. Risk: Medium.
- **CLI tools (xmllint.c, xmlcatalog.c)**: Command-line entry points processing user-provided files and options. Risk: Low (local execution only).
- **C API (public headers in include/libxml/)**: All exported functions accept untrusted data; consumers set trust boundaries through parser options. Risk: Varies by consumer configuration.

## Trust Boundaries

- **XML_PARSE_NOENT (entity substitution)**: Disabled by default (`substituteEntitiesDefaultValue = 0`). When enabled, external entity references are substituted. Enforcement: opt-in, not enforced.
- **XML_PARSE_DTDLOAD (external DTD loading)**: Disabled by default (`loadExtDtdDefaultValue = 0`). Enforcement: opt-in, not enforced.
- **Entity amplification limit**: Enforced via `xmlCtxtSetMaxAmplification()` with default `XML_MAX_AMPLIFICATION_DEFAULT = 5`. Enforcement: active by default.
- **Network I/O**: HTTP client removed in 2.15 (`nanohttp.c` contains only ABI stubs). Network access now requires consumer-registered I/O callbacks. Enforcement: active by default.
- **Recursion depth (XPath)**: Bounded by `XPATH_MAX_RECURSION_DEPTH = 500-5000`. Enforcement: active.
- **Node set size (XPath)**: Bounded by `XPATH_MAX_NODESET_LENGTH = 10,000,000`. Enforcement: active.
- **Error limit**: Capped at `XML_MAX_ERRORS = 100`. Enforcement: active.

## Attacker Profiles

- **Remote attacker via XML input**: Can supply malicious XML documents to any application using libxml2 without proper parser hardening. Capabilities: craft documents triggering parser edge cases, entity expansion, or quadratic XPath comparisons. Motivation: DoS, RCE in buggy applications.
- **Upstream supply chain attacker**: Could introduce vulnerabilities in libxml2 itself via contributed patches or dependency poisoning. Motivation: widespread impact (libxml2 is used by thousands of projects).
- **Local user**: Can craft malicious XML/HTML files and pass them to `xmllint` or library consumers. Motivation: privilege escalation if xmllint runs setuid, or fuzzing to find new bugs.
- **Schema/DTD author**: A malicious DTD or schema loaded by a trusting application could trigger SSRF (if `XML_PARSE_DTDLOAD` is enabled) or entity amplification.

## Architectural Threats

- **XXE / SSRF via entity expansion**: If consumers enable `XML_PARSE_NOENT` + `XML_PARSE_DTDLOAD` on untrusted input, external entities are fetched and substituted. This is a design-level threat: safe defaults exist but cannot prevent misconfigured consumers. Severity: High. Not fixable by library; mitigated by documentation.
- **Quadratic complexity in XPath string comparison**: `xmlXPathEqualNodeSets` in xpath.c implements O(n×m) string-value comparisons for node-set vs node-set equality/inequality (`=`/`!=`). An attacker-controlled document with large node sets could cause CPU DoS. Severity: Medium. Partially mitigated by `XPATH_MAX_NODESET_LENGTH`.
- **Deprecated global state APIs**: `xmlSubstituteEntitiesDefault()` and `xmlLoadExtDtdDefaultValue()` modify global (thread-local) state, creating risk of inadvertent configuration changes in multi-library environments. Severity: Medium. Inherent in legacy API design.

## Deployment-Resolvable Threats

- **XXE via external entities**: Mitigated by not enabling `XML_PARSE_NOENT` and `XML_PARSE_DTDLOAD` when parsing untrusted input.
- **Billion laughs / entity amplification**: Configurable via `xmlCtxtSetMaxAmplification()`; default is 5×. Increase restriction for highly untrusted environments.
- **Malicious schema files**: Validate schema sources before loading; avoid loading schemas from untrusted origins.
- **SSRF via catalog resolution**: Use `XML_PARSE_NONET` option when loading documents from untrusted sources.

## Assets at Stake

- **Host process memory**: Buffer overflows or use-after-free in the parser could give attackers read/write primitives in the consuming process.
- **File system access**: XXE with `file://` URIs can read arbitrary files readable by the process.
- **Internal network services**: XXE with `http://` URIs (if custom I/O handlers are registered) can probe internal services (SSRF).
- **Downstream library consumers**: Widely used by Python (lxml), PHP, Ruby (Nokogiri), Node.js, GNOME, and many others — a vulnerability here has very broad blast radius.

## Existing Mitigations

- **Entity amplification protection**: `XML_MAX_AMPLIFICATION_DEFAULT = 5` limits billion-laughs attacks by default.
- **Recursion depth limits**: `XPATH_MAX_RECURSION_DEPTH` and parser depth limits prevent stack overflow via deeply nested documents.
- **Node set size limits**: `XPATH_MAX_NODESET_LENGTH = 10,000,000` bounds memory usage in XPath evaluation.
- **HTTP client removal**: As of 2.15, the built-in HTTP client (`nanohttp`) was removed, eliminating a class of SSRF vulnerabilities.
- **Safe defaults**: External entity loading and entity substitution are disabled by default.
- **Pinned GitHub Actions workflows**: All GitHub Actions workflows use full SHA-pinned actions, preventing supply chain compromise via tag mutation.
- **Fuzz testing infrastructure**: `fuzz/` directory contains fuzzing harnesses for continuous security testing.

## Recommended Mitigations

1. **Add SECURITY.md** with private disclosure process and supported version matrix (see separate issue).
2. **Add release signing and SBOM**: Sign release tarballs with sigstore/cosign; generate CycloneDX SBOM at release time. Tracked separately.
3. **Fix out-of-bounds read in `xmlSnprintfElementContent`** (valid.c): Guard `buf[len - 1]` access with `len > 0` check. Low severity but cleanly fixable. Tracked separately.
4. **Add NONET default documentation**: Clearly document in the API reference that `XML_PARSE_NONET` should be used when parsing untrusted content, even though the built-in HTTP client is removed (custom I/O handlers can still fetch network resources).
5. **Consider deprecating global state mutation APIs**: `xmlSubstituteEntitiesDefault()` and similar thread-state setters create implicit configuration risk; encourage per-context options instead.
