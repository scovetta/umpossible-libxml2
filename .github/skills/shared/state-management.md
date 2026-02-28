# State Management

How agents persist state across workflow runs using cache memory.

## Cache Memory

GitHub Agentic Workflows provide a persistent cache at `/tmp/gh-aw/cache-memory/`. Each agent maintains its own state file.

### State File Convention

```
/tmp/gh-aw/cache-memory/
├── securibot-state.json
├── testbot-state.json
├── buildbot-state.json
├── depbot-state.json
├── docbot-state.json
├── perfbot-state.json
├── licensebot-state.json
├── releasebot-state.json
├── fixer-state.json
├── pm-state.json
├── report-state.json
└── responder-state.json
```

### State File Format

Every state file follows this base structure:

```json
{
  "agent": "agent-name",
  "last_run": "2024-01-15T06:00:00Z",
  "run_count": 42,
  "cycle": "2024-01-15",
  "status": "completed",
  "findings": {
    "total": 5,
    "new": 2,
    "resolved": 1,
    "carried_over": 2
  },
  "actions_taken": [
    {"type": "create-issue", "number": 123, "title": "[security] Buffer overflow in parse_header"},
    {"type": "add-labels", "issue": 100, "labels": ["priority-high"]}
  ],
  "notes": "Free-form observations for institutional memory"
}
```

### Reading State

```bash
# Check if state file exists
if [ -f /tmp/gh-aw/cache-memory/securibot-state.json ]; then
  cat /tmp/gh-aw/cache-memory/securibot-state.json | jq .
fi
```

### Writing State

```bash
cat > /tmp/gh-aw/cache-memory/securibot-state.json << 'EOF'
{
  "agent": "securibot",
  "last_run": "2024-01-15T06:05:00Z",
  "run_count": 43,
  "cycle": "2024-01-15",
  "status": "completed",
  "findings": {
    "total": 3,
    "new": 1,
    "resolved": 2,
    "carried_over": 0
  },
  "actions_taken": [],
  "notes": "All previously flagged issues have been resolved via PRs"
}
EOF
```

## State Management Rules

1. **Always read before write** — Load existing state before updating to preserve history.
2. **Increment, don't reset** — `run_count` always increases. Don't lose history.
3. **Track findings delta** — Note what's new vs what carried over from last run.
4. **Record all actions** — Every issue/PR/comment created should be logged in state.
5. **Update atomically** — Write the complete state file, don't append.

## Cross-Agent Communication

Agents communicate through:

1. **Labels on issues** — An agent can check what other agents have flagged
2. **State files** — The daily-report agent reads all state files to compile reports
3. **Issue title prefixes** — `[security]`, `[testing]`, etc. allow filtering

### Reading Other Agents' State

```bash
# PM Orchestrator reads all state files to assess cycle progress
for f in /tmp/gh-aw/cache-memory/*-state.json; do
  echo "=== $(basename $f) ==="
  jq '{agent, last_run, status, findings}' "$f"
done
```

## Noop Protocol

When an agent determines there is nothing to do, it MUST:

1. **Update its state file** with `"status": "noop"` and a reason
2. **Call the noop tool** with a descriptive message

```json
{
  "agent": "securibot",
  "last_run": "2024-01-15T06:05:00Z",
  "status": "noop",
  "noop_reason": "No code changes since last scan, no new CVEs in dependencies",
  "findings": {
    "total": 0,
    "new": 0,
    "resolved": 0,
    "carried_over": 0
  }
}
```

This is critical for the daily-report agent and the orchestrator to distinguish "agent ran and found nothing" from "agent didn't run."
