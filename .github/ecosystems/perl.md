# Ecosystem: Perl

## Identity

- ecosyste.ms registry: `cpan.org`
- Package URL scheme: `pkg:cpan/Distribution-Name`
- Package manager: CPAN (via `cpanm` or `cpan` client)

## File patterns

- Manifests: `Makefile.PL` (ExtUtils::MakeMaker), `Build.PL` (Module::Build), `META.json`, `META.yml`, `cpanfile`
- Lockfile: `cpanfile.snapshot` (when using Carton)
- Config: `.perlcriticrc`, `.perltidyrc`, `MANIFEST`, `MANIFEST.SKIP`
- Version file: version is typically in the main module file (`lib/My/Module.pm`) as `$VERSION`

## CODEOWNERS paths

```
# Dependencies
Makefile.PL @depbot
Build.PL @depbot
cpanfile @depbot
META.json @depbot
META.yml @depbot

# Releases
Makefile.PL @releasebot
Build.PL @releasebot
```

## CI

- Container image: `perl:{version}`
- Install cpanminus inside the container: `cpan App::cpanminus` or `curl -L https://cpanmin.us | perl - App::cpanminus`
- Install deps: `cpanm --installdeps .` or `cpanm --with-develop --installdeps .` (to include author/test deps)
- Version matrix: check endoflife.date/perl for supported versions. Perl has long support cycles — even-numbered releases (5.36, 5.38, 5.40) are stable.

## Commands

| Task | Command |
|------|---------|
| Install deps | `cpanm --installdeps .` |
| Run tests | `prove -l t/` or `make test` |
| Lint | `perlcritic lib/` |
| Format | `perltidy -b lib/**/*.pm` |
| Build | `perl Makefile.PL && make` or `perl Build.PL && ./Build` |
| Build dist | `make dist` or `./Build dist` (creates a .tar.gz) |
| Publish | `cpan-upload My-Module-0.01.tar.gz` (via CPAN::Uploader) |
| Audit | `cpan-audit` (via CPAN::Audit) |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

## Smoke test

```sh
perl Makefile.PL && make && make test
# or
perl Build.PL && ./Build && ./Build test
```

## Version bumping

Perl uses decimal or dotted-decimal versions (e.g. `0.42`, `1.003004`, `v1.2.3`). Convention varies by project. Follow whatever the module already uses.

Patch: bug fixes, test fixes, documentation
Minor: new features, new exports, deprecations
Major: breaking changes, removed API, minimum Perl version bump

## Package metadata

`Makefile.PL` or `Build.PL` contains package metadata: name, version, abstract, author, license, repository URL, bugtracker URL. `META.json`/`META.yml` are generated from these during `make dist`. When forking or mirroring, update the repository and bugtracker resources to point to the correct repository URL.

## Notes

- CPAN distributions use `::` for module namespacing but `-` for distribution names (e.g. module `My::Module` lives in distribution `My-Module`)
- PAUSE (the upload system) requires an author account.
- Many older CPAN modules use `Makefile.PL` with `ExtUtils::MakeMaker`. Newer ones often use `Dist::Zilla` or `Minilla` which generate `Makefile.PL`/`Build.PL` at release time.

## Dangerous patterns

- **String `eval`**: `eval "some $string"` executes arbitrary Perl code. Use `eval { block }` for exception handling instead. Never eval user-supplied strings.
- **Two-argument `open` with pipe injection**: `open(FH, $filename)` interprets leading `|` or trailing `|` as a command pipe. Use three-argument open: `open(my $fh, '<', $filename)`.
  ```perl
  # Vulnerable: $file = "| rm -rf /"
  open(FH, $file);
  # Safe
  open(my $fh, '<', $file) or die "Cannot open: $!";
  ```
- **`system()` / backticks with interpolation**: `system("grep $input file.txt")` is injectable. Use the list form: `system("grep", $input, "file.txt")`.
- **`Storable::thaw` / `Storable::retrieve`**: deserializes arbitrary Perl data structures, which can trigger destructors with side effects. Never deserialize untrusted data with `Storable`. Use JSON instead.
- **Taint mode bypass**: Perl's taint mode (`-T`) tracks untrusted data, but regex captures (`$1`) automatically untaint. A permissive regex like `(.*)` defeats the entire mechanism. Untaint with restrictive patterns only.
- **Regex injection**: interpolating user input directly into a regex (`m/$user_input/`) allows ReDoS or unexpected matching behavior. Use `\Q...\E` to escape metacharacters: `m/\Q$user_input\E/`.
- **Insecure defaults**: `LWP::UserAgent` does not verify SSL certificates by default (set `ssl_opts => {verify_hostname => 1, SSL_ca_file => ...}`), `CGI.pm` does not set `HttpOnly` or `Secure` cookie flags, `DBI` connections without `RaiseError => 1` silently ignore SQL errors.
