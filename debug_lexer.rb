#!/usr/bin/env ruby
# frozen_string_literal: true

require_relative "lib/mlc/parser/lexer"

source = "export fn add(a: i32, b: i32) -> i32"

lexer = MLC::Parser::Lexer.new(source)
tokens = lexer.tokenize

puts "=== Tokens ==="
tokens.each_with_index do |token, idx|
  puts "#{idx}: #{token.type.inspect} => #{token.value.inspect}"
end
