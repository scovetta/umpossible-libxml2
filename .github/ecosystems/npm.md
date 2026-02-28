# Ecosystem: npm

## Identity

- ecosyste.ms registry: `npmjs.org`
- Package URL scheme: `pkg:npm/packagename`

## File patterns

- Manifests: `package.json`
- Lockfile: `package-lock.json`
- Config: `.npmrc`, `.nvmrc`, `.node-version`

## CODEOWNERS paths

```
# Dependencies
package.json @depbot
package-lock.json @depbot

# Releases
package.json @releasebot
```

## CI

- Container image: `node:{version}`
- Remove lockfile before install in matrix builds so dependencies resolve fresh per version
- Version matrix: check endoflife.date/nodejs for supported versions

## Commands

| Task | Command |
|------|---------|
| Install deps | `npm install` |
| Run tests | `npm test` |
| Lint | `npm run lint` |
| Build | `npm run build` (if build script exists) |
| Publish | `npm publish` |
| Audit | `npm audit` |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

## Smoke test

```sh
npm pack && npm install *.tgz
node -e "require('packagename'); console.log('OK')"
```

## Version bumping

Patch: bug fixes, performance improvements, dependency patches
Minor: new features, backward-compatible API additions
Major: breaking changes, removed API, minimum Node.js version bump

## Package metadata

`package.json` contains package metadata: name, version, homepage, repository, license. When forking or mirroring, update the homepage and repository fields to point to the correct repository URL.

## Dangerous patterns

- **Prototype pollution**: merging user-controlled objects into defaults with recursive spread or `Object.assign` lets attackers set `__proto__` properties. Use `Object.create(null)` for lookup maps, or validate keys.
  ```js
  // Vulnerable: attacker sends {"__proto__": {"admin": true}}
  function merge(target, source) {
    for (const key in source) {
      target[key] = source[key]; // pollutes Object.prototype
    }
  }
  ```
- **`eval` / `Function()` / `vm.runInNewContext`**: executes arbitrary strings as code. Avoid entirely with user input; use `JSON.parse` for data, structured parsers for expressions.
- **`child_process.exec` injection**: passes the command through a shell, so interpolated user input is injectable. Use `child_process.execFile` or `spawn` with an array of arguments.
  ```js
  // Vulnerable
  exec(`git clone ${userUrl}`);
  // Safe
  execFile('git', ['clone', userUrl]);
  ```
- **ReDoS**: regular expressions with nested quantifiers (e.g. `(a+)+$`) cause exponential backtracking on crafted input. Use `re2` or test regexes with worst-case inputs.
- **Path traversal via `path.join`**: `path.join('/uploads', userInput)` does not prevent `../` sequences. Resolve the path and verify it starts with the intended directory.
- **Unvalidated redirects**: `res.redirect(req.query.next)` sends users to arbitrary URLs. Validate against an allowlist or ensure the target is a relative path.
- **Insecure defaults**: Express does not set security headers by default (use `helmet`), `cors()` with no options allows all origins, `cookie-session` without `secure: true` and `httpOnly: true` sends cookies over plain HTTP, `jsonwebtoken` defaults to `HS256` which needs a strong secret.
