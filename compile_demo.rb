require_relative 'lib/mlc/common/index'

source = File.read('misc/examples/string_operations_demo.mlc')
cpp_code = MLC.to_cpp(source)
File.write('/tmp/string_demo.cpp', cpp_code)
puts '✓ Generated C++ code'
