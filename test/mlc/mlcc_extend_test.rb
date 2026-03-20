# frozen_string_literal: true

require "test_helper"

class MlccExtendTest < Minitest::Test
  PREAMBLE = <<~MLC
    type TKind = Eof | Ident(string) | LInt(i32) | LStr(string)
              | KFn | KType | KLet | KMut | KConst | KIf | KUnless | KElse | KEnd
              | KDo | KWhile | KReturn | KBreak | KContinue | KTrue | KFalse
              | KMatch | KFor | KExtern | KExtend | KThen
              | Equal | Bar | Pipe | Colon | Comma | Dot | Spread | Arrow | FatArrow
              | Question | LParen | RParen | LBrace | RBrace | LBracket | RBracket
              | Semicolon | Op(string)

    type Token = Token { kind: TKind, line: i32 }
    type Parser = Parser { tokens: [Token], pos: i32 }

    extend TKind {
      fn is_eof(self)    -> bool = match self { Eof     => true, _ => false }
      fn is_if(self)     -> bool = match self { KIf     => true, _ => false }
      fn is_unless(self) -> bool = match self { KUnless => true, _ => false }
      fn is_else(self)   -> bool = match self { KElse   => true, _ => false }
      fn is_end(self)    -> bool = match self { KEnd    => true, _ => false }
      fn is_do(self)     -> bool = match self { KDo     => true, _ => false }
      fn is_comma(self)  -> bool = match self { Comma   => true, _ => false }
      fn is_pipe(self)   -> bool = match self { Pipe    => true, _ => false }
      fn is_ident(self)  -> bool = match self { Ident(_) => true, _ => false }
      fn get_ident(self) -> string = match self { Ident(name) => name, _ => "" }
    }

    extend Parser {
      fn kind(self)      -> TKind  = tokens[pos].kind
      fn advance(self)   -> Parser = Parser { tokens, pos: pos + 1 }
      fn advance_by(self, count: i32) -> Parser = Parser { tokens, pos: pos + count }
      fn at_eof(self)    -> bool   = match tokens[pos].kind { Eof => true, _ => false }
      fn skip_semi(self) -> Parser =
        match tokens[pos].kind { Semicolon => self.advance(), _ => self }
    }

    fn make_parser(kinds: [TKind]) -> Parser = do
      let mut tokens: [Token] = []
      let mut index = 0
      while index < kinds.length() do
        tokens.push(Token { kind: kinds[index], line: 1 })
        index = index + 1
      end
      Parser { tokens, pos: 0 }
    end
  MLC

  def compile(source)
    MLC.to_cpp(source)
  end

  def test_extend_parser_kind_is_if
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> bool = do
        const parser = make_parser([KIf, Eof])
        parser.kind().is_if()
      end
    MLC
    assert_includes cpp, "Parser_kind("
    assert_includes cpp, "TKind_is_if("
  end

  def test_extend_parser_at_eof
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> bool = do
        const parser = make_parser([Eof])
        parser.at_eof()
      end
    MLC
    assert_includes cpp, "Parser_at_eof("
  end

  def test_extend_parser_advance_then_eof
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> bool = do
        const parser = make_parser([KIf, Eof])
        parser.advance().at_eof()
      end
    MLC
    assert_includes cpp, "Parser_advance("
    assert_includes cpp, "Parser_at_eof("
  end

  def test_extend_parser_skip_semi
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> bool = do
        const parser = make_parser([Semicolon, Eof])
        parser.skip_semi().at_eof()
      end
    MLC
    assert_includes cpp, "Parser_skip_semi("
  end

  def test_extend_parser_chain_kind_is_unless
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> bool = do
        const parser = make_parser([KUnless, Eof])
        parser.kind().is_unless()
      end
    MLC
    assert_includes cpp, "TKind_is_unless("
  end

  def test_extend_tkind_get_ident
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> string = do
        const parser = make_parser([Ident("hello"), Eof])
        parser.kind().get_ident()
      end
    MLC
    assert_includes cpp, "TKind_get_ident("
  end

  def test_mlcc_compiles_extend_block
    source = <<~MLC
      type Counter = Counter { value: i32 }
      extend Counter {
        fn increment(self) -> Counter = Counter { value: self.value + 1 }
        fn get(self) -> i32 = self.value
      }
      fn main() -> i32 = do
        const counter = Counter { value: 0 }
        counter.increment().get()
      end
    MLC
    cpp = compile(source)
    assert_includes cpp, "Counter_increment"
    assert_includes cpp, "Counter_get"
  end

  def test_record_shorthand_in_mlcc_source
    source = <<~MLC
      type Point = Point { x: i32, y: i32 }
      fn make(x: i32, y: i32) -> Point = Point { x, y }
      fn main() -> i32 = do
        const p = make(3, 4)
        p.x + p.y
      end
    MLC
    cpp = compile(source)
    assert_includes cpp, "Point{"
  end

  def test_extend_lexout_has_errors
    source = <<~MLC
      type LexOut = LexOut { errors: [string] }
      extend LexOut {
        fn has_errors(self) -> bool = errors.length() > 0
      }
      fn main() -> bool = do
        const out = LexOut { errors: [] }
        !out.has_errors()
      end
    MLC
    cpp = compile(source)
    assert_includes cpp, "LexOut_has_errors"
  end

  def test_extend_checkout_has_errors
    source = <<~MLC
      type CheckOut = CheckOut { errors: [string] }
      extend CheckOut {
        fn has_errors(self) -> bool = errors.length() > 0
      }
      fn main() -> bool = do
        const result = CheckOut { errors: [] }
        !result.has_errors()
      end
    MLC
    cpp = compile(source)
    assert_includes cpp, "CheckOut_has_errors"
  end

  def test_extend_token_line_column
    source = <<~MLC
      type Token = Token { kind: i32, line: i32, col: i32 }
      extend Token {
        fn kind_value(self) -> i32 = kind
        fn line_number(self) -> i32 = line
        fn column(self) -> i32 = col
      }
      fn main() -> i32 = do
        const token = Token { kind: 1, line: 5, col: 10 }
        token.line_number() + token.column()
      end
    MLC
    cpp = compile(source)
    assert_includes cpp, "Token_line_number"
    assert_includes cpp, "Token_column"
  end

  def test_record_shorthand_in_parser_advance
    cpp = compile(PREAMBLE + <<~MLC)
      fn main() -> i32 = do
        const parser = make_parser([KIf, KElse, Eof])
        const advanced = parser.advance()
        advanced.pos
      end
    MLC
    refute_nil cpp
  end
end
