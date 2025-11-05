# frozen_string_literal: true
+
+module MLC
+  module IRGenV2
+    module Passes
+      class ModuleImportPass
+        def initialize(import_service:, function_registry:, type_registry:)
+          @import_service = import_service
+          @function_registry = function_registry
+          @type_registry = type_registry
+        end
+
+        def run(program)
+          Array(program.imports).each do |import_decl|
+            @import_service.process(
+              import_decl,
+              function_registry: @function_registry,
+              type_registry: @type_registry
+            )
+          end
+        end
+      end
+    end
+  end
+end
