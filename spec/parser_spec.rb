# frozen_string_literal: true

require "parser"

RSpec.describe Parser do
  it "returns the input source as a string" do
    parser = described_class.new
    expect(parser.parse("hello")).to eq("hello")
  end
end

