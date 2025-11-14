# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCLetTest < Minitest::Test
  def test_let_lowering_generates_lambda_iife
    mlc_source = <<~MLC
      fn main() -> i32 =
        let x = 1
        x + 1
    MLC

    cpp = MLC.to_cpp(mlc_source)

    refute_includes cpp, "[&]()"
    assert_includes cpp, "const int x = 1;"
    assert_includes cpp, "return x + 1;"
  end

  def test_let_expression_retains_constexpr_effect
    source = <<~MLC
      fn main() -> i32 =
        let x = 1
        x + 1
    MLC

    ast = MLC.parse(source)
    core_ir, = MLC.transform_to_core_with_registry(ast)
    func = core_ir.items.grep(MLC::SemanticIR::Func).first

    refute_nil func
    assert_includes func.effects, :constexpr
  end
end
