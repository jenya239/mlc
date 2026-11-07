#pragma once

// Invariants:
// - At most one command is in flight until command_take_ready_value consumes it.
// - The worker is detached. The gallery frame only polls the mailbox.
// - A launch while a command is in flight returns 0 and starts nothing.

#include "mlc/process/process_abi.hpp"

#include <mutex>
#include <thread>
#include <utility>

namespace mlc {
namespace process {

struct CommandState {
    std::mutex mutex;
    bool in_flight = false;
    bool ready = false;
    int generation = 0;
    String name;
    String call_identifier;
    String arguments;
    String text;
};

struct CommandStaged {
    int generation = 0;
    String name;
    String call_identifier;
    String arguments;
    String text;
};

inline CommandState& command_state() {
    static CommandState state;
    return state;
}

inline CommandStaged& command_staged() {
    static CommandStaged staged;
    return staged;
}

inline void post_command(int generation, String name, String call_identifier, String arguments, String text) {
    std::lock_guard<std::mutex> guard(command_state().mutex);
    CommandState& state = command_state();
    state.ready = true;
    state.generation = generation;
    state.name = std::move(name);
    state.call_identifier = std::move(call_identifier);
    state.arguments = std::move(arguments);
    state.text = std::move(text);
}

inline int command_launch_value(
    int generation,
    String name,
    String call_identifier,
    String arguments,
    String command,
    String directory) {
    {
        std::lock_guard<std::mutex> guard(command_state().mutex);
        if (command_state().in_flight) {
            return 0;
        }
        command_state().in_flight = true;
        command_state().ready = false;
    }
    try {
        std::thread worker([generation, name, call_identifier, arguments, command, directory]() mutable {
            String text = command_capture_value(std::move(command), std::move(directory), 20000, 32768);
            post_command(generation, std::move(name), std::move(call_identifier), std::move(arguments), std::move(text));
        });
        worker.detach();
    } catch (...) {
        post_command(generation, std::move(name), std::move(call_identifier), std::move(arguments), String("refused: spawn"));
    }
    return 1;
}

inline int command_take_ready_value() {
    std::lock_guard<std::mutex> guard(command_state().mutex);
    CommandState& state = command_state();
    if (!state.ready) {
        return 0;
    }
    CommandStaged& staged = command_staged();
    staged.generation = state.generation;
    staged.name = state.name;
    staged.call_identifier = state.call_identifier;
    staged.arguments = state.arguments;
    staged.text = state.text;
    state.ready = false;
    state.in_flight = false;
    state.text = String();
    return 1;
}

inline int command_staged_generation_value() {
    return command_staged().generation;
}

inline String command_staged_name_value() {
    return command_staged().name;
}

inline String command_staged_call_identifier_value() {
    return command_staged().call_identifier;
}

inline String command_staged_arguments_value() {
    return command_staged().arguments;
}

inline String command_staged_text_value() {
    return command_staged().text;
}

}  // namespace process
}  // namespace mlc
