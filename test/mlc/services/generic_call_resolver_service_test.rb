# frozen_string_literal: true

require_relative "../../test_helper"
require "ostruct"

class GenericCallResolverServiceTest < Minitest::Test
  Info = Struct.new(:name, :type_params, :param_types, :ret_type)

  def setup
    # Mock transformer with necessary methods
    @transformer = Object.new

    # Mock generic_call_resolver that returns Instantiation
    @instantiation = MLC::TypeSystem::TypeConstraintSolver::Instantiation.new(
      { "T" => "i32" },  # type_map
      ["i32"],            # param_types
      "i32"               # ret_type
    )

    @mock_resolver = Object.new
    mock_inst = @instantiation  # Capture for closure
    @mock_resolver.define_singleton_method(:instantiate) do |_info, _args, name:|
      mock_inst
    end

    mock_resolver = @mock_resolver  # Capture for closure
    @transformer.define_singleton_method(:generic_call_resolver) do
      mock_resolver
    end

    @service = MLC::Services::GenericCallResolverService.new(@transformer)
  end

  def test_instantiate_delegates_to_transformer_resolver
    type_param = OpenStruct.new(name: "T")
    info = Info.new("unwrap", [type_param], ["Option<T>", "T"], "T")
    args = [
      OpenStruct.new(type: "Option<i32>"),
      OpenStruct.new(type: "i32")
    ]

    result = @service.instantiate(info, args, name: "unwrap")

    assert_instance_of MLC::TypeSystem::TypeConstraintSolver::Instantiation, result
    assert_equal ["i32"], result.param_types
    assert_equal "i32", result.ret_type
  end

  def test_generic_function_with_type_params
    type_param = OpenStruct.new(name: "T")
    info = Info.new("identity", [type_param], ["T"], "T")

    assert @service.generic_function?(info)
  end

  def test_generic_function_without_type_params
    info = Info.new("add", [], ["i32", "i32"], "i32")

    refute @service.generic_function?(info)
  end

  def test_generic_function_with_empty_type_params
    info = Info.new("noop", [], [], "void")

    refute @service.generic_function?(info)
  end

  def test_type_map_extraction
    instantiation = MLC::TypeSystem::TypeConstraintSolver::Instantiation.new(
      { "T" => "i32", "U" => "str" },
      ["i32", "str"],
      "bool"
    )

    type_map = @service.type_map(instantiation)

    assert_equal({ "T" => "i32", "U" => "str" }, type_map)
  end

  def test_param_types_extraction
    instantiation = MLC::TypeSystem::TypeConstraintSolver::Instantiation.new(
      { "T" => "i32" },
      ["i32", "i32"],
      "i32"
    )

    params = @service.param_types(instantiation)

    assert_equal ["i32", "i32"], params
  end

  def test_return_type_extraction
    instantiation = MLC::TypeSystem::TypeConstraintSolver::Instantiation.new(
      { "T" => "str" },
      ["str"],
      "str"
    )

    ret_type = @service.return_type(instantiation)

    assert_equal "str", ret_type
  end

  def test_service_can_be_used_from_context
    # Simulate how rules would use the service
    context = {
      generic_call_resolver: @service
    }

    resolver = context[:generic_call_resolver]
    type_param = OpenStruct.new(name: "T")
    info = Info.new("map", [type_param], ["T"], "T")
    args = [OpenStruct.new(type: "i32")]

    result = resolver.instantiate(info, args, name: "map")

    assert_instance_of MLC::TypeSystem::TypeConstraintSolver::Instantiation, result
  end

  def test_service_integration_with_real_transformer
    # Test with actual IRGen transformer
    bus = MLC::EventBus.new(min_level: :error)
    transformer = MLC::IRGen.new(event_bus: bus)
    service = MLC::Services::GenericCallResolverService.new(transformer)

    # Create a generic function info
    type_param = OpenStruct.new(name: "T", constraint: nil)
    info = MLC::IRGen::FunctionInfo.new(
      "identity",
      [MLC::HighIR::TypeVariable.new(name: "T")],
      MLC::HighIR::TypeVariable.new(name: "T"),
      [type_param]
    )

    # Create args with concrete types
    arg_type = MLC::HighIR::Builder.primitive_type("i32")
    args = [MLC::HighIR::Builder.literal(42, arg_type)]

    # Instantiate should work
    result = service.instantiate(info, args, name: "identity")

    assert_instance_of MLC::TypeSystem::TypeConstraintSolver::Instantiation, result
    assert result.ret_type
  end

  def test_generic_function_detection_with_real_info
    type_param = OpenStruct.new(name: "T", constraint: nil)
    info = MLC::IRGen::FunctionInfo.new(
      "map",
      [MLC::HighIR::TypeVariable.new(name: "T")],
      MLC::HighIR::ArrayType.new(element_type: MLC::HighIR::TypeVariable.new(name: "T")),
      [type_param]
    )

    # Service should correctly identify this as a generic function
    bus = MLC::EventBus.new(min_level: :error)
    transformer = MLC::IRGen.new(event_bus: bus)
    service = MLC::Services::GenericCallResolverService.new(transformer)

    assert service.generic_function?(info)
  end

  def test_non_generic_function_detection_with_real_info
    info = MLC::IRGen::FunctionInfo.new(
      "add",
      [MLC::HighIR::Builder.primitive_type("i32"), MLC::HighIR::Builder.primitive_type("i32")],
      MLC::HighIR::Builder.primitive_type("i32"),
      []
    )

    bus = MLC::EventBus.new(min_level: :error)
    transformer = MLC::IRGen.new(event_bus: bus)
    service = MLC::Services::GenericCallResolverService.new(transformer)

    refute service.generic_function?(info)
  end
end
