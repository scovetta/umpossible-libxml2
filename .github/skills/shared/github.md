# GitHub Platform Reference

This skill provides GitHub-specific platform knowledge for all agents.

## GitHub CLI (`gh`)

The primary interface for interacting with GitHub from within workflows.

### Issue Operations

```bash
# List issues
gh issue list --state open --label "security" --json number,title,labels,createdAt

# Create an issue
gh issue create --title "[security] Finding title" --body "Description" --label "security,automation"

# Close an issue
gh issue close 42 --reason completed

# Add a comment
gh issue comment 42 --body "Analysis update..."

# Add labels
gh issue edit 42 --add-label "priority-high,security"

# Search issues
gh issue list --search "label:security created:>2024-01-01" --json number,title,state
```

### Pull Request Operations

```bash
# List PRs
gh pr list --state open --json number,title,labels,headRefName

# Create a PR
gh pr create --title "[test-improvement] Add edge case tests" --body "Description" --label "testing,automation" --draft

# Review a PR
gh pr review 42 --comment --body "Review feedback"

# Check PR diff
gh pr diff 42

# Get PR details
gh pr view 42 --json title,body,files,additions,deletions,labels
```

### Repository Operations

```bash
# Get repo info
gh repo view --json name,description,defaultBranchRef,languages

# List recent activity
gh api repos/{owner}/{repo}/activity --jq '.[0:10]'

# Search code
gh api search/code -X GET -f q="keyword+repo:{owner}/{repo}" --jq '.items[].path'
```

### Workflow Operations

```bash
# Dispatch a workflow
gh workflow run workflow-name.yml -f input_key=value

# List recent runs
gh run list --workflow=workflow-name.yml --json status,conclusion,createdAt -L 5

# Get run details
gh run view 12345 --json status,conclusion,jobs
```

## GitHub API (REST)

Use `gh api` for operations not covered by high-level commands:

```bash
# GET request
gh api repos/{owner}/{repo}/contents/path/to/file --jq '.content' | base64 -d

# POST request
gh api repos/{owner}/{repo}/issues -f title="Title" -f body="Body" -f labels[]="label1"

# GraphQL
gh api graphql -f query='{ repository(owner: "{owner}", name: "{repo}") { issues(first: 10) { nodes { title number } } } }'
```

## Safe Outputs vs Direct API

In GitHub Agentic Workflows, prefer **safe-outputs** over direct API calls:

| Action | Safe Output | Direct API |
|--------|------------|------------|
| Create issue | `create-issue` tool | `gh issue create` |
| Add comment | `add-comment` tool | `gh issue comment` |
| Create PR | `create-pull-request` tool | `gh pr create` |
| Add labels | `add-labels` tool | `gh issue edit --add-label` |

Safe outputs are:
- Rate-limited and bounded (max counts in frontmatter)
- Auditable (logged by the workflow system)
- Safer (prevent runaway automation)

Use direct `gh` CLI only for **read operations** (listing, viewing, searching).

## Authentication

- `gh` CLI is pre-authenticated in GitHub Actions via `GITHUB_TOKEN`
- No additional auth setup needed for same-repo operations
- Cross-repo operations require explicit token permissions

## Rate Limits

- GitHub API: 5000 requests/hour for authenticated requests
- Search API: 30 requests/minute
- Code Scanning API: separate limits
- Be conservative — batch queries where possible

## Labels Convention

Agent-created issues use title prefixes and labels:

| Agent | Title Prefix | Labels |
|-------|-------------|--------|
| Securibot | `[security]` | `security`, `automation` |
| Testbot | `[testing]` | `testing`, `automation` |
| Buildbot | `[build]` | `build`, `automation` |
| Depbot | `[dependencies]` | `dependencies`, `automation` |
| Docbot | `[documentation]` | `documentation`, `automation` |
| Perfbot | `[performance]` | `performance`, `automation` |
| Licensebot | `[license]` | `license`, `automation` |
| Releasebot | `[release]` | `release`, `automation` |
| PM Orchestrator | `[pm-daily]` | `automation`, `daily-report` |
| Fixer Agent | N/A (assigns Copilot) | `copilot-fix` |

Priority labels: `priority-high`, `priority-medium`, `priority-low`
