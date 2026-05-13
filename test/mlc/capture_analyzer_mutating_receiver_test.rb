# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/representations/semantic/gen/services/features/capture_analyzer"

# Mutating-collection calls (receiver.push/set/...) change COW payloads; lambdas must
# capture receivers by reference, else outer bindings stay stale (#fold + push pattern).
class CaptureAnalyzerMutatingReceiverTest < Minitest::Test
  AST = MLC::Source::AST

  class StubVarTypeRegistry
    def initialize
      @bindings = {}
    end

    def add(name, type_stub)
      @bindings[name.to_s] = type_stub
    end

    def get(name)
      @bindings[name.to_s]
    end
  end

  def analyzer_for_scope_type
    registry = StubVarTypeRegistry.new
    registry.add("scope", Object.new)
    MLC::Representations::Semantic::Gen::Services::CaptureAnalyzer.new(var_type_registry: registry)
  end

  def test_member_push_marks_receiver_for_reference_capture
    push_call_expression = AST::Call.new(
      callee: AST::MemberAccess.new(
        object: AST::VarRef.new(name: "scope"),
        member: "push",
      ),
      args: [AST::StringLit.new(value: "hello")]
    )
    lambda_body_expression = AST::BlockExpr.new(
      statements: [AST::ExprStmt.new(expr: push_call_expression)],
      result_expr: AST::UnitLit.new
    )
    lambda_under_test = AST::Lambda.new(params: [], body: lambda_body_expression)

    captures = analyzer_for_scope_type.analyze(lambda_under_test)
    scope_capture = captures.find { |candidate| candidate[:name] == "scope" }
    refute_nil scope_capture
    assert_equal :ref, scope_capture[:mode]
  end
end
