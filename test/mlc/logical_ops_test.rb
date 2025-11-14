# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCLogicalOpsTest < Minitest::Test
  def test_logical_or_in_condition
    source = <<~MLC
      fn main() -> i32 =
        if true || false then 1 else 0
    MLC

    cpp = MLC.to_cpp(source)
    assert_includes cpp, "true || false"
  end

  def test_logical_and_and_or_precedence
    source = <<~MLC
      fn eval(flag: bool) -> bool =
        if flag && true || false then true else false
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    condition = func.body.condition

    assert_instance_of MLC::AST::BinaryOp, condition
    assert_equal "||", condition.op
    assert_instance_of MLC::AST::BinaryOp, condition.left
    assert_equal "&&", condition.left.op
  end
end
