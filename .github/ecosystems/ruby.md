# Ecosystem: Ruby

## Identity

- ecosyste.ms registry: `rubygems.org`
- Package URL scheme: `pkg:gem/packagename`

## File patterns

- Manifests: `Gemfile`, `*.gemspec`
- Lockfile: `Gemfile.lock`
- Config: `.rubocop.yml`, `.ruby-version`, `.gemrc`
- Version file: `lib/*/version.rb`

## CODEOWNERS paths

```
# Dependencies
Gemfile @depbot
Gemfile.lock @depbot
*.gemspec @depbot

# Releases
*.gemspec @releasebot
```

## CI

- Container image: `ruby:{version}`
- Install: `bundle install`
- Remove lockfile before install in matrix builds so dependencies resolve fresh per version
- Set `BUNDLE_WITHOUT: development` at the job level (not step level) to skip dev-only groups
- Version matrix: check endoflife.date/ruby for supported versions

## Commands

| Task | Command |
|------|---------|
| Install deps | `bundle install` |
| Run tests | `rake test` or `bundle exec rspec` |
| Lint | `bundle exec rubocop` |
| Build | `gem build *.gemspec` |
| Publish | `gem push *.gem` |
| Audit | `bundler-audit check` |
| SBOM | `cyclonedx-ruby -p . -o sbom.json` |

## Smoke test

```sh
gem install *.gem
ruby -e "require 'packagename'; puts 'OK'"
```

## Version bumping

Patch: bug fixes, performance improvements, dependency patches
Minor: new features, new public API, deprecations
Major: breaking changes, removed public API, minimum Ruby version bump

## Package metadata

The gemspec file (`*.gemspec`) contains package metadata: name, version, homepage, source code URI, changelog URI, license. When forking or mirroring, update the homepage and source code URIs to point to the correct repository URL.

## Dangerous patterns

- **`eval` / `send` / `constantize` with user input**: arbitrary code execution. Use allowlists instead of dynamic dispatch.
- **`YAML.load`**: deserializes arbitrary Ruby objects, leading to RCE. Use `YAML.safe_load`.
- **Mass assignment**: `update(params)` without strong parameters lets attackers set admin flags. Always use `permit`.
- **SQL via string interpolation**: `where("name = '#{input}'"` is injectable. Use `where("name = ?", input)` or hash syntax.
- **Command injection**: backticks, `system()`, `exec()`, `%x{}` with interpolated strings. Use `Open3.capture3` with array args.
- **Regex anchors**: `^` and `$` match line boundaries, not string boundaries. Use `\A` and `\z` for validation.
  ```ruby
  # Bypassable with "valid\n<script>alert(1)</script>"
  /^https?:\/\/example\.com/
  # Correct
  /\Ahttps?:\/\/example\.com/
  ```
- **`ERB.new` with user input**: server-side template injection. Never pass user data as the template.
- **`File.open` pipe prefix**: `File.open("| rm -rf /")` executes commands. Validate paths don't start with `|`.
- **Path traversal**: `File.read(params[:file])` with `../` sequences. Use `File.expand_path` and verify the result is within the expected directory.
- **Insecure defaults**: `YAML.load` vs `YAML.safe_load` (the default method name is the dangerous one), `Rails.application.config.force_ssl = false` or missing, `protect_from_forgery` removed or set to `with: :null_session` without reason, session secret derived from app name or guessable value, `config.consider_all_requests_local = true` leaking to production.
