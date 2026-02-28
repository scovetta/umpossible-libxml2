# Ecosystem: PHP

## Identity

- ecosyste.ms registry: `packagist.org`
- Package URL scheme: `pkg:composer/vendor/package`

## File patterns

- Manifests: `composer.json`
- Lockfile: `composer.lock`
- Config: `php.ini`, `phpunit.xml`, `phpstan.neon`, `.php-cs-fixer.php`, `phpcs.xml`

## CODEOWNERS paths

```
# Dependencies
composer.json @depbot
composer.lock @depbot

# Releases
composer.json @releasebot
```

## CI

- Container image: `php:{version}`
- Install Composer inside the container: `curl -sS https://getcomposer.org/installer | php -- --install-dir=/usr/local/bin --filename=composer`
- Remove `composer.lock` before install in matrix builds so dependencies resolve fresh per version
- Version matrix: check endoflife.date/php for supported versions

## Commands

| Task | Command |
|------|---------|
| Install deps | `composer install` |
| Run tests | `./vendor/bin/phpunit` |
| Lint | `./vendor/bin/phpstan analyse` or `./vendor/bin/phpcs` |
| Format check | `./vendor/bin/php-cs-fixer fix --dry-run --diff` |
| Build | N/A (PHP packages are distributed as source) |
| Publish | Packagist auto-publishes from tagged releases via webhook or API |
| Audit | `composer audit` |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

## Smoke test

```sh
composer install --no-dev
php -r "require 'vendor/autoload.php'; echo 'OK' . PHP_EOL;"
```

## Version bumping

Patch: bug fixes, performance improvements, dependency patches
Minor: new features, new public API, deprecations
Major: breaking changes, removed public API, minimum PHP version bump

## Package metadata

`composer.json` contains package metadata: name, description, homepage, license, support URLs. When forking or mirroring, update the homepage and support URLs to point to the correct repository URL.

## Dangerous patterns

- **`eval` / `assert` with strings**: `eval($userInput)` and `assert($userInput)` (before PHP 8.0) execute arbitrary code. Avoid entirely; there is almost never a legitimate reason to eval user data.
- **`unserialize` on untrusted data**: deserializes arbitrary PHP objects, enabling RCE through magic methods (`__wakeup`, `__destruct`). Use `json_decode` for data interchange, or pass `['allowed_classes' => false]` as the second argument.
- **`shell_exec` / backticks / `system` / `passthru` / `exec`**: all execute shell commands. Interpolated user input is injectable. Use `escapeshellarg()` on each argument, or avoid shell commands in favor of PHP built-in functions.
  ```php
  // Vulnerable
  system("convert " . $_GET['file'] . " output.png");
  // Safer
  system("convert " . escapeshellarg($_GET['file']) . " output.png");
  ```
- **`include` / `require` with user paths (LFI/RFI)**: `include($_GET['page'] . '.php')` allows local file inclusion and, if `allow_url_include` is on, remote code execution. Use an allowlist of valid pages.
- **`extract($_POST)` / `extract($_GET)`**: imports all request parameters as local variables, overwriting existing ones (including auth flags). Never use `extract` on superglobals.
- **`file_get_contents` SSRF**: `file_get_contents($userUrl)` fetches arbitrary URLs including internal services (`http://169.254.169.254/`). Validate and restrict URLs before fetching.
- **SQL interpolation**: `$db->query("SELECT * FROM users WHERE id = '$id'")` is injectable. Use prepared statements with `PDO::prepare` and bound parameters.
- **Insecure defaults**: `display_errors = On` in production leaks paths and stack traces, `allow_url_include = On` enables remote file inclusion, `session.cookie_httponly = 0` exposes session cookies to JavaScript, `session.cookie_secure = 0` sends session cookies over HTTP, missing `Content-Security-Policy` and `X-Content-Type-Options` headers.
