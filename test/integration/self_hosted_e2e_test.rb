# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

# Self-hosted compiler readiness tests.
# Each test exercises a pattern that would appear in a real MLC→MLC compiler.
class SelfHostedE2ETest < Minitest::Test
  tag :slow

  CLI = File.expand_path("../../bin/mlc", __dir__)

  def run_mlc(source_code)
    Dir.mktmpdir do |dir|
      source = File.join(dir, "test.mlc")
      File.write(source, source_code)
      stdout, stderr, status = Open3.capture3(CLI, source)
      compile_errors = stderr.lines.select { |l| l.include?("error:") }
      refute compile_errors.any?, "Compilation failed:\n#{compile_errors.join}"
      yield stdout, stderr, status if block_given?
    end
  end

  # ─── Lexer patterns ────────────────────────────────────────────────────

  # Classify characters for a lexer (digit / letter / other)
  def test_char_classification
    run_mlc(<<~MLC) do |_out, _err, status|
      fn is_digit(c: string) -> bool = c >= "0" && c <= "9"
      fn is_alpha(c: string) -> bool = c >= "a" && c <= "z"

      fn classify(c: string) -> i32 =
        if is_digit(c) then 1
        else if is_alpha(c) then 2
        else 0
        end

      fn main() -> i32 = classify("5") * 10 + classify("x") * 2 + classify("!")
    MLC
      assert_equal 14, status.exitstatus # 1*10 + 2*2 + 0
    end
  end

  # Tokenize a source string into digit/letter/space counts
  def test_lexer_character_scan
    run_mlc(<<~MLC) do |_out, _err, status|
      fn is_digit(c: string) -> bool = c >= "0" && c <= "9"
      fn is_alpha(c: string) -> bool = c >= "a" && c <= "z"

      fn scan(src: string) -> i32 = do
        let digits = 0
        let letters = 0
        let i = 0
        while i < src.length() do
          let c = src.char_at(i)
          if is_digit(c) then
            digits = digits + 1
          else if is_alpha(c) then
            letters = letters + 1
          end
          i = i + 1
        end
        digits * 10 + letters
      end

      fn main() -> i32 = scan("ab1c2d3")
    MLC
      assert_equal 34, status.exitstatus # 3 digits * 10 + 4 letters
    end
  end

  # Build a token string by accumulating characters
  def test_string_accumulation
    run_mlc(<<~MLC) do |_out, _err, status|
      fn is_digit(c: string) -> bool = c >= "0" && c <= "9"

      fn extract_number(src: string, start: i32) -> string = do
        let token = ""
        let i = start
        while i < src.length() do
          let c = src.char_at(i)
          if is_digit(c) then
            token = token + c
          end
          i = i + 1
        end
        token
      end

      fn main() -> i32 = extract_number("abc123def", 3).length()
    MLC
      assert_equal 3, status.exitstatus # "123" has 3 chars
    end
  end

  # ─── ADT / Token types ─────────────────────────────────────────────────

  # ADT for token types with data
  def test_adt_token_array
    run_mlc(<<~MLC) do |_out, _err, status|
      type Token = TInt(i32) | TIdent(string) | TPlus | TMinus | TEof

      fn token_kind(t: Token) -> i32 =
        match t
          TInt(_) => 1
          TIdent(_) => 2
          TPlus => 3
          TMinus => 4
          TEof => 0
        end

      fn main() -> i32 = do
        let tokens = [TInt(42), TPlus, TIdent("x"), TMinus, TEof]
        let kinds = 0
        let i = 0
        while i < tokens.length() do
          kinds = kinds + token_kind(tokens.get(i))
          i = i + 1
        end
        kinds
      end
    MLC
      assert_equal 10, status.exitstatus # 1+3+2+4+0
    end
  end

  # Extract integer value from token
  def test_adt_token_value_extraction
    run_mlc(<<~MLC) do |_out, _err, status|
      type Token = TInt(i32) | TPlus | TEof

      fn get_int_value(t: Token) -> i32 =
        match t
          TInt(v) => v
          _ => 0
        end

      fn main() -> i32 = do
        let arr = [TInt(10), TInt(20), TPlus, TInt(30)]
        let sum = 0
        let i = 0
        while i < arr.length() do
          sum = sum + get_int_value(arr.get(i))
          i = i + 1
        end
        sum
      end
    MLC
      assert_equal 60, status.exitstatus
    end
  end

  # ─── Symbol table / Environment ────────────────────────────────────────

  def test_symbol_table_basic
    run_mlc(<<~MLC) do |_out, _err, status|
      fn main() -> i32 = do
        let env: Map<string, i32> = Map.new()
        env.set("x", 10)
        env.set("y", 20)
        env.set("z", 30)
        env.get("x") + env.get("z")
      end
    MLC
      assert_equal 40, status.exitstatus
    end
  end

  def test_symbol_table_overwrite
    run_mlc(<<~MLC) do |_out, _err, status|
      fn main() -> i32 = do
        let env: Map<string, i32> = Map.new()
        env.set("x", 10)
        env.set("x", 42)
        env.get("x")
      end
    MLC
      assert_equal 42, status.exitstatus
    end
  end

  def test_symbol_table_has_check
    run_mlc(<<~MLC) do |_out, _err, status|
      fn lookup(env: Map<string, i32>, name: string) -> i32 =
        if env.has(name) then env.get(name) else 0 end

      fn main() -> i32 = do
        let env: Map<string, i32> = Map.new()
        env.set("x", 99)
        lookup(env, "x") + lookup(env, "y")
      end
    MLC
      assert_equal 99, status.exitstatus
    end
  end

  # ─── AST evaluation patterns ────────────────────────────────────────────

  # Simple expression evaluator (flat ADT, no recursion)
  def test_flat_expr_evaluator
    run_mlc(<<~MLC) do |_out, _err, status|
      type Expr = Lit(i32) | Add(i32, i32) | Mul(i32, i32) | Sub(i32, i32)

      fn eval(e: Expr) -> i32 =
        match e
          Lit(n) => n
          Add(a, b) => a + b
          Mul(a, b) => a * b
          Sub(a, b) => a - b
        end

      fn main() -> i32 = do
        let r1 = eval(Add(10, 5))
        let r2 = eval(Mul(3, 4))
        let r3 = eval(Sub(20, 8))
        r1 + r2 + r3
      end
    MLC
      assert_equal 39, status.exitstatus # 15+12+12
    end
  end

  # Multi-field ADT with conditional dispatch (like operator dispatch in codegen)
  def test_operator_dispatch
    run_mlc(<<~MLC) do |_out, _err, status|
      type Op = BinOp(string, i32, i32) | UnOp(string, i32)

      fn eval_op(op: Op) -> i32 =
        match op
          BinOp(name, a, b) =>
            if name == "add" then a + b
            else if name == "sub" then a - b
            else if name == "mul" then a * b
            else 0
            end
          UnOp(name, x) =>
            if name == "neg" then 0 - x
            else x
            end
        end

      fn main() -> i32 = do
        let r1 = eval_op(BinOp("add", 10, 5))
        let r2 = eval_op(BinOp("mul", 3, 4))
        let r3 = eval_op(UnOp("neg", 7))
        r1 + r2 + r3
      end
    MLC
      assert_equal 20, status.exitstatus # 15+12-7
    end
  end

  # ─── Code generation patterns ───────────────────────────────────────────

  # String building for code emission
  def test_codegen_emit
    run_mlc(<<~MLC) do |_out, _err, status|
      fn emit_load(reg: i32, val: i32) -> string =
        "LOAD r" + to_string(reg) + ", " + to_string(val)

      fn emit_add(dst: i32, src: i32) -> string =
        "ADD r" + to_string(dst) + ", r" + to_string(src)

      fn main() -> i32 = do
        let i1 = emit_load(1, 42)
        let i2 = emit_add(0, 1)
        i1.length() + i2.length()
      end
    MLC
      assert_equal 21, status.exitstatus # "LOAD r1, 42"=11 + "ADD r0, r1"=10
    end
  end

  # Collect emitted instructions into array
  def test_codegen_instruction_collection
    run_mlc(<<~MLC) do |_out, _err, status|
      type Instr = Load(i32, i32) | Add(i32, i32) | Ret

      fn instr_size(i: Instr) -> i32 =
        match i
          Load(_, _) => 4
          Add(_, _) => 2
          Ret => 1
        end

      fn main() -> i32 = do
        let program = [Load(0, 42), Add(0, 1), Load(1, 10), Ret]
        let total = 0
        for instr in program do
          total = total + instr_size(instr)
        end
        total
      end
    MLC
      assert_equal 11, status.exitstatus # 4+2+4+1
    end
  end

  # ─── Recursive functions ────────────────────────────────────────────────

  def test_recursive_fibonacci
    run_mlc(<<~MLC) do |_out, _err, status|
      fn fib(n: i32) -> i32 =
        if n <= 1 then n
        else fib(n - 1) + fib(n - 2)
        end

      fn main() -> i32 = fib(10)
    MLC
      assert_equal 55, status.exitstatus
    end
  end

  def test_recursive_count_digits
    run_mlc(<<~MLC) do |_out, _err, status|
      fn is_digit(c: string) -> bool = c >= "0" && c <= "9"

      fn count(src: string) -> i32 = do
        let n = 0
        let i = 0
        while i < src.length() do
          if is_digit(src.char_at(i)) then
            n = n + 1
          end
          i = i + 1
        end
        n
      end

      fn main() -> i32 = count("a1b2c3d4e5")
    MLC
      assert_equal 5, status.exitstatus
    end
  end

  # ─── Scope / environment ────────────────────────────────────────────────

  def test_scope_shadowing_via_map
    run_mlc(<<~MLC) do |_out, _err, status|
      fn push_scope(outer: Map<string, i32>, k: string, v: i32) -> Map<string, i32> = do
        let inner: Map<string, i32> = Map.new()
        inner.set(k, v)
        inner
      end

      fn main() -> i32 = do
        let global: Map<string, i32> = Map.new()
        global.set("x", 1)
        let local = push_scope(global, "x", 99)
        local.get("x")
      end
    MLC
      assert_equal 99, status.exitstatus
    end
  end

  # ─── File I/O integration ────────────────────────────────────────────────

  def test_file_read_write_parse
    run_mlc(<<~MLC) do |_out, _err, status|
      fn count_chars(src: string) -> i32 = do
        let n = 0
        let i = 0
        while i < src.length() do
          n = n + 1
          i = i + 1
        end
        n
      end

      fn main() -> i32 = do
        let path = "/tmp/mlc_sh_test_input.txt"
        File.write(path, "hello world")
        let content = File.read(path)
        count_chars(content)
      end
    MLC
      assert_equal 11, status.exitstatus # "hello world".length = 11
    end
  end

  # ─── stdout output (critical for compiler diagnostics) ──────────────────

  def test_println_basic
    run_mlc(<<~MLC) do |stdout, _err, status|
      fn main() -> i32 = do
        println("hello world")
        0
      end
    MLC
      assert_equal 0, status.exitstatus
      assert_equal "hello world", stdout.strip
    end
  end

  def test_println_integer
    run_mlc(<<~MLC) do |stdout, _err, status|
      fn main() -> i32 = do
        let x = 42
        println(to_string(x))
        0
      end
    MLC
      assert_equal "42", stdout.strip
    end
  end

  def test_println_in_loop
    run_mlc(<<~MLC) do |stdout, _err, status|
      fn main() -> i32 = do
        let i = 0
        while i < 4 do
          println(to_string(i))
          i = i + 1
        end
        0
      end
    MLC
      assert_equal "0\n1\n2\n3", stdout.strip
    end
  end

  # Compiler-style diagnostic output: "error at line N: message"
  def test_println_diagnostic_format
    run_mlc(<<~MLC) do |stdout, _err, status|
      fn report_error(line: i32, msg: string) -> i32 = do
        println("error at line " + to_string(line) + ": " + msg)
        1
      end

      fn main() -> i32 = do
        let code = report_error(5, "unexpected token")
        let code2 = report_error(12, "undefined variable")
        code + code2
      end
    MLC
      lines = stdout.strip.split("\n")
      assert_equal 2, lines.length
      assert_equal "error at line 5: unexpected token", lines[0]
      assert_equal "error at line 12: undefined variable", lines[1]
      assert_equal 2, status.exitstatus
    end
  end

  # Token stream debug output
  def test_println_token_debug
    run_mlc(<<~MLC) do |stdout, _err, status|
      type Token = TInt(i32) | TIdent(string) | TPlus | TEof

      fn token_to_string(t: Token) -> string =
        match t
          TInt(v) => "TInt(" + to_string(v) + ")"
          TIdent(s) => "TIdent(" + s + ")"
          TPlus => "TPlus"
          TEof => "TEof"
        end

      fn main() -> i32 = do
        let tokens = [TInt(42), TPlus, TIdent("x"), TEof]
        let i = 0
        while i < tokens.length() do
          println(token_to_string(tokens.get(i)))
          i = i + 1
        end
        0
      end
    MLC
      lines = stdout.strip.split("\n")
      assert_equal ["TInt(42)", "TPlus", "TIdent(x)", "TEof"], lines
    end
  end
end
