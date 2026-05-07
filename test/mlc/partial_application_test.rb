# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"
require "mlc/representations/semantic/gen/engine"

class PartialApplicationTest < Minitest::Test
  def setup
    @engine = MLC::Representations::Semantic::Gen::Engine.new
    @factory = @engine.services.ast_factory
  end

  def test_rewrite_single_placeholder_becomes_lambda
    ast = MLC.parse("fn add(x: i32, y: i32) -> i32 = x + y\nfn dummy() -> i32 = add(5, _)")
    body = ast.declarations.last.body
    out = MLC::Representations::Semantic::Gen::Services::Features::PartialApplicationDesugar.rewrite(
      body,
      factory: @factory,
      serial: [0]
    )
    assert_instance_of MLC::Source::AST::Lambda, out
    assert_equal 1, out.params.length
    assert_instance_of MLC::Source::AST::Call, out.body
    assert_equal "partial_application_parameter_0", out.params.first.name
    assert_instance_of MLC::Source::AST::VarRef, out.body.args.last
    assert_equal "partial_application_parameter_0", out.body.args.last.name
  end

  def test_rewrite_two_placeholders
    ast = MLC.parse("fn f(a: i32, b: i32) -> i32 = a + b\nfn g() -> i32 = f(_, _)")
    body = ast.declarations.last.body
    out = MLC::Representations::Semantic::Gen::Services::Features::PartialApplicationDesugar.rewrite(
      body,
      factory: @factory,
      serial: [0]
    )
    assert_instance_of MLC::Source::AST::Lambda, out
    assert_equal 2, out.params.length
    assert_equal "partial_application_parameter_0", out.params[0].name
    assert_equal "partial_application_parameter_1", out.params[1].name
  end

  def test_nested_calls_inner_first
    ast = MLC.parse("fn f(a: i32) -> i32 = a\nfn g(b: i32) -> i32 = b\nfn h() -> i32 = f(g(_))")
    body = ast.declarations.last.body
    out = MLC::Representations::Semantic::Gen::Services::Features::PartialApplicationDesugar.rewrite(
      body,
      factory: @factory,
      serial: [0]
    )
    assert_instance_of MLC::Source::AST::Call, out
    inner_lambda = out.args.first
    assert_instance_of MLC::Source::AST::Lambda, inner_lambda
    assert_instance_of MLC::Source::AST::Call, inner_lambda.body
    assert_equal "g", inner_lambda.body.callee.name
  end
end
