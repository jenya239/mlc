# frozen_string_literal: true

module MLC
  class ParseError < StandardError; end

  class CompileError < StandardError
    attr_reader :origin

    def initialize(message = nil, origin: nil)
      super(message)
      @origin = origin
    end

    def message
      base = super
      origin_label = origin&.label
      origin_label ? "#{origin_label}: #{base}" : base
    end

    def full_message(highlight: true, order: :top, trace: nil)
      origin_label = origin&.label
      base = super(highlight: highlight, order: order, trace: trace)
      text = origin_label ? "#{origin_label}: #{base}" : base
      highlight ? text : text
    end
  end
end
