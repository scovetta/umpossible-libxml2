# Ecosystem: NuGet

## Identity

- ecosyste.ms registry: `nuget.org`
- Package URL scheme: `pkg:nuget/packagename`

## File patterns

- Manifests: `*.csproj`, `*.fsproj`, `*.vbproj`, `Directory.Build.props`, `Directory.Packages.props`
- Lockfile: `packages.lock.json` (when enabled with `RestorePackagesWithLockFile`)
- Config: `nuget.config`, `global.json`, `Directory.Build.targets`
- Solution files: `*.sln`

## CODEOWNERS paths

```
# Dependencies
*.csproj @depbot
*.fsproj @depbot
Directory.Build.props @depbot
Directory.Packages.props @depbot
packages.lock.json @depbot

# Releases
*.csproj @releasebot
Directory.Build.props @releasebot
```

## CI

- Container image: `mcr.microsoft.com/dotnet/sdk:{version}`
- Version matrix: check endoflife.date/dotnet for supported versions (currently .NET 6, 8, 9)
- Use `global.json` to pin SDK version if present

## Commands

| Task | Command |
|------|---------|
| Install deps | `dotnet restore` |
| Run tests | `dotnet test` |
| Lint | `dotnet format --verify-no-changes` |
| Build | `dotnet build -c Release` |
| Pack | `dotnet pack -c Release` |
| Publish | `dotnet nuget push *.nupkg -s https://api.nuget.org/v3/index.json -k $NUGET_API_KEY` |
| Audit | `dotnet list package --vulnerable` |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

## Smoke test

```sh
dotnet build -c Release
dotnet test --no-build -c Release
```

## Version bumping

Version is typically set in the `.csproj` file (`<Version>` or `<PackageVersion>` element) or in `Directory.Build.props` for multi-project repos.

Patch: bug fixes, performance improvements, dependency patches
Minor: new features, new public API, deprecations
Major: breaking changes, removed public API, minimum .NET version bump

## Package metadata

The `.csproj` file (or `Directory.Build.props`) contains package metadata: `PackageId`, `Version`, `Description`, `PackageProjectUrl`, `RepositoryUrl`, `License`. When forking or mirroring, update `PackageProjectUrl` and `RepositoryUrl` to point to the correct repository URL.

## Dangerous patterns

- **`BinaryFormatter` / `NetDataContractSerializer`**: deserializes arbitrary .NET objects from untrusted data, leading to RCE. Both are obsolete as of .NET 8. Use `System.Text.Json` or `XmlSerializer` with known types.
- **`Process.Start` injection**: `Process.Start("cmd.exe", "/c " + userInput)` passes input through a shell. Use `ProcessStartInfo` with `ArgumentList` (which handles escaping) or pass arguments without a shell.
  ```csharp
  // Vulnerable
  Process.Start("cmd.exe", $"/c ping {userInput}");
  // Safe
  var psi = new ProcessStartInfo("ping") { UseShellExecute = false };
  psi.ArgumentList.Add(userInput);
  Process.Start(psi);
  ```
- **SQL string interpolation**: `$"SELECT * FROM Users WHERE Id = {id}"` passed to `SqlCommand` is injectable. Use parameterized queries with `SqlParameter` or an ORM.
- **XML DTD processing (XXE)**: `XmlReader` and `XmlDocument` resolve external entities by default in older .NET Framework versions. Set `DtdProcessing = DtdProcessing.Prohibit` or use `XmlReaderSettings` with `ProhibitDtd = true`.
- **`ServerCertificateValidationCallback` bypass**: setting `ServicePointManager.ServerCertificateValidationCallback = (s, cert, chain, errors) => true` disables TLS certificate validation globally. Never ship this outside local development.
- **Missing `[ValidateAntiForgeryToken]`**: ASP.NET MVC actions that handle POST/PUT/DELETE without `[ValidateAntiForgeryToken]` are vulnerable to CSRF. Apply the attribute to all state-changing actions, or use the global `AutoValidateAntiforgeryToken` filter.
- **Insecure defaults**: `HttpClientHandler.ServerCertificateCustomValidationCallback` returning `true`, `CookieOptions` without `Secure = true` and `HttpOnly = true`, ASP.NET Core apps without `UseHttpsRedirection`, `JsonSerializerOptions.MaxDepth` defaults to 64 which may be too deep for untrusted input.
