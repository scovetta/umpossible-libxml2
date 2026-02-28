# Ecosystem: Python

## Identity

- ecosyste.ms registry: `pypi.org`
- Package URL scheme: `pkg:pypi/packagename`

## File patterns

- Manifests: `pyproject.toml`, `setup.py`, `setup.cfg`, `requirements.txt`
- Lockfile: `requirements.txt` (when pinned), `poetry.lock`, `Pipfile.lock`
- Config: `pyproject.toml`, `tox.ini`, `.flake8`, `ruff.toml`, `.python-version`

## CODEOWNERS paths

```
# Dependencies
pyproject.toml @depbot
setup.py @depbot
setup.cfg @depbot
requirements*.txt @depbot
poetry.lock @depbot
Pipfile* @depbot

# Releases
pyproject.toml @releasebot
setup.py @releasebot
setup.cfg @releasebot
```

## CI

- Container image: `python:{version}`
- Install: `pip install -e .` or `pip install -e .[dev]`
- Remove lockfiles before install in matrix builds so dependencies resolve fresh per version
- Version matrix: check endoflife.date/python for supported versions
- Set `PIP_NO_CACHE_DIR: 1` at the job level to avoid cache issues in CI

## Commands

| Task | Command |
|------|---------|
| Install deps | `pip install -e .` or `pip install -e .[dev]` |
| Run tests | `pytest` or `python -m pytest` |
| Lint | `ruff check .` or `flake8` |
| Format | `ruff format .` or `black --check .` |
| Build | `python -m build` |
| Publish | `twine upload dist/*` |
| Audit | `pip-audit` |
| SBOM | `syft . -o cyclonedx-json > sbom.json` |

## Smoke test

```sh
pip install dist/*.whl
python -c "import packagename; print('OK')"
```

## Version bumping

Patch: bug fixes, performance improvements, dependency patches
Minor: new features, new public API, deprecations
Major: breaking changes, removed public API, minimum Python version bump

## Package metadata

`pyproject.toml` (or `setup.py`/`setup.cfg` in older projects) contains package metadata: name, version, description, homepage, repository URL, license. When forking or mirroring, update the project URLs to point to the correct repository URL.

## Dangerous patterns

- **`eval` / `exec`**: executes arbitrary Python code from strings. Never use with user input; use `ast.literal_eval` for parsing data literals.
- **`pickle.load` / `pickle.loads`**: deserializes arbitrary Python objects, allowing code execution. Never unpickle untrusted data. Use JSON or `msgpack` for data interchange.
- **`subprocess` with `shell=True`**: passes the command through a shell, so interpolated user input is injectable. Use a list of arguments with `shell=False` (the default).
  ```python
  # Vulnerable
  subprocess.run(f"grep {user_input} /var/log/app.log", shell=True)
  # Safe
  subprocess.run(["grep", user_input, "/var/log/app.log"])
  ```
- **`yaml.load` without Loader**: `yaml.load(data)` can instantiate arbitrary Python objects. Use `yaml.safe_load` or `yaml.load(data, Loader=yaml.SafeLoader)`.
- **Jinja2 server-side template injection**: rendering user-supplied strings as Jinja2 templates allows code execution via `{{ config }}` or `{{ ''.__class__.__mro__ }}`. Never pass user input as the template itself.
- **`tarfile.extractall`**: tar archives can contain entries with `../` paths or absolute paths, writing files outside the target directory. Use `tarfile.data_filter` (Python 3.12+) or validate each member's path before extracting.
- **SQL string formatting**: `cursor.execute(f"SELECT * FROM users WHERE id = {uid}")` is injectable. Use parameterized queries: `cursor.execute("SELECT * FROM users WHERE id = %s", (uid,))`.
- **Insecure defaults**: Django `DEBUG = True` in production exposes tracebacks and settings, `SECRET_KEY` committed in source, `ALLOWED_HOSTS = ['*']` permits host header attacks, Flask `app.run(debug=True)` enables the interactive debugger (which allows RCE), `requests.get(url, verify=False)` disables TLS certificate checking.
