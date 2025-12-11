# frozen_string_literal: true

# Minimal parser stub used by RSpec smoke test.
class Parser
  # Echoes the provided source to demonstrate the parser interface.
  def parse(source)
    source.to_s
  end

  # Very small "complex" parse: split source into non-empty tokens.
  # This is intentionally minimal to act as a smoke-testable surface.
  def parse_complex_input(source)
    source.to_s.split(/\s+/).reject(&:empty?)
  end
end
