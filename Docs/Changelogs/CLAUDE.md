# Changelogs & MR descriptions

Rules for `Docs/Changelogs/<year>/Changelog.md` and the matching GitLab merge
request description. The full authoring guide is the `changelog` skill; this
file is the short version plus the split between the two documents.

## Changelog.md — for the reader, net effect only

The changelog is a **release document read by the user** ("what changed in the
app I run?") and the developer ("what changed in the code I build on?"). Every
line must serve one of them.

- **Net effect only.** An MR is squashed into its end state. If a branch went
  A → B → C, document **C** only. Never mention intermediate state B — a class
  that was renamed before merge, a refactor a later commit replaced, or a bug
  that was introduced *and* fixed on the same branch. For the reader it never
  existed.
  - Exception: a bug that was **released to users** always goes in `#### Fixed`,
    because the user actually experienced it.
- Lead with the effect, then the mechanism: *what changed for me*, then *how*.
- Sections in order, only the ones with content: `#### New`, `#### Fixed`,
  `#### Removed`, `#### Changed`. One `##` block per MR, newest at the bottom.
- Link the commit(s): `([\`abc1234\`](.../commit/abc1234))`. Several hashes on
  one bullet beats duplicate bullets.
- Leave out: formatting/comment-only commits, pure internal renames, merge/
  revert mechanics, editor config, the changelog commit itself, and per-commit
  narration ("then we also tried…").

## MR description — summary, then the highlights

The MR description is **not** a copy of the changelog. The detailed, per-change
version lives in `Changelog.md`; the MR description is the overview.

- Open with a short **overall summary** paragraph: what this MR is about as a
  whole and why.
- Follow with a **bullet list of the biggest points** only — the handful of
  changes that define the MR, not every entry.
- Link to the changelog section for the full detail rather than repeating it.

## Before merging

Diff the changelog section against the real MR range and account for every
commit (covered / deliberately omitted / missing):

```bash
git log --oneline --no-merges $(git merge-base HEAD origin/master)..HEAD
```

Watch especially for commits made **after** the changelog commit — the
changelog is almost never the last commit on a branch.
