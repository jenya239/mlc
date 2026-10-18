# frozen_string_literal: true

require "minitest/autorun"
require_relative "../../lib/mlc/common/index"

class IfExpressionBlockTest < Minitest::Test
  def parse!(source)
    MLC.parse(source, filename: "if_block.mlc")
  end

  def test_if_else_do_is_last_expression_of_function_block
    parse!(<<~MLC)
      fn f(a: bool) -> i32 = do
        if a then 1 else do
          2
        end
      end
    MLC
  end

  def test_if_else_expression_keeps_its_own_end
    parse!(<<~MLC)
      fn f(a: bool) -> i32 = do
        const x = 1
        if a then x else 2
        end
      end
    MLC
  end

  def test_same_line_end_closes_if_and_statements_follow
    parse!(<<~MLC)
      fn f(a: bool) -> i32 = do
        let mut x = 0
        if a then x = 1 else do
          x = 2
        end end
        x
      end
    MLC
  end

  def test_same_line_end_closes_if_before_function_end
    parse!(<<~MLC)
      fn f(a: bool) -> i32 = do
        if a then 1 else do
          2
        end end
      end
    MLC
  end

  def test_if_else_do_end_then_outer_end
    parse!(<<~MLC)
      fn f(a: bool) -> i32 = do
        if a then 1 else do
          2
        end
        end
      end
    MLC
  end

  def test_if_as_function_body_keeps_its_end
    parse!(<<~MLC)
      fn f(a: bool) -> i32 =
        if a then 1 else do
          2
        end end
    MLC
  end

  def test_match_arm_if_consumes_its_end
    parse!(<<~MLC)
      fn f(n: i32) -> i32 =
        match n
          | 0 => if n == 0 then 1 else do 2 end end
          | _ => 0
    MLC
  end

  def test_record_update_generator_parses
    parse!(File.read("compiler/codegen/expr/record_gen.mlc"))
  end
end
