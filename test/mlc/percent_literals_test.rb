# frozen_string_literal: true

require_relative "../test_helper"
require_relative "../../lib/mlc/common/index"

class MLCPercentLiteralsTest < Minitest::Test
  # ========== Lexer Tests for %w[] ==========

  def test_lexer_percent_w_simple
    source = "%w[foo bar baz]"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    string_array_tokens = tokens.select { |t| t.type == :STRING_ARRAY }
    assert_equal 1, string_array_tokens.size
    assert_equal %w[foo bar baz], string_array_tokens.first.value
  end

  def test_lexer_percent_w_empty
    source = "%w[]"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    string_array_tokens = tokens.select { |t| t.type == :STRING_ARRAY }
    assert_equal 1, string_array_tokens.size
    assert_equal [], string_array_tokens.first.value
  end

  def test_lexer_percent_w_with_braces
    source = "%w{one two three}"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    string_array_tokens = tokens.select { |t| t.type == :STRING_ARRAY }
    assert_equal 1, string_array_tokens.size
    assert_equal %w[one two three], string_array_tokens.first.value
  end

  def test_lexer_percent_w_with_parens
    source = "%w(alpha beta gamma)"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    string_array_tokens = tokens.select { |t| t.type == :STRING_ARRAY }
    assert_equal 1, string_array_tokens.size
    assert_equal %w[alpha beta gamma], string_array_tokens.first.value
  end

  def test_lexer_percent_w_multiline
    source = "%w[\n  hello\n  world\n]"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    string_array_tokens = tokens.select { |t| t.type == :STRING_ARRAY }
    assert_equal 1, string_array_tokens.size
    assert_equal %w[hello world], string_array_tokens.first.value
  end

  def test_lexer_percent_w_escaped_space
    source = "%w[hello\\ world foo]"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    string_array_tokens = tokens.select { |t| t.type == :STRING_ARRAY }
    assert_equal 1, string_array_tokens.size
    # \s in %w means literal space within word
    assert_equal ["hello world", "foo"], string_array_tokens.first.value
  end

  # ========== Lexer Tests for %i[] ==========

  def test_lexer_percent_i_simple
    source = "%i[foo bar baz]"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    symbol_array_tokens = tokens.select { |t| t.type == :SYMBOL_ARRAY }
    assert_equal 1, symbol_array_tokens.size
    assert_equal %w[foo bar baz], symbol_array_tokens.first.value
  end

  def test_lexer_percent_i_empty
    source = "%i[]"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    symbol_array_tokens = tokens.select { |t| t.type == :SYMBOL_ARRAY }
    assert_equal 1, symbol_array_tokens.size
    assert_equal [], symbol_array_tokens.first.value
  end

  # ========== Lexer Tests for %W[] and %I[] ==========

  def test_lexer_percent_W_interpolated
    source = "%W[foo bar baz]"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    interp_tokens = tokens.select { |t| t.type == :STRING_ARRAY_INTERP }
    assert_equal 1, interp_tokens.size
    assert_equal %w[foo bar baz], interp_tokens.first.value
  end

  def test_lexer_percent_I_interpolated
    source = "%I[foo bar baz]"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    interp_tokens = tokens.select { |t| t.type == :SYMBOL_ARRAY_INTERP }
    assert_equal 1, interp_tokens.size
    assert_equal %w[foo bar baz], interp_tokens.first.value
  end

  # ========== Parser Tests ==========

  def test_parser_percent_w_array_literal
    source = <<~MLC
      fn test() -> str = %w[hello world]
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::ArrayLiteral, body
    assert_equal 2, body.elements.size
    assert_instance_of MLC::Source::AST::StringLit, body.elements[0]
    assert_equal "hello", body.elements[0].value
    assert_equal "world", body.elements[1].value
  end

  def test_parser_percent_i_symbol_array
    source = <<~MLC
      fn test() -> str = %i[foo bar baz]
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    assert_instance_of MLC::Source::AST::ArrayLiteral, body
    assert_equal 3, body.elements.size
    assert_instance_of MLC::Source::AST::SymbolLit, body.elements[0]
    assert_equal "foo", body.elements[0].name
    assert_equal "bar", body.elements[1].name
    assert_equal "baz", body.elements[2].name
  end

  def test_parser_percent_w_in_expression
    source = <<~MLC
      fn first_word() -> str = %w[alpha beta gamma][0]
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    # Should be an index access on an array literal
    assert_instance_of MLC::Source::AST::IndexAccess, body
    assert_instance_of MLC::Source::AST::ArrayLiteral, body.object
  end

  # ========== Origin/Location Tests ==========

  def test_percent_w_has_origin
    source = <<~MLC
      fn test() -> str = %w[a b c]
    MLC

    ast = MLC.parse(source)
    func = ast.declarations.first
    body = func.body

    refute_nil body.origin, "ArrayLiteral from %w[] should have origin"
    assert_respond_to body.origin, :line
  end

  # ========== Edge Cases ==========

  def test_lexer_percent_w_single_word
    source = "%w[single]"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    string_array_tokens = tokens.select { |t| t.type == :STRING_ARRAY }
    assert_equal 1, string_array_tokens.size
    assert_equal %w[single], string_array_tokens.first.value
  end

  def test_lexer_percent_w_with_numbers
    source = "%w[item1 item2 item3]"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    string_array_tokens = tokens.select { |t| t.type == :STRING_ARRAY }
    assert_equal 1, string_array_tokens.size
    assert_equal %w[item1 item2 item3], string_array_tokens.first.value
  end

  def test_lexer_percent_w_with_underscores
    source = "%w[snake_case camelCase PascalCase]"
    lexer = MLC::Source::Parser::Lexer.new(source)
    tokens = lexer.tokenize

    string_array_tokens = tokens.select { |t| t.type == :STRING_ARRAY }
    assert_equal 1, string_array_tokens.size
    assert_equal %w[snake_case camelCase PascalCase], string_array_tokens.first.value
  end
end
