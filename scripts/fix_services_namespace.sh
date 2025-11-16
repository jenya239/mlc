#!/bin/bash
# Fix MLC::Services → MLC::Representations::Semantic::Gen::Services namespace

set -e

echo "=== Services Namespace Refactor ==="
echo

# Count files first
echo "Analyzing files..."
service_files=$(find lib/mlc/representations/semantic/gen/services -name "*.rb" -type f)
file_count=$(echo "$service_files" | wc -l)

echo "Found $file_count service definition files"
echo

# Backup directory
backup_dir="backup_services_$(date +%Y%m%d_%H%M%S)"
mkdir -p "$backup_dir"

# Process each file
processed=0
for file in $service_files; do
    # Create backup
    cp "$file" "$backup_dir/"

    # Detect current pattern and fix
    if grep -q "^module MLC\$" "$file" && grep -q "^  module Services\$" "$file"; then
        # Pattern 1: module MLC → module Services (direct, missing intermediate)
        sed -i '1,/^module MLC$/!b; n; s/^  module Services$/  module Representations\n    module Semantic\n      module Gen\n        module Services/' "$file"

        # Fix closing ends (add 3 extra closing ends before final end)
        sed -i '$s/^end$/        end\n      end\n    end\n  end\nend/' "$file"

        echo "Fixed (Pattern 1): $file"
        ((processed++))

    elif grep -q "^module MLC\$" "$file" && grep -q "^  module SemanticGen\$" "$file"; then
        # Pattern 2: module MLC → module SemanticGen → module Services
        # Replace SemanticGen with proper nesting
        awk '
        /^module MLC$/ { print; inMLC=1; next }
        /^  module SemanticGen$/ && inMLC {
            print "  module Representations"
            print "    module Semantic"
            print "      module Gen"
            inSemanticGen=1
            next
        }
        /^    module Services$/ && inSemanticGen {
            print "        module Services"
            inServices=1
            next
        }
        /^    end$/ && inServices { print "        end"; inServices=0; next }
        /^  end$/ && inSemanticGen {
            print "      end"
            print "    end"
            print "  end"
            inSemanticGen=0
            next
        }
        { print }
        ' "$file" > "$file.tmp" && mv "$file.tmp" "$file"

        echo "Fixed (Pattern 2): $file"
        ((processed++))
    fi
done

echo
echo "=== Summary ==="
echo "Processed: $processed files"
echo "Backup saved to: $backup_dir/"
echo
echo "Next steps:"
echo "1. Run: rake test"
echo "2. If tests pass: git add -A && git commit -m 'refactor: Services namespace'"
echo "3. If tests fail: ./scripts/restore_services_backup.sh $backup_dir"
