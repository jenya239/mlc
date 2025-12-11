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
    expect { parser.parse(nil) }.to raise_error(ArgumentError)
    expect { parser.parse_complex_input(nil) }.to raise_error(ArgumentError)
  end
end
