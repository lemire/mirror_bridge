# Maintainers

This document lists the maintainers of Mirror Bridge and describes the contribution process.

## Current Maintainers

| Name | GitHub | Role |
|------|--------|------|
| Francisco Thiesen | [@FranciscoThiesen](https://github.com/FranciscoThiesen) | Lead Maintainer |

## Becoming a Maintainer

Active contributors who demonstrate:
- Consistent, high-quality contributions
- Understanding of the C++26 reflection ecosystem
- Responsiveness in code reviews
- Alignment with project goals

...may be invited to become maintainers.

## Contribution Process

### For External Contributors

1. **Fork** the repository
2. **Create a branch** for your changes
3. **Submit a Pull Request** against `main`
4. **Wait for review** from at least one maintainer
5. **Address feedback** if requested
6. **Merge** once approved

### Branch Protection Rules

The `main` branch is protected with the following rules:

- ✅ **Require pull request reviews** - At least 1 maintainer approval required
- ✅ **Require status checks to pass** - All CI tests must pass (Python, Lua, JavaScript)
- ✅ **Require branches to be up to date** - PRs must be rebased on latest main
- ❌ **No direct pushes** - All changes must go through PRs (except maintainers for emergencies)

### CI Tests Required

All PRs must pass these checks before merging:

| Test | Description |
|------|-------------|
| `test-python` | Python bindings including static methods, const refs |
| `test-lua` | Lua bindings with same feature coverage |
| `test-javascript` | JavaScript/Node.js bindings |

## Code of Conduct

We follow the [Contributor Covenant](https://www.contributor-covenant.org/). Be respectful, constructive, and welcoming to newcomers.

## Security

If you discover a security vulnerability, please email the maintainers directly rather than opening a public issue.
