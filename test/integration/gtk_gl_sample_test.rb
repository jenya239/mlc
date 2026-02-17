# frozen_string_literal: true

require_relative "../test_helper"

class GtkGlSampleTest < Minitest::Test
  GTK_GL_DIR = "/home/jenya/workspaces/experimental/gtk-gl-cpp-2025"

  def test_parse_buffer_hpp
    skip "gtk-gl-cpp-2025 not available" unless File.exist?("#{GTK_GL_DIR}/include/gl/buffer.hpp")

    source = File.read("#{GTK_GL_DIR}/include/gl/buffer.hpp")
    program = CppAst.parse(source)
    output = program.to_source
    assert_equal source, output
  end

  def test_parse_texture_atlas_hpp
    skip "gtk-gl-cpp-2025 not available" unless File.exist?("#{GTK_GL_DIR}/include/text/texture_atlas.hpp")

    source = File.read("#{GTK_GL_DIR}/include/text/texture_atlas.hpp")
    program = CppAst.parse(source)
    output = program.to_source
    assert_equal source, output
  end

  def test_parse_simple_class_with_nested_enum
    source = <<~CPP
      class Buffer {
      public:
          enum class Type {
              Array = 0,
              ElementArray = 1
          };
      private:
          Type type_;
      };
    CPP

    program = CppAst.parse(source)
    output = program.to_source

    assert_equal source, output
  end

  def test_parse_template_method_in_class
    source = <<~CPP
      class Buffer {
      public:
          template <typename T>
          void data(T value) {
              bind();
          }
      };
    CPP

    program = CppAst.parse(source)
    output = program.to_source

    assert_equal source, output
  end

  def test_parse_deleted_operators
    source = <<~CPP
      class Buffer {
      public:
          Buffer(const Buffer&) = delete;
          Buffer& operator=(const Buffer&) = delete;
      };
    CPP

    program = CppAst.parse(source)
    output = program.to_source

    assert_equal source, output
  end
end
