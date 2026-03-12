#include "mlc/core/string.hpp"
#include "mlc/core/collections.hpp"
#include "mlc/core/hashmap.hpp"
#include "mlc/io/io.hpp"
#include "mlc/io/file.hpp"
#include "mlc/core/match.hpp"
#include "mlc/core/result.hpp"
struct KFn;
struct KType;
struct KLet;
struct KMut;
struct KConst;
struct KReturn;
struct KBreak;
struct KContinue;
struct KIf;
struct KThen;
struct KElse;
struct KUnless;
struct KWhile;
struct KFor;
struct KIn;
struct KDo;
struct KEnd;
struct KMatch;
struct KImport;
struct KFrom;
struct KAs;
struct KExtern;
struct KExtend;
struct KWhere;
struct KTrue;
struct KFalse;
struct LInt;
struct LStr;
struct Ident;
struct Arrow;
struct FatArrow;
struct Pipe;
struct Bar;
struct Equal;
struct Question;
struct Dot;
struct Spread;
struct LParen;
struct RParen;
struct LBrace;
struct RBrace;
struct LBracket;
struct RBracket;
struct Comma;
struct Semicolon;
struct Colon;
struct Op;
struct Eof;
using TKind = std::variant<KFn, KType, KLet, KMut, KConst, KReturn, KBreak, KContinue, KIf, KThen, KElse, KUnless, KWhile, KFor, KIn, KDo, KEnd, KMatch, KImport, KFrom, KAs, KExtern, KExtend, KWhere, KTrue, KFalse, LInt, LStr, Ident, Arrow, FatArrow, Pipe, Bar, Equal, Question, Dot, Spread, LParen, RParen, LBrace, RBrace, LBracket, RBracket, Comma, Semicolon, Colon, Op, Eof>;
struct TyI32;
struct TyString;
struct TyBool;
struct TyUnit;
struct TyNamed;
struct TyArray;
struct TyShared;
struct TyGeneric;
struct TyFn;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor>;
struct ExprInt;
struct ExprStr;
struct ExprBool;
struct ExprUnit;
struct ExprIdent;
struct ExprBin;
struct ExprUn;
struct ExprCall;
struct ExprMethod;
struct ExprField;
struct ExprIndex;
struct ExprIf;
struct ExprBlock;
struct ExprWhile;
struct ExprFor;
struct ExprMatch;
struct ExprRecord;
struct ExprRecordUpdate;
struct ExprArray;
struct ExprQuestion;
struct ExprLambda;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord>;
struct DeclFn;
struct DeclType;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported>;
struct TI32;
struct TString;
struct TBool;
struct TUnit;
struct TArray;
struct TShared;
struct TNamed;
struct TFn;
struct TUnknown;
using Type = std::variant<TI32, TString, TBool, TUnit, TArray, TShared, TNamed, TFn, TUnknown>;
struct CKAuto;
struct CKConst;
struct CKConstexpr;
struct CKIf;
struct CKElse;
struct CKWhile;
struct CKFor;
struct CKReturn;
struct CKBreak;
struct CKStruct;
struct CKUsing;
struct CKNoexcept;
struct CKInline;
struct CKStatic;
struct CKVoid;
struct CKInt;
struct CKBool;
struct CKChar;
struct CKTrue;
struct CKFalse;
struct CKNullptr;
struct CKTemplate;
struct CKTypename;
struct CKNamespace;
struct CKNew;
struct CKDelete;
struct CKOperator;
struct CIdent;
struct CLInt;
struct CLStr;
struct CArrow;
struct CScope;
struct CEq;
struct CNe;
struct CLe;
struct CGe;
struct CAnd;
struct COr;
struct CInc;
struct CDec;
struct CLShift;
struct CRShift;
struct CLParen;
struct CRParen;
struct CLBrace;
struct CRBrace;
struct CLBracket;
struct CRBracket;
struct CLAngle;
struct CRAngle;
struct COp;
struct CEof;
using CppTKind = std::variant<CKAuto, CKConst, CKConstexpr, CKIf, CKElse, CKWhile, CKFor, CKReturn, CKBreak, CKStruct, CKUsing, CKNoexcept, CKInline, CKStatic, CKVoid, CKInt, CKBool, CKChar, CKTrue, CKFalse, CKNullptr, CKTemplate, CKTypename, CKNamespace, CKNew, CKDelete, CKOperator, CIdent, CLInt, CLStr, CArrow, CScope, CEq, CNe, CLe, CGe, CAnd, COr, CInc, CDec, CLShift, CRShift, CLParen, CRParen, CLBrace, CRBrace, CLBracket, CRBracket, CLAngle, CRAngle, COp, CEof>;
struct CppInt;
struct CppStr;
struct CppIdent;
struct CppCall;
struct CppMember;
struct CppIndex;
struct CppBinary;
struct CppUnary;
using CppExpr = std::variant<CppInt, CppStr, CppIdent, CppCall, CppMember, CppIndex, CppBinary, CppUnary>;
struct CppAutoDecl;
struct CppReturn;
struct CppExprStmt;
struct CppBlock;
struct CppIf;
struct CppWhile;
using CppStmt = std::variant<CppAutoDecl, CppReturn, CppExprStmt, CppBlock, CppIf, CppWhile>;
struct CppInclude;
struct CppUsing;
struct CppStruct;
struct CppFnProto;
struct CppFnDef;
using CppDecl = std::variant<CppInclude, CppUsing, CppStruct, CppFnProto, CppFnDef>;
struct Pass;
struct Fail;
using TestResult = std::variant<Pass, Fail>;
struct KFn {};
struct KType {};
struct KLet {};
struct KMut {};
struct KConst {};
struct KReturn {};
struct KBreak {};
struct KContinue {};
struct KIf {};
struct KThen {};
struct KElse {};
struct KUnless {};
struct KWhile {};
struct KFor {};
struct KIn {};
struct KDo {};
struct KEnd {};
struct KMatch {};
struct KImport {};
struct KFrom {};
struct KAs {};
struct KExtern {};
struct KExtend {};
struct KWhere {};
struct KTrue {};
struct KFalse {};
struct LInt {int field0;};
struct LStr {mlc::String field0;};
struct Ident {mlc::String field0;};
struct Arrow {};
struct FatArrow {};
struct Pipe {};
struct Bar {};
struct Equal {};
struct Question {};
struct Dot {};
struct Spread {};
struct LParen {};
struct RParen {};
struct LBrace {};
struct RBrace {};
struct LBracket {};
struct RBracket {};
struct Comma {};
struct Semicolon {};
struct Colon {};
struct Op {mlc::String field0;};
struct Eof {};
struct Token {TKind kind;int line;int col;};
struct LexOut {mlc::Array<Token> tokens;mlc::Array<mlc::String> errors;};
struct TyI32 {};
struct TyString {};
struct TyBool {};
struct TyUnit {};
struct TyNamed {mlc::String field0;};
struct TyArray {std::shared_ptr<TypeExpr> field0;};
struct TyShared {std::shared_ptr<TypeExpr> field0;};
struct TyGeneric {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct TyFn {mlc::Array<std::shared_ptr<TypeExpr>> field0;std::shared_ptr<TypeExpr> field1;};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct FieldPat {mlc::String name;std::shared_ptr<Pat> pat;};
struct MatchArm {std::shared_ptr<Pat> pat;std::shared_ptr<Expr> body;};
struct FieldVal {mlc::String name;std::shared_ptr<Expr> val;};
struct ExprInt {int field0;};
struct ExprStr {mlc::String field0;};
struct ExprBool {bool field0;};
struct ExprUnit {};
struct ExprIdent {mlc::String field0;};
struct ExprBin {mlc::String field0;std::shared_ptr<Expr> field1;std::shared_ptr<Expr> field2;};
struct ExprUn {mlc::String field0;std::shared_ptr<Expr> field1;};
struct ExprCall {std::shared_ptr<Expr> field0;mlc::Array<std::shared_ptr<Expr>> field1;};
struct ExprMethod {std::shared_ptr<Expr> field0;mlc::String field1;mlc::Array<std::shared_ptr<Expr>> field2;};
struct ExprField {std::shared_ptr<Expr> field0;mlc::String field1;};
struct ExprIndex {std::shared_ptr<Expr> field0;std::shared_ptr<Expr> field1;};
struct ExprIf {std::shared_ptr<Expr> field0;std::shared_ptr<Expr> field1;std::shared_ptr<Expr> field2;};
struct ExprBlock {mlc::Array<std::shared_ptr<Stmt>> field0;std::shared_ptr<Expr> field1;};
struct ExprWhile {std::shared_ptr<Expr> field0;mlc::Array<std::shared_ptr<Stmt>> field1;};
struct ExprFor {mlc::String field0;std::shared_ptr<Expr> field1;mlc::Array<std::shared_ptr<Stmt>> field2;};
struct ExprMatch {std::shared_ptr<Expr> field0;mlc::Array<std::shared_ptr<MatchArm>> field1;};
struct ExprRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldVal>> field1;};
struct ExprRecordUpdate {mlc::String field0;std::shared_ptr<Expr> field1;mlc::Array<std::shared_ptr<FieldVal>> field2;};
struct ExprArray {mlc::Array<std::shared_ptr<Expr>> field0;};
struct ExprQuestion {std::shared_ptr<Expr> field0;};
struct ExprLambda {mlc::Array<mlc::String> field0;std::shared_ptr<Expr> field1;};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<Decl>> field1;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
struct LexState {mlc::String src;int pos;int line;int col;};
struct ScanResult {LexState state;Token token;};
struct ScanStrResult {LexState state;Token token;mlc::String error;};
struct Parser {mlc::Array<Token> tokens;int pos;};
struct ExprResult {std::shared_ptr<Expr> expr;Parser parser;};
struct TypeResult {std::shared_ptr<TypeExpr> type_expr;Parser parser;};
struct PatResult {std::shared_ptr<Pat> pat;Parser parser;};
struct StmtResult {std::shared_ptr<Stmt> stmt;Parser parser;};
struct StmtsResult {mlc::Array<std::shared_ptr<Stmt>> stmts;Parser parser;};
struct ExprsResult {mlc::Array<std::shared_ptr<Expr>> exprs;Parser parser;};
struct ArmsResult {mlc::Array<std::shared_ptr<MatchArm>> arms;Parser parser;};
struct VariantResult {std::shared_ptr<TypeVariant> variant;Parser parser;};
struct VariantsResult {mlc::Array<std::shared_ptr<TypeVariant>> variants;Parser parser;};
struct ParamsResult {mlc::Array<std::shared_ptr<Param>> params;Parser parser;};
struct DeclResult {std::shared_ptr<Decl> decl;Parser parser;};
struct FieldValsResult {mlc::Array<std::shared_ptr<FieldVal>> field_vals;Parser parser;};
struct PatsResult {mlc::Array<std::shared_ptr<Pat>> pats;Parser parser;};
struct FieldDefsResult {mlc::Array<std::shared_ptr<FieldDef>> field_defs;Parser parser;};
struct TypesResult {mlc::Array<std::shared_ptr<TypeExpr>> types;Parser parser;};
struct ParamResult {std::shared_ptr<Param> param;Parser parser;};
struct NamesResult {mlc::Array<mlc::String> exprs;Parser parser;};
struct NameCheckResult {mlc::Array<mlc::String> errors;mlc::Array<mlc::String> scope;};
struct TI32 {};
struct TString {};
struct TBool {};
struct TUnit {};
struct TArray {std::shared_ptr<Type> field0;};
struct TShared {std::shared_ptr<Type> field0;};
struct TNamed {mlc::String field0;};
struct TFn {mlc::Array<std::shared_ptr<Type>> field0;std::shared_ptr<Type> field1;};
struct TUnknown {};
struct TypeRegistry {mlc::HashMap<mlc::String, std::shared_ptr<Type>> fn_types;mlc::HashMap<mlc::String, std::shared_ptr<Type>> ctor_types;mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<Type>>> ctor_params;mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<Type>>> field_types;};
struct InferResult {std::shared_ptr<Type> inferred_type;mlc::Array<mlc::String> errors;};
struct StmtInferResult {mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env;mlc::Array<mlc::String> errors;};
struct CheckOut {mlc::Array<mlc::String> errors;TypeRegistry registry;};
struct FieldOrder {mlc::String type_name;mlc::Array<mlc::String> fields;};
struct CodegenContext {mlc::Array<std::shared_ptr<FieldOrder>> field_orders;};
struct CKAuto {};
struct CKConst {};
struct CKConstexpr {};
struct CKIf {};
struct CKElse {};
struct CKWhile {};
struct CKFor {};
struct CKReturn {};
struct CKBreak {};
struct CKStruct {};
struct CKUsing {};
struct CKNoexcept {};
struct CKInline {};
struct CKStatic {};
struct CKVoid {};
struct CKInt {};
struct CKBool {};
struct CKChar {};
struct CKTrue {};
struct CKFalse {};
struct CKNullptr {};
struct CKTemplate {};
struct CKTypename {};
struct CKNamespace {};
struct CKNew {};
struct CKDelete {};
struct CKOperator {};
struct CIdent {mlc::String field0;};
struct CLInt {int field0;};
struct CLStr {mlc::String field0;};
struct CArrow {};
struct CScope {};
struct CEq {};
struct CNe {};
struct CLe {};
struct CGe {};
struct CAnd {};
struct COr {};
struct CInc {};
struct CDec {};
struct CLShift {};
struct CRShift {};
struct CLParen {};
struct CRParen {};
struct CLBrace {};
struct CRBrace {};
struct CLBracket {};
struct CRBracket {};
struct CLAngle {};
struct CRAngle {};
struct COp {mlc::String field0;};
struct CEof {};
struct CppToken {CppTKind kind;int line;int col;};
struct CppLexOut {mlc::Array<CppToken> tokens;mlc::Array<mlc::String> errors;};
struct CppScanIdR {LexState st;CppToken tok;};
struct CppScanIntR {LexState st;CppToken tok;};
struct CppScanStrR {LexState st;CppToken tok;mlc::String err;};
struct CppScanOpR {LexState st;CppToken tok;};
struct CppInt {int field0;};
struct CppStr {mlc::String field0;};
struct CppIdent {mlc::String field0;};
struct CppCall {std::shared_ptr<CppExpr> field0;mlc::Array<std::shared_ptr<CppExpr>> field1;};
struct CppMember {std::shared_ptr<CppExpr> field0;mlc::String field1;};
struct CppIndex {std::shared_ptr<CppExpr> field0;std::shared_ptr<CppExpr> field1;};
struct CppBinary {mlc::String field0;std::shared_ptr<CppExpr> field1;std::shared_ptr<CppExpr> field2;};
struct CppUnary {mlc::String field0;std::shared_ptr<CppExpr> field1;};
struct CppAutoDecl {mlc::String field0;std::shared_ptr<CppExpr> field1;};
struct CppReturn {std::shared_ptr<CppExpr> field0;};
struct CppExprStmt {std::shared_ptr<CppExpr> field0;};
struct CppBlock {mlc::Array<std::shared_ptr<CppStmt>> field0;};
struct CppIf {std::shared_ptr<CppExpr> field0;std::shared_ptr<CppStmt> field1;std::shared_ptr<CppStmt> field2;};
struct CppWhile {std::shared_ptr<CppExpr> field0;std::shared_ptr<CppStmt> field1;};
struct CppField {mlc::String typ;mlc::String name;};
struct CppInclude {bool field0;mlc::String field1;};
struct CppUsing {mlc::String field0;mlc::String field1;};
struct CppStruct {mlc::String field0;mlc::Array<std::shared_ptr<CppField>> field1;};
struct CppFnProto {mlc::String field0;mlc::String field1;mlc::Array<mlc::String> field2;};
struct CppFnDef {mlc::String field0;mlc::String field1;mlc::Array<mlc::String> field2;mlc::Array<std::shared_ptr<CppStmt>> field3;};
struct CppProgram {mlc::Array<std::shared_ptr<CppDecl>> decls;};
struct Pass {mlc::String name;};
struct Fail {mlc::String name;mlc::String message;};
constexpr bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
constexpr int Token_line_number(Token self) noexcept;
constexpr int Token_column(Token self) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
constexpr bool LexState_eof(LexState self) noexcept;
mlc::String LexState_current(LexState self) noexcept;
mlc::String LexState_peek(LexState self, int offset) noexcept;
LexState LexState_advance(LexState self) noexcept;
LexState LexState_advance_by(LexState self, int count) noexcept;
Token LexState_token(LexState self, TKind kind) noexcept;
constexpr bool is_alpha(mlc::String character) noexcept;
constexpr bool is_digit(mlc::String character) noexcept;
constexpr bool is_alnum(mlc::String character) noexcept;
constexpr bool is_ws(mlc::String character) noexcept;
TKind keyword_kind(mlc::String word) noexcept;
ScanResult scan_ident(LexState state) noexcept;
ScanResult scan_int(LexState state) noexcept;
mlc::String map_escape(mlc::String character) noexcept;
ScanStrResult scan_string(LexState state) noexcept;
LexState skip_whitespace(LexState state) noexcept;
ScanResult scan_op(LexState state) noexcept;
LexOut tokenize(mlc::String source) noexcept;
constexpr int lex_token_count(mlc::String source) noexcept;
TKind lex_first_kind(mlc::String source) noexcept;
constexpr int lex_error_count(mlc::String source) noexcept;
mlc::Array<TestResult> lexer_tests() noexcept;
Parser parser_new(mlc::Array<Token> tokens) noexcept;
TKind parser_kind(Parser parser) noexcept;
Parser parser_advance(Parser parser) noexcept;
Parser parser_advance_by(Parser parser, int count) noexcept;
bool parser_at_eof(Parser parser) noexcept;
Parser parser_skip_semi(Parser parser) noexcept;
bool is_keyword_fn(TKind kind) noexcept;
bool is_keyword_type(TKind kind) noexcept;
bool is_keyword_let(TKind kind) noexcept;
bool is_keyword_mut(TKind kind) noexcept;
bool is_keyword_const(TKind kind) noexcept;
bool is_keyword_if(TKind kind) noexcept;
bool is_keyword_unless(TKind kind) noexcept;
bool is_keyword_else(TKind kind) noexcept;
bool is_keyword_end(TKind kind) noexcept;
bool is_keyword_do(TKind kind) noexcept;
bool is_keyword_while(TKind kind) noexcept;
bool is_keyword_for(TKind kind) noexcept;
bool is_keyword_match(TKind kind) noexcept;
bool is_keyword_return(TKind kind) noexcept;
bool is_keyword_break(TKind kind) noexcept;
bool is_keyword_continue(TKind kind) noexcept;
bool is_keyword_true(TKind kind) noexcept;
bool is_keyword_false(TKind kind) noexcept;
bool is_keyword_extern(TKind kind) noexcept;
bool is_keyword_extend(TKind kind) noexcept;
bool is_ident(TKind kind) noexcept;
bool is_int_literal(TKind kind) noexcept;
bool is_str_literal(TKind kind) noexcept;
bool is_equal(TKind kind) noexcept;
bool is_bar(TKind kind) noexcept;
bool is_pipe(TKind kind) noexcept;
bool is_colon(TKind kind) noexcept;
bool is_comma(TKind kind) noexcept;
bool is_dot(TKind kind) noexcept;
bool is_spread(TKind kind) noexcept;
bool is_arrow(TKind kind) noexcept;
bool is_fat_arrow(TKind kind) noexcept;
bool is_question(TKind kind) noexcept;
bool is_left_paren(TKind kind) noexcept;
bool is_right_paren(TKind kind) noexcept;
bool is_left_brace(TKind kind) noexcept;
bool is_right_brace(TKind kind) noexcept;
bool is_left_bracket(TKind kind) noexcept;
bool is_right_bracket(TKind kind) noexcept;
bool is_operator(TKind kind) noexcept;
mlc::String get_ident(TKind kind) noexcept;
int get_int_literal(TKind kind) noexcept;
mlc::String get_str_literal(TKind kind) noexcept;
mlc::String get_operator(TKind kind) noexcept;
bool is_ctor_name(mlc::String name) noexcept;
bool TKind_is_fn(TKind self) noexcept;
bool TKind_is_type(TKind self) noexcept;
bool TKind_is_let(TKind self) noexcept;
bool TKind_is_mut(TKind self) noexcept;
bool TKind_is_const(TKind self) noexcept;
bool TKind_is_if(TKind self) noexcept;
bool TKind_is_unless(TKind self) noexcept;
bool TKind_is_else(TKind self) noexcept;
bool TKind_is_end(TKind self) noexcept;
bool TKind_is_do(TKind self) noexcept;
bool TKind_is_while(TKind self) noexcept;
bool TKind_is_for(TKind self) noexcept;
bool TKind_is_match(TKind self) noexcept;
bool TKind_is_return(TKind self) noexcept;
bool TKind_is_break(TKind self) noexcept;
bool TKind_is_continue(TKind self) noexcept;
bool TKind_is_true(TKind self) noexcept;
bool TKind_is_false(TKind self) noexcept;
bool TKind_is_extern(TKind self) noexcept;
bool TKind_is_extend(TKind self) noexcept;
bool TKind_is_import(TKind self) noexcept;
bool TKind_is_from(TKind self) noexcept;
bool TKind_is_eof(TKind self) noexcept;
bool TKind_is_ident(TKind self) noexcept;
bool TKind_is_int(TKind self) noexcept;
bool TKind_is_str(TKind self) noexcept;
bool TKind_is_equal(TKind self) noexcept;
bool TKind_is_bar(TKind self) noexcept;
bool TKind_is_pipe(TKind self) noexcept;
bool TKind_is_colon(TKind self) noexcept;
bool TKind_is_comma(TKind self) noexcept;
bool TKind_is_dot(TKind self) noexcept;
bool TKind_is_spread(TKind self) noexcept;
bool TKind_is_arrow(TKind self) noexcept;
bool TKind_is_fat_arrow(TKind self) noexcept;
bool TKind_is_question(TKind self) noexcept;
bool TKind_is_lparen(TKind self) noexcept;
bool TKind_is_rparen(TKind self) noexcept;
bool TKind_is_lbrace(TKind self) noexcept;
bool TKind_is_rbrace(TKind self) noexcept;
bool TKind_is_lbracket(TKind self) noexcept;
bool TKind_is_rbracket(TKind self) noexcept;
bool TKind_is_op(TKind self) noexcept;
mlc::String TKind_ident(TKind self) noexcept;
int TKind_int_val(TKind self) noexcept;
mlc::String TKind_str_val(TKind self) noexcept;
mlc::String TKind_op_val(TKind self) noexcept;
TKind Parser_kind(Parser self) noexcept;
Parser Parser_advance(Parser self) noexcept;
Parser Parser_advance_by(Parser self, int count) noexcept;
bool Parser_at_eof(Parser self) noexcept;
Parser Parser_skip_semi(Parser self) noexcept;
TypeResult parse_type(Parser parser) noexcept;
TypeResult parse_base_type(Parser parser) noexcept;
TypesResult parse_type_args(Parser parser) noexcept;
StmtsResult parse_stmts_until_end(Parser parser) noexcept;
StmtsResult parse_stmts_until_else_end(Parser parser) noexcept;
StmtResult parse_stmt(Parser parser) noexcept;
PatResult parse_pat(Parser parser) noexcept;
PatsResult parse_pat_args(Parser parser) noexcept;
ExprResult parse_expr(Parser parser) noexcept;
std::shared_ptr<Expr> pipe_desugar(std::shared_ptr<Expr> left_expr, std::shared_ptr<Expr> right_expr) noexcept;
ExprResult parse_pipe(Parser parser) noexcept;
ExprResult parse_or(Parser parser) noexcept;
ExprResult parse_and(Parser parser) noexcept;
constexpr bool is_cmp_op(mlc::String op) noexcept;
ExprResult parse_cmp(Parser parser) noexcept;
ExprResult parse_add(Parser parser) noexcept;
ExprResult parse_mul(Parser parser) noexcept;
ExprResult parse_unary(Parser parser) noexcept;
ExprResult parse_postfix(Parser parser) noexcept;
ExprsResult parse_arg_list(Parser parser) noexcept;
ExprResult parse_primary(Parser parser) noexcept;
bool looks_like_lambda_params(Parser parser) noexcept;
NamesResult parse_lambda_params(Parser parser) noexcept;
ExprResult parse_array_lit(Parser parser) noexcept;
ExprResult parse_if_expr(Parser parser) noexcept;
ExprResult parse_block(Parser parser) noexcept;
bool is_unit_expr(std::shared_ptr<Expr> expr) noexcept;
std::shared_ptr<Expr> block_result(mlc::Array<std::shared_ptr<Stmt>> stmts) noexcept;
mlc::Array<std::shared_ptr<Stmt>> block_body(mlc::Array<std::shared_ptr<Stmt>> stmts) noexcept;
std::shared_ptr<Expr> StmtsResult_to_block_expr(StmtsResult self) noexcept;
ExprResult parse_while_expr(Parser parser) noexcept;
ExprResult parse_for_expr(Parser parser) noexcept;
ExprResult parse_match_expr(Parser parser) noexcept;
ArmsResult parse_arms(Parser parser) noexcept;
FieldValsResult parse_record_fields(Parser parser) noexcept;
ExprResult parse_record_expr(Parser parser, mlc::String record_name) noexcept;
DeclResult parse_import_decl(Parser parser) noexcept;
DeclResult parse_extend_decl(Parser parser) noexcept;
DeclResult parse_extend_method(Parser parser, mlc::String type_name) noexcept;
DeclResult parse_decl(Parser parser) noexcept;
DeclResult parse_fn_decl(Parser parser) noexcept;
ParamsResult parse_params(Parser parser) noexcept;
ParamResult parse_param(Parser parser) noexcept;
DeclResult parse_type_decl(Parser parser) noexcept;
bool is_decl_start(TKind kind) noexcept;
bool parse_variants_continue(Parser state) noexcept;
VariantsResult parse_variants(Parser parser) noexcept;
VariantResult parse_variant(Parser parser) noexcept;
TypesResult parse_tuple_types(Parser parser) noexcept;
FieldDefsResult parse_field_defs(Parser parser) noexcept;
Program parse_program(mlc::Array<Token> tokens) noexcept;
Program parse_source(mlc::String source) noexcept;
constexpr int decl_count(mlc::String source) noexcept;
std::shared_ptr<Decl> first_decl(mlc::String source) noexcept;
mlc::Array<TestResult> parser_tests() noexcept;
bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept;
mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<Pat> pattern) noexcept;
mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<Pat> pattern, mlc::Array<mlc::String> accumulator) noexcept;
mlc::Array<mlc::String> collect_globals(Program prog) noexcept;
mlc::Array<mlc::String> check_names_expr(std::shared_ptr<Expr> expr, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept;
constexpr mlc::Array<mlc::String> NameCheckResult_append_expr_errors(NameCheckResult self, mlc::Array<mlc::String> expr_errors) noexcept;
NameCheckResult check_names_stmts(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept;
std::shared_ptr<Type> TypeRegistry_fn_type(TypeRegistry self, mlc::String name) noexcept;
constexpr bool TypeRegistry_has_fn(TypeRegistry self, mlc::String name) noexcept;
std::shared_ptr<Type> TypeRegistry_ctor_type(TypeRegistry self, mlc::String name) noexcept;
constexpr bool TypeRegistry_has_ctor(TypeRegistry self, mlc::String name) noexcept;
mlc::Array<std::shared_ptr<Type>> TypeRegistry_ctor_params_for(TypeRegistry self, mlc::String name) noexcept;
constexpr mlc::HashMap<mlc::String, std::shared_ptr<Type>> TypeRegistry_fields_for(TypeRegistry self, mlc::String type_name) noexcept;
constexpr bool TypeRegistry_has_fields(TypeRegistry self, mlc::String type_name) noexcept;
TypeRegistry empty_registry() noexcept;
std::shared_ptr<Type> type_from_annotation(std::shared_ptr<TypeExpr> type_expr) noexcept;
TypeRegistry build_registry(Program prog) noexcept;
TypeRegistry register_decl(TypeRegistry registry, std::shared_ptr<Decl> decl) noexcept;
TypeRegistry register_variant(TypeRegistry registry, mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept;
InferResult InferResult_with_type(InferResult self, std::shared_ptr<Type> new_type) noexcept;
InferResult InferResult_absorb(InferResult self, InferResult other) noexcept;
InferResult InferResult_absorb_stmt(InferResult self, StmtInferResult stmt_result) noexcept;
InferResult infer_ok(std::shared_ptr<Type> type_val) noexcept;
std::shared_ptr<Type> binary_op_type(mlc::String op, std::shared_ptr<Type> left_type) noexcept;
std::shared_ptr<Type> method_return_type(mlc::String method_name) noexcept;
InferResult infer_args_errors(InferResult initial, mlc::Array<std::shared_ptr<Expr>> args, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept;
InferResult infer_field_vals_errors(InferResult initial, mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept;
std::shared_ptr<Type> field_lookup_type(std::shared_ptr<Type> obj_type, mlc::String field_name, TypeRegistry registry) noexcept;
InferResult infer_expr(std::shared_ptr<Expr> expr, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept;
StmtInferResult infer_stmts(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<Type>> env_for_pat(mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, std::shared_ptr<Pat> pat, TypeRegistry registry) noexcept;
mlc::HashMap<mlc::String, std::shared_ptr<Type>> env_for_pat_with_type(mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, std::shared_ptr<Pat> pat, std::shared_ptr<Type> type_val, TypeRegistry registry) noexcept;
constexpr bool CheckOut_has_errors(CheckOut self) noexcept;
CheckOut check(Program prog) noexcept;
CheckOut check_source(mlc::String source) noexcept;
constexpr int check_error_count(mlc::String source) noexcept;
mlc::Array<TestResult> checker_tests() noexcept;
CodegenContext create_codegen_context(Program prog) noexcept;
mlc::Array<std::shared_ptr<FieldOrder>> build_field_orders(Program prog) noexcept;
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept;
constexpr bool cpp_keyword(mlc::String name) noexcept;
mlc::String cpp_safe(mlc::String name) noexcept;
mlc::String lower_first(mlc::String name) noexcept;
mlc::String type_to_cpp(std::shared_ptr<TypeExpr> type_expr) noexcept;
mlc::String escape_str(mlc::String input) noexcept;
mlc::String gen_args(mlc::Array<std::shared_ptr<Expr>> args, CodegenContext ctx) noexcept;
mlc::String map_method(mlc::String method_name) noexcept;
mlc::String map_builtin(mlc::String name) noexcept;
mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<Stmt>> stmts, CodegenContext ctx) noexcept;
mlc::String gen_block_body(std::shared_ptr<Expr> expr, CodegenContext ctx) noexcept;
mlc::String gen_expr(std::shared_ptr<Expr> expr, CodegenContext ctx) noexcept;
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::Array<mlc::String> field_order, CodegenContext ctx) noexcept;
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<FieldVal>> field_vals, CodegenContext ctx) noexcept;
mlc::String gen_record_update_ordered(std::shared_ptr<Expr> base, mlc::Array<std::shared_ptr<FieldVal>> overrides, mlc::Array<mlc::String> field_order, CodegenContext ctx) noexcept;
std::shared_ptr<Expr> find_field_val(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::String name) noexcept;
mlc::String gen_match(std::shared_ptr<Expr> subject, mlc::Array<std::shared_ptr<MatchArm>> arms, CodegenContext ctx) noexcept;
mlc::String gen_arm(std::shared_ptr<MatchArm> arm, CodegenContext ctx) noexcept;
mlc::String gen_pat_header(std::shared_ptr<Pat> pat) noexcept;
mlc::String pat_bind_names(mlc::Array<std::shared_ptr<Pat>> sub_pats) noexcept;
mlc::String gen_stmt(std::shared_ptr<Stmt> stmt, CodegenContext ctx) noexcept;
mlc::String gen_stmt_expr(std::shared_ptr<Expr> expr, CodegenContext ctx) noexcept;
mlc::String gen_return_body(std::shared_ptr<Expr> expr, CodegenContext ctx) noexcept;
mlc::String gen_return_if_stmt(std::shared_ptr<Expr> expr, CodegenContext ctx) noexcept;
mlc::String gen_fn_body(std::shared_ptr<Expr> body, CodegenContext ctx) noexcept;
mlc::String gen_params(mlc::Array<std::shared_ptr<Param>> params) noexcept;
mlc::String gen_type_decl_fwd(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept;
mlc::String gen_type_decl_body(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept;
mlc::String gen_single_variant(mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept;
mlc::String gen_adt_fwd(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept;
mlc::String gen_adt_defs(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept;
mlc::String variant_ctor_name(std::shared_ptr<TypeVariant> variant) noexcept;
mlc::String gen_variant_struct(mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept;
mlc::String gen_fn_decl(mlc::String name, mlc::Array<std::shared_ptr<Param>> params, std::shared_ptr<TypeExpr> return_type, std::shared_ptr<Expr> body, CodegenContext ctx) noexcept;
mlc::String gen_fn_proto(mlc::String name, mlc::Array<std::shared_ptr<Param>> params, std::shared_ptr<TypeExpr> return_type) noexcept;
mlc::String gen_type_decl(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept;
mlc::String gen_decl(std::shared_ptr<Decl> decl, CodegenContext ctx) noexcept;
mlc::String gen_proto(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<Decl>> decls, CodegenContext ctx, int phase) noexcept;
mlc::String gen_program(Program prog) noexcept;
mlc::Array<TestResult> codegen_tests() noexcept;
mlc::Array<TestResult> pipe_and_record_update_tests() noexcept;
CppTKind cpp_keyword_kind(mlc::String word) noexcept;
CppScanIdR cpp_scan_ident(LexState state) noexcept;
CppScanIntR cpp_scan_int(LexState state) noexcept;
CppScanStrR cpp_scan_string(LexState state) noexcept;
CppScanOpR cpp_scan_op(LexState state) noexcept;
CppLexOut cpp_tokenize(mlc::String source) noexcept;
constexpr int cpp_tok_count(mlc::String src) noexcept;
CppTKind cpp_kind_at(mlc::String src, int i) noexcept;
CppTKind cpp_first_kind(mlc::String src) noexcept;
mlc::String cpp_ident_at(mlc::String src, int i) noexcept;
mlc::String cpp_str_at(mlc::String src, int i) noexcept;
int cpp_int_at(mlc::String src, int i) noexcept;
mlc::Array<TestResult> cpp_lexer_tests() noexcept;
mlc::Array<TestResult> cpp_ast_tests() noexcept;
TestResult assert_true(mlc::String test_name, bool condition) noexcept;
TestResult assert_eq_int(mlc::String test_name, int actual, int expected) noexcept;
TestResult assert_eq_str(mlc::String test_name, mlc::String actual, mlc::String expected) noexcept;
mlc::String run_all(mlc::Array<TestResult> results) noexcept;
mlc::Array<TestResult> all_tests() noexcept;
int main(int argc, char** argv) noexcept;
constexpr bool LexOut_has_errors(LexOut self) noexcept{return self.errors.size() > 0;}
TKind Token_kind_value(Token self) noexcept{return self.kind;}
constexpr int Token_line_number(Token self) noexcept{return self.line;}
constexpr int Token_column(Token self) noexcept{return self.col;}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{return [&]() -> std::shared_ptr<Decl> { if (std::holds_alternative<DeclExported>((*decl))) { auto _v_declexported = std::get<DeclExported>((*decl)); auto [d] = _v_declexported; return d; } return decl; }();}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{return std::visit(overloaded{
  [&](const DeclFn& declfn) { auto [name, _w0, _w1, _w2] = declfn; return name; },
  [&](const DeclType& decltype_) { auto [name, _w0] = decltype_; return name; },
  [&](const DeclExtend& declextend) { auto [type_name, _w0] = declextend; return type_name; },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) { auto [d] = declexported; return decl_name(d); }
}, (*decl));}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
int i = 0;
while (i < src.size()){
{
dst.push_back(src[i]);
i = i + 1;
}
}
return dst;
}
constexpr bool LexState_eof(LexState self) noexcept{return self.pos >= self.src.length();}
mlc::String LexState_current(LexState self) noexcept{return LexState_eof(self) ? mlc::String("\0") : self.src.char_at(self.pos);}
mlc::String LexState_peek(LexState self, int offset) noexcept{
int position = self.pos + offset;
return position >= self.src.length() ? mlc::String("\0") : self.src.char_at(position);
}
LexState LexState_advance(LexState self) noexcept{return LexState_current(self) == mlc::String("\n") ? LexState{self.src, self.pos + 1, self.line + 1, 1} : LexState{self.src, self.pos + 1, self.line, self.col + 1};}
LexState LexState_advance_by(LexState self, int count) noexcept{
LexState state = std::move(self);
int i = 0;
while (i < count){
{
state = LexState_advance(state);
i = i + 1;
}
}
return state;
}
Token LexState_token(LexState self, TKind kind) noexcept{return Token{kind, self.line, self.col};}
constexpr bool is_alpha(mlc::String character) noexcept{return character >= mlc::String("a") && character <= mlc::String("z") || character >= mlc::String("A") && character <= mlc::String("Z") || character == mlc::String("_");}
constexpr bool is_digit(mlc::String character) noexcept{return character >= mlc::String("0") && character <= mlc::String("9");}
constexpr bool is_alnum(mlc::String character) noexcept{return is_alpha(character) || is_digit(character);}
constexpr bool is_ws(mlc::String character) noexcept{return character == mlc::String(" ") || character == mlc::String("\t") || character == mlc::String("\r") || character == mlc::String("\n");}
TKind keyword_kind(mlc::String word) noexcept{return word == mlc::String("fn") ? TKind(KFn{}) : TKind(word == mlc::String("type") ? TKind(KType{}) : TKind(word == mlc::String("let") ? TKind(KLet{}) : TKind(word == mlc::String("mut") ? TKind(KMut{}) : TKind(word == mlc::String("const") ? TKind(KConst{}) : TKind(word == mlc::String("return") ? TKind(KReturn{}) : TKind(word == mlc::String("break") ? TKind(KBreak{}) : TKind(word == mlc::String("continue") ? TKind(KContinue{}) : TKind(word == mlc::String("if") ? TKind(KIf{}) : TKind(word == mlc::String("then") ? TKind(KThen{}) : TKind(word == mlc::String("else") ? TKind(KElse{}) : TKind(word == mlc::String("unless") ? TKind(KUnless{}) : TKind(word == mlc::String("while") ? TKind(KWhile{}) : TKind(word == mlc::String("for") ? TKind(KFor{}) : TKind(word == mlc::String("in") ? TKind(KIn{}) : TKind(word == mlc::String("do") ? TKind(KDo{}) : TKind(word == mlc::String("end") ? TKind(KEnd{}) : TKind(word == mlc::String("match") ? TKind(KMatch{}) : TKind(word == mlc::String("import") ? TKind(KImport{}) : TKind(word == mlc::String("from") ? TKind(KFrom{}) : TKind(word == mlc::String("as") ? TKind(KAs{}) : TKind(word == mlc::String("extern") ? TKind(KExtern{}) : TKind(word == mlc::String("extend") ? TKind(KExtend{}) : TKind(word == mlc::String("where") ? TKind(KWhere{}) : TKind(word == mlc::String("true") ? TKind(KTrue{}) : TKind(word == mlc::String("false") ? TKind(KFalse{}) : TKind(Ident(word)))))))))))))))))))))))))));}
ScanResult scan_ident(LexState state) noexcept{
int start = state.pos;
mlc::String source = state.src;
int token_line = state.line;
int token_col = state.col;
LexState current = std::move(state);
while (!LexState_eof(current) && is_alnum(LexState_current(current))){
{
current = LexState_advance(current);
}
}
mlc::String word = source.substring(start, current.pos - start);
return ScanResult{current, Token{keyword_kind(word), token_line, token_col}};
}
ScanResult scan_int(LexState state) noexcept{
int token_line = state.line;
int token_col = state.col;
LexState current = std::move(state);
int value = 0;
while (!LexState_eof(current) && is_digit(LexState_current(current))){
{
value = value * 10 + LexState_current(current).to_i();
current = LexState_advance(current);
}
}
return ScanResult{current, Token{LInt(value), token_line, token_col}};
}
mlc::String map_escape(mlc::String character) noexcept{return character == mlc::String("n") ? mlc::String("\n") : character == mlc::String("t") ? mlc::String("\t") : character == mlc::String("r") ? mlc::String("\r") : character == mlc::String("\"") ? mlc::String("\"") : character == mlc::String("\\") ? mlc::String("\\") : character == mlc::String("0") ? mlc::String("\0") : character == mlc::String("{") ? mlc::String("{") : character == mlc::String("}") ? mlc::String("}") : mlc::String("");}
ScanStrResult scan_string(LexState state) noexcept{
int token_line = state.line;
int token_col = state.col;
LexState current = LexState_advance(state);
mlc::Array<mlc::String> parts = {};
mlc::String error = mlc::String("");
while (!LexState_eof(current) && LexState_current(current) != mlc::String("\"")){
{
if (LexState_current(current) == mlc::String("\\")){
{
current = LexState_advance(current);
mlc::String mapped = map_escape(LexState_current(current));
if (mapped != mlc::String("")){
{
parts.push_back(mapped);
}
} else {
{
parts.push_back(mlc::String("\\"));
parts.push_back(LexState_current(current));
}
}
current = LexState_advance(current);
}
} else {
{
parts.push_back(LexState_current(current));
current = LexState_advance(current);
}
}
}
}
if (LexState_eof(current)){
{
error = mlc::String("unterminated string");
}
} else {
{
current = LexState_advance(current);
}
}
return ScanStrResult{current, Token{LStr(parts.join(mlc::String(""))), token_line, token_col}, error};
}
LexState skip_whitespace(LexState state) noexcept{
LexState current = std::move(state);
while (!LexState_eof(current)){
{
mlc::String character = LexState_current(current);
if (is_ws(character)){
{
current = LexState_advance(current);
}
} else {
{
if (character == mlc::String("/") && LexState_peek(current, 1) == mlc::String("/")){
current = LexState_advance_by(current, 2);
[&]() { 
  while (!LexState_eof(current) && LexState_current(current) != mlc::String("\n")){
{
current = LexState_advance(current);
}
}
 }();
} else {
if (character == mlc::String("/") && LexState_peek(current, 1) == mlc::String("*")){
current = LexState_advance_by(current, 2);
[&]() { 
  while (!LexState_eof(current)){
{
if (LexState_current(current) == mlc::String("*") && LexState_peek(current, 1) == mlc::String("/")){
{
current = LexState_advance_by(current, 2);
break;
}
}
current = LexState_advance(current);
}
}
 }();
} else {
break;
}
}
}
}
}
}
return current;
}
ScanResult scan_op(LexState state) noexcept{
int token_line = state.line;
int token_col = state.col;
mlc::String character = LexState_current(state);
mlc::String next = LexState_peek(state, 1);
return character == mlc::String("-") && next == mlc::String(">") ? ScanResult{LexState_advance_by(state, 2), LexState_token(state, Arrow{})} : character == mlc::String("=") && next == mlc::String(">") ? ScanResult{LexState_advance_by(state, 2), LexState_token(state, FatArrow{})} : character == mlc::String("|") && next == mlc::String(">") ? ScanResult{LexState_advance_by(state, 2), LexState_token(state, Pipe{})} : character == mlc::String("=") && next == mlc::String("=") ? ScanResult{LexState_advance_by(state, 2), LexState_token(state, Op(mlc::String("==")))} : character == mlc::String("!") && next == mlc::String("=") ? ScanResult{LexState_advance_by(state, 2), LexState_token(state, Op(mlc::String("!=")))} : character == mlc::String("<") && next == mlc::String("=") ? ScanResult{LexState_advance_by(state, 2), LexState_token(state, Op(mlc::String("<=")))} : character == mlc::String(">") && next == mlc::String("=") ? ScanResult{LexState_advance_by(state, 2), LexState_token(state, Op(mlc::String(">=")))} : character == mlc::String("&") && next == mlc::String("&") ? ScanResult{LexState_advance_by(state, 2), LexState_token(state, Op(mlc::String("&&")))} : character == mlc::String("|") && next == mlc::String("|") ? ScanResult{LexState_advance_by(state, 2), LexState_token(state, Op(mlc::String("||")))} : character == mlc::String("=") ? ScanResult{LexState_advance(state), LexState_token(state, Equal{})} : character == mlc::String("|") ? ScanResult{LexState_advance(state), LexState_token(state, Bar{})} : character == mlc::String("?") ? ScanResult{LexState_advance(state), LexState_token(state, Question{})} : character == mlc::String(".") && next == mlc::String(".") && LexState_peek(state, 2) == mlc::String(".") ? ScanResult{LexState_advance_by(state, 3), LexState_token(state, Spread{})} : character == mlc::String(".") ? ScanResult{LexState_advance(state), LexState_token(state, Dot{})} : character == mlc::String("(") ? ScanResult{LexState_advance(state), LexState_token(state, LParen{})} : character == mlc::String(")") ? ScanResult{LexState_advance(state), LexState_token(state, RParen{})} : character == mlc::String("{") ? ScanResult{LexState_advance(state), LexState_token(state, LBrace{})} : character == mlc::String("}") ? ScanResult{LexState_advance(state), LexState_token(state, RBrace{})} : character == mlc::String("[") ? ScanResult{LexState_advance(state), LexState_token(state, LBracket{})} : character == mlc::String("]") ? ScanResult{LexState_advance(state), LexState_token(state, RBracket{})} : character == mlc::String(",") ? ScanResult{LexState_advance(state), LexState_token(state, Comma{})} : character == mlc::String(";") ? ScanResult{LexState_advance(state), LexState_token(state, Semicolon{})} : character == mlc::String(":") ? ScanResult{LexState_advance(state), LexState_token(state, Colon{})} : ScanResult{LexState_advance(state), LexState_token(state, Op(character))};
}
LexOut tokenize(mlc::String source) noexcept{
LexState state = LexState{source, 0, 1, 1};
mlc::Array<Token> tokens = {};
mlc::Array<mlc::String> errors = {};
while (!LexState_eof(state)){
{
state = skip_whitespace(state);
if (LexState_eof(state)){
{
break;
}
}
mlc::String character = LexState_current(state);
if (is_alpha(character)){
{
ScanResult result = scan_ident(state);
tokens.push_back(result.token);
state = result.state;
}
} else {
{
if (is_digit(character)){
ScanResult result = scan_int(state);
tokens.push_back(result.token);
state = result.state;
} else {
if (character == mlc::String("\"")){
ScanStrResult result = scan_string(state);
tokens.push_back(result.token);
state = result.state;
if (result.error != mlc::String("")){
errors.push_back(result.error);
}
} else {
ScanResult result = scan_op(state);
tokens.push_back(result.token);
state = result.state;
}
}
}
}
}
}
tokens.push_back(LexState_token(state, Eof{}));
return LexOut{tokens, errors};
}
constexpr int lex_token_count(mlc::String source) noexcept{return tokenize(source).tokens.size();}
TKind lex_first_kind(mlc::String source) noexcept{return tokenize(source).tokens[0].kind;}
constexpr int lex_error_count(mlc::String source) noexcept{return tokenize(source).errors.size();}
mlc::Array<TestResult> lexer_tests() noexcept{
mlc::Array<TestResult> results = {};
results.push_back(assert_eq_int(mlc::String("empty source → 1 token (Eof)"), lex_token_count(mlc::String("")), 1));
results.push_back(assert_eq_int(mlc::String("whitespace only → 1 token (Eof)"), lex_token_count(mlc::String("   ")), 1));
results.push_back(assert_eq_int(mlc::String("comment only → 1 token (Eof)"), lex_token_count(mlc::String("// hi")), 1));
results.push_back(assert_eq_int(mlc::String("'fn' → 2 tokens"), lex_token_count(mlc::String("fn")), 2));
results.push_back(assert_eq_int(mlc::String("'type' → 2 tokens"), lex_token_count(mlc::String("type")), 2));
results.push_back(assert_eq_int(mlc::String("'let' → 2 tokens"), lex_token_count(mlc::String("let")), 2));
results.push_back(assert_eq_int(mlc::String("'if' → 2 tokens"), lex_token_count(mlc::String("if")), 2));
results.push_back(assert_eq_int(mlc::String("'match' → 2 tokens"), lex_token_count(mlc::String("match")), 2));
results.push_back(assert_eq_int(mlc::String("'end' → 2 tokens"), lex_token_count(mlc::String("end")), 2));
results.push_back(assert_eq_int(mlc::String("integer literal → 2 tokens"), lex_token_count(mlc::String("42")), 2));
results.push_back(assert_eq_int(mlc::String("string literal → 2 tokens"), lex_token_count(mlc::String("\"hi\"")), 2));
results.push_back(assert_eq_int(mlc::String("identifier → 2 tokens"), lex_token_count(mlc::String("foo")), 2));
results.push_back(assert_eq_int(mlc::String("no lex errors on valid source"), lex_error_count(mlc::String("fn f() -> i32 = 42")), 0));
results.push_back(assert_eq_int(mlc::String("fn decl → correct token count"), lex_token_count(mlc::String("fn f() -> i32 = 42")), 9));
return results;
}
Parser parser_new(mlc::Array<Token> tokens) noexcept{return Parser{tokens, 0};}
TKind parser_kind(Parser parser) noexcept{return parser.tokens[parser.pos].kind;}
Parser parser_advance(Parser parser) noexcept{return Parser{parser.tokens, parser.pos + 1};}
Parser parser_advance_by(Parser parser, int count) noexcept{return Parser{parser.tokens, parser.pos + count};}
bool parser_at_eof(Parser parser) noexcept{return [&]() -> bool { if (std::holds_alternative<Eof>(parser_kind(parser))) {  return true; } return false; }();}
Parser parser_skip_semi(Parser parser) noexcept{return [&]() -> Parser { if (std::holds_alternative<Semicolon>(parser_kind(parser))) {  return parser_advance(parser); } return parser; }();}
bool is_keyword_fn(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KFn>(kind)) {  return true; } return false; }();}
bool is_keyword_type(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KType>(kind)) {  return true; } return false; }();}
bool is_keyword_let(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KLet>(kind)) {  return true; } return false; }();}
bool is_keyword_mut(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KMut>(kind)) {  return true; } return false; }();}
bool is_keyword_const(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KConst>(kind)) {  return true; } return false; }();}
bool is_keyword_if(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KIf>(kind)) {  return true; } return false; }();}
bool is_keyword_unless(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KUnless>(kind)) {  return true; } return false; }();}
bool is_keyword_else(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KElse>(kind)) {  return true; } return false; }();}
bool is_keyword_end(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KEnd>(kind)) {  return true; } return false; }();}
bool is_keyword_do(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KDo>(kind)) {  return true; } return false; }();}
bool is_keyword_while(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KWhile>(kind)) {  return true; } return false; }();}
bool is_keyword_for(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KFor>(kind)) {  return true; } return false; }();}
bool is_keyword_match(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KMatch>(kind)) {  return true; } return false; }();}
bool is_keyword_return(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KReturn>(kind)) {  return true; } return false; }();}
bool is_keyword_break(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KBreak>(kind)) {  return true; } return false; }();}
bool is_keyword_continue(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KContinue>(kind)) {  return true; } return false; }();}
bool is_keyword_true(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KTrue>(kind)) {  return true; } return false; }();}
bool is_keyword_false(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KFalse>(kind)) {  return true; } return false; }();}
bool is_keyword_extern(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KExtern>(kind)) {  return true; } return false; }();}
bool is_keyword_extend(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<KExtend>(kind)) {  return true; } return false; }();}
bool is_ident(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Ident>(kind)) { auto _v_ident = std::get<Ident>(kind); auto [_w0] = _v_ident; return true; } return false; }();}
bool is_int_literal(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<LInt>(kind)) { auto _v_lint = std::get<LInt>(kind); auto [_w0] = _v_lint; return true; } return false; }();}
bool is_str_literal(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<LStr>(kind)) { auto _v_lstr = std::get<LStr>(kind); auto [_w0] = _v_lstr; return true; } return false; }();}
bool is_equal(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Equal>(kind)) {  return true; } return false; }();}
bool is_bar(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Bar>(kind)) {  return true; } return false; }();}
bool is_pipe(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Pipe>(kind)) {  return true; } return false; }();}
bool is_colon(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Colon>(kind)) {  return true; } return false; }();}
bool is_comma(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Comma>(kind)) {  return true; } return false; }();}
bool is_dot(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Dot>(kind)) {  return true; } return false; }();}
bool is_spread(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Spread>(kind)) {  return true; } return false; }();}
bool is_arrow(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Arrow>(kind)) {  return true; } return false; }();}
bool is_fat_arrow(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<FatArrow>(kind)) {  return true; } return false; }();}
bool is_question(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Question>(kind)) {  return true; } return false; }();}
bool is_left_paren(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<LParen>(kind)) {  return true; } return false; }();}
bool is_right_paren(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<RParen>(kind)) {  return true; } return false; }();}
bool is_left_brace(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<LBrace>(kind)) {  return true; } return false; }();}
bool is_right_brace(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<RBrace>(kind)) {  return true; } return false; }();}
bool is_left_bracket(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<LBracket>(kind)) {  return true; } return false; }();}
bool is_right_bracket(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<RBracket>(kind)) {  return true; } return false; }();}
bool is_operator(TKind kind) noexcept{return [&]() -> bool { if (std::holds_alternative<Op>(kind)) { auto _v_op = std::get<Op>(kind); auto [_w0] = _v_op; return true; } return false; }();}
mlc::String get_ident(TKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<Ident>(kind)) { auto _v_ident = std::get<Ident>(kind); auto [name] = _v_ident; return name; } return mlc::String(""); }();}
int get_int_literal(TKind kind) noexcept{return [&]() { if (std::holds_alternative<LInt>(kind)) { auto _v_lint = std::get<LInt>(kind); auto [n] = _v_lint; return n; } return 0; }();}
mlc::String get_str_literal(TKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<LStr>(kind)) { auto _v_lstr = std::get<LStr>(kind); auto [s] = _v_lstr; return s; } return mlc::String(""); }();}
mlc::String get_operator(TKind kind) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<Op>(kind)) { auto _v_op = std::get<Op>(kind); auto [s] = _v_op; return s; } return mlc::String(""); }();}
bool is_ctor_name(mlc::String name) noexcept{return name.char_at(0) >= mlc::String("A") && name.char_at(0) <= mlc::String("Z");}
bool TKind_is_fn(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KFn>(self)) {  return true; } return false; }();}
bool TKind_is_type(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KType>(self)) {  return true; } return false; }();}
bool TKind_is_let(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KLet>(self)) {  return true; } return false; }();}
bool TKind_is_mut(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KMut>(self)) {  return true; } return false; }();}
bool TKind_is_const(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KConst>(self)) {  return true; } return false; }();}
bool TKind_is_if(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KIf>(self)) {  return true; } return false; }();}
bool TKind_is_unless(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KUnless>(self)) {  return true; } return false; }();}
bool TKind_is_else(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KElse>(self)) {  return true; } return false; }();}
bool TKind_is_end(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KEnd>(self)) {  return true; } return false; }();}
bool TKind_is_do(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KDo>(self)) {  return true; } return false; }();}
bool TKind_is_while(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KWhile>(self)) {  return true; } return false; }();}
bool TKind_is_for(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KFor>(self)) {  return true; } return false; }();}
bool TKind_is_match(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KMatch>(self)) {  return true; } return false; }();}
bool TKind_is_return(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KReturn>(self)) {  return true; } return false; }();}
bool TKind_is_break(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KBreak>(self)) {  return true; } return false; }();}
bool TKind_is_continue(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KContinue>(self)) {  return true; } return false; }();}
bool TKind_is_true(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KTrue>(self)) {  return true; } return false; }();}
bool TKind_is_false(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KFalse>(self)) {  return true; } return false; }();}
bool TKind_is_extern(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KExtern>(self)) {  return true; } return false; }();}
bool TKind_is_extend(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KExtend>(self)) {  return true; } return false; }();}
bool TKind_is_import(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KImport>(self)) {  return true; } return false; }();}
bool TKind_is_from(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<KFrom>(self)) {  return true; } return false; }();}
bool TKind_is_eof(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<Eof>(self)) {  return true; } return false; }();}
bool TKind_is_ident(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<Ident>(self)) { auto _v_ident = std::get<Ident>(self); auto [_w0] = _v_ident; return true; } return false; }();}
bool TKind_is_int(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<LInt>(self)) { auto _v_lint = std::get<LInt>(self); auto [_w0] = _v_lint; return true; } return false; }();}
bool TKind_is_str(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<LStr>(self)) { auto _v_lstr = std::get<LStr>(self); auto [_w0] = _v_lstr; return true; } return false; }();}
bool TKind_is_equal(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<Equal>(self)) {  return true; } return false; }();}
bool TKind_is_bar(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<Bar>(self)) {  return true; } return false; }();}
bool TKind_is_pipe(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<Pipe>(self)) {  return true; } return false; }();}
bool TKind_is_colon(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<Colon>(self)) {  return true; } return false; }();}
bool TKind_is_comma(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<Comma>(self)) {  return true; } return false; }();}
bool TKind_is_dot(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<Dot>(self)) {  return true; } return false; }();}
bool TKind_is_spread(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<Spread>(self)) {  return true; } return false; }();}
bool TKind_is_arrow(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<Arrow>(self)) {  return true; } return false; }();}
bool TKind_is_fat_arrow(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<FatArrow>(self)) {  return true; } return false; }();}
bool TKind_is_question(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<Question>(self)) {  return true; } return false; }();}
bool TKind_is_lparen(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<LParen>(self)) {  return true; } return false; }();}
bool TKind_is_rparen(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<RParen>(self)) {  return true; } return false; }();}
bool TKind_is_lbrace(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<LBrace>(self)) {  return true; } return false; }();}
bool TKind_is_rbrace(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<RBrace>(self)) {  return true; } return false; }();}
bool TKind_is_lbracket(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<LBracket>(self)) {  return true; } return false; }();}
bool TKind_is_rbracket(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<RBracket>(self)) {  return true; } return false; }();}
bool TKind_is_op(TKind self) noexcept{return [&]() -> bool { if (std::holds_alternative<Op>(self)) { auto _v_op = std::get<Op>(self); auto [_w0] = _v_op; return true; } return false; }();}
mlc::String TKind_ident(TKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<Ident>(self)) { auto _v_ident = std::get<Ident>(self); auto [name] = _v_ident; return name; } return mlc::String(""); }();}
int TKind_int_val(TKind self) noexcept{return [&]() { if (std::holds_alternative<LInt>(self)) { auto _v_lint = std::get<LInt>(self); auto [n] = _v_lint; return n; } return 0; }();}
mlc::String TKind_str_val(TKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<LStr>(self)) { auto _v_lstr = std::get<LStr>(self); auto [s] = _v_lstr; return s; } return mlc::String(""); }();}
mlc::String TKind_op_val(TKind self) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<Op>(self)) { auto _v_op = std::get<Op>(self); auto [s] = _v_op; return s; } return mlc::String(""); }();}
TKind Parser_kind(Parser self) noexcept{return self.tokens[self.pos].kind;}
Parser Parser_advance(Parser self) noexcept{return Parser{self.tokens, self.pos + 1};}
Parser Parser_advance_by(Parser self, int count) noexcept{return Parser{self.tokens, self.pos + count};}
bool Parser_at_eof(Parser self) noexcept{return TKind_is_eof(self.tokens[self.pos].kind);}
Parser Parser_skip_semi(Parser self) noexcept{return [&]() -> Parser { if (std::holds_alternative<Semicolon>(self.tokens[self.pos].kind)) {  return Parser_advance(self); } return self; }();}
TypeResult parse_type(Parser parser) noexcept{
TypeResult base = parse_base_type(parser);
return TKind_is_arrow(Parser_kind(base.parser)) ? [&]() -> TypeResult { 
  TypeResult ret = parse_type(Parser_advance(base.parser));
  mlc::Array<std::shared_ptr<TypeExpr>> params = mlc::Array<std::shared_ptr<TypeExpr>>{base.type_expr};
  return TypeResult{std::make_shared<TypeExpr>(TyFn(params, ret.type_expr)), ret.parser};
 }() : base;
}
TypeResult parse_base_type(Parser parser) noexcept{
TKind kind = Parser_kind(parser);
return TKind_is_lbracket(kind) ? [&]() -> TypeResult { 
  TypeResult inner = parse_type(Parser_advance(parser));
  return TypeResult{std::make_shared<TypeExpr>(TyArray(inner.type_expr)), Parser_advance(inner.parser)};
 }() : TKind_is_ident(kind) ? [&]() -> TypeResult { 
  mlc::String name = TKind_ident(kind);
  return name == mlc::String("i32") ? TypeResult{std::make_shared<TypeExpr>(TyI32{}), Parser_advance(parser)} : name == mlc::String("string") ? TypeResult{std::make_shared<TypeExpr>(TyString{}), Parser_advance(parser)} : name == mlc::String("bool") ? TypeResult{std::make_shared<TypeExpr>(TyBool{}), Parser_advance(parser)} : name == mlc::String("unit") ? TypeResult{std::make_shared<TypeExpr>(TyUnit{}), Parser_advance(parser)} : [&]() -> TypeResult { 
  Parser after = Parser_advance(parser);
  return TKind_is_op(Parser_kind(after)) && TKind_op_val(Parser_kind(after)) == mlc::String("<") ? [&]() -> TypeResult { 
  TypesResult args = parse_type_args(Parser_advance(after));
  std::shared_ptr<TypeExpr> type_expr = name == mlc::String("Shared") && args.types.size() == 1 ? std::make_shared<TypeExpr>(TyShared(args.types[0])) : std::make_shared<TypeExpr>(TyGeneric(name, args.types));
  return TypeResult{type_expr, args.parser};
 }() : TypeResult{std::make_shared<TypeExpr>(TyNamed(name)), after};
 }();
 }() : TypeResult{std::make_shared<TypeExpr>(TyUnit{}), Parser_advance_by(parser, 2)};
}
TypesResult parse_type_args(Parser parser) noexcept{
mlc::Array<std::shared_ptr<TypeExpr>> types = {};
TypeResult first = parse_type(parser);
types.push_back(first.type_expr);
Parser state = first.parser;
while (TKind_is_comma(Parser_kind(state))){
{
TypeResult next = parse_type(Parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
}
Parser end_state = TKind_is_op(Parser_kind(state)) && TKind_op_val(Parser_kind(state)) == mlc::String(">") ? Parser_advance(state) : state;
return TypesResult{types, end_state};
}
StmtsResult parse_stmts_until_end(Parser parser) noexcept{
mlc::Array<std::shared_ptr<Stmt>> stmts = {};
Parser state = std::move(parser);
while (!TKind_is_end(Parser_kind(state)) && !Parser_at_eof(state)){
{
StmtResult result = parse_stmt(state);
stmts.push_back(result.stmt);
state = Parser_skip_semi(result.parser);
}
}
return StmtsResult{stmts, Parser_advance(state)};
}
StmtsResult parse_stmts_until_else_end(Parser parser) noexcept{
mlc::Array<std::shared_ptr<Stmt>> stmts = {};
Parser state = std::move(parser);
while (!TKind_is_end(Parser_kind(state)) && !TKind_is_else(Parser_kind(state)) && !Parser_at_eof(state)){
{
StmtResult result = parse_stmt(state);
stmts.push_back(result.stmt);
state = Parser_skip_semi(result.parser);
}
}
return StmtsResult{stmts, state};
}
StmtResult parse_stmt(Parser parser) noexcept{
TKind kind = Parser_kind(parser);
return TKind_is_let(kind) ? [&]() -> StmtResult { 
  Parser after_let = Parser_advance(parser);
  bool is_const = TKind_is_const(Parser_kind(after_let));
  Parser name_pos = is_const ? Parser_advance(after_let) : after_let;
  mlc::String var_name = TKind_ident(Parser_kind(name_pos));
  TypeResult type_result = TKind_is_colon(Parser_kind(Parser_advance(name_pos))) ? parse_type(Parser_advance_by(name_pos, 2)) : TypeResult{std::make_shared<TypeExpr>(TyUnit{}), Parser_advance(name_pos)};
  ExprResult value_result = parse_expr(Parser_advance(type_result.parser));
  return StmtResult{std::make_shared<Stmt>(StmtLet(var_name, !is_const, type_result.type_expr, value_result.expr)), value_result.parser};
 }() : TKind_is_const(kind) ? [&]() -> StmtResult { 
  Parser name_pos = Parser_advance(parser);
  mlc::String var_name = TKind_ident(Parser_kind(name_pos));
  TypeResult type_result = TKind_is_colon(Parser_kind(Parser_advance(name_pos))) ? parse_type(Parser_advance_by(name_pos, 2)) : TypeResult{std::make_shared<TypeExpr>(TyUnit{}), Parser_advance(name_pos)};
  ExprResult value_result = parse_expr(Parser_advance(type_result.parser));
  return StmtResult{std::make_shared<Stmt>(StmtLet(var_name, false, type_result.type_expr, value_result.expr)), value_result.parser};
 }() : TKind_is_break(kind) ? StmtResult{std::make_shared<Stmt>(StmtBreak{}), Parser_advance(parser)} : TKind_is_continue(kind) ? StmtResult{std::make_shared<Stmt>(StmtContinue{}), Parser_advance(parser)} : TKind_is_return(kind) ? [&]() -> StmtResult { 
  ExprResult value_result = parse_expr(Parser_advance(parser));
  return StmtResult{std::make_shared<Stmt>(StmtReturn(value_result.expr)), value_result.parser};
 }() : [&]() -> StmtResult { 
  ExprResult expr_result = parse_expr(parser);
  return TKind_is_equal(Parser_kind(expr_result.parser)) ? [&]() -> StmtResult { 
  ExprResult rhs = parse_expr(Parser_advance(expr_result.parser));
  return StmtResult{std::make_shared<Stmt>(StmtExpr(std::make_shared<Expr>(ExprBin(mlc::String("="), expr_result.expr, rhs.expr)))), rhs.parser};
 }() : StmtResult{std::make_shared<Stmt>(StmtExpr(expr_result.expr)), expr_result.parser};
 }();
}
PatResult parse_pat(Parser parser) noexcept{
TKind kind = Parser_kind(parser);
return TKind_is_ident(kind) ? [&]() -> PatResult { 
  mlc::String name = TKind_ident(kind);
  return name == mlc::String("_") ? PatResult{std::make_shared<Pat>(PatWild{}), Parser_advance(parser)} : is_ctor_name(name) ? [&]() -> PatResult { 
  Parser after = Parser_advance(parser);
  return TKind_is_lparen(Parser_kind(after)) ? [&]() -> PatResult { 
  PatsResult sub_pats = parse_pat_args(Parser_advance(after));
  return PatResult{std::make_shared<Pat>(PatCtor(name, sub_pats.pats)), sub_pats.parser};
 }() : PatResult{std::make_shared<Pat>(PatCtor(name, {})), after};
 }() : PatResult{std::make_shared<Pat>(PatIdent(name)), Parser_advance(parser)};
 }() : TKind_is_int(kind) ? PatResult{std::make_shared<Pat>(PatInt(TKind_int_val(kind))), Parser_advance(parser)} : TKind_is_str(kind) ? PatResult{std::make_shared<Pat>(PatStr(TKind_str_val(kind))), Parser_advance(parser)} : TKind_is_true(kind) ? PatResult{std::make_shared<Pat>(PatBool(true)), Parser_advance(parser)} : TKind_is_false(kind) ? PatResult{std::make_shared<Pat>(PatBool(false)), Parser_advance(parser)} : PatResult{std::make_shared<Pat>(PatWild{}), Parser_advance(parser)};
}
PatsResult parse_pat_args(Parser parser) noexcept{
mlc::Array<std::shared_ptr<Pat>> pats = {};
return TKind_is_rparen(Parser_kind(parser)) ? PatsResult{pats, Parser_advance(parser)} : [&]() -> PatsResult { 
  PatResult first = parse_pat(parser);
  pats.push_back(first.pat);
  Parser state = first.parser;
  while (TKind_is_comma(Parser_kind(state))){
{
PatResult next = parse_pat(Parser_advance(state));
pats.push_back(next.pat);
state = next.parser;
}
}
  return PatsResult{pats, Parser_advance(state)};
 }();
}
ExprResult parse_expr(Parser parser) noexcept{return parse_pipe(parser);}
std::shared_ptr<Expr> pipe_desugar(std::shared_ptr<Expr> left_expr, std::shared_ptr<Expr> right_expr) noexcept{return [&]() -> std::shared_ptr<Expr> { if (std::holds_alternative<ExprCall>((*right_expr))) { auto _v_exprcall = std::get<ExprCall>((*right_expr)); auto [callee, existing_args] = _v_exprcall; return [&]() -> std::shared_ptr<Expr> { 
  mlc::Array<std::shared_ptr<Expr>> new_args = mlc::Array<std::shared_ptr<Expr>>{left_expr};
  int i = 0;
  while (i < existing_args.size()){
{
new_args.push_back(existing_args[i]);
i = i + 1;
}
}
  return std::make_shared<Expr>(ExprCall(callee, new_args));
 }(); } return [&]() -> std::shared_ptr<Expr> { 
  mlc::Array<std::shared_ptr<Expr>> call_args = mlc::Array<std::shared_ptr<Expr>>{left_expr};
  return std::make_shared<Expr>(ExprCall(right_expr, call_args));
 }(); }();}
ExprResult parse_pipe(Parser parser) noexcept{
ExprResult left = parse_or(parser);
std::shared_ptr<Expr> expr = left.expr;
Parser state = left.parser;
while (TKind_is_pipe(Parser_kind(state))){
{
ExprResult right = parse_or(Parser_advance(state));
expr = pipe_desugar(expr, right.expr);
state = right.parser;
}
}
return ExprResult{expr, state};
}
ExprResult parse_or(Parser parser) noexcept{
ExprResult left = parse_and(parser);
std::shared_ptr<Expr> expr = left.expr;
Parser state = left.parser;
while (TKind_is_op(Parser_kind(state)) && TKind_op_val(Parser_kind(state)) == mlc::String("||")){
{
ExprResult right = parse_and(Parser_advance(state));
expr = std::make_shared<Expr>(ExprBin(mlc::String("||"), expr, right.expr));
state = right.parser;
}
}
return ExprResult{expr, state};
}
ExprResult parse_and(Parser parser) noexcept{
ExprResult left = parse_cmp(parser);
std::shared_ptr<Expr> expr = left.expr;
Parser state = left.parser;
while (TKind_is_op(Parser_kind(state)) && TKind_op_val(Parser_kind(state)) == mlc::String("&&")){
{
ExprResult right = parse_cmp(Parser_advance(state));
expr = std::make_shared<Expr>(ExprBin(mlc::String("&&"), expr, right.expr));
state = right.parser;
}
}
return ExprResult{expr, state};
}
constexpr bool is_cmp_op(mlc::String op) noexcept{return op == mlc::String("==") || op == mlc::String("!=") || op == mlc::String("<") || op == mlc::String(">") || op == mlc::String("<=") || op == mlc::String(">=");}
ExprResult parse_cmp(Parser parser) noexcept{
ExprResult left = parse_add(parser);
std::shared_ptr<Expr> expr = left.expr;
Parser state = left.parser;
TKind kind = Parser_kind(state);
if (TKind_is_op(kind) && is_cmp_op(TKind_op_val(kind))){
{
ExprResult right = parse_add(Parser_advance(state));
expr = std::make_shared<Expr>(ExprBin(TKind_op_val(kind), expr, right.expr));
state = right.parser;
}
}
return ExprResult{expr, state};
}
ExprResult parse_add(Parser parser) noexcept{
ExprResult left = parse_mul(parser);
std::shared_ptr<Expr> expr = left.expr;
Parser state = left.parser;
bool go = true;
while (go){
{
TKind kind = Parser_kind(state);
if (TKind_is_op(kind) && TKind_op_val(kind) == mlc::String("+") || TKind_op_val(kind) == mlc::String("-")){
{
ExprResult right = parse_mul(Parser_advance(state));
expr = std::make_shared<Expr>(ExprBin(TKind_op_val(kind), expr, right.expr));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return ExprResult{expr, state};
}
ExprResult parse_mul(Parser parser) noexcept{
ExprResult left = parse_unary(parser);
std::shared_ptr<Expr> expr = left.expr;
Parser state = left.parser;
bool go = true;
while (go){
{
TKind kind = Parser_kind(state);
if (TKind_is_op(kind) && TKind_op_val(kind) == mlc::String("*") || TKind_op_val(kind) == mlc::String("/") || TKind_op_val(kind) == mlc::String("%")){
{
ExprResult right = parse_unary(Parser_advance(state));
expr = std::make_shared<Expr>(ExprBin(TKind_op_val(kind), expr, right.expr));
state = right.parser;
}
} else {
{
go = false;
}
}
}
}
return ExprResult{expr, state};
}
ExprResult parse_unary(Parser parser) noexcept{
TKind kind = Parser_kind(parser);
return TKind_is_op(kind) && TKind_op_val(kind) == mlc::String("!") || TKind_op_val(kind) == mlc::String("-") ? [&]() -> ExprResult { 
  ExprResult inner = parse_unary(Parser_advance(parser));
  return ExprResult{std::make_shared<Expr>(ExprUn(TKind_op_val(kind), inner.expr)), inner.parser};
 }() : parse_postfix(parser);
}
ExprResult parse_postfix(Parser parser) noexcept{
ExprResult base = parse_primary(parser);
std::shared_ptr<Expr> expr = base.expr;
Parser state = base.parser;
bool go = true;
while (go){
{
TKind kind = Parser_kind(state);
if (TKind_is_dot(kind)){
{
mlc::String field_name = TKind_ident(Parser_kind(Parser_advance(state)));
if (TKind_is_lparen(Parser_kind(Parser_advance_by(state, 2)))){
ExprsResult args = parse_arg_list(Parser_advance_by(state, 3));
expr = std::make_shared<Expr>(ExprMethod(expr, field_name, args.exprs));
state = args.parser;
} else {
expr = std::make_shared<Expr>(ExprField(expr, field_name));
state = Parser_advance_by(state, 2);
}
}
} else {
{
if (TKind_is_lparen(kind)){
ExprsResult args = parse_arg_list(Parser_advance(state));
expr = std::make_shared<Expr>(ExprCall(expr, args.exprs));
state = args.parser;
} else {
if (TKind_is_lbracket(kind)){
ExprResult index = parse_expr(Parser_advance(state));
expr = std::make_shared<Expr>(ExprIndex(expr, index.expr));
state = Parser_advance(index.parser);
} else {
if (TKind_is_question(kind)){
expr = std::make_shared<Expr>(ExprQuestion(expr));
state = Parser_advance(state);
} else {
go = false;
}
}
}
}
}
}
}
return ExprResult{expr, state};
}
ExprsResult parse_arg_list(Parser parser) noexcept{
mlc::Array<std::shared_ptr<Expr>> exprs = {};
return TKind_is_rparen(Parser_kind(parser)) ? ExprsResult{exprs, Parser_advance(parser)} : [&]() -> ExprsResult { 
  ExprResult first = parse_expr(parser);
  exprs.push_back(first.expr);
  Parser state = first.parser;
  while (TKind_is_comma(Parser_kind(state))){
{
ExprResult next = parse_expr(Parser_advance(state));
exprs.push_back(next.expr);
state = next.parser;
}
}
  return ExprsResult{exprs, Parser_advance(state)};
 }();
}
ExprResult parse_primary(Parser parser) noexcept{
TKind kind = Parser_kind(parser);
return TKind_is_int(kind) ? ExprResult{std::make_shared<Expr>(ExprInt(TKind_int_val(kind))), Parser_advance(parser)} : TKind_is_str(kind) ? ExprResult{std::make_shared<Expr>(ExprStr(TKind_str_val(kind))), Parser_advance(parser)} : TKind_is_true(kind) ? ExprResult{std::make_shared<Expr>(ExprBool(true)), Parser_advance(parser)} : TKind_is_false(kind) ? ExprResult{std::make_shared<Expr>(ExprBool(false)), Parser_advance(parser)} : TKind_is_lparen(kind) ? TKind_is_rparen(Parser_kind(Parser_advance(parser))) ? TKind_is_fat_arrow(Parser_kind(Parser_advance_by(parser, 2))) ? [&]() -> ExprResult { 
  mlc::Array<mlc::String> params = {};
  ExprResult body = parse_expr(Parser_advance_by(parser, 3));
  return ExprResult{std::make_shared<Expr>(ExprLambda(params, body.expr)), body.parser};
 }() : ExprResult{std::make_shared<Expr>(ExprUnit{}), Parser_advance_by(parser, 2)} : looks_like_lambda_params(Parser_advance(parser)) ? [&]() -> ExprResult { 
  NamesResult param_result = parse_lambda_params(Parser_advance(parser));
  ExprResult body = parse_expr(Parser_advance(param_result.parser));
  return ExprResult{std::make_shared<Expr>(ExprLambda(param_result.exprs, body.expr)), body.parser};
 }() : [&]() -> ExprResult { 
  ExprResult inner = parse_expr(Parser_advance(parser));
  return ExprResult{inner.expr, Parser_advance(inner.parser)};
 }() : TKind_is_lbracket(kind) ? parse_array_lit(Parser_advance(parser)) : TKind_is_if(kind) || TKind_is_unless(kind) ? parse_if_expr(parser) : TKind_is_do(kind) ? parse_block(Parser_advance(parser)) : TKind_is_while(kind) ? parse_while_expr(Parser_advance(parser)) : TKind_is_for(kind) ? parse_for_expr(Parser_advance(parser)) : TKind_is_match(kind) ? parse_match_expr(Parser_advance(parser)) : TKind_is_return(kind) ? [&]() -> ExprResult { 
  ExprResult value = parse_expr(Parser_advance(parser));
  mlc::Array<std::shared_ptr<Stmt>> stmts = {};
  stmts.push_back(std::make_shared<Stmt>(StmtReturn(value.expr)));
  return ExprResult{std::make_shared<Expr>(ExprBlock(stmts, std::make_shared<Expr>(ExprUnit{}))), value.parser};
 }() : TKind_is_ident(kind) ? [&]() -> ExprResult { 
  mlc::String name = TKind_ident(kind);
  return TKind_is_fat_arrow(Parser_kind(Parser_advance(parser))) ? [&]() -> ExprResult { 
  mlc::Array<mlc::String> params = mlc::Array<mlc::String>{name};
  ExprResult body = parse_expr(Parser_advance_by(parser, 2));
  return ExprResult{std::make_shared<Expr>(ExprLambda(params, body.expr)), body.parser};
 }() : is_ctor_name(name) && TKind_is_lbrace(Parser_kind(Parser_advance(parser))) ? parse_record_expr(Parser_advance_by(parser, 2), name) : ExprResult{std::make_shared<Expr>(ExprIdent(name)), Parser_advance(parser)};
 }() : ExprResult{std::make_shared<Expr>(ExprUnit{}), Parser_advance(parser)};
}
bool looks_like_lambda_params(Parser parser) noexcept{
Parser state = std::move(parser);
bool ok = true;
if (!TKind_is_ident(Parser_kind(state))){
{
ok = false;
}
} else {
{
state = Parser_advance(state);
while (TKind_is_comma(Parser_kind(state))){
{
state = Parser_advance(state);
if (!TKind_is_ident(Parser_kind(state))){
{
ok = false;
state = Parser_advance(state);
}
} else {
{
state = Parser_advance(state);
}
}
}
}
if (!TKind_is_rparen(Parser_kind(state))){
ok = false;
} else {
ok = TKind_is_fat_arrow(Parser_kind(Parser_advance(state)));
}
}
}
return ok;
}
NamesResult parse_lambda_params(Parser parser) noexcept{
mlc::Array<mlc::String> names = {};
Parser state = std::move(parser);
names.push_back(TKind_ident(Parser_kind(state)));
state = Parser_advance(state);
while (TKind_is_comma(Parser_kind(state))){
{
state = Parser_advance(state);
names.push_back(TKind_ident(Parser_kind(state)));
state = Parser_advance(state);
}
}
return NamesResult{names, Parser_advance(state)};
}
ExprResult parse_array_lit(Parser parser) noexcept{
mlc::Array<std::shared_ptr<Expr>> exprs = {};
return TKind_is_rbracket(Parser_kind(parser)) ? ExprResult{std::make_shared<Expr>(ExprArray(exprs)), Parser_advance(parser)} : [&]() -> ExprResult { 
  ExprResult first = parse_expr(parser);
  exprs.push_back(first.expr);
  Parser state = first.parser;
  while (TKind_is_comma(Parser_kind(state))){
{
ExprResult next = parse_expr(Parser_advance(state));
exprs.push_back(next.expr);
state = next.parser;
}
}
  return ExprResult{std::make_shared<Expr>(ExprArray(exprs)), Parser_advance(state)};
 }();
}
ExprResult parse_if_expr(Parser parser) noexcept{
bool is_negated = TKind_is_unless(Parser_kind(parser));
ExprResult cond_result = parse_expr(Parser_advance(parser));
std::shared_ptr<Expr> condition = is_negated ? std::make_shared<Expr>(ExprUn(mlc::String("!"), cond_result.expr)) : cond_result.expr;
StmtsResult then_result = parse_stmts_until_else_end(Parser_advance(cond_result.parser));
std::shared_ptr<Expr> then_expr = StmtsResult_to_block_expr(then_result);
return TKind_is_else(Parser_kind(then_result.parser)) ? [&]() -> ExprResult { 
  Parser after_else = Parser_advance(then_result.parser);
  return TKind_is_if(Parser_kind(after_else)) || TKind_is_unless(Parser_kind(after_else)) ? [&]() -> ExprResult { 
  ExprResult else_result = parse_if_expr(after_else);
  return ExprResult{std::make_shared<Expr>(ExprIf(condition, then_expr, else_result.expr)), else_result.parser};
 }() : [&]() -> ExprResult { 
  StmtsResult else_stmts = parse_stmts_until_end(after_else);
  return ExprResult{std::make_shared<Expr>(ExprIf(condition, then_expr, StmtsResult_to_block_expr(else_stmts))), else_stmts.parser};
 }();
 }() : ExprResult{std::make_shared<Expr>(ExprIf(condition, then_expr, std::make_shared<Expr>(ExprUnit{}))), Parser_advance(then_result.parser)};
}
ExprResult parse_block(Parser parser) noexcept{
StmtsResult result = parse_stmts_until_end(parser);
return ExprResult{StmtsResult_to_block_expr(result), result.parser};
}
bool is_unit_expr(std::shared_ptr<Expr> expr) noexcept{return [&]() -> bool { if (std::holds_alternative<ExprUnit>((*expr))) {  return true; } if (std::holds_alternative<ExprBlock>((*expr))) { auto _v_exprblock = std::get<ExprBlock>((*expr)); auto [_w0, result] = _v_exprblock; return is_unit_expr(result); } if (std::holds_alternative<ExprIf>((*expr))) { auto _v_exprif = std::get<ExprIf>((*expr)); auto [_w0, _w1, else_expr] = _v_exprif; return is_unit_expr(else_expr); } return false; }();}
std::shared_ptr<Expr> block_result(mlc::Array<std::shared_ptr<Stmt>> stmts) noexcept{return stmts.size() == 0 ? std::make_shared<Expr>(ExprUnit{}) : [&]() { if (std::holds_alternative<StmtExpr>((*stmts[stmts.size() - 1]))) { auto _v_stmtexpr = std::get<StmtExpr>((*stmts[stmts.size() - 1])); auto [expr] = _v_stmtexpr; return is_unit_expr(expr) ? std::make_shared<Expr>(ExprUnit{}) : expr; } return std::make_shared<Expr>(ExprUnit{}); }();}
mlc::Array<std::shared_ptr<Stmt>> block_body(mlc::Array<std::shared_ptr<Stmt>> stmts) noexcept{
return stmts.size() == 0 ? stmts : [&]() -> mlc::Array<std::shared_ptr<Stmt>> { if (std::holds_alternative<StmtExpr>((*stmts[stmts.size() - 1]))) { auto _v_stmtexpr = std::get<StmtExpr>((*stmts[stmts.size() - 1])); auto [expr] = _v_stmtexpr; return is_unit_expr(expr) ? stmts : [&]() -> mlc::Array<std::shared_ptr<Stmt>> { 
  mlc::Array<std::shared_ptr<Stmt>> result = {};
  int i = 0;
  while (i < stmts.size() - 1){
{
result.push_back(stmts[i]);
i = i + 1;
}
}
  return result;
 }(); } return stmts; }();
}
std::shared_ptr<Expr> StmtsResult_to_block_expr(StmtsResult self) noexcept{return std::make_shared<Expr>(ExprBlock(block_body(self.stmts), block_result(self.stmts)));}
ExprResult parse_while_expr(Parser parser) noexcept{
ExprResult cond = parse_expr(parser);
StmtsResult body = parse_stmts_until_end(Parser_advance(cond.parser));
return ExprResult{std::make_shared<Expr>(ExprWhile(cond.expr, body.stmts)), body.parser};
}
ExprResult parse_for_expr(Parser parser) noexcept{
mlc::String var_name = TKind_ident(Parser_kind(parser));
ExprResult iter = parse_expr(Parser_advance_by(parser, 2));
StmtsResult body = parse_stmts_until_end(Parser_advance(iter.parser));
return ExprResult{std::make_shared<Expr>(ExprFor(var_name, iter.expr, body.stmts)), body.parser};
}
ExprResult parse_match_expr(Parser parser) noexcept{
ExprResult subject = parse_expr(parser);
ArmsResult arms = parse_arms(Parser_advance(subject.parser));
return ExprResult{std::make_shared<Expr>(ExprMatch(subject.expr, arms.arms)), arms.parser};
}
ArmsResult parse_arms(Parser parser) noexcept{
mlc::Array<std::shared_ptr<MatchArm>> arms = {};
Parser state = std::move(parser);
while (!TKind_is_rbrace(Parser_kind(state)) && !Parser_at_eof(state)){
{
PatResult pat_result = parse_pat(state);
ExprResult body_result = parse_expr(Parser_advance(pat_result.parser));
arms.push_back(std::make_shared<MatchArm>(MatchArm{pat_result.pat, body_result.expr}));
state = body_result.parser;
if (TKind_is_comma(Parser_kind(state))){
{
state = Parser_advance(state);
}
}
}
}
return ArmsResult{arms, Parser_advance(state)};
}
FieldValsResult parse_record_fields(Parser parser) noexcept{
mlc::Array<std::shared_ptr<FieldVal>> field_vals = {};
Parser state = std::move(parser);
while (!TKind_is_rbrace(Parser_kind(state)) && !Parser_at_eof(state)){
{
mlc::String field_name = TKind_ident(Parser_kind(state));
Parser after_name = Parser_advance(state);
if (TKind_is_colon(Parser_kind(after_name))){
{
ExprResult value_result = parse_expr(Parser_advance(after_name));
field_vals.push_back(std::make_shared<FieldVal>(FieldVal{field_name, value_result.expr}));
state = value_result.parser;
}
} else {
{
field_vals.push_back(std::make_shared<FieldVal>(FieldVal{field_name, std::make_shared<Expr>(ExprIdent(field_name))}));
state = after_name;
}
}
if (TKind_is_comma(Parser_kind(state))){
{
state = Parser_advance(state);
}
}
}
}
return FieldValsResult{field_vals, Parser_advance(state)};
}
ExprResult parse_record_expr(Parser parser, mlc::String record_name) noexcept{return TKind_is_spread(Parser_kind(parser)) ? [&]() -> ExprResult { 
  ExprResult base_result = parse_expr(Parser_advance(parser));
  Parser after_base = base_result.parser;
  if (TKind_is_comma(Parser_kind(after_base))){
{
after_base = Parser_advance(after_base);
}
}
  FieldValsResult fields_result = parse_record_fields(after_base);
  return ExprResult{std::make_shared<Expr>(ExprRecordUpdate(record_name, base_result.expr, fields_result.field_vals)), fields_result.parser};
 }() : [&]() -> ExprResult { 
  FieldValsResult fields_result = parse_record_fields(parser);
  return ExprResult{std::make_shared<Expr>(ExprRecord(record_name, fields_result.field_vals)), fields_result.parser};
 }();}
DeclResult parse_import_decl(Parser parser) noexcept{
Parser state = std::move(parser);
mlc::Array<mlc::String> symbols = {};
if (TKind_is_op(Parser_kind(state)) && TKind_op_val(Parser_kind(state)) == mlc::String("*")){
{
symbols.push_back(mlc::String("*"));
state = Parser_advance(state);
}
} else {
{
if (TKind_is_lbrace(Parser_kind(state))){
state = Parser_advance(state);
while (TKind_is_ident(Parser_kind(state)) || TKind_is_op(Parser_kind(state)) && TKind_op_val(Parser_kind(state)) == mlc::String("*")){
{
if (TKind_is_op(Parser_kind(state)) && TKind_op_val(Parser_kind(state)) == mlc::String("*")){
{
symbols.push_back(mlc::String("*"));
state = Parser_advance(state);
}
} else {
{
symbols.push_back(TKind_ident(Parser_kind(state)));
state = Parser_advance(state);
}
}
if (TKind_is_comma(Parser_kind(state))){
{
state = Parser_advance(state);
}
}
}
}
state = TKind_is_rbrace(Parser_kind(state)) ? Parser_advance(state) : state;
}
}
}
state = TKind_is_from(Parser_kind(state)) ? Parser_advance(state) : state;
mlc::String path = TKind_is_str(Parser_kind(state)) ? TKind_str_val(Parser_kind(state)) : mlc::String("");
return DeclResult{std::make_shared<Decl>(DeclImport(path, symbols)), Parser_advance(state)};
}
DeclResult parse_extend_decl(Parser parser) noexcept{
mlc::String type_name = TKind_ident(Parser_kind(parser));
Parser state = std::move(parser);
while (TKind_is_colon(Parser_kind(state)) || TKind_is_ident(Parser_kind(state)) && !TKind_is_lbrace(Parser_kind(state))){
{
state = Parser_advance(state);
}
}
Parser methods_state = TKind_is_lbrace(Parser_kind(state)) ? Parser_advance(state) : state;
mlc::Array<std::shared_ptr<Decl>> methods = {};
while (!TKind_is_rbrace(Parser_kind(methods_state)) && !Parser_at_eof(methods_state)){
{
if (TKind_is_fn(Parser_kind(methods_state))){
{
DeclResult method_result = parse_extend_method(methods_state, type_name);
methods.push_back(method_result.decl);
methods_state = method_result.parser;
}
} else {
{
methods_state = Parser_advance(methods_state);
}
}
}
}
return DeclResult{std::make_shared<Decl>(DeclExtend(type_name, methods)), Parser_advance(methods_state)};
}
DeclResult parse_extend_method(Parser parser, mlc::String type_name) noexcept{
mlc::String fn_name = TKind_ident(Parser_kind(Parser_advance(parser)));
mlc::String mangled_name = type_name + mlc::String("_") + fn_name;
Parser after_lparen = Parser_advance_by(parser, 3);
Parser after_self = TKind_is_ident(Parser_kind(after_lparen)) && TKind_ident(Parser_kind(after_lparen)) == mlc::String("self") ? Parser_advance(after_lparen) : after_lparen;
std::shared_ptr<Param> self_param = std::make_shared<Param>(Param{mlc::String("self"), false, std::make_shared<TypeExpr>(TyNamed(type_name))});
mlc::Array<std::shared_ptr<Param>> params = {};
params.push_back(self_param);
Parser params_state = TKind_is_comma(Parser_kind(after_self)) ? Parser_advance(after_self) : after_self;
ParamsResult rest_params = TKind_is_rparen(Parser_kind(params_state)) ? ParamsResult{{}, Parser_advance(params_state)} : parse_params(params_state);
int index = 0;
while (index < rest_params.params.size()){
{
params.push_back(rest_params.params[index]);
index = index + 1;
}
}
TypeResult ret_type_result = parse_type(Parser_advance(rest_params.parser));
ExprResult body_result = parse_expr(Parser_advance(ret_type_result.parser));
return DeclResult{std::make_shared<Decl>(DeclFn(mangled_name, params, ret_type_result.type_expr, body_result.expr)), body_result.parser};
}
DeclResult parse_decl(Parser parser) noexcept{
TKind kind = Parser_kind(parser);
return TKind_is_ident(kind) && TKind_ident(kind) == mlc::String("export") ? [&]() -> DeclResult { 
  DeclResult inner = parse_decl(Parser_advance(parser));
  return DeclResult{std::make_shared<Decl>(DeclExported(inner.decl)), inner.parser};
 }() : TKind_is_fn(kind) || TKind_is_extern(kind) && TKind_is_fn(Parser_kind(Parser_advance(parser))) ? parse_fn_decl(parser) : TKind_is_type(kind) ? parse_type_decl(Parser_advance(parser)) : TKind_is_extend(kind) ? parse_extend_decl(Parser_advance(parser)) : TKind_is_import(kind) ? parse_import_decl(Parser_advance(parser)) : DeclResult{std::make_shared<Decl>(DeclFn(mlc::String("__skip__"), {}, std::make_shared<TypeExpr>(TyUnit{}), std::make_shared<Expr>(ExprUnit{}))), Parser_advance(parser)};
}
DeclResult parse_fn_decl(Parser parser) noexcept{
Parser fn_start = TKind_is_extern(Parser_kind(parser)) ? Parser_advance(parser) : parser;
mlc::String fn_name = TKind_ident(Parser_kind(Parser_advance(fn_start)));
ParamsResult params_result = parse_params(Parser_advance_by(fn_start, 3));
TypeResult ret_type_result = parse_type(Parser_advance(params_result.parser));
ExprResult body_result = parse_expr(Parser_advance(ret_type_result.parser));
return DeclResult{std::make_shared<Decl>(DeclFn(fn_name, params_result.params, ret_type_result.type_expr, body_result.expr)), body_result.parser};
}
ParamsResult parse_params(Parser parser) noexcept{
mlc::Array<std::shared_ptr<Param>> params = {};
return TKind_is_rparen(Parser_kind(parser)) ? ParamsResult{params, Parser_advance(parser)} : [&]() -> ParamsResult { 
  ParamResult first = parse_param(parser);
  params.push_back(first.param);
  Parser state = first.parser;
  while (TKind_is_comma(Parser_kind(state))){
{
ParamResult next = parse_param(Parser_advance(state));
params.push_back(next.param);
state = next.parser;
}
}
  return ParamsResult{params, Parser_advance(state)};
 }();
}
ParamResult parse_param(Parser parser) noexcept{
bool is_mutable = TKind_is_mut(Parser_kind(parser));
Parser name_pos = is_mutable ? Parser_advance(parser) : parser;
TypeResult type_result = parse_type(Parser_advance_by(name_pos, 2));
return ParamResult{std::make_shared<Param>(Param{TKind_ident(Parser_kind(name_pos)), is_mutable, type_result.type_expr}), type_result.parser};
}
DeclResult parse_type_decl(Parser parser) noexcept{
mlc::String type_name = TKind_ident(Parser_kind(parser));
Parser after_eq = Parser_advance_by(parser, 2);
TKind first_kind = Parser_kind(after_eq);
return TKind_is_lbrace(first_kind) ? [&]() -> DeclResult { 
  FieldDefsResult field_defs_result = parse_field_defs(Parser_advance(after_eq));
  mlc::Array<std::shared_ptr<TypeVariant>> variants = {};
  variants.push_back(std::make_shared<TypeVariant>(VarRecord(type_name, field_defs_result.field_defs)));
  return DeclResult{std::make_shared<Decl>(DeclType(type_name, variants)), field_defs_result.parser};
 }() : TKind_is_ident(first_kind) && is_ctor_name(TKind_ident(first_kind)) && TKind_is_lbrace(Parser_kind(Parser_advance(after_eq))) ? [&]() -> DeclResult { 
  FieldDefsResult field_defs_result = parse_field_defs(Parser_advance_by(after_eq, 2));
  mlc::Array<std::shared_ptr<TypeVariant>> variants = {};
  variants.push_back(std::make_shared<TypeVariant>(VarRecord(TKind_ident(first_kind), field_defs_result.field_defs)));
  return DeclResult{std::make_shared<Decl>(DeclType(type_name, variants)), field_defs_result.parser};
 }() : [&]() -> DeclResult { 
  VariantsResult variants_result = parse_variants(after_eq);
  return DeclResult{std::make_shared<Decl>(DeclType(type_name, variants_result.variants)), variants_result.parser};
 }();
}
bool is_decl_start(TKind kind) noexcept{return TKind_is_type(kind) || TKind_is_fn(kind) || TKind_is_extend(kind) || TKind_is_import(kind) || TKind_is_ident(kind) && TKind_ident(kind) == mlc::String("export") || TKind_ident(kind) == mlc::String("type") || TKind_ident(kind) == mlc::String("fn") || TKind_ident(kind) == mlc::String("extend") || TKind_ident(kind) == mlc::String("import");}
bool parse_variants_continue(Parser state) noexcept{return Parser_at_eof(state) ? false : is_decl_start(Parser_kind(state)) ? false : TKind_is_bar(Parser_kind(state)) || TKind_is_ident(Parser_kind(state));}
VariantsResult parse_variants(Parser parser) noexcept{
mlc::Array<std::shared_ptr<TypeVariant>> variants = {};
Parser state = TKind_is_bar(Parser_kind(parser)) ? Parser_advance(parser) : parser;
while (parse_variants_continue(state)){
{
VariantResult variant_result = parse_variant(state);
variants.push_back(variant_result.variant);
state = variant_result.parser;
if (TKind_is_bar(Parser_kind(state))){
{
state = Parser_advance(state);
}
}
}
}
return VariantsResult{variants, state};
}
VariantResult parse_variant(Parser parser) noexcept{
mlc::String variant_name = TKind_ident(Parser_kind(parser));
Parser after_name = Parser_advance(parser);
return TKind_is_lparen(Parser_kind(after_name)) ? [&]() -> VariantResult { 
  TypesResult types_result = parse_tuple_types(Parser_advance(after_name));
  return VariantResult{std::make_shared<TypeVariant>(VarTuple(variant_name, types_result.types)), types_result.parser};
 }() : TKind_is_lbrace(Parser_kind(after_name)) ? [&]() -> VariantResult { 
  FieldDefsResult field_defs_result = parse_field_defs(Parser_advance(after_name));
  return VariantResult{std::make_shared<TypeVariant>(VarRecord(variant_name, field_defs_result.field_defs)), field_defs_result.parser};
 }() : VariantResult{std::make_shared<TypeVariant>(VarUnit(variant_name)), after_name};
}
TypesResult parse_tuple_types(Parser parser) noexcept{
mlc::Array<std::shared_ptr<TypeExpr>> types = {};
TypeResult first = parse_type(parser);
types.push_back(first.type_expr);
Parser state = first.parser;
while (TKind_is_comma(Parser_kind(state))){
{
TypeResult next = parse_type(Parser_advance(state));
types.push_back(next.type_expr);
state = next.parser;
}
}
return TypesResult{types, Parser_advance(state)};
}
FieldDefsResult parse_field_defs(Parser parser) noexcept{
mlc::Array<std::shared_ptr<FieldDef>> field_defs = {};
Parser state = std::move(parser);
while (!TKind_is_rbrace(Parser_kind(state)) && !Parser_at_eof(state)){
{
mlc::String field_name = TKind_ident(Parser_kind(state));
TypeResult type_result = parse_type(Parser_advance_by(state, 2));
field_defs.push_back(std::make_shared<FieldDef>(FieldDef{field_name, type_result.type_expr}));
state = type_result.parser;
if (TKind_is_comma(Parser_kind(state))){
{
state = Parser_advance(state);
}
}
}
}
return FieldDefsResult{field_defs, Parser_advance(state)};
}
Program parse_program(mlc::Array<Token> tokens) noexcept{
mlc::Array<std::shared_ptr<Decl>> decls = {};
Parser state = parser_new(tokens);
while (!Parser_at_eof(state)){
{
DeclResult result = parse_decl(state);
decls.push_back(result.decl);
state = result.parser;
}
}
return Program{decls};
}
Program parse_source(mlc::String source) noexcept{return parse_program(tokenize(source).tokens);}
constexpr int decl_count(mlc::String source) noexcept{return parse_source(source).decls.size();}
std::shared_ptr<Decl> first_decl(mlc::String source) noexcept{return parse_source(source).decls[0];}
mlc::Array<TestResult> parser_tests() noexcept{
mlc::Array<TestResult> results = {};
results.push_back(assert_eq_int(mlc::String("empty source → 0 decls"), decl_count(mlc::String("")), 0));
results.push_back(assert_eq_int(mlc::String("single fn decl → 1 decl"), decl_count(mlc::String("fn f() -> i32 = 42")), 1));
results.push_back(assert_eq_int(mlc::String("two fn decls → 2 decls"), decl_count(mlc::String("fn f() -> i32 = 1\nfn g() -> i32 = 2")), 2));
results.push_back(assert_eq_int(mlc::String("type decl → 1 decl"), decl_count(mlc::String("type Color = Red | Green | Blue")), 1));
results.push_back(assert_eq_int(mlc::String("fn with params → 1 decl"), decl_count(mlc::String("fn add(x: i32, y: i32) -> i32 = x + y")), 1));
results.push_back(assert_eq_int(mlc::String("fn with do block → 1 decl"), decl_count(mlc::String("fn f() -> i32 = do\n  const x = 1\n  x\nend")), 1));
results.push_back(assert_eq_int(mlc::String("type + fn → 2 decls"), decl_count(mlc::String("type Color = Red\nfn f() -> i32 = 1")), 2));
results.push_back(assert_true(mlc::String("fn decl is DeclFn"), std::visit(overloaded{
  [&](const DeclFn& declfn) { auto [_w0, _w1, _w2, _w3] = declfn; return true; },
  [&](const DeclType& decltype_) { auto [_w0, _w1] = decltype_; return false; }
}, (*first_decl(mlc::String("fn f() -> i32 = 42"))))));
results.push_back(assert_true(mlc::String("type decl is DeclType"), std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [_w0, _w1] = decltype_; return true; },
  [&](const DeclFn& declfn) { auto [_w0, _w1, _w2, _w3] = declfn; return false; }
}, (*first_decl(mlc::String("type Color = Red | Green"))))));
results.push_back(assert_eq_int(mlc::String("fn with if expr → 1 decl"), decl_count(mlc::String("fn f(x: i32) -> i32 = if x > 0 then 1 else 0 end")), 1));
return results;
}
bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept{
bool found = false;
int index = 0;
while (index < scope.size()){
{
if (scope[index] == name){
{
found = true;
}
}
index = index + 1;
}
}
return found;
}
mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<Pat> pattern) noexcept{
mlc::Array<mlc::String> accumulator = {};
return collect_pattern_bindings(pattern, accumulator);
}
mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<Pat> pattern, mlc::Array<mlc::String> accumulator) noexcept{
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<PatIdent>((*pattern))) { auto _v_patident = std::get<PatIdent>((*pattern)); auto [name] = _v_patident; return [&]() -> mlc::Array<mlc::String> { 
  accumulator.push_back(name);
  return accumulator;
 }(); } if (std::holds_alternative<PatCtor>((*pattern))) { auto _v_patctor = std::get<PatCtor>((*pattern)); auto [_w0, sub_patterns] = _v_patctor; return [&]() -> mlc::Array<mlc::String> { 
  int index = 0;
  while (index < sub_patterns.size()){
{
accumulator = collect_pattern_bindings(sub_patterns[index], accumulator);
index = index + 1;
}
}
  return accumulator;
 }(); } return accumulator; }();
}
mlc::Array<mlc::String> collect_globals(Program prog) noexcept{
mlc::Array<mlc::String> names = {};
names.push_back(mlc::String("true"));
names.push_back(mlc::String("false"));
names.push_back(mlc::String("exit"));
names.push_back(mlc::String("print"));
names.push_back(mlc::String("println"));
names.push_back(mlc::String("args"));
names.push_back(mlc::String("File"));
names.push_back(mlc::String("Shared"));
names.push_back(mlc::String("Map"));
int i = 0;
while (i < prog.decls.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) { auto [name, _w0, _w1, _w2] = declfn; return [&]() -> std::tuple<> { 
  names.push_back(name);
  return std::make_tuple();
 }(); },
  [&](const DeclType& decltype_) { auto [name, variants] = decltype_; return [&]() -> std::tuple<> { 
  names.push_back(name);
  int j = 0;
  while (j < variants.size()){
{
std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [variant_name] = varunit; return [&]() -> std::tuple<> { 
  names.push_back(variant_name);
  return std::make_tuple();
 }(); },
  [&](const VarTuple& vartuple) { auto [variant_name, _w0] = vartuple; return [&]() -> std::tuple<> { 
  names.push_back(variant_name);
  return std::make_tuple();
 }(); },
  [&](const VarRecord& varrecord) { auto [variant_name, _w0] = varrecord; return [&]() -> std::tuple<> { 
  names.push_back(variant_name);
  return std::make_tuple();
 }(); }
}, (*variants[j]));
j = j + 1;
}
}
  return std::make_tuple();
 }(); },
  [&](const DeclExtend& declextend) { auto [_w0, _w1] = declextend; return std::make_tuple(); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return std::make_tuple(); },
  [&](const DeclExported& declexported) { auto [_w0] = declexported; return std::make_tuple(); }
}, (*decl_inner(prog.decls[i])));
i = i + 1;
}
}
return names;
}
mlc::Array<mlc::String> check_names_expr(std::shared_ptr<Expr> expr, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept{
return [&]() -> mlc::Array<mlc::String> { if (std::holds_alternative<ExprIdent>((*expr))) { auto _v_exprident = std::get<ExprIdent>((*expr)); auto [name] = _v_exprident; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = {};
  if (!scope_contains(locals, name) && !scope_contains(globals, name)){
{
errors.push_back(mlc::String("undefined: ") + name);
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprBin>((*expr))) { auto _v_exprbin = std::get<ExprBin>((*expr)); auto [_w0, left, right] = _v_exprbin; return errs_append(check_names_expr(left, locals, globals), check_names_expr(right, locals, globals)); } if (std::holds_alternative<ExprUn>((*expr))) { auto _v_exprun = std::get<ExprUn>((*expr)); auto [_w0, inner] = _v_exprun; return check_names_expr(inner, locals, globals); } if (std::holds_alternative<ExprCall>((*expr))) { auto _v_exprcall = std::get<ExprCall>((*expr)); auto [func, args] = _v_exprcall; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = check_names_expr(func, locals, globals);
  int i = 0;
  while (i < args.size()){
{
errors = errs_append(errors, check_names_expr(args[i], locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprMethod>((*expr))) { auto _v_exprmethod = std::get<ExprMethod>((*expr)); auto [obj, _w0, args] = _v_exprmethod; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = check_names_expr(obj, locals, globals);
  int i = 0;
  while (i < args.size()){
{
errors = errs_append(errors, check_names_expr(args[i], locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprField>((*expr))) { auto _v_exprfield = std::get<ExprField>((*expr)); auto [obj, _w0] = _v_exprfield; return check_names_expr(obj, locals, globals); } if (std::holds_alternative<ExprIndex>((*expr))) { auto _v_exprindex = std::get<ExprIndex>((*expr)); auto [obj, idx] = _v_exprindex; return errs_append(check_names_expr(obj, locals, globals), check_names_expr(idx, locals, globals)); } if (std::holds_alternative<ExprIf>((*expr))) { auto _v_exprif = std::get<ExprIf>((*expr)); auto [cond, then_expr, else_expr] = _v_exprif; return errs_append(errs_append(check_names_expr(cond, locals, globals), check_names_expr(then_expr, locals, globals)), check_names_expr(else_expr, locals, globals)); } if (std::holds_alternative<ExprBlock>((*expr))) { auto _v_exprblock = std::get<ExprBlock>((*expr)); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::Array<mlc::String> { 
  NameCheckResult stmt_result = check_names_stmts(stmts, locals, globals);
  return NameCheckResult_append_expr_errors(stmt_result, check_names_expr(result, stmt_result.scope, globals));
 }(); } if (std::holds_alternative<ExprWhile>((*expr))) { auto _v_exprwhile = std::get<ExprWhile>((*expr)); auto [cond, stmts] = _v_exprwhile; return [&]() -> mlc::Array<mlc::String> { 
  NameCheckResult check_result = check_names_stmts(stmts, locals, globals);
  return errs_append(check_names_expr(cond, locals, globals), check_result.errors);
 }(); } if (std::holds_alternative<ExprFor>((*expr))) { auto _v_exprfor = std::get<ExprFor>((*expr)); auto [var, iter, stmts] = _v_exprfor; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> inner = locals;
  inner.push_back(var);
  NameCheckResult check_result = check_names_stmts(stmts, inner, globals);
  return errs_append(check_names_expr(iter, locals, globals), check_result.errors);
 }(); } if (std::holds_alternative<ExprMatch>((*expr))) { auto _v_exprmatch = std::get<ExprMatch>((*expr)); auto [subject, arms] = _v_exprmatch; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = check_names_expr(subject, locals, globals);
  int i = 0;
  while (i < arms.size()){
{
mlc::Array<mlc::String> bindings = pattern_bindings(arms[i]->pat);
mlc::Array<mlc::String> arm_scope = locals;
int index = 0;
while (index < bindings.size()){
{
arm_scope.push_back(bindings[index]);
index = index + 1;
}
}
errors = errs_append(errors, check_names_expr(arms[i]->body, arm_scope, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprRecord>((*expr))) { auto _v_exprrecord = std::get<ExprRecord>((*expr)); auto [_w0, field_vals] = _v_exprrecord; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = {};
  int i = 0;
  while (i < field_vals.size()){
{
errors = errs_append(errors, check_names_expr(field_vals[i]->val, locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprRecordUpdate>((*expr))) { auto _v_exprrecordupdate = std::get<ExprRecordUpdate>((*expr)); auto [_w0, base, field_vals] = _v_exprrecordupdate; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = check_names_expr(base, locals, globals);
  int i = 0;
  while (i < field_vals.size()){
{
errors = errs_append(errors, check_names_expr(field_vals[i]->val, locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprArray>((*expr))) { auto _v_exprarray = std::get<ExprArray>((*expr)); auto [exprs] = _v_exprarray; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> errors = {};
  int i = 0;
  while (i < exprs.size()){
{
errors = errs_append(errors, check_names_expr(exprs[i], locals, globals));
i = i + 1;
}
}
  return errors;
 }(); } if (std::holds_alternative<ExprQuestion>((*expr))) { auto _v_exprquestion = std::get<ExprQuestion>((*expr)); auto [inner] = _v_exprquestion; return check_names_expr(inner, locals, globals); } if (std::holds_alternative<ExprLambda>((*expr))) { auto _v_exprlambda = std::get<ExprLambda>((*expr)); auto [params, body] = _v_exprlambda; return [&]() -> mlc::Array<mlc::String> { 
  mlc::Array<mlc::String> lambda_locals = {};
  int i = 0;
  while (i < locals.size()){
{
lambda_locals.push_back(locals[i]);
i = i + 1;
}
}
  int j = 0;
  while (j < params.size()){
{
lambda_locals.push_back(params[j]);
j = j + 1;
}
}
  return check_names_expr(body, lambda_locals, globals);
 }(); } return {}; }();
}
constexpr mlc::Array<mlc::String> NameCheckResult_append_expr_errors(NameCheckResult self, mlc::Array<mlc::String> expr_errors) noexcept{return errs_append(self.errors, expr_errors);}
NameCheckResult check_names_stmts(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept{
mlc::Array<mlc::String> errors = {};
mlc::Array<mlc::String> scope = locals;
int i = 0;
while (i < stmts.size()){
{
std::visit(overloaded{
  [&](const StmtLet& stmtlet) { auto [name, _w0, _w1, value] = stmtlet; return [&]() -> std::tuple<> { 
  errors = errs_append(errors, check_names_expr(value, scope, globals));
  scope.push_back(name);
  return std::make_tuple();
 }(); },
  [&](const StmtExpr& stmtexpr) { auto [expr] = stmtexpr; return [&]() -> std::tuple<> { 
  errors = errs_append(errors, check_names_expr(expr, scope, globals));
  return std::make_tuple();
 }(); },
  [&](const StmtReturn& stmtreturn) { auto [expr] = stmtreturn; return [&]() -> std::tuple<> { 
  errors = errs_append(errors, check_names_expr(expr, scope, globals));
  return std::make_tuple();
 }(); },
  [&](const StmtBreak& stmtbreak) { return std::make_tuple(); },
  [&](const StmtContinue& stmtcontinue) { return std::make_tuple(); }
}, (*stmts[i]));
i = i + 1;
}
}
return NameCheckResult{errors, scope};
}
std::shared_ptr<Type> TypeRegistry_fn_type(TypeRegistry self, mlc::String name) noexcept{return self.fn_types.get(name);}
constexpr bool TypeRegistry_has_fn(TypeRegistry self, mlc::String name) noexcept{return self.fn_types.has(name);}
std::shared_ptr<Type> TypeRegistry_ctor_type(TypeRegistry self, mlc::String name) noexcept{return self.ctor_types.get(name);}
constexpr bool TypeRegistry_has_ctor(TypeRegistry self, mlc::String name) noexcept{return self.ctor_types.has(name);}
mlc::Array<std::shared_ptr<Type>> TypeRegistry_ctor_params_for(TypeRegistry self, mlc::String name) noexcept{return self.ctor_params.has(name) ? self.ctor_params.get(name) : mlc::Array<std::shared_ptr<Type>>{};}
constexpr mlc::HashMap<mlc::String, std::shared_ptr<Type>> TypeRegistry_fields_for(TypeRegistry self, mlc::String type_name) noexcept{return self.field_types.get(type_name);}
constexpr bool TypeRegistry_has_fields(TypeRegistry self, mlc::String type_name) noexcept{return self.field_types.has(type_name);}
TypeRegistry empty_registry() noexcept{return TypeRegistry{mlc::HashMap<mlc::String, std::shared_ptr<Type>>(), mlc::HashMap<mlc::String, std::shared_ptr<Type>>(), mlc::HashMap<mlc::String, mlc::Array<std::shared_ptr<Type>>>(), mlc::HashMap<mlc::String, mlc::HashMap<mlc::String, std::shared_ptr<Type>>>()};}
std::shared_ptr<Type> type_from_annotation(std::shared_ptr<TypeExpr> type_expr) noexcept{return [&]() -> std::shared_ptr<Type> { if (std::holds_alternative<TyI32>((*type_expr))) {  return std::make_shared<Type>(TI32{}); } if (std::holds_alternative<TyString>((*type_expr))) {  return std::make_shared<Type>(TString{}); } if (std::holds_alternative<TyBool>((*type_expr))) {  return std::make_shared<Type>(TBool{}); } if (std::holds_alternative<TyUnit>((*type_expr))) {  return std::make_shared<Type>(TUnit{}); } if (std::holds_alternative<TyNamed>((*type_expr))) { auto _v_tynamed = std::get<TyNamed>((*type_expr)); auto [name] = _v_tynamed; return std::make_shared<Type>(TNamed(name)); } if (std::holds_alternative<TyArray>((*type_expr))) { auto _v_tyarray = std::get<TyArray>((*type_expr)); auto [inner] = _v_tyarray; return std::make_shared<Type>(TArray(type_from_annotation(inner))); } if (std::holds_alternative<TyShared>((*type_expr))) { auto _v_tyshared = std::get<TyShared>((*type_expr)); auto [inner] = _v_tyshared; return std::make_shared<Type>(TShared(type_from_annotation(inner))); } if (std::holds_alternative<TyFn>((*type_expr))) { auto _v_tyfn = std::get<TyFn>((*type_expr)); auto [params, ret] = _v_tyfn; return [&]() -> std::shared_ptr<Type> { 
  mlc::Array<std::shared_ptr<Type>> param_types = {};
  int i = 0;
  while (i < params.size()){
{
param_types.push_back(type_from_annotation(params[i]));
i = i + 1;
}
}
  return std::make_shared<Type>(TFn(param_types, type_from_annotation(ret)));
 }(); } return std::make_shared<Type>(TUnknown{}); }();}
TypeRegistry build_registry(Program prog) noexcept{
TypeRegistry registry = empty_registry();
int i = 0;
while (i < prog.decls.size()){
{
registry = register_decl(registry, prog.decls[i]);
i = i + 1;
}
}
return registry;
}
TypeRegistry register_decl(TypeRegistry registry, std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{
  [&](const DeclExtend& declextend) { auto [_w0, _w1] = declextend; return registry; },
  [&](const DeclFn& declfn) { auto [name, params, return_type, _w0] = declfn; return [&]() -> TypeRegistry { 
  mlc::Array<std::shared_ptr<Type>> param_types = {};
  int i = 0;
  while (i < params.size()){
{
param_types.push_back(type_from_annotation(params[i]->typ));
i = i + 1;
}
}
  registry.fn_types.set(name, std::make_shared<Type>(TFn(param_types, type_from_annotation(return_type))));
  return registry;
 }(); },
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return [&]() -> TypeRegistry { 
  int i = 0;
  while (i < variants.size()){
{
registry = register_variant(registry, type_name, variants[i]);
i = i + 1;
}
}
  return registry;
 }(); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return registry; },
  [&](const DeclExported& declexported) { auto [d] = declexported; return register_decl(registry, d); }
}, (*decl));
}
TypeRegistry register_variant(TypeRegistry registry, mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept{
std::shared_ptr<Type> result_type = std::make_shared<Type>(TNamed(type_name));
return std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [variant_name] = varunit; return [&]() -> TypeRegistry { 
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, {});
  return registry;
 }(); },
  [&](const VarTuple& vartuple) { auto [variant_name, field_types] = vartuple; return [&]() -> TypeRegistry { 
  mlc::Array<std::shared_ptr<Type>> field_type_list = {};
  int i = 0;
  while (i < field_types.size()){
{
field_type_list.push_back(type_from_annotation(field_types[i]));
i = i + 1;
}
}
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, field_type_list);
  return registry;
 }(); },
  [&](const VarRecord& varrecord) { auto [variant_name, field_defs] = varrecord; return [&]() -> TypeRegistry { 
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> field_map = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
  int i = 0;
  while (i < field_defs.size()){
{
field_map.set(field_defs[i]->name, type_from_annotation(field_defs[i]->typ));
i = i + 1;
}
}
  registry.ctor_types.set(variant_name, result_type);
  registry.ctor_params.set(variant_name, {});
  registry.field_types.set(variant_name, field_map);
  registry.field_types.set(type_name, field_map);
  return registry;
 }(); }
}, (*variant));
}
InferResult InferResult_with_type(InferResult self, std::shared_ptr<Type> new_type) noexcept{return InferResult{new_type, self.errors};}
InferResult InferResult_absorb(InferResult self, InferResult other) noexcept{return InferResult{self.inferred_type, errs_append(self.errors, other.errors)};}
InferResult InferResult_absorb_stmt(InferResult self, StmtInferResult stmt_result) noexcept{return InferResult{self.inferred_type, errs_append(stmt_result.errors, self.errors)};}
InferResult infer_ok(std::shared_ptr<Type> type_val) noexcept{return InferResult{type_val, {}};}
std::shared_ptr<Type> binary_op_type(mlc::String op, std::shared_ptr<Type> left_type) noexcept{return op == mlc::String("+") || op == mlc::String("-") || op == mlc::String("*") || op == mlc::String("/") || op == mlc::String("%") ? [&]() { if (std::holds_alternative<TString>((*left_type))) {  return std::make_shared<Type>(TString{}); } return std::make_shared<Type>(TI32{}); }() : op == mlc::String("=") ? std::make_shared<Type>(TUnit{}) : std::make_shared<Type>(TBool{});}
std::shared_ptr<Type> method_return_type(mlc::String method_name) noexcept{return method_name == mlc::String("length") || method_name == mlc::String("size") || method_name == mlc::String("to_i") ? std::make_shared<Type>(TI32{}) : method_name == mlc::String("push") || method_name == mlc::String("set") ? std::make_shared<Type>(TUnit{}) : method_name == mlc::String("char_at") || method_name == mlc::String("join") || method_name == mlc::String("to_string") || method_name == mlc::String("substring") || method_name == mlc::String("to_lower") ? std::make_shared<Type>(TString{}) : method_name == mlc::String("has") ? std::make_shared<Type>(TBool{}) : std::make_shared<Type>(TUnknown{});}
InferResult infer_args_errors(InferResult initial, mlc::Array<std::shared_ptr<Expr>> args, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept{
InferResult result = std::move(initial);
int index = 0;
while (index < args.size()){
{
result = InferResult_absorb(result, infer_expr(args[index], type_env, registry));
index = index + 1;
}
}
return result;
}
InferResult infer_field_vals_errors(InferResult initial, mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept{
InferResult result = std::move(initial);
int index = 0;
while (index < field_vals.size()){
{
result = InferResult_absorb(result, infer_expr(field_vals[index]->val, type_env, registry));
index = index + 1;
}
}
return result;
}
std::shared_ptr<Type> field_lookup_type(std::shared_ptr<Type> obj_type, mlc::String field_name, TypeRegistry registry) noexcept{
mlc::String type_name = [&]() -> mlc::String { if (std::holds_alternative<TNamed>((*obj_type))) { auto _v_tnamed = std::get<TNamed>((*obj_type)); auto [name] = _v_tnamed; return name; } return mlc::String(""); }();
return type_name != mlc::String("") && TypeRegistry_has_fields(registry, type_name) ? [&]() -> std::shared_ptr<Type> { 
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> field_map = TypeRegistry_fields_for(registry, type_name);
  return field_map.has(field_name) ? field_map.get(field_name) : std::make_shared<Type>(TUnknown{});
 }() : std::make_shared<Type>(TUnknown{});
}
InferResult infer_expr(std::shared_ptr<Expr> expr, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept{
return std::visit(overloaded{
  [&](const ExprInt& exprint) { auto [_w0] = exprint; return infer_ok(std::make_shared<Type>(TI32{})); },
  [&](const ExprStr& exprstr) { auto [_w0] = exprstr; return infer_ok(std::make_shared<Type>(TString{})); },
  [&](const ExprBool& exprbool) { auto [_w0] = exprbool; return infer_ok(std::make_shared<Type>(TBool{})); },
  [&](const ExprUnit& exprunit) { return infer_ok(std::make_shared<Type>(TUnit{})); },
  [&](const ExprIdent& exprident) { auto [name] = exprident; return type_env.has(name) ? infer_ok(type_env.get(name)) : TypeRegistry_has_fn(registry, name) ? infer_ok(TypeRegistry_fn_type(registry, name)) : TypeRegistry_has_ctor(registry, name) ? infer_ok(TypeRegistry_ctor_type(registry, name)) : infer_ok(std::make_shared<Type>(TUnknown{})); },
  [&](const ExprBin& exprbin) { auto [op, left, right] = exprbin; return [&]() -> InferResult { 
  InferResult left_result = infer_expr(left, type_env, registry);
  InferResult right_result = infer_expr(right, type_env, registry);
  return InferResult_with_type(InferResult_absorb(left_result, right_result), binary_op_type(op, left_result.inferred_type));
 }(); },
  [&](const ExprUn& exprun) { auto [op, inner] = exprun; return [&]() -> InferResult { 
  InferResult inner_result = infer_expr(inner, type_env, registry);
  auto result_type = op == mlc::String("!") ? std::make_shared<Type>(TBool{}) : inner_result.inferred_type;
  return InferResult_with_type(inner_result, result_type);
 }(); },
  [&](const ExprCall& exprcall) { auto [func, args] = exprcall; return [&]() -> InferResult { 
  InferResult func_result = infer_expr(func, type_env, registry);
  InferResult with_args = infer_args_errors(func_result, args, type_env, registry);
  std::shared_ptr<Type> return_type = [&]() -> std::shared_ptr<Type> { if (std::holds_alternative<TFn>((*func_result.inferred_type))) { auto _v_tfn = std::get<TFn>((*func_result.inferred_type)); auto [_w0, ret] = _v_tfn; return ret; } return std::make_shared<Type>(TUnknown{}); }();
  return InferResult_with_type(with_args, return_type);
 }(); },
  [&](const ExprMethod& exprmethod) { auto [obj, method_name, args] = exprmethod; return [&]() -> InferResult { 
  InferResult obj_result = infer_expr(obj, type_env, registry);
  return InferResult_with_type(infer_args_errors(obj_result, args, type_env, registry), method_return_type(method_name));
 }(); },
  [&](const ExprField& exprfield) { auto [obj, field_name] = exprfield; return [&]() -> InferResult { 
  InferResult obj_result = infer_expr(obj, type_env, registry);
  return InferResult_with_type(obj_result, field_lookup_type(obj_result.inferred_type, field_name, registry));
 }(); },
  [&](const ExprIndex& exprindex) { auto [obj, idx] = exprindex; return [&]() -> InferResult { 
  InferResult obj_result = infer_expr(obj, type_env, registry);
  InferResult idx_result = infer_expr(idx, type_env, registry);
  std::shared_ptr<Type> elem_type = [&]() -> std::shared_ptr<Type> { if (std::holds_alternative<TArray>((*obj_result.inferred_type))) { auto _v_tarray = std::get<TArray>((*obj_result.inferred_type)); auto [inner] = _v_tarray; return inner; } return std::make_shared<Type>(TUnknown{}); }();
  return InferResult_with_type(InferResult_absorb(obj_result, idx_result), elem_type);
 }(); },
  [&](const ExprIf& exprif) { auto [cond, then_expr, else_expr] = exprif; return [&]() -> InferResult { 
  InferResult cond_result = infer_expr(cond, type_env, registry);
  InferResult then_result = infer_expr(then_expr, type_env, registry);
  InferResult else_result = infer_expr(else_expr, type_env, registry);
  return InferResult_absorb(InferResult_absorb(then_result, cond_result), else_result);
 }(); },
  [&](const ExprBlock& exprblock) { auto [stmts, result] = exprblock; return [&]() -> InferResult { 
  StmtInferResult stmts_result = infer_stmts(stmts, type_env, registry);
  InferResult result_infer = infer_expr(result, stmts_result.type_env, registry);
  return InferResult_absorb_stmt(result_infer, stmts_result);
 }(); },
  [&](const ExprWhile& exprwhile) { auto [cond, stmts] = exprwhile; return [&]() -> InferResult { 
  InferResult cond_result = infer_expr(cond, type_env, registry);
  StmtInferResult stmts_result = infer_stmts(stmts, type_env, registry);
  return InferResult_with_type(InferResult_absorb_stmt(cond_result, stmts_result), std::make_shared<Type>(TUnit{}));
 }(); },
  [&](const ExprFor& exprfor) { auto [var, iter, stmts] = exprfor; return [&]() -> InferResult { 
  InferResult iter_result = infer_expr(iter, type_env, registry);
  std::shared_ptr<Type> elem_type = [&]() -> std::shared_ptr<Type> { if (std::holds_alternative<TArray>((*iter_result.inferred_type))) { auto _v_tarray = std::get<TArray>((*iter_result.inferred_type)); auto [inner] = _v_tarray; return inner; } return std::make_shared<Type>(TUnknown{}); }();
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> inner_env = type_env;
  inner_env.set(var, elem_type);
  StmtInferResult stmts_result = infer_stmts(stmts, inner_env, registry);
  return InferResult_with_type(InferResult_absorb_stmt(iter_result, stmts_result), std::make_shared<Type>(TUnit{}));
 }(); },
  [&](const ExprMatch& exprmatch) { auto [subject, arms] = exprmatch; return [&]() -> InferResult { 
  InferResult subject_result = infer_expr(subject, type_env, registry);
  mlc::Array<mlc::String> errors = subject_result.errors;
  std::shared_ptr<Type> arm_type = std::make_shared<Type>(TUnknown{});
  int i = 0;
  while (i < arms.size()){
{
mlc::HashMap<mlc::String, std::shared_ptr<Type>> arm_env = env_for_pat(type_env, arms[i]->pat, registry);
InferResult arm_result = infer_expr(arms[i]->body, arm_env, registry);
errors = errs_append(errors, arm_result.errors);
if (i == 0){
{
arm_type = arm_result.inferred_type;
}
}
i = i + 1;
}
}
  return InferResult{arm_type, errors};
 }(); },
  [&](const ExprRecord& exprrecord) { auto [name, field_vals] = exprrecord; return infer_field_vals_errors(infer_ok(std::make_shared<Type>(TNamed(name))), field_vals, type_env, registry); },
  [&](const ExprRecordUpdate& exprrecordupdate) { auto [name, base, field_vals] = exprrecordupdate; return [&]() -> InferResult { 
  InferResult base_result = infer_expr(base, type_env, registry);
  return infer_field_vals_errors(InferResult_with_type(base_result, std::make_shared<Type>(TNamed(name))), field_vals, type_env, registry);
 }(); },
  [&](const ExprArray& exprarray) { auto [exprs] = exprarray; return [&]() -> InferResult { 
  std::shared_ptr<Type> first_type = exprs.size() > 0 ? infer_expr(exprs[0], type_env, registry).inferred_type : std::make_shared<Type>(TUnknown{});
  return infer_args_errors(infer_ok(std::make_shared<Type>(TArray(first_type))), exprs, type_env, registry);
 }(); },
  [&](const ExprQuestion& exprquestion) { auto [inner] = exprquestion; return infer_expr(inner, type_env, registry); },
  [&](const ExprLambda& exprlambda) { auto [params, body] = exprlambda; return [&]() -> InferResult { 
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> lambda_env = type_env;
  int i = 0;
  while (i < params.size()){
{
lambda_env.set(params[i], std::make_shared<Type>(TUnknown{}));
i = i + 1;
}
}
  InferResult ret_result = infer_expr(body, lambda_env, registry);
  return infer_ok(std::make_shared<Type>(TFn({}, ret_result.inferred_type)));
 }(); }
}, (*expr));
}
StmtInferResult infer_stmts(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept{
mlc::Array<mlc::String> errors = {};
mlc::HashMap<mlc::String, std::shared_ptr<Type>> current_env = type_env;
int i = 0;
while (i < stmts.size()){
{
std::visit(overloaded{
  [&](const StmtLet& stmtlet) {
auto [name, _w0, _w1, value] = stmtlet;
{
InferResult value_result = infer_expr(value, current_env, registry);
current_env.set(name, value_result.inferred_type);
errors = errs_append(errors, value_result.errors);
}
},
  [&](const StmtExpr& stmtexpr) {
auto [expr] = stmtexpr;
{
InferResult expr_res = infer_expr(expr, current_env, registry);
errors = errs_append(errors, expr_res.errors);
}
},
  [&](const StmtReturn& stmtreturn) {
auto [expr] = stmtreturn;
{
InferResult ret_res = infer_expr(expr, current_env, registry);
errors = errs_append(errors, ret_res.errors);
}
},
  [&](const StmtBreak& stmtbreak) {
{
}
},
  [&](const StmtContinue& stmtcontinue) {
{
}
}
}, (*stmts[i]));
i = i + 1;
}
}
return StmtInferResult{current_env, errors};
}
mlc::HashMap<mlc::String, std::shared_ptr<Type>> env_for_pat(mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, std::shared_ptr<Pat> pat, TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<Type>> { if (std::holds_alternative<PatIdent>((*pat))) { auto _v_patident = std::get<PatIdent>((*pat)); auto [name] = _v_patident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> env = type_env;
  env.set(name, std::make_shared<Type>(TUnknown{}));
  return env;
 }(); } if (std::holds_alternative<PatCtor>((*pat))) { auto _v_patctor = std::get<PatCtor>((*pat)); auto [ctor_name, sub_pats] = _v_patctor; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<Type>> { 
  mlc::Array<std::shared_ptr<Type>> param_types = TypeRegistry_ctor_params_for(registry, ctor_name);
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> current_env = type_env;
  int i = 0;
  while (i < sub_pats.size()){
{
std::shared_ptr<Type> sub_type = i < param_types.size() ? param_types[i] : std::make_shared<Type>(TUnknown{});
current_env = env_for_pat_with_type(current_env, sub_pats[i], sub_type, registry);
i = i + 1;
}
}
  return current_env;
 }(); } return type_env; }();
}
mlc::HashMap<mlc::String, std::shared_ptr<Type>> env_for_pat_with_type(mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env, std::shared_ptr<Pat> pat, std::shared_ptr<Type> type_val, TypeRegistry registry) noexcept{
return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<Type>> { if (std::holds_alternative<PatIdent>((*pat))) { auto _v_patident = std::get<PatIdent>((*pat)); auto [name] = _v_patident; return [&]() -> mlc::HashMap<mlc::String, std::shared_ptr<Type>> { 
  mlc::HashMap<mlc::String, std::shared_ptr<Type>> env = type_env;
  env.set(name, type_val);
  return env;
 }(); } return env_for_pat(type_env, pat, registry); }();
}
constexpr bool CheckOut_has_errors(CheckOut self) noexcept{return self.errors.size() > 0;}
CheckOut check(Program prog) noexcept{
mlc::Array<mlc::String> globals = collect_globals(prog);
TypeRegistry registry = build_registry(prog);
mlc::Array<mlc::String> all_errors = {};
int i = 0;
while (i < prog.decls.size()){
{
std::visit(overloaded{
  [&](const DeclFn& declfn) {
auto [_w0, params, _w1, body] = declfn;
{
mlc::Array<mlc::String> locals = {};
mlc::HashMap<mlc::String, std::shared_ptr<Type>> type_env = mlc::HashMap<mlc::String, std::shared_ptr<Type>>();
int j = 0;
while (j < params.size()){
{
locals.push_back(params[j]->name);
type_env.set(params[j]->name, type_from_annotation(params[j]->typ));
j = j + 1;
}
}
all_errors = errs_append(all_errors, check_names_expr(body, locals, globals));
InferResult infer_result = infer_expr(body, type_env, registry);
all_errors = errs_append(all_errors, infer_result.errors);
}
},
  [&](const DeclType& decltype_) {
auto [_w0, _w1] = decltype_;
{
}
},
  [&](const DeclExtend& declextend) {
auto [_w0, _w1] = declextend;
{
}
},
  [&](const DeclImport& declimport) {
auto [_w0, _w1] = declimport;
{
}
},
  [&](const DeclExported& declexported) {
auto [d] = declexported;
{
}
}
}, (*decl_inner(prog.decls[i])));
i = i + 1;
}
}
return CheckOut{all_errors, registry};
}
CheckOut check_source(mlc::String source) noexcept{return check(parse_program(tokenize(source).tokens));}
constexpr int check_error_count(mlc::String source) noexcept{return check_source(source).errors.size();}
mlc::Array<TestResult> checker_tests() noexcept{
mlc::Array<TestResult> results = {};
results.push_back(assert_eq_int(mlc::String("empty program → 0 errors"), check_error_count(mlc::String("")), 0));
results.push_back(assert_eq_int(mlc::String("fn returning literal → 0 errors"), check_error_count(mlc::String("fn f() -> i32 = 42")), 0));
results.push_back(assert_eq_int(mlc::String("fn using its own param → 0 errors"), check_error_count(mlc::String("fn f(x: i32) -> i32 = x")), 0));
results.push_back(assert_eq_int(mlc::String("fn calling another fn → 0 errors"), check_error_count(mlc::String("fn add(x: i32, y: i32) -> i32 = x + y\nfn main() -> i32 = add(1, 2)")), 0));
results.push_back(assert_true(mlc::String("undefined name → at least 1 error"), check_error_count(mlc::String("fn f() -> i32 = undefined_name")) > 0));
results.push_back(assert_eq_int(mlc::String("type decl with variants → 0 errors"), check_error_count(mlc::String("type Color = Red | Green | Blue")), 0));
results.push_back(assert_eq_int(mlc::String("fn returning bool → 0 errors"), check_error_count(mlc::String("fn f() -> bool = true")), 0));
results.push_back(assert_eq_int(mlc::String("fn with const binding → 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  const x = 1\n  x\nend")), 0));
results.push_back(assert_true(mlc::String("two undefined names → at least 1 error"), check_error_count(mlc::String("fn f() -> i32 = foo + bar")) > 0));
results.push_back(assert_eq_int(mlc::String("builtin print → 0 errors"), check_error_count(mlc::String("fn f() -> i32 = do\n  print(\"hi\")\n  0\nend")), 0));
return results;
}
CodegenContext create_codegen_context(Program prog) noexcept{return CodegenContext{build_field_orders(prog)};}
mlc::Array<std::shared_ptr<FieldOrder>> build_field_orders(Program prog) noexcept{
mlc::Array<std::shared_ptr<FieldOrder>> orders = {};
int i = 0;
while (i < prog.decls.size()){
{
std::visit(overloaded{
  [&](const DeclExported& declexported) { auto [d] = declexported; return [&]() { 
  std::visit(overloaded{
  [&](const DeclType& decltype_) {
auto [type_name, variants] = decltype_;
{
int j = 0;
while (j < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) {
auto [variant_name, field_defs] = varrecord;
{
mlc::Array<mlc::String> field_names = {};
int k = 0;
while (k < field_defs.size()){
{
field_names.push_back(field_defs[k]->name);
k = k + 1;
}
}
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{variant_name, field_names}));
if (variants.size() == 1){
{
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{type_name, field_names}));
}
}
int _d1 = 0;
}
},
  [&](const VarTuple& vartuple) {
auto [_w0, _w1] = vartuple;
{
}
},
  [&](const VarUnit& varunit) {
auto [_w0] = varunit;
{
}
}
}, (*variants[j]));
j = j + 1;
}
}
int _d2 = 0;
}
},
  [&](const DeclFn& declfn) {
auto [_w0, _w1, _w2, _w3] = declfn;
{
}
},
  [&](const DeclExtend& declextend) {
auto [_w0, _w1] = declextend;
{
}
},
  [&](const DeclImport& declimport) {
auto [_w0, _w1] = declimport;
{
}
},
  [&](const DeclExported& declexported) {
auto [_w0] = declexported;
{
}
}
}, (*decl_inner(d)));
 }(); },
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return [&]() { 
  int j = 0;
  while (j < variants.size()){
{
std::visit(overloaded{
  [&](const VarRecord& varrecord) {
auto [variant_name, field_defs] = varrecord;
{
mlc::Array<mlc::String> field_names = {};
int k = 0;
while (k < field_defs.size()){
{
field_names.push_back(field_defs[k]->name);
k = k + 1;
}
}
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{variant_name, field_names}));
if (variants.size() == 1){
{
orders.push_back(std::make_shared<FieldOrder>(FieldOrder{type_name, field_names}));
}
}
int _d1 = 0;
}
},
  [&](const VarTuple& vartuple) {
auto [_w0, _w1] = vartuple;
{
}
},
  [&](const VarUnit& varunit) {
auto [_w0] = varunit;
{
}
}
}, (*variants[j]));
j = j + 1;
}
}
  int _d2 = 0;
 }(); },
  [&](const DeclFn& declfn) { auto [_w0, _w1, _w2, _w3] = declfn; return ; },
  [&](const DeclExtend& declextend) { auto [_w0, _w1] = declextend; return ; },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return ; }
}, (*prog.decls[i]));
i = i + 1;
}
}
return orders;
}
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept{
mlc::Array<mlc::String> result = {};
int i = 0;
bool found = false;
while (i < orders.size() && !found){
{
if (orders[i]->type_name == name){
{
result = orders[i]->fields;
found = true;
}
}
i = i + 1;
}
}
return result;
}
constexpr bool cpp_keyword(mlc::String name) noexcept{return name == mlc::String("export") || name == mlc::String("import") || name == mlc::String("module") || name == mlc::String("struct") || name == mlc::String("class") || name == mlc::String("template") || name == mlc::String("typename") || name == mlc::String("const") || name == mlc::String("return") || name == mlc::String("if") || name == mlc::String("else") || name == mlc::String("for") || name == mlc::String("while") || name == mlc::String("do") || name == mlc::String("switch") || name == mlc::String("case") || name == mlc::String("default") || name == mlc::String("break") || name == mlc::String("continue") || name == mlc::String("try") || name == mlc::String("catch") || name == mlc::String("throw") || name == mlc::String("new") || name == mlc::String("delete") || name == mlc::String("virtual") || name == mlc::String("override") || name == mlc::String("final") || name == mlc::String("public") || name == mlc::String("private") || name == mlc::String("protected") || name == mlc::String("friend") || name == mlc::String("static") || name == mlc::String("extern") || name == mlc::String("inline") || name == mlc::String("using") || name == mlc::String("namespace") || name == mlc::String("typedef") || name == mlc::String("enum") || name == mlc::String("union") || name == mlc::String("true") || name == mlc::String("false") || name == mlc::String("nullptr") || name == mlc::String("void") || name == mlc::String("bool") || name == mlc::String("int") || name == mlc::String("long") || name == mlc::String("short") || name == mlc::String("char") || name == mlc::String("float") || name == mlc::String("double") || name == mlc::String("signed") || name == mlc::String("unsigned") || name == mlc::String("auto") || name == mlc::String("typeid") || name == mlc::String("sizeof") || name == mlc::String("and") || name == mlc::String("or") || name == mlc::String("not") || name == mlc::String("xor") || name == mlc::String("and_eq") || name == mlc::String("or_eq") || name == mlc::String("not_eq") || name == mlc::String("xor_eq") || name == mlc::String("bitand") || name == mlc::String("bitor") || name == mlc::String("compl") || name == mlc::String("alignas") || name == mlc::String("alignof") || name == mlc::String("asm") || name == mlc::String("concept") || name == mlc::String("consteval") || name == mlc::String("constexpr") || name == mlc::String("constinit") || name == mlc::String("const_cast") || name == mlc::String("decltype") || name == mlc::String("dynamic_cast") || name == mlc::String("explicit") || name == mlc::String("mutable") || name == mlc::String("noexcept") || name == mlc::String("operator") || name == mlc::String("reinterpret_cast") || name == mlc::String("requires") || name == mlc::String("static_cast") || name == mlc::String("this") || name == mlc::String("thread_local") || name == mlc::String("volatile") || name == mlc::String("wchar_t");}
mlc::String cpp_safe(mlc::String name) noexcept{return cpp_keyword(name) ? name + mlc::String("_") : name;}
mlc::String lower_first(mlc::String name) noexcept{return name.length() == 0 ? name : name.char_at(0).to_lower() + name.substring(1, name.length());}
mlc::String type_to_cpp(std::shared_ptr<TypeExpr> type_expr) noexcept{return std::visit(overloaded{
  [&](const TyI32& tyi32) { return mlc::String("int"); },
  [&](const TyString& tystring) { return mlc::String("mlc::String"); },
  [&](const TyBool& tybool) { return mlc::String("bool"); },
  [&](const TyUnit& tyunit) { return mlc::String("void"); },
  [&](const TyNamed& tynamed) { auto [name] = tynamed; return cpp_safe(name); },
  [&](const TyArray& tyarray) { auto [inner] = tyarray; return mlc::String("mlc::Array<") + type_to_cpp(inner) + mlc::String(">"); },
  [&](const TyShared& tyshared) { auto [inner] = tyshared; return mlc::String("std::shared_ptr<") + type_to_cpp(inner) + mlc::String(">"); },
  [&](const TyGeneric& tygeneric) { auto [name, args] = tygeneric; return [&]() -> mlc::String { 
  mlc::String safe = cpp_safe(name);
  return args.size() == 0 ? safe : args.size() == 1 ? safe + mlc::String("<") + type_to_cpp(args[0]) + mlc::String(">") : safe + mlc::String("<") + type_to_cpp(args[0]) + mlc::String(", ") + type_to_cpp(args[1]) + mlc::String(">");
 }(); },
  [&](const TyFn& tyfn) { auto [params, ret] = tyfn; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> param_types = {};
  int i = 0;
  while (i < params.size()){
{
param_types.push_back(type_to_cpp(params[i]));
i = i + 1;
}
}
  return mlc::String("std::function<") + type_to_cpp(ret) + mlc::String("(") + param_types.join(mlc::String(", ")) + mlc::String(")>");
 }(); }
}, (*type_expr));}
mlc::String escape_str(mlc::String input) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < input.length()){
{
mlc::String c = input.char_at(i);
if (c == mlc::String("\\")){
{
parts.push_back(mlc::String("\\\\"));
}
} else {
{
if (c == mlc::String("\"")){
parts.push_back(mlc::String("\\\""));
} else {
if (c == mlc::String("\n")){
parts.push_back(mlc::String("\\n"));
} else {
if (c == mlc::String("\r")){
parts.push_back(mlc::String("\\r"));
} else {
if (c == mlc::String("\t")){
parts.push_back(mlc::String("\\t"));
} else {
if (c == mlc::String("\0")){
parts.push_back(mlc::String("\\0"));
} else {
parts.push_back(c);
}
}
}
}
}
}
}
i = i + 1;
}
}
return parts.join(mlc::String(""));
}
mlc::String gen_args(mlc::Array<std::shared_ptr<Expr>> args, CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < args.size()){
{
parts.push_back(gen_expr(args[i], ctx));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}
mlc::String map_method(mlc::String method_name) noexcept{return method_name == mlc::String("length") || method_name == mlc::String("len") ? mlc::String("length") : method_name == mlc::String("push") ? mlc::String("push_back") : method_name == mlc::String("to_string") ? mlc::String("to_string") : method_name == mlc::String("to_int") || method_name == mlc::String("to_i") ? mlc::String("to_i") : method_name == mlc::String("upper") || method_name == mlc::String("to_upper") ? mlc::String("upper") : method_name == mlc::String("lower") || method_name == mlc::String("to_lower") ? mlc::String("lower") : method_name;}
mlc::String map_builtin(mlc::String name) noexcept{return name == mlc::String("print") ? mlc::String("mlc::io::print") : name == mlc::String("println") ? mlc::String("mlc::io::println") : name == mlc::String("exit") ? mlc::String("mlc::io::exit") : name == mlc::String("args") ? mlc::String("mlc::io::args") : name;}
mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<Stmt>> stmts, CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < stmts.size()){
{
parts.push_back(gen_stmt(stmts[i], ctx));
i = i + 1;
}
}
return parts.join(mlc::String(""));
}
mlc::String gen_block_body(std::shared_ptr<Expr> expr, CodegenContext ctx) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ExprBlock>((*expr))) { auto _v_exprblock = std::get<ExprBlock>((*expr)); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = gen_stmts_str(stmts, ctx);
  mlc::String result_code = gen_expr(result, ctx);
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + result_code + mlc::String(";\n");
 }(); } if (std::holds_alternative<ExprIf>((*expr))) { auto _v_exprif = std::get<ExprIf>((*expr)); auto [_w0, _w1, _w2] = _v_exprif; return gen_stmt_expr(expr, ctx); } return gen_expr(expr, ctx) + mlc::String(";\n"); }();}
mlc::String gen_expr(std::shared_ptr<Expr> expr, CodegenContext ctx) noexcept{return std::visit(overloaded{
  [&](const ExprInt& exprint) { auto [n] = exprint; return mlc::to_string(n); },
  [&](const ExprStr& exprstr) { auto [s] = exprstr; return mlc::String("mlc::String(\"") + escape_str(s) + mlc::String("\")"); },
  [&](const ExprBool& exprbool) { auto [b] = exprbool; return b ? mlc::String("true") : mlc::String("false"); },
  [&](const ExprUnit& exprunit) { return mlc::String("/* unit */"); },
  [&](const ExprIdent& exprident) { auto [name] = exprident; return [&]() -> mlc::String { 
  mlc::String mapped = map_builtin(name);
  mlc::String safe = cpp_safe(mapped);
  mlc::String first = safe.char_at(0);
  return first >= mlc::String("A") && first <= mlc::String("Z") ? safe + mlc::String("{}") : safe;
 }(); },
  [&](const ExprBin& exprbin) { auto [op, left_expr, right_expr] = exprbin; return mlc::String("(") + gen_expr(left_expr, ctx) + mlc::String(" ") + op + mlc::String(" ") + gen_expr(right_expr, ctx) + mlc::String(")"); },
  [&](const ExprUn& exprun) { auto [op, inner] = exprun; return mlc::String("(") + op + gen_expr(inner, ctx) + mlc::String(")"); },
  [&](const ExprCall& exprcall) { auto [func, args] = exprcall; return [&]() -> mlc::String { 
  mlc::String fn_code = [&]() -> mlc::String { if (std::holds_alternative<ExprIdent>((*func))) { auto _v_exprident = std::get<ExprIdent>((*func)); auto [name] = _v_exprident; return cpp_safe(map_builtin(name)); } return gen_expr(func, ctx); }();
  return fn_code + mlc::String("(") + gen_args(args, ctx) + mlc::String(")");
 }(); },
  [&](const ExprMethod& exprmethod) { auto [obj, method_name, args] = exprmethod; return [&]() -> mlc::String { 
  mlc::String obj_code = [&]() -> mlc::String { if (std::holds_alternative<ExprIdent>((*obj))) { auto _v_exprident = std::get<ExprIdent>((*obj)); auto [name] = _v_exprident; return cpp_safe(map_builtin(name)); } return gen_expr(obj, ctx); }();
  return obj_code == mlc::String("File") ? [&]() -> mlc::String { 
  mlc::String cpp_fn = method_name == mlc::String("read") ? mlc::String("mlc::file::read_to_string") : mlc::String("mlc::file::") + method_name;
  return cpp_fn + mlc::String("(") + gen_args(args, ctx) + mlc::String(")");
 }() : [&]() -> mlc::String { 
  mlc::String base = obj_code + mlc::String(".") + map_method(method_name) + mlc::String("(");
  return args.size() == 0 ? base + mlc::String(")") : base + gen_args(args, ctx) + mlc::String(")");
 }();
 }(); },
  [&](const ExprField& exprfield) { auto [obj, field_name] = exprfield; return gen_expr(obj, ctx) + mlc::String(".") + cpp_safe(field_name); },
  [&](const ExprIndex& exprindex) { auto [obj, idx] = exprindex; return gen_expr(obj, ctx) + mlc::String("[") + gen_expr(idx, ctx) + mlc::String("]"); },
  [&](const ExprIf& exprif) { auto [cond, then_expr, else_expr] = exprif; return mlc::String("(") + gen_expr(cond, ctx) + mlc::String(" ? ") + gen_expr(then_expr, ctx) + mlc::String(" : ") + gen_expr(else_expr, ctx) + mlc::String(")"); },
  [&](const ExprBlock& exprblock) { auto [stmts, result] = exprblock; return stmts.size() == 0 ? gen_expr(result, ctx) : [&]() -> mlc::String { 
  mlc::String body = gen_stmts_str(stmts, ctx) + mlc::String("return ") + gen_expr(result, ctx) + mlc::String(";\n");
  return mlc::String("[&]() {\n") + body + mlc::String("}()");
 }(); },
  [&](const ExprWhile& exprwhile) { auto [cond, stmts] = exprwhile; return mlc::String("[&]() {\nwhile (") + gen_expr(cond, ctx) + mlc::String(") {\n") + gen_stmts_str(stmts, ctx) + mlc::String("}\n}()"); },
  [&](const ExprFor& exprfor) { auto [var, iter, stmts] = exprfor; return mlc::String("[&]() {\nfor (auto ") + cpp_safe(var) + mlc::String(" : ") + gen_expr(iter, ctx) + mlc::String(") {\n") + gen_stmts_str(stmts, ctx) + mlc::String("}\n}()"); },
  [&](const ExprMatch& exprmatch) { auto [subject, arms] = exprmatch; return gen_match(subject, arms, ctx); },
  [&](const ExprRecord& exprrecord) { auto [name, field_vals] = exprrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> field_order = lookup_fields(ctx.field_orders, name);
  mlc::String vals = field_order.size() > 0 ? gen_record_ordered(field_vals, field_order, ctx) : gen_record_unordered(field_vals, ctx);
  return cpp_safe(name) + mlc::String("{") + vals + mlc::String("}");
 }(); },
  [&](const ExprRecordUpdate& exprrecordupdate) { auto [name, base, overrides] = exprrecordupdate; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> field_order = lookup_fields(ctx.field_orders, name);
  mlc::String vals = gen_record_update_ordered(base, overrides, field_order, ctx);
  return cpp_safe(name) + mlc::String("{") + vals + mlc::String("}");
 }(); },
  [&](const ExprArray& exprarray) { auto [exprs] = exprarray; return mlc::String("mlc::Array<auto>{") + gen_args(exprs, ctx) + mlc::String("}"); },
  [&](const ExprQuestion& exprquestion) { auto [inner] = exprquestion; return gen_expr(inner, ctx); },
  [&](const ExprLambda& exprlambda) { auto [params, body] = exprlambda; return [&]() -> mlc::String { 
  mlc::String capture = params.size() == 0 ? mlc::String("[]") : mlc::String("[=]");
  mlc::String param_list = params.size() == 0 ? mlc::String("") : [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < params.size()){
{
parts.push_back(mlc::String("auto ") + cpp_safe(params[i]));
i = i + 1;
}
}
  return parts.join(mlc::String(", "));
 }();
  mlc::String body_code = gen_expr(body, ctx);
  return capture + mlc::String("(") + param_list + mlc::String(") { return ") + body_code + mlc::String("; }");
 }(); }
}, (*expr));}
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::Array<mlc::String> field_order, CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < field_order.size()){
{
std::shared_ptr<Expr> value = find_field_val(field_vals, field_order[i]);
parts.push_back(gen_expr(value, ctx));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<FieldVal>> field_vals, CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < field_vals.size()){
{
parts.push_back(gen_expr(field_vals[i]->val, ctx));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}
mlc::String gen_record_update_ordered(std::shared_ptr<Expr> base, mlc::Array<std::shared_ptr<FieldVal>> overrides, mlc::Array<mlc::String> field_order, CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < field_order.size()){
{
mlc::String field_name = field_order[i];
std::shared_ptr<Expr> override_val = find_field_val(overrides, field_name);
mlc::String val_str = [&]() -> mlc::String { if (std::holds_alternative<ExprUnit>((*override_val))) {  return gen_expr(std::make_shared<Expr>(ExprField(base, field_name)), ctx); } return gen_expr(override_val, ctx); }();
parts.push_back(val_str);
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}
std::shared_ptr<Expr> find_field_val(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::String name) noexcept{
std::shared_ptr<Expr> result = std::make_shared<Expr>(ExprUnit{});
int i = 0;
while (i < field_vals.size()){
{
if (field_vals[i]->name == name){
{
result = field_vals[i]->val;
}
}
i = i + 1;
}
}
return result;
}
mlc::String gen_match(std::shared_ptr<Expr> subject, mlc::Array<std::shared_ptr<MatchArm>> arms, CodegenContext ctx) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < arms.size()){
{
parts.push_back(gen_arm(arms[i], ctx));
i = i + 1;
}
}
return mlc::String("std::visit(overloaded{") + parts.join(mlc::String(",\n")) + mlc::String("\n}, ") + gen_expr(subject, ctx) + mlc::String(")");
}
mlc::String gen_arm(std::shared_ptr<MatchArm> arm, CodegenContext ctx) noexcept{return gen_pat_header(arm->pat) + mlc::String("{ return ") + gen_expr(arm->body, ctx) + mlc::String("; }");}
mlc::String gen_pat_header(std::shared_ptr<Pat> pat) noexcept{return std::visit(overloaded{
  [&](const PatWild& patwild) { return mlc::String("[&](auto& __v) "); },
  [&](const PatUnit& patunit) { return mlc::String("[&](auto& __v) "); },
  [&](const PatBool& patbool) { auto [_w0] = patbool; return mlc::String("[&](auto& __v) "); },
  [&](const PatInt& patint) { auto [_w0] = patint; return mlc::String("[&](auto& __v) "); },
  [&](const PatStr& patstr) { auto [_w0] = patstr; return mlc::String("[&](auto& __v) "); },
  [&](const PatIdent& patident) { auto [name] = patident; return mlc::String("[&](auto ") + cpp_safe(name) + mlc::String(") "); },
  [&](const PatCtor& patctor) { auto [name, sub_pats] = patctor; return [&]() -> mlc::String { 
  mlc::String lower_name = cpp_safe(lower_first(name));
  mlc::String binding = sub_pats.size() == 0 ? mlc::String("") : mlc::String("{ auto [") + pat_bind_names(sub_pats) + mlc::String("] = ") + lower_name + mlc::String("; } ");
  return mlc::String("[&](const ") + cpp_safe(name) + mlc::String("& ") + lower_name + mlc::String(") ") + binding;
 }(); }
}, (*pat));}
mlc::String pat_bind_names(mlc::Array<std::shared_ptr<Pat>> sub_pats) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < sub_pats.size()){
{
mlc::String bind_name = [&]() -> mlc::String { if (std::holds_alternative<PatIdent>((*sub_pats[i]))) { auto _v_patident = std::get<PatIdent>((*sub_pats[i])); auto [name] = _v_patident; return cpp_safe(name); } return mlc::String("__") + mlc::to_string(i); }();
parts.push_back(bind_name);
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}
mlc::String gen_stmt(std::shared_ptr<Stmt> stmt, CodegenContext ctx) noexcept{return std::visit(overloaded{
  [&](const StmtLet& stmtlet) { auto [name, _w0, typ, value] = stmtlet; return [&]() -> mlc::String { 
  mlc::String val_code = [&]() -> mlc::String { if (std::holds_alternative<ExprArray>((*value))) { auto _v_exprarray = std::get<ExprArray>((*value)); auto [elems] = _v_exprarray; return elems.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<TyArray>((*typ))) { auto _v_tyarray = std::get<TyArray>((*typ)); auto [inner] = _v_tyarray; return mlc::String("mlc::Array<") + type_to_cpp(inner) + mlc::String(">{}"); } return mlc::String("mlc::Array<auto>{}"); }() : mlc::String("mlc::Array<auto>{") + gen_args(elems, ctx) + mlc::String("}"); } return gen_expr(value, ctx); }();
  return mlc::String("auto ") + cpp_safe(name) + mlc::String(" = ") + val_code + mlc::String(";\n");
 }(); },
  [&](const StmtExpr& stmtexpr) { auto [expr] = stmtexpr; return gen_stmt_expr(expr, ctx); },
  [&](const StmtBreak& stmtbreak) { return mlc::String("break;\n"); },
  [&](const StmtContinue& stmtcontinue) { return mlc::String("continue;\n"); },
  [&](const StmtReturn& stmtreturn) { auto [expr] = stmtreturn; return mlc::String("return ") + gen_expr(expr, ctx) + mlc::String(";\n"); }
}, (*stmt));}
mlc::String gen_stmt_expr(std::shared_ptr<Expr> expr, CodegenContext ctx) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ExprBin>((*expr))) { auto _v_exprbin = std::get<ExprBin>((*expr)); auto [op, left_expr, right_expr] = _v_exprbin; return op == mlc::String("=") ? gen_expr(left_expr, ctx) + mlc::String(" = ") + gen_expr(right_expr, ctx) + mlc::String(";\n") : mlc::String("(") + gen_expr(left_expr, ctx) + mlc::String(" ") + op + mlc::String(" ") + gen_expr(right_expr, ctx) + mlc::String(");\n"); } if (std::holds_alternative<ExprIf>((*expr))) { auto _v_exprif = std::get<ExprIf>((*expr)); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(cond, ctx) + mlc::String(") {\n") + gen_block_body(then_expr, ctx) + mlc::String("}");
  mlc::String else_str = [&]() -> mlc::String { if (std::holds_alternative<ExprUnit>((*else_expr))) {  return mlc::String(""); } if (std::holds_alternative<ExprBlock>((*else_expr))) { auto _v_exprblock = std::get<ExprBlock>((*else_expr)); auto [stmts, result] = _v_exprblock; return stmts.size() == 0 ? [&]() -> mlc::String { if (std::holds_alternative<ExprUnit>((*result))) {  return mlc::String(""); } return mlc::String(" else {\n") + gen_block_body(else_expr, ctx) + mlc::String("}"); }() : mlc::String(" else {\n") + gen_block_body(else_expr, ctx) + mlc::String("}"); } if (std::holds_alternative<ExprIf>((*else_expr))) { auto _v_exprif = std::get<ExprIf>((*else_expr)); auto [_w0, _w1, _w2] = _v_exprif; return mlc::String(" else ") + gen_stmt_expr(else_expr, ctx); } return mlc::String(" else {\n") + gen_block_body(else_expr, ctx) + mlc::String("}"); }();
  if (else_str.length() > 0){
{
output = output + else_str;
}
}
  return output + mlc::String("\n");
 }(); } if (std::holds_alternative<ExprWhile>((*expr))) { auto _v_exprwhile = std::get<ExprWhile>((*expr)); auto [cond, stmts] = _v_exprwhile; return mlc::String("while (") + gen_expr(cond, ctx) + mlc::String(") {\n") + gen_stmts_str(stmts, ctx) + mlc::String("}\n"); } if (std::holds_alternative<ExprFor>((*expr))) { auto _v_exprfor = std::get<ExprFor>((*expr)); auto [var, iter, stmts] = _v_exprfor; return mlc::String("for (auto ") + cpp_safe(var) + mlc::String(" : ") + gen_expr(iter, ctx) + mlc::String(") {\n") + gen_stmts_str(stmts, ctx) + mlc::String("}\n"); } if (std::holds_alternative<ExprBlock>((*expr))) { auto _v_exprblock = std::get<ExprBlock>((*expr)); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = gen_stmts_str(stmts, ctx);
  mlc::String result_code = gen_expr(result, ctx);
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + result_code + mlc::String(";\n");
 }(); } return gen_expr(expr, ctx) + mlc::String(";\n"); }();}
mlc::String gen_return_body(std::shared_ptr<Expr> expr, CodegenContext ctx) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ExprBlock>((*expr))) { auto _v_exprblock = std::get<ExprBlock>((*expr)); auto [stmts, result] = _v_exprblock; return [&]() -> mlc::String { 
  mlc::String stmts_code = gen_stmts_str(stmts, ctx);
  mlc::String result_code = gen_expr(result, ctx);
  return result_code == mlc::String("/* unit */") ? stmts_code : stmts_code + mlc::String("return ") + result_code + mlc::String(";\n");
 }(); } if (std::holds_alternative<ExprIf>((*expr))) { auto _v_exprif = std::get<ExprIf>((*expr)); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(cond, ctx) + mlc::String(") {\n") + gen_return_body(then_expr, ctx) + mlc::String("}");
  mlc::String else_str = [&]() -> mlc::String { if (std::holds_alternative<ExprUnit>((*else_expr))) {  return mlc::String(""); } if (std::holds_alternative<ExprIf>((*else_expr))) { auto _v_exprif = std::get<ExprIf>((*else_expr)); auto [_w0, _w1, _w2] = _v_exprif; return mlc::String(" else ") + gen_return_if_stmt(else_expr, ctx); } return mlc::String(" else {\n") + gen_return_body(else_expr, ctx) + mlc::String("}"); }();
  if (else_str.length() > 0){
{
output = output + else_str;
}
}
  return output + mlc::String("\n");
 }(); } return mlc::String("return ") + gen_expr(expr, ctx) + mlc::String(";\n"); }();}
mlc::String gen_return_if_stmt(std::shared_ptr<Expr> expr, CodegenContext ctx) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<ExprIf>((*expr))) { auto _v_exprif = std::get<ExprIf>((*expr)); auto [cond, then_expr, else_expr] = _v_exprif; return [&]() -> mlc::String { 
  mlc::String output = mlc::String("if (") + gen_expr(cond, ctx) + mlc::String(") {\n") + gen_return_body(then_expr, ctx) + mlc::String("}");
  mlc::String else_str = [&]() -> mlc::String { if (std::holds_alternative<ExprUnit>((*else_expr))) {  return mlc::String(""); } if (std::holds_alternative<ExprIf>((*else_expr))) { auto _v_exprif = std::get<ExprIf>((*else_expr)); auto [_w0, _w1, _w2] = _v_exprif; return mlc::String(" else ") + gen_return_if_stmt(else_expr, ctx); } return mlc::String(" else {\n") + gen_return_body(else_expr, ctx) + mlc::String("}"); }();
  if (else_str.length() > 0){
{
output = output + else_str;
}
}
  return output + mlc::String("\n");
 }(); } return mlc::String("if (true) {\n") + gen_return_body(expr, ctx) + mlc::String("}\n"); }();}
mlc::String gen_fn_body(std::shared_ptr<Expr> body, CodegenContext ctx) noexcept{return gen_return_body(body, ctx);}
mlc::String gen_params(mlc::Array<std::shared_ptr<Param>> params) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < params.size()){
{
parts.push_back(type_to_cpp(params[i]->typ) + mlc::String(" ") + cpp_safe(params[i]->name));
i = i + 1;
}
}
return parts.join(mlc::String(", "));
}
mlc::String gen_type_decl_fwd(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept{return variants.size() == 1 ? mlc::String("") : gen_adt_fwd(type_name, variants);}
mlc::String gen_type_decl_body(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept{return variants.size() == 1 ? gen_single_variant(type_name, variants[0]) : gen_adt_defs(type_name, variants);}
mlc::String gen_single_variant(mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarRecord& varrecord) { auto [_w0, field_defs] = varrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_defs.size()){
{
parts.push_back(type_to_cpp(field_defs[i]->typ) + mlc::String(" ") + cpp_safe(field_defs[i]->name) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + cpp_safe(type_name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); },
  [&](const VarTuple& vartuple) { auto [_w0, field_types] = vartuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_types.size()){
{
parts.push_back(type_to_cpp(field_types[i]) + mlc::String(" field") + mlc::to_string(i) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + cpp_safe(type_name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); },
  [&](const VarUnit& varunit) { auto [_w0] = varunit; return mlc::String("struct ") + cpp_safe(type_name) + mlc::String(" {};\n"); }
}, (*variant));}
mlc::String gen_adt_fwd(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> parts = {};
mlc::Array<mlc::String> variant_names = {};
int index = 0;
while (index < variants.size()){
{
mlc::String variant_name = cpp_safe(variant_ctor_name(variants[index]));
variant_names.push_back(variant_name);
parts.push_back(mlc::String("struct ") + variant_name + mlc::String(";\n"));
index = index + 1;
}
}
return parts.join(mlc::String("")) + mlc::String("using ") + cpp_safe(type_name) + mlc::String(" = std::variant<") + variant_names.join(mlc::String(", ")) + mlc::String(">;\n");
}
mlc::String gen_adt_defs(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept{
mlc::Array<mlc::String> parts = {};
int i = 0;
while (i < variants.size()){
{
parts.push_back(gen_variant_struct(type_name, variants[i]));
i = i + 1;
}
}
return parts.join(mlc::String(""));
}
mlc::String variant_ctor_name(std::shared_ptr<TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [name] = varunit; return name; },
  [&](const VarTuple& vartuple) { auto [name, _w0] = vartuple; return name; },
  [&](const VarRecord& varrecord) { auto [name, _w0] = varrecord; return name; }
}, (*variant));}
mlc::String gen_variant_struct(mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept{return std::visit(overloaded{
  [&](const VarUnit& varunit) { auto [name] = varunit; return mlc::String("struct ") + cpp_safe(name) + mlc::String(" {};\n"); },
  [&](const VarTuple& vartuple) { auto [name, field_types] = vartuple; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_types.size()){
{
parts.push_back(type_to_cpp(field_types[i]) + mlc::String(" field") + mlc::to_string(i) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + cpp_safe(name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); },
  [&](const VarRecord& varrecord) { auto [name, field_defs] = varrecord; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int i = 0;
  while (i < field_defs.size()){
{
parts.push_back(type_to_cpp(field_defs[i]->typ) + mlc::String(" ") + cpp_safe(field_defs[i]->name) + mlc::String(";"));
i = i + 1;
}
}
  return mlc::String("struct ") + cpp_safe(name) + mlc::String(" {") + parts.join(mlc::String("")) + mlc::String("};\n");
 }(); }
}, (*variant));}
mlc::String gen_fn_decl(mlc::String name, mlc::Array<std::shared_ptr<Param>> params, std::shared_ptr<TypeExpr> return_type, std::shared_ptr<Expr> body, CodegenContext ctx) noexcept{
mlc::String safe_name = cpp_safe(name);
return name == mlc::String("main") && params.size() == 0 ? [&]() -> mlc::String { 
  mlc::String preamble = mlc::String("mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));\n");
  return type_to_cpp(return_type) + mlc::String(" ") + safe_name + mlc::String("(int argc, char** argv) noexcept{\n") + preamble + gen_fn_body(body, ctx) + mlc::String("}\n");
 }() : type_to_cpp(return_type) + mlc::String(" ") + safe_name + mlc::String("(") + gen_params(params) + mlc::String(") noexcept{\n") + gen_fn_body(body, ctx) + mlc::String("}\n");
}
mlc::String gen_fn_proto(mlc::String name, mlc::Array<std::shared_ptr<Param>> params, std::shared_ptr<TypeExpr> return_type) noexcept{
mlc::String safe_name = cpp_safe(name);
return name == mlc::String("main") && params.size() == 0 ? type_to_cpp(return_type) + mlc::String(" ") + safe_name + mlc::String("(int argc, char** argv) noexcept;\n") : type_to_cpp(return_type) + mlc::String(" ") + safe_name + mlc::String("(") + gen_params(params) + mlc::String(") noexcept;\n");
}
mlc::String gen_type_decl(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept{return gen_type_decl_fwd(type_name, variants) + gen_type_decl_body(type_name, variants);}
mlc::String gen_decl(std::shared_ptr<Decl> decl, CodegenContext ctx) noexcept{return std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return gen_type_decl(type_name, variants); },
  [&](const DeclFn& declfn) { auto [name, params, return_type, body] = declfn; return gen_fn_decl(name, params, return_type, body, ctx); },
  [&](const DeclExtend& declextend) { auto [_w0, methods] = declextend; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int index = 0;
  while (index < methods.size()){
{
parts.push_back(gen_decl(methods[index], ctx));
index = index + 1;
}
}
  return parts.join(mlc::String(""));
 }(); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) { auto [d] = declexported; return gen_decl(d, ctx); }
}, (*decl));}
mlc::String gen_proto(std::shared_ptr<Decl> decl) noexcept{return std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [_w0, _w1] = decltype_; return mlc::String(""); },
  [&](const DeclFn& declfn) { auto [name, params, ret, _w0] = declfn; return gen_fn_proto(name, params, ret); },
  [&](const DeclExtend& declextend) { auto [_w0, methods] = declextend; return [&]() -> mlc::String { 
  mlc::Array<mlc::String> parts = {};
  int index = 0;
  while (index < methods.size()){
{
parts.push_back(gen_proto(methods[index]));
index = index + 1;
}
}
  return parts.join(mlc::String(""));
 }(); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) { auto [d] = declexported; return gen_proto(d); }
}, (*decl));}
mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<Decl>> decls, CodegenContext ctx, int phase) noexcept{
mlc::Array<mlc::String> parts = {};
int index = 0;
while (index < decls.size()){
{
mlc::String segment = std::visit(overloaded{
  [&](const DeclType& decltype_) { auto [type_name, variants] = decltype_; return phase == 0 ? gen_type_decl_fwd(type_name, variants) : phase == 1 ? gen_type_decl_body(type_name, variants) : mlc::String(""); },
  [&](const DeclFn& declfn) { auto [_w0, _w1, _w2, _w3] = declfn; return phase == 2 ? gen_proto(decls[index]) : phase == 3 ? gen_decl(decls[index], ctx) : mlc::String(""); },
  [&](const DeclExtend& declextend) { auto [_w0, _w1] = declextend; return phase == 2 ? gen_proto(decls[index]) : phase == 3 ? gen_decl(decls[index], ctx) : mlc::String(""); },
  [&](const DeclImport& declimport) { auto [_w0, _w1] = declimport; return mlc::String(""); },
  [&](const DeclExported& declexported) { auto [d] = declexported; return phase == 2 ? gen_proto(d) : phase == 3 ? gen_decl(d, ctx) : mlc::String(""); }
}, (*decls[index]));
if (segment.length() > 0){
{
parts.push_back(segment);
}
}
index = index + 1;
}
}
return parts;
}
mlc::String gen_program(Program prog) noexcept{
CodegenContext ctx = create_codegen_context(prog);
mlc::Array<mlc::String> preamble = mlc::Array<mlc::String>{mlc::String("#include \"mlc/core/string.hpp\"\n"), mlc::String("#include \"mlc/core/array.hpp\"\n"), mlc::String("#include \"mlc/core/hashmap.hpp\"\n"), mlc::String("#include \"mlc/core/match.hpp\"\n"), mlc::String("#include \"mlc/io/io.hpp\"\n"), mlc::String("#include \"mlc/io/file.hpp\"\n"), mlc::String("#include <functional>\n"), mlc::String("#include <memory>\n"), mlc::String("#include <variant>\n"), mlc::String("#include <vector>\n\n")};
mlc::Array<mlc::String> type_fwds = collect_decl_parts(prog.decls, ctx, 0);
mlc::Array<mlc::String> type_defs = collect_decl_parts(prog.decls, ctx, 1);
mlc::Array<mlc::String> fn_protos = collect_decl_parts(prog.decls, ctx, 2);
mlc::Array<mlc::String> fn_decls = collect_decl_parts(prog.decls, ctx, 3);
return errs_append(errs_append(errs_append(errs_append(preamble, type_fwds), type_defs), errs_append(fn_protos, mlc::Array<mlc::String>{mlc::String("\n")})), fn_decls).join(mlc::String(""));
}
mlc::Array<TestResult> codegen_tests() noexcept{
mlc::Array<TestResult> results = {};
Program empty_prog = Program{{}};
CodegenContext ctx = create_codegen_context(empty_prog);
results.push_back(assert_eq_str(mlc::String("create_codegen_context: gen_expr with ctx"), gen_expr(std::make_shared<Expr>(ExprInt(42)), ctx), mlc::String("42")));
results.push_back(assert_eq_str(mlc::String("TyI32 → 'int'"), type_to_cpp(std::make_shared<TypeExpr>(TyI32{})), mlc::String("int")));
results.push_back(assert_eq_str(mlc::String("TyString → 'mlc::String'"), type_to_cpp(std::make_shared<TypeExpr>(TyString{})), mlc::String("mlc::String")));
results.push_back(assert_eq_str(mlc::String("TyBool → 'bool'"), type_to_cpp(std::make_shared<TypeExpr>(TyBool{})), mlc::String("bool")));
results.push_back(assert_eq_str(mlc::String("TyUnit → 'void'"), type_to_cpp(std::make_shared<TypeExpr>(TyUnit{})), mlc::String("void")));
results.push_back(assert_eq_str(mlc::String("TyArray(TyI32) → 'mlc::Array<int>'"), type_to_cpp(std::make_shared<TypeExpr>(TyArray(std::make_shared<TypeExpr>(TyI32{})))), mlc::String("mlc::Array<int>")));
results.push_back(assert_eq_str(mlc::String("TyShared(TyI32) → 'std::shared_ptr<int>'"), type_to_cpp(std::make_shared<TypeExpr>(TyShared(std::make_shared<TypeExpr>(TyI32{})))), mlc::String("std::shared_ptr<int>")));
results.push_back(assert_eq_str(mlc::String("ExprInt(42) → '42'"), gen_expr(std::make_shared<Expr>(ExprInt(42)), ctx), mlc::String("42")));
results.push_back(assert_eq_str(mlc::String("ExprBool(true) → 'true'"), gen_expr(std::make_shared<Expr>(ExprBool(true)), ctx), mlc::String("true")));
results.push_back(assert_eq_str(mlc::String("ExprStr('hi') → 'mlc::String(\"hi\")'"), gen_expr(std::make_shared<Expr>(ExprStr(mlc::String("hi"))), ctx), mlc::String("mlc::String(\"hi\")")));
results.push_back(assert_eq_str(mlc::String("ExprUnit → '/* unit */'"), gen_expr(std::make_shared<Expr>(ExprUnit{}), ctx), mlc::String("/* unit */")));
results.push_back(assert_eq_str(mlc::String("ExprIdent('x') → 'x'"), gen_expr(std::make_shared<Expr>(ExprIdent(mlc::String("x"))), ctx), mlc::String("x")));
results.push_back(assert_eq_str(mlc::String("ExprIdent('print') → 'mlc::io::print'"), gen_expr(std::make_shared<Expr>(ExprIdent(mlc::String("print"))), ctx), mlc::String("mlc::io::print")));
results.push_back(assert_eq_str(mlc::String("ExprIdent('println') → 'mlc::io::println'"), gen_expr(std::make_shared<Expr>(ExprIdent(mlc::String("println"))), ctx), mlc::String("mlc::io::println")));
results.push_back(assert_eq_str(mlc::String("ExprBin add → '(1 + 2)'"), gen_expr(std::make_shared<Expr>(ExprBin(mlc::String("+"), std::make_shared<Expr>(ExprInt(1)), std::make_shared<Expr>(ExprInt(2)))), ctx), mlc::String("(1 + 2)")));
results.push_back(assert_eq_str(mlc::String("ExprUn neg → '(-42)'"), gen_expr(std::make_shared<Expr>(ExprUn(mlc::String("-"), std::make_shared<Expr>(ExprInt(42)))), ctx), mlc::String("(-42)")));
results.push_back(assert_eq_str(mlc::String("map_builtin passthrough → 'foo'"), map_builtin(mlc::String("foo")), mlc::String("foo")));
mlc::Array<mlc::String> single_params = mlc::Array<mlc::String>{mlc::String("x")};
results.push_back(assert_eq_str(mlc::String("ExprLambda single param → '[=](auto x) { return ... }'"), gen_expr(std::make_shared<Expr>(ExprLambda(single_params, std::make_shared<Expr>(ExprBin(mlc::String("+"), std::make_shared<Expr>(ExprIdent(mlc::String("x"))), std::make_shared<Expr>(ExprInt(1)))))), ctx), mlc::String("[=](auto x) { return (x + 1); }")));
mlc::Array<mlc::String> two_params = mlc::Array<mlc::String>{mlc::String("a"), mlc::String("b")};
results.push_back(assert_eq_str(mlc::String("ExprLambda two params → '[=](auto a, auto b) { return ... }'"), gen_expr(std::make_shared<Expr>(ExprLambda(two_params, std::make_shared<Expr>(ExprBin(mlc::String("+"), std::make_shared<Expr>(ExprIdent(mlc::String("a"))), std::make_shared<Expr>(ExprIdent(mlc::String("b"))))))), ctx), mlc::String("[=](auto a, auto b) { return (a + b); }")));
mlc::Array<mlc::String> zero_params = {};
results.push_back(assert_eq_str(mlc::String("ExprLambda zero params → '[]() { return ... }'"), gen_expr(std::make_shared<Expr>(ExprLambda(zero_params, std::make_shared<Expr>(ExprInt(42)))), ctx), mlc::String("[]() { return 42; }")));
results.push_back(assert_eq_str(mlc::String("map_method: length → length"), map_method(mlc::String("length")), mlc::String("length")));
results.push_back(assert_eq_str(mlc::String("map_method: len → length"), map_method(mlc::String("len")), mlc::String("length")));
results.push_back(assert_eq_str(mlc::String("map_method: push → push_back"), map_method(mlc::String("push")), mlc::String("push_back")));
results.push_back(assert_eq_str(mlc::String("map_method: upper → upper"), map_method(mlc::String("upper")), mlc::String("upper")));
results.push_back(assert_eq_str(mlc::String("map_method: to_upper → upper"), map_method(mlc::String("to_upper")), mlc::String("upper")));
results.push_back(assert_eq_str(mlc::String("map_method: to_lower → lower"), map_method(mlc::String("to_lower")), mlc::String("lower")));
results.push_back(assert_eq_str(mlc::String("map_method: trim passthrough"), map_method(mlc::String("trim")), mlc::String("trim")));
results.push_back(assert_eq_str(mlc::String("map_method: split passthrough"), map_method(mlc::String("split")), mlc::String("split")));
results.push_back(assert_eq_str(mlc::String("map_method: chars passthrough"), map_method(mlc::String("chars")), mlc::String("chars")));
results.push_back(assert_eq_str(mlc::String("map_method: lines passthrough"), map_method(mlc::String("lines")), mlc::String("lines")));
return results;
}
mlc::Array<TestResult> pipe_and_record_update_tests() noexcept{
mlc::Array<TestResult> results = {};
Program empty_prog = Program{{}};
CodegenContext ctx = create_codegen_context(empty_prog);
std::shared_ptr<Expr> left_expr = std::make_shared<Expr>(ExprIdent(mlc::String("x")));
std::shared_ptr<Expr> right_fn = std::make_shared<Expr>(ExprIdent(mlc::String("double")));
std::shared_ptr<Expr> plain_pipe = pipe_desugar(left_expr, right_fn);
results.push_back(assert_eq_str(mlc::String("pipe x |> double → double(x)"), gen_expr(plain_pipe, ctx), mlc::String("double(x)")));
mlc::Array<std::shared_ptr<Expr>> right_call_args = mlc::Array<std::shared_ptr<Expr>>{std::make_shared<Expr>(ExprInt(5))};
std::shared_ptr<Expr> right_call = std::make_shared<Expr>(ExprCall(std::make_shared<Expr>(ExprIdent(mlc::String("add"))), right_call_args));
std::shared_ptr<Expr> partial_pipe = pipe_desugar(left_expr, right_call);
results.push_back(assert_eq_str(mlc::String("pipe x |> add(5) → add(x, 5)"), gen_expr(partial_pipe, ctx), mlc::String("add(x, 5)")));
mlc::Array<std::shared_ptr<Expr>> right_multi_args = mlc::Array<std::shared_ptr<Expr>>{std::make_shared<Expr>(ExprInt(3)), std::make_shared<Expr>(ExprInt(4))};
std::shared_ptr<Expr> right_multi_call = std::make_shared<Expr>(ExprCall(std::make_shared<Expr>(ExprIdent(mlc::String("clamp"))), right_multi_args));
std::shared_ptr<Expr> multi_partial_pipe = pipe_desugar(left_expr, right_multi_call);
results.push_back(assert_eq_str(mlc::String("pipe x |> clamp(3, 4) → clamp(x, 3, 4)"), gen_expr(multi_partial_pipe, ctx), mlc::String("clamp(x, 3, 4)")));
std::shared_ptr<Decl> point_decl = std::make_shared<Decl>(DeclType(mlc::String("Point"), mlc::Array<std::shared_ptr<TypeVariant>>{std::make_shared<TypeVariant>(VarRecord(mlc::String("Point"), mlc::Array<std::shared_ptr<FieldDef>>{std::make_shared<FieldDef>(FieldDef{mlc::String("x"), std::make_shared<TypeExpr>(TyI32{})}), std::make_shared<FieldDef>(FieldDef{mlc::String("y"), std::make_shared<TypeExpr>(TyI32{})})}))}));
Program point_prog = Program{mlc::Array<std::shared_ptr<Decl>>{point_decl}};
CodegenContext ctx_point = create_codegen_context(point_prog);
mlc::Array<std::shared_ptr<FieldVal>> override_fields = mlc::Array<std::shared_ptr<FieldVal>>{std::make_shared<FieldVal>(FieldVal{mlc::String("x"), std::make_shared<Expr>(ExprInt(10))})};
std::shared_ptr<Expr> base_expr = std::make_shared<Expr>(ExprIdent(mlc::String("p")));
std::shared_ptr<Expr> update_expr = std::make_shared<Expr>(ExprRecordUpdate(mlc::String("Point"), base_expr, override_fields));
results.push_back(assert_eq_str(mlc::String("Point { ...p, x: 10 } → Point{10, p.y}"), gen_expr(update_expr, ctx_point), mlc::String("Point{10, p.y}")));
mlc::Array<std::shared_ptr<FieldVal>> two_overrides = mlc::Array<std::shared_ptr<FieldVal>>{std::make_shared<FieldVal>(FieldVal{mlc::String("x"), std::make_shared<Expr>(ExprInt(0))}), std::make_shared<FieldVal>(FieldVal{mlc::String("y"), std::make_shared<Expr>(ExprInt(0))})};
std::shared_ptr<Expr> all_overrides_expr = std::make_shared<Expr>(ExprRecordUpdate(mlc::String("Point"), base_expr, two_overrides));
results.push_back(assert_eq_str(mlc::String("Point { ...p, x: 0, y: 0 } → Point{0, 0}"), gen_expr(all_overrides_expr, ctx_point), mlc::String("Point{0, 0}")));
LexOut spread_tokens = tokenize(mlc::String("..."));
results.push_back(assert_eq_int(mlc::String("'...' → 2 tokens (Spread + Eof)"), spread_tokens.tokens.size(), 2));
int first_token_is_spread = [&]() { if (std::holds_alternative<Spread>(spread_tokens.tokens[0].kind)) {  return 1; } return 0; }();
results.push_back(assert_eq_int(mlc::String("first token of '...' is Spread"), first_token_is_spread, 1));
return results;
}
CppTKind cpp_keyword_kind(mlc::String word) noexcept{return word == mlc::String("auto") ? CppTKind(CKAuto{}) : CppTKind(word == mlc::String("const") ? CppTKind(CKConst{}) : CppTKind(word == mlc::String("constexpr") ? CppTKind(CKConstexpr{}) : CppTKind(word == mlc::String("if") ? CppTKind(CKIf{}) : CppTKind(word == mlc::String("else") ? CppTKind(CKElse{}) : CppTKind(word == mlc::String("while") ? CppTKind(CKWhile{}) : CppTKind(word == mlc::String("for") ? CppTKind(CKFor{}) : CppTKind(word == mlc::String("return") ? CppTKind(CKReturn{}) : CppTKind(word == mlc::String("break") ? CppTKind(CKBreak{}) : CppTKind(word == mlc::String("struct") ? CppTKind(CKStruct{}) : CppTKind(word == mlc::String("using") ? CppTKind(CKUsing{}) : CppTKind(word == mlc::String("noexcept") ? CppTKind(CKNoexcept{}) : CppTKind(word == mlc::String("inline") ? CppTKind(CKInline{}) : CppTKind(word == mlc::String("static") ? CppTKind(CKStatic{}) : CppTKind(word == mlc::String("void") ? CppTKind(CKVoid{}) : CppTKind(word == mlc::String("int") ? CppTKind(CKInt{}) : CppTKind(word == mlc::String("bool") ? CppTKind(CKBool{}) : CppTKind(word == mlc::String("char") ? CppTKind(CKChar{}) : CppTKind(word == mlc::String("true") ? CppTKind(CKTrue{}) : CppTKind(word == mlc::String("false") ? CppTKind(CKFalse{}) : CppTKind(word == mlc::String("nullptr") ? CppTKind(CKNullptr{}) : CppTKind(word == mlc::String("template") ? CppTKind(CKTemplate{}) : CppTKind(word == mlc::String("typename") ? CppTKind(CKTypename{}) : CppTKind(word == mlc::String("namespace") ? CppTKind(CKNamespace{}) : CppTKind(word == mlc::String("new") ? CppTKind(CKNew{}) : CppTKind(word == mlc::String("delete") ? CppTKind(CKDelete{}) : CppTKind(word == mlc::String("operator") ? CppTKind(CKOperator{}) : CppTKind(CIdent(word))))))))))))))))))))))))))));}
CppScanIdR cpp_scan_ident(LexState state) noexcept{
int start = state.pos;
mlc::String source = state.src;
int line = state.line;
int column = state.col;
LexState current = std::move(state);
while (!LexState_eof(current) && is_alnum(LexState_current(current))){
{
current = LexState_advance(current);
}
}
mlc::String word = source.substring(start, current.pos - start);
return CppScanIdR{current, CppToken{cpp_keyword_kind(word), line, column}};
}
CppScanIntR cpp_scan_int(LexState state) noexcept{
int line = state.line;
int column = state.col;
LexState current = std::move(state);
int value = 0;
while (!LexState_eof(current) && is_digit(LexState_current(current))){
{
value = value * 10 + LexState_current(current).to_i();
current = LexState_advance(current);
}
}
return CppScanIntR{current, CppToken{CLInt(value), line, column}};
}
CppScanStrR cpp_scan_string(LexState state) noexcept{
int line = state.line;
int column = state.col;
LexState current = LexState_advance(state);
mlc::Array<mlc::String> parts = {};
mlc::String error = mlc::String("");
while (!LexState_eof(current) && LexState_current(current) != mlc::String("\"")){
{
if (LexState_current(current) == mlc::String("\\")){
{
current = LexState_advance(current);
mlc::String mapped = map_escape(LexState_current(current));
if (mapped != mlc::String("")){
{
parts.push_back(mapped);
}
} else {
{
parts.push_back(mlc::String("\\"));
parts.push_back(LexState_current(current));
}
}
current = LexState_advance(current);
}
} else {
{
parts.push_back(LexState_current(current));
current = LexState_advance(current);
}
}
}
}
if (LexState_eof(current)){
{
error = mlc::String("unterminated string");
}
} else {
{
current = LexState_advance(current);
}
}
return CppScanStrR{current, CppToken{CLStr(parts.join(mlc::String(""))), line, column}, error};
}
CppScanOpR cpp_scan_op(LexState state) noexcept{
int line = state.line;
int column = state.col;
mlc::String character = LexState_current(state);
mlc::String next_char = LexState_peek(state, 1);
return character == mlc::String("-") && next_char == mlc::String(">") ? CppScanOpR{LexState_advance_by(state, 2), CppToken{CArrow{}, line, column}} : character == mlc::String(":") && next_char == mlc::String(":") ? CppScanOpR{LexState_advance_by(state, 2), CppToken{CScope{}, line, column}} : character == mlc::String("=") && next_char == mlc::String("=") ? CppScanOpR{LexState_advance_by(state, 2), CppToken{CEq{}, line, column}} : character == mlc::String("!") && next_char == mlc::String("=") ? CppScanOpR{LexState_advance_by(state, 2), CppToken{CNe{}, line, column}} : character == mlc::String("<") && next_char == mlc::String("=") ? CppScanOpR{LexState_advance_by(state, 2), CppToken{CLe{}, line, column}} : character == mlc::String(">") && next_char == mlc::String("=") ? CppScanOpR{LexState_advance_by(state, 2), CppToken{CGe{}, line, column}} : character == mlc::String("&") && next_char == mlc::String("&") ? CppScanOpR{LexState_advance_by(state, 2), CppToken{CAnd{}, line, column}} : character == mlc::String("|") && next_char == mlc::String("|") ? CppScanOpR{LexState_advance_by(state, 2), CppToken{COr{}, line, column}} : character == mlc::String("+") && next_char == mlc::String("+") ? CppScanOpR{LexState_advance_by(state, 2), CppToken{CInc{}, line, column}} : character == mlc::String("-") && next_char == mlc::String("-") ? CppScanOpR{LexState_advance_by(state, 2), CppToken{CDec{}, line, column}} : character == mlc::String("<") && next_char == mlc::String("<") ? CppScanOpR{LexState_advance_by(state, 2), CppToken{CLShift{}, line, column}} : character == mlc::String(">") && next_char == mlc::String(">") ? CppScanOpR{LexState_advance_by(state, 2), CppToken{CRShift{}, line, column}} : character == mlc::String("(") ? CppScanOpR{LexState_advance(state), CppToken{CLParen{}, line, column}} : character == mlc::String(")") ? CppScanOpR{LexState_advance(state), CppToken{CRParen{}, line, column}} : character == mlc::String("{") ? CppScanOpR{LexState_advance(state), CppToken{CLBrace{}, line, column}} : character == mlc::String("}") ? CppScanOpR{LexState_advance(state), CppToken{CRBrace{}, line, column}} : character == mlc::String("[") ? CppScanOpR{LexState_advance(state), CppToken{CLBracket{}, line, column}} : character == mlc::String("]") ? CppScanOpR{LexState_advance(state), CppToken{CRBracket{}, line, column}} : character == mlc::String("<") ? CppScanOpR{LexState_advance(state), CppToken{CLAngle{}, line, column}} : character == mlc::String(">") ? CppScanOpR{LexState_advance(state), CppToken{CRAngle{}, line, column}} : CppScanOpR{LexState_advance(state), CppToken{COp(character), line, column}};
}
CppLexOut cpp_tokenize(mlc::String source) noexcept{
LexState state = LexState{source, 0, 1, 1};
mlc::Array<CppToken> tokens = {};
mlc::Array<mlc::String> errors = {};
while (!LexState_eof(state)){
{
state = skip_whitespace(state);
if (LexState_eof(state)){
{
break;
}
}
mlc::String character = LexState_current(state);
if (is_alpha(character)){
{
CppScanIdR result = cpp_scan_ident(state);
tokens.push_back(result.tok);
state = result.st;
}
} else {
{
if (is_digit(character)){
CppScanIntR result = cpp_scan_int(state);
tokens.push_back(result.tok);
state = result.st;
} else {
if (character == mlc::String("\"")){
CppScanStrR result = cpp_scan_string(state);
tokens.push_back(result.tok);
state = result.st;
if (result.err != mlc::String("")){
errors.push_back(result.err);
}
} else {
CppScanOpR result = cpp_scan_op(state);
tokens.push_back(result.tok);
state = result.st;
}
}
}
}
}
}
tokens.push_back(CppToken{CEof{}, state.line, state.col});
return CppLexOut{tokens, errors};
}
constexpr int cpp_tok_count(mlc::String src) noexcept{return cpp_tokenize(src).tokens.size();}
CppTKind cpp_kind_at(mlc::String src, int i) noexcept{return cpp_tokenize(src).tokens[i].kind;}
CppTKind cpp_first_kind(mlc::String src) noexcept{return cpp_kind_at(src, 0);}
mlc::String cpp_ident_at(mlc::String src, int i) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<CIdent>(cpp_kind_at(src, i))) { auto _v_cident = std::get<CIdent>(cpp_kind_at(src, i)); auto [s] = _v_cident; return s; } return mlc::String(""); }();}
mlc::String cpp_str_at(mlc::String src, int i) noexcept{return [&]() -> mlc::String { if (std::holds_alternative<CLStr>(cpp_kind_at(src, i))) { auto _v_clstr = std::get<CLStr>(cpp_kind_at(src, i)); auto [s] = _v_clstr; return s; } return mlc::String(""); }();}
int cpp_int_at(mlc::String src, int i) noexcept{return [&]() { if (std::holds_alternative<CLInt>(cpp_kind_at(src, i))) { auto _v_clint = std::get<CLInt>(cpp_kind_at(src, i)); auto [n] = _v_clint; return n; } return -1; }();}
mlc::Array<TestResult> cpp_lexer_tests() noexcept{
mlc::Array<TestResult> results = {};
results.push_back(assert_eq_int(mlc::String("empty → 1 token (CEof)"), cpp_tok_count(mlc::String("")), 1));
results.push_back(assert_eq_int(mlc::String("'int x' → 3 tokens"), cpp_tok_count(mlc::String("int x")), 3));
results.push_back(assert_eq_int(mlc::String("'::'  → 2 tokens"), cpp_tok_count(mlc::String("::")), 2));
results.push_back(assert_eq_int(mlc::String("'->'  → 2 tokens"), cpp_tok_count(mlc::String("->")), 2));
results.push_back(assert_eq_int(mlc::String("'// comment\\nint' → 2 tokens"), cpp_tok_count(mlc::String("// comment\nint")), 2));
results.push_back(assert_eq_int(mlc::String("'/* block */int' → 2 tokens"), cpp_tok_count(mlc::String("/* block */int")), 2));
results.push_back(assert_eq_int(mlc::String("'#include' → 3 tokens"), cpp_tok_count(mlc::String("#include")), 3));
results.push_back(assert_true(mlc::String("'auto'       → CKAuto"), [&]() -> bool { if (std::holds_alternative<CKAuto>(cpp_first_kind(mlc::String("auto")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'const'      → CKConst"), [&]() -> bool { if (std::holds_alternative<CKConst>(cpp_first_kind(mlc::String("const")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'constexpr'  → CKConstexpr"), [&]() -> bool { if (std::holds_alternative<CKConstexpr>(cpp_first_kind(mlc::String("constexpr")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'int'        → CKInt"), [&]() -> bool { if (std::holds_alternative<CKInt>(cpp_first_kind(mlc::String("int")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'void'       → CKVoid"), [&]() -> bool { if (std::holds_alternative<CKVoid>(cpp_first_kind(mlc::String("void")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'bool'       → CKBool"), [&]() -> bool { if (std::holds_alternative<CKBool>(cpp_first_kind(mlc::String("bool")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'return'     → CKReturn"), [&]() -> bool { if (std::holds_alternative<CKReturn>(cpp_first_kind(mlc::String("return")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'struct'     → CKStruct"), [&]() -> bool { if (std::holds_alternative<CKStruct>(cpp_first_kind(mlc::String("struct")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'using'      → CKUsing"), [&]() -> bool { if (std::holds_alternative<CKUsing>(cpp_first_kind(mlc::String("using")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'noexcept'   → CKNoexcept"), [&]() -> bool { if (std::holds_alternative<CKNoexcept>(cpp_first_kind(mlc::String("noexcept")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'::'  → CScope"), [&]() -> bool { if (std::holds_alternative<CScope>(cpp_first_kind(mlc::String("::")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'->'  → CArrow"), [&]() -> bool { if (std::holds_alternative<CArrow>(cpp_first_kind(mlc::String("->")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'=='  → CEq"), [&]() -> bool { if (std::holds_alternative<CEq>(cpp_first_kind(mlc::String("==")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'!='  → CNe"), [&]() -> bool { if (std::holds_alternative<CNe>(cpp_first_kind(mlc::String("!=")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'<='  → CLe"), [&]() -> bool { if (std::holds_alternative<CLe>(cpp_first_kind(mlc::String("<=")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'>='  → CGe"), [&]() -> bool { if (std::holds_alternative<CGe>(cpp_first_kind(mlc::String(">=")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'&&'  → CAnd"), [&]() -> bool { if (std::holds_alternative<CAnd>(cpp_first_kind(mlc::String("&&")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'||'  → COr"), [&]() -> bool { if (std::holds_alternative<COr>(cpp_first_kind(mlc::String("||")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'++'  → CInc"), [&]() -> bool { if (std::holds_alternative<CInc>(cpp_first_kind(mlc::String("++")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'--'  → CDec"), [&]() -> bool { if (std::holds_alternative<CDec>(cpp_first_kind(mlc::String("--")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'{' → CLBrace"), [&]() -> bool { if (std::holds_alternative<CLBrace>(cpp_first_kind(mlc::String("{")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'}' → CRBrace"), [&]() -> bool { if (std::holds_alternative<CRBrace>(cpp_first_kind(mlc::String("}")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'<' → CLAngle"), [&]() -> bool { if (std::holds_alternative<CLAngle>(cpp_first_kind(mlc::String("<")))) {  return true; } return false; }()));
results.push_back(assert_true(mlc::String("'>' → CRAngle"), [&]() -> bool { if (std::holds_alternative<CRAngle>(cpp_first_kind(mlc::String(">")))) {  return true; } return false; }()));
results.push_back(assert_eq_str(mlc::String("'foo' → CIdent(\"foo\")"), cpp_ident_at(mlc::String("foo"), 0), mlc::String("foo")));
results.push_back(assert_eq_str(mlc::String("'std::string' [0] → \"std\""), cpp_ident_at(mlc::String("std::string"), 0), mlc::String("std")));
results.push_back(assert_true(mlc::String("'std::string' [1] → CScope"), [&]() -> bool { if (std::holds_alternative<CScope>(cpp_kind_at(mlc::String("std::string"), 1))) {  return true; } return false; }()));
results.push_back(assert_eq_str(mlc::String("'std::string' [2] → \"string\""), cpp_ident_at(mlc::String("std::string"), 2), mlc::String("string")));
results.push_back(assert_eq_int(mlc::String("'42' → CLInt(42)"), cpp_int_at(mlc::String("42"), 0), 42));
results.push_back(assert_eq_int(mlc::String("'0'  → CLInt(0)"), cpp_int_at(mlc::String("0"), 0), 0));
results.push_back(assert_eq_str(mlc::String("'\"hello\"' → CLStr(\"hello\")"), cpp_str_at(mlc::String("\"hello\""), 0), mlc::String("hello")));
results.push_back(assert_eq_str(mlc::String("escape \\n in string"), cpp_str_at(mlc::String("\"a\\nb\""), 0), mlc::String("a\nb")));
results.push_back(assert_eq_str(mlc::String("escaped quote in string"), cpp_str_at(mlc::String("\"a\\\"b\""), 0), mlc::String("a\"b")));
results.push_back(assert_true(mlc::String("'#' → COp(\"#\")"), [&]() -> bool { if (std::holds_alternative<COp>(cpp_first_kind(mlc::String("#")))) { auto _v_cop = std::get<COp>(cpp_first_kind(mlc::String("#"))); auto [s] = _v_cop; return s == mlc::String("#"); } return false; }()));
return results;
}
mlc::Array<TestResult> cpp_ast_tests() noexcept{
mlc::Array<TestResult> results = {};
std::shared_ptr<CppDecl> inc = std::make_shared<CppDecl>(CppInclude(true, mlc::String("string")));
CppProgram prog = CppProgram{mlc::Array<std::shared_ptr<CppDecl>>{inc}};
results.push_back(assert_eq_int(mlc::String("CppProgram with 1 decl"), prog.decls.size(), 1));
std::shared_ptr<CppField> field = std::make_shared<CppField>(CppField{mlc::String("int"), mlc::String("x")});
std::shared_ptr<CppDecl> st = std::make_shared<CppDecl>(CppStruct(mlc::String("Point"), mlc::Array<std::shared_ptr<CppField>>{field}));
CppProgram prog2 = CppProgram{mlc::Array<std::shared_ptr<CppDecl>>{st}};
results.push_back(assert_true(mlc::String("CppStruct in program"), [&]() -> bool { if (std::holds_alternative<CppStruct>((*prog2.decls[0]))) { auto _v_cppstruct = std::get<CppStruct>((*prog2.decls[0])); auto [_w0, _w1] = _v_cppstruct; return true; } return false; }()));
return results;
}
TestResult assert_true(mlc::String test_name, bool condition) noexcept{return condition ? TestResult(Pass{test_name}) : TestResult(Fail{test_name, mlc::String("expected true, got false")});}
TestResult assert_eq_int(mlc::String test_name, int actual, int expected) noexcept{return actual == expected ? TestResult(Pass{test_name}) : TestResult(Fail{test_name, mlc::String("expected ") + mlc::to_string(expected) + mlc::String(", got ") + mlc::to_string(actual)});}
TestResult assert_eq_str(mlc::String test_name, mlc::String actual, mlc::String expected) noexcept{return actual == expected ? TestResult(Pass{test_name}) : TestResult(Fail{test_name, mlc::String("expected '") + expected + mlc::String("', got '") + actual + mlc::String("'")});}
mlc::String run_all(mlc::Array<TestResult> results) noexcept{
int passed = 0;
int failed = 0;
mlc::String output = mlc::String("");
int i = 0;
while (i < results.size()){
{
std::visit(overloaded{
  [&](const Pass& pass) {
auto [_w0] = pass;
{
passed = passed + 1;
}
},
  [&](const Fail& fail) {
auto [name, message] = fail;
{
failed = failed + 1;
output = output + mlc::String("FAIL: ") + name + mlc::String(" — ") + message + mlc::String("\n");
}
}
}, results[i]);
i = i + 1;
}
}
return output + mlc::to_string(passed) + mlc::String(" passed, ") + mlc::to_string(failed) + mlc::String(" failed\n");
}
mlc::Array<TestResult> all_tests() noexcept{
mlc::Array<TestResult> all = {};
int i = 0;
mlc::Array<TestResult> lex_results = lexer_tests();
while (i < lex_results.size()){
{
all.push_back(lex_results[i]);
i = i + 1;
}
}
mlc::Array<TestResult> parse_results = parser_tests();
i = 0;
while (i < parse_results.size()){
{
all.push_back(parse_results[i]);
i = i + 1;
}
}
mlc::Array<TestResult> check_results = checker_tests();
i = 0;
while (i < check_results.size()){
{
all.push_back(check_results[i]);
i = i + 1;
}
}
mlc::Array<TestResult> gen_results = codegen_tests();
i = 0;
while (i < gen_results.size()){
{
all.push_back(gen_results[i]);
i = i + 1;
}
}
mlc::Array<TestResult> pipe_record_results = pipe_and_record_update_tests();
i = 0;
while (i < pipe_record_results.size()){
{
all.push_back(pipe_record_results[i]);
i = i + 1;
}
}
mlc::Array<TestResult> cpp_lex_results = cpp_lexer_tests();
i = 0;
while (i < cpp_lex_results.size()){
{
all.push_back(cpp_lex_results[i]);
i = i + 1;
}
}
mlc::Array<TestResult> cpp_ast_results = cpp_ast_tests();
i = 0;
while (i < cpp_ast_results.size()){
{
all.push_back(cpp_ast_results[i]);
i = i + 1;
}
}
return all;
}
int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
mlc::io::print(run_all(all_tests()));
return 0;
}
