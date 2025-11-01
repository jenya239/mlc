# frozen_string_literal: true

require_relative "../test_helper"

class EventBusExtensionsTest < Minitest::Test
  # Test backward compatibility
  def test_backward_compatibility_publish_subscribe
    bus = MLC::EventBus.new
    payloads = []
    bus.subscribe(:demo) { |payload| payloads << payload }

    bus.publish(:demo, { value: 1 })
    bus.publish(:demo, { value: 2 })

    assert_equal [{ value: 1 }, { value: 2 }], payloads
  end

  # Test log levels
  def test_debug_event
    bus = MLC::EventBus.new
    events = []
    bus.subscribe(:test) { |payload| events << payload }

    bus.debug(:test, message: "debug message")

    assert_equal 1, events.size
  end

  def test_info_event
    bus = MLC::EventBus.new
    events = []
    bus.subscribe(:test) { |payload| events << payload }

    bus.info(:test, message: "info message")

    assert_equal 1, events.size
  end

  def test_warning_event
    bus = MLC::EventBus.new
    events = []
    bus.subscribe(:test) { |payload| events << payload }

    bus.warning(:test, message: "warning message")

    assert_equal 1, events.size
  end

  def test_error_event
    bus = MLC::EventBus.new
    events = []
    bus.subscribe(:test) { |payload| events << payload }

    bus.error(:test, message: "error message")

    assert_equal 1, events.size
  end

  # Test bus-level filtering
  def test_bus_level_filtering_info
    bus = MLC::EventBus.new(min_level: :info)
    events = []
    bus.subscribe(:test) { |payload| events << payload }

    bus.debug(:test, message: "debug")  # Filtered out
    bus.info(:test, message: "info")    # Included
    bus.warning(:test, message: "warn") # Included
    bus.error(:test, message: "error")  # Included

    assert_equal 3, events.size
  end

  def test_bus_level_filtering_warning
    bus = MLC::EventBus.new(min_level: :warning)
    events = []
    bus.subscribe(:test) { |payload| events << payload }

    bus.debug(:test, message: "debug")   # Filtered out
    bus.info(:test, message: "info")     # Filtered out
    bus.warning(:test, message: "warn")  # Included
    bus.error(:test, message: "error")   # Included

    assert_equal 2, events.size
  end

  def test_bus_level_filtering_error
    bus = MLC::EventBus.new(min_level: :error)
    events = []
    bus.subscribe(:test) { |payload| events << payload }

    bus.debug(:test, message: "debug")   # Filtered out
    bus.info(:test, message: "info")     # Filtered out
    bus.warning(:test, message: "warn")  # Filtered out
    bus.error(:test, message: "error")   # Included

    assert_equal 1, events.size
  end

  # Test handler-level filtering
  def test_handler_level_filtering
    bus = MLC::EventBus.new(min_level: :debug)
    info_events = []
    error_events = []

    # When using min_level in subscribe, handler receives Event objects
    bus.subscribe(:test, min_level: :info) { |event| info_events << event }
    bus.subscribe(:test, min_level: :error) { |event| error_events << event }

    bus.debug(:test, message: "debug")
    bus.info(:test, message: "info")
    bus.warning(:test, message: "warn")
    bus.error(:test, message: "error")

    # First handler (min_level: :info) should receive info, warning, error (3)
    assert_equal 3, info_events.size

    # Second handler (min_level: :error) should receive only error (1)
    assert_equal 1, error_events.size
  end

  # Test structured events
  def test_structured_event
    bus = MLC::EventBus.new
    events = []
    bus.subscribe(:test) { |event| events << event }

    bus.publish(:test, { value: 42 }, level: :info, structured: true)

    assert_equal 1, events.size
    event = events.first
    assert_instance_of MLC::EventBus::Event, event
    assert_equal :test, event.name
    assert_equal({ value: 42 }, event.payload)
    assert_equal :info, event.level
    assert_instance_of Time, event.timestamp
  end

  # Test handler count
  def test_handler_count
    bus = MLC::EventBus.new
    assert_equal 0, bus.handler_count

    bus.subscribe(:test) { }
    assert_equal 1, bus.handler_count
    assert_equal 1, bus.handler_count(:test)

    bus.subscribe(:test) { }
    assert_equal 2, bus.handler_count
    assert_equal 2, bus.handler_count(:test)

    bus.subscribe(:other) { }
    assert_equal 3, bus.handler_count
    assert_equal 2, bus.handler_count(:test)
    assert_equal 1, bus.handler_count(:other)
  end

  # Test clear handlers
  def test_clear_all_handlers
    bus = MLC::EventBus.new
    bus.subscribe(:test1) { }
    bus.subscribe(:test2) { }

    assert_equal 2, bus.handler_count

    bus.clear_handlers
    assert_equal 0, bus.handler_count
  end

  def test_clear_specific_handler
    bus = MLC::EventBus.new
    bus.subscribe(:test1) { }
    bus.subscribe(:test2) { }

    assert_equal 2, bus.handler_count

    bus.clear_handlers(:test1)
    assert_equal 1, bus.handler_count
    assert_equal 0, bus.handler_count(:test1)
    assert_equal 1, bus.handler_count(:test2)
  end

  # Test error handling in handlers
  def test_handler_errors_dont_break_bus
    bus = MLC::EventBus.new
    success_count = 0

    bus.subscribe(:test) { raise "error" }
    bus.subscribe(:test) { success_count += 1 }

    # Should not raise, second handler should still execute
    assert_output(nil, /EventBus handler error/) do
      bus.publish(:test, {})
    end

    assert_equal 1, success_count
  end

  # Test setting min_level
  def test_set_min_level
    bus = MLC::EventBus.new(min_level: :debug)
    assert_equal :debug, bus.min_level

    bus.min_level = :warning
    assert_equal :warning, bus.min_level

    events = []
    bus.subscribe(:test) { |payload| events << payload }

    bus.info(:test, message: "info")  # Filtered out after level change
    bus.warning(:test, message: "warn")  # Included

    assert_equal 1, events.size
  end

  def test_set_invalid_min_level
    bus = MLC::EventBus.new

    error = assert_raises(ArgumentError) do
      bus.min_level = :invalid
    end

    assert_match(/Unknown level/, error.message)
  end

  # Test Event struct
  def test_event_structure
    event = MLC::EventBus::Event.new(
      name: :test,
      payload: { value: 42 },
      level: :info,
      timestamp: Time.now
    )

    assert_equal :test, event.name
    assert_equal({ value: 42 }, event.payload)
    assert_equal :info, event.level
    assert_instance_of Time, event.timestamp
  end

  # Test multiple subscribers with different filters
  def test_multiple_subscribers_with_filters
    bus = MLC::EventBus.new(min_level: :debug)

    debug_events = []
    info_events = []
    warning_events = []

    # When using min_level in subscribe, handlers receive Event objects
    bus.subscribe(:log, min_level: :debug) { |e| debug_events << e }
    bus.subscribe(:log, min_level: :info) { |e| info_events << e }
    bus.subscribe(:log, min_level: :warning) { |e| warning_events << e }

    bus.debug(:log, msg: "d")
    bus.info(:log, msg: "i")
    bus.warning(:log, msg: "w")
    bus.error(:log, msg: "e")

    assert_equal 4, debug_events.size   # Receives all
    assert_equal 3, info_events.size    # Receives info, warning, error
    assert_equal 2, warning_events.size # Receives warning, error
  end
end
