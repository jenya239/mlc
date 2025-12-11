# frozen_string_literal: true

# Minimal parser stub used by RSpec smoke test.
class Parser
  # Normalizes input to a string so callers can pass anything string-like.
  def normalize_source(source)
    raise ArgumentError, "source cannot be nil" if source.nil?

    source.to_s
  end

  # Echoes the provided source to demonstrate the parser interface.
  def parse(source)
    normalize_source(source)
  end

  # Very small "complex" parse: split source into non-empty tokens.
  # This is intentionally minimal to act as a smoke-testable surface.
  def parse_complex_input(source)
    tokenize(normalize_source(source))
  end

  private

  def tokenize(str)
    return [] if str.empty?

    str.split(/\s+/).reject(&:empty?)
  end
end
