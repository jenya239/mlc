#!/usr/bin/env ruby
# Simple fix for engine.rb using text replacements

file = "lib/mlc/representations/semantic/gen/engine.rb"
content = File.read(file)

puts "Original namespace: #{content.scan(/^module MLC\n  module \w+/).first.inspect}"

# Step 1: Replace module namespace declaration
content.gsub!(
  "module MLC\n  module SemanticGen\n    # Engine orchestrates SemanticGen rule evaluation with shared services\n    class Engine",
  "module MLC\n  module Representations\n    module Semantic\n      module Gen\n        # Engine orchestrates SemanticGen rule evaluation with shared services\n        class Engine"
)

# Step 2: Replace all internal references to Rules
content.gsub!(/MLC::SemanticGen::Rules::/, 'Rules::')

# Step 3: Replace closing ends
content.gsub!(
  /^    end\n  end\n end\Z/,
  "        end\n      end\n    end\n  end\nend"
)

# Write back
File.write(file, content)

puts "✓ Fixed #{file}"
puts "  - Namespace: MLC::SemanticGen → MLC::Representations::Semantic::Gen"
puts "  - Internal refs: MLC::SemanticGen::Rules::* → Rules::*"
puts "  - Closing ends updated"
