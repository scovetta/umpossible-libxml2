# Ecosystem: Go

## Identity

- ecosyste.ms registry: `proxy.golang.org`
- Package URL scheme: `pkg:golang/module@version`

## File patterns

- Manifests: `go.mod`
- Lockfile: `go.sum`
- Config: `.golangci.yml`

## CODEOWNERS paths

```
# Dependencies
go.mod @depbot
go.sum @depbot

# Releases
go.mod @releasebot
```

## CI

- Container image: `golang:{version}`
- No lockfile removal needed (go.sum is a verification file, not a constraint file)
- Version matrix: check endoflife.date/go for supported versions

## Commands

| Task | Command |
|------|---------|
| Install deps | `go mod download` |
| Run tests | `go test ./...` |
| Lint | `go vet ./...` or `golangci-lint run` |
| Build | `go build ./...` |
| Publish | Tag and push (Go modules publish via proxy automatically) |
| Audit | `govulncheck ./...` |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

## Smoke test

```sh
go build ./...
go test ./... -count=1
```

## Version bumping

Go modules use git tags for versioning (e.g. `v1.2.3`).
Patch: bug fixes, performance improvements
Minor: new features, backward-compatible API changes
Major: breaking changes (requires new major version path in module, e.g. `/v2`)

## Package metadata

`go.mod` contains the module path and Go version requirement. No separate package metadata file. Version is determined by git tags.

## Dangerous patterns

- **Silent integer overflow**: `int32(largeInt64)` truncates without error. Check bounds before casting, or use `math.MaxInt32`.
- **Slice aliasing after append**: appending to a slice may or may not allocate a new backing array. Copy slices explicitly if the original must not be modified.
  ```go
  // Bug: b may share backing array with a
  b := append(a[:2], newItem)
  // Safe: copy into a new slice
  b := make([]int, 2, 3)
  copy(b, a[:2])
  b = append(b, newItem)
  ```
- **Interface nil confusion**: a nil pointer stored in an interface is not a nil interface. Always return the interface type directly, not a concrete nil pointer.
- **JSON decoder quirks**: `encoding/json` matches field names case-insensitively and silently ignores unknown fields. Call `Decoder.DisallowUnknownFields()` when strict parsing matters.
- **`defer` in loops**: deferred calls don't run until the function returns, not at end of loop iteration. Extract the loop body into a function or close resources manually.
- **Goroutine leaks**: a goroutine blocked on a channel that nobody reads from never gets garbage collected. Use `context.Context` for cancellation and `select` with a done channel.
- **Concurrent map access**: reading and writing a `map` from multiple goroutines panics. Use `sync.Map` or protect with a mutex.
- **Error shadowing**: `:=` in an inner scope creates a new variable instead of assigning to the outer `err`. Use `=` when the variable already exists.
  ```go
  var err error
  if condition {
      // Bug: shadows outer err, which stays nil
      result, err := doSomething()
      // Fix: use = instead of :=
      result, err = doSomething()
  }
  ```
- **Insecure defaults**: TLS config without `MinVersion` defaults to TLS 1.0, `InsecureSkipVerify: true` disables certificate checks, HTTP server without `ReadTimeout`/`WriteTimeout` defaults to unlimited, `os.MkdirAll` with 0777 permissions.
