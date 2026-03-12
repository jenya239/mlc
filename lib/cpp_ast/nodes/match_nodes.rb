# frozen_string_literal: true

# rubocop:disable Lint/MissingSuper

module CppAst
  module Nodes
    # Match expression - generates std::visit with overloaded
    class MatchExpression < Expression
      attr_accessor :value, :arms, :arm_separators

      def initialize(value:, arms:, arm_separators: [])
        @value = value
        @arms = arms
        @arm_separators = arm_separators
      end

      def to_source
        result = +"std::visit(overloaded{\n"

        arms.each_with_index do |arm, i|
          result << "  " << arm.to_source # добавить отступ
          result << (i < arms.length - 1 ? ",\n" : "")
        end

        result << "\n}, "
        value_str = value.respond_to?(:to_source) ? value.to_source : value.to_s
        result << value_str << ")"
        result
      end
    end

    # Match arm - generates lambda for one case
    class MatchArm < Node
      attr_accessor :case_name, :bindings, :body, :cpp_param_type

      def initialize(case_name:, body:, bindings: [], cpp_param_type: nil)
        @case_name = case_name
        @bindings = bindings
        @body = body
        @cpp_param_type = cpp_param_type
      end

      CPP_KEYWORDS = %w[
        alignas alignof and and_eq asm auto bitand bitor bool break case catch char char8_t
        char16_t char32_t class compl concept const consteval constexpr constinit const_cast
        continue co_await co_return co_yield decltype default delete do double dynamic_cast
        else enum explicit export extern false float for friend goto if inline int long
        mutable namespace new noexcept not not_eq nullptr operator or or_eq private protected
        public register reinterpret_cast requires return short signed sizeof static static_assert
        static_cast struct switch template this thread_local throw true try typedef typeid
        typename union unsigned using virtual void volatile wchar_t while xor xor_eq
      ].freeze

      def to_source
        param_type = cpp_param_type || case_name
        raw_name = case_name.downcase
        var_name = CPP_KEYWORDS.include?(raw_name) ? "#{raw_name}_" : raw_name
        result = +"[&](const #{param_type}& #{var_name}) { "

        binding_items = Array(bindings).compact

        if binding_items.any?
          binding_list = binding_items.map { |b| b.respond_to?(:to_source) ? b.to_source : b.to_s }.join(", ")
          result << "auto [#{binding_list}] = #{var_name}; "
        end

        body_str = body.respond_to?(:to_source) ? body.to_source : body.to_s
        result << "return #{body_str}; }"
        result
      end
    end

    # Wildcard match arm - generates generic lambda for catch-all cases
    class WildcardMatchArm < Node
      attr_accessor :var_name, :body

      def initialize(var_name:, body:)
        @var_name = var_name
        @body = body
      end

      def to_source
        body_str = body.respond_to?(:to_source) ? body.to_source : body.to_s
        "[&](const auto& #{var_name}) { return #{body_str}; }"
      end
    end

    # Match statement - generates std::visit visitor returning void
    class MatchStatement < Statement
      attr_accessor :value, :arms, :arm_separators

      def initialize(value:, arms:, arm_separators: [])
        super()
        @value = value
        @arms = arms
        @arm_separators = arm_separators
      end

      def to_source
        result = +"std::visit(overloaded{\n"

        arms.each_with_index do |arm, index|
          result << "  " << arm.to_source
          result << (index < arms.length - 1 ? ",\n" : "")
        end

        result << "\n}, #{value.to_source});"
        result
      end
    end

    class MatchArmStatement < Node
      attr_accessor :case_name, :var_name, :body

      def initialize(case_name:, var_name:, body:)
        @case_name = case_name
        @var_name = var_name
        @body = body
      end

      def to_source
        "[&](const #{case_name}& #{var_name}) #{body.to_source}"
      end
    end

    class WildcardMatchArmStatement < Node
      attr_accessor :var_name, :body

      def initialize(var_name:, body:)
        @var_name = var_name
        @body = body
      end

      def to_source
        "[&](const auto& #{var_name}) #{body.to_source}"
      end
    end
  end
end
# rubocop:enable Lint/MissingSuper
