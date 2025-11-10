# Guild Coding Standard - Charter
Version: 1.1.2
Author: Guild of Dense Coders
Date: 2025-09-26

## Rule classes
MUST: p1-p4; r1, r2, r3, r4, r5, r6, r7, r8, r11, r12, r14, r15, r17; g1, g2, g3, g6, g8, g10; a1-a12; e1, e2; d1-d3; t1-t2; c1-c2; m1, m2
SHOULD: r10, r13, r15; g5, g7, g9; e3, e4; t3; m3-m4; c3-c4

## 1) Environment
e1. Primary monitor shows >=150 visible columns at default zoom. [MUST]
e2. Keep code <=150 columns unless splitting is impractical; hard cap 180. [MUST]
e3. Use portrait side monitors for tall diffs/logs. [SHOULD]
e4. Editors and code hosts enable soft-wrap for review; a formatter runs on save and at pre-commit to restore lines to <=180. [SHOULD]

## 2) GPU (graphics and shader code)
g1. Vulkan is primary. DX12 allowed only when a Vulkan path exists. GL/D3D11 are legacy. [MUST]
g2. Target SPIR-V (Vulkan) and DXIL (DX12); e.g., HLSL via DXC. [MUST]
g3. No Geometry Shader in new work. Prefer mesh/task where supported; else VS/TS/FS or compute by profile. [MUST]
g5. Prefer bindless/descriptor indexing; immutable samplers; push constants <=128B; avoid per-draw rebinding. [SHOULD]
g6. Use explicit barriers; document hazards; no global catch-all barriers in hot paths. [MUST]
g7. Tune workgroup size to subgroup/occupancy; use specialization constants. [SHOULD]
g8. Include captures (RenderDoc/RGP/Nsight) and show >=3% win or equal performance with simplification. [MUST]
g9. Prefer device-local memory; staged uploads; batch small updates; obey alignment. [SHOULD]
g10. Feature checks gate optional paths; clear fallbacks; no silent extension reliance. [MUST]

## 3) Performance
p1. Inline in headers only if hot by profile and ODR/size safe; else .cpp. [MUST]
p2. Use explicit, alignment-aware allocators (malloc32, zalloc64) with matching frees. [MUST]
p3. Prefer SIMD wherever faster; keep scalar baseline; compile-time specializations + run-time CPUID dispatch; expose thread status via atomics; document memory order. [MUST]
p4. Performance over idiom: choose C vs C++ by benchmark on target builds; keep microbenches in bench/. [MUST]

## 4) Bench discipline
bd1. Provide a benchmark diff and build config with any performance claim. [POLICY]
bd2. Accept >=3% win on target SKU or equal performance with meaningful simplification. [POLICY]

## 5) Typedefs and vectors
t1. Vector naming: ui256=__m256i, fl32x8=__m256, fl64x4=__m256d, ui512=__m512i, fl32x16=__m512, fl64x8=__m512d. [MUST]
t2. Constness mnemonic: leading c binds the pointee; trailing c binds the pointer; repeat per indirection. [MUST]
t3. Do not mix raw const T* style with alias forms in the same TU. [SHOULD]
## 6) Pointer-array macros (approved)
m1. Declarations (spec): defpa/defpa2/defp1a1. [MUST]
m2. Cast helpers (spec): refpa/refpa2/refp1a1. [MUST]
m3. Parameters must be identifiers or parenthesized expressions. [SHOULD]
m4. Provide >=3 usage examples in docs/tests. [SHOULD]
## 7) Readability
r1. Types encode width/sign: ui8, ui16, ui32, ui64, si8, si16, si32, si64, fl32, fl64. [MUST]
r2. Put const/volatile and indirection in typedefs, not identifiers; use exact aliases (cui32, ui32ptr, cui32ptr, ui32ptrc, cui32ptrc, vui512, cfl32x4, vfl32x8, ...). [MUST]
r3. Spreadsheet-style padding when it improves readability. Apply within the local context that benefits. [MUST]
r4. Same-line statements are allowed only when r3 justifies readability; when used, separate statements with exactly three spaces. [MUST]
r5. Comments: /// for API docs with tags (@param, @return, @tparam, @note); // for notes; grouping headers //==, //--. [MUST]
r6. Disable code with /* ... */ if >5 lines, else //. [MUST]
r7. Line width hard cap 180 (mirrors e2). [MUST]
r8. Indent 3 spaces. No tabs. [MUST]
r10. Tool-align EOL comments; if alignment would breach 180, move comment above. Manual alignment discouraged. [SHOULD]
r11. Functions use PascalCase. [MUST]
r12. Tables/macros and global constants use UPPER_SNAKE. [MUST]
r13. Control structures: no spaces before (, exactly one space after each ;. Auto-fix preferred. [SHOULD]
r14. Braces: { on same line as control; } on its own line. [MUST]
r15. Place the opening brace on the same line as the function signature. Put exactly one space before the brace. Do not place the brace on its own line. [MUST]
r16. Setters small, orthogonal, uniform signatures; no side effects beyond the named field. [SHOULD]
r17. File prolog - block-comment spec (visual). [MUST]
Scope. File header only. Not API docs.
Format. Delimiters on their own lines: /* ... */. Inner lines start with exactly ' * '; ASCII only; no tabs. Wrap at 150 (W=150). If an unbreakable token must exceed, allow <=180 and append ' // WIDTH-EXEMPT'. No 'History:'; use CHANGELOG.md.
Required fields (lettered).
A. File: <filename.ext>
B. Version: vMAJOR.MINOR[.PATCH]
C. Owner: <person or role>
D. Created: YYYY-MM-DD
E. Last Modified: YYYY-MM-DD (only field tools may auto-update)
F. Description: one concise line only; details in docs/.
G. To Do: numbered items; one per line: 1) ..., 2) ..., 3) ... (continuations align under the value, in order of importance).
H. Dependencies: None or comma-separated list
I. ISA: tokens from {Scalar, SSE4.2, AVX2, AVX-512} separated by |
J. Thread-safety: {N/A, Reentrant, MT-safe}
K. Reviewers: group or names
L. License: <SPDX>  Copyright: <holder>
Validation (minimal). Open ^\s*/\*\s*$; close ^\s*\*/\s*$; inner line ^ \* .{0,146}$; dates ^ \* (Created|Last Modified): \d{4}-\d{2}-\d{2}$; version ^ \* Version: v\d+\.\d+(\.\d+)?$; license ^ \* License: [A-Za-z0-9.+-]+  Copyright: .+$; ISA tokens in allowed set with ' | ' separators; no 'History:' label.
Template:
/*
 * File: [ExactFilename.ext]
 * Version: [vX.Y[.Z]]
 * Owner: [Name or role]
 * Created: YYYY-MM-DD
 * Last Modified: YYYY-MM-DD
 * Description: [Concise one-line summary only]
 * To Do: 1) [Highest-impact task]
 *        2) [Next task]
 *        3) [Next task]
 * Dependencies: [None|libA, libB, ModuleX]
 * ISA: [Scalar|SSE4.2|AVX2|AVX-512]
 * Thread-safety: [N/A|Reentrant|MT-safe]
 * Reviewers: [Team or names]
 * License: [SPDX]
 * Copyright: [Holder]
 */

## 8) Docs
d1. /// only for API docs; public APIs require it with tags. [MUST]
d2. Header guides must be short; duplicate full guides in docs/. [MUST]
d3. If not concise, point to the module's docs/ guide instead of inlining. [MUST]

## 9) Tooling configs
tc1. .clang-format: IndentWidth 3, UseTab Never, ColumnLimit 180, BreakBeforeBraces Attach, align decls/assigns and trailing comments. [SPEC]
tc2. .editorconfig: UTF-8, CRLF, indent_size=3, max_line_length=180. [SPEC]

## 10) Enforcement
en1. Pre-commit: clang-format; typedef/macro lint; r13 spacing fix; r10 EOL aligner; codespell; r16 validator; GPU gates (g3/g8/g10); archiving lints (a3/a4/a6 tags, a7 parity+perf). [SPEC]
en2. CI: formatter check; r16 conformance; doc-tag checker; ban new f32/f64; no mixing alias vs raw const per TU; GPU captures present and thresholds met; archiving gates (a5/a7/a9). [SPEC]
en3. Review norm: no spacing/alignment nitpicks; bots fix or fail. Use // RULE-DEV:<rule-id> <why> for intentional deviations. [SPEC]

## 11) Changelog policy
c1. No history in prologs. [MUST]
c2. Root CHANGELOG.md for repo; optional per-module logs for large subsystems. [MUST]
c3. Format: [Unreleased] + version sections grouped by Added/Changed/Fixed/Removed/Perf with PR/issue links. [SHOULD]
c4. Tag releases and generate notes from the changelog. [SHOULD]

## 12) Archiving
a1. Statuses: Active (ships), Oracle (single slow reference for tests only). [MUST]
a2. Tech cut-off: CPU baseline x86-64 AVX2+FMA3+BMI2 and GPU baseline Shader Model 5.0. Older -> Museum. 32-bit, SSE-only, single-threaded variants unsupported. [MUST]
a3. Build guards: compile with /arch:AVX2 (or equivalent) and static_assert(__AVX2__). Tests may define ARMADA_ALLOW_RELICS. [MUST]
a4. Ban legacy macros USE_OLD_CODE and USE_ANCIENT_CODE in production; enforcement header errors unless ARMADA_ALLOW_RELICS. [MUST]
a5. Oracle isolation: one oracle per algorithm in src/relics/<subsystem>/; tests-only; interface frozen. [MUST]
a6. Museum policy: pre-AVX2 SIMD, single-core designs, legacy allocators, obsolete graphics paths live only in tagged snapshots; not linked. Tag relic/<subsystem>/<name>/<YYYYMMDD>. [MUST]
a7. CI gates: modern path matches oracle within tolerances on deterministic fixtures and meets a perf floor vs previous AVX2 build. [MUST]
a8. Fallbacks: run-time dispatch only one step above baseline (e.g., AVX-512 microkernels) or for a supported non-baseline target. No compile-time forks. [MUST]
a9. Removal: delete if unused >=90 days, or modern is >=10% faster with parity, or relic bug density exceeds modern for 2 consecutive months; track owners/dates. [MUST]
a10. Lore capture: each deletion gets a 1-page Relic Note (origin, design, bugs, retirement reason, replacement, perf deltas, key commits). [MUST]
a11. Guardrails: no new behavior #ifdefs; use traits/strategy or separate TUs chosen at link/dispatch time. [MUST]
a12. Exceptions: paying non-AVX2 targets are separate ports; no backflow of relic code to mainline. [MUST]
