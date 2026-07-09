# Track: parser gaps H1-H4 (bitwise ops, unary ~/+, numeric suffixes, postfix if/unless)

Parent: [../PLAN.md](../PLAN.md). Source: [../PARITY_AUDIT_2026_07.md](../PARITY_AUDIT_2026_07.md)
(Opus 4.8 batch audit) + manual verification on the real binary, same session.
Findings recorded: [../mlc2.md](../mlc2.md) sections H1-H4.

## Status: **closed** (`ae2cb1fb`, steps 1–4: `30897587` … `ae2cb1fb`)

Each step is independent (different lexer/parser code path). **Do them one at
a time, one commit each** — do not combine.

## Verify gate (every step)

```bash
bundle exec rake test_compiler_mlc   # must stay green (447 baseline)
compiler/build.sh
compiler/out/mlcc -o .tmp_selfhost/p1 compiler/main.mlc
compiler/build_bin.sh .tmp_selfhost/p1 .tmp_selfhost/mlcc2
.tmp_selfhost/mlcc2 -o .tmp_selfhost/p2 compiler/main.mlc
diff -rq .tmp_selfhost/p1 .tmp_selfhost/p2   # must be empty
```

Plus a manual repro compile before/after (see each step) — the bug must
reproduce **before** the fix and disappear **after**, on `compiler/out/mlcc`
(the binary rebuilt by `compiler/build.sh` in this same turn — not a stale
`out/mlcc` from before your edit).

| Step | Item | Status |
|------|------|--------|
| 1 | Bitwise operators `&`, `\|`, `^`, `<<`, `>>` (see below) | done (`30897587`) |
| 2 | Unary `~` and `+` (see below) | done (`898b4915`) |
| 3 | Numeric literal suffixes `i8 i16 i32 u16 u32 u64` (see below) | done (`65da5390`) |
| 4 | Postfix `if`/`unless` statement modifier (see below) | done (`ae2cb1fb`) |

---

## STEP=1 — bitwise operators

**Files:** `compiler/frontend/parser/exprs.mlc` (precedence ladder), `compiler/codegen/expr_eval.mlc` (binary op codegen — check it, may already handle these strings once parsing exists).

**Ruby reference (do not touch, read-only):** `lib/mlc/source/parser/expression_parser.rb` — ladder `parse_logical_or → parse_logical_and → parse_bitwise_or (|) → parse_bitwise_xor (^) → parse_bitwise_and (&) → parse_equality → parse_pipe → parse_comparison → parse_shift (<< >>) → parse_addition`.

**Current mlcc ladder** (`compiler/frontend/parser/exprs.mlc`): `parse_or (||) → parse_and (&&) → parse_equality → parse_pipe → parse_comparison_relational → parse_add → parse_mul → parse_unary`. No bitwise/shift levels.

**Action:** insert `parse_bitwise_or`, `parse_bitwise_xor`, `parse_bitwise_and`, `parse_shift` at the same relative position as Ruby (between `parse_and` and `parse_equality` for the bitwise trio; between `parse_comparison_relational` and `parse_add` for shift). Follow the exact structural pattern of an existing level, e.g. `parse_comparison_relational` (loop consuming a fixed set of operator strings via `kind.op_val()`, building `ExprBin`). Lexer already emits `&`, `|`, `^`, `<<`, `>>` as tokens — do not touch the lexer.

**Repro (before fix must fail, after must pass):**

```mlc
fn main() -> i32 = do
  let a = 5 & 3
  let b = 5 | 2
  let c = 5 ^ 1
  let d = 1 << 3
  let e = 16 >> 2
  a + b + c + d + e
end
```

`compiler/out/mlcc /tmp/t1.mlc -o /tmp/t1_out` — before: `E008`/`E005` misparse errors. After: exits 0, generated C++ compiles, runtime result `3 | 7 | 4 | 8 | 4 = 26`.

**Ruby cross-check** (must already pass, do not need to fix Ruby):
```bash
bundle exec ruby -Ilib -e '
require "mlc/common/index"; require "mlc/common/modular_compilation/modular_compiler"
MLC::Common::ModularCompilation::ModularCompiler.new(entry_path: "/tmp/t1.mlc", out_dir: "/tmp/t1_ruby", root_dir: "/tmp", binary_name: "t1").compile
puts "Ruby: OK"'
```

**Add test:** `compiler/tests/test_parser.mlc` — new test asserting `parse_expr_source("5 & 3")` produces `ExprBin("&", ...)` (follow existing `ExprBin` assertion pattern in the same file, e.g. search for `ExprBin` usages already in the file). Also add a codegen-level or e2e test if `compiler/tests/test_codegen.mlc` has a binary-op precedent — check first, mirror it, do not invent a new test style.

---

## STEP=2 — unary `~` and `+`

**Files:** `compiler/frontend/parser/exprs.mlc:507-516` (`parse_unary`), `compiler/codegen/expr_eval.mlc` (unary op codegen).

**Current code** (`compiler/frontend/parser/exprs.mlc`):
```
fn parse_unary(parser: Parser) -> ExprResult = do
  const kind = parser.kind()
  if kind.is_op() && (kind.op_val() == "!" || kind.op_val() == "-") then
    ...
```

**Action:** extend the condition to also match `"~"` and `"+"`. Lexer already emits both as `Op(...)` tokens (`scan_op_single` in `compiler/frontend/lexer.mlc`) — do not touch the lexer. Check `compiler/codegen/expr_eval.mlc` (or wherever `ExprUn` is lowered to C++) actually emits `~`/`+` as-is for these two new cases — C++ has both as valid unary operators on integers, so this should be a direct passthrough, but verify, do not assume.

**Repro:**

```mlc
fn main() -> i32 = do
  let a = ~5
  let b = +3
  a + b
end
```

Before: parse error. After: exits 0, matches Ruby-compiled binary's output.

**Add test:** `compiler/tests/test_parser.mlc`, alongside step 1's new tests — assert `ExprUn("~", ...)` and `ExprUn("+", ...)`.

---

## STEP=3 — numeric literal suffixes

**Files:** `compiler/frontend/lexer.mlc:144` (`try_scan_suffix`, `numeric_type_suffixes` list) and the `token_kind` match after `try_scan_suffix` (around `compiler/frontend/lexer.mlc:187`, matches `'i64' | 'u8' | 'usize' | ...` to `LI64`/`LU8`/etc.).

**Ruby reference (read-only):** `lib/mlc/source/parser/lexer.rb`, `NUMERIC_SUFFIXES = i8 i16 i32 i64 u8 u16 u32 u64 usize f32 f64`.

**Current mlcc:** `numeric_type_suffixes = ['i64', 'u8', 'usize', 'f64', 'f32']` — missing `i8`, `i16`, `i32`, `u16`, `u32`, `u64`.

**Action:**
1. Add the 6 missing strings to `numeric_type_suffixes`.
2. Find the corresponding token-kind types — check `compiler/frontend/ast.mlc` (or wherever `LI64`/`LU8` etc. are defined as token/literal variants) for whether `LI8`/`LI16`/`LI32`/`LU16`/`LU32`/`LU64` **already exist** (likely yes, since the checker/codegen almost certainly already handles the full `i8..u64` type family elsewhere — grep before assuming they need to be created). If they exist, wire them into the `token_kind` match the same way `LI64`/`LU8` are wired. If they do not exist as literal-token variants, this step is bigger than it looks — stop and flag it in this track's table rather than inventing new AST variants ad hoc.

**Repro:**

```mlc
fn main() -> i32 = do
  let a: i32 = 5i32
  let b: u32 = 10u32
  a
end
```

Before: `E001: undefined: i32` (suffix not recognized, falls back to bare int + separate identifier). After: exits 0.

**Add test:** `compiler/tests/test_parser.mlc` (lexer/token-level, if there's a suffix-focused test already — mirror it) or `compiler/tests/test_lexer.mlc` if that file exists — check first.

---

## STEP=4 — postfix `if`/`unless` statement modifier

**Files:** `compiler/frontend/parser/exprs.mlc` (`parse_if_expr`, from line 854 — prefix-only today).

**Ruby reference (read-only):** `lib/mlc/source/parser/expression_parser.rb`, `parse_postfix_conditional` — wraps an already-parsed expression in `if cond then expr else ()` (or negated for `unless`) **only if `if`/`unless` appears on the same source line as the expression**.

**Current mlcc:** `if`/`unless` only parsed as a **prefix** form inside `parse_primary`; there is no postfix wrapping stage anywhere in the expression parser.

**Action:** this is the riskiest of the 4 — it changes how a whole statement/expression is parsed, not just adding an operator level. Find where `parse_primary`'s result flows back up (likely `parse_postfix` or one level above `parse_unary`) and add a same-line check for a trailing `if`/`unless` token, wrapping the already-built expression the same way Ruby's `parse_postfix_conditional` does. **Do this step last**, after 1-3 are done and verified — it's the one most likely to need a second pass.

**Repro (mlcc currently corrupts parsing of subsequent declarations — not just a clean parse error):**

```mlc
fn helper() -> unit = do
  println("hi") if true
end

fn main() -> i32 = do
  helper()
  0
end
```

Before: cascading nonsense errors (`undefined: main`, etc.) — proof the parser desyncs, not just rejects the construct cleanly. After: exits 0.

**Add test:** `compiler/tests/test_parser.mlc` — assert the postfix form parses to the same AST shape Ruby produces (an `ExprIf` wrapping the left-hand expression as the `then` branch, implicit unit `else`). If genuinely too large for one step once you're in the code, split into STEP=4a (postfix `if`) / STEP=4b (postfix `unless`) and update this table — do not silently do half the work.

---

## Per-turn template

```
| step | <1-4> |
| done | <one line> |
| verify | test_compiler_mlc N/0; diff_exit=0; repro before=fail after=pass |
| next | STEP=<n+1> |
```
