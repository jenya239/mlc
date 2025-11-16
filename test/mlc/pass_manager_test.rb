# frozen_string_literal: true

require_relative "../test_helper"

class PassManagerTest < Minitest::Test
  # Basic functionality tests (backward compatibility)

  def test_runs_passes_in_order
    manager = MLC::Common::Analysis::PassManager.new
    trace = []

    manager.register(:first) do |context|
      trace << :first
      context[:count] = 1
    end

    manager.register(:second) do |context|
      trace << :second
      context[:count] += 1
    end

    context = manager.run({})

    assert_equal [:first, :second], trace
    assert_equal 2, context[:count]
  end

  def test_accepts_callable_objects
    manager = MLC::Common::Analysis::PassManager.new
    accumulator = Class.new do
      attr_reader :calls
      def initialize
        @calls = 0
      end
      def call(context)
        @calls += 1
        context[:calls] = @calls
      end
    end.new

    manager.register(:callable, accumulator)
    context = manager.run({})

    assert_equal 1, accumulator.calls
    assert_equal 1, context[:calls]
  end

  def test_bubbles_up_pass_errors_with_context
    manager = MLC::Common::Analysis::PassManager.new
    manager.register(:boom) { |_ctx| raise RuntimeError, "failure" }

    error = assert_raises(RuntimeError) { manager.run({}) }
    assert_match(/Pass boom failed: failure/, error.message)
  end

  # BasePass integration tests

  def test_accepts_base_pass_objects
    manager = MLC::Common::Analysis::PassManager.new
    pass = create_test_pass(:test_pass, [:input], [:output])

    manager.register(:my_pass, pass)
    context = manager.run({ input: "data" })

    assert_equal "processed", context[:output]
  end

  def test_extracts_metadata_from_base_pass
    manager = MLC::Common::Analysis::PassManager.new
    pass = create_test_pass(:test_pass, [:input], [:output])

    manager.register(:my_pass, pass)
    metadata = manager.metadata(:my_pass)

    assert_equal :semantic_ir, metadata[:input_level]
    assert_equal :semantic_ir, metadata[:output_level]
    assert_equal [:input], metadata[:required_keys]
    assert_equal [:output], metadata[:produced_keys]
  end

  def test_validates_required_keys_for_base_pass
    manager = MLC::Common::Analysis::PassManager.new(validate: true)
    pass = create_test_pass(:test_pass, [:input, :config], [:output])

    manager.register(:my_pass, pass)

    # Should fail with missing keys
    error = assert_raises(ArgumentError) { manager.run({}) }
    assert_match(/missing required context keys/, error.message)
    assert_match(/:input/, error.message)
    assert_match(/:config/, error.message)
  end

  def test_passes_with_all_required_keys
    manager = MLC::Common::Analysis::PassManager.new(validate: true)
    pass = create_test_pass(:test_pass, [:input], [:output])

    manager.register(:my_pass, pass)
    context = manager.run({ input: "data" })

    assert_equal "processed", context[:output]
  end

  def test_validation_can_be_disabled
    manager = MLC::Common::Analysis::PassManager.new(validate: false)
    pass = create_test_pass(:test_pass, [:input], [:output])

    manager.register(:my_pass, pass)

    # Should not raise even with missing keys when validation is off
    context = manager.run({})
    assert context
  end

  # EventBus integration tests

  def test_publishes_events_to_event_bus
    event_bus = MLC::Common::Diagnostics::EventBus.new
    events = []
    event_bus.subscribe(:pipeline_start) { |payload| events << [:pipeline_start, payload] }
    event_bus.subscribe(:pass_start) { |payload| events << [:pass_start, payload] }
    event_bus.subscribe(:pass_end) { |payload| events << [:pass_end, payload] }
    event_bus.subscribe(:pipeline_end) { |payload| events << [:pipeline_end, payload] }

    manager = MLC::Common::Analysis::PassManager.new(event_bus: event_bus)
    manager.register(:test) { |ctx| ctx[:done] = true }
    manager.run({})

    assert_equal 4, events.size
    assert_equal :pipeline_start, events[0][0]
    assert_equal :pass_start, events[1][0]
    assert_equal :pass_end, events[2][0]
    assert_equal :pipeline_end, events[3][0]
  end

  def test_includes_duration_in_pass_end_event
    event_bus = MLC::Common::Diagnostics::EventBus.new
    duration = nil
    event_bus.subscribe(:pass_end) { |payload| duration = payload[:duration] }

    manager = MLC::Common::Analysis::PassManager.new(event_bus: event_bus)
    manager.register(:slow_pass) { |ctx| sleep 0.01 }
    manager.run({})

    assert duration
    assert duration > 0
  end

  # Metadata tests

  def test_returns_nil_for_unknown_pass_metadata
    manager = MLC::Common::Analysis::PassManager.new
    assert_nil manager.metadata(:nonexistent)
  end

  def test_stores_custom_metadata_for_non_base_pass
    manager = MLC::Common::Analysis::PassManager.new
    manager.register(:custom, ->(_ctx) {}, metadata: { author: "test", version: "1.0" })

    metadata = manager.metadata(:custom)
    assert_equal "test", metadata[:author]
    assert_equal "1.0", metadata[:version]
  end

  private

  def create_test_pass(name, required_keys, produced_keys)
    Class.new(MLC::Common::Analysis::BasePass) do
      define_method(:initialize) do
        super(name: name)
        @required = required_keys
        @produced = produced_keys
      end

      define_method(:required_keys) { @required }
      define_method(:produced_keys) { @produced }

      define_method(:run) do |context|
        context[:output] = "processed"
      end
    end.new
  end
end
