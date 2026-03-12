# frozen_string_literal: true

require_relative "../test_helper"
require "open3"
require "tmpdir"

# Infrastructure tests for self-hosted MLC compiler.
# Covers all language patterns required for MLC-in-MLC compilation.
class SelfHostingInfrastructureTest < Minitest::Test
  tag :slow

  CLI = File.expand_path("../../bin/mlc", __dir__)

  def run_mlc(source_code)
    Dir.mktmpdir do |dir|
      source = File.join(dir, "test.mlc")
      File.write(source, source_code)
      stdout, stderr, status = Open3.capture3(CLI, source)
      compile_errors = stderr.lines.select { |l| l.include?("error:") }
      refute compile_errors.any?, "Compilation failed:\n#{compile_errors.join}\nSource:\n#{source_code}"
      yield stdout, stderr, status if block_given?
    end
  end

  # ─── Named Record Types ─────────────────────────────────────────────────────
  # Pattern: type Tok = Tok { kind: i32, start: i32, end: i32 }

  def test_named_record_construction
    run_mlc(<<~MLC) do |_out, _err, status|
      type Span = Span { start: i32, len: i32 }

      fn make_span(s: i32, l: i32) -> Span = Span { start: s, len: l }

      fn main() -> i32 =
        let sp = make_span(10, 5)
        sp.start + sp.len
    MLC
      assert_equal 15, status.exitstatus
    end
  end

  def test_named_record_nested
    run_mlc(<<~MLC) do |_out, _err, status|
      type Span = Span { start: i32, len: i32 }
      type Token = Token { kind: i32, span: Span }

      fn main() -> i32 = do
        let sp = Span { start: 3, len: 4 }
        let tok = Token { kind: 7, span: sp }
        tok.kind + tok.span.start + tok.span.len
      end
    MLC
      assert_equal 14, status.exitstatus
    end
  end

  # ─── Recursive ADTs ─────────────────────────────────────────────────────────
  # Pattern: type Expr = Num(i32) | Add(Shared<Expr>, Shared<Expr>)

  def test_recursive_adt_depth
    run_mlc(<<~MLC) do |_out, _err, status|
      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      type Tree = Leaf(i32) | Node(Shared<Tree>, Shared<Tree>)

      fn depth(t: Tree) -> i32 =
        match t
        | Leaf(_) => 0
        | Node(l, r) => 1
        end

      fn main() -> i32 =
        let leaf = Leaf(42)
        depth(leaf)
    MLC
      assert_equal 0, status.exitstatus
    end
  end

  def test_recursive_adt_value_extraction
    run_mlc(<<~MLC) do |_out, _err, status|
      extend Shared<T> {
        extern fn new(value: T) -> Shared<T>
      }

      type Expr = Lit(i32) | Neg(Shared<Expr>)

      fn eval(e: Expr) -> i32 =
        match e
        | Lit(n) => n
        | Neg(_) => 0
        end

      fn main() -> i32 =
        let e = Lit(99)
        eval(e)
    MLC
      assert_equal 99, status.exitstatus
    end
  end

  # ─── Mutable Parameters ─────────────────────────────────────────────────────
  # Pattern: fn push_tokens(mut tokens: Token[], t: Token) -> unit

  def test_mut_parameter_array
    run_mlc(<<~MLC) do |_out, _err, status|
      fn collect(mut acc: i32[], v: i32) -> i32 = do
        acc.push(v)
        acc.length()
      end

      fn main() -> i32 = do
        let arr = [10, 20]
        collect(arr, 30)
      end
    MLC
      assert_equal 3, status.exitstatus
    end
  end

  def test_mut_parameter_string_building
    run_mlc(<<~MLC) do |_out, _err, status|
      fn append_n(mut parts: string[], s: string, n: i32) -> i32 = do
        let i = 0
        while i < n do
          parts.push(s)
          i = i + 1
        end
        parts.length()
      end

      fn main() -> i32 = do
        let buf = ["x"]
        append_n(buf, "y", 3)
      end
    MLC
      assert_equal 4, status.exitstatus
    end
  end

  # ─── Map / Symbol Table ─────────────────────────────────────────────────────
  # Pattern: let env = Map.new()

  def test_map_as_symbol_table
    run_mlc(<<~MLC) do |_out, _err, status|
      fn main() -> i32 = do
        let table: Map<string, i32> = Map.new()
        table.set("x", 10)
        table.set("y", 20)
        table.set("z", 30)
        table.get("x") + table.get("y") + table.get("z")
      end
    MLC
      assert_equal 60, status.exitstatus
    end
  end

  def test_map_lookup_missing_returns_zero
    run_mlc(<<~MLC) do |_out, _err, status|
      fn lookup(table: Map<string, i32>, key: string) -> i32 =
        table.get(key)

      fn main() -> i32 = do
        let t: Map<string, i32> = Map.new()
        t.set("a", 5)
        lookup(t, "a") + lookup(t, "b")
      end
    MLC
      assert_equal 5, status.exitstatus
    end
  end

  # ─── String Operations ───────────────────────────────────────────────────────
  # Pattern: lexer char-by-char scanning

  def test_string_scan_chars
    run_mlc(<<~MLC) do |_out, _err, status|
      fn count_digits(src: string) -> i32 = do
        let count = 0
        let i = 0
        while i < src.length() do
          let c = src.char_at(i)
          if c >= "0" && c <= "9" then
            count = count + 1
          end
          i = i + 1
        end
        count
      end

      fn main() -> i32 = count_digits("abc123def456")
    MLC
      assert_equal 6, status.exitstatus
    end
  end

  def test_string_build_from_chars
    run_mlc(<<~MLC) do |_out, _err, status|
      fn take_while_digit(src: string, start: i32) -> string = do
        let result = ""
        let i = start
        while i < src.length() do
          let c = src.char_at(i)
          if c >= "0" && c <= "9" then
            result = result + c
          else
            i = src.length()
          end
          i = i + 1
        end
        result
      end

      fn main() -> i32 = take_while_digit("abc123xyz", 3).length()
    MLC
      assert_equal 3, status.exitstatus
    end
  end

  # ─── ADT with Named Records (Token-like) ────────────────────────────────────
  # Pattern: type Token = TIdent { value: string } | TNum { value: i32 } | TEof

  def test_token_adt
    run_mlc(<<~MLC) do |_out, _err, status|
      type Token = TIdent(string) | TNum(i32) | TEof

      fn token_kind(t: Token) -> i32 =
        match t
        | TIdent(_) => 1
        | TNum(_)   => 2
        | TEof      => 0
        end

      fn main() -> i32 =
        token_kind(TIdent("foo")) * 100 +
        token_kind(TNum(42)) * 10 +
        token_kind(TEof)
    MLC
      assert_equal 120, status.exitstatus
    end
  end

  def test_token_value_extraction
    run_mlc(<<~MLC) do |_out, _err, status|
      type Token = TIdent(string) | TNum(i32) | TEof

      fn token_int_value(t: Token) -> i32 =
        match t
        | TNum(n) => n
        | _       => -1
        end

      fn main() -> i32 =
        token_int_value(TNum(77)) + token_int_value(TIdent("x")) + 1
    MLC
      assert_equal 77, status.exitstatus
    end
  end

  # ─── Array of ADTs ──────────────────────────────────────────────────────────
  # Pattern: let tokens: Token[] = [], then push and iterate

  def test_array_of_adt
    run_mlc(<<~MLC) do |_out, _err, status|
      type Token = TNum(i32) | TEof

      fn sum_nums(tokens: Token[], i: i32, acc: i32) -> i32 =
        if i >= tokens.length() then acc
        else
          match tokens[i]
          | TNum(n) => sum_nums(tokens, i + 1, acc + n)
          | TEof    => acc
          end
        end

      fn main() -> i32 = do
        let tokens = [TNum(10), TNum(20), TNum(30), TEof]
        sum_nums(tokens, 0, 0)
      end
    MLC
      assert_equal 60, status.exitstatus
    end
  end

  # ─── Pipe Operator ──────────────────────────────────────────────────────────
  # Pattern: src |> tokenize |> parse |> eval

  def test_pipe_chain
    run_mlc(<<~MLC) do |_out, _err, status|
      fn double(x: i32) -> i32 = x * 2
      fn add_one(x: i32) -> i32 = x + 1
      fn negate(x: i32) -> i32 = 0 - x

      fn main() -> i32 = 5 |> double |> add_one |> negate
    MLC
      # exit(-11) wraps to 245 on Linux (256 - 11)
      assert_equal 245, status.exitstatus
    end
  end

  # ─── Result<T,E> and ? operator ─────────────────────────────────────────────
  # Pattern: parse functions returning Result

  def test_result_ok_path
    run_mlc(<<~MLC) do |_out, _err, status|
      import { Result, ok, err } from "Result"

      fn safe_div(a: i32, b: i32) -> Result<i32, string> =
        if b == 0 then err("div0")
        else ok(a / b)
        end

      fn compute(a: i32, b: i32) -> Result<i32, string> =
        let x = safe_div(a, b)?
        ok(x * 2)

      fn main() -> i32 =
        match compute(10, 2)
        | Ok(v)  => v
        | Err(_) => 0
        end
    MLC
      assert_equal 10, status.exitstatus
    end
  end

  # ─── Recursive descent parser pattern ──────────────────────────────────────
  # Pattern: parser with index state, recursive functions

  def test_recursive_descent_pattern
    run_mlc(<<~MLC) do |_out, _err, status|
      fn parse_sum(tokens: i32[], pos: i32) -> i32 =
        if pos >= tokens.length() then 0
        else tokens[pos] + parse_sum(tokens, pos + 1)
        end

      fn main() -> i32 = do
        let nums = [1, 2, 3, 4, 5]
        parse_sum(nums, 0)
      end
    MLC
      assert_equal 15, status.exitstatus
    end
  end

  # ─── Type-tagged union (variant dispatch) ────────────────────────────────────
  # Pattern: codegen dispatching on IR node types

  def test_variant_dispatch_codegen
    run_mlc(<<~MLC) do |_out, _err, status|
      type IR = IConst(i32) | IAdd(i32, i32) | INeg(i32)

      fn eval_ir(node: IR) -> i32 =
        match node
        | IConst(v)    => v
        | IAdd(a, b)   => a + b
        | INeg(v)      => 0 - v
        end

      fn main() -> i32 =
        eval_ir(IConst(5)) +
        eval_ir(IAdd(3, 4)) +
        eval_ir(INeg(2))
    MLC
      assert_equal 10, status.exitstatus
    end
  end

  # ─── String array join (for code emission) ───────────────────────────────────
  # Pattern: collecting emitted code fragments and joining

  def test_array_join_for_emission
    run_mlc(<<~MLC) do |_out, _err, status|
      fn emit_lines(lines: string[]) -> string = lines.join("\n")

      fn main() -> i32 = do
        let code = ["int x = 1;", "int y = 2;", "return x + y;"]
        let src = emit_lines(code)
        src.length()
      end
    MLC
      assert_equal 35, status.exitstatus # "int x = 1;\nint y = 2;\nreturn x + y;" = 35 chars
    end
  end
end
