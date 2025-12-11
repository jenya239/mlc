# frozen_string_literal: true

require "parser"

RSpec.describe Parser do
  it "returns the input source as a string" do
    parser = described_class.new
    expect(parser.parse("hello")).to eq("hello")
  end

  it "splits complex input into tokens" do
    parser = described_class.new
    expect(parser.parse_complex_input("one two  three")).to eq(%w[one two three])
  end

  it "raises on nil input" do
    parser = described_class.new
    expect { parser.parse(nil) }.to raise_error(Parser::InvalidInputError)
    expect { parser.parse_complex_input(nil) }.to raise_error(Parser::InvalidInputError)
  end

  it "raises on non-stringable input" do
    parser = described_class.new
    bad_obj = Object.new
    def bad_obj.to_s; raise NoMethodError; end

    expect { parser.parse(bad_obj) }.to raise_error(Parser::InvalidInputError)
    expect { parser.parse_complex_input(bad_obj) }.to raise_error(Parser::InvalidInputError)
  end

  it "returns empty array for empty complex input" do
    parser = described_class.new
    expect(parser.parse_complex_input("")).to eq([])
  end
end
