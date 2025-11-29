# frozen_string_literal: true

require_relative "../../../../../test_helper"

# Explicitly require the service
require_relative "../../../../../../lib/mlc/representations/semantic/gen/services/scope/var_type_registry"

class VarTypeRegistryTest < Minitest::Test
  def setup
    @registry = MLC::Representations::Semantic::Gen::Services::VarTypeRegistry.new
  end

  # ========== Basic operations ==========

  def test_set_and_get
    type = int_type

    @registry.set("x", type)

    assert_equal type, @registry.get("x")
  end

  def test_get_returns_nil_for_unknown
    assert_nil @registry.get("unknown")
  end

  def test_has_returns_true_for_existing
    @registry.set("x", int_type)

    assert @registry.has?("x")
  end

  def test_has_returns_false_for_unknown
    refute @registry.has?("unknown")
  end

  def test_delete_removes_variable
    @registry.set("x", int_type)

    deleted = @registry.delete("x")

    assert_equal int_type.name, deleted.name
    refute @registry.has?("x")
  end

  def test_delete_returns_nil_for_unknown
    assert_nil @registry.delete("unknown")
  end

  def test_keys_returns_all_variable_names
    @registry.set("a", int_type)
    @registry.set("b", string_type)
    @registry.set("c", bool_type)

    keys = @registry.keys

    assert_includes keys, "a"
    assert_includes keys, "b"
    assert_includes keys, "c"
    assert_equal 3, keys.size
  end

  def test_clear_removes_all_variables
    @registry.set("x", int_type)
    @registry.set("y", string_type)

    @registry.clear

    assert @registry.empty?
    refute @registry.has?("x")
    refute @registry.has?("y")
  end

  def test_size_returns_variable_count
    assert_equal 0, @registry.size

    @registry.set("x", int_type)
    assert_equal 1, @registry.size

    @registry.set("y", string_type)
    assert_equal 2, @registry.size
  end

  def test_empty_returns_true_when_no_variables
    assert @registry.empty?

    @registry.set("x", int_type)
    refute @registry.empty?
  end

  # ========== Update type ==========

  def test_update_type_changes_existing_variable
    @registry.set("x", int_type)

    @registry.update_type("x", string_type)

    assert_equal "string", @registry.get("x").name
  end

  def test_update_type_with_nil_does_nothing
    @registry.set("x", int_type)

    @registry.update_type(nil, string_type)
    @registry.update_type("x", nil)

    assert_equal "i32", @registry.get("x").name
  end

  # ========== Initializer tracking ==========

  def test_set_with_initializer
    init_value = MLC::SemanticIR::LiteralExpr.new(value: 42, type: int_type)

    @registry.set("x", int_type, initializer: init_value)

    assert_equal init_value, @registry.initializer("x")
  end

  def test_initializer_returns_nil_when_not_set
    @registry.set("x", int_type)

    assert_nil @registry.initializer("x")
  end

  # ========== Snapshot and restore ==========

  def test_snapshot_creates_copy
    @registry.set("x", int_type)

    snapshot = @registry.snapshot

    assert_instance_of MLC::Representations::Semantic::Gen::Services::VarTypeRegistry::Snapshot, snapshot
    assert_equal "i32", snapshot["x"].name
  end

  def test_restore_resets_to_snapshot_state
    @registry.set("x", int_type)
    snapshot = @registry.snapshot

    @registry.set("y", string_type)
    @registry.update_type("x", bool_type)

    @registry.restore(snapshot)

    assert @registry.has?("x")
    refute @registry.has?("y")
    assert_equal "i32", @registry.get("x").name
  end

  def test_restore_with_nil_raises_error
    assert_raises(ArgumentError) do
      @registry.restore(nil)
    end
  end

  def test_restore_from_plain_hash
    @registry.set("x", int_type)

    # Simulate legacy hash snapshot
    legacy_snapshot = { "x" => int_type, "y" => string_type }

    @registry.restore(legacy_snapshot)

    assert @registry.has?("x")
    assert @registry.has?("y")
  end

  def test_snapshot_isolation
    @registry.set("x", int_type)
    snapshot = @registry.snapshot

    # Modify original after snapshot
    @registry.set("z", bool_type)

    # Snapshot should not be affected
    refute snapshot.types.key?("z")
  end

  # ========== Move semantics tracking ==========

  def test_mark_moved
    @registry.set("x", int_type)

    @registry.mark_moved("x")

    assert @registry.moved?("x")
  end

  def test_moved_returns_false_for_non_moved
    @registry.set("x", int_type)

    refute @registry.moved?("x")
  end

  def test_reset_moved_clears_moved_state
    @registry.set("x", int_type)
    @registry.mark_moved("x")

    @registry.reset_moved("x")

    refute @registry.moved?("x")
  end

  def test_clear_resets_moved_vars
    @registry.set("x", int_type)
    @registry.mark_moved("x")

    @registry.clear

    refute @registry.moved?("x")
  end

  def test_snapshot_includes_moved_vars
    @registry.set("x", int_type)
    @registry.mark_moved("x")

    snapshot = @registry.snapshot

    assert snapshot.moved_vars.include?("x")
  end

  def test_restore_restores_moved_vars
    @registry.set("x", int_type)
    @registry.mark_moved("x")
    snapshot = @registry.snapshot

    @registry.reset_moved("x")
    @registry.restore(snapshot)

    assert @registry.moved?("x")
  end

  # ========== has_move_semantics? class method ==========

  def test_has_move_semantics_for_owned_type
    base = MLC::SemanticIR::Builder.primitive_type("Owned")
    inner = MLC::SemanticIR::Builder.primitive_type("i32")
    owned_type = MLC::SemanticIR::GenericType.new(base_type: base, type_args: [inner])

    assert MLC::Representations::Semantic::Gen::Services::VarTypeRegistry.has_move_semantics?(owned_type)
  end

  def test_has_move_semantics_false_for_shared_type
    base = MLC::SemanticIR::Builder.primitive_type("Shared")
    inner = MLC::SemanticIR::Builder.primitive_type("i32")
    shared_type = MLC::SemanticIR::GenericType.new(base_type: base, type_args: [inner])

    refute MLC::Representations::Semantic::Gen::Services::VarTypeRegistry.has_move_semantics?(shared_type)
  end

  def test_has_move_semantics_false_for_primitive
    refute MLC::Representations::Semantic::Gen::Services::VarTypeRegistry.has_move_semantics?(int_type)
  end

  # ========== inspect and to_s ==========

  def test_inspect
    @registry.set("x", int_type)
    @registry.set("y", string_type)

    result = @registry.inspect

    assert_includes result, "VarTypeRegistry"
    assert_includes result, "x"
    assert_includes result, "y"
  end

  def test_to_s_equals_inspect
    @registry.set("x", int_type)

    assert_equal @registry.inspect, @registry.to_s
  end

  private

  def int_type
    MLC::SemanticIR::Builder.primitive_type("i32")
  end

  def string_type
    MLC::SemanticIR::Builder.primitive_type("string")
  end

  def bool_type
    MLC::SemanticIR::Builder.primitive_type("bool")
  end
end
