# frozen_string_literal: true

module MLC
  module Common
    module Analysis
      class PassManager
    # Internal representation of a registered pass
    Pass = Struct.new(:name, :callable, :metadata, keyword_init: true) do
      def base_pass?
        metadata.key?(:input_level)
      end

      def transformation?
        metadata[:transformation] == true
      end
    end

    attr_reader :event_bus

    def initialize(event_bus: nil, validate: true, verbose: false)
      @passes = []
      @event_bus = event_bus
      @validate = validate
      @verbose = verbose
    end

    # Register a pass (supports both BasePass objects and callable/blocks)
    #
    # @param name [Symbol] Pass name
    # @param callable [#call, Analysis::BasePass] Pass object or callable
    # @param metadata [Hash] Optional metadata for non-BasePass callables
    # @yield [context] Block to execute as pass
    def register(name, callable = nil, metadata: {}, &block)
      raise ArgumentError, "Provide a callable or a block" if callable.nil? && block.nil?
      raise ArgumentError, "Only one of callable or block is allowed" if callable && block

      pass_object = callable || block

      # Extract metadata before wrapping (needs the original object)
      pass_metadata = extract_metadata(pass_object, metadata)

      # Wrap BasePass objects to provide call interface
      pass_callable = if pass_object.respond_to?(:input_level)
                        # BasePass object - wrap its run method
                        ->(context) { pass_object.run(context) }
                      else
                        # Already callable (block or lambda)
                        pass_object
                      end

      @passes << Pass.new(
        name: name.to_sym,
        callable: pass_callable,
        metadata: pass_metadata
      )
    end

    # Run all registered passes in order
    #
    # @param context [Hash] Shared context for all passes
    # @return [Hash] Updated context
    def run(context = {})
      log_pipeline_start(context)

      @passes.each_with_index do |pass, index|
        run_pass(pass, context, index)
      end

      log_pipeline_end(context)
      context
    end

    # Get list of registered pass names
    #
    # @return [Array<Symbol>] Pass names
    def passes
      @passes.map(&:name)
    end

    # Get pass metadata
    #
    # @param name [Symbol] Pass name
    # @return [Hash, nil] Pass metadata
    def metadata(name)
      pass = @passes.find { |p| p.name == name.to_sym }
      pass&.metadata
    end

    private

    def run_pass(pass, context, index)
      log_pass_start(pass, context, index)

      validate_required_keys!(pass, context) if @validate && pass.base_pass?

      start_time = Time.now
      pass.callable.call(context)
      duration = Time.now - start_time

      log_pass_end(pass, context, duration)
      mark_produced_keys(pass, context) if pass.base_pass?
    rescue StandardError => e
      log_pass_error(pass, e)
      raise e.class, "Pass #{pass.name} failed: #{e.message}", e.backtrace
    end

    def extract_metadata(callable, provided_metadata)
      return provided_metadata unless callable.respond_to?(:input_level)

      # Extract metadata from BasePass
      {
        input_level: callable.input_level,
        output_level: callable.output_level,
        required_keys: callable.required_keys,
        produced_keys: callable.produced_keys,
        transformation: callable.transformation?
      }.merge(provided_metadata)
    end

    def validate_required_keys!(pass, context)
      return unless pass.metadata[:required_keys]

      missing = pass.metadata[:required_keys] - context.keys
      return if missing.empty?

      raise ArgumentError,
            "Pass #{pass.name} missing required context keys: #{missing.inspect}"
    end

    def mark_produced_keys(pass, context)
      return unless pass.metadata[:produced_keys]

      pass.metadata[:produced_keys].each do |key|
        context["__produced_by_#{key}".to_sym] = pass.name
      end
    end

    # Logging methods

    def log_pipeline_start(context)
      return unless @event_bus || @verbose

      message = "PassManager pipeline started (#{@passes.size} passes)"
      log(:pipeline_start, message: message, passes: passes, context_keys: context.keys)
    end

    def log_pipeline_end(context)
      return unless @event_bus || @verbose

      message = "PassManager pipeline completed"
      log(:pipeline_end, message: message, context_keys: context.keys)
    end

    def log_pass_start(pass, context, index)
      return unless @event_bus || @verbose

      message = "[#{index + 1}/#{@passes.size}] Running pass: #{pass.name}"
      if pass.base_pass?
        message += " (#{pass.metadata[:input_level]} → #{pass.metadata[:output_level]})"
      end

      log(:pass_start, message: message, pass: pass.name, metadata: pass.metadata)
    end

    def log_pass_end(pass, context, duration)
      return unless @event_bus || @verbose

      message = "Completed pass: #{pass.name} (#{(duration * 1000).round(2)}ms)"
      log(:pass_end, message: message, pass: pass.name, duration: duration)
    end

    def log_pass_error(pass, error)
      return unless @event_bus || @verbose

      message = "Pass #{pass.name} failed: #{error.message}"
      log(:pass_error, message: message, pass: pass.name, error: error.class.name)
    end

    def log(event, payload)
      if @event_bus
        @event_bus.publish(event, payload)
      elsif @verbose
        puts "[PassManager] #{payload[:message]}"
      end
    end
      end
    end
  end
end
