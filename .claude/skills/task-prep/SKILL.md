---
name: task-prep
description: Research a ticket spec and produce a structured task file at `.claude/tasks/<ID>.md` by spawning the Plan agent. Use when given a non-trivial ticket spec (3+ files, multi-domain, references "analogous" implementations, or ambiguous scope) or when explicitly invoked.
argument-hint: "[ticket-id]"
user-invocable: true
---

# Task Prep

Turn a raw ticket spec into a structured, durable task file by spawning the `Plan` agent to research the codebase, decompose the work, and surface open questions before any implementation starts.

## When to invoke
- User explicitly runs `/task-prep` — always honor it regardless of task size
- A request contains a ticket ID (`D0\d+`) and the spec is **non-trivial**: 3+ files, multiple domains, references an analogue to mimic, or contains ambiguities
- **Skip** for trivial single-file work — for those, just create a short skeleton task file yourself (see CLAUDE.md → Task Files)

## Inputs
- **Ticket ID**: extract `\d+` from the spec or from the current branch (e.g. `feature/020636-xyz` → `020636`). If none found, ask the user.
- **Spec**: use the verbatim spec from the user's most recent message (or from `$ARGUMENTS` if pasted there). Do not paraphrase or summarize before passing to the Plan agent.

## Workflow

1. **Resolve the ticket ID** (`\d+`). If absent in both the spec and branch, ask the user.

2. **Pre-flight: working tree + branch** — verify the repo is in a sensible state to start a new task. Skip these checks only if the user explicitly says so.
   - Run `git status --porcelain` via Bash. If output is non-empty, list the changed/untracked files and use `AskUserQuestion` to ask: **commit first** (recommended if changes belong to the previous task), **stash**, **abort**, or **proceed anyway** (only if changes are intentionally part of this new task).
   - Run `git rev-parse --abbrev-ref HEAD`. If the branch name does not contain the resolved ticket ID, use `AskUserQuestion` to ask: **create and switch to `feature/<ID>-<slug>`** (recommended), **proceed on current branch**, or **abort**. When creating, ask the user for the `<slug>` (kebab-case short description).
   - If either check surfaces an unexpected file or branch (e.g. leftover work from someone else, uncommitted migrations, in-progress changes you didn't make), stop and surface to the user before doing anything else — do not destructively resolve it.

3. **Check for an existing file** at `.claude/tasks/<ID>.md`. If it exists with unchecked boxes, ask whether to (a) refresh from scratch, (b) merge new findings into it, or (c) abort.

4. **Spawn the `Plan` agent** (`subagent_type: Plan`, `model: sonnet`) with a prompt of this shape:

   ```
   Spec (verbatim, as supplied by the user — do not paraphrase):
   <full spec here>

   Branch: <current branch>
   Project: Snake3D (CLAUDE.md is loaded; conventions live there).

   Research the Snake3D codebase and produce a structured task plan. Specifically:

   Return your output in this exact markdown structure (no preamble, no closing
   remarks, no surrounding code fences):

   # <TICKET_ID> — <short title from spec>

   ## Goal
   <one-line outcome>

   ## Context
   <scope, constraints, reference implementations with concrete file paths,
   related tickets, why this matters>

   ## Plan
   - [ ] Step 1 (with concrete file paths where known)
   - [ ] Step 2
   - [ ] ...

   ## Decisions log
   <empty — to be filled during implementation>

   ## Files touched
   <empty — to be filled during implementation>

   ## Open questions
   - <question 1>
   - <question 2>
   ```

5. **Save** the agent's output verbatim to `.claude/tasks/<TICKET_ID>.md` using the Write tool (overwrite or merge per step 3). The file is gitignored.

6. **Present the Plan to the user**: paste back the **Goal** and **Plan** (checkboxes) sections so they see the scope.

7. **Resolve open questions interactively** using `AskUserQuestion`:
   - For each entry in the **Open questions** section, invoke `AskUserQuestion`. Batch up to 4 questions per call (the tool's max).
   - When the Plan agent's recommendation is suitable for a multi-choice question, list it first and suffix the label with "(Recommended)". Add 1–2 alternative options. Include a brief description on each option explaining the trade-off.
   - When the question is genuinely free-form (e.g., "which field name?"), present a couple of likely options plus "Other" — the harness always adds the Other choice automatically.
   - After all answers are collected, **edit the task file** to append the resolved decisions to the **Decisions log** section (one bullet per resolved question, dated, with the chosen answer and any user-provided rationale).

8. **Ask for final approval** before implementation begins.

9. **Do not implement** anything in this skill. Hand off the approved plan to the appropriate `snake3D-*` execution agent per CLAUDE.md → Task Routing.

## Notes
- The task file is gitignored (`.claude/tasks/`) and persists across `/clear`, new sessions, and conversation compaction via the SessionStart hook.
- During implementation, tick `- [x]` immediately as steps complete, append to **Decisions log** for non-trivial choices, and append to **Files touched** for each file edited/created.
- If the Plan agent's output is missing one of the required sections, add the missing heading with an empty body before saving — the SessionStart hook expects the full structure.
