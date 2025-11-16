# frozen_string_literal: true

module MLC
  module Common
    module Diagnostics
      class EventBus
    # Event metadata structure
    Event = Struct.new(:name, :payload, :level, :timestamp, keyword_init: true)

    # Log levels (ordered by severity)
    LEVELS = {
      debug: 0,
      info: 1,
      warning: 2,
      error: 3
    }.freeze

    attr_reader :min_level

    def initialize(min_level: :debug)
      @handlers = Hash.new { |hash, key| hash[key] = [] }
      @min_level = min_level
    end

    # Subscribe to an event
    #
    # @param event [Symbol] Event name
    # @param callable [#call, nil] Optional callable object
    # @param min_level [Symbol, nil] Minimum level to receive (nil = receive all)
    # @yield [payload or Event] Event payload or Event object
    def subscribe(event, callable = nil, min_level: nil, &block)
      handler = callable || block
      raise ArgumentError, "Provide a callable or block" unless handler

      wrapped_handler = if min_level
                          FilteredHandler.new(handler, min_level)
                        else
                          handler
                        end

      @handlers[event.to_sym] << wrapped_handler
    end

    # Publish an event
    #
    # @param event [Symbol] Event name
    # @param payload [Hash] Event payload (can be passed as positional or via **kwargs)
    # @param level [Symbol] Event level (:debug, :info, :warning, :error)
    # @param structured [Boolean] If true, pass Event object to handlers
    def publish(event, payload = nil, level: :info, structured: false, **kwargs)
      # Handle backward compatibility: if payload is nil and kwargs provided,
      # use kwargs as payload (old API: publish(:event, key: value))
      # But if kwargs only contains :level or :structured, treat them as options
      payload_keys = kwargs.keys - [:level, :structured]

      if payload.nil? && !payload_keys.empty?
        # Old API: publish(:event, key: value, ...)
        final_level = kwargs[:level] || level
        final_structured = kwargs[:structured] || structured
        # Extract payload from kwargs (excluding :level and :structured)
        payload_hash = kwargs.reject { |k, _| k == :level || k == :structured }
      else
        # New API: publish(:event, {key: value}, level: :info)
        payload_hash = payload || {}
        final_level = level
        final_structured = structured
      end

      # Filter by bus-level minimum
      return if should_filter_level?(final_level)

      event_obj = Event.new(
        name: event,
        payload: payload_hash,
        level: final_level,
        timestamp: Time.now
      )

      @handlers[event.to_sym].each do |handler|
        begin
          # FilteredHandler always needs Event object to check level
          if handler.is_a?(FilteredHandler)
            handler.call(event_obj)
          else
            # Backward compatibility: pass only payload by default
            handler.call(final_structured ? event_obj : payload_hash)
          end
        rescue => e
          # Don't let handler errors break the event bus
          warn "EventBus handler error for #{event}: #{e.message}"
        end
      end
    end

    # Publish a debug event
    def debug(event, payload = nil, **kwargs)
      publish(event, payload, level: :debug, **kwargs)
    end

    # Publish an info event
    def info(event, payload = nil, **kwargs)
      publish(event, payload, level: :info, **kwargs)
    end

    # Publish a warning event
    def warning(event, payload = nil, **kwargs)
      publish(event, payload, level: :warning, **kwargs)
    end

    # Publish an error event
    def error(event, payload = nil, **kwargs)
      publish(event, payload, level: :error, **kwargs)
    end

    # Set minimum level for this bus
    def min_level=(level)
      raise ArgumentError, "Unknown level: #{level}" unless LEVELS.key?(level)
      @min_level = level
    end

    # Get event count (for testing/metrics)
    def handler_count(event = nil)
      if event
        @handlers[event.to_sym].size
      else
        @handlers.values.sum(&:size)
      end
    end

    # Clear all handlers (useful for testing)
    def clear_handlers(event = nil)
      if event
        @handlers.delete(event.to_sym)
      else
        @handlers.clear
      end
    end

    private

    def should_filter_level?(level)
      level_value = LEVELS[level] || LEVELS[:info]
      min_level_value = LEVELS[@min_level] || LEVELS[:debug]
      level_value < min_level_value
    end

    # Internal wrapper for handlers with level filtering
    class FilteredHandler
      attr_reader :handler, :min_level

      def initialize(handler, min_level)
        @handler = handler
        @min_level = min_level
      end

      def call(event_or_payload)
        # If it's an Event object, check level
        if event_or_payload.is_a?(Event)
          level_value = LEVELS[event_or_payload.level] || LEVELS[:info]
          min_level_value = LEVELS[@min_level] || LEVELS[:debug]
          return if level_value < min_level_value
        end

        @handler.call(event_or_payload)
      end
    end
      end
    end
  end
end
