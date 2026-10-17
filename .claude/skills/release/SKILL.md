---
name: release
description: Bump this project's firmware version (FW_VERSION in include/config.h) using Semantic Versioning and add a matching entry to CHANGELOG.md in Keep a Changelog format. Use this whenever the user asks to "bump the version", "cut a release", "release a new version", "update the changelog", or wants to ship a finished set of changes as a new version — including invocations like "/release", "/release minor", or "/release major". Also consider using it proactively once a meaningful chunk of work is complete and the user seems ready to wrap up, by asking whether they want to cut a release before stopping.
---

# Release

This repo has no `package.json` — the version of record lives in two places that must
always move together:

- `include/config.h` — `constexpr char FW_VERSION[] = "x.y.z";`
- `CHANGELOG.md` — a `## [x.y.z] - YYYY-MM-DD` entry at the top, Keep a Changelog style

If a `package.json`, `Cargo.toml`, `pyproject.toml`, or similar manifest is ever added to
this repo, treat its `version` field the same way as `FW_VERSION` below — read it, bump
it, write it back — in addition to the changelog entry.

## 1. Read the current version

Read `include/config.h` and find the `FW_VERSION` line. That's the current version,
`MAJOR.MINOR.PATCH`.

## 2. Figure out what changed since the last release

The top entry in `CHANGELOG.md` marks the last release. Look at what's changed since then:

- `git log` and `git diff` for commits/work since the commit that introduced that changelog
  entry (its message usually mirrors the entry, e.g. "Always show red when the internet is
  down (v1.1.1)")
- Any currently uncommitted changes (`git status` / `git diff`)

Summarize the changes into Keep a Changelog sections as applicable: `### Added`,
`### Changed`, `### Fixed`, `### Removed`. Write bullets for a changelog reader (what
changed and why it matters), not implementation detail — match the tone of the existing
entries in CHANGELOG.md.

## 3. Pick the bump type

If the user gave one explicitly (e.g. "bump minor", "/release major"), use it.
Otherwise infer it from the summarized changes, using standard SemVer rules:

- **major** — breaking changes: anything that would make an existing user's build stop
  working or behave incompatibly without changes on their end (e.g. renamed/removed config
  options in `include/config.h`, changed wiring/pin assumptions, removed features)
- **minor** — new, backwards-compatible functionality (new features, new config options
  with sensible defaults)
- **patch** — bug fixes, behavior corrections, docs/cleanup with no functional change to
  the public interface

When genuinely ambiguous, default to the smaller bump (patch over minor, minor over major)
and say why you chose it.

Compute the new version: bumping MAJOR resets MINOR and PATCH to 0; bumping MINOR resets
PATCH to 0; bumping PATCH only increments PATCH.

## 4. Apply the bump

1. Update `FW_VERSION` in `include/config.h` to the new version.
2. Insert a new entry at the top of `CHANGELOG.md`, directly under the header/intro lines
   and above the previous top entry:

   ```
   ## [x.y.z] - YYYY-MM-DD

   ### Added
   - ...

   ### Fixed
   - ...
   ```

   Use today's actual date. Only include the `###` sections that have content.

## 5. Report, don't auto-commit

Show the user the new version number, the changelog entry you wrote, and a one-line
rationale for the bump type chosen. This repo's commit messages reference the version
in parentheses (e.g. `Add ISP internet reachability check (v1.1.0)`) — suggest a commit
message in that style, but don't create the commit yourself unless the user asks for it.
