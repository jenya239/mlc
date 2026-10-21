current_step: 419
status: ready
last_result: done step 419: red — codec не ограничивает суммарный вход и глубину вложенности; encode принимает payload_size=16 (23 слова > max_input=15), nested record decode не отклоняет
last_verify: ruby scripts/run_script_vm_unit.rb script_host/tests/ipc_codec_unit.mlc => EXIT:1, [script_vm ipc_codec_unit] FAIL: payload_size over limit not rejected words=23 max_input=15
next_step: 420
consult_count: 0
consult_response:
consult_verdict:
