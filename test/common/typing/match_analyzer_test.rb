# frozen_string_literal: true

require_relative "../../test_helper"

class MatchAnalyzerTest < Minitest::Test
  def setup
    @compatible_calls = []
    @ensure_compatible = ->(actual, expected, context) {
      @compatible_calls << { actual: actual, expected: expected, context: context }
    }
  end

  def test_analyze_requires_at_least_one_arm
    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: @ensure_compatible
    )

    error = assert_raises(ArgumentError) do
      analyzer.analyze(
        scrutinee_type: "i32",
        arms: [],
        transform_arm: ->(type, arm) { arm }
      )
    end

    assert_equal "match expression requires at least one arm", error.message
  end

  def test_analyze_requires_arm_body_to_have_type
    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: @ensure_compatible
    )

    mock_body = Struct.new(:type).new(nil)

    error = assert_raises(ArgumentError) do
      analyzer.analyze(
        scrutinee_type: "i32",
        arms: [{ pattern: nil, body: mock_body }],
        transform_arm: ->(type, arm) { arm }
      )
    end

    assert_equal "match arm body must have a type", error.message
  end

  def test_analyze_single_arm
    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: @ensure_compatible
    )

    mock_body = Struct.new(:type).new("i32")

    result = analyzer.analyze(
      scrutinee_type: "i32",
      arms: [{ pattern: nil, body: mock_body }],
      transform_arm: ->(type, arm) { arm }
    )

    assert_instance_of MLC::Common::Typing::MatchAnalyzer::Analysis, result
    assert_equal "i32", result.result_type
    assert_equal 1, result.arms.size
  end

  def test_analyze_multiple_arms_checks_type_compatibility
    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: @ensure_compatible
    )

    mock_body1 = Struct.new(:type).new("i32")
    mock_body2 = Struct.new(:type).new("i32")
    mock_body3 = Struct.new(:type).new("i32")

    result = analyzer.analyze(
      scrutinee_type: "Option<i32>",
      arms: [
        { pattern: nil, body: mock_body1 },
        { pattern: nil, body: mock_body2 },
        { pattern: nil, body: mock_body3 }
      ],
      transform_arm: ->(type, arm) { arm }
    )

    assert_equal 3, result.arms.size
    assert_equal "i32", result.result_type
    # First arm is reference, so compatibility check called for arms 2 and 3
    assert_equal 3, @compatible_calls.size
    assert_equal "match arm 1", @compatible_calls[0][:context]
    assert_equal "match arm 2", @compatible_calls[1][:context]
    assert_equal "match arm 3", @compatible_calls[2][:context]
  end

  def test_analyze_uses_transform_arm_callback
    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: @ensure_compatible
    )

    transform_calls = []
    mock_body = Struct.new(:type).new("bool")

    result = analyzer.analyze(
      scrutinee_type: "Shape",
      arms: [{ pattern: :circle, body: mock_body }],
      transform_arm: ->(type, arm) {
        transform_calls << { type: type, arm: arm }
        arm
      }
    )

    assert_equal 1, transform_calls.size
    assert_equal "Shape", transform_calls.first[:type]
    assert_equal :circle, transform_calls.first[:arm][:pattern]
  end

  # Exhaustiveness checking tests

  def test_exhaustiveness_check_with_missing_variant
    type_registry = MockTypeRegistry.new(
      "Option" => MockTypeInfo.new(
        sum: true,
        variants: [{ name: "Some" }, { name: "None" }]
      )
    )

    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: @ensure_compatible,
      type_registry: type_registry,
      check_exhaustiveness: true
    )

    mock_body = Struct.new(:type).new("i32")
    some_pattern = MockPattern.new(:constructor, { name: "Some" })

    error = assert_raises(MLC::CompileError) do
      analyzer.analyze(
        scrutinee_type: "Option",
        arms: [{ pattern: some_pattern, body: mock_body }],
        transform_arm: ->(type, arm) { arm }
      )
    end

    assert_match(/Non-exhaustive match/, error.message)
    assert_match(/None/, error.message)
  end

  def test_exhaustiveness_check_passes_when_all_variants_covered
    type_registry = MockTypeRegistry.new(
      "Option" => MockTypeInfo.new(
        sum: true,
        variants: [{ name: "Some" }, { name: "None" }]
      )
    )

    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: @ensure_compatible,
      type_registry: type_registry,
      check_exhaustiveness: true
    )

    mock_body = Struct.new(:type).new("i32")
    some_pattern = MockPattern.new(:constructor, { name: "Some" })
    none_pattern = MockPattern.new(:constructor, { name: "None" })

    result = analyzer.analyze(
      scrutinee_type: "Option",
      arms: [
        { pattern: some_pattern, body: mock_body },
        { pattern: none_pattern, body: mock_body }
      ],
      transform_arm: ->(type, arm) { arm }
    )

    assert_equal 2, result.arms.size
  end

  def test_exhaustiveness_check_passes_with_wildcard
    type_registry = MockTypeRegistry.new(
      "Option" => MockTypeInfo.new(
        sum: true,
        variants: [{ name: "Some" }, { name: "None" }]
      )
    )

    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: @ensure_compatible,
      type_registry: type_registry,
      check_exhaustiveness: true
    )

    mock_body = Struct.new(:type).new("i32")
    some_pattern = MockPattern.new(:constructor, { name: "Some" })
    wildcard_pattern = MockPattern.new(:wildcard, {})

    result = analyzer.analyze(
      scrutinee_type: "Option",
      arms: [
        { pattern: some_pattern, body: mock_body },
        { pattern: wildcard_pattern, body: mock_body }
      ],
      transform_arm: ->(type, arm) { arm }
    )

    assert_equal 2, result.arms.size
  end

  def test_exhaustiveness_check_with_or_patterns
    type_registry = MockTypeRegistry.new(
      "Color" => MockTypeInfo.new(
        sum: true,
        variants: [{ name: "Red" }, { name: "Green" }, { name: "Blue" }]
      )
    )

    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: @ensure_compatible,
      type_registry: type_registry,
      check_exhaustiveness: true
    )

    mock_body = Struct.new(:type).new("str")
    red_pattern = MockPattern.new(:constructor, { name: "Red" })
    green_pattern = MockPattern.new(:constructor, { name: "Green" })
    or_pattern = MockPattern.new(:or, {
      alternatives: [red_pattern, green_pattern]
    })
    blue_pattern = MockPattern.new(:constructor, { name: "Blue" })

    result = analyzer.analyze(
      scrutinee_type: "Color",
      arms: [
        { pattern: or_pattern, body: mock_body },
        { pattern: blue_pattern, body: mock_body }
      ],
      transform_arm: ->(type, arm) { arm }
    )

    assert_equal 2, result.arms.size
  end

  def test_extract_type_name_from_generic_string
    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: @ensure_compatible
    )

    # Test with generic type string
    result = analyzer.send(:extract_type_name, "Option<i32>")
    assert_equal "Option", result

    # Test with nested generic
    result = analyzer.send(:extract_type_name, "Result<Option<T>, Error>")
    assert_equal "Result", result

    # Test with qualified name
    result = analyzer.send(:extract_type_name, "Std::Option<T>")
    assert_equal "Option", result
  end

  def test_no_exhaustiveness_check_when_disabled
    type_registry = MockTypeRegistry.new(
      "Option" => MockTypeInfo.new(
        sum: true,
        variants: [{ name: "Some" }, { name: "None" }]
      )
    )

    analyzer = MLC::Common::Typing::MatchAnalyzer.new(
      ensure_compatible_type: @ensure_compatible,
      type_registry: type_registry,
      check_exhaustiveness: false
    )

    mock_body = Struct.new(:type).new("i32")
    some_pattern = MockPattern.new(:constructor, { name: "Some" })

    # Should not raise even with missing None
    result = analyzer.analyze(
      scrutinee_type: "Option",
      arms: [{ pattern: some_pattern, body: mock_body }],
      transform_arm: ->(type, arm) { arm }
    )

    assert_equal 1, result.arms.size
  end

  private

  # Mock helpers
  MockPattern = Struct.new(:kind, :data)

  class MockTypeInfo
    def initialize(sum:, variants: [])
      @sum = sum
      @variants = variants
    end

    def sum?
      @sum
    end

    attr_reader :variants
  end

  class MockTypeRegistry
    def initialize(types = {})
      @types = types
    end

    def lookup(name)
      @types[name]
    end
  end
end
