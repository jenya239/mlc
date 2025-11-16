# frozen_string_literal: true

require_relative "../test_helper"
require "ostruct"

class MatchAnalyzerTest < Minitest::Test
  Arm = Struct.new(:pattern, :guard, :body)

  def build_analyzer
    MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: ->(actual, expected, _context) do
        raise "type mismatch" unless actual == expected
      end
    )
  end

  def test_analyzer_returns_result_type
    analyzer = build_analyzer
    body_type = OpenStruct.new(type: "i32")
    arms = [
      { pattern: nil, guard: nil, body: OpenStruct.new(type: "i32") },
      { pattern: nil, guard: nil, body: OpenStruct.new(type: "i32") }
    ]

    analysis = analyzer.analyze(
      scrutinee_type: "Option<i32>",
      arms: arms,
      transform_arm: ->(_scrutinee, arm) { arm }
    )

    assert_equal "i32", analysis.result_type
    assert_equal arms, analysis.arms
  end

  def test_analyzer_persists_guards
    analyzer = build_analyzer
    guard = OpenStruct.new(type: "bool")
    arms = [
      { pattern: nil, guard: guard, body: OpenStruct.new(type: "i32") }
    ]

    analysis = analyzer.analyze(
      scrutinee_type: "Option<i32>",
      arms: arms,
      transform_arm: ->(_scrutinee, arm) { arm }
    )

    assert_equal guard, analysis.arms.first[:guard]
  end

  def test_analyzer_rejects_empty_match
    analyzer = build_analyzer

    assert_raises(ArgumentError) do
      analyzer.analyze(
        scrutinee_type: "Option<i32>",
        arms: [],
        transform_arm: ->(_scrutinee, arm) { arm }
      )
    end
  end

  def test_analyzer_propagates_type_mismatches
    ensure_calls = []
    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: lambda do |actual, expected, context|
        ensure_calls << [actual, expected, context]
        raise MLC::CompileError, "Mismatch" unless actual == expected
      end
    )
    arms = [
      { pattern: nil, guard: nil, body: OpenStruct.new(type: "i32") },
      { pattern: nil, guard: nil, body: OpenStruct.new(type: "string") }
    ]

    assert_raises(MLC::CompileError) do
      analyzer.analyze(
        scrutinee_type: "Option<i32>",
        arms: arms,
        transform_arm: ->(_scrutinee, arm) { arm }
      )
    end

    assert_equal 2, ensure_calls.length
    assert_equal "match arm 2", ensure_calls.last.last
  end

  def test_exhaustiveness_check_all_variants_covered
    # Mock type registry with sum type
    type_registry = Minitest::Mock.new
    sum_type_info = Object.new
    sum_type_info.define_singleton_method(:sum?) { true }
    sum_type_info.define_singleton_method(:variants) do
      [{ name: "Some" }, { name: "None" }]
    end

    type_registry.expect(:lookup, sum_type_info, ["Option"])

    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: ->(_actual, _expected, _context) {},
      type_registry: type_registry,
      check_exhaustiveness: true
    )

    # All variants covered
    arms = [
      { pattern: OpenStruct.new(kind: :constructor, data: { name: "Some" }), body: OpenStruct.new(type: "i32") },
      { pattern: OpenStruct.new(kind: :constructor, data: { name: "None" }), body: OpenStruct.new(type: "i32") }
    ]

    # Should not raise
    analysis = analyzer.analyze(
      scrutinee_type: "Option<i32>",
      arms: arms,
      transform_arm: ->(_scrutinee, arm) { arm }
    )

    assert_equal 2, analysis.arms.length
    type_registry.verify
  end

  def test_exhaustiveness_check_missing_variant
    # Mock type registry with sum type
    type_registry = Minitest::Mock.new
    sum_type_info = Object.new
    sum_type_info.define_singleton_method(:sum?) { true }
    sum_type_info.define_singleton_method(:variants) do
      [{ name: "Ok" }, { name: "Err" }]
    end

    type_registry.expect(:lookup, sum_type_info, ["Result"])

    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: ->(_actual, _expected, _context) {},
      type_registry: type_registry,
      check_exhaustiveness: true
    )

    # Only Ok covered, missing Err
    arms = [
      { pattern: OpenStruct.new(kind: :constructor, data: { name: "Ok" }), body: OpenStruct.new(type: "i32") }
    ]

    # Should raise CompileError
    error = assert_raises(MLC::CompileError) do
      analyzer.analyze(
        scrutinee_type: "Result<i32>",
        arms: arms,
        transform_arm: ->(_scrutinee, arm) { arm }
      )
    end

    assert_match(/Non-exhaustive match/, error.message)
    assert_match(/Err/, error.message)
    type_registry.verify
  end

  def test_exhaustiveness_check_wildcard_satisfies
    # Mock type registry with sum type
    type_registry = Minitest::Mock.new
    sum_type_info = Object.new
    sum_type_info.define_singleton_method(:sum?) { true }
    sum_type_info.define_singleton_method(:variants) do
      [{ name: "Some" }, { name: "None" }]
    end

    type_registry.expect(:lookup, sum_type_info, ["Option"])

    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: ->(_actual, _expected, _context) {},
      type_registry: type_registry,
      check_exhaustiveness: true
    )

    # Only Some covered, but wildcard present
    arms = [
      { pattern: OpenStruct.new(kind: :constructor, data: { name: "Some" }), body: OpenStruct.new(type: "i32") },
      { pattern: OpenStruct.new(kind: :wildcard), body: OpenStruct.new(type: "i32") }
    ]

    # Should not raise because wildcard covers remaining cases
    analysis = analyzer.analyze(
      scrutinee_type: "Option<i32>",
      arms: arms,
      transform_arm: ->(_scrutinee, arm) { arm }
    )

    assert_equal 2, analysis.arms.length
    type_registry.verify
  end

  def test_exhaustiveness_check_disabled
    # Mock type registry (should not be called)
    type_registry = Minitest::Mock.new

    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: ->(_actual, _expected, _context) {},
      type_registry: type_registry,
      check_exhaustiveness: false  # Disabled
    )

    # Missing variant, but checking disabled
    arms = [
      { pattern: OpenStruct.new(kind: :constructor, data: { name: "Ok" }), body: OpenStruct.new(type: "i32") }
    ]

    # Should not raise or call type_registry
    analysis = analyzer.analyze(
      scrutinee_type: "Result<i32>",
      arms: arms,
      transform_arm: ->(_scrutinee, arm) { arm }
    )

    assert_equal 1, analysis.arms.length
    # type_registry.verify would fail if lookup was called
  end

  def test_exhaustiveness_check_non_sum_type
    # Mock type registry with record type
    type_registry = Minitest::Mock.new
    record_type_info = Object.new
    record_type_info.define_singleton_method(:sum?) { false }

    type_registry.expect(:lookup, record_type_info, ["Point"])

    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: ->(_actual, _expected, _context) {},
      type_registry: type_registry,
      check_exhaustiveness: true
    )

    # Record type, not sum type - no exhaustiveness check
    arms = [
      { pattern: OpenStruct.new(kind: :literal, data: { value: 0 }), body: OpenStruct.new(type: "i32") }
    ]

    # Should not raise
    analysis = analyzer.analyze(
      scrutinee_type: "Point",
      arms: arms,
      transform_arm: ->(_scrutinee, arm) { arm }
    )

    assert_equal 1, analysis.arms.length
    type_registry.verify
  end
end
