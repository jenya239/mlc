// Tests for mlc::String SSO (Small String Optimization)
// Compile:
//   g++ -std=c++20 -I../include -o test_string_sso test_string_sso.cpp ../src/core/string.cpp

#include "mlc/core/string.hpp"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

static int passed = 0;
static int failed = 0;

#define CHECK(expr) do { \
    if (expr) { ++passed; } \
    else { ++failed; std::cerr << "FAIL: " #expr " at line " << __LINE__ << "\n"; } \
} while(0)

#define SECTION(name) std::cout << "  " name "... " << std::flush

// ── 1. Storage selection ─────────────────────────────────────────────────────

void test_storage_selection() {
    SECTION("empty → SSO");
    { mlc::String s; CHECK(s.is_sso()); CHECK(s.byte_size() == 0); }
    std::cout << "\n";

    SECTION("short literal → SSO");
    { mlc::String s("hi"); CHECK(s.is_sso()); CHECK(s.byte_size() == 2); }
    std::cout << "\n";

    SECTION("exactly 22 bytes → SSO");
    { mlc::String s("0123456789012345678901");
      CHECK(s.byte_size() == 22); CHECK(s.is_sso()); }
    std::cout << "\n";

    SECTION("23 bytes → heap");
    { mlc::String s("01234567890123456789012");
      CHECK(s.byte_size() == 23); CHECK(!s.is_sso()); }
    std::cout << "\n";

    SECTION("long string → heap");
    { mlc::String s("This is a longer string that exceeds the SSO limit of 22 bytes");
      CHECK(!s.is_sso()); }
    std::cout << "\n";
}

// ── 2. Content correctness ───────────────────────────────────────────────────

void test_content() {
    SECTION("SSO content via as_std_string");
    { mlc::String s("hello");
      CHECK(s.as_std_string() == "hello"); }
    std::cout << "\n";

    SECTION("heap content via as_std_string");
    { std::string long_str(50, 'x');
      mlc::String s(long_str);
      CHECK(s.as_std_string() == long_str); }
    std::cout << "\n";

    SECTION("view() matches as_std_string for SSO");
    { mlc::String s("test");
      auto sv = s.view();
      CHECK(std::string(sv) == "test"); }
    std::cout << "\n";

    SECTION("view() matches as_std_string for heap");
    { mlc::String s(std::string(30, 'z'));
      auto sv = s.view();
      CHECK(sv.size() == 30);
      CHECK(sv.find_first_not_of('z') == std::string_view::npos); }
    std::cout << "\n";
}

// ── 3. Copy semantics ────────────────────────────────────────────────────────

void test_copy() {
    SECTION("SSO copy is independent");
    { mlc::String a("hello");
      mlc::String b = a;
      CHECK(b.is_sso());
      CHECK(b.as_std_string() == "hello"); }
    std::cout << "\n";

    SECTION("heap copy shares ownership (O(1))");
    { mlc::String a(std::string(30, 'a'));
      mlc::String b = a;
      CHECK(!b.is_sso());
      CHECK(b.as_std_string() == a.as_std_string()); }
    std::cout << "\n";

    SECTION("move SSO");
    { mlc::String a("hello");
      mlc::String b = std::move(a);
      CHECK(b.as_std_string() == "hello"); }
    std::cout << "\n";
}

// ── 4. Concatenation ─────────────────────────────────────────────────────────

void test_concat() {
    SECTION("SSO + SSO → SSO (short result)");
    { mlc::String a("hello"), b(" w");
      mlc::String c = a + b;
      CHECK(c.is_sso());
      CHECK(c.as_std_string() == "hello w"); }
    std::cout << "\n";

    SECTION("SSO + SSO → heap (long result)");
    { mlc::String a("0123456789012"), b("0123456789012");
      mlc::String c = a + b;
      CHECK(!c.is_sso());
      CHECK(c.byte_size() == 26);
      CHECK(c.as_std_string() == "01234567890120123456789012"); }
    std::cout << "\n";

    SECTION("heap + SSO");
    { mlc::String a(std::string(25, 'a')), b("!");
      mlc::String c = a + b;
      CHECK(c.byte_size() == 26);
      CHECK(c.as_std_string().back() == '!'); }
    std::cout << "\n";

    SECTION("empty + s = s");
    { mlc::String a(""), b("world");
      CHECK((a + b).as_std_string() == "world"); }
    std::cout << "\n";
}

// ── 5. Comparison operators ──────────────────────────────────────────────────

void test_comparison() {
    SECTION("== SSO");
    { mlc::String a("abc"), b("abc"), c("abd");
      CHECK(a == b); CHECK(!(a == c)); }
    std::cout << "\n";

    SECTION("== heap");
    { mlc::String a(std::string(25, 'x')), b(std::string(25, 'x'));
      CHECK(a == b); }
    std::cout << "\n";

    SECTION("< ordering");
    { mlc::String a("abc"), b("abd");
      CHECK(a < b); CHECK(!(b < a)); }
    std::cout << "\n";

    SECTION("SSO == heap (same content)");
    { mlc::String a("hello");
      mlc::String b(std::string("hello"));  // forced same content
      CHECK(a == b); }
    std::cout << "\n";
}

// ── 6. length() — character count, not byte count ───────────────────────────

void test_length() {
    SECTION("ASCII length");
    { mlc::String s("hello");
      CHECK(s.length() == 5); }
    std::cout << "\n";

    SECTION("empty length");
    { mlc::String s(""); CHECK(s.length() == 0); }
    std::cout << "\n";

    // Cyrillic: each char = 2 bytes in UTF-8
    SECTION("Cyrillic SSO: 6 chars, 12 bytes");
    { mlc::String s("Привет");   // П-р-и-в-е-т: 6×2=12 bytes ≤ 22 → SSO
      CHECK(s.is_sso());
      CHECK(s.byte_size() == 12);
      CHECK(s.length() == 6); }
    std::cout << "\n";

    SECTION("Cyrillic heap: 12 chars, 24 bytes");
    { mlc::String s("ПриветПривет");  // 12×2=24 bytes > 22 → heap
      CHECK(!s.is_sso());
      CHECK(s.length() == 12); }
    std::cout << "\n";

    // Chinese: each char = 3 bytes in UTF-8
    SECTION("Chinese SSO: 7 chars, 21 bytes");
    { mlc::String s("你好世界你好世");  // 7×3=21 bytes ≤ 22 → SSO
      CHECK(s.is_sso());
      CHECK(s.byte_size() == 21);
      CHECK(s.length() == 7); }
    std::cout << "\n";

    SECTION("Chinese heap: 8 chars, 24 bytes");
    { mlc::String s("你好世界你好世界");  // 8×3=24 bytes > 22 → heap
      CHECK(!s.is_sso());
      CHECK(s.length() == 8); }
    std::cout << "\n";
}

// ── 7. char_at() — UTF-8 aware ───────────────────────────────────────────────

void test_char_at() {
    SECTION("ASCII char_at SSO");
    { mlc::String s("hello");
      CHECK(s.char_at(0).as_std_string() == "h");
      CHECK(s.char_at(4).as_std_string() == "o"); }
    std::cout << "\n";

    SECTION("Cyrillic char_at SSO");
    { mlc::String s("Привет");
      CHECK(s.char_at(0).as_std_string() == "П");
      CHECK(s.char_at(5).as_std_string() == "т"); }
    std::cout << "\n";

    SECTION("Cyrillic char_at heap");
    { mlc::String s("ПриветПривет");  // 24 bytes → heap
      CHECK(!s.is_sso());
      CHECK(s.char_at(0).as_std_string() == "П");
      CHECK(s.char_at(6).as_std_string() == "П");
      CHECK(s.char_at(11).as_std_string() == "т"); }
    std::cout << "\n";

    SECTION("Chinese char_at SSO");
    { mlc::String s("你好世界");
      CHECK(s.char_at(0).as_std_string() == "你");
      CHECK(s.char_at(3).as_std_string() == "界"); }
    std::cout << "\n";
}

// ── 8. substring() ───────────────────────────────────────────────────────────

void test_substring() {
    SECTION("ASCII substring SSO");
    { mlc::String s("hello world");
      CHECK(s.substring(6, 5).as_std_string() == "world"); }
    std::cout << "\n";

    SECTION("Cyrillic substring SSO → might become SSO or heap");
    { mlc::String s("Привет мир");
      mlc::String sub = s.substring(7, 3);  // "мир"
      CHECK(sub.as_std_string() == "мир");
      CHECK(sub.length() == 3); }
    std::cout << "\n";

    SECTION("substring(0) = full string SSO");
    { mlc::String s("hello");
      CHECK(s.substring(0).as_std_string() == "hello"); }
    std::cout << "\n";
}

// ── 9. Boundary: exactly 22 bytes ────────────────────────────────────────────

void test_boundary() {
    SECTION("22 ASCII bytes stays SSO");
    { std::string s22(22, 'a');
      mlc::String ms(s22);
      CHECK(ms.is_sso());
      CHECK(ms.byte_size() == 22);
      CHECK(ms.length() == 22);
      CHECK(ms.as_std_string() == s22); }
    std::cout << "\n";

    SECTION("22-byte SSO + 1-byte SSO → heap");
    { mlc::String a(std::string(22, 'a')), b("!");
      mlc::String c = a + b;
      CHECK(!c.is_sso());
      CHECK(c.byte_size() == 23); }
    std::cout << "\n";

    SECTION("11 Cyrillic chars = 22 bytes → SSO");
    { mlc::String s("ПриветПривеТ");  // 12 chars but only need 11 for 22 bytes
      // "АБВГДЕЖЗИЙК" = 11×2 = 22 bytes
      mlc::String s2("АБВГДЕЖЗИЙК");
      CHECK(s2.byte_size() == 22);
      CHECK(s2.is_sso());
      CHECK(s2.length() == 11); }
    std::cout << "\n";
}

// ── 10. Array<String> join (tests the join optimization) ────────────────────

void test_array_join() {
    SECTION("join SSO strings");
    { mlc::Array<mlc::String> arr;
      arr.push_back(mlc::String("a"));
      arr.push_back(mlc::String("b"));
      arr.push_back(mlc::String("c"));
      mlc::String result = arr.join(mlc::String(","));
      CHECK(result.as_std_string() == "a,b,c"); }
    std::cout << "\n";

    SECTION("join empty array");
    { mlc::Array<mlc::String> arr;
      mlc::String result = arr.join(mlc::String(","));
      CHECK(result.as_std_string() == ""); }
    std::cout << "\n";

    SECTION("join single element");
    { mlc::Array<mlc::String> arr;
      arr.push_back(mlc::String("only"));
      CHECK(arr.join(mlc::String("|")).as_std_string() == "only"); }
    std::cout << "\n";

    SECTION("join with empty separator");
    { mlc::Array<mlc::String> arr;
      arr.push_back(mlc::String("foo"));
      arr.push_back(mlc::String("bar"));
      CHECK(arr.join(mlc::String("")).as_std_string() == "foobar"); }
    std::cout << "\n";
}

// ── 11. null bytes (for codegen literal encoding) ────────────────────────────

void test_null_bytes() {
    SECTION("String with explicit length handles null bytes");
    { const char data[] = {'a', '\0', 'b'};
      mlc::String s(data, 3);
      CHECK(s.byte_size() == 3);
      CHECK(s.is_sso()); }
    std::cout << "\n";
}

// ── 12. to_i() ───────────────────────────────────────────────────────────────

void test_to_i() {
    SECTION("to_i SSO");
    { CHECK(mlc::String("42").to_i() == 42);
      CHECK(mlc::String("-7").to_i() == -7);
      CHECK(mlc::String("0").to_i() == 0); }
    std::cout << "\n";
}

// ── main ──────────────────────────────────────────────────────────────────────

int main() {
    std::cout << "=== mlc::String SSO tests ===\n\n";

    std::cout << "1. Storage selection:\n";
    test_storage_selection();

    std::cout << "2. Content correctness:\n";
    test_content();

    std::cout << "3. Copy semantics:\n";
    test_copy();

    std::cout << "4. Concatenation:\n";
    test_concat();

    std::cout << "5. Comparison:\n";
    test_comparison();

    std::cout << "6. length() (UTF-8 chars):\n";
    test_length();

    std::cout << "7. char_at() (UTF-8 aware):\n";
    test_char_at();

    std::cout << "8. substring():\n";
    test_substring();

    std::cout << "9. Boundary (22 bytes):\n";
    test_boundary();

    std::cout << "10. Array<String>::join:\n";
    test_array_join();

    std::cout << "11. Null bytes:\n";
    test_null_bytes();

    std::cout << "12. to_i():\n";
    test_to_i();

    std::cout << "\n";
    if (failed == 0) {
        std::cout << "ALL " << passed << " checks PASSED\n";
    } else {
        std::cout << passed << " passed, " << failed << " FAILED\n";
    }
    return failed > 0 ? 1 : 0;
}
