# frozen_string_literal: true

require_relative "../../test_helper"

class EffectAnalyzerTest < Minitest::Test
  def test_default_effects_include_noexcept
    analyzer = MLC::Common::Typing::EffectAnalyzer.new(
      pure_expression: ->(_) { true },
      non_literal_type: ->(_) { false }
    )

    result = analyzer.analyze(nil)

    assert_includes result, :noexcept
  end

  def test_nil_body_returns_default_effects
    analyzer = MLC::Common::Typing::EffectAnalyzer.new(
      pure_expression: ->(_) { true },
      non_literal_type: ->(_) { false }
    )

    result = analyzer.analyze(nil)

    assert_equal [:noexcept], result
  end

  def test_pure_body_with_literal_type_adds_constexpr
    analyzer = MLC::Common::Typing::EffectAnalyzer.new(
      pure_expression: ->(_) { true },
      non_literal_type: ->(_) { false }
    )

    mock_body = Object.new

    result = analyzer.analyze(mock_body, return_type: "i32")

    assert_includes result, :constexpr
    assert_includes result, :noexcept
    assert_equal [:constexpr, :noexcept], result
  end

  def test_impure_body_does_not_add_constexpr
    analyzer = MLC::Common::Typing::EffectAnalyzer.new(
      pure_expression: ->(_) { false },
      non_literal_type: ->(_) { false }
    )

    mock_body = Object.new

    result = analyzer.analyze(mock_body, return_type: "i32")

    refute_includes result, :constexpr
    assert_includes result, :noexcept
  end

  def test_non_literal_return_type_does_not_add_constexpr
    analyzer = MLC::Common::Typing::EffectAnalyzer.new(
      pure_expression: ->(_) { true },
      non_literal_type: ->(_) { true }
    )

    mock_body = Object.new

    result = analyzer.analyze(mock_body, return_type: "SomeClass")

    refute_includes result, :constexpr
    assert_includes result, :noexcept
  end

  def test_nil_return_type_treated_as_literal
    analyzer = MLC::Common::Typing::EffectAnalyzer.new(
      pure_expression: ->(_) { true },
      non_literal_type: ->(_) { true }
    )

    mock_body = Object.new

    result = analyzer.analyze(mock_body, return_type: nil)

    assert_includes result, :constexpr
  end

  def test_custom_default_effects
    analyzer = MLC::Common::Typing::EffectAnalyzer.new(
      pure_expression: ->(_) { false },
      non_literal_type: ->(_) { false },
      default_effects: [:inline, :noexcept]
    )

    result = analyzer.analyze(nil)

    assert_includes result, :inline
    assert_includes result, :noexcept
  end

  def test_no_duplicate_effects
    analyzer = MLC::Common::Typing::EffectAnalyzer.new(
      pure_expression: ->(_) { true },
      non_literal_type: ->(_) { false },
      default_effects: [:noexcept, :noexcept]
    )

    mock_body = Object.new

    result = analyzer.analyze(mock_body)

    assert_equal result, result.uniq
  end

  def test_pure_expression_callback_receives_body
    received_body = nil
    analyzer = MLC::Common::Typing::EffectAnalyzer.new(
      pure_expression: ->(body) {
        received_body = body
        true
      },
      non_literal_type: ->(_) { false }
    )

    mock_body = Object.new
    analyzer.analyze(mock_body)

    assert_equal mock_body, received_body
  end

  def test_non_literal_type_callback_receives_return_type
    received_type = nil
    analyzer = MLC::Common::Typing::EffectAnalyzer.new(
      pure_expression: ->(_) { true },
      non_literal_type: ->(type) {
        received_type = type
        false
      }
    )

    mock_body = Object.new
    analyzer.analyze(mock_body, return_type: "CustomType")

    assert_equal "CustomType", received_type
  end
end
