# frozen_string_literal: true

# Minimal parser stub used by RSpec smoke test.
class Parser
  class InvalidInputError < StandardError; end

  # Normalizes input to a string so callers can pass anything string-like.
  def normalize_source(source)
    validate_input!(source)

    source.to_s
  end

  # Echoes the provided source to demonstrate the parser interface.
  def parse(source)
    normalize_source(source)
  end

  # Very small "complex" parse: split source into non-empty tokens.
  # This is intentionally minimal to act as a smoke-testable surface.
  def parse_complex_input(source, downcase: false, reject_pattern: nil)
    tokens = tokenize(normalize_source(source))
    tokens.map!(&:downcase) if downcase

    if reject_pattern
      bad = tokens.find { |t| t.match?(reject_pattern) }
      raise InvalidInputError, "token '#{bad}' rejected by pattern #{reject_pattern.inspect}" if bad
    end

    tokens
  end

  private

  def validate_input!(source)
    raise InvalidInputError, "source cannot be nil" if source.nil?
    raise InvalidInputError, "source must respond to #to_s" unless source.respond_to?(:to_s)
  end

  def tokenize(str)
    str = str.strip
    return [] if str.empty?

    str.split(/\s+/).reject(&:empty?)
  end
end
