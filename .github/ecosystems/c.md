# Ecosystem: C / C++

## Identity

- ecosyste.ms registry: N/A (no central package registry)
- Package URL scheme: `pkg:generic/packagename` or `pkg:conan/packagename`

## File patterns

- Manifests: `Makefile`, `CMakeLists.txt`, `configure`, `configure.ac`, `meson.build`, `BUILD.bazel`, `MODULE.bazel`, `conanfile.txt`, `conanfile.py`, `vcpkg.json`
- Lockfile: `conan.lock`, `vcpkg-configuration.json`
- Headers: `*.h`, `*.hpp`, `*.hxx`
- Sources: `*.c`, `*.cc`, `*.cpp`, `*.cxx`
- Config: `.clang-format`, `.clang-tidy`, `.editorconfig`, `compile_commands.json`

## CODEOWNERS paths

```
# Build system
Makefile @buildbot
Makefile.in @buildbot
CMakeLists.txt @buildbot
configure @buildbot
configure.ac @buildbot
BUILD.bazel @buildbot
MODULE.bazel @buildbot

# Dependencies
vcpkg.json @depbot
conanfile.txt @depbot
conanfile.py @depbot

# Releases
CMakeLists.txt @releasebot
configure.ac @releasebot
```

## CI

- Container image: `ubuntu:latest` or `gcc:{version}` or `debian:bookworm`
- Install build tools: `apt-get update && apt-get install -y build-essential cmake`
- Cross-platform builds often use matrix with `gcc` and `clang` compilers
- Version matrix: test against multiple compiler versions (GCC 11/12/13/14, Clang 15/16/17/18)
- For CMake projects, test both Debug and Release build types
- Set `CC` and `CXX` environment variables at the job level for compiler selection

## Commands

| Task | Command |
|------|---------|
| Configure (autotools) | `./configure` |
| Configure (CMake) | `cmake -B build -DCMAKE_BUILD_TYPE=Release` |
| Build (Make) | `make -j$(nproc)` |
| Build (CMake) | `cmake --build build` |
| Run tests | `make test` or `ctest --test-dir build` |
| Install | `make install DESTDIR=/tmp/staging` or `cmake --install build --prefix /tmp/staging` |
| Static analysis | `cppcheck --enable=all .` or `scan-build make` |
| Format | `clang-format -i *.c *.h` |
| Lint | `clang-tidy *.c -- -I.` |
| Valgrind | `valgrind --leak-check=full --error-exitcode=1 ./test_binary` |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

## Smoke test

```sh
# Autotools
./configure && make && make test

# CMake
cmake -B build && cmake --build build && ctest --test-dir build
```

## Version bumping

C/C++ projects typically store version in multiple locations that must be kept in sync:
- `configure.ac` or `CMakeLists.txt` (primary version definition)
- Header files (e.g., `zlib.h` defines `ZLIB_VERSION`)
- `*.pc.in` files (pkg-config)
- README or documentation

Patch: bug fixes, security fixes
Minor: new API functions (backward compatible), new features
Major: ABI/API breaking changes, removed functions, changed function signatures

## Package metadata

C libraries typically declare metadata in:
- `configure.ac` / `CMakeLists.txt` (version, project name)
- `*.pc.in` / `*.pc.cmakein` (pkg-config: name, description, version, URL)
- Header files (version macros)
- `LICENSE` file (license declaration)

When forking or mirroring, update the homepage and repository URLs in pkg-config templates and README.

## Dangerous patterns

```c
/* Buffer overflows */
strcpy(dst, src);           /* Use strncpy, strlcpy, or snprintf */
strcat(dst, src);           /* Use strncat or snprintf */
sprintf(buf, fmt, ...);     /* Use snprintf */
gets(buf);                  /* REMOVED in C11 — use fgets */
scanf("%s", buf);           /* Use width-limited: scanf("%255s", buf) */

/* Format string vulnerabilities */
printf(user_input);         /* Use printf("%s", user_input) */
fprintf(fp, user_input);    /* Use fprintf(fp, "%s", user_input) */
syslog(pri, user_input);   /* Use syslog(pri, "%s", user_input) */

/* Integer overflow leading to small allocation */
malloc(n * sizeof(type));   /* Check for overflow: if (n > SIZE_MAX / sizeof(type)) */

/* Command injection */
system(cmd);                /* Avoid; use exec* family with explicit argv */
popen(cmd, mode);           /* Avoid; use pipe + fork + exec */

/* Memory safety */
free(ptr); /* ...later... */ use(ptr);   /* Use-after-free: set ptr = NULL after free */
free(ptr); free(ptr);       /* Double free: set ptr = NULL after free */

/* Missing error checks */
malloc(size);               /* Always check return != NULL */
fopen(path, mode);          /* Always check return != NULL */
read(fd, buf, len);         /* Always check return value and errno */

/* Signed integer overflow (undefined behavior in C) */
int a = INT_MAX; a + 1;    /* Use unsigned or check before arithmetic */

/* Uninitialized memory */
int buf[256]; use(buf);     /* Always initialize: memset or = {0} */
```
