# Ecosystem: Java

## Identity

- ecosyste.ms registry: `maven.org`
- Package URL scheme: `pkg:maven/groupId/artifactId`

## File patterns

- Manifests: `pom.xml` (Maven), `build.gradle`, `build.gradle.kts` (Gradle)
- Lockfile: `gradle.lockfile` (Gradle with locking enabled). Maven has no lockfile.
- Config: `settings.xml`, `gradle.properties`, `mvnw`, `gradlew`
- Wrapper scripts: `mvnw`/`gradlew` (prefer these over system-installed Maven/Gradle)

## CODEOWNERS paths

```
# Dependencies
pom.xml @depbot
build.gradle @depbot
build.gradle.kts @depbot
gradle.lockfile @depbot

# Releases
pom.xml @releasebot
build.gradle @releasebot
build.gradle.kts @releasebot
```

## CI

- Container image: `eclipse-temurin:{version}` (or `maven:{version}` for Maven projects, `gradle:{version}` for Gradle)
- Version matrix: check endoflife.date/java for supported LTS versions (currently 11, 17, 21)
- If the project includes `mvnw` or `gradlew`, use those instead of system Maven/Gradle

## Commands

### Maven projects

| Task | Command |
|------|---------|
| Install deps | `./mvnw dependency:resolve` or `mvn dependency:resolve` |
| Run tests | `./mvnw test` |
| Lint | `./mvnw checkstyle:check` or `./mvnw spotbugs:check` |
| Build | `./mvnw package -DskipTests` |
| Publish | `./mvnw deploy` |
| Audit | `./mvnw org.owasp:dependency-check-maven:check` |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

### Gradle projects

| Task | Command |
|------|---------|
| Install deps | `./gradlew dependencies` |
| Run tests | `./gradlew test` |
| Lint | `./gradlew checkstyleMain` or `./gradlew spotbugsMain` |
| Build | `./gradlew build -x test` |
| Publish | `./gradlew publish` |
| Audit | `./gradlew dependencyCheckAnalyze` |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

## Smoke test

```sh
# Maven
./mvnw package -DskipTests
java -jar target/*.jar --version 2>/dev/null || echo "Library — no main class"

# Gradle
./gradlew build -x test
java -jar build/libs/*.jar --version 2>/dev/null || echo "Library — no main class"
```

## Version bumping

Patch: bug fixes, performance improvements, dependency patches
Minor: new features, new public API, deprecations
Major: breaking changes, removed public API, minimum Java version bump

## Package metadata

`pom.xml` (Maven) or `build.gradle` (Gradle) contains package metadata: groupId, artifactId, version, description, URL, license, SCM. When forking or mirroring, update the URL and SCM fields to point to the correct repository URL.

## Dangerous patterns

- **`ObjectInputStream` deserialization**: deserializing untrusted data via `ObjectInputStream.readObject()` leads to remote code execution through gadget chains (commons-collections, etc.). Use allowlist-based `ObjectInputFilter` (Java 9+) or avoid Java serialization entirely in favor of JSON.
- **JNDI injection**: passing user input to `InitialContext.lookup(userInput)` allows remote class loading and RCE (the Log4Shell pattern). Validate JNDI names against an allowlist, disable remote codebases with `com.sun.jndi.ldap.object.trustURLCodebase=false`.
- **XXE (XML External Entity)**: default XML parsers resolve external entities, allowing file reads and SSRF. Disable DTDs and external entities on `DocumentBuilderFactory`, `SAXParserFactory`, and `XMLInputFactory`.
  ```java
  DocumentBuilderFactory dbf = DocumentBuilderFactory.newInstance();
  dbf.setFeature("http://apache.org/xml/features/disallow-doctype-decl", true);
  ```
- **`Runtime.exec` / `ProcessBuilder` injection**: concatenating user input into command strings is injectable. Pass arguments as separate list elements.
  ```java
  // Vulnerable
  Runtime.getRuntime().exec("grep " + userInput + " /var/log/app.log");
  // Safe
  new ProcessBuilder("grep", userInput, "/var/log/app.log").start();
  ```
- **SQL string concatenation**: `stmt.execute("SELECT * FROM users WHERE id = '" + id + "'")` is injectable. Use `PreparedStatement` with parameterized queries.
- **`Random` vs `SecureRandom`**: `java.util.Random` is predictable and must not be used for tokens, session IDs, or cryptographic purposes. Use `SecureRandom`.
- **Insecure defaults**: `X509TrustManager` that accepts all certificates (empty `checkServerTrusted`), `HostnameVerifier` returning `true` for all hosts, `SSLContext` initialized with permissive trust managers, XML parsers with DTD processing enabled, Spring CSRF protection disabled via `http.csrf().disable()`.
