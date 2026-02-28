# Signing and Provenance

Verification of release signing and build provenance for supply chain security.

## Release Artifacts

Check that releases include proper signing and verification:

### GPG/PGP Signing
- Release tarballs should have `.asc` or `.sig` signature files
- Signing key should be published and retrievable
- Verify signatures match the artifacts

### Sigstore/Cosign
- Container images should be signed with cosign
- Verify signatures against the Sigstore transparency log
- Check for SLSA provenance attestations

### GitHub Attestations
```bash
# Check for attestations on a release
gh attestation verify artifact.tar.gz --repo {owner}/{repo}
```

## Build Provenance (SLSA)

Check for Supply-chain Levels for Software Artifacts compliance:

| Level | Requirement |
|-------|------------|
| SLSA 1 | Build process is documented |
| SLSA 2 | Build service is authenticated, provenance is generated |
| SLSA 3 | Build platform is hardened, provenance is non-forgeable |

### GitHub Actions Provenance
- Check for `actions/attest-build-provenance` in workflows
- Verify `permissions: id-token: write` is set for OIDC
- Check for `slsa-framework/slsa-github-generator` usage

## Verification Checklist

- [ ] Release artifacts are signed (GPG or Sigstore)
- [ ] Signing key is documented and accessible
- [ ] Build provenance is generated and published
- [ ] Provenance can be verified independently
- [ ] No artifacts are published without signing
- [ ] CI/CD pipeline has appropriate access controls
- [ ] Workflow files are protected from unauthorized modification

## Finding Format

```markdown
### Supply Chain Finding

- **Type**: Missing signing / Weak provenance / Unsigned artifact
- **Severity**: Medium-High
- **Details**: [What's missing or misconfigured]
- **Recommendation**: [Specific steps to add signing/provenance]
```
