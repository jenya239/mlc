#ifndef MLC_YAML_HPP
#define MLC_YAML_HPP

#include <charconv>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>
#include <yaml.h>
#include "mlc/core/string.hpp"

namespace mlc::yaml {

struct YamlValue;

struct YamlFailure {
    mlc::String message;
};

using YamlObject = std::vector<std::pair<mlc::String, YamlValue>>;
using YamlParseResult = std::variant<YamlValue, YamlFailure>;

struct YamlValue {
    std::variant<
        std::monostate,
        bool,
        double,
        mlc::String,
        std::vector<YamlValue>,
        YamlObject
    > value;

    YamlValue() : value(std::monostate{}) {}
    YamlValue(std::monostate) : value(std::monostate{}) {}
    YamlValue(bool boolean) : value(boolean) {}
    YamlValue(double number) : value(number) {}
    YamlValue(float number) : value(static_cast<double>(number)) {}
    YamlValue(int32_t number) : value(static_cast<double>(number)) {}
    YamlValue(const mlc::String& text) : value(text) {}
    YamlValue(const std::vector<YamlValue>& items) : value(items) {}
    YamlValue(const YamlObject& object) : value(object) {}

    bool is_null() const { return std::holds_alternative<std::monostate>(value); }
    bool is_bool() const { return std::holds_alternative<bool>(value); }
    bool is_number() const { return std::holds_alternative<double>(value); }
    bool is_string() const { return std::holds_alternative<mlc::String>(value); }
    bool is_array() const { return std::holds_alternative<std::vector<YamlValue>>(value); }
    bool is_object() const { return std::holds_alternative<YamlObject>(value); }

    std::optional<bool> as_bool() const {
        if (const bool* boolean = std::get_if<bool>(&value)) return *boolean;
        return std::nullopt;
    }

    std::optional<double> as_number() const {
        if (const double* number = std::get_if<double>(&value)) return *number;
        return std::nullopt;
    }

    std::optional<mlc::String> as_string() const {
        if (const mlc::String* text = std::get_if<mlc::String>(&value)) return *text;
        return std::nullopt;
    }

    std::optional<std::vector<YamlValue>> as_array() const {
        if (const std::vector<YamlValue>* items = std::get_if<std::vector<YamlValue>>(&value)) return *items;
        return std::nullopt;
    }

    std::optional<YamlObject> as_object() const {
        if (const YamlObject* object = std::get_if<YamlObject>(&value)) return *object;
        return std::nullopt;
    }
};

inline YamlValue yaml_null() { return YamlValue(std::monostate{}); }
inline YamlValue yaml_bool(bool boolean) { return YamlValue(boolean); }
inline YamlValue yaml_number(double number) { return YamlValue(number); }
inline YamlValue yaml_string(const mlc::String& text) { return YamlValue(text); }
inline YamlValue yaml_array(const std::vector<YamlValue>& items) { return YamlValue(items); }
inline YamlValue yaml_object(const YamlObject& object) { return YamlValue(object); }

inline bool yaml_is_failure(const YamlParseResult& result) {
    return std::holds_alternative<YamlFailure>(result);
}

inline std::optional<YamlValue> yaml_get(const YamlValue& object, const mlc::String& key) {
    const YamlObject* mapping = std::get_if<YamlObject>(&object.value);
    if (mapping == nullptr) return std::nullopt;
    for (const std::pair<mlc::String, YamlValue>& entry : *mapping) {
        if (entry.first == key) return entry.second;
    }
    return std::nullopt;
}

inline std::vector<mlc::String> yaml_keys(const YamlValue& object) {
    std::vector<mlc::String> keys;
    const YamlObject* mapping = std::get_if<YamlObject>(&object.value);
    if (mapping == nullptr) return keys;
    keys.reserve(mapping->size());
    for (const std::pair<mlc::String, YamlValue>& entry : *mapping) {
        keys.push_back(entry.first);
    }
    return keys;
}

namespace {

struct ByteText {
    const unsigned char* data;
    size_t size;
};

inline ByteText bytes_of(const mlc::String& text) {
    return ByteText{
        reinterpret_cast<const unsigned char*>(text.c_str()),
        text.size()
    };
}

inline std::string copy_yaml_chars(const yaml_char_t* text) {
    if (text == nullptr) return std::string();
    return std::string(reinterpret_cast<const char*>(text));
}

inline bool tag_is(const yaml_char_t* tag, const char* expected) {
    if (tag == nullptr) return false;
    return std::strcmp(reinterpret_cast<const char*>(tag), expected) == 0;
}

inline bool scalar_tag_allowed(const yaml_char_t* tag) {
    if (tag == nullptr) return true;
    return tag_is(tag, "tag:yaml.org,2002:str")
        || tag_is(tag, "tag:yaml.org,2002:int")
        || tag_is(tag, "tag:yaml.org,2002:float")
        || tag_is(tag, "tag:yaml.org,2002:bool")
        || tag_is(tag, "tag:yaml.org,2002:null");
}

inline bool collection_tag_allowed(const yaml_char_t* tag, const char* expected) {
    if (tag == nullptr) return true;
    return tag_is(tag, expected);
}

inline bool word_is(const std::string& text, const char* expected) {
    return text == expected;
}

inline std::optional<bool> plain_bool(const std::string& text) {
    if (word_is(text, "y") || word_is(text, "Y") || word_is(text, "yes") || word_is(text, "Yes") || word_is(text, "YES")
        || word_is(text, "true") || word_is(text, "True") || word_is(text, "TRUE")
        || word_is(text, "on") || word_is(text, "On") || word_is(text, "ON")) {
        return true;
    }
    if (word_is(text, "n") || word_is(text, "N") || word_is(text, "no") || word_is(text, "No") || word_is(text, "NO")
        || word_is(text, "false") || word_is(text, "False") || word_is(text, "FALSE")
        || word_is(text, "off") || word_is(text, "Off") || word_is(text, "OFF")) {
        return false;
    }
    return std::nullopt;
}

inline bool plain_null(const std::string& text) {
    return text.empty()
        || word_is(text, "~")
        || word_is(text, "null")
        || word_is(text, "Null")
        || word_is(text, "NULL");
}

inline bool plain_infinity(const std::string& text, double& number) {
    std::string body = text;
    double sign = 1.0;
    if (!body.empty() && (body[0] == '+' || body[0] == '-')) {
        if (body[0] == '-') sign = -1.0;
        body = body.substr(1);
    }
    if (body == ".inf" || body == ".Inf" || body == ".INF") {
        number = sign * std::numeric_limits<double>::infinity();
        return true;
    }
    if (body == ".nan" || body == ".NaN" || body == ".NAN") {
        number = std::numeric_limits<double>::quiet_NaN();
        return true;
    }
    return false;
}

inline bool has_leading_zero_integer(const std::string& text) {
    size_t index = 0;
    if (!text.empty() && (text[0] == '+' || text[0] == '-')) index = 1;
    if (index >= text.size() || text[index] != '0') return false;
    if (index + 1 >= text.size()) return false;
    return text[index + 1] >= '0' && text[index + 1] <= '9';
}

inline std::optional<double> plain_number(const std::string& text) {
    double infinity = 0.0;
    if (plain_infinity(text, infinity)) return infinity;
    if (has_leading_zero_integer(text)) return std::nullopt;
    double number = 0.0;
    const char* first = text.data();
    const char* last = text.data() + text.size();
    std::from_chars_result parsed = std::from_chars(first, last, number);
    if (parsed.ec != std::errc() || parsed.ptr != last) return std::nullopt;
    return number;
}

inline YamlValue resolve_scalar(const std::string& text, const yaml_char_t* tag, yaml_scalar_style_t style, std::string& message) {
    if (tag_is(tag, "tag:yaml.org,2002:str")) return yaml_string(mlc::String(text));
    if (tag_is(tag, "tag:yaml.org,2002:null")) {
        if (!plain_null(text)) {
            message = "scalar";
            return yaml_null();
        }
        return yaml_null();
    }
    if (tag_is(tag, "tag:yaml.org,2002:bool")) {
        std::optional<bool> boolean = plain_bool(text);
        if (!boolean.has_value()) {
            message = "scalar";
            return yaml_null();
        }
        return yaml_bool(*boolean);
    }
    if (tag_is(tag, "tag:yaml.org,2002:int") || tag_is(tag, "tag:yaml.org,2002:float")) {
        std::optional<double> number = plain_number(text);
        if (!number.has_value()) {
            message = "scalar";
            return yaml_null();
        }
        return yaml_number(*number);
    }
    if (style != YAML_PLAIN_SCALAR_STYLE) return yaml_string(mlc::String(text));
    if (plain_null(text)) return yaml_null();
    std::optional<bool> boolean = plain_bool(text);
    if (boolean.has_value()) return yaml_bool(*boolean);
    std::optional<double> number = plain_number(text);
    if (number.has_value()) return yaml_number(*number);
    return yaml_string(mlc::String(text));
}

struct AnchorTable {
    std::vector<std::pair<std::string, YamlValue>> finished;
    std::vector<std::string> open;

    bool is_open(const std::string& name) const {
        for (const std::string& open_name : open) {
            if (open_name == name) return true;
        }
        return false;
    }

    std::optional<YamlValue> find_finished(const std::string& name) const {
        for (const std::pair<std::string, YamlValue>& entry : finished) {
            if (entry.first == name) return entry.second;
        }
        return std::nullopt;
    }

    void remember(const std::string& name, const YamlValue& value) {
        if (name.empty()) return;
        for (std::pair<std::string, YamlValue>& entry : finished) {
            if (entry.first == name) {
                entry.second = value;
                return;
            }
        }
        finished.push_back(std::pair<std::string, YamlValue>(name, value));
    }
};

struct EventReader {
    yaml_parser_t* parser;
    std::string message;
    bool holding_event;
    yaml_event_t held_event;

    explicit EventReader(yaml_parser_t* parser_pointer)
        : parser(parser_pointer), holding_event(false), held_event() {}

    ~EventReader() {
        if (holding_event) yaml_event_delete(&held_event);
    }

    EventReader(const EventReader&) = delete;
    EventReader& operator=(const EventReader&) = delete;

    bool pull() {
        if (holding_event || !message.empty()) return message.empty();
        if (yaml_parser_parse(parser, &held_event) == 0) {
            message = parser->problem != nullptr ? parser->problem : "syntax";
            return false;
        }
        holding_event = true;
        return true;
    }

    yaml_event_type_t peek_type() {
        if (!pull()) return YAML_NO_EVENT;
        return held_event.type;
    }

    yaml_event_t take() {
        pull();
        yaml_event_t event = held_event;
        holding_event = false;
        return event;
    }
};

inline std::optional<YamlValue> parse_node(EventReader& reader, AnchorTable& anchors);

inline std::optional<YamlValue> resolve_alias(const yaml_event_t& event, AnchorTable& anchors, std::string& message) {
    std::string name = copy_yaml_chars(event.data.alias.anchor);
    if (anchors.is_open(name)) {
        message = "cyclic alias";
        return std::nullopt;
    }
    std::optional<YamlValue> found = anchors.find_finished(name);
    if (!found.has_value()) {
        message = "unknown alias";
        return std::nullopt;
    }
    return *found;
}

inline std::optional<std::string> parse_mapping_key(EventReader& reader, AnchorTable& anchors) {
    if (reader.peek_type() == YAML_NO_EVENT) return std::nullopt;
    if (reader.peek_type() == YAML_ALIAS_EVENT) {
        yaml_event_t event = reader.take();
        std::optional<YamlValue> aliased = resolve_alias(event, anchors, reader.message);
        yaml_event_delete(&event);
        if (!aliased.has_value()) return std::nullopt;
        std::optional<mlc::String> text = aliased->as_string();
        if (!text.has_value()) {
            reader.message = "key";
            return std::nullopt;
        }
        return std::string(text->c_str(), text->size());
    }
    if (reader.peek_type() != YAML_SCALAR_EVENT) {
        reader.message = "key";
        return std::nullopt;
    }
    yaml_event_t event = reader.take();
    if (!scalar_tag_allowed(event.data.scalar.tag)) {
        reader.message = "tag";
        yaml_event_delete(&event);
        return std::nullopt;
    }
    std::string key(reinterpret_cast<const char*>(event.data.scalar.value), event.data.scalar.length);
    yaml_event_delete(&event);
    return key;
}

inline std::optional<YamlValue> parse_sequence(EventReader& reader, AnchorTable& anchors, const std::string& anchor) {
    if (!anchor.empty()) anchors.open.push_back(anchor);
    std::vector<YamlValue> items;
    while (reader.peek_type() != YAML_SEQUENCE_END_EVENT) {
        if (reader.peek_type() == YAML_NO_EVENT) {
            if (!anchor.empty()) anchors.open.pop_back();
            return std::nullopt;
        }
        std::optional<YamlValue> item = parse_node(reader, anchors);
        if (!item.has_value()) {
            if (!anchor.empty()) anchors.open.pop_back();
            return std::nullopt;
        }
        items.push_back(*item);
    }
    yaml_event_t end_event = reader.take();
    yaml_event_delete(&end_event);
    YamlValue value = yaml_array(items);
    if (!anchor.empty()) {
        anchors.open.pop_back();
        anchors.remember(anchor, value);
    }
    return value;
}

inline std::optional<YamlValue> parse_mapping(EventReader& reader, AnchorTable& anchors, const std::string& anchor) {
    if (!anchor.empty()) anchors.open.push_back(anchor);
    YamlObject object;
    while (reader.peek_type() != YAML_MAPPING_END_EVENT) {
        if (reader.peek_type() == YAML_NO_EVENT) {
            if (!anchor.empty()) anchors.open.pop_back();
            return std::nullopt;
        }
        std::optional<std::string> key = parse_mapping_key(reader, anchors);
        if (!key.has_value()) {
            if (!anchor.empty()) anchors.open.pop_back();
            return std::nullopt;
        }
        std::optional<YamlValue> entry_value = parse_node(reader, anchors);
        if (!entry_value.has_value()) {
            if (!anchor.empty()) anchors.open.pop_back();
            return std::nullopt;
        }
        object.push_back(std::pair<mlc::String, YamlValue>(mlc::String(*key), *entry_value));
    }
    yaml_event_t end_event = reader.take();
    yaml_event_delete(&end_event);
    YamlValue value = yaml_object(object);
    if (!anchor.empty()) {
        anchors.open.pop_back();
        anchors.remember(anchor, value);
    }
    return value;
}

inline std::optional<YamlValue> parse_node(EventReader& reader, AnchorTable& anchors) {
    if (reader.peek_type() == YAML_NO_EVENT) return std::nullopt;
    yaml_event_t event = reader.take();
    std::optional<YamlValue> parsed;
    if (event.type == YAML_ALIAS_EVENT) {
        parsed = resolve_alias(event, anchors, reader.message);
        yaml_event_delete(&event);
        return parsed;
    }
    if (event.type == YAML_SCALAR_EVENT) {
        if (!scalar_tag_allowed(event.data.scalar.tag)) {
            reader.message = "tag";
            yaml_event_delete(&event);
            return std::nullopt;
        }
        std::string text(reinterpret_cast<const char*>(event.data.scalar.value), event.data.scalar.length);
        std::string anchor = copy_yaml_chars(event.data.scalar.anchor);
        parsed = resolve_scalar(text, event.data.scalar.tag, event.data.scalar.style, reader.message);
        yaml_event_delete(&event);
        if (!reader.message.empty() && (reader.message == "scalar" || reader.message == "tag")) return std::nullopt;
        anchors.remember(anchor, *parsed);
        return parsed;
    }
    if (event.type == YAML_SEQUENCE_START_EVENT) {
        if (!collection_tag_allowed(event.data.sequence_start.tag, "tag:yaml.org,2002:seq")) {
            reader.message = "tag";
            yaml_event_delete(&event);
            return std::nullopt;
        }
        std::string anchor = copy_yaml_chars(event.data.sequence_start.anchor);
        yaml_event_delete(&event);
        return parse_sequence(reader, anchors, anchor);
    }
    if (event.type == YAML_MAPPING_START_EVENT) {
        if (!collection_tag_allowed(event.data.mapping_start.tag, "tag:yaml.org,2002:map")) {
            reader.message = "tag";
            yaml_event_delete(&event);
            return std::nullopt;
        }
        std::string anchor = copy_yaml_chars(event.data.mapping_start.anchor);
        yaml_event_delete(&event);
        return parse_mapping(reader, anchors, anchor);
    }
    reader.message = "syntax";
    yaml_event_delete(&event);
    return std::nullopt;
}

inline std::optional<YamlValue> parse_document(EventReader& reader, AnchorTable& anchors) {
    yaml_event_t start_event = reader.take();
    yaml_event_delete(&start_event);
    std::optional<YamlValue> value;
    if (reader.peek_type() == YAML_DOCUMENT_END_EVENT) {
        value = yaml_null();
    } else {
        value = parse_node(reader, anchors);
        if (!value.has_value()) return std::nullopt;
    }
    if (reader.peek_type() != YAML_DOCUMENT_END_EVENT) {
        if (reader.message.empty()) reader.message = "syntax";
        return std::nullopt;
    }
    yaml_event_t end_event = reader.take();
    yaml_event_delete(&end_event);
    return value;
}

struct ParserOwner {
    yaml_parser_t parser;
    bool ready;

    ParserOwner() : ready(yaml_parser_initialize(&parser) != 0) {}

    ~ParserOwner() {
        if (ready) yaml_parser_delete(&parser);
    }

    ParserOwner(const ParserOwner&) = delete;
    ParserOwner& operator=(const ParserOwner&) = delete;
};

inline YamlParseResult failure_from(const EventReader& reader) {
    std::string message = reader.message.empty() ? std::string("syntax") : reader.message;
    return YamlFailure{mlc::String(message)};
}

inline YamlParseResult parse_stream(const mlc::String& text) {
    ParserOwner owner;
    if (!owner.ready) return YamlFailure{mlc::String("syntax")};
    ByteText input = bytes_of(text);
    yaml_parser_set_input_string(&owner.parser, input.data, input.size);
    EventReader reader(&owner.parser);
    AnchorTable anchors;
    if (reader.peek_type() != YAML_STREAM_START_EVENT) return failure_from(reader);
    yaml_event_t stream_start = reader.take();
    yaml_event_delete(&stream_start);
    std::optional<YamlValue> document;
    if (reader.peek_type() == YAML_STREAM_END_EVENT) {
        document = yaml_null();
    } else if (reader.peek_type() == YAML_DOCUMENT_START_EVENT) {
        document = parse_document(reader, anchors);
        if (!document.has_value()) return failure_from(reader);
        if (reader.peek_type() == YAML_DOCUMENT_START_EVENT) {
            return YamlFailure{mlc::String("multiple documents")};
        }
    } else {
        return failure_from(reader);
    }
    if (reader.peek_type() != YAML_STREAM_END_EVENT) return failure_from(reader);
    yaml_event_t stream_end = reader.take();
    yaml_event_delete(&stream_end);
    return *document;
}

inline bool emit_scalar(yaml_emitter_t& emitter, const std::string& text, yaml_scalar_style_t style, int plain_implicit) {
    yaml_event_t event;
    int quoted_implicit = plain_implicit == 0 ? 1 : 1;
    if (yaml_scalar_event_initialize(
            &event,
            nullptr,
            nullptr,
            reinterpret_cast<const yaml_char_t*>(text.data()),
            static_cast<int>(text.size()),
            plain_implicit,
            quoted_implicit,
            style) == 0) {
        return false;
    }
    return yaml_emitter_emit(&emitter, &event) != 0;
}

inline std::string format_number(double number) {
    if (std::isnan(number)) return ".nan";
    if (std::isinf(number)) return number > 0.0 ? ".inf" : "-.inf";
    double integral = 0.0;
    if (std::modf(number, &integral) == 0.0
        && number >= -9007199254740992.0
        && number <= 9007199254740992.0) {
        return std::to_string(static_cast<long long>(number));
    }
    char buffer[64];
    std::to_chars_result formatted = std::to_chars(buffer, buffer + sizeof(buffer), number, std::chars_format::general, 15);
    if (formatted.ec != std::errc()) return "0";
    return std::string(buffer, formatted.ptr);
}

inline bool emit_value(yaml_emitter_t& emitter, const YamlValue& value) {
    if (value.is_null()) return emit_scalar(emitter, "null", YAML_PLAIN_SCALAR_STYLE, 1);
    if (value.is_bool()) return emit_scalar(emitter, *value.as_bool() ? "true" : "false", YAML_PLAIN_SCALAR_STYLE, 1);
    if (value.is_number()) return emit_scalar(emitter, format_number(*value.as_number()), YAML_PLAIN_SCALAR_STYLE, 1);
    if (value.is_string()) {
        mlc::String text = *value.as_string();
        return emit_scalar(emitter, std::string(text.c_str(), text.size()), YAML_DOUBLE_QUOTED_SCALAR_STYLE, 0);
    }
    if (value.is_array()) {
        std::optional<std::vector<YamlValue>> items = value.as_array();
        if (!items.has_value()) return false;
        yaml_event_t start_event;
        if (yaml_sequence_start_event_initialize(&start_event, nullptr, nullptr, 1, YAML_BLOCK_SEQUENCE_STYLE) == 0) return false;
        if (yaml_emitter_emit(&emitter, &start_event) == 0) return false;
        for (const YamlValue& item : *items) {
            if (!emit_value(emitter, item)) return false;
        }
        yaml_event_t end_event;
        if (yaml_sequence_end_event_initialize(&end_event) == 0) return false;
        return yaml_emitter_emit(&emitter, &end_event) != 0;
    }
    std::optional<YamlObject> object = value.as_object();
    if (!object.has_value()) return false;
    yaml_event_t start_event;
    if (yaml_mapping_start_event_initialize(&start_event, nullptr, nullptr, 1, YAML_BLOCK_MAPPING_STYLE) == 0) return false;
    if (yaml_emitter_emit(&emitter, &start_event) == 0) return false;
    for (const std::pair<mlc::String, YamlValue>& entry : *object) {
        std::string key(entry.first.c_str(), entry.first.size());
        if (!emit_scalar(emitter, key, YAML_DOUBLE_QUOTED_SCALAR_STYLE, 0)) return false;
        if (!emit_value(emitter, entry.second)) return false;
    }
    yaml_event_t end_event;
    if (yaml_mapping_end_event_initialize(&end_event) == 0) return false;
    return yaml_emitter_emit(&emitter, &end_event) != 0;
}

inline bool emit_document(const YamlValue& value, unsigned char* buffer, size_t capacity, size_t& written) {
    yaml_emitter_t emitter;
    if (yaml_emitter_initialize(&emitter) == 0) return false;
    yaml_emitter_set_output_string(&emitter, buffer, capacity, &written);
    yaml_emitter_set_unicode(&emitter, 1);
    yaml_emitter_set_width(&emitter, -1);
    yaml_event_t stream_start;
    if (yaml_stream_start_event_initialize(&stream_start, YAML_UTF8_ENCODING) == 0) {
        yaml_emitter_delete(&emitter);
        return false;
    }
    if (yaml_emitter_emit(&emitter, &stream_start) == 0) {
        yaml_emitter_delete(&emitter);
        return false;
    }
    yaml_event_t document_start;
    if (yaml_document_start_event_initialize(&document_start, nullptr, nullptr, nullptr, 1) == 0) {
        yaml_emitter_delete(&emitter);
        return false;
    }
    if (yaml_emitter_emit(&emitter, &document_start) == 0) {
        yaml_emitter_delete(&emitter);
        return false;
    }
    if (!emit_value(emitter, value)) {
        yaml_emitter_delete(&emitter);
        return false;
    }
    yaml_event_t document_end;
    if (yaml_document_end_event_initialize(&document_end, 1) == 0) {
        yaml_emitter_delete(&emitter);
        return false;
    }
    if (yaml_emitter_emit(&emitter, &document_end) == 0) {
        yaml_emitter_delete(&emitter);
        return false;
    }
    yaml_event_t stream_end;
    if (yaml_stream_end_event_initialize(&stream_end) == 0) {
        yaml_emitter_delete(&emitter);
        return false;
    }
    bool emitted = yaml_emitter_emit(&emitter, &stream_end) != 0;
    yaml_emitter_delete(&emitter);
    return emitted;
}

}  // namespace

// One YAML 1.1 document, via libyaml.
// Plain scalars use the 1.1 words: yes/no/on/off/y/n are bools, and an empty plain scalar is null.
// Quoted scalars stay strings. A second document, a tag outside the core schema, an unknown alias,
// or an alias into a node that is still open is YamlFailure. Merge keys are ordinary keys.
inline YamlParseResult parse_yaml(const mlc::String& text) {
    return parse_stream(text);
}

inline mlc::String stringify_yaml(const YamlValue& value) {
    size_t capacity = 256;
    while (capacity <= 8u * 1024u * 1024u) {
        std::vector<unsigned char> buffer(capacity);
        size_t written = 0;
        if (emit_document(value, buffer.data(), capacity, written)) {
            return mlc::String(reinterpret_cast<const char*>(buffer.data()), written);
        }
        capacity *= 2;
    }
    return mlc::String("");
}

}  // namespace mlc::yaml

#endif
