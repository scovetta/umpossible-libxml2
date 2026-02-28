# Ecosystem: Elixir

## Identity

- ecosyste.ms registry: `hex.pm`
- Package URL scheme: `pkg:hex/packagename`
- Package manager: Mix (built into Elixir) with Hex as the package registry

## File patterns

- Manifests: `mix.exs`
- Lockfile: `mix.lock`
- Config: `config/config.exs`, `config/dev.exs`, `config/test.exs`, `config/prod.exs`, `.formatter.exs`, `.credo.exs`

## CODEOWNERS paths

```
# Dependencies
mix.exs @depbot
mix.lock @depbot

# Releases
mix.exs @releasebot
```

## CI

- Container image: `elixir:{version}` (includes Erlang/OTP). For specific OTP versions, use `elixir:{elixir-version}-otp-{otp-version}` or `hexpm/elixir:{elixir-version}-erlang-{otp-version}-ubuntu-jammy-20230126`
- Remove `mix.lock` before install in matrix builds to test against fresh dependency resolution
- Version matrix: check endoflife.date/elixir for supported versions. Elixir versions are tied to minimum Erlang/OTP versions — check the compatibility table at hexdocs.pm/elixir/compatibility-and-deprecations.html
- Set `MIX_ENV=test` at the job level

## Commands

| Task | Command |
|------|---------|
| Install deps | `mix deps.get` |
| Compile | `mix compile --warnings-as-errors` |
| Run tests | `mix test` |
| Lint | `mix credo --strict` |
| Format check | `mix format --check-formatted` |
| Type check | `mix dialyzer` (if dialyxir is a dependency) |
| Build release | `mix release` (for applications) |
| Build package | `mix hex.build` |
| Publish | `mix hex.publish` |
| Audit | `mix hex.audit` or `mix deps.audit` |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

## Smoke test

```sh
mix deps.get
mix compile --warnings-as-errors
mix test
```

## Version bumping

Version is set in `mix.exs` in the `project/0` function (the `version` key).

Patch: bug fixes, performance improvements, dependency patches
Minor: new features, new public API, deprecations
Major: breaking changes, removed public API, minimum Elixir/OTP version bump

## Package metadata

`mix.exs` contains package metadata in two places: the `project/0` function (name, version, elixir version requirement, deps) and the `package/0` function (description, licenses, links, files to include). When forking or mirroring, update the links map (`:source_url`, `:homepage_url`) to point to the correct repository URL.

## Notes

- Hex packages include both Elixir source and compiled BEAM bytecode. `mix hex.build` creates the package tarball.
- Elixir projects often use umbrella apps (`apps/` directory with multiple child applications). Each child can be a separate hex package.
- Documentation is generated with ExDoc (`mix docs`) and published to hexdocs.pm alongside the package.
- `mix.lock` pins exact versions including the registry hash. It's more like a verification file than a constraint file.

## Dangerous patterns

- **`Code.eval_string` / `Code.eval_quoted`**: executes arbitrary Elixir code from strings. Never use with user-supplied input. If you need dynamic dispatch, use a map of allowed functions.
- **`:erlang.binary_to_term` without `:safe`**: deserializes arbitrary Erlang terms including functions and references, enabling code execution. Always pass the `:safe` option: `:erlang.binary_to_term(data, [:safe])`.
- **Ecto raw SQL injection**: `Ecto.Adapters.SQL.query(Repo, "SELECT * FROM users WHERE id = '#{id}'")` is injectable. Use parameterized queries: `Ecto.Adapters.SQL.query(Repo, "SELECT * FROM users WHERE id = $1", [id])`. Same applies to `fragment` in Ecto queries.
  ```elixir
  # Vulnerable
  from(u in User, where: fragment("name = '#{^name}'"))
  # Safe
  from(u in User, where: fragment("name = ?", ^name))
  ```
- **Atom exhaustion**: atoms are never garbage collected. Creating atoms from user input (`String.to_atom(user_input)`) can exhaust the atom table and crash the VM. Use `String.to_existing_atom` or keep user data as strings.
- **Hardcoded `secret_key_base`**: Phoenix generates a random `secret_key_base` in `config/dev.exs`, but if the same value is copied to production config and committed to source control, session cookies can be forged. Load secrets from environment variables in `config/runtime.exs`.
- **Insecure defaults**: Phoenix sets `secure: false` on session cookies in development (must be `true` in production), `:crypto` functions use Erlang defaults which may select weak algorithms if not specified, `Plug.SSL` is not enabled by default and must be added to the endpoint for HTTPS enforcement.
