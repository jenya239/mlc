# frozen_string_literal: true

require_relative "../../../test_helper"
require "stringio"
require "json"
require "time"

class StructuredLoggerTest < Minitest::Test
  def test_log_event_text_format
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :text, color: :never)

    event = MLC::Infrastructure::EventBus::Event.new(
      name: :test_event,
      payload: { key: "value" },
      level: :info,
      timestamp: Time.parse("2025-01-01 12:00:00")
    )

    logger.log_event(:test_event, event)

    output = io.string
    assert_match(/2025-01-01 12:00:00/, output)
    assert_match(/INFO/, output)
    assert_match(/test_event/, output)
    assert_match(/key="value"/, output)
  end

  def test_log_event_json_format
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :json)

    event = MLC::Infrastructure::EventBus::Event.new(
      name: :test_event,
      payload: { key: "value" },
      level: :info,
      timestamp: Time.parse("2025-01-01T12:00:00Z")
    )

    logger.log_event(:test_event, event)

    output = io.string.strip
    parsed = JSON.parse(output)

    assert_equal "info", parsed["level"]
    assert_equal "test_event", parsed["event"]
    assert_equal({ "key" => "value" }, parsed["payload"])
    assert parsed["timestamp"]
  end

  def test_log_event_compact_format
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :compact, color: :never)

    event = MLC::Infrastructure::EventBus::Event.new(
      name: :test_event,
      payload: { key: "value" },
      level: :warning,
      timestamp: Time.now
    )

    logger.log_event(:test_event, event)

    output = io.string
    assert_match(/W/, output)  # Short level indicator
    assert_match(/test_event/, output)
    assert_match(/key="value"/, output)
  end

  def test_attach_to_event_bus
    io = StringIO.new
    bus = MLC::Infrastructure::EventBus.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :compact, color: :never)

    logger.attach(bus, events: [:custom_event])

    bus.info(:custom_event, message: "test")

    output = io.string
    assert_match(/custom_event/, output)
    assert_match(/message="test"/, output)
  end

  def test_attach_with_min_level_filtering
    io = StringIO.new
    bus = MLC::Infrastructure::EventBus.new(min_level: :debug)
    logger = MLC::Diagnostics::StructuredLogger.new(
      io: io,
      format: :compact,
      color: :never,
      min_level: :warning
    )

    logger.attach(bus, events: [:test])

    bus.debug(:test, msg: "debug")    # Filtered by logger
    bus.info(:test, msg: "info")      # Filtered by logger
    bus.warning(:test, msg: "warn")   # Logged
    bus.error(:test, msg: "error")    # Logged

    lines = io.string.split("\n")
    assert_equal 2, lines.size
  end

  def test_format_value_string
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :text, color: :never)

    event = MLC::Infrastructure::EventBus::Event.new(
      name: :test,
      payload: { str: "hello world" },
      level: :info,
      timestamp: Time.now
    )

    logger.log_event(:test, event)

    output = io.string
    assert_match(/str="hello world"/, output)
  end

  def test_format_value_symbol
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :text, color: :never)

    event = MLC::Infrastructure::EventBus::Event.new(
      name: :test,
      payload: { sym: :value },
      level: :info,
      timestamp: Time.now
    )

    logger.log_event(:test, event)

    output = io.string
    assert_match(/sym=:value/, output)
  end

  def test_format_value_array
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :text, color: :never)

    event = MLC::Infrastructure::EventBus::Event.new(
      name: :test,
      payload: { arr: [1, 2, 3] },
      level: :info,
      timestamp: Time.now
    )

    logger.log_event(:test, event)

    output = io.string
    assert_match(/arr=\[1, 2, 3\]/, output)
  end

  def test_format_value_hash
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :text, color: :never)

    event = MLC::Infrastructure::EventBus::Event.new(
      name: :test,
      payload: { obj: { key: "value" } },
      level: :info,
      timestamp: Time.now
    )

    logger.log_event(:test, event)

    output = io.string
    assert_match(/obj=\{key: "value"\}/, output)
  end

  def test_compact_format_array_shows_size
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :compact, color: :never)

    event = MLC::Infrastructure::EventBus::Event.new(
      name: :test,
      payload: { arr: [1, 2, 3, 4, 5] },
      level: :info,
      timestamp: Time.now
    )

    logger.log_event(:test, event)

    output = io.string
    assert_match(/arr=\[5\]/, output)  # Shows array size in compact format
  end

  def test_compact_format_hash_shows_size
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :compact, color: :never)

    event = MLC::Infrastructure::EventBus::Event.new(
      name: :test,
      payload: { obj: { a: 1, b: 2, c: 3 } },
      level: :info,
      timestamp: Time.now
    )

    logger.log_event(:test, event)

    output = io.string
    assert_match(/obj=\{3\}/, output)  # Shows hash size in compact format
  end

  def test_empty_payload
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :text, color: :never)

    event = MLC::Infrastructure::EventBus::Event.new(
      name: :test,
      payload: {},
      level: :info,
      timestamp: Time.now
    )

    logger.log_event(:test, event)

    output = io.string
    assert_match(/test/, output)
    # Should not have any key=value pairs
    refute_match(/=/, output)
  end

  def test_color_enabled
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :text, color: :always)

    assert logger.color_enabled
  end

  def test_color_disabled
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :text, color: :never)

    refute logger.color_enabled
  end

  def test_log_event_with_plain_payload
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :text, color: :never)

    # When passing a plain hash (not an Event object)
    logger.log_event(:test, { message: "plain" })

    output = io.string
    assert_match(/test/, output)
    assert_match(/message="plain"/, output)
  end

  def test_all_log_levels
    io = StringIO.new
    logger = MLC::Diagnostics::StructuredLogger.new(io: io, format: :text, color: :never)

    [:debug, :info, :warning, :error].each do |level|
      event = MLC::Infrastructure::EventBus::Event.new(
        name: :test,
        payload: { level: level },
        level: level,
        timestamp: Time.now
      )

      logger.log_event(:test, event)
    end

    output = io.string
    assert_match(/DEBUG/, output)
    assert_match(/INFO/, output)
    assert_match(/WARNING/, output)
    assert_match(/ERROR/, output)
  end
end
