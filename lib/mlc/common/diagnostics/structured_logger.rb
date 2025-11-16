# frozen_string_literal: true

require "json"

module MLC
  module Common
    module Diagnostics
      # Structured logger for EventBus with JSON output support
      class StructuredLogger
      LEVEL_COLORS = {
        debug: "\e[36m",    # Cyan
        info: "\e[32m",     # Green
        warning: "\e[33m",  # Yellow
        error: "\e[31m"     # Red
      }.freeze

      RESET = "\e[0m"
      BOLD = "\e[1m"

      attr_reader :format, :color_enabled

      def initialize(io: $stderr, format: :text, color: :auto, min_level: :debug)
        @io = io
        @format = format
        @color_enabled = determine_color_support(color)
        @min_level = min_level
      end

      # Attach this logger to an EventBus
      #
      # @param bus [EventBus] The event bus to attach to
      # @param events [Array<Symbol>, :all] Events to log (:all for all events)
      # @return [void]
      def attach(bus, events: :all)
        if events == :all
          # Subscribe to a wildcard (we'll need to manually track all published events)
          # For now, attach to common events
          attach_to_common_events(bus)
        else
          events.each do |event|
            bus.subscribe(event, min_level: @min_level) do |payload|
              log_event(event, payload)
            end
          end
        end
      end

      # Log a single event
      def log_event(event_name, payload_or_event)
        event_obj = if payload_or_event.is_a?(EventBus::Event)
                      payload_or_event
                    else
                      EventBus::Event.new(
                        name: event_name,
                        payload: payload_or_event,
                        level: :info,
                        timestamp: Time.now
                      )
                    end

        case @format
        when :json
          @io.puts format_json(event_obj)
        when :text
          @io.puts format_text(event_obj)
        when :compact
          @io.puts format_compact(event_obj)
        else
          @io.puts format_text(event_obj)
        end
      end

      private

      def attach_to_common_events(bus)
        # Common compiler events
        common_events = %i[
          pipeline_start
          pipeline_end
          pass_start
          pass_end
          pass_error
          type_check_start
          type_check_end
          transform_start
          transform_end
          stdlib_function_imported
          stdlib_type_imported
          stdlib_missing_item
          type_mismatch
          cpp_function_rule_applied
          cpp_function_lowered
        ]

        common_events.each do |event|
          bus.subscribe(event, min_level: @min_level) do |payload|
            log_event(event, payload)
          end
        end
      end

      def determine_color_support(color_option)
        case color_option
        when :auto
          @io.tty? && (ENV["TERM"] && ENV["TERM"] != "dumb")
        when true, :always
          true
        when false, :never
          false
        else
          false
        end
      end

      def format_json(event)
        data = {
          timestamp: event.timestamp.iso8601(3),
          level: event.level,
          event: event.name,
          payload: event.payload
        }
        JSON.generate(data)
      end

      def format_text(event)
        timestamp = event.timestamp.strftime("%Y-%m-%d %H:%M:%S")
        level_str = format_level_label(event.level)
        event_name = colorize(event.name.to_s, :bold)

        parts = ["[#{timestamp}]", level_str, event_name]

        # Format payload
        unless event.payload.empty?
          payload_str = event.payload.map { |k, v| "#{k}=#{format_value(v)}" }.join(" ")
          parts << payload_str
        end

        parts.join(" ")
      end

      def format_compact(event)
        level_str = format_level_label(event.level, short: true)
        event_name = colorize(event.name.to_s, :bold)

        parts = [level_str, event_name]

        # Format payload (compact)
        unless event.payload.empty?
          payload_str = event.payload.map { |k, v| "#{k}=#{format_value(v, compact: true)}" }.join(" ")
          parts << payload_str
        end

        parts.join(" ")
      end

      def format_level_label(level, short: false)
        label = short ? level.to_s[0].upcase : level.to_s.upcase.ljust(7)
        color = LEVEL_COLORS[level] || LEVEL_COLORS[:info]
        colorize(label, color, :bold)
      end

      def format_value(value, compact: false)
        case value
        when String
          compact ? value.inspect : value.inspect
        when Symbol
          ":#{value}"
        when Array
          compact ? "[#{value.size}]" : value.inspect
        when Hash
          compact ? "{#{value.size}}" : value.inspect
        else
          value.inspect
        end
      end

      def colorize(text, *codes)
        return text unless @color_enabled

        codes_str = codes.map { |code|
          code.is_a?(Symbol) ? const_get(code.upcase) : code
        }.join

        "#{codes_str}#{text}#{RESET}"
      end
      end
    end
  end
end
