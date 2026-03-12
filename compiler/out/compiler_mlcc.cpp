#include "mlc/core/string.hpp"
#include "mlc/core/array.hpp"
#include "mlc/core/hashmap.hpp"
#include "mlc/core/match.hpp"
#include "mlc/io/io.hpp"
#include "mlc/io/file.hpp"
#include <functional>
#include <memory>
#include <variant>
#include <vector>

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
struct export;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, export>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct export;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, export>;
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
struct export;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda, export>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
struct export;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn, export>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
struct export;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord, export>;
struct DeclFn;
struct DeclType;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct export;
using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported, export>;
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
struct export;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, export>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct export;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, export>;
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
struct export;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda, export>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
struct export;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn, export>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
struct export;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord, export>;
struct DeclFn;
struct DeclType;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct export;
using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported, export>;
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
struct export;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, export>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct export;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, export>;
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
struct export;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda, export>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
struct export;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn, export>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
struct export;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord, export>;
struct DeclFn;
struct DeclType;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct export;
using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported, export>;
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
struct export;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, export>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct export;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, export>;
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
struct export;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda, export>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
struct export;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn, export>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
struct export;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord, export>;
struct DeclFn;
struct DeclType;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct export;
using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported, export>;
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
struct export;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, export>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct export;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, export>;
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
struct export;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda, export>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
struct export;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn, export>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
struct export;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord, export>;
struct DeclFn;
struct DeclType;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct export;
using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported, export>;
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
struct export;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, export>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct export;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, export>;
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
struct export;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda, export>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
struct export;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn, export>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
struct export;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord, export>;
struct DeclFn;
struct DeclType;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct export;
using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported, export>;
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
struct export;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, export>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct export;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, export>;
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
struct export;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda, export>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
struct export;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn, export>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
struct export;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord, export>;
struct DeclFn;
struct DeclType;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct export;
using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported, export>;
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
struct export;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, export>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct export;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, export>;
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
struct export;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda, export>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
struct export;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn, export>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
struct export;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord, export>;
struct DeclFn;
struct DeclType;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct export;
using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported, export>;
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
struct export;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, export>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct export;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, export>;
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
struct export;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda, export>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
struct export;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn, export>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
struct export;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord, export>;
struct DeclFn;
struct DeclType;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct export;
using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported, export>;
struct TI32;
struct TString;
struct TBool;
struct TUnit;
struct TArray;
struct TShared;
struct TNamed;
struct TFn;
struct TUnknown;
struct export;
using Type = std::variant<TI32, TString, TBool, TUnit, TArray, TShared, TNamed, TFn, TUnknown, export>;
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
struct export;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, export>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct export;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, export>;
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
struct export;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda, export>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
struct export;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn, export>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
struct export;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord, export>;
struct DeclFn;
struct DeclType;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct export;
using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported, export>;
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
struct export;
using TypeExpr = std::variant<TyI32, TyString, TyBool, TyUnit, TyNamed, TyArray, TyShared, TyGeneric, TyFn, export>;
struct PatWild;
struct PatIdent;
struct PatInt;
struct PatStr;
struct PatBool;
struct PatUnit;
struct PatCtor;
struct export;
using Pat = std::variant<PatWild, PatIdent, PatInt, PatStr, PatBool, PatUnit, PatCtor, export>;
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
struct export;
using Expr = std::variant<ExprInt, ExprStr, ExprBool, ExprUnit, ExprIdent, ExprBin, ExprUn, ExprCall, ExprMethod, ExprField, ExprIndex, ExprIf, ExprBlock, ExprWhile, ExprFor, ExprMatch, ExprRecord, ExprRecordUpdate, ExprArray, ExprQuestion, ExprLambda, export>;
struct StmtLet;
struct StmtExpr;
struct StmtBreak;
struct StmtContinue;
struct StmtReturn;
struct export;
using Stmt = std::variant<StmtLet, StmtExpr, StmtBreak, StmtContinue, StmtReturn, export>;
struct VarUnit;
struct VarTuple;
struct VarRecord;
struct export;
using TypeVariant = std::variant<VarUnit, VarTuple, VarRecord, export>;
struct DeclFn;
struct DeclType;
struct DeclExtend;
struct DeclImport;
struct DeclExported;
struct export;
using Decl = std::variant<DeclFn, DeclType, DeclExtend, DeclImport, DeclExported, export>;
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
struct export {};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct export {};
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
struct export {};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct export {};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct export {};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<Decl>> field1;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct export {};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
struct LexState {mlc::String src;int pos;int line;int col;};
struct ScanResult {LexState state;Token token;};
struct ScanStrResult {LexState state;Token token;mlc::String error;};
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
struct export {};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct export {};
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
struct export {};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct export {};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct export {};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<Decl>> field1;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct export {};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
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
struct export {};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct export {};
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
struct export {};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct export {};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct export {};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<Decl>> field1;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct export {};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
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
struct export {};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct export {};
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
struct export {};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct export {};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct export {};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<Decl>> field1;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct export {};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
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
struct export {};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct export {};
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
struct export {};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct export {};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct export {};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<Decl>> field1;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct export {};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
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
struct export {};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct export {};
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
struct export {};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct export {};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct export {};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<Decl>> field1;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct export {};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
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
struct export {};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct export {};
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
struct export {};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct export {};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct export {};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<Decl>> field1;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct export {};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
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
struct export {};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct export {};
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
struct export {};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct export {};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct export {};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<Decl>> field1;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct export {};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
struct NameCheckResult {mlc::Array<mlc::String> errors;mlc::Array<mlc::String> scope;};
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
struct export {};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct export {};
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
struct export {};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct export {};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct export {};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<Decl>> field1;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct export {};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
struct TI32 {};
struct TString {};
struct TBool {};
struct TUnit {};
struct TArray {std::shared_ptr<Type> field0;};
struct TShared {std::shared_ptr<Type> field0;};
struct TNamed {mlc::String field0;};
struct TFn {mlc::Array<std::shared_ptr<Type>> field0;std::shared_ptr<Type> field1;};
struct TUnknown {};
struct export {};
struct TypeRegistry {Map<mlc::String, std::shared_ptr<Type>> fn_types;Map<mlc::String, std::shared_ptr<Type>> ctor_types;Map<mlc::String, mlc::Array<std::shared_ptr<Type>>> ctor_params;Map<mlc::String, Map<mlc::String, std::shared_ptr<Type>>> field_types;};
struct InferResult {std::shared_ptr<Type> inferred_type;mlc::Array<mlc::String> errors;};
struct StmtInferResult {Map<mlc::String, std::shared_ptr<Type>> type_env;mlc::Array<mlc::String> errors;};
struct CheckOut {mlc::Array<mlc::String> errors;TypeRegistry registry;};
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
struct export {};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct export {};
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
struct export {};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct export {};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct export {};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<Decl>> field1;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct export {};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
struct FieldOrder {mlc::String type_name;mlc::Array<mlc::String> fields;};
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
struct export {};
struct PatWild {};
struct PatIdent {mlc::String field0;};
struct PatInt {int field0;};
struct PatStr {mlc::String field0;};
struct PatBool {bool field0;};
struct PatUnit {};
struct PatCtor {mlc::String field0;mlc::Array<std::shared_ptr<Pat>> field1;};
struct export {};
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
struct export {};
struct StmtLet {mlc::String field0;bool field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct StmtExpr {std::shared_ptr<Expr> field0;};
struct StmtBreak {};
struct StmtContinue {};
struct StmtReturn {std::shared_ptr<Expr> field0;};
struct export {};
struct Param {mlc::String name;bool is_mut;std::shared_ptr<TypeExpr> typ;};
struct FieldDef {mlc::String name;std::shared_ptr<TypeExpr> typ;};
struct VarUnit {mlc::String field0;};
struct VarTuple {mlc::String field0;mlc::Array<std::shared_ptr<TypeExpr>> field1;};
struct VarRecord {mlc::String field0;mlc::Array<std::shared_ptr<FieldDef>> field1;};
struct export {};
struct DeclFn {mlc::String field0;mlc::Array<std::shared_ptr<Param>> field1;std::shared_ptr<TypeExpr> field2;std::shared_ptr<Expr> field3;};
struct DeclType {mlc::String field0;mlc::Array<std::shared_ptr<TypeVariant>> field1;};
struct DeclExtend {mlc::String field0;mlc::Array<std::shared_ptr<Decl>> field1;};
struct DeclImport {mlc::String field0;mlc::Array<mlc::String> field1;};
struct DeclExported {std::shared_ptr<Decl> field0;};
struct export {};
struct Program {mlc::Array<std::shared_ptr<Decl>> decls;};
struct LoadResult {Program prog;mlc::Array<mlc::String> errors;};
struct MergeResult {Program prog;mlc::Array<mlc::String> errors;};
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept;
bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
bool LexState_eof(LexState self) noexcept;
mlc::String LexState_current(LexState self) noexcept;
mlc::String LexState_peek(LexState self, int offset) noexcept;
LexState LexState_advance(LexState self) noexcept;
LexState LexState_advance_by(LexState self, int count) noexcept;
Token LexState_token(LexState self, TKind kind) noexcept;
bool is_alpha(mlc::String character) noexcept;
bool is_digit(mlc::String character) noexcept;
bool is_alnum(mlc::String character) noexcept;
bool is_ws(mlc::String character) noexcept;
TKind keyword_kind(mlc::String word) noexcept;
ScanResult scan_ident(LexState state) noexcept;
ScanResult scan_int(LexState state) noexcept;
mlc::String map_escape(mlc::String character) noexcept;
ScanStrResult scan_string(LexState state) noexcept;
LexState skip_whitespace(LexState state) noexcept;
ScanResult scan_op(LexState state) noexcept;
LexOut tokenize(mlc::String source) noexcept;
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept;
bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
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
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept;
bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
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
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept;
bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
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
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept;
bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
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
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept;
bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
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
bool is_cmp_op(mlc::String op) noexcept;
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
VariantsResult parse_variants(Parser parser) noexcept;
VariantResult parse_variant(Parser parser) noexcept;
TypesResult parse_tuple_types(Parser parser) noexcept;
FieldDefsResult parse_field_defs(Parser parser) noexcept;
Program parse_program(mlc::Array<Token> tokens) noexcept;
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept;
bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept;
bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept;
mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<Pat> pattern) noexcept;
mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<Pat> pattern, mlc::Array<mlc::String> accumulator) noexcept;
mlc::Array<mlc::String> collect_globals(Program prog) noexcept;
mlc::Array<mlc::String> check_names_expr(std::shared_ptr<Expr> expr, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept;
mlc::Array<mlc::String> NameCheckResult_append_expr_errors(NameCheckResult self, mlc::Array<mlc::String> expr_errors) noexcept;
NameCheckResult check_names_stmts(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept;
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept;
bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
std::shared_ptr<Type> TypeRegistry_fn_type(TypeRegistry self, mlc::String name) noexcept;
bool TypeRegistry_has_fn(TypeRegistry self, mlc::String name) noexcept;
std::shared_ptr<Type> TypeRegistry_ctor_type(TypeRegistry self, mlc::String name) noexcept;
bool TypeRegistry_has_ctor(TypeRegistry self, mlc::String name) noexcept;
mlc::Array<std::shared_ptr<Type>> TypeRegistry_ctor_params_for(TypeRegistry self, mlc::String name) noexcept;
Map<mlc::String, std::shared_ptr<Type>> TypeRegistry_fields_for(TypeRegistry self, mlc::String type_name) noexcept;
bool TypeRegistry_has_fields(TypeRegistry self, mlc::String type_name) noexcept;
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
InferResult infer_args_errors(InferResult initial, mlc::Array<std::shared_ptr<Expr>> args, Map<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept;
InferResult infer_field_vals_errors(InferResult initial, mlc::Array<std::shared_ptr<FieldVal>> field_vals, Map<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept;
std::shared_ptr<Type> field_lookup_type(std::shared_ptr<Type> obj_type, mlc::String field_name, TypeRegistry registry) noexcept;
InferResult infer_expr(std::shared_ptr<Expr> expr, Map<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept;
StmtInferResult infer_stmts(mlc::Array<std::shared_ptr<Stmt>> stmts, Map<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept;
Map<mlc::String, std::shared_ptr<Type>> env_for_pat(Map<mlc::String, std::shared_ptr<Type>> type_env, std::shared_ptr<Pat> pat, TypeRegistry registry) noexcept;
Map<mlc::String, std::shared_ptr<Type>> env_for_pat_with_type(Map<mlc::String, std::shared_ptr<Type>> type_env, std::shared_ptr<Pat> pat, std::shared_ptr<Type> type_val, TypeRegistry registry) noexcept;
bool CheckOut_has_errors(CheckOut self) noexcept;
CheckOut check(Program prog) noexcept;
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept;
bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
mlc::Array<std::shared_ptr<FieldOrder>> build_field_orders(Program prog) noexcept;
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept;
mlc::String lower_first(mlc::String name) noexcept;
mlc::String type_to_cpp(std::shared_ptr<TypeExpr> type_expr) noexcept;
mlc::String escape_str(mlc::String input) noexcept;
mlc::String gen_args(mlc::Array<std::shared_ptr<Expr>> args, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String map_method(mlc::String method_name) noexcept;
mlc::String map_builtin(mlc::String name) noexcept;
mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_block_body(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_expr(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::Array<mlc::String> field_order, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_record_update_ordered(std::shared_ptr<Expr> base, mlc::Array<std::shared_ptr<FieldVal>> overrides, mlc::Array<mlc::String> field_order, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
std::shared_ptr<Expr> find_field_val(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::String name) noexcept;
mlc::String gen_match(std::shared_ptr<Expr> subject, mlc::Array<std::shared_ptr<MatchArm>> arms, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_arm(std::shared_ptr<MatchArm> arm, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_pat_header(std::shared_ptr<Pat> pat) noexcept;
mlc::String pat_bind_names(mlc::Array<std::shared_ptr<Pat>> sub_pats) noexcept;
mlc::String gen_stmt(std::shared_ptr<Stmt> stmt, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_stmt_expr(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_return_body(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_return_if_stmt(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_fn_body(std::shared_ptr<Expr> body, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_params(mlc::Array<std::shared_ptr<Param>> params) noexcept;
mlc::String gen_type_decl_fwd(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept;
mlc::String gen_type_decl_body(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept;
mlc::String gen_single_variant(mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept;
mlc::String gen_adt_fwd(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept;
mlc::String gen_adt_defs(mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept;
mlc::String variant_ctor_name(std::shared_ptr<TypeVariant> variant) noexcept;
mlc::String gen_variant_struct(std::shared_ptr<TypeVariant> variant) noexcept;
mlc::String gen_fn_decl(mlc::String name, mlc::Array<std::shared_ptr<Param>> params, std::shared_ptr<TypeExpr> return_type, std::shared_ptr<Expr> body, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_fn_proto(mlc::String name, mlc::Array<std::shared_ptr<Param>> params, std::shared_ptr<TypeExpr> return_type) noexcept;
mlc::String gen_type_decl(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept;
mlc::String gen_decl(std::shared_ptr<Decl> decl, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept;
mlc::String gen_proto(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<Decl>> decls, mlc::Array<std::shared_ptr<FieldOrder>> field_orders, int phase) noexcept;
mlc::String gen_program(Program prog) noexcept;
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept;
bool LexOut_has_errors(LexOut self) noexcept;
TKind Token_kind_value(Token self) noexcept;
int Token_line_number(Token self) noexcept;
int Token_column(Token self) noexcept;
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept;
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept;
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept;
mlc::String dirname(mlc::String path) noexcept;
mlc::String resolve_dotdot(mlc::String path) noexcept;
mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept;
bool path_in_loaded(mlc::String path, mlc::Array<mlc::String> loaded) noexcept;
LoadResult load_module_impl(mlc::String path, mlc::Array<mlc::String> loaded, Map<mlc::String, LoadResult> cache) noexcept;
LoadResult load_module(mlc::String path, Map<mlc::String, LoadResult> cache) noexcept;
MergeResult merge_program(mlc::String entry_path, Program prog) noexcept;
mlc::String compile_source(mlc::String src, mlc::String entry_path) noexcept;
mlc::String format_errs(mlc::String label, mlc::Array<mlc::String> errors) noexcept;
int main(int argc, char** argv) noexcept;

std::shared_ptr<T> Shared_new(Shared self, T value) noexcept{
return export;
}
bool LexOut_has_errors(LexOut self) noexcept{
return (errors.length() > 0);
}
TKind Token_kind_value(Token self) noexcept{
return kind;
}
int Token_line_number(Token self) noexcept{
return line;
}
int Token_column(Token self) noexcept{
return col;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [d] = declExported; } { return d; },
[&](auto& __v) { return decl; }
}, decl);
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; } { return name; },
[&](const DeclType& declType) { auto [name, __1] = declType; } { return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1] = declExtend; } { return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return decl_name(d); }
}, decl);
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
auto i = 0;
while ((i < src.length())) {
dst.push_back(src[i]);
i = (i + 1);
}
return dst;
}
bool LexState_eof(LexState self) noexcept{
return (pos >= src.length());
}
mlc::String LexState_current(LexState self) noexcept{
if (self.eof()) {
return mlc::String("\\0");
} else {
return src.char_at(pos);
}
}
mlc::String LexState_peek(LexState self, int offset) noexcept{
auto position = (pos + offset);
return ((position >= src.length()) ? mlc::String("\\0") : src.char_at(position));
}
LexState LexState_advance(LexState self) noexcept{
if ((self.current() == mlc::String("\n"))) {
return LexState{src, (pos + 1), (line + 1), 1};
} else {
return LexState{src, (pos + 1), line, (col + 1)};
}
}
LexState LexState_advance_by(LexState self, int count) noexcept{
auto state = self;
auto i = 0;
while ((i < count)) {
state = state.advance();
i = (i + 1);
}
return state;
}
Token LexState_token(LexState self, TKind kind) noexcept{
return Token{kind, line, col};
}
bool is_alpha(mlc::String character) noexcept{
return ((((character >= mlc::String("a")) && (character <= mlc::String("z"))) || ((character >= mlc::String("A")) && (character <= mlc::String("Z")))) || (character == mlc::String("_")));
}
bool is_digit(mlc::String character) noexcept{
return ((character >= mlc::String("0")) && (character <= mlc::String("9")));
}
bool is_alnum(mlc::String character) noexcept{
return (is_alpha(character) || is_digit(character));
}
bool is_ws(mlc::String character) noexcept{
return ((((character == mlc::String(" ")) || (character == mlc::String("\t"))) || (character == mlc::String("\r"))) || (character == mlc::String("\n")));
}
TKind keyword_kind(mlc::String word) noexcept{
if ((word == mlc::String("fn"))) {
return KFn{};
} else if ((word == mlc::String("type"))) {
return KType{};
} else if ((word == mlc::String("let"))) {
return KLet{};
} else if ((word == mlc::String("mut"))) {
return KMut{};
} else if ((word == mlc::String("const"))) {
return KConst{};
} else if ((word == mlc::String("return"))) {
return KReturn{};
} else if ((word == mlc::String("break"))) {
return KBreak{};
} else if ((word == mlc::String("continue"))) {
return KContinue{};
} else if ((word == mlc::String("if"))) {
return KIf{};
} else if ((word == mlc::String("then"))) {
return KThen{};
} else if ((word == mlc::String("else"))) {
return KElse{};
} else if ((word == mlc::String("unless"))) {
return KUnless{};
} else if ((word == mlc::String("while"))) {
return KWhile{};
} else if ((word == mlc::String("for"))) {
return KFor{};
} else if ((word == mlc::String("in"))) {
return KIn{};
} else if ((word == mlc::String("do"))) {
return KDo{};
} else if ((word == mlc::String("end"))) {
return KEnd{};
} else if ((word == mlc::String("match"))) {
return KMatch{};
} else if ((word == mlc::String("import"))) {
return KImport{};
} else if ((word == mlc::String("from"))) {
return KFrom{};
} else if ((word == mlc::String("as"))) {
return KAs{};
} else if ((word == mlc::String("extern"))) {
return KExtern{};
} else if ((word == mlc::String("extend"))) {
return KExtend{};
} else if ((word == mlc::String("where"))) {
return KWhere{};
} else if ((word == mlc::String("true"))) {
return KTrue{};
} else if ((word == mlc::String("false"))) {
return KFalse{};
} else {
return Ident(word);
}

























}
ScanResult scan_ident(LexState state) noexcept{
auto start = state.pos;
auto source = state.src;
auto token_line = state.line;
auto token_col = state.col;
auto current = state;
while (((!current.eof()) && is_alnum(current.current()))) {
current = current.advance();
}
auto word = source.substring(start, (current.pos - start));
return ScanResult{current, Token{keyword_kind(word), token_line, token_col}};
}
ScanResult scan_int(LexState state) noexcept{
auto token_line = state.line;
auto token_col = state.col;
auto current = state;
auto value = 0;
while (((!current.eof()) && is_digit(current.current()))) {
value = ((value * 10) + current.current().to_i());
current = current.advance();
}
return ScanResult{current, Token{LInt(value), token_line, token_col}};
}
mlc::String map_escape(mlc::String character) noexcept{
if ((character == mlc::String("n"))) {
return mlc::String("\n");
} else if ((character == mlc::String("t"))) {
return mlc::String("\t");
} else if ((character == mlc::String("r"))) {
return mlc::String("\r");
} else if ((character == mlc::String("\""))) {
return mlc::String("\"");
} else if ((character == mlc::String("\\"))) {
return mlc::String("\\");
} else if ((character == mlc::String("0"))) {
return mlc::String("\\0");
} else if ((character == mlc::String("{"))) {
return mlc::String("{");
} else if ((character == mlc::String("}"))) {
return mlc::String("}");
} else {
return mlc::String("");
}







}
ScanStrResult scan_string(LexState state) noexcept{
auto token_line = state.line;
auto token_col = state.col;
auto current = state.advance();
auto parts = mlc::Array<mlc::String>{};
auto error = mlc::String("");
while (((!current.eof()) && (current.current() != mlc::String("\"")))) {
if ((current.current() == mlc::String("\\"))) {
current = current.advance();
auto mapped = map_escape(current.current());
if ((mapped != mlc::String(""))) {
parts.push_back(mapped);
} else {
parts.push_back(mlc::String("\\"));
parts.push_back(current.current());
}
(current = current.advance());
} else {
parts.push_back(current.current());
(current = current.advance());
}
}
if (current.eof()) {
(error = mlc::String("unterminated string"));
} else {
(current = current.advance());
}
return ScanStrResult{current, Token{LStr(parts.join(mlc::String(""))), token_line, token_col}, error};
}
LexState skip_whitespace(LexState state) noexcept{
auto current = state;
while ((!current.eof())) {
auto character = current.current();
if (is_ws(character)) {
(current = current.advance());
} else if (((character == mlc::String("/")) && (current.peek(1) == mlc::String("/")))) {
current = current.advance_by(2);
[&]() {
while (((!current.eof()) && (current.current() != mlc::String("\n")))) {
current = current.advance();
}
}();
} else if (((character == mlc::String("/")) && (current.peek(1) == mlc::String("*")))) {
current = current.advance_by(2);
[&]() {
while ((!current.eof())) {
if (((current.current() == mlc::String("*")) && (current.peek(1) == mlc::String("/")))) {
current = current.advance_by(2);
break;
}
current = current.advance();
}
}();
} else {
break;
}


}
return current;
}
ScanResult scan_op(LexState state) noexcept{
auto token_line = state.line;
auto token_col = state.col;
auto character = state.current();
auto next = state.peek(1);
return (((character == mlc::String("-")) && (next == mlc::String(">"))) ? ScanResult{state.advance_by(2), state.token(Arrow{})} : (((character == mlc::String("=")) && (next == mlc::String(">"))) ? ScanResult{state.advance_by(2), state.token(FatArrow{})} : (((character == mlc::String("|")) && (next == mlc::String(">"))) ? ScanResult{state.advance_by(2), state.token(Pipe{})} : (((character == mlc::String("=")) && (next == mlc::String("="))) ? ScanResult{state.advance_by(2), state.token(Op(mlc::String("==")))} : (((character == mlc::String("!")) && (next == mlc::String("="))) ? ScanResult{state.advance_by(2), state.token(Op(mlc::String("!=")))} : (((character == mlc::String("<")) && (next == mlc::String("="))) ? ScanResult{state.advance_by(2), state.token(Op(mlc::String("<=")))} : (((character == mlc::String(">")) && (next == mlc::String("="))) ? ScanResult{state.advance_by(2), state.token(Op(mlc::String(">=")))} : (((character == mlc::String("&")) && (next == mlc::String("&"))) ? ScanResult{state.advance_by(2), state.token(Op(mlc::String("&&")))} : (((character == mlc::String("|")) && (next == mlc::String("|"))) ? ScanResult{state.advance_by(2), state.token(Op(mlc::String("||")))} : ((character == mlc::String("=")) ? ScanResult{state.advance(), state.token(Equal{})} : ((character == mlc::String("|")) ? ScanResult{state.advance(), state.token(Bar{})} : ((character == mlc::String("?")) ? ScanResult{state.advance(), state.token(Question{})} : ((((character == mlc::String(".")) && (next == mlc::String("."))) && (state.peek(2) == mlc::String("."))) ? ScanResult{state.advance_by(3), state.token(Spread{})} : ((character == mlc::String(".")) ? ScanResult{state.advance(), state.token(Dot{})} : ((character == mlc::String("(")) ? ScanResult{state.advance(), state.token(LParen{})} : ((character == mlc::String(")")) ? ScanResult{state.advance(), state.token(RParen{})} : ((character == mlc::String("{")) ? ScanResult{state.advance(), state.token(LBrace{})} : ((character == mlc::String("}")) ? ScanResult{state.advance(), state.token(RBrace{})} : ((character == mlc::String("[")) ? ScanResult{state.advance(), state.token(LBracket{})} : ((character == mlc::String("]")) ? ScanResult{state.advance(), state.token(RBracket{})} : ((character == mlc::String(",")) ? ScanResult{state.advance(), state.token(Comma{})} : ((character == mlc::String(";")) ? ScanResult{state.advance(), state.token(Semicolon{})} : ((character == mlc::String(":")) ? ScanResult{state.advance(), state.token(Colon{})} : ScanResult{state.advance(), state.token(Op(character))})))))))))))))))))))))));
}
LexOut tokenize(mlc::String source) noexcept{
auto state = LexState{source, 0, 1, 1};
auto tokens = mlc::Array<Token>{};
auto errors = mlc::Array<mlc::String>{};
while ((!state.eof())) {
state = skip_whitespace(state);
if (state.eof()) {
break;
}
auto character = state.current();
if (is_alpha(character)) {
auto result = scan_ident(state);
tokens.push_back(result.token);
(state = result.state);
} else if (is_digit(character)) {
auto result = scan_int(state);
tokens.push_back(result.token);
(state = result.state);
} else if ((character == mlc::String("\""))) {
auto result = scan_string(state);
tokens.push_back(result.token);
state = result.state;
if ((result.error != mlc::String(""))) {
errors.push_back(result.error);
}
} else {
auto result = scan_op(state);
tokens.push_back(result.token);
(state = result.state);
}


}
tokens.push_back(state.token(Eof{}));
return LexOut{tokens, errors};
}
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept{
return export;
}
bool LexOut_has_errors(LexOut self) noexcept{
return (errors.length() > 0);
}
TKind Token_kind_value(Token self) noexcept{
return kind;
}
int Token_line_number(Token self) noexcept{
return line;
}
int Token_column(Token self) noexcept{
return col;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [d] = declExported; } { return d; },
[&](auto& __v) { return decl; }
}, decl);
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; } { return name; },
[&](const DeclType& declType) { auto [name, __1] = declType; } { return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1] = declExtend; } { return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return decl_name(d); }
}, decl);
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
auto i = 0;
while ((i < src.length())) {
dst.push_back(src[i]);
i = (i + 1);
}
return dst;
}
Parser parser_new(mlc::Array<Token> tokens) noexcept{
return Parser{tokens, 0};
}
TKind parser_kind(Parser parser) noexcept{
return parser.tokens[parser.pos].kind;
}
Parser parser_advance(Parser parser) noexcept{
return Parser{parser.tokens, (parser.pos + 1)};
}
Parser parser_advance_by(Parser parser, int count) noexcept{
return Parser{parser.tokens, (parser.pos + count)};
}
bool parser_at_eof(Parser parser) noexcept{
return std::visit(overloaded{[&](const Eof& eof) { return true; },
[&](auto& __v) { return false; }
}, parser_kind(parser));
}
Parser parser_skip_semi(Parser parser) noexcept{
return std::visit(overloaded{[&](const Semicolon& semicolon) { return parser_advance(parser); },
[&](auto& __v) { return parser; }
}, parser_kind(parser));
}
bool is_keyword_fn(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFn& kFn) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_type(TKind kind) noexcept{
return std::visit(overloaded{[&](const KType& kType) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_let(TKind kind) noexcept{
return std::visit(overloaded{[&](const KLet& kLet) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_mut(TKind kind) noexcept{
return std::visit(overloaded{[&](const KMut& kMut) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_const(TKind kind) noexcept{
return std::visit(overloaded{[&](const KConst& kConst) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_if(TKind kind) noexcept{
return std::visit(overloaded{[&](const KIf& kIf) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_unless(TKind kind) noexcept{
return std::visit(overloaded{[&](const KUnless& kUnless) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_else(TKind kind) noexcept{
return std::visit(overloaded{[&](const KElse& kElse) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_end(TKind kind) noexcept{
return std::visit(overloaded{[&](const KEnd& kEnd) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_do(TKind kind) noexcept{
return std::visit(overloaded{[&](const KDo& kDo) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_while(TKind kind) noexcept{
return std::visit(overloaded{[&](const KWhile& kWhile) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_for(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFor& kFor) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_match(TKind kind) noexcept{
return std::visit(overloaded{[&](const KMatch& kMatch) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_return(TKind kind) noexcept{
return std::visit(overloaded{[&](const KReturn& kReturn) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_break(TKind kind) noexcept{
return std::visit(overloaded{[&](const KBreak& kBreak) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_continue(TKind kind) noexcept{
return std::visit(overloaded{[&](const KContinue& kContinue) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_true(TKind kind) noexcept{
return std::visit(overloaded{[&](const KTrue& kTrue) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_false(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFalse& kFalse) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_extern(TKind kind) noexcept{
return std::visit(overloaded{[&](const KExtern& kExtern) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_extend(TKind kind) noexcept{
return std::visit(overloaded{[&](const KExtend& kExtend) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_ident(TKind kind) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [__0] = ident; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_int_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [__0] = lInt; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_str_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [__0] = lStr; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_equal(TKind kind) noexcept{
return std::visit(overloaded{[&](const Equal& equal) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_bar(TKind kind) noexcept{
return std::visit(overloaded{[&](const Bar& bar) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_pipe(TKind kind) noexcept{
return std::visit(overloaded{[&](const Pipe& pipe) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_colon(TKind kind) noexcept{
return std::visit(overloaded{[&](const Colon& colon) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_comma(TKind kind) noexcept{
return std::visit(overloaded{[&](const Comma& comma) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_dot(TKind kind) noexcept{
return std::visit(overloaded{[&](const Dot& dot) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_spread(TKind kind) noexcept{
return std::visit(overloaded{[&](const Spread& spread) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_arrow(TKind kind) noexcept{
return std::visit(overloaded{[&](const Arrow& arrow) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_fat_arrow(TKind kind) noexcept{
return std::visit(overloaded{[&](const FatArrow& fatArrow) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_question(TKind kind) noexcept{
return std::visit(overloaded{[&](const Question& question) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_paren(TKind kind) noexcept{
return std::visit(overloaded{[&](const LParen& lParen) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_paren(TKind kind) noexcept{
return std::visit(overloaded{[&](const RParen& rParen) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_brace(TKind kind) noexcept{
return std::visit(overloaded{[&](const LBrace& lBrace) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_brace(TKind kind) noexcept{
return std::visit(overloaded{[&](const RBrace& rBrace) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_bracket(TKind kind) noexcept{
return std::visit(overloaded{[&](const LBracket& lBracket) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_bracket(TKind kind) noexcept{
return std::visit(overloaded{[&](const RBracket& rBracket) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_operator(TKind kind) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [__0] = op; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
mlc::String get_ident(TKind kind) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [name] = ident; } { return name; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
int get_int_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [n] = lInt; } { return n; },
[&](auto& __v) { return 0; }
}, kind);
}
mlc::String get_str_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [s] = lStr; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
mlc::String get_operator(TKind kind) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [s] = op; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
bool is_ctor_name(mlc::String name) noexcept{
return ((name.char_at(0) >= mlc::String("A")) && (name.char_at(0) <= mlc::String("Z")));
}
bool TKind_is_fn(TKind self) noexcept{
return std::visit(overloaded{[&](const KFn& kFn) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_type(TKind self) noexcept{
return std::visit(overloaded{[&](const KType& kType) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_let(TKind self) noexcept{
return std::visit(overloaded{[&](const KLet& kLet) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_mut(TKind self) noexcept{
return std::visit(overloaded{[&](const KMut& kMut) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_const(TKind self) noexcept{
return std::visit(overloaded{[&](const KConst& kConst) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_if(TKind self) noexcept{
return std::visit(overloaded{[&](const KIf& kIf) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_unless(TKind self) noexcept{
return std::visit(overloaded{[&](const KUnless& kUnless) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_else(TKind self) noexcept{
return std::visit(overloaded{[&](const KElse& kElse) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_end(TKind self) noexcept{
return std::visit(overloaded{[&](const KEnd& kEnd) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_do(TKind self) noexcept{
return std::visit(overloaded{[&](const KDo& kDo) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_while(TKind self) noexcept{
return std::visit(overloaded{[&](const KWhile& kWhile) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_for(TKind self) noexcept{
return std::visit(overloaded{[&](const KFor& kFor) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_match(TKind self) noexcept{
return std::visit(overloaded{[&](const KMatch& kMatch) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_return(TKind self) noexcept{
return std::visit(overloaded{[&](const KReturn& kReturn) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_break(TKind self) noexcept{
return std::visit(overloaded{[&](const KBreak& kBreak) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_continue(TKind self) noexcept{
return std::visit(overloaded{[&](const KContinue& kContinue) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_true(TKind self) noexcept{
return std::visit(overloaded{[&](const KTrue& kTrue) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_false(TKind self) noexcept{
return std::visit(overloaded{[&](const KFalse& kFalse) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_extern(TKind self) noexcept{
return std::visit(overloaded{[&](const KExtern& kExtern) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_extend(TKind self) noexcept{
return std::visit(overloaded{[&](const KExtend& kExtend) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_import(TKind self) noexcept{
return std::visit(overloaded{[&](const KImport& kImport) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_from(TKind self) noexcept{
return std::visit(overloaded{[&](const KFrom& kFrom) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_eof(TKind self) noexcept{
return std::visit(overloaded{[&](const Eof& eof) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_ident(TKind self) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [__0] = ident; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_int(TKind self) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [__0] = lInt; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_str(TKind self) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [__0] = lStr; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_equal(TKind self) noexcept{
return std::visit(overloaded{[&](const Equal& equal) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_bar(TKind self) noexcept{
return std::visit(overloaded{[&](const Bar& bar) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_pipe(TKind self) noexcept{
return std::visit(overloaded{[&](const Pipe& pipe) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_colon(TKind self) noexcept{
return std::visit(overloaded{[&](const Colon& colon) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_comma(TKind self) noexcept{
return std::visit(overloaded{[&](const Comma& comma) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_dot(TKind self) noexcept{
return std::visit(overloaded{[&](const Dot& dot) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_spread(TKind self) noexcept{
return std::visit(overloaded{[&](const Spread& spread) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_arrow(TKind self) noexcept{
return std::visit(overloaded{[&](const Arrow& arrow) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_fat_arrow(TKind self) noexcept{
return std::visit(overloaded{[&](const FatArrow& fatArrow) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_question(TKind self) noexcept{
return std::visit(overloaded{[&](const Question& question) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lparen(TKind self) noexcept{
return std::visit(overloaded{[&](const LParen& lParen) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rparen(TKind self) noexcept{
return std::visit(overloaded{[&](const RParen& rParen) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lbrace(TKind self) noexcept{
return std::visit(overloaded{[&](const LBrace& lBrace) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rbrace(TKind self) noexcept{
return std::visit(overloaded{[&](const RBrace& rBrace) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lbracket(TKind self) noexcept{
return std::visit(overloaded{[&](const LBracket& lBracket) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rbracket(TKind self) noexcept{
return std::visit(overloaded{[&](const RBracket& rBracket) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_op(TKind self) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [__0] = op; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
mlc::String TKind_ident(TKind self) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [name] = ident; } { return name; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
int TKind_int_val(TKind self) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [n] = lInt; } { return n; },
[&](auto& __v) { return 0; }
}, self);
}
mlc::String TKind_str_val(TKind self) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [s] = lStr; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
mlc::String TKind_op_val(TKind self) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [s] = op; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
TKind Parser_kind(Parser self) noexcept{
return tokens[pos].kind;
}
Parser Parser_advance(Parser self) noexcept{
return Parser{tokens, (pos + 1)};
}
Parser Parser_advance_by(Parser self, int count) noexcept{
return Parser{tokens, (pos + count)};
}
bool Parser_at_eof(Parser self) noexcept{
return tokens[pos].kind.is_eof();
}
Parser Parser_skip_semi(Parser self) noexcept{
return std::visit(overloaded{[&](const Semicolon& semicolon) { return self.advance(); },
[&](auto& __v) { return self; }
}, tokens[pos].kind);
}
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept{
return export;
}
bool LexOut_has_errors(LexOut self) noexcept{
return (errors.length() > 0);
}
TKind Token_kind_value(Token self) noexcept{
return kind;
}
int Token_line_number(Token self) noexcept{
return line;
}
int Token_column(Token self) noexcept{
return col;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [d] = declExported; } { return d; },
[&](auto& __v) { return decl; }
}, decl);
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; } { return name; },
[&](const DeclType& declType) { auto [name, __1] = declType; } { return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1] = declExtend; } { return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return decl_name(d); }
}, decl);
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
auto i = 0;
while ((i < src.length())) {
dst.push_back(src[i]);
i = (i + 1);
}
return dst;
}
Parser parser_new(mlc::Array<Token> tokens) noexcept{
return Parser{tokens, 0};
}
TKind parser_kind(Parser parser) noexcept{
return parser.tokens[parser.pos].kind;
}
Parser parser_advance(Parser parser) noexcept{
return Parser{parser.tokens, (parser.pos + 1)};
}
Parser parser_advance_by(Parser parser, int count) noexcept{
return Parser{parser.tokens, (parser.pos + count)};
}
bool parser_at_eof(Parser parser) noexcept{
return std::visit(overloaded{[&](const Eof& eof) { return true; },
[&](auto& __v) { return false; }
}, parser_kind(parser));
}
Parser parser_skip_semi(Parser parser) noexcept{
return std::visit(overloaded{[&](const Semicolon& semicolon) { return parser_advance(parser); },
[&](auto& __v) { return parser; }
}, parser_kind(parser));
}
bool is_keyword_fn(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFn& kFn) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_type(TKind kind) noexcept{
return std::visit(overloaded{[&](const KType& kType) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_let(TKind kind) noexcept{
return std::visit(overloaded{[&](const KLet& kLet) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_mut(TKind kind) noexcept{
return std::visit(overloaded{[&](const KMut& kMut) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_const(TKind kind) noexcept{
return std::visit(overloaded{[&](const KConst& kConst) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_if(TKind kind) noexcept{
return std::visit(overloaded{[&](const KIf& kIf) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_unless(TKind kind) noexcept{
return std::visit(overloaded{[&](const KUnless& kUnless) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_else(TKind kind) noexcept{
return std::visit(overloaded{[&](const KElse& kElse) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_end(TKind kind) noexcept{
return std::visit(overloaded{[&](const KEnd& kEnd) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_do(TKind kind) noexcept{
return std::visit(overloaded{[&](const KDo& kDo) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_while(TKind kind) noexcept{
return std::visit(overloaded{[&](const KWhile& kWhile) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_for(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFor& kFor) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_match(TKind kind) noexcept{
return std::visit(overloaded{[&](const KMatch& kMatch) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_return(TKind kind) noexcept{
return std::visit(overloaded{[&](const KReturn& kReturn) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_break(TKind kind) noexcept{
return std::visit(overloaded{[&](const KBreak& kBreak) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_continue(TKind kind) noexcept{
return std::visit(overloaded{[&](const KContinue& kContinue) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_true(TKind kind) noexcept{
return std::visit(overloaded{[&](const KTrue& kTrue) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_false(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFalse& kFalse) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_extern(TKind kind) noexcept{
return std::visit(overloaded{[&](const KExtern& kExtern) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_extend(TKind kind) noexcept{
return std::visit(overloaded{[&](const KExtend& kExtend) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_ident(TKind kind) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [__0] = ident; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_int_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [__0] = lInt; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_str_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [__0] = lStr; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_equal(TKind kind) noexcept{
return std::visit(overloaded{[&](const Equal& equal) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_bar(TKind kind) noexcept{
return std::visit(overloaded{[&](const Bar& bar) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_pipe(TKind kind) noexcept{
return std::visit(overloaded{[&](const Pipe& pipe) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_colon(TKind kind) noexcept{
return std::visit(overloaded{[&](const Colon& colon) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_comma(TKind kind) noexcept{
return std::visit(overloaded{[&](const Comma& comma) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_dot(TKind kind) noexcept{
return std::visit(overloaded{[&](const Dot& dot) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_spread(TKind kind) noexcept{
return std::visit(overloaded{[&](const Spread& spread) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_arrow(TKind kind) noexcept{
return std::visit(overloaded{[&](const Arrow& arrow) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_fat_arrow(TKind kind) noexcept{
return std::visit(overloaded{[&](const FatArrow& fatArrow) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_question(TKind kind) noexcept{
return std::visit(overloaded{[&](const Question& question) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_paren(TKind kind) noexcept{
return std::visit(overloaded{[&](const LParen& lParen) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_paren(TKind kind) noexcept{
return std::visit(overloaded{[&](const RParen& rParen) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_brace(TKind kind) noexcept{
return std::visit(overloaded{[&](const LBrace& lBrace) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_brace(TKind kind) noexcept{
return std::visit(overloaded{[&](const RBrace& rBrace) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_bracket(TKind kind) noexcept{
return std::visit(overloaded{[&](const LBracket& lBracket) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_bracket(TKind kind) noexcept{
return std::visit(overloaded{[&](const RBracket& rBracket) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_operator(TKind kind) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [__0] = op; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
mlc::String get_ident(TKind kind) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [name] = ident; } { return name; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
int get_int_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [n] = lInt; } { return n; },
[&](auto& __v) { return 0; }
}, kind);
}
mlc::String get_str_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [s] = lStr; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
mlc::String get_operator(TKind kind) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [s] = op; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
bool is_ctor_name(mlc::String name) noexcept{
return ((name.char_at(0) >= mlc::String("A")) && (name.char_at(0) <= mlc::String("Z")));
}
bool TKind_is_fn(TKind self) noexcept{
return std::visit(overloaded{[&](const KFn& kFn) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_type(TKind self) noexcept{
return std::visit(overloaded{[&](const KType& kType) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_let(TKind self) noexcept{
return std::visit(overloaded{[&](const KLet& kLet) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_mut(TKind self) noexcept{
return std::visit(overloaded{[&](const KMut& kMut) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_const(TKind self) noexcept{
return std::visit(overloaded{[&](const KConst& kConst) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_if(TKind self) noexcept{
return std::visit(overloaded{[&](const KIf& kIf) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_unless(TKind self) noexcept{
return std::visit(overloaded{[&](const KUnless& kUnless) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_else(TKind self) noexcept{
return std::visit(overloaded{[&](const KElse& kElse) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_end(TKind self) noexcept{
return std::visit(overloaded{[&](const KEnd& kEnd) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_do(TKind self) noexcept{
return std::visit(overloaded{[&](const KDo& kDo) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_while(TKind self) noexcept{
return std::visit(overloaded{[&](const KWhile& kWhile) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_for(TKind self) noexcept{
return std::visit(overloaded{[&](const KFor& kFor) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_match(TKind self) noexcept{
return std::visit(overloaded{[&](const KMatch& kMatch) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_return(TKind self) noexcept{
return std::visit(overloaded{[&](const KReturn& kReturn) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_break(TKind self) noexcept{
return std::visit(overloaded{[&](const KBreak& kBreak) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_continue(TKind self) noexcept{
return std::visit(overloaded{[&](const KContinue& kContinue) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_true(TKind self) noexcept{
return std::visit(overloaded{[&](const KTrue& kTrue) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_false(TKind self) noexcept{
return std::visit(overloaded{[&](const KFalse& kFalse) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_extern(TKind self) noexcept{
return std::visit(overloaded{[&](const KExtern& kExtern) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_extend(TKind self) noexcept{
return std::visit(overloaded{[&](const KExtend& kExtend) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_import(TKind self) noexcept{
return std::visit(overloaded{[&](const KImport& kImport) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_from(TKind self) noexcept{
return std::visit(overloaded{[&](const KFrom& kFrom) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_eof(TKind self) noexcept{
return std::visit(overloaded{[&](const Eof& eof) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_ident(TKind self) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [__0] = ident; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_int(TKind self) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [__0] = lInt; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_str(TKind self) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [__0] = lStr; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_equal(TKind self) noexcept{
return std::visit(overloaded{[&](const Equal& equal) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_bar(TKind self) noexcept{
return std::visit(overloaded{[&](const Bar& bar) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_pipe(TKind self) noexcept{
return std::visit(overloaded{[&](const Pipe& pipe) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_colon(TKind self) noexcept{
return std::visit(overloaded{[&](const Colon& colon) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_comma(TKind self) noexcept{
return std::visit(overloaded{[&](const Comma& comma) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_dot(TKind self) noexcept{
return std::visit(overloaded{[&](const Dot& dot) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_spread(TKind self) noexcept{
return std::visit(overloaded{[&](const Spread& spread) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_arrow(TKind self) noexcept{
return std::visit(overloaded{[&](const Arrow& arrow) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_fat_arrow(TKind self) noexcept{
return std::visit(overloaded{[&](const FatArrow& fatArrow) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_question(TKind self) noexcept{
return std::visit(overloaded{[&](const Question& question) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lparen(TKind self) noexcept{
return std::visit(overloaded{[&](const LParen& lParen) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rparen(TKind self) noexcept{
return std::visit(overloaded{[&](const RParen& rParen) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lbrace(TKind self) noexcept{
return std::visit(overloaded{[&](const LBrace& lBrace) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rbrace(TKind self) noexcept{
return std::visit(overloaded{[&](const RBrace& rBrace) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lbracket(TKind self) noexcept{
return std::visit(overloaded{[&](const LBracket& lBracket) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rbracket(TKind self) noexcept{
return std::visit(overloaded{[&](const RBracket& rBracket) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_op(TKind self) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [__0] = op; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
mlc::String TKind_ident(TKind self) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [name] = ident; } { return name; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
int TKind_int_val(TKind self) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [n] = lInt; } { return n; },
[&](auto& __v) { return 0; }
}, self);
}
mlc::String TKind_str_val(TKind self) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [s] = lStr; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
mlc::String TKind_op_val(TKind self) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [s] = op; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
TKind Parser_kind(Parser self) noexcept{
return tokens[pos].kind;
}
Parser Parser_advance(Parser self) noexcept{
return Parser{tokens, (pos + 1)};
}
Parser Parser_advance_by(Parser self, int count) noexcept{
return Parser{tokens, (pos + count)};
}
bool Parser_at_eof(Parser self) noexcept{
return tokens[pos].kind.is_eof();
}
Parser Parser_skip_semi(Parser self) noexcept{
return std::visit(overloaded{[&](const Semicolon& semicolon) { return self.advance(); },
[&](auto& __v) { return self; }
}, tokens[pos].kind);
}
TypeResult parse_type(Parser parser) noexcept{
auto base = parse_base_type(parser);
return (base.parser.kind().is_arrow() ? [&]() {
auto ret = parse_type(base.parser.advance());
auto params = mlc::Array<auto>{base.type_expr};
return TypeResult{Shared.new(TyFn(params, ret.type_expr)), ret.parser};
}() : base);
}
TypeResult parse_base_type(Parser parser) noexcept{
auto kind = parser.kind();
return (kind.is_lbracket() ? [&]() {
auto inner = parse_type(parser.advance());
return TypeResult{Shared.new(TyArray(inner.type_expr)), inner.parser.advance()};
}() : (kind.is_ident() ? [&]() {
auto name = kind.ident();
return ((name == mlc::String("i32")) ? TypeResult{Shared.new(TyI32{}), parser.advance()} : ((name == mlc::String("string")) ? TypeResult{Shared.new(TyString{}), parser.advance()} : ((name == mlc::String("bool")) ? TypeResult{Shared.new(TyBool{}), parser.advance()} : ((name == mlc::String("unit")) ? TypeResult{Shared.new(TyUnit{}), parser.advance()} : [&]() {
auto after = parser.advance();
return ((after.kind().is_op() && (after.kind().op_val() == mlc::String("<"))) ? [&]() {
auto args = parse_type_args(after.advance());
auto type_expr = (((name == mlc::String("Shared")) && (mlc::io::args.types.length() == 1)) ? Shared.new(TyShared(mlc::io::args.types[0])) : Shared.new(TyGeneric(name, mlc::io::args.types)));
return TypeResult{type_expr, mlc::io::args.parser};
}() : TypeResult{Shared.new(TyNamed(name)), after});
}()))));
}() : TypeResult{Shared.new(TyUnit{}), parser.advance_by(2)}));
}
TypesResult parse_type_args(Parser parser) noexcept{
auto types = mlc::Array<std::shared_ptr<TypeExpr>>{};
auto first = parse_type(parser);
types.push_back(first.type_expr);
auto state = first.parser;
while (state.kind().is_comma()) {
auto next = parse_type(state.advance());
types.push_back(next.type_expr);
state = next.parser;
}
auto end_state = ((state.kind().is_op() && (state.kind().op_val() == mlc::String(">"))) ? state.advance() : state);
return TypesResult{types, end_state};
}
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept{
return export;
}
bool LexOut_has_errors(LexOut self) noexcept{
return (errors.length() > 0);
}
TKind Token_kind_value(Token self) noexcept{
return kind;
}
int Token_line_number(Token self) noexcept{
return line;
}
int Token_column(Token self) noexcept{
return col;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [d] = declExported; } { return d; },
[&](auto& __v) { return decl; }
}, decl);
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; } { return name; },
[&](const DeclType& declType) { auto [name, __1] = declType; } { return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1] = declExtend; } { return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return decl_name(d); }
}, decl);
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
auto i = 0;
while ((i < src.length())) {
dst.push_back(src[i]);
i = (i + 1);
}
return dst;
}
Parser parser_new(mlc::Array<Token> tokens) noexcept{
return Parser{tokens, 0};
}
TKind parser_kind(Parser parser) noexcept{
return parser.tokens[parser.pos].kind;
}
Parser parser_advance(Parser parser) noexcept{
return Parser{parser.tokens, (parser.pos + 1)};
}
Parser parser_advance_by(Parser parser, int count) noexcept{
return Parser{parser.tokens, (parser.pos + count)};
}
bool parser_at_eof(Parser parser) noexcept{
return std::visit(overloaded{[&](const Eof& eof) { return true; },
[&](auto& __v) { return false; }
}, parser_kind(parser));
}
Parser parser_skip_semi(Parser parser) noexcept{
return std::visit(overloaded{[&](const Semicolon& semicolon) { return parser_advance(parser); },
[&](auto& __v) { return parser; }
}, parser_kind(parser));
}
bool is_keyword_fn(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFn& kFn) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_type(TKind kind) noexcept{
return std::visit(overloaded{[&](const KType& kType) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_let(TKind kind) noexcept{
return std::visit(overloaded{[&](const KLet& kLet) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_mut(TKind kind) noexcept{
return std::visit(overloaded{[&](const KMut& kMut) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_const(TKind kind) noexcept{
return std::visit(overloaded{[&](const KConst& kConst) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_if(TKind kind) noexcept{
return std::visit(overloaded{[&](const KIf& kIf) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_unless(TKind kind) noexcept{
return std::visit(overloaded{[&](const KUnless& kUnless) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_else(TKind kind) noexcept{
return std::visit(overloaded{[&](const KElse& kElse) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_end(TKind kind) noexcept{
return std::visit(overloaded{[&](const KEnd& kEnd) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_do(TKind kind) noexcept{
return std::visit(overloaded{[&](const KDo& kDo) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_while(TKind kind) noexcept{
return std::visit(overloaded{[&](const KWhile& kWhile) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_for(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFor& kFor) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_match(TKind kind) noexcept{
return std::visit(overloaded{[&](const KMatch& kMatch) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_return(TKind kind) noexcept{
return std::visit(overloaded{[&](const KReturn& kReturn) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_break(TKind kind) noexcept{
return std::visit(overloaded{[&](const KBreak& kBreak) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_continue(TKind kind) noexcept{
return std::visit(overloaded{[&](const KContinue& kContinue) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_true(TKind kind) noexcept{
return std::visit(overloaded{[&](const KTrue& kTrue) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_false(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFalse& kFalse) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_extern(TKind kind) noexcept{
return std::visit(overloaded{[&](const KExtern& kExtern) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_extend(TKind kind) noexcept{
return std::visit(overloaded{[&](const KExtend& kExtend) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_ident(TKind kind) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [__0] = ident; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_int_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [__0] = lInt; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_str_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [__0] = lStr; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_equal(TKind kind) noexcept{
return std::visit(overloaded{[&](const Equal& equal) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_bar(TKind kind) noexcept{
return std::visit(overloaded{[&](const Bar& bar) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_pipe(TKind kind) noexcept{
return std::visit(overloaded{[&](const Pipe& pipe) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_colon(TKind kind) noexcept{
return std::visit(overloaded{[&](const Colon& colon) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_comma(TKind kind) noexcept{
return std::visit(overloaded{[&](const Comma& comma) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_dot(TKind kind) noexcept{
return std::visit(overloaded{[&](const Dot& dot) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_spread(TKind kind) noexcept{
return std::visit(overloaded{[&](const Spread& spread) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_arrow(TKind kind) noexcept{
return std::visit(overloaded{[&](const Arrow& arrow) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_fat_arrow(TKind kind) noexcept{
return std::visit(overloaded{[&](const FatArrow& fatArrow) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_question(TKind kind) noexcept{
return std::visit(overloaded{[&](const Question& question) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_paren(TKind kind) noexcept{
return std::visit(overloaded{[&](const LParen& lParen) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_paren(TKind kind) noexcept{
return std::visit(overloaded{[&](const RParen& rParen) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_brace(TKind kind) noexcept{
return std::visit(overloaded{[&](const LBrace& lBrace) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_brace(TKind kind) noexcept{
return std::visit(overloaded{[&](const RBrace& rBrace) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_bracket(TKind kind) noexcept{
return std::visit(overloaded{[&](const LBracket& lBracket) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_bracket(TKind kind) noexcept{
return std::visit(overloaded{[&](const RBracket& rBracket) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_operator(TKind kind) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [__0] = op; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
mlc::String get_ident(TKind kind) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [name] = ident; } { return name; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
int get_int_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [n] = lInt; } { return n; },
[&](auto& __v) { return 0; }
}, kind);
}
mlc::String get_str_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [s] = lStr; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
mlc::String get_operator(TKind kind) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [s] = op; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
bool is_ctor_name(mlc::String name) noexcept{
return ((name.char_at(0) >= mlc::String("A")) && (name.char_at(0) <= mlc::String("Z")));
}
bool TKind_is_fn(TKind self) noexcept{
return std::visit(overloaded{[&](const KFn& kFn) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_type(TKind self) noexcept{
return std::visit(overloaded{[&](const KType& kType) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_let(TKind self) noexcept{
return std::visit(overloaded{[&](const KLet& kLet) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_mut(TKind self) noexcept{
return std::visit(overloaded{[&](const KMut& kMut) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_const(TKind self) noexcept{
return std::visit(overloaded{[&](const KConst& kConst) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_if(TKind self) noexcept{
return std::visit(overloaded{[&](const KIf& kIf) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_unless(TKind self) noexcept{
return std::visit(overloaded{[&](const KUnless& kUnless) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_else(TKind self) noexcept{
return std::visit(overloaded{[&](const KElse& kElse) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_end(TKind self) noexcept{
return std::visit(overloaded{[&](const KEnd& kEnd) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_do(TKind self) noexcept{
return std::visit(overloaded{[&](const KDo& kDo) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_while(TKind self) noexcept{
return std::visit(overloaded{[&](const KWhile& kWhile) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_for(TKind self) noexcept{
return std::visit(overloaded{[&](const KFor& kFor) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_match(TKind self) noexcept{
return std::visit(overloaded{[&](const KMatch& kMatch) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_return(TKind self) noexcept{
return std::visit(overloaded{[&](const KReturn& kReturn) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_break(TKind self) noexcept{
return std::visit(overloaded{[&](const KBreak& kBreak) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_continue(TKind self) noexcept{
return std::visit(overloaded{[&](const KContinue& kContinue) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_true(TKind self) noexcept{
return std::visit(overloaded{[&](const KTrue& kTrue) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_false(TKind self) noexcept{
return std::visit(overloaded{[&](const KFalse& kFalse) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_extern(TKind self) noexcept{
return std::visit(overloaded{[&](const KExtern& kExtern) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_extend(TKind self) noexcept{
return std::visit(overloaded{[&](const KExtend& kExtend) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_import(TKind self) noexcept{
return std::visit(overloaded{[&](const KImport& kImport) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_from(TKind self) noexcept{
return std::visit(overloaded{[&](const KFrom& kFrom) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_eof(TKind self) noexcept{
return std::visit(overloaded{[&](const Eof& eof) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_ident(TKind self) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [__0] = ident; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_int(TKind self) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [__0] = lInt; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_str(TKind self) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [__0] = lStr; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_equal(TKind self) noexcept{
return std::visit(overloaded{[&](const Equal& equal) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_bar(TKind self) noexcept{
return std::visit(overloaded{[&](const Bar& bar) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_pipe(TKind self) noexcept{
return std::visit(overloaded{[&](const Pipe& pipe) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_colon(TKind self) noexcept{
return std::visit(overloaded{[&](const Colon& colon) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_comma(TKind self) noexcept{
return std::visit(overloaded{[&](const Comma& comma) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_dot(TKind self) noexcept{
return std::visit(overloaded{[&](const Dot& dot) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_spread(TKind self) noexcept{
return std::visit(overloaded{[&](const Spread& spread) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_arrow(TKind self) noexcept{
return std::visit(overloaded{[&](const Arrow& arrow) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_fat_arrow(TKind self) noexcept{
return std::visit(overloaded{[&](const FatArrow& fatArrow) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_question(TKind self) noexcept{
return std::visit(overloaded{[&](const Question& question) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lparen(TKind self) noexcept{
return std::visit(overloaded{[&](const LParen& lParen) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rparen(TKind self) noexcept{
return std::visit(overloaded{[&](const RParen& rParen) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lbrace(TKind self) noexcept{
return std::visit(overloaded{[&](const LBrace& lBrace) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rbrace(TKind self) noexcept{
return std::visit(overloaded{[&](const RBrace& rBrace) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lbracket(TKind self) noexcept{
return std::visit(overloaded{[&](const LBracket& lBracket) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rbracket(TKind self) noexcept{
return std::visit(overloaded{[&](const RBracket& rBracket) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_op(TKind self) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [__0] = op; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
mlc::String TKind_ident(TKind self) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [name] = ident; } { return name; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
int TKind_int_val(TKind self) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [n] = lInt; } { return n; },
[&](auto& __v) { return 0; }
}, self);
}
mlc::String TKind_str_val(TKind self) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [s] = lStr; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
mlc::String TKind_op_val(TKind self) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [s] = op; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
TKind Parser_kind(Parser self) noexcept{
return tokens[pos].kind;
}
Parser Parser_advance(Parser self) noexcept{
return Parser{tokens, (pos + 1)};
}
Parser Parser_advance_by(Parser self, int count) noexcept{
return Parser{tokens, (pos + count)};
}
bool Parser_at_eof(Parser self) noexcept{
return tokens[pos].kind.is_eof();
}
Parser Parser_skip_semi(Parser self) noexcept{
return std::visit(overloaded{[&](const Semicolon& semicolon) { return self.advance(); },
[&](auto& __v) { return self; }
}, tokens[pos].kind);
}
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept{
return export;
}
bool LexOut_has_errors(LexOut self) noexcept{
return (errors.length() > 0);
}
TKind Token_kind_value(Token self) noexcept{
return kind;
}
int Token_line_number(Token self) noexcept{
return line;
}
int Token_column(Token self) noexcept{
return col;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [d] = declExported; } { return d; },
[&](auto& __v) { return decl; }
}, decl);
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; } { return name; },
[&](const DeclType& declType) { auto [name, __1] = declType; } { return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1] = declExtend; } { return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return decl_name(d); }
}, decl);
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
auto i = 0;
while ((i < src.length())) {
dst.push_back(src[i]);
i = (i + 1);
}
return dst;
}
Parser parser_new(mlc::Array<Token> tokens) noexcept{
return Parser{tokens, 0};
}
TKind parser_kind(Parser parser) noexcept{
return parser.tokens[parser.pos].kind;
}
Parser parser_advance(Parser parser) noexcept{
return Parser{parser.tokens, (parser.pos + 1)};
}
Parser parser_advance_by(Parser parser, int count) noexcept{
return Parser{parser.tokens, (parser.pos + count)};
}
bool parser_at_eof(Parser parser) noexcept{
return std::visit(overloaded{[&](const Eof& eof) { return true; },
[&](auto& __v) { return false; }
}, parser_kind(parser));
}
Parser parser_skip_semi(Parser parser) noexcept{
return std::visit(overloaded{[&](const Semicolon& semicolon) { return parser_advance(parser); },
[&](auto& __v) { return parser; }
}, parser_kind(parser));
}
bool is_keyword_fn(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFn& kFn) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_type(TKind kind) noexcept{
return std::visit(overloaded{[&](const KType& kType) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_let(TKind kind) noexcept{
return std::visit(overloaded{[&](const KLet& kLet) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_mut(TKind kind) noexcept{
return std::visit(overloaded{[&](const KMut& kMut) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_const(TKind kind) noexcept{
return std::visit(overloaded{[&](const KConst& kConst) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_if(TKind kind) noexcept{
return std::visit(overloaded{[&](const KIf& kIf) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_unless(TKind kind) noexcept{
return std::visit(overloaded{[&](const KUnless& kUnless) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_else(TKind kind) noexcept{
return std::visit(overloaded{[&](const KElse& kElse) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_end(TKind kind) noexcept{
return std::visit(overloaded{[&](const KEnd& kEnd) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_do(TKind kind) noexcept{
return std::visit(overloaded{[&](const KDo& kDo) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_while(TKind kind) noexcept{
return std::visit(overloaded{[&](const KWhile& kWhile) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_for(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFor& kFor) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_match(TKind kind) noexcept{
return std::visit(overloaded{[&](const KMatch& kMatch) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_return(TKind kind) noexcept{
return std::visit(overloaded{[&](const KReturn& kReturn) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_break(TKind kind) noexcept{
return std::visit(overloaded{[&](const KBreak& kBreak) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_continue(TKind kind) noexcept{
return std::visit(overloaded{[&](const KContinue& kContinue) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_true(TKind kind) noexcept{
return std::visit(overloaded{[&](const KTrue& kTrue) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_false(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFalse& kFalse) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_extern(TKind kind) noexcept{
return std::visit(overloaded{[&](const KExtern& kExtern) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_extend(TKind kind) noexcept{
return std::visit(overloaded{[&](const KExtend& kExtend) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_ident(TKind kind) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [__0] = ident; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_int_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [__0] = lInt; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_str_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [__0] = lStr; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_equal(TKind kind) noexcept{
return std::visit(overloaded{[&](const Equal& equal) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_bar(TKind kind) noexcept{
return std::visit(overloaded{[&](const Bar& bar) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_pipe(TKind kind) noexcept{
return std::visit(overloaded{[&](const Pipe& pipe) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_colon(TKind kind) noexcept{
return std::visit(overloaded{[&](const Colon& colon) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_comma(TKind kind) noexcept{
return std::visit(overloaded{[&](const Comma& comma) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_dot(TKind kind) noexcept{
return std::visit(overloaded{[&](const Dot& dot) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_spread(TKind kind) noexcept{
return std::visit(overloaded{[&](const Spread& spread) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_arrow(TKind kind) noexcept{
return std::visit(overloaded{[&](const Arrow& arrow) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_fat_arrow(TKind kind) noexcept{
return std::visit(overloaded{[&](const FatArrow& fatArrow) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_question(TKind kind) noexcept{
return std::visit(overloaded{[&](const Question& question) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_paren(TKind kind) noexcept{
return std::visit(overloaded{[&](const LParen& lParen) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_paren(TKind kind) noexcept{
return std::visit(overloaded{[&](const RParen& rParen) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_brace(TKind kind) noexcept{
return std::visit(overloaded{[&](const LBrace& lBrace) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_brace(TKind kind) noexcept{
return std::visit(overloaded{[&](const RBrace& rBrace) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_bracket(TKind kind) noexcept{
return std::visit(overloaded{[&](const LBracket& lBracket) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_bracket(TKind kind) noexcept{
return std::visit(overloaded{[&](const RBracket& rBracket) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_operator(TKind kind) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [__0] = op; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
mlc::String get_ident(TKind kind) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [name] = ident; } { return name; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
int get_int_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [n] = lInt; } { return n; },
[&](auto& __v) { return 0; }
}, kind);
}
mlc::String get_str_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [s] = lStr; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
mlc::String get_operator(TKind kind) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [s] = op; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
bool is_ctor_name(mlc::String name) noexcept{
return ((name.char_at(0) >= mlc::String("A")) && (name.char_at(0) <= mlc::String("Z")));
}
bool TKind_is_fn(TKind self) noexcept{
return std::visit(overloaded{[&](const KFn& kFn) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_type(TKind self) noexcept{
return std::visit(overloaded{[&](const KType& kType) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_let(TKind self) noexcept{
return std::visit(overloaded{[&](const KLet& kLet) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_mut(TKind self) noexcept{
return std::visit(overloaded{[&](const KMut& kMut) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_const(TKind self) noexcept{
return std::visit(overloaded{[&](const KConst& kConst) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_if(TKind self) noexcept{
return std::visit(overloaded{[&](const KIf& kIf) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_unless(TKind self) noexcept{
return std::visit(overloaded{[&](const KUnless& kUnless) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_else(TKind self) noexcept{
return std::visit(overloaded{[&](const KElse& kElse) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_end(TKind self) noexcept{
return std::visit(overloaded{[&](const KEnd& kEnd) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_do(TKind self) noexcept{
return std::visit(overloaded{[&](const KDo& kDo) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_while(TKind self) noexcept{
return std::visit(overloaded{[&](const KWhile& kWhile) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_for(TKind self) noexcept{
return std::visit(overloaded{[&](const KFor& kFor) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_match(TKind self) noexcept{
return std::visit(overloaded{[&](const KMatch& kMatch) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_return(TKind self) noexcept{
return std::visit(overloaded{[&](const KReturn& kReturn) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_break(TKind self) noexcept{
return std::visit(overloaded{[&](const KBreak& kBreak) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_continue(TKind self) noexcept{
return std::visit(overloaded{[&](const KContinue& kContinue) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_true(TKind self) noexcept{
return std::visit(overloaded{[&](const KTrue& kTrue) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_false(TKind self) noexcept{
return std::visit(overloaded{[&](const KFalse& kFalse) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_extern(TKind self) noexcept{
return std::visit(overloaded{[&](const KExtern& kExtern) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_extend(TKind self) noexcept{
return std::visit(overloaded{[&](const KExtend& kExtend) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_import(TKind self) noexcept{
return std::visit(overloaded{[&](const KImport& kImport) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_from(TKind self) noexcept{
return std::visit(overloaded{[&](const KFrom& kFrom) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_eof(TKind self) noexcept{
return std::visit(overloaded{[&](const Eof& eof) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_ident(TKind self) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [__0] = ident; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_int(TKind self) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [__0] = lInt; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_str(TKind self) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [__0] = lStr; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_equal(TKind self) noexcept{
return std::visit(overloaded{[&](const Equal& equal) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_bar(TKind self) noexcept{
return std::visit(overloaded{[&](const Bar& bar) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_pipe(TKind self) noexcept{
return std::visit(overloaded{[&](const Pipe& pipe) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_colon(TKind self) noexcept{
return std::visit(overloaded{[&](const Colon& colon) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_comma(TKind self) noexcept{
return std::visit(overloaded{[&](const Comma& comma) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_dot(TKind self) noexcept{
return std::visit(overloaded{[&](const Dot& dot) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_spread(TKind self) noexcept{
return std::visit(overloaded{[&](const Spread& spread) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_arrow(TKind self) noexcept{
return std::visit(overloaded{[&](const Arrow& arrow) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_fat_arrow(TKind self) noexcept{
return std::visit(overloaded{[&](const FatArrow& fatArrow) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_question(TKind self) noexcept{
return std::visit(overloaded{[&](const Question& question) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lparen(TKind self) noexcept{
return std::visit(overloaded{[&](const LParen& lParen) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rparen(TKind self) noexcept{
return std::visit(overloaded{[&](const RParen& rParen) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lbrace(TKind self) noexcept{
return std::visit(overloaded{[&](const LBrace& lBrace) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rbrace(TKind self) noexcept{
return std::visit(overloaded{[&](const RBrace& rBrace) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lbracket(TKind self) noexcept{
return std::visit(overloaded{[&](const LBracket& lBracket) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rbracket(TKind self) noexcept{
return std::visit(overloaded{[&](const RBracket& rBracket) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_op(TKind self) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [__0] = op; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
mlc::String TKind_ident(TKind self) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [name] = ident; } { return name; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
int TKind_int_val(TKind self) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [n] = lInt; } { return n; },
[&](auto& __v) { return 0; }
}, self);
}
mlc::String TKind_str_val(TKind self) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [s] = lStr; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
mlc::String TKind_op_val(TKind self) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [s] = op; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
TKind Parser_kind(Parser self) noexcept{
return tokens[pos].kind;
}
Parser Parser_advance(Parser self) noexcept{
return Parser{tokens, (pos + 1)};
}
Parser Parser_advance_by(Parser self, int count) noexcept{
return Parser{tokens, (pos + count)};
}
bool Parser_at_eof(Parser self) noexcept{
return tokens[pos].kind.is_eof();
}
Parser Parser_skip_semi(Parser self) noexcept{
return std::visit(overloaded{[&](const Semicolon& semicolon) { return self.advance(); },
[&](auto& __v) { return self; }
}, tokens[pos].kind);
}
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept{
return export;
}
bool LexOut_has_errors(LexOut self) noexcept{
return (errors.length() > 0);
}
TKind Token_kind_value(Token self) noexcept{
return kind;
}
int Token_line_number(Token self) noexcept{
return line;
}
int Token_column(Token self) noexcept{
return col;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [d] = declExported; } { return d; },
[&](auto& __v) { return decl; }
}, decl);
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; } { return name; },
[&](const DeclType& declType) { auto [name, __1] = declType; } { return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1] = declExtend; } { return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return decl_name(d); }
}, decl);
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
auto i = 0;
while ((i < src.length())) {
dst.push_back(src[i]);
i = (i + 1);
}
return dst;
}
Parser parser_new(mlc::Array<Token> tokens) noexcept{
return Parser{tokens, 0};
}
TKind parser_kind(Parser parser) noexcept{
return parser.tokens[parser.pos].kind;
}
Parser parser_advance(Parser parser) noexcept{
return Parser{parser.tokens, (parser.pos + 1)};
}
Parser parser_advance_by(Parser parser, int count) noexcept{
return Parser{parser.tokens, (parser.pos + count)};
}
bool parser_at_eof(Parser parser) noexcept{
return std::visit(overloaded{[&](const Eof& eof) { return true; },
[&](auto& __v) { return false; }
}, parser_kind(parser));
}
Parser parser_skip_semi(Parser parser) noexcept{
return std::visit(overloaded{[&](const Semicolon& semicolon) { return parser_advance(parser); },
[&](auto& __v) { return parser; }
}, parser_kind(parser));
}
bool is_keyword_fn(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFn& kFn) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_type(TKind kind) noexcept{
return std::visit(overloaded{[&](const KType& kType) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_let(TKind kind) noexcept{
return std::visit(overloaded{[&](const KLet& kLet) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_mut(TKind kind) noexcept{
return std::visit(overloaded{[&](const KMut& kMut) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_const(TKind kind) noexcept{
return std::visit(overloaded{[&](const KConst& kConst) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_if(TKind kind) noexcept{
return std::visit(overloaded{[&](const KIf& kIf) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_unless(TKind kind) noexcept{
return std::visit(overloaded{[&](const KUnless& kUnless) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_else(TKind kind) noexcept{
return std::visit(overloaded{[&](const KElse& kElse) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_end(TKind kind) noexcept{
return std::visit(overloaded{[&](const KEnd& kEnd) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_do(TKind kind) noexcept{
return std::visit(overloaded{[&](const KDo& kDo) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_while(TKind kind) noexcept{
return std::visit(overloaded{[&](const KWhile& kWhile) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_for(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFor& kFor) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_match(TKind kind) noexcept{
return std::visit(overloaded{[&](const KMatch& kMatch) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_return(TKind kind) noexcept{
return std::visit(overloaded{[&](const KReturn& kReturn) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_break(TKind kind) noexcept{
return std::visit(overloaded{[&](const KBreak& kBreak) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_continue(TKind kind) noexcept{
return std::visit(overloaded{[&](const KContinue& kContinue) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_true(TKind kind) noexcept{
return std::visit(overloaded{[&](const KTrue& kTrue) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_false(TKind kind) noexcept{
return std::visit(overloaded{[&](const KFalse& kFalse) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_extern(TKind kind) noexcept{
return std::visit(overloaded{[&](const KExtern& kExtern) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_keyword_extend(TKind kind) noexcept{
return std::visit(overloaded{[&](const KExtend& kExtend) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_ident(TKind kind) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [__0] = ident; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_int_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [__0] = lInt; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_str_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [__0] = lStr; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_equal(TKind kind) noexcept{
return std::visit(overloaded{[&](const Equal& equal) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_bar(TKind kind) noexcept{
return std::visit(overloaded{[&](const Bar& bar) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_pipe(TKind kind) noexcept{
return std::visit(overloaded{[&](const Pipe& pipe) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_colon(TKind kind) noexcept{
return std::visit(overloaded{[&](const Colon& colon) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_comma(TKind kind) noexcept{
return std::visit(overloaded{[&](const Comma& comma) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_dot(TKind kind) noexcept{
return std::visit(overloaded{[&](const Dot& dot) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_spread(TKind kind) noexcept{
return std::visit(overloaded{[&](const Spread& spread) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_arrow(TKind kind) noexcept{
return std::visit(overloaded{[&](const Arrow& arrow) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_fat_arrow(TKind kind) noexcept{
return std::visit(overloaded{[&](const FatArrow& fatArrow) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_question(TKind kind) noexcept{
return std::visit(overloaded{[&](const Question& question) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_paren(TKind kind) noexcept{
return std::visit(overloaded{[&](const LParen& lParen) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_paren(TKind kind) noexcept{
return std::visit(overloaded{[&](const RParen& rParen) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_brace(TKind kind) noexcept{
return std::visit(overloaded{[&](const LBrace& lBrace) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_brace(TKind kind) noexcept{
return std::visit(overloaded{[&](const RBrace& rBrace) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_left_bracket(TKind kind) noexcept{
return std::visit(overloaded{[&](const LBracket& lBracket) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_right_bracket(TKind kind) noexcept{
return std::visit(overloaded{[&](const RBracket& rBracket) { return true; },
[&](auto& __v) { return false; }
}, kind);
}
bool is_operator(TKind kind) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [__0] = op; } { return true; },
[&](auto& __v) { return false; }
}, kind);
}
mlc::String get_ident(TKind kind) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [name] = ident; } { return name; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
int get_int_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [n] = lInt; } { return n; },
[&](auto& __v) { return 0; }
}, kind);
}
mlc::String get_str_literal(TKind kind) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [s] = lStr; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
mlc::String get_operator(TKind kind) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [s] = op; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, kind);
}
bool is_ctor_name(mlc::String name) noexcept{
return ((name.char_at(0) >= mlc::String("A")) && (name.char_at(0) <= mlc::String("Z")));
}
bool TKind_is_fn(TKind self) noexcept{
return std::visit(overloaded{[&](const KFn& kFn) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_type(TKind self) noexcept{
return std::visit(overloaded{[&](const KType& kType) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_let(TKind self) noexcept{
return std::visit(overloaded{[&](const KLet& kLet) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_mut(TKind self) noexcept{
return std::visit(overloaded{[&](const KMut& kMut) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_const(TKind self) noexcept{
return std::visit(overloaded{[&](const KConst& kConst) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_if(TKind self) noexcept{
return std::visit(overloaded{[&](const KIf& kIf) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_unless(TKind self) noexcept{
return std::visit(overloaded{[&](const KUnless& kUnless) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_else(TKind self) noexcept{
return std::visit(overloaded{[&](const KElse& kElse) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_end(TKind self) noexcept{
return std::visit(overloaded{[&](const KEnd& kEnd) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_do(TKind self) noexcept{
return std::visit(overloaded{[&](const KDo& kDo) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_while(TKind self) noexcept{
return std::visit(overloaded{[&](const KWhile& kWhile) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_for(TKind self) noexcept{
return std::visit(overloaded{[&](const KFor& kFor) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_match(TKind self) noexcept{
return std::visit(overloaded{[&](const KMatch& kMatch) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_return(TKind self) noexcept{
return std::visit(overloaded{[&](const KReturn& kReturn) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_break(TKind self) noexcept{
return std::visit(overloaded{[&](const KBreak& kBreak) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_continue(TKind self) noexcept{
return std::visit(overloaded{[&](const KContinue& kContinue) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_true(TKind self) noexcept{
return std::visit(overloaded{[&](const KTrue& kTrue) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_false(TKind self) noexcept{
return std::visit(overloaded{[&](const KFalse& kFalse) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_extern(TKind self) noexcept{
return std::visit(overloaded{[&](const KExtern& kExtern) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_extend(TKind self) noexcept{
return std::visit(overloaded{[&](const KExtend& kExtend) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_import(TKind self) noexcept{
return std::visit(overloaded{[&](const KImport& kImport) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_from(TKind self) noexcept{
return std::visit(overloaded{[&](const KFrom& kFrom) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_eof(TKind self) noexcept{
return std::visit(overloaded{[&](const Eof& eof) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_ident(TKind self) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [__0] = ident; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_int(TKind self) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [__0] = lInt; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_str(TKind self) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [__0] = lStr; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_equal(TKind self) noexcept{
return std::visit(overloaded{[&](const Equal& equal) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_bar(TKind self) noexcept{
return std::visit(overloaded{[&](const Bar& bar) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_pipe(TKind self) noexcept{
return std::visit(overloaded{[&](const Pipe& pipe) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_colon(TKind self) noexcept{
return std::visit(overloaded{[&](const Colon& colon) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_comma(TKind self) noexcept{
return std::visit(overloaded{[&](const Comma& comma) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_dot(TKind self) noexcept{
return std::visit(overloaded{[&](const Dot& dot) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_spread(TKind self) noexcept{
return std::visit(overloaded{[&](const Spread& spread) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_arrow(TKind self) noexcept{
return std::visit(overloaded{[&](const Arrow& arrow) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_fat_arrow(TKind self) noexcept{
return std::visit(overloaded{[&](const FatArrow& fatArrow) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_question(TKind self) noexcept{
return std::visit(overloaded{[&](const Question& question) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lparen(TKind self) noexcept{
return std::visit(overloaded{[&](const LParen& lParen) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rparen(TKind self) noexcept{
return std::visit(overloaded{[&](const RParen& rParen) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lbrace(TKind self) noexcept{
return std::visit(overloaded{[&](const LBrace& lBrace) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rbrace(TKind self) noexcept{
return std::visit(overloaded{[&](const RBrace& rBrace) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_lbracket(TKind self) noexcept{
return std::visit(overloaded{[&](const LBracket& lBracket) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_rbracket(TKind self) noexcept{
return std::visit(overloaded{[&](const RBracket& rBracket) { return true; },
[&](auto& __v) { return false; }
}, self);
}
bool TKind_is_op(TKind self) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [__0] = op; } { return true; },
[&](auto& __v) { return false; }
}, self);
}
mlc::String TKind_ident(TKind self) noexcept{
return std::visit(overloaded{[&](const Ident& ident) { auto [name] = ident; } { return name; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
int TKind_int_val(TKind self) noexcept{
return std::visit(overloaded{[&](const LInt& lInt) { auto [n] = lInt; } { return n; },
[&](auto& __v) { return 0; }
}, self);
}
mlc::String TKind_str_val(TKind self) noexcept{
return std::visit(overloaded{[&](const LStr& lStr) { auto [s] = lStr; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
mlc::String TKind_op_val(TKind self) noexcept{
return std::visit(overloaded{[&](const Op& op) { auto [s] = op; } { return s; },
[&](auto& __v) { return mlc::String(""); }
}, self);
}
TKind Parser_kind(Parser self) noexcept{
return tokens[pos].kind;
}
Parser Parser_advance(Parser self) noexcept{
return Parser{tokens, (pos + 1)};
}
Parser Parser_advance_by(Parser self, int count) noexcept{
return Parser{tokens, (pos + count)};
}
bool Parser_at_eof(Parser self) noexcept{
return tokens[pos].kind.is_eof();
}
Parser Parser_skip_semi(Parser self) noexcept{
return std::visit(overloaded{[&](const Semicolon& semicolon) { return self.advance(); },
[&](auto& __v) { return self; }
}, tokens[pos].kind);
}
TypeResult parse_type(Parser parser) noexcept{
auto base = parse_base_type(parser);
return (base.parser.kind().is_arrow() ? [&]() {
auto ret = parse_type(base.parser.advance());
auto params = mlc::Array<auto>{base.type_expr};
return TypeResult{Shared.new(TyFn(params, ret.type_expr)), ret.parser};
}() : base);
}
TypeResult parse_base_type(Parser parser) noexcept{
auto kind = parser.kind();
return (kind.is_lbracket() ? [&]() {
auto inner = parse_type(parser.advance());
return TypeResult{Shared.new(TyArray(inner.type_expr)), inner.parser.advance()};
}() : (kind.is_ident() ? [&]() {
auto name = kind.ident();
return ((name == mlc::String("i32")) ? TypeResult{Shared.new(TyI32{}), parser.advance()} : ((name == mlc::String("string")) ? TypeResult{Shared.new(TyString{}), parser.advance()} : ((name == mlc::String("bool")) ? TypeResult{Shared.new(TyBool{}), parser.advance()} : ((name == mlc::String("unit")) ? TypeResult{Shared.new(TyUnit{}), parser.advance()} : [&]() {
auto after = parser.advance();
return ((after.kind().is_op() && (after.kind().op_val() == mlc::String("<"))) ? [&]() {
auto args = parse_type_args(after.advance());
auto type_expr = (((name == mlc::String("Shared")) && (mlc::io::args.types.length() == 1)) ? Shared.new(TyShared(mlc::io::args.types[0])) : Shared.new(TyGeneric(name, mlc::io::args.types)));
return TypeResult{type_expr, mlc::io::args.parser};
}() : TypeResult{Shared.new(TyNamed(name)), after});
}()))));
}() : TypeResult{Shared.new(TyUnit{}), parser.advance_by(2)}));
}
TypesResult parse_type_args(Parser parser) noexcept{
auto types = mlc::Array<std::shared_ptr<TypeExpr>>{};
auto first = parse_type(parser);
types.push_back(first.type_expr);
auto state = first.parser;
while (state.kind().is_comma()) {
auto next = parse_type(state.advance());
types.push_back(next.type_expr);
state = next.parser;
}
auto end_state = ((state.kind().is_op() && (state.kind().op_val() == mlc::String(">"))) ? state.advance() : state);
return TypesResult{types, end_state};
}
StmtsResult parse_stmts_until_end(Parser parser) noexcept{
auto stmts = mlc::Array<std::shared_ptr<Stmt>>{};
auto state = parser;
while (((!state.kind().is_end()) && (!state.at_eof()))) {
auto result = parse_stmt(state);
stmts.push_back(result.stmt);
state = result.parser.skip_semi();
}
return StmtsResult{stmts, state.advance()};
}
StmtsResult parse_stmts_until_else_end(Parser parser) noexcept{
auto stmts = mlc::Array<std::shared_ptr<Stmt>>{};
auto state = parser;
while ((((!state.kind().is_end()) && (!state.kind().is_else())) && (!state.at_eof()))) {
auto result = parse_stmt(state);
stmts.push_back(result.stmt);
state = result.parser.skip_semi();
}
return StmtsResult{stmts, state};
}
StmtResult parse_stmt(Parser parser) noexcept{
auto kind = parser.kind();
return (kind.is_let() ? [&]() {
auto after_let = parser.advance();
auto is_const = after_let.kind().is_const();
auto name_pos = (is_const ? after_let.advance() : after_let);
auto var_name = name_pos.kind().ident();
auto type_result = (name_pos.advance().kind().is_colon() ? parse_type(name_pos.advance_by(2)) : TypeResult{Shared.new(TyUnit{}), name_pos.advance()});
auto value_result = parse_expr(type_result.parser.advance());
return StmtResult{Shared.new(StmtLet(var_name, (!is_const), type_result.type_expr, value_result.expr)), value_result.parser};
}() : (kind.is_const() ? [&]() {
auto name_pos = parser.advance();
auto var_name = name_pos.kind().ident();
auto type_result = (name_pos.advance().kind().is_colon() ? parse_type(name_pos.advance_by(2)) : TypeResult{Shared.new(TyUnit{}), name_pos.advance()});
auto value_result = parse_expr(type_result.parser.advance());
return StmtResult{Shared.new(StmtLet(var_name, false, type_result.type_expr, value_result.expr)), value_result.parser};
}() : (kind.is_break() ? StmtResult{Shared.new(StmtBreak{}), parser.advance()} : (kind.is_continue() ? StmtResult{Shared.new(StmtContinue{}), parser.advance()} : (kind.is_return() ? [&]() {
auto value_result = parse_expr(parser.advance());
return StmtResult{Shared.new(StmtReturn(value_result.expr)), value_result.parser};
}() : [&]() {
auto expr_result = parse_expr(parser);
return (expr_result.parser.kind().is_equal() ? [&]() {
auto rhs = parse_expr(expr_result.parser.advance());
return StmtResult{Shared.new(StmtExpr(Shared.new(ExprBin(mlc::String("="), expr_result.expr, rhs.expr)))), rhs.parser};
}() : StmtResult{Shared.new(StmtExpr(expr_result.expr)), expr_result.parser});
}())))));
}
PatResult parse_pat(Parser parser) noexcept{
auto kind = parser.kind();
return (kind.is_ident() ? [&]() {
auto name = kind.ident();
return ((name == mlc::String("_")) ? PatResult{Shared.new(PatWild{}), parser.advance()} : (is_ctor_name(name) ? [&]() {
auto after = parser.advance();
return (after.kind().is_lparen() ? [&]() {
auto sub_pats = parse_pat_args(after.advance());
return PatResult{Shared.new(PatCtor(name, sub_pats.pats)), sub_pats.parser};
}() : PatResult{Shared.new(PatCtor(name, mlc::Array<auto>{})), after});
}() : PatResult{Shared.new(PatIdent(name)), parser.advance()}));
}() : (kind.is_int() ? PatResult{Shared.new(PatInt(kind.int_val())), parser.advance()} : (kind.is_str() ? PatResult{Shared.new(PatStr(kind.str_val())), parser.advance()} : (kind.is_true() ? PatResult{Shared.new(PatBool(true)), parser.advance()} : (kind.is_false() ? PatResult{Shared.new(PatBool(false)), parser.advance()} : PatResult{Shared.new(PatWild{}), parser.advance()})))));
}
PatsResult parse_pat_args(Parser parser) noexcept{
auto pats = mlc::Array<std::shared_ptr<Pat>>{};
return (parser.kind().is_rparen() ? PatsResult{pats, parser.advance()} : [&]() {
auto first = parse_pat(parser);
pats.push_back(first.pat);
auto state = first.parser;
while (state.kind().is_comma()) {
auto next = parse_pat(state.advance());
pats.push_back(next.pat);
state = next.parser;
}
return PatsResult{pats, state.advance()};
}());
}
ExprResult parse_expr(Parser parser) noexcept{
return parse_pipe(parser);
}
std::shared_ptr<Expr> pipe_desugar(std::shared_ptr<Expr> left_expr, std::shared_ptr<Expr> right_expr) noexcept{
return std::visit(overloaded{[&](const ExprCall& exprCall) { auto [callee, existing_args] = exprCall; } { return [&]() {
auto new_args = mlc::Array<auto>{left_expr};
auto i = 0;
while ((i < existing_args.length())) {
new_args.push_back(existing_args[i]);
i = (i + 1);
}
return Shared.new(ExprCall(callee, new_args));
}(); },
[&](auto& __v) { return [&]() {
auto call_args = mlc::Array<auto>{left_expr};
return Shared.new(ExprCall(right_expr, call_args));
}(); }
}, right_expr);
}
ExprResult parse_pipe(Parser parser) noexcept{
auto left = parse_or(parser);
auto expr = left.expr;
auto state = left.parser;
while (state.kind().is_pipe()) {
auto right = parse_or(state.advance());
expr = pipe_desugar(expr, right.expr);
state = right.parser;
}
return ExprResult{expr, state};
}
ExprResult parse_or(Parser parser) noexcept{
auto left = parse_and(parser);
auto expr = left.expr;
auto state = left.parser;
while ((state.kind().is_op() && (state.kind().op_val() == mlc::String("||")))) {
auto right = parse_and(state.advance());
expr = Shared.new(ExprBin(mlc::String("||"), expr, right.expr));
state = right.parser;
}
return ExprResult{expr, state};
}
ExprResult parse_and(Parser parser) noexcept{
auto left = parse_cmp(parser);
auto expr = left.expr;
auto state = left.parser;
while ((state.kind().is_op() && (state.kind().op_val() == mlc::String("&&")))) {
auto right = parse_cmp(state.advance());
expr = Shared.new(ExprBin(mlc::String("&&"), expr, right.expr));
state = right.parser;
}
return ExprResult{expr, state};
}
bool is_cmp_op(mlc::String op) noexcept{
return ((((((op == mlc::String("==")) || (op == mlc::String("!="))) || (op == mlc::String("<"))) || (op == mlc::String(">"))) || (op == mlc::String("<="))) || (op == mlc::String(">=")));
}
ExprResult parse_cmp(Parser parser) noexcept{
auto left = parse_add(parser);
auto expr = left.expr;
auto state = left.parser;
auto kind = state.kind();
if ((kind.is_op() && is_cmp_op(kind.op_val()))) {
auto right = parse_add(state.advance());
expr = Shared.new(ExprBin(kind.op_val(), expr, right.expr));
(state = right.parser);
}
return ExprResult{expr, state};
}
ExprResult parse_add(Parser parser) noexcept{
auto left = parse_mul(parser);
auto expr = left.expr;
auto state = left.parser;
auto go = true;
while (go) {
auto kind = state.kind();
if ((kind.is_op() && ((kind.op_val() == mlc::String("+")) || (kind.op_val() == mlc::String("-"))))) {
auto right = parse_mul(state.advance());
expr = Shared.new(ExprBin(kind.op_val(), expr, right.expr));
(state = right.parser);
} else {
(go = false);
}
}
return ExprResult{expr, state};
}
ExprResult parse_mul(Parser parser) noexcept{
auto left = parse_unary(parser);
auto expr = left.expr;
auto state = left.parser;
auto go = true;
while (go) {
auto kind = state.kind();
if ((kind.is_op() && (((kind.op_val() == mlc::String("*")) || (kind.op_val() == mlc::String("/"))) || (kind.op_val() == mlc::String("%"))))) {
auto right = parse_unary(state.advance());
expr = Shared.new(ExprBin(kind.op_val(), expr, right.expr));
(state = right.parser);
} else {
(go = false);
}
}
return ExprResult{expr, state};
}
ExprResult parse_unary(Parser parser) noexcept{
auto kind = parser.kind();
return ((kind.is_op() && ((kind.op_val() == mlc::String("!")) || (kind.op_val() == mlc::String("-")))) ? [&]() {
auto inner = parse_unary(parser.advance());
return ExprResult{Shared.new(ExprUn(kind.op_val(), inner.expr)), inner.parser};
}() : parse_postfix(parser));
}
ExprResult parse_postfix(Parser parser) noexcept{
auto base = parse_primary(parser);
auto expr = base.expr;
auto state = base.parser;
auto go = true;
while (go) {
auto kind = state.kind();
if (kind.is_dot()) {
auto field_name = state.advance().kind().ident();
(state.advance_by(2).kind().is_lparen() ? [&]() {
auto args = parse_arg_list(state.advance_by(3));
expr = Shared.new(ExprMethod(expr, field_name, mlc::io::args.exprs));
return (state = mlc::io::args.parser);
}() : [&]() {
expr = Shared.new(ExprField(expr, field_name));
return (state = state.advance_by(2));
}());
} else if (kind.is_lparen()) {
auto args = parse_arg_list(state.advance());
expr = Shared.new(ExprCall(expr, mlc::io::args.exprs));
(state = mlc::io::args.parser);
} else if (kind.is_lbracket()) {
auto index = parse_expr(state.advance());
expr = Shared.new(ExprIndex(expr, index.expr));
(state = index.parser.advance());
} else if (kind.is_question()) {
expr = Shared.new(ExprQuestion(expr));
(state = state.advance());
} else {
(go = false);
}



}
return ExprResult{expr, state};
}
ExprsResult parse_arg_list(Parser parser) noexcept{
auto exprs = mlc::Array<std::shared_ptr<Expr>>{};
return (parser.kind().is_rparen() ? ExprsResult{exprs, parser.advance()} : [&]() {
auto first = parse_expr(parser);
exprs.push_back(first.expr);
auto state = first.parser;
while (state.kind().is_comma()) {
auto next = parse_expr(state.advance());
exprs.push_back(next.expr);
state = next.parser;
}
return ExprsResult{exprs, state.advance()};
}());
}
ExprResult parse_primary(Parser parser) noexcept{
auto kind = parser.kind();
return (kind.is_int() ? ExprResult{Shared.new(ExprInt(kind.int_val())), parser.advance()} : (kind.is_str() ? ExprResult{Shared.new(ExprStr(kind.str_val())), parser.advance()} : (kind.is_true() ? ExprResult{Shared.new(ExprBool(true)), parser.advance()} : (kind.is_false() ? ExprResult{Shared.new(ExprBool(false)), parser.advance()} : (kind.is_lparen() ? (parser.advance().kind().is_rparen() ? (parser.advance_by(2).kind().is_fat_arrow() ? [&]() {
auto params = mlc::Array<mlc::String>{};
auto body = parse_expr(parser.advance_by(3));
return ExprResult{Shared.new(ExprLambda(params, body.expr)), body.parser};
}() : ExprResult{Shared.new(ExprUnit{}), parser.advance_by(2)}) : (looks_like_lambda_params(parser.advance()) ? [&]() {
auto param_result = parse_lambda_params(parser.advance());
auto body = parse_expr(param_result.parser.advance());
return ExprResult{Shared.new(ExprLambda(param_result.exprs, body.expr)), body.parser};
}() : [&]() {
auto inner = parse_expr(parser.advance());
return ExprResult{inner.expr, inner.parser.advance()};
}())) : (kind.is_lbracket() ? parse_array_lit(parser.advance()) : ((kind.is_if() || kind.is_unless()) ? parse_if_expr(parser) : (kind.is_do() ? parse_block(parser.advance()) : (kind.is_while() ? parse_while_expr(parser.advance()) : (kind.is_for() ? parse_for_expr(parser.advance()) : (kind.is_match() ? parse_match_expr(parser.advance()) : (kind.is_return() ? [&]() {
auto value = parse_expr(parser.advance());
auto stmts = mlc::Array<std::shared_ptr<Stmt>>{};
stmts.push_back(Shared.new(StmtReturn(value.expr)));
return ExprResult{Shared.new(ExprBlock(stmts, Shared.new(ExprUnit{}))), value.parser};
}() : (kind.is_ident() ? [&]() {
auto name = kind.ident();
return (parser.advance().kind().is_fat_arrow() ? [&]() {
auto params = mlc::Array<auto>{name};
auto body = parse_expr(parser.advance_by(2));
return ExprResult{Shared.new(ExprLambda(params, body.expr)), body.parser};
}() : ((is_ctor_name(name) && parser.advance().kind().is_lbrace()) ? parse_record_expr(parser.advance_by(2), name) : ExprResult{Shared.new(ExprIdent(name)), parser.advance()}));
}() : ExprResult{Shared.new(ExprUnit{}), parser.advance()})))))))))))));
}
bool looks_like_lambda_params(Parser parser) noexcept{
auto state = parser;
auto ok = true;
if ((!state.kind().is_ident())) {
(ok = false);
} else {
state = state.advance();
while (state.kind().is_comma()) {
state = state.advance();
if ((!state.kind().is_ident())) {
ok = false;
(state = state.advance());
} else {
(state = state.advance());
}
}
((!state.kind().is_rparen()) ? (ok = false) : (ok = state.advance().kind().is_fat_arrow()));
}
return ok;
}
NamesResult parse_lambda_params(Parser parser) noexcept{
auto names = mlc::Array<mlc::String>{};
auto state = parser;
names.push_back(state.kind().ident());
state = state.advance();
while (state.kind().is_comma()) {
state = state.advance();
names.push_back(state.kind().ident());
state = state.advance();
}
return NamesResult{names, state.advance()};
}
ExprResult parse_array_lit(Parser parser) noexcept{
auto exprs = mlc::Array<std::shared_ptr<Expr>>{};
return (parser.kind().is_rbracket() ? ExprResult{Shared.new(ExprArray(exprs)), parser.advance()} : [&]() {
auto first = parse_expr(parser);
exprs.push_back(first.expr);
auto state = first.parser;
while (state.kind().is_comma()) {
auto next = parse_expr(state.advance());
exprs.push_back(next.expr);
state = next.parser;
}
return ExprResult{Shared.new(ExprArray(exprs)), state.advance()};
}());
}
ExprResult parse_if_expr(Parser parser) noexcept{
auto is_negated = parser.kind().is_unless();
auto cond_result = parse_expr(parser.advance());
auto condition = (is_negated ? Shared.new(ExprUn(mlc::String("!"), cond_result.expr)) : cond_result.expr);
auto then_result = parse_stmts_until_else_end(cond_result.parser.advance());
auto then_expr = then_result.to_block_expr();
return (then_result.parser.kind().is_else() ? [&]() {
auto after_else = then_result.parser.advance();
return ((after_else.kind().is_if() || after_else.kind().is_unless()) ? [&]() {
auto else_result = parse_if_expr(after_else);
return ExprResult{Shared.new(ExprIf(condition, then_expr, else_result.expr)), else_result.parser};
}() : [&]() {
auto else_stmts = parse_stmts_until_end(after_else);
return ExprResult{Shared.new(ExprIf(condition, then_expr, else_stmts.to_block_expr())), else_stmts.parser};
}());
}() : ExprResult{Shared.new(ExprIf(condition, then_expr, Shared.new(ExprUnit{}))), then_result.parser.advance()});
}
ExprResult parse_block(Parser parser) noexcept{
auto result = parse_stmts_until_end(parser);
return ExprResult{result.to_block_expr(), result.parser};
}
bool is_unit_expr(std::shared_ptr<Expr> expr) noexcept{
return std::visit(overloaded{[&](const ExprUnit& exprUnit) { return true; },
[&](const ExprBlock& exprBlock) { auto [__0, result] = exprBlock; } { return is_unit_expr(result); },
[&](const ExprIf& exprIf) { auto [__0, __1, else_expr] = exprIf; } { return is_unit_expr(else_expr); },
[&](auto& __v) { return false; }
}, expr);
}
std::shared_ptr<Expr> block_result(mlc::Array<std::shared_ptr<Stmt>> stmts) noexcept{
if ((stmts.length() == 0)) {
return Shared.new(ExprUnit{});
} else {
return std::visit(overloaded{[&](const StmtExpr& stmtExpr) { auto [expr] = stmtExpr; } { return (is_unit_expr(expr) ? Shared.new(ExprUnit{}) : expr); },
[&](auto& __v) { return Shared.new(ExprUnit{}); }
}, stmts[(stmts.length() - 1)]);
}
}
mlc::Array<std::shared_ptr<Stmt>> block_body(mlc::Array<std::shared_ptr<Stmt>> stmts) noexcept{
return ((stmts.length() == 0) ? stmts : std::visit(overloaded{[&](const StmtExpr& stmtExpr) { auto [expr] = stmtExpr; } { return (is_unit_expr(expr) ? stmts : [&]() {
auto result = mlc::Array<std::shared_ptr<Stmt>>{};
auto i = 0;
while ((i < (stmts.length() - 1))) {
result.push_back(stmts[i]);
i = (i + 1);
}
return result;
}()); },
[&](auto& __v) { return stmts; }
}, stmts[(stmts.length() - 1)]));
}
std::shared_ptr<Expr> StmtsResult_to_block_expr(StmtsResult self) noexcept{
return Shared.new(ExprBlock(block_body(stmts), block_result(stmts)));
}
ExprResult parse_while_expr(Parser parser) noexcept{
auto cond = parse_expr(parser);
auto body = parse_stmts_until_end(cond.parser.advance());
return ExprResult{Shared.new(ExprWhile(cond.expr, body.stmts)), body.parser};
}
ExprResult parse_for_expr(Parser parser) noexcept{
auto var_name = parser.kind().ident();
auto iter = parse_expr(parser.advance_by(2));
auto body = parse_stmts_until_end(iter.parser.advance());
return ExprResult{Shared.new(ExprFor(var_name, iter.expr, body.stmts)), body.parser};
}
ExprResult parse_match_expr(Parser parser) noexcept{
auto subject = parse_expr(parser);
auto arms = parse_arms(subject.parser.advance());
return ExprResult{Shared.new(ExprMatch(subject.expr, arms.arms)), arms.parser};
}
ArmsResult parse_arms(Parser parser) noexcept{
auto arms = mlc::Array<std::shared_ptr<MatchArm>>{};
auto state = parser;
while (((!state.kind().is_rbrace()) && (!state.at_eof()))) {
auto pat_result = parse_pat(state);
auto body_result = parse_expr(pat_result.parser.advance());
arms.push_back(Shared.new(MatchArm{pat_result.pat, body_result.expr}));
state = body_result.parser;
if (state.kind().is_comma()) {
(state = state.advance());
}
}
return ArmsResult{arms, state.advance()};
}
FieldValsResult parse_record_fields(Parser parser) noexcept{
auto field_vals = mlc::Array<std::shared_ptr<FieldVal>>{};
auto state = parser;
while (((!state.kind().is_rbrace()) && (!state.at_eof()))) {
auto field_name = state.kind().ident();
auto after_name = state.advance();
if (after_name.kind().is_colon()) {
auto value_result = parse_expr(after_name.advance());
field_vals.push_back(Shared.new(FieldVal{field_name, value_result.expr}));
(state = value_result.parser);
} else {
field_vals.push_back(Shared.new(FieldVal{field_name, Shared.new(ExprIdent(field_name))}));
(state = after_name);
}
if (state.kind().is_comma()) {
(state = state.advance());
}
}
return FieldValsResult{field_vals, state.advance()};
}
ExprResult parse_record_expr(Parser parser, mlc::String record_name) noexcept{
if (parser.kind().is_spread()) {
auto base_result = parse_expr(parser.advance());
auto after_base = base_result.parser;
if (after_base.kind().is_comma()) {
(after_base = after_base.advance());
}
auto fields_result = parse_record_fields(after_base);
return ExprResult{Shared.new(ExprRecordUpdate(record_name, base_result.expr, fields_result.field_vals)), fields_result.parser};
} else {
auto fields_result = parse_record_fields(parser);
return ExprResult{Shared.new(ExprRecord(record_name, fields_result.field_vals)), fields_result.parser};
}
}
DeclResult parse_import_decl(Parser parser) noexcept{
auto state = parser;
auto symbols = mlc::Array<mlc::String>{};
if ((state.kind().is_op() && (state.kind().op_val() == mlc::String("*")))) {
symbols.push_back(mlc::String("*"));
(state = state.advance());
} else if (state.kind().is_lbrace()) {
state = state.advance();
while ((state.kind().is_ident() || (state.kind().is_op() && (state.kind().op_val() == mlc::String("*"))))) {
if ((state.kind().is_op() && (state.kind().op_val() == mlc::String("*")))) {
symbols.push_back(mlc::String("*"));
(state = state.advance());
} else {
symbols.push_back(state.kind().ident());
(state = state.advance());
}
if (state.kind().is_comma()) {
(state = state.advance());
}
}
(state = (state.kind().is_rbrace() ? state.advance() : state));
}

state = (state.kind().is_from() ? state.advance() : state);
auto path = (state.kind().is_str() ? state.kind().str_val() : mlc::String(""));
return DeclResult{Shared.new(DeclImport(path, symbols)), state.advance()};
}
DeclResult parse_extend_decl(Parser parser) noexcept{
auto type_name = parser.kind().ident();
auto state = parser;
while ((state.kind().is_colon() || (state.kind().is_ident() && (!state.kind().is_lbrace())))) {
state = state.advance();
}
auto methods_state = (state.kind().is_lbrace() ? state.advance() : state);
auto methods = mlc::Array<std::shared_ptr<Decl>>{};
while (((!methods_state.kind().is_rbrace()) && (!methods_state.at_eof()))) {
if (methods_state.kind().is_fn()) {
auto method_result = parse_extend_method(methods_state, type_name);
methods.push_back(method_result.decl);
(methods_state = method_result.parser);
} else {
(methods_state = methods_state.advance());
}
}
return DeclResult{Shared.new(DeclExtend(type_name, methods)), methods_state.advance()};
}
DeclResult parse_extend_method(Parser parser, mlc::String type_name) noexcept{
auto fn_name = parser.advance().kind().ident();
auto mangled_name = ((type_name + mlc::String("_")) + fn_name);
auto after_lparen = parser.advance_by(3);
auto after_self = ((after_lparen.kind().is_ident() && (after_lparen.kind().ident() == mlc::String("self"))) ? after_lparen.advance() : after_lparen);
auto self_param = Shared.new(Param{mlc::String("self"), false, Shared.new(TyNamed(type_name))});
auto params = mlc::Array<std::shared_ptr<Param>>{};
params.push_back(self_param);
auto params_state = (after_self.kind().is_comma() ? after_self.advance() : after_self);
auto rest_params = (params_state.kind().is_rparen() ? ParamsResult{mlc::Array<auto>{}, params_state.advance()} : parse_params(params_state));
auto index = 0;
while ((index < rest_params.params.length())) {
params.push_back(rest_params.params[index]);
index = (index + 1);
}
auto ret_type_result = parse_type(rest_params.parser.advance());
auto body_result = parse_expr(ret_type_result.parser.advance());
return DeclResult{Shared.new(DeclFn(mangled_name, params, ret_type_result.type_expr, body_result.expr)), body_result.parser};
}
DeclResult parse_decl(Parser parser) noexcept{
auto kind = parser.kind();
return ((kind.is_ident() && (kind.ident() == mlc::String("export"))) ? [&]() {
auto inner = parse_decl(parser.advance());
return DeclResult{Shared.new(DeclExported(inner.decl)), inner.parser};
}() : ((kind.is_fn() || (kind.is_extern() && parser.advance().kind().is_fn())) ? parse_fn_decl(parser) : (kind.is_type() ? parse_type_decl(parser.advance()) : (kind.is_extend() ? parse_extend_decl(parser.advance()) : (kind.is_import() ? parse_import_decl(parser.advance()) : DeclResult{Shared.new(DeclFn(mlc::String("__skip__"), mlc::Array<auto>{}, Shared.new(TyUnit{}), Shared.new(ExprUnit{}))), parser.advance()})))));
}
DeclResult parse_fn_decl(Parser parser) noexcept{
auto fn_start = (parser.kind().is_extern() ? parser.advance() : parser);
auto fn_name = fn_start.advance().kind().ident();
auto params_result = parse_params(fn_start.advance_by(3));
auto ret_type_result = parse_type(params_result.parser.advance());
auto body_result = parse_expr(ret_type_result.parser.advance());
return DeclResult{Shared.new(DeclFn(fn_name, params_result.params, ret_type_result.type_expr, body_result.expr)), body_result.parser};
}
ParamsResult parse_params(Parser parser) noexcept{
auto params = mlc::Array<std::shared_ptr<Param>>{};
return (parser.kind().is_rparen() ? ParamsResult{params, parser.advance()} : [&]() {
auto first = parse_param(parser);
params.push_back(first.param);
auto state = first.parser;
while (state.kind().is_comma()) {
auto next = parse_param(state.advance());
params.push_back(next.param);
state = next.parser;
}
return ParamsResult{params, state.advance()};
}());
}
ParamResult parse_param(Parser parser) noexcept{
auto is_mutable = parser.kind().is_mut();
auto name_pos = (is_mutable ? parser.advance() : parser);
auto type_result = parse_type(name_pos.advance_by(2));
return ParamResult{Shared.new(Param{name_pos.kind().ident(), is_mutable, type_result.type_expr}), type_result.parser};
}
DeclResult parse_type_decl(Parser parser) noexcept{
auto type_name = parser.kind().ident();
auto after_eq = parser.advance_by(2);
auto first_kind = after_eq.kind();
return (first_kind.is_lbrace() ? [&]() {
auto field_defs_result = parse_field_defs(after_eq.advance());
auto variants = mlc::Array<std::shared_ptr<TypeVariant>>{};
variants.push_back(Shared.new(VarRecord(type_name, field_defs_result.field_defs)));
return DeclResult{Shared.new(DeclType(type_name, variants)), field_defs_result.parser};
}() : (((first_kind.is_ident() && is_ctor_name(first_kind.ident())) && after_eq.advance().kind().is_lbrace()) ? [&]() {
auto field_defs_result = parse_field_defs(after_eq.advance_by(2));
auto variants = mlc::Array<std::shared_ptr<TypeVariant>>{};
variants.push_back(Shared.new(VarRecord(first_kind.ident(), field_defs_result.field_defs)));
return DeclResult{Shared.new(DeclType(type_name, variants)), field_defs_result.parser};
}() : [&]() {
auto variants_result = parse_variants(after_eq);
return DeclResult{Shared.new(DeclType(type_name, variants_result.variants)), variants_result.parser};
}()));
}
VariantsResult parse_variants(Parser parser) noexcept{
auto variants = mlc::Array<std::shared_ptr<TypeVariant>>{};
auto state = (parser.kind().is_bar() ? parser.advance() : parser);
while (((!state.at_eof()) && (state.kind().is_bar() || state.kind().is_ident()))) {
auto variant_result = parse_variant(state);
variants.push_back(variant_result.variant);
state = variant_result.parser;
if (state.kind().is_bar()) {
(state = state.advance());
}
}
return VariantsResult{variants, state};
}
VariantResult parse_variant(Parser parser) noexcept{
auto variant_name = parser.kind().ident();
auto after_name = parser.advance();
return (after_name.kind().is_lparen() ? [&]() {
auto types_result = parse_tuple_types(after_name.advance());
return VariantResult{Shared.new(VarTuple(variant_name, types_result.types)), types_result.parser};
}() : (after_name.kind().is_lbrace() ? [&]() {
auto field_defs_result = parse_field_defs(after_name.advance());
return VariantResult{Shared.new(VarRecord(variant_name, field_defs_result.field_defs)), field_defs_result.parser};
}() : VariantResult{Shared.new(VarUnit(variant_name)), after_name}));
}
TypesResult parse_tuple_types(Parser parser) noexcept{
auto types = mlc::Array<std::shared_ptr<TypeExpr>>{};
auto first = parse_type(parser);
types.push_back(first.type_expr);
auto state = first.parser;
while (state.kind().is_comma()) {
auto next = parse_type(state.advance());
types.push_back(next.type_expr);
state = next.parser;
}
return TypesResult{types, state.advance()};
}
FieldDefsResult parse_field_defs(Parser parser) noexcept{
auto field_defs = mlc::Array<std::shared_ptr<FieldDef>>{};
auto state = parser;
while (((!state.kind().is_rbrace()) && (!state.at_eof()))) {
auto field_name = state.kind().ident();
auto type_result = parse_type(state.advance_by(2));
field_defs.push_back(Shared.new(FieldDef{field_name, type_result.type_expr}));
state = type_result.parser;
if (state.kind().is_comma()) {
(state = state.advance());
}
}
return FieldDefsResult{field_defs, state.advance()};
}
Program parse_program(mlc::Array<Token> tokens) noexcept{
auto decls = mlc::Array<std::shared_ptr<Decl>>{};
auto state = parser_new(tokens);
while ((!state.at_eof())) {
auto result = parse_decl(state);
decls.push_back(result.decl);
state = result.parser;
}
return Program{decls};
}
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept{
return export;
}
bool LexOut_has_errors(LexOut self) noexcept{
return (errors.length() > 0);
}
TKind Token_kind_value(Token self) noexcept{
return kind;
}
int Token_line_number(Token self) noexcept{
return line;
}
int Token_column(Token self) noexcept{
return col;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [d] = declExported; } { return d; },
[&](auto& __v) { return decl; }
}, decl);
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; } { return name; },
[&](const DeclType& declType) { auto [name, __1] = declType; } { return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1] = declExtend; } { return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return decl_name(d); }
}, decl);
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
auto i = 0;
while ((i < src.length())) {
dst.push_back(src[i]);
i = (i + 1);
}
return dst;
}
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept{
return export;
}
bool LexOut_has_errors(LexOut self) noexcept{
return (errors.length() > 0);
}
TKind Token_kind_value(Token self) noexcept{
return kind;
}
int Token_line_number(Token self) noexcept{
return line;
}
int Token_column(Token self) noexcept{
return col;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [d] = declExported; } { return d; },
[&](auto& __v) { return decl; }
}, decl);
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; } { return name; },
[&](const DeclType& declType) { auto [name, __1] = declType; } { return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1] = declExtend; } { return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return decl_name(d); }
}, decl);
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
auto i = 0;
while ((i < src.length())) {
dst.push_back(src[i]);
i = (i + 1);
}
return dst;
}
bool scope_contains(mlc::Array<mlc::String> scope, mlc::String name) noexcept{
auto found = false;
auto index = 0;
while ((index < scope.length())) {
if ((scope[index] == name)) {
(found = true);
}
index = (index + 1);
}
return found;
}
mlc::Array<mlc::String> pattern_bindings(std::shared_ptr<Pat> pattern) noexcept{
auto accumulator = mlc::Array<mlc::String>{};
return collect_pattern_bindings(pattern, accumulator);
}
mlc::Array<mlc::String> collect_pattern_bindings(std::shared_ptr<Pat> pattern, mlc::Array<mlc::String> accumulator) noexcept{
return std::visit(overloaded{[&](const PatIdent& patIdent) { auto [name] = patIdent; } { return [&]() {
accumulator.push_back(name);
return accumulator;
}(); },
[&](const PatCtor& patCtor) { auto [__0, sub_patterns] = patCtor; } { return [&]() {
auto index = 0;
while ((index < sub_patterns.length())) {
accumulator = collect_pattern_bindings(sub_patterns[index], accumulator);
index = (index + 1);
}
return accumulator;
}(); },
[&](auto& __v) { return accumulator; }
}, pattern);
}
mlc::Array<mlc::String> collect_globals(Program prog) noexcept{
auto names = mlc::Array<mlc::String>{};
names.push_back(mlc::String("true"));
names.push_back(mlc::String("false"));
names.push_back(mlc::String("exit"));
names.push_back(mlc::String("print"));
names.push_back(mlc::String("println"));
names.push_back(mlc::String("args"));
names.push_back(mlc::String("File"));
names.push_back(mlc::String("Shared"));
names.push_back(mlc::String("Map"));
auto i = 0;
while ((i < prog.decls.length())) {
std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; } { return [&]() {
names.push_back(name);
/* unit */;
return /* unit */;
}(); },
[&](const DeclType& declType) { auto [name, variants] = declType; } { return [&]() {
names.push_back(name);
auto j = 0;
return [&]() {
while ((j < variants.length())) {
std::visit(overloaded{[&](const VarUnit& varUnit) { auto [variant_name] = varUnit; } { return [&]() {
names.push_back(variant_name);
/* unit */;
return /* unit */;
}(); },
[&](const VarTuple& varTuple) { auto [variant_name, __1] = varTuple; } { return [&]() {
names.push_back(variant_name);
/* unit */;
return /* unit */;
}(); },
[&](const VarRecord& varRecord) { auto [variant_name, __1] = varRecord; } { return [&]() {
names.push_back(variant_name);
/* unit */;
return /* unit */;
}(); }
}, variants[j]);
j = (j + 1);
}
}()();
}(); },
[&](const DeclExtend& declExtend) { auto [__0, __1] = declExtend; } { return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const DeclExported& declExported) { auto [__0] = declExported; } { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, decl_inner(prog.decls[i]));
i = (i + 1);
}
return names;
}
mlc::Array<mlc::String> check_names_expr(std::shared_ptr<Expr> expr, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept{
return std::visit(overloaded{[&](const ExprIdent& exprIdent) { auto [name] = exprIdent; } { return [&]() {
auto errors = mlc::Array<mlc::String>{};
if (((!scope_contains(locals, name)) && (!scope_contains(globals, name)))) {
errors.push_back((mlc::String("undefined: ") + name));
}
return errors;
}(); },
[&](const ExprBin& exprBin) { auto [__0, left, right] = exprBin; } { return errs_append(check_names_expr(left, locals, globals), check_names_expr(right, locals, globals)); },
[&](const ExprUn& exprUn) { auto [__0, inner] = exprUn; } { return check_names_expr(inner, locals, globals); },
[&](const ExprCall& exprCall) { auto [func, args] = exprCall; } { return [&]() {
auto errors = check_names_expr(func, locals, globals);
auto i = 0;
while ((i < mlc::io::args.length())) {
errors = errs_append(errors, check_names_expr(mlc::io::args[i], locals, globals));
i = (i + 1);
}
return errors;
}(); },
[&](const ExprMethod& exprMethod) { auto [obj, __1, args] = exprMethod; } { return [&]() {
auto errors = check_names_expr(obj, locals, globals);
auto i = 0;
while ((i < mlc::io::args.length())) {
errors = errs_append(errors, check_names_expr(mlc::io::args[i], locals, globals));
i = (i + 1);
}
return errors;
}(); },
[&](const ExprField& exprField) { auto [obj, __1] = exprField; } { return check_names_expr(obj, locals, globals); },
[&](const ExprIndex& exprIndex) { auto [obj, idx] = exprIndex; } { return errs_append(check_names_expr(obj, locals, globals), check_names_expr(idx, locals, globals)); },
[&](const ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; } { return errs_append(errs_append(check_names_expr(cond, locals, globals), check_names_expr(then_expr, locals, globals)), check_names_expr(else_expr, locals, globals)); },
[&](const ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; } { return [&]() {
auto stmt_result = check_names_stmts(stmts, locals, globals);
return stmt_result.append_expr_errors(check_names_expr(result, stmt_result.scope, globals));
}(); },
[&](const ExprWhile& exprWhile) { auto [cond, stmts] = exprWhile; } { return [&]() {
auto check_result = check_names_stmts(stmts, locals, globals);
return errs_append(check_names_expr(cond, locals, globals), check_result.errors);
}(); },
[&](const ExprFor& exprFor) { auto [var, iter, stmts] = exprFor; } { return [&]() {
auto inner = locals;
inner.push_back(var);
auto check_result = check_names_stmts(stmts, inner, globals);
return errs_append(check_names_expr(iter, locals, globals), check_result.errors);
}(); },
[&](const ExprMatch& exprMatch) { auto [subject, arms] = exprMatch; } { return [&]() {
auto errors = check_names_expr(subject, locals, globals);
auto i = 0;
while ((i < arms.length())) {
auto bindings = pattern_bindings(arms[i].pat);
auto arm_scope = locals;
auto index = 0;
while ((index < bindings.length())) {
arm_scope.push_back(bindings[index]);
index = (index + 1);
}
errors = errs_append(errors, check_names_expr(arms[i].body, arm_scope, globals));
i = (i + 1);
}
return errors;
}(); },
[&](const ExprRecord& exprRecord) { auto [__0, field_vals] = exprRecord; } { return [&]() {
auto errors = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_vals.length())) {
errors = errs_append(errors, check_names_expr(field_vals[i].val, locals, globals));
i = (i + 1);
}
return errors;
}(); },
[&](const ExprRecordUpdate& exprRecordUpdate) { auto [__0, base, field_vals] = exprRecordUpdate; } { return [&]() {
auto errors = check_names_expr(base, locals, globals);
auto i = 0;
while ((i < field_vals.length())) {
errors = errs_append(errors, check_names_expr(field_vals[i].val, locals, globals));
i = (i + 1);
}
return errors;
}(); },
[&](const ExprArray& exprArray) { auto [exprs] = exprArray; } { return [&]() {
auto errors = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < exprs.length())) {
errors = errs_append(errors, check_names_expr(exprs[i], locals, globals));
i = (i + 1);
}
return errors;
}(); },
[&](const ExprQuestion& exprQuestion) { auto [inner] = exprQuestion; } { return check_names_expr(inner, locals, globals); },
[&](const ExprLambda& exprLambda) { auto [params, body] = exprLambda; } { return [&]() {
auto lambda_locals = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < locals.length())) {
lambda_locals.push_back(locals[i]);
i = (i + 1);
}
auto j = 0;
while ((j < params.length())) {
lambda_locals.push_back(params[j]);
j = (j + 1);
}
return check_names_expr(body, lambda_locals, globals);
}(); },
[&](auto& __v) { return mlc::Array<auto>{}; }
}, expr);
}
mlc::Array<mlc::String> NameCheckResult_append_expr_errors(NameCheckResult self, mlc::Array<mlc::String> expr_errors) noexcept{
return errs_append(errors, expr_errors);
}
NameCheckResult check_names_stmts(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::Array<mlc::String> locals, mlc::Array<mlc::String> globals) noexcept{
auto errors = mlc::Array<mlc::String>{};
auto scope = locals;
auto i = 0;
while ((i < stmts.length())) {
std::visit(overloaded{[&](const StmtLet& stmtLet) { auto [name, __1, __2, value] = stmtLet; } { return [&]() {
errors = errs_append(errors, check_names_expr(value, scope, globals));
return scope.push_back(name)();
}(); },
[&](const StmtExpr& stmtExpr) { auto [expr] = stmtExpr; } { return [&]() {
errors = errs_append(errors, check_names_expr(expr, scope, globals));
/* unit */;
return /* unit */;
}(); },
[&](const StmtReturn& stmtReturn) { auto [expr] = stmtReturn; } { return [&]() {
errors = errs_append(errors, check_names_expr(expr, scope, globals));
/* unit */;
return /* unit */;
}(); },
[&](const StmtBreak& stmtBreak) { return [&]() {
/* unit */;
return /* unit */;
}(); },
[&](const StmtContinue& stmtContinue) { return [&]() {
/* unit */;
return /* unit */;
}(); }
}, stmts[i]);
i = (i + 1);
}
return NameCheckResult{errors, scope};
}
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept{
return export;
}
bool LexOut_has_errors(LexOut self) noexcept{
return (errors.length() > 0);
}
TKind Token_kind_value(Token self) noexcept{
return kind;
}
int Token_line_number(Token self) noexcept{
return line;
}
int Token_column(Token self) noexcept{
return col;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [d] = declExported; } { return d; },
[&](auto& __v) { return decl; }
}, decl);
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; } { return name; },
[&](const DeclType& declType) { auto [name, __1] = declType; } { return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1] = declExtend; } { return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return decl_name(d); }
}, decl);
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
auto i = 0;
while ((i < src.length())) {
dst.push_back(src[i]);
i = (i + 1);
}
return dst;
}
std::shared_ptr<Type> TypeRegistry_fn_type(TypeRegistry self, mlc::String name) noexcept{
return fn_types.get(name);
}
bool TypeRegistry_has_fn(TypeRegistry self, mlc::String name) noexcept{
return fn_types.has(name);
}
std::shared_ptr<Type> TypeRegistry_ctor_type(TypeRegistry self, mlc::String name) noexcept{
return ctor_types.get(name);
}
bool TypeRegistry_has_ctor(TypeRegistry self, mlc::String name) noexcept{
return ctor_types.has(name);
}
mlc::Array<std::shared_ptr<Type>> TypeRegistry_ctor_params_for(TypeRegistry self, mlc::String name) noexcept{
if (ctor_params.has(name)) {
return ctor_params.get(name);
} else {
return mlc::Array<auto>{};
}
}
Map<mlc::String, std::shared_ptr<Type>> TypeRegistry_fields_for(TypeRegistry self, mlc::String type_name) noexcept{
return field_types.get(type_name);
}
bool TypeRegistry_has_fields(TypeRegistry self, mlc::String type_name) noexcept{
return field_types.has(type_name);
}
TypeRegistry empty_registry() noexcept{
return TypeRegistry{Map.new(), Map.new(), Map.new(), Map.new()};
}
std::shared_ptr<Type> type_from_annotation(std::shared_ptr<TypeExpr> type_expr) noexcept{
return std::visit(overloaded{[&](const TyI32& tyI32) { return Shared.new(TI32{}); },
[&](const TyString& tyString) { return Shared.new(TString{}); },
[&](const TyBool& tyBool) { return Shared.new(TBool{}); },
[&](const TyUnit& tyUnit) { return Shared.new(TUnit{}); },
[&](const TyNamed& tyNamed) { auto [name] = tyNamed; } { return Shared.new(TNamed(name)); },
[&](const TyArray& tyArray) { auto [inner] = tyArray; } { return Shared.new(TArray(type_from_annotation(inner))); },
[&](const TyShared& tyShared) { auto [inner] = tyShared; } { return Shared.new(TShared(type_from_annotation(inner))); },
[&](const TyFn& tyFn) { auto [params, ret] = tyFn; } { return [&]() {
auto param_types = mlc::Array<std::shared_ptr<Type>>{};
auto i = 0;
while ((i < params.length())) {
param_types.push_back(type_from_annotation(params[i]));
i = (i + 1);
}
return Shared.new(TFn(param_types, type_from_annotation(ret)));
}(); },
[&](auto& __v) { return Shared.new(TUnknown{}); }
}, type_expr);
}
TypeRegistry build_registry(Program prog) noexcept{
auto registry = empty_registry();
auto i = 0;
while ((i < prog.decls.length())) {
registry = register_decl(registry, prog.decls[i]);
i = (i + 1);
}
return registry;
}
TypeRegistry register_decl(TypeRegistry registry, std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclExtend& declExtend) { auto [__0, __1] = declExtend; } { return registry; },
[&](const DeclFn& declFn) { auto [name, params, return_type, __3] = declFn; } { return [&]() {
auto param_types = mlc::Array<std::shared_ptr<Type>>{};
auto i = 0;
while ((i < params.length())) {
param_types.push_back(type_from_annotation(params[i].typ));
i = (i + 1);
}
registry.fn_types.set(name, Shared.new(TFn(param_types, type_from_annotation(return_type))));
return registry;
}(); },
[&](const DeclType& declType) { auto [type_name, variants] = declType; } { return [&]() {
auto i = 0;
while ((i < variants.length())) {
registry = register_variant(registry, type_name, variants[i]);
i = (i + 1);
}
return registry;
}(); },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return registry; },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return register_decl(registry, d); }
}, decl);
}
TypeRegistry register_variant(TypeRegistry registry, mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept{
auto result_type = Shared.new(TNamed(type_name));
return std::visit(overloaded{[&](const VarUnit& varUnit) { auto [variant_name] = varUnit; } { return [&]() {
registry.ctor_types.set(variant_name, result_type);
registry.ctor_params.set(variant_name, mlc::Array<auto>{});
return registry;
}(); },
[&](const VarTuple& varTuple) { auto [variant_name, field_types] = varTuple; } { return [&]() {
auto field_type_list = mlc::Array<std::shared_ptr<Type>>{};
auto i = 0;
while ((i < field_types.length())) {
field_type_list.push_back(type_from_annotation(field_types[i]));
i = (i + 1);
}
registry.ctor_types.set(variant_name, result_type);
registry.ctor_params.set(variant_name, field_type_list);
return registry;
}(); },
[&](const VarRecord& varRecord) { auto [variant_name, field_defs] = varRecord; } { return [&]() {
auto field_map = Map.new();
auto i = 0;
while ((i < field_defs.length())) {
field_map.set(field_defs[i].name, type_from_annotation(field_defs[i].typ));
i = (i + 1);
}
registry.ctor_types.set(variant_name, result_type);
registry.ctor_params.set(variant_name, mlc::Array<auto>{});
registry.field_types.set(variant_name, field_map);
registry.field_types.set(type_name, field_map);
return registry;
}(); }
}, variant);
}
InferResult InferResult_with_type(InferResult self, std::shared_ptr<Type> new_type) noexcept{
return InferResult{new_type, errors};
}
InferResult InferResult_absorb(InferResult self, InferResult other) noexcept{
return InferResult{inferred_type, errs_append(errors, other.errors)};
}
InferResult InferResult_absorb_stmt(InferResult self, StmtInferResult stmt_result) noexcept{
return InferResult{inferred_type, errs_append(stmt_result.errors, errors)};
}
InferResult infer_ok(std::shared_ptr<Type> type_val) noexcept{
return InferResult{type_val, mlc::Array<auto>{}};
}
std::shared_ptr<Type> binary_op_type(mlc::String op, std::shared_ptr<Type> left_type) noexcept{
if ((((((op == mlc::String("+")) || (op == mlc::String("-"))) || (op == mlc::String("*"))) || (op == mlc::String("/"))) || (op == mlc::String("%")))) {
return std::visit(overloaded{[&](const TString& tString) { return Shared.new(TString{}); },
[&](auto& __v) { return Shared.new(TI32{}); }
}, left_type);
} else if ((op == mlc::String("="))) {
return Shared.new(TUnit{});
} else {
return Shared.new(TBool{});
}

}
std::shared_ptr<Type> method_return_type(mlc::String method_name) noexcept{
if ((((method_name == mlc::String("length")) || (method_name == mlc::String("size"))) || (method_name == mlc::String("to_i")))) {
return Shared.new(TI32{});
} else if (((method_name == mlc::String("push")) || (method_name == mlc::String("set")))) {
return Shared.new(TUnit{});
} else if ((((((method_name == mlc::String("char_at")) || (method_name == mlc::String("join"))) || (method_name == mlc::String("to_string"))) || (method_name == mlc::String("substring"))) || (method_name == mlc::String("to_lower")))) {
return Shared.new(TString{});
} else if ((method_name == mlc::String("has"))) {
return Shared.new(TBool{});
} else {
return Shared.new(TUnknown{});
}



}
InferResult infer_args_errors(InferResult initial, mlc::Array<std::shared_ptr<Expr>> args, Map<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept{
auto result = initial;
auto index = 0;
while ((index < mlc::io::args.length())) {
result = result.absorb(infer_expr(mlc::io::args[index], type_env, registry));
index = (index + 1);
}
return result;
}
InferResult infer_field_vals_errors(InferResult initial, mlc::Array<std::shared_ptr<FieldVal>> field_vals, Map<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept{
auto result = initial;
auto index = 0;
while ((index < field_vals.length())) {
result = result.absorb(infer_expr(field_vals[index].val, type_env, registry));
index = (index + 1);
}
return result;
}
std::shared_ptr<Type> field_lookup_type(std::shared_ptr<Type> obj_type, mlc::String field_name, TypeRegistry registry) noexcept{
auto type_name = std::visit(overloaded{[&](const TNamed& tNamed) { auto [name] = tNamed; } { return name; },
[&](auto& __v) { return mlc::String(""); }
}, obj_type);
return (((type_name != mlc::String("")) && registry.has_fields(type_name)) ? [&]() {
auto field_map = registry.fields_for(type_name);
return (field_map.has(field_name) ? field_map.get(field_name) : Shared.new(TUnknown{}));
}() : Shared.new(TUnknown{}));
}
InferResult infer_expr(std::shared_ptr<Expr> expr, Map<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const ExprInt& exprInt) { auto [__0] = exprInt; } { return infer_ok(Shared.new(TI32{})); },
[&](const ExprStr& exprStr) { auto [__0] = exprStr; } { return infer_ok(Shared.new(TString{})); },
[&](const ExprBool& exprBool) { auto [__0] = exprBool; } { return infer_ok(Shared.new(TBool{})); },
[&](const ExprUnit& exprUnit) { return infer_ok(Shared.new(TUnit{})); },
[&](const ExprIdent& exprIdent) { auto [name] = exprIdent; } { return (type_env.has(name) ? infer_ok(type_env.get(name)) : (registry.has_fn(name) ? infer_ok(registry.fn_type(name)) : (registry.has_ctor(name) ? infer_ok(registry.ctor_type(name)) : infer_ok(Shared.new(TUnknown{}))))); },
[&](const ExprBin& exprBin) { auto [op, left, right] = exprBin; } { return [&]() {
auto left_result = infer_expr(left, type_env, registry);
auto right_result = infer_expr(right, type_env, registry);
return left_result.absorb(right_result).with_type(binary_op_type(op, left_result.inferred_type));
}(); },
[&](const ExprUn& exprUn) { auto [op, inner] = exprUn; } { return [&]() {
auto inner_result = infer_expr(inner, type_env, registry);
auto result_type = ((op == mlc::String("!")) ? Shared.new(TBool{}) : inner_result.inferred_type);
return inner_result.with_type(result_type);
}(); },
[&](const ExprCall& exprCall) { auto [func, args] = exprCall; } { return [&]() {
auto func_result = infer_expr(func, type_env, registry);
auto with_args = infer_args_errors(func_result, mlc::io::args, type_env, registry);
auto return_type = std::visit(overloaded{[&](const TFn& tFn) { auto [__0, ret] = tFn; } { return ret; },
[&](auto& __v) { return Shared.new(TUnknown{}); }
}, func_result.inferred_type);
return with_args.with_type(return_type);
}(); },
[&](const ExprMethod& exprMethod) { auto [obj, method_name, args] = exprMethod; } { return [&]() {
auto obj_result = infer_expr(obj, type_env, registry);
return infer_args_errors(obj_result, mlc::io::args, type_env, registry).with_type(method_return_type(method_name));
}(); },
[&](const ExprField& exprField) { auto [obj, field_name] = exprField; } { return [&]() {
auto obj_result = infer_expr(obj, type_env, registry);
return obj_result.with_type(field_lookup_type(obj_result.inferred_type, field_name, registry));
}(); },
[&](const ExprIndex& exprIndex) { auto [obj, idx] = exprIndex; } { return [&]() {
auto obj_result = infer_expr(obj, type_env, registry);
auto idx_result = infer_expr(idx, type_env, registry);
auto elem_type = std::visit(overloaded{[&](const TArray& tArray) { auto [inner] = tArray; } { return inner; },
[&](auto& __v) { return Shared.new(TUnknown{}); }
}, obj_result.inferred_type);
return obj_result.absorb(idx_result).with_type(elem_type);
}(); },
[&](const ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; } { return [&]() {
auto cond_result = infer_expr(cond, type_env, registry);
auto then_result = infer_expr(then_expr, type_env, registry);
auto else_result = infer_expr(else_expr, type_env, registry);
return then_result.absorb(cond_result).absorb(else_result);
}(); },
[&](const ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; } { return [&]() {
auto stmts_result = infer_stmts(stmts, type_env, registry);
auto result_infer = infer_expr(result, stmts_result.type_env, registry);
return result_infer.absorb_stmt(stmts_result);
}(); },
[&](const ExprWhile& exprWhile) { auto [cond, stmts] = exprWhile; } { return [&]() {
auto cond_result = infer_expr(cond, type_env, registry);
auto stmts_result = infer_stmts(stmts, type_env, registry);
return cond_result.absorb_stmt(stmts_result).with_type(Shared.new(TUnit{}));
}(); },
[&](const ExprFor& exprFor) { auto [var, iter, stmts] = exprFor; } { return [&]() {
auto iter_result = infer_expr(iter, type_env, registry);
auto elem_type = std::visit(overloaded{[&](const TArray& tArray) { auto [inner] = tArray; } { return inner; },
[&](auto& __v) { return Shared.new(TUnknown{}); }
}, iter_result.inferred_type);
auto inner_env = type_env;
inner_env.set(var, elem_type);
auto stmts_result = infer_stmts(stmts, inner_env, registry);
return iter_result.absorb_stmt(stmts_result).with_type(Shared.new(TUnit{}));
}(); },
[&](const ExprMatch& exprMatch) { auto [subject, arms] = exprMatch; } { return [&]() {
auto subject_result = infer_expr(subject, type_env, registry);
auto errors = subject_result.errors;
auto arm_type = Shared.new(TUnknown{});
auto i = 0;
while ((i < arms.length())) {
auto arm_env = env_for_pat(type_env, arms[i].pat, registry);
auto arm_result = infer_expr(arms[i].body, arm_env, registry);
errors = errs_append(errors, arm_result.errors);
if ((i == 0)) {
(arm_type = arm_result.inferred_type);
}
i = (i + 1);
}
return InferResult{arm_type, errors};
}(); },
[&](const ExprRecord& exprRecord) { auto [name, field_vals] = exprRecord; } { return infer_field_vals_errors(infer_ok(Shared.new(TNamed(name))), field_vals, type_env, registry); },
[&](const ExprRecordUpdate& exprRecordUpdate) { auto [name, base, field_vals] = exprRecordUpdate; } { return [&]() {
auto base_result = infer_expr(base, type_env, registry);
return infer_field_vals_errors(base_result.with_type(Shared.new(TNamed(name))), field_vals, type_env, registry);
}(); },
[&](const ExprArray& exprArray) { auto [exprs] = exprArray; } { return [&]() {
auto first_type = ((exprs.length() > 0) ? infer_expr(exprs[0], type_env, registry).inferred_type : Shared.new(TUnknown{}));
return infer_args_errors(infer_ok(Shared.new(TArray(first_type))), exprs, type_env, registry);
}(); },
[&](const ExprQuestion& exprQuestion) { auto [inner] = exprQuestion; } { return infer_expr(inner, type_env, registry); },
[&](const ExprLambda& exprLambda) { auto [params, body] = exprLambda; } { return [&]() {
auto lambda_env = type_env;
auto i = 0;
while ((i < params.length())) {
lambda_env.set(params[i], Shared.new(TUnknown{}));
i = (i + 1);
}
auto ret_result = infer_expr(body, lambda_env, registry);
return infer_ok(Shared.new(TFn(mlc::Array<auto>{}, ret_result.inferred_type)));
}(); }
}, expr);
}
StmtInferResult infer_stmts(mlc::Array<std::shared_ptr<Stmt>> stmts, Map<mlc::String, std::shared_ptr<Type>> type_env, TypeRegistry registry) noexcept{
auto errors = mlc::Array<mlc::String>{};
auto current_env = type_env;
auto i = 0;
while ((i < stmts.length())) {
std::visit(overloaded{[&](const StmtLet& stmtLet) { auto [name, __1, __2, value] = stmtLet; } { return [&]() {
auto value_result = infer_expr(value, current_env, registry);
current_env.set(name, value_result.inferred_type);
return (errors = errs_append(errors, value_result.errors));
}(); },
[&](const StmtExpr& stmtExpr) { auto [expr] = stmtExpr; } { return [&]() {
auto expr_res = infer_expr(expr, current_env, registry);
return (errors = errs_append(errors, expr_res.errors));
}(); },
[&](const StmtReturn& stmtReturn) { auto [expr] = stmtReturn; } { return [&]() {
auto ret_res = infer_expr(expr, current_env, registry);
return (errors = errs_append(errors, ret_res.errors));
}(); },
[&](const StmtBreak& stmtBreak) { return /* unit */; },
[&](const StmtContinue& stmtContinue) { return /* unit */; }
}, stmts[i]);
i = (i + 1);
}
return StmtInferResult{current_env, errors};
}
Map<mlc::String, std::shared_ptr<Type>> env_for_pat(Map<mlc::String, std::shared_ptr<Type>> type_env, std::shared_ptr<Pat> pat, TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const PatIdent& patIdent) { auto [name] = patIdent; } { return [&]() {
auto env = type_env;
env.set(name, Shared.new(TUnknown{}));
return env;
}(); },
[&](const PatCtor& patCtor) { auto [ctor_name, sub_pats] = patCtor; } { return [&]() {
auto param_types = registry.ctor_params_for(ctor_name);
auto current_env = type_env;
auto i = 0;
while ((i < sub_pats.length())) {
auto sub_type = ((i < param_types.length()) ? param_types[i] : Shared.new(TUnknown{}));
current_env = env_for_pat_with_type(current_env, sub_pats[i], sub_type, registry);
i = (i + 1);
}
return current_env;
}(); },
[&](auto& __v) { return type_env; }
}, pat);
}
Map<mlc::String, std::shared_ptr<Type>> env_for_pat_with_type(Map<mlc::String, std::shared_ptr<Type>> type_env, std::shared_ptr<Pat> pat, std::shared_ptr<Type> type_val, TypeRegistry registry) noexcept{
return std::visit(overloaded{[&](const PatIdent& patIdent) { auto [name] = patIdent; } { return [&]() {
auto env = type_env;
env.set(name, type_val);
return env;
}(); },
[&](auto& __v) { return env_for_pat(type_env, pat, registry); }
}, pat);
}
bool CheckOut_has_errors(CheckOut self) noexcept{
return (errors.length() > 0);
}
CheckOut check(Program prog) noexcept{
auto globals = collect_globals(prog);
auto registry = build_registry(prog);
auto all_errors = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < prog.decls.length())) {
std::visit(overloaded{[&](const DeclFn& declFn) { auto [__0, params, __2, body] = declFn; } { return [&]() {
auto locals = mlc::Array<mlc::String>{};
auto type_env = Map.new();
auto j = 0;
while ((j < params.length())) {
locals.push_back(params[j].name);
type_env.set(params[j].name, type_from_annotation(params[j].typ));
j = (j + 1);
}
all_errors = errs_append(all_errors, check_names_expr(body, locals, globals));
auto infer_result = infer_expr(body, type_env, registry);
return (all_errors = errs_append(all_errors, infer_result.errors));
}(); },
[&](const DeclType& declType) { auto [__0, __1] = declType; } { return /* unit */; },
[&](const DeclExtend& declExtend) { auto [__0, __1] = declExtend; } { return /* unit */; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return /* unit */; },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return /* unit */; }
}, decl_inner(prog.decls[i]));
i = (i + 1);
}
return CheckOut{all_errors, registry};
}
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept{
return export;
}
bool LexOut_has_errors(LexOut self) noexcept{
return (errors.length() > 0);
}
TKind Token_kind_value(Token self) noexcept{
return kind;
}
int Token_line_number(Token self) noexcept{
return line;
}
int Token_column(Token self) noexcept{
return col;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [d] = declExported; } { return d; },
[&](auto& __v) { return decl; }
}, decl);
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; } { return name; },
[&](const DeclType& declType) { auto [name, __1] = declType; } { return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1] = declExtend; } { return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return decl_name(d); }
}, decl);
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
auto i = 0;
while ((i < src.length())) {
dst.push_back(src[i]);
i = (i + 1);
}
return dst;
}
mlc::Array<std::shared_ptr<FieldOrder>> build_field_orders(Program prog) noexcept{
auto orders = mlc::Array<std::shared_ptr<FieldOrder>>{};
auto i = 0;
while ((i < prog.decls.length())) {
std::visit(overloaded{[&](const DeclExported& declExported) { auto [d] = declExported; } { return std::visit(overloaded{[&](const DeclType& declType) { auto [type_name, variants] = declType; } { return [&]() {
auto j = 0;
while ((j < variants.length())) {
std::visit(overloaded{[&](const VarRecord& varRecord) { auto [variant_name, field_defs] = varRecord; } { return [&]() {
auto field_names = mlc::Array<mlc::String>{};
auto k = 0;
while ((k < field_defs.length())) {
field_names.push_back(field_defs[k].name);
k = (k + 1);
}
orders.push_back(Shared.new(FieldOrder{variant_name, field_names}));
if ((variants.length() == 1)) {
orders.push_back(Shared.new(FieldOrder{type_name, field_names}));
}
auto _d1 = 0;
return /* unit */;
}(); },
[&](const VarTuple& varTuple) { auto [__0, __1] = varTuple; } { return /* unit */; },
[&](const VarUnit& varUnit) { auto [__0] = varUnit; } { return /* unit */; }
}, variants[j]);
j = (j + 1);
}
auto _d2 = 0;
return /* unit */;
}(); },
[&](const DeclFn& declFn) { auto [__0, __1, __2, __3] = declFn; } { return /* unit */; },
[&](const DeclExtend& declExtend) { auto [__0, __1] = declExtend; } { return /* unit */; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return /* unit */; },
[&](const DeclExported& declExported) { auto [__0] = declExported; } { return /* unit */; }
}, decl_inner(d)); },
[&](const DeclType& declType) { auto [type_name, variants] = declType; } { return [&]() {
auto j = 0;
while ((j < variants.length())) {
std::visit(overloaded{[&](const VarRecord& varRecord) { auto [variant_name, field_defs] = varRecord; } { return [&]() {
auto field_names = mlc::Array<mlc::String>{};
auto k = 0;
while ((k < field_defs.length())) {
field_names.push_back(field_defs[k].name);
k = (k + 1);
}
orders.push_back(Shared.new(FieldOrder{variant_name, field_names}));
if ((variants.length() == 1)) {
orders.push_back(Shared.new(FieldOrder{type_name, field_names}));
}
auto _d1 = 0;
return /* unit */;
}(); },
[&](const VarTuple& varTuple) { auto [__0, __1] = varTuple; } { return /* unit */; },
[&](const VarUnit& varUnit) { auto [__0] = varUnit; } { return /* unit */; }
}, variants[j]);
j = (j + 1);
}
auto _d2 = 0;
return /* unit */;
}(); },
[&](const DeclFn& declFn) { auto [__0, __1, __2, __3] = declFn; } { return /* unit */; },
[&](const DeclExtend& declExtend) { auto [__0, __1] = declExtend; } { return /* unit */; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return /* unit */; }
}, prog.decls[i]);
i = (i + 1);
}
return orders;
}
mlc::Array<mlc::String> lookup_fields(mlc::Array<std::shared_ptr<FieldOrder>> orders, mlc::String name) noexcept{
auto result = mlc::Array<mlc::String>{};
auto i = 0;
auto found = false;
while (((i < orders.length()) && (!found))) {
if ((orders[i].type_name == name)) {
result = orders[i].fields;
(found = true);
}
i = (i + 1);
}
return result;
}
mlc::String lower_first(mlc::String name) noexcept{
if ((name.length() == 0)) {
return name;
} else {
return (name.char_at(0).lower() + name.substring(1, name.length()));
}
}
mlc::String type_to_cpp(std::shared_ptr<TypeExpr> type_expr) noexcept{
return std::visit(overloaded{[&](const TyI32& tyI32) { return mlc::String("int"); },
[&](const TyString& tyString) { return mlc::String("mlc::String"); },
[&](const TyBool& tyBool) { return mlc::String("bool"); },
[&](const TyUnit& tyUnit) { return mlc::String("void"); },
[&](const TyNamed& tyNamed) { auto [name] = tyNamed; } { return name; },
[&](const TyArray& tyArray) { auto [inner] = tyArray; } { return ((mlc::String("mlc::Array<") + type_to_cpp(inner)) + mlc::String(">")); },
[&](const TyShared& tyShared) { auto [inner] = tyShared; } { return ((mlc::String("std::shared_ptr<") + type_to_cpp(inner)) + mlc::String(">")); },
[&](const TyGeneric& tyGeneric) { auto [name, args] = tyGeneric; } { return ((mlc::io::args.length() == 0) ? name : ((mlc::io::args.length() == 1) ? (((name + mlc::String("<")) + type_to_cpp(mlc::io::args[0])) + mlc::String(">")) : (((((name + mlc::String("<")) + type_to_cpp(mlc::io::args[0])) + mlc::String(", ")) + type_to_cpp(mlc::io::args[1])) + mlc::String(">")))); },
[&](const TyFn& tyFn) { auto [params, ret] = tyFn; } { return [&]() {
auto param_types = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
param_types.push_back(type_to_cpp(params[i]));
i = (i + 1);
}
return ((((mlc::String("std::function<") + type_to_cpp(ret)) + mlc::String("(")) + param_types.join(mlc::String(", "))) + mlc::String(")>"));
}(); }
}, type_expr);
}
mlc::String escape_str(mlc::String input) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < input.length())) {
auto c = input.char_at(i);
if ((c == mlc::String("\\"))) {
parts.push_back(mlc::String("\\\\"));
} else if ((c == mlc::String("\""))) {
parts.push_back(mlc::String("\\\""));
} else if ((c == mlc::String("\n"))) {
parts.push_back(mlc::String("\\n"));
} else if ((c == mlc::String("\r"))) {
parts.push_back(mlc::String("\\r"));
} else if ((c == mlc::String("\t"))) {
parts.push_back(mlc::String("\\t"));
} else if ((c == mlc::String("\\0"))) {
parts.push_back(mlc::String("\\0"));
} else {
parts.push_back(c);
}





i = (i + 1);
}
return parts.join(mlc::String(""));
}
mlc::String gen_args(mlc::Array<std::shared_ptr<Expr>> args, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < mlc::io::args.length())) {
parts.push_back(gen_expr(mlc::io::args[i], field_orders));
i = (i + 1);
}
return parts.join(mlc::String(", "));
}
mlc::String map_method(mlc::String method_name) noexcept{
if (((method_name == mlc::String("length")) || (method_name == mlc::String("len")))) {
return mlc::String("length");
} else if ((method_name == mlc::String("push"))) {
return mlc::String("push_back");
} else if ((method_name == mlc::String("to_string"))) {
return mlc::String("to_string");
} else if (((method_name == mlc::String("to_int")) || (method_name == mlc::String("to_i")))) {
return mlc::String("to_i");
} else if (((method_name == mlc::String("upper")) || (method_name == mlc::String("to_upper")))) {
return mlc::String("upper");
} else if (((method_name == mlc::String("lower")) || (method_name == mlc::String("to_lower")))) {
return mlc::String("lower");
} else {
return method_name;
}





}
mlc::String map_builtin(mlc::String name) noexcept{
if ((name == mlc::String("print"))) {
return mlc::String("mlc::io::print");
} else if ((name == mlc::String("println"))) {
return mlc::String("mlc::io::println");
} else if ((name == mlc::String("exit"))) {
return mlc::String("mlc::io::exit");
} else if ((name == mlc::String("args"))) {
return mlc::String("mlc::io::args");
} else {
return name;
}



}
mlc::String gen_stmts_str(mlc::Array<std::shared_ptr<Stmt>> stmts, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < stmts.length())) {
parts.push_back(gen_stmt(stmts[i], field_orders));
i = (i + 1);
}
return parts.join(mlc::String(""));
}
mlc::String gen_block_body(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
return std::visit(overloaded{[&](const ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; } { return [&]() {
auto stmts_code = gen_stmts_str(stmts, field_orders);
auto result_code = gen_expr(result, field_orders);
return ((result_code == mlc::String("/* unit */")) ? stmts_code : ((stmts_code + result_code) + mlc::String(";\n")));
}(); },
[&](const ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; } { return gen_stmt_expr(expr, field_orders); },
[&](auto& __v) { return (gen_expr(expr, field_orders) + mlc::String(";\n")); }
}, expr);
}
mlc::String gen_expr(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
return std::visit(overloaded{[&](const ExprInt& exprInt) { auto [n] = exprInt; } { return n.to_string(); },
[&](const ExprStr& exprStr) { auto [s] = exprStr; } { return ((mlc::String("mlc::String(\"") + escape_str(s)) + mlc::String("\")")); },
[&](const ExprBool& exprBool) { auto [b] = exprBool; } { return (b ? mlc::String("true") : mlc::String("false")); },
[&](const ExprUnit& exprUnit) { return mlc::String("/* unit */"); },
[&](const ExprIdent& exprIdent) { auto [name] = exprIdent; } { return [&]() {
auto mapped = map_builtin(name);
auto first = mapped.char_at(0);
return (((first >= mlc::String("A")) && (first <= mlc::String("Z"))) ? (mapped + mlc::String("{}")) : mapped);
}(); },
[&](const ExprBin& exprBin) { auto [op, left_expr, right_expr] = exprBin; } { return ((((((mlc::String("(") + gen_expr(left_expr, field_orders)) + mlc::String(" ")) + op) + mlc::String(" ")) + gen_expr(right_expr, field_orders)) + mlc::String(")")); },
[&](const ExprUn& exprUn) { auto [op, inner] = exprUn; } { return (((mlc::String("(") + op) + gen_expr(inner, field_orders)) + mlc::String(")")); },
[&](const ExprCall& exprCall) { auto [func, args] = exprCall; } { return [&]() {
auto fn_code = std::visit(overloaded{[&](const ExprIdent& exprIdent) { auto [name] = exprIdent; } { return map_builtin(name); },
[&](auto& __v) { return gen_expr(func, field_orders); }
}, func);
return (((fn_code + mlc::String("(")) + gen_args(mlc::io::args, field_orders)) + mlc::String(")"));
}(); },
[&](const ExprMethod& exprMethod) { auto [obj, method_name, args] = exprMethod; } { return [&]() {
auto obj_code = std::visit(overloaded{[&](const ExprIdent& exprIdent) { auto [name] = exprIdent; } { return map_builtin(name); },
[&](auto& __v) { return gen_expr(obj, field_orders); }
}, obj);
return ((obj_code == mlc::String("File")) ? [&]() {
auto cpp_fn = ((method_name == mlc::String("read")) ? mlc::String("mlc::file::read_to_string") : (mlc::String("mlc::file::") + method_name));
return (((cpp_fn + mlc::String("(")) + gen_args(mlc::io::args, field_orders)) + mlc::String(")"));
}() : [&]() {
auto base = (((obj_code + mlc::String(".")) + map_method(method_name)) + mlc::String("("));
return ((mlc::io::args.length() == 0) ? (base + mlc::String(")")) : ((base + gen_args(mlc::io::args, field_orders)) + mlc::String(")")));
}());
}(); },
[&](const ExprField& exprField) { auto [obj, field_name] = exprField; } { return ((gen_expr(obj, field_orders) + mlc::String(".")) + field_name); },
[&](const ExprIndex& exprIndex) { auto [obj, idx] = exprIndex; } { return (((gen_expr(obj, field_orders) + mlc::String("[")) + gen_expr(idx, field_orders)) + mlc::String("]")); },
[&](const ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; } { return ((((((mlc::String("(") + gen_expr(cond, field_orders)) + mlc::String(" ? ")) + gen_expr(then_expr, field_orders)) + mlc::String(" : ")) + gen_expr(else_expr, field_orders)) + mlc::String(")")); },
[&](const ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; } { return ((stmts.length() == 0) ? gen_expr(result, field_orders) : [&]() {
auto body = (((gen_stmts_str(stmts, field_orders) + mlc::String("return ")) + gen_expr(result, field_orders)) + mlc::String(";\n"));
return ((mlc::String("[&]() {\n") + body) + mlc::String("}()"));
}()); },
[&](const ExprWhile& exprWhile) { auto [cond, stmts] = exprWhile; } { return ((((mlc::String("[&]() {\nwhile (") + gen_expr(cond, field_orders)) + mlc::String(") {\n")) + gen_stmts_str(stmts, field_orders)) + mlc::String("}\n}()")); },
[&](const ExprFor& exprFor) { auto [var, iter, stmts] = exprFor; } { return ((((((mlc::String("[&]() {\nfor (auto ") + var) + mlc::String(" : ")) + gen_expr(iter, field_orders)) + mlc::String(") {\n")) + gen_stmts_str(stmts, field_orders)) + mlc::String("}\n}()")); },
[&](const ExprMatch& exprMatch) { auto [subject, arms] = exprMatch; } { return gen_match(subject, arms, field_orders); },
[&](const ExprRecord& exprRecord) { auto [name, field_vals] = exprRecord; } { return [&]() {
auto field_order = lookup_fields(field_orders, name);
auto vals = ((field_order.length() > 0) ? gen_record_ordered(field_vals, field_order, field_orders) : gen_record_unordered(field_vals, field_orders));
return (((name + mlc::String("{")) + vals) + mlc::String("}"));
}(); },
[&](const ExprRecordUpdate& exprRecordUpdate) { auto [name, base, overrides] = exprRecordUpdate; } { return [&]() {
auto field_order = lookup_fields(field_orders, name);
auto vals = gen_record_update_ordered(base, overrides, field_order, field_orders);
return (((name + mlc::String("{")) + vals) + mlc::String("}"));
}(); },
[&](const ExprArray& exprArray) { auto [exprs] = exprArray; } { return ((mlc::String("mlc::Array<auto>{") + gen_args(exprs, field_orders)) + mlc::String("}")); },
[&](const ExprQuestion& exprQuestion) { auto [inner] = exprQuestion; } { return gen_expr(inner, field_orders); },
[&](const ExprLambda& exprLambda) { auto [params, body] = exprLambda; } { return [&]() {
auto capture = ((params.length() == 0) ? mlc::String("[]") : mlc::String("[=]"));
auto param_list = ((params.length() == 0) ? mlc::String("") : [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
parts.push_back((mlc::String("auto ") + params[i]));
i = (i + 1);
}
return parts.join(mlc::String(", "));
}());
auto body_code = gen_expr(body, field_orders);
return (((((capture + mlc::String("(")) + param_list) + mlc::String(") { return ")) + body_code) + mlc::String("; }"));
}(); }
}, expr);
}
mlc::String gen_record_ordered(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::Array<mlc::String> field_order, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_order.length())) {
auto value = find_field_val(field_vals, field_order[i]);
parts.push_back(gen_expr(value, field_orders));
i = (i + 1);
}
return parts.join(mlc::String(", "));
}
mlc::String gen_record_unordered(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_vals.length())) {
parts.push_back(gen_expr(field_vals[i].val, field_orders));
i = (i + 1);
}
return parts.join(mlc::String(", "));
}
mlc::String gen_record_update_ordered(std::shared_ptr<Expr> base, mlc::Array<std::shared_ptr<FieldVal>> overrides, mlc::Array<mlc::String> field_order, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_order.length())) {
auto field_name = field_order[i];
auto override_val = find_field_val(overrides, field_name);
auto val_str = std::visit(overloaded{[&](const ExprUnit& exprUnit) { return gen_expr(Shared.new(ExprField(base, field_name)), field_orders); },
[&](auto& __v) { return gen_expr(override_val, field_orders); }
}, override_val);
parts.push_back(val_str);
i = (i + 1);
}
return parts.join(mlc::String(", "));
}
std::shared_ptr<Expr> find_field_val(mlc::Array<std::shared_ptr<FieldVal>> field_vals, mlc::String name) noexcept{
auto result = Shared.new(ExprUnit{});
auto i = 0;
while ((i < field_vals.length())) {
if ((field_vals[i].name == name)) {
(result = field_vals[i].val);
}
i = (i + 1);
}
return result;
}
mlc::String gen_match(std::shared_ptr<Expr> subject, mlc::Array<std::shared_ptr<MatchArm>> arms, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < arms.length())) {
parts.push_back(gen_arm(arms[i], field_orders));
i = (i + 1);
}
return ((((mlc::String("std::visit(overloaded{") + parts.join(mlc::String(",\n"))) + mlc::String("\n}, ")) + gen_expr(subject, field_orders)) + mlc::String(")"));
}
mlc::String gen_arm(std::shared_ptr<MatchArm> arm, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
return (((gen_pat_header(arm.pat) + mlc::String("{ return ")) + gen_expr(arm.body, field_orders)) + mlc::String("; }"));
}
mlc::String gen_pat_header(std::shared_ptr<Pat> pat) noexcept{
return std::visit(overloaded{[&](const PatWild& patWild) { return mlc::String("[&](auto& __v) "); },
[&](const PatUnit& patUnit) { return mlc::String("[&](auto& __v) "); },
[&](const PatBool& patBool) { auto [__0] = patBool; } { return mlc::String("[&](auto& __v) "); },
[&](const PatInt& patInt) { auto [__0] = patInt; } { return mlc::String("[&](auto& __v) "); },
[&](const PatStr& patStr) { auto [__0] = patStr; } { return mlc::String("[&](auto& __v) "); },
[&](const PatIdent& patIdent) { auto [name] = patIdent; } { return ((mlc::String("[&](auto ") + name) + mlc::String(") ")); },
[&](const PatCtor& patCtor) { auto [name, sub_pats] = patCtor; } { return [&]() {
auto lower_name = lower_first(name);
auto binding = ((sub_pats.length() == 0) ? mlc::String("") : ((((mlc::String("{ auto [") + pat_bind_names(sub_pats)) + mlc::String("] = ")) + lower_name) + mlc::String("; } ")));
return (((((mlc::String("[&](const ") + name) + mlc::String("& ")) + lower_name) + mlc::String(") ")) + binding);
}(); }
}, pat);
}
mlc::String pat_bind_names(mlc::Array<std::shared_ptr<Pat>> sub_pats) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < sub_pats.length())) {
auto bind_name = std::visit(overloaded{[&](const PatIdent& patIdent) { auto [name] = patIdent; } { return name; },
[&](auto& __v) { return (mlc::String("__") + i.to_string()); }
}, sub_pats[i]);
parts.push_back(bind_name);
i = (i + 1);
}
return parts.join(mlc::String(", "));
}
mlc::String gen_stmt(std::shared_ptr<Stmt> stmt, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
return std::visit(overloaded{[&](const StmtLet& stmtLet) { auto [name, __1, typ, value] = stmtLet; } { return [&]() {
auto val_code = std::visit(overloaded{[&](const ExprArray& exprArray) { auto [elems] = exprArray; } { return ((elems.length() == 0) ? std::visit(overloaded{[&](const TyArray& tyArray) { auto [inner] = tyArray; } { return ((mlc::String("mlc::Array<") + type_to_cpp(inner)) + mlc::String(">{}")); },
[&](auto& __v) { return mlc::String("mlc::Array<auto>{}"); }
}, typ) : ((mlc::String("mlc::Array<auto>{") + gen_args(elems, field_orders)) + mlc::String("}"))); },
[&](auto& __v) { return gen_expr(value, field_orders); }
}, value);
return ((((mlc::String("auto ") + name) + mlc::String(" = ")) + val_code) + mlc::String(";\n"));
}(); },
[&](const StmtExpr& stmtExpr) { auto [expr] = stmtExpr; } { return gen_stmt_expr(expr, field_orders); },
[&](const StmtBreak& stmtBreak) { return mlc::String("break;\n"); },
[&](const StmtContinue& stmtContinue) { return mlc::String("continue;\n"); },
[&](const StmtReturn& stmtReturn) { auto [expr] = stmtReturn; } { return ((mlc::String("return ") + gen_expr(expr, field_orders)) + mlc::String(";\n")); }
}, stmt);
}
mlc::String gen_stmt_expr(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
return std::visit(overloaded{[&](const ExprBin& exprBin) { auto [op, left_expr, right_expr] = exprBin; } { return ((op == mlc::String("=")) ? (((gen_expr(left_expr, field_orders) + mlc::String(" = ")) + gen_expr(right_expr, field_orders)) + mlc::String(";\n")) : ((((((mlc::String("(") + gen_expr(left_expr, field_orders)) + mlc::String(" ")) + op) + mlc::String(" ")) + gen_expr(right_expr, field_orders)) + mlc::String(");\n"))); },
[&](const ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; } { return [&]() {
auto output = ((((mlc::String("if (") + gen_expr(cond, field_orders)) + mlc::String(") {\n")) + gen_block_body(then_expr, field_orders)) + mlc::String("}"));
auto else_str = std::visit(overloaded{[&](const ExprUnit& exprUnit) { return mlc::String(""); },
[&](const ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; } { return ((stmts.length() == 0) ? std::visit(overloaded{[&](const ExprUnit& exprUnit) { return mlc::String(""); },
[&](auto& __v) { return ((mlc::String(" else {\n") + gen_block_body(else_expr, field_orders)) + mlc::String("}")); }
}, result) : ((mlc::String(" else {\n") + gen_block_body(else_expr, field_orders)) + mlc::String("}"))); },
[&](const ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; } { return (mlc::String(" else ") + gen_stmt_expr(else_expr, field_orders)); },
[&](auto& __v) { return ((mlc::String(" else {\n") + gen_block_body(else_expr, field_orders)) + mlc::String("}")); }
}, else_expr);
if ((else_str.length() > 0)) {
(output = (output + else_str));
}
return (output + mlc::String("\n"));
}(); },
[&](const ExprWhile& exprWhile) { auto [cond, stmts] = exprWhile; } { return ((((mlc::String("while (") + gen_expr(cond, field_orders)) + mlc::String(") {\n")) + gen_stmts_str(stmts, field_orders)) + mlc::String("}\n")); },
[&](const ExprFor& exprFor) { auto [var, iter, stmts] = exprFor; } { return ((((((mlc::String("for (auto ") + var) + mlc::String(" : ")) + gen_expr(iter, field_orders)) + mlc::String(") {\n")) + gen_stmts_str(stmts, field_orders)) + mlc::String("}\n")); },
[&](const ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; } { return [&]() {
auto stmts_code = gen_stmts_str(stmts, field_orders);
auto result_code = gen_expr(result, field_orders);
return ((result_code == mlc::String("/* unit */")) ? stmts_code : ((stmts_code + result_code) + mlc::String(";\n")));
}(); },
[&](auto& __v) { return (gen_expr(expr, field_orders) + mlc::String(";\n")); }
}, expr);
}
mlc::String gen_return_body(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
return std::visit(overloaded{[&](const ExprBlock& exprBlock) { auto [stmts, result] = exprBlock; } { return [&]() {
auto stmts_code = gen_stmts_str(stmts, field_orders);
auto result_code = gen_expr(result, field_orders);
return ((result_code == mlc::String("/* unit */")) ? stmts_code : (((stmts_code + mlc::String("return ")) + result_code) + mlc::String(";\n")));
}(); },
[&](const ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; } { return [&]() {
auto output = ((((mlc::String("if (") + gen_expr(cond, field_orders)) + mlc::String(") {\n")) + gen_return_body(then_expr, field_orders)) + mlc::String("}"));
auto else_str = std::visit(overloaded{[&](const ExprUnit& exprUnit) { return mlc::String(""); },
[&](const ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; } { return (mlc::String(" else ") + gen_return_if_stmt(else_expr, field_orders)); },
[&](auto& __v) { return ((mlc::String(" else {\n") + gen_return_body(else_expr, field_orders)) + mlc::String("}")); }
}, else_expr);
if ((else_str.length() > 0)) {
(output = (output + else_str));
}
return (output + mlc::String("\n"));
}(); },
[&](auto& __v) { return ((mlc::String("return ") + gen_expr(expr, field_orders)) + mlc::String(";\n")); }
}, expr);
}
mlc::String gen_return_if_stmt(std::shared_ptr<Expr> expr, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
return std::visit(overloaded{[&](const ExprIf& exprIf) { auto [cond, then_expr, else_expr] = exprIf; } { return [&]() {
auto output = ((((mlc::String("if (") + gen_expr(cond, field_orders)) + mlc::String(") {\n")) + gen_return_body(then_expr, field_orders)) + mlc::String("}"));
auto else_str = std::visit(overloaded{[&](const ExprUnit& exprUnit) { return mlc::String(""); },
[&](const ExprIf& exprIf) { auto [__0, __1, __2] = exprIf; } { return (mlc::String(" else ") + gen_return_if_stmt(else_expr, field_orders)); },
[&](auto& __v) { return ((mlc::String(" else {\n") + gen_return_body(else_expr, field_orders)) + mlc::String("}")); }
}, else_expr);
if ((else_str.length() > 0)) {
(output = (output + else_str));
}
return (output + mlc::String("\n"));
}(); },
[&](auto& __v) { return ((mlc::String("if (true) {\n") + gen_return_body(expr, field_orders)) + mlc::String("}\n")); }
}, expr);
}
mlc::String gen_fn_body(std::shared_ptr<Expr> body, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
return gen_return_body(body, field_orders);
}
mlc::String gen_params(mlc::Array<std::shared_ptr<Param>> params) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < params.length())) {
parts.push_back(((type_to_cpp(params[i].typ) + mlc::String(" ")) + params[i].name));
i = (i + 1);
}
return parts.join(mlc::String(", "));
}
mlc::String gen_type_decl_fwd(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept{
if ((variants.length() == 1)) {
return mlc::String("");
} else {
return gen_adt_fwd(type_name, variants);
}
}
mlc::String gen_type_decl_body(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept{
if ((variants.length() == 1)) {
return gen_single_variant(type_name, variants[0]);
} else {
return gen_adt_defs(variants);
}
}
mlc::String gen_single_variant(mlc::String type_name, std::shared_ptr<TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const VarRecord& varRecord) { auto [__0, field_defs] = varRecord; } { return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_defs.length())) {
parts.push_back((((type_to_cpp(field_defs[i].typ) + mlc::String(" ")) + field_defs[i].name) + mlc::String(";")));
i = (i + 1);
}
return ((((mlc::String("struct ") + type_name) + mlc::String(" {")) + parts.join(mlc::String(""))) + mlc::String("};\n"));
}(); },
[&](const VarTuple& varTuple) { auto [__0, field_types] = varTuple; } { return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_types.length())) {
parts.push_back((((type_to_cpp(field_types[i]) + mlc::String(" field")) + i.to_string()) + mlc::String(";")));
i = (i + 1);
}
return ((((mlc::String("struct ") + type_name) + mlc::String(" {")) + parts.join(mlc::String(""))) + mlc::String("};\n"));
}(); },
[&](const VarUnit& varUnit) { auto [__0] = varUnit; } { return ((mlc::String("struct ") + type_name) + mlc::String(" {};\n")); }
}, variant);
}
mlc::String gen_adt_fwd(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto variant_names = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < variants.length())) {
auto variant_name = variant_ctor_name(variants[index]);
variant_names.push_back(variant_name);
parts.push_back(((mlc::String("struct ") + variant_name) + mlc::String(";\n")));
index = (index + 1);
}
return (((((parts.join(mlc::String("")) + mlc::String("using ")) + type_name) + mlc::String(" = std::variant<")) + variant_names.join(mlc::String(", "))) + mlc::String(">;\n"));
}
mlc::String gen_adt_defs(mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < variants.length())) {
parts.push_back(gen_variant_struct(variants[i]));
i = (i + 1);
}
return parts.join(mlc::String(""));
}
mlc::String variant_ctor_name(std::shared_ptr<TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const VarUnit& varUnit) { auto [name] = varUnit; } { return name; },
[&](const VarTuple& varTuple) { auto [name, __1] = varTuple; } { return name; },
[&](const VarRecord& varRecord) { auto [name, __1] = varRecord; } { return name; }
}, variant);
}
mlc::String gen_variant_struct(std::shared_ptr<TypeVariant> variant) noexcept{
return std::visit(overloaded{[&](const VarUnit& varUnit) { auto [name] = varUnit; } { return ((mlc::String("struct ") + name) + mlc::String(" {};\n")); },
[&](const VarTuple& varTuple) { auto [name, field_types] = varTuple; } { return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_types.length())) {
parts.push_back((((type_to_cpp(field_types[i]) + mlc::String(" field")) + i.to_string()) + mlc::String(";")));
i = (i + 1);
}
return ((((mlc::String("struct ") + name) + mlc::String(" {")) + parts.join(mlc::String(""))) + mlc::String("};\n"));
}(); },
[&](const VarRecord& varRecord) { auto [name, field_defs] = varRecord; } { return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto i = 0;
while ((i < field_defs.length())) {
parts.push_back((((type_to_cpp(field_defs[i].typ) + mlc::String(" ")) + field_defs[i].name) + mlc::String(";")));
i = (i + 1);
}
return ((((mlc::String("struct ") + name) + mlc::String(" {")) + parts.join(mlc::String(""))) + mlc::String("};\n"));
}(); }
}, variant);
}
mlc::String gen_fn_decl(mlc::String name, mlc::Array<std::shared_ptr<Param>> params, std::shared_ptr<TypeExpr> return_type, std::shared_ptr<Expr> body, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
if (((name == mlc::String("main")) && (params.length() == 0))) {
auto preamble = mlc::String("mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));\n");
return ((((((type_to_cpp(return_type) + mlc::String(" ")) + name) + mlc::String("(int argc, char** argv) noexcept{\n")) + preamble) + gen_fn_body(body, field_orders)) + mlc::String("}\n"));
} else {
return (((((((type_to_cpp(return_type) + mlc::String(" ")) + name) + mlc::String("(")) + gen_params(params)) + mlc::String(") noexcept{\n")) + gen_fn_body(body, field_orders)) + mlc::String("}\n"));
}
}
mlc::String gen_fn_proto(mlc::String name, mlc::Array<std::shared_ptr<Param>> params, std::shared_ptr<TypeExpr> return_type) noexcept{
if (((name == mlc::String("main")) && (params.length() == 0))) {
return (((type_to_cpp(return_type) + mlc::String(" ")) + name) + mlc::String("(int argc, char** argv) noexcept;\n"));
} else {
return (((((type_to_cpp(return_type) + mlc::String(" ")) + name) + mlc::String("(")) + gen_params(params)) + mlc::String(") noexcept;\n"));
}
}
mlc::String gen_type_decl(mlc::String type_name, mlc::Array<std::shared_ptr<TypeVariant>> variants) noexcept{
return (gen_type_decl_fwd(type_name, variants) + gen_type_decl_body(type_name, variants));
}
mlc::String gen_decl(std::shared_ptr<Decl> decl, mlc::Array<std::shared_ptr<FieldOrder>> field_orders) noexcept{
return std::visit(overloaded{[&](const DeclType& declType) { auto [type_name, variants] = declType; } { return gen_type_decl(type_name, variants); },
[&](const DeclFn& declFn) { auto [name, params, return_type, body] = declFn; } { return gen_fn_decl(name, params, return_type, body, field_orders); },
[&](const DeclExtend& declExtend) { auto [__0, methods] = declExtend; } { return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < methods.length())) {
parts.push_back(gen_decl(methods[index], field_orders));
index = (index + 1);
}
return parts.join(mlc::String(""));
}(); },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return gen_decl(d, field_orders); }
}, decl);
}
mlc::String gen_proto(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclType& declType) { auto [__0, __1] = declType; } { return mlc::String(""); },
[&](const DeclFn& declFn) { auto [name, params, ret, __3] = declFn; } { return gen_fn_proto(name, params, ret); },
[&](const DeclExtend& declExtend) { auto [__0, methods] = declExtend; } { return [&]() {
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < methods.length())) {
parts.push_back(gen_proto(methods[index]));
index = (index + 1);
}
return parts.join(mlc::String(""));
}(); },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return gen_proto(d); }
}, decl);
}
mlc::Array<mlc::String> collect_decl_parts(mlc::Array<std::shared_ptr<Decl>> decls, mlc::Array<std::shared_ptr<FieldOrder>> field_orders, int phase) noexcept{
auto parts = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < decls.length())) {
auto segment = std::visit(overloaded{[&](const DeclType& declType) { auto [type_name, variants] = declType; } { return ((phase == 0) ? gen_type_decl_fwd(type_name, variants) : ((phase == 1) ? gen_type_decl_body(type_name, variants) : mlc::String(""))); },
[&](const DeclFn& declFn) { auto [__0, __1, __2, __3] = declFn; } { return ((phase == 2) ? gen_proto(decls[index]) : ((phase == 3) ? gen_decl(decls[index], field_orders) : mlc::String(""))); },
[&](const DeclExtend& declExtend) { auto [__0, __1] = declExtend; } { return ((phase == 2) ? gen_proto(decls[index]) : ((phase == 3) ? gen_decl(decls[index], field_orders) : mlc::String(""))); },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return ((phase == 2) ? gen_proto(d) : ((phase == 3) ? gen_decl(d, field_orders) : mlc::String(""))); }
}, decls[index]);
if ((segment.length() > 0)) {
parts.push_back(segment);
}
index = (index + 1);
}
return parts;
}
mlc::String gen_program(Program prog) noexcept{
auto field_orders = build_field_orders(prog);
auto preamble = mlc::Array<auto>{mlc::String("#include \"mlc/core/string.hpp\"\n"), mlc::String("#include \"mlc/core/array.hpp\"\n"), mlc::String("#include \"mlc/core/hashmap.hpp\"\n"), mlc::String("#include \"mlc/core/match.hpp\"\n"), mlc::String("#include \"mlc/io/io.hpp\"\n"), mlc::String("#include \"mlc/io/file.hpp\"\n"), mlc::String("#include <functional>\n"), mlc::String("#include <memory>\n"), mlc::String("#include <variant>\n"), mlc::String("#include <vector>\n\n")};
auto type_fwds = collect_decl_parts(prog.decls, field_orders, 0);
auto type_defs = collect_decl_parts(prog.decls, field_orders, 1);
auto fn_protos = collect_decl_parts(prog.decls, field_orders, 2);
auto fn_decls = collect_decl_parts(prog.decls, field_orders, 3);
return errs_append(errs_append(errs_append(errs_append(preamble, type_fwds), type_defs), errs_append(fn_protos, mlc::Array<auto>{mlc::String("\n")})), fn_decls).join(mlc::String(""));
}
std::shared_ptr<T> Shared_new(Shared self, T value) noexcept{
return export;
}
bool LexOut_has_errors(LexOut self) noexcept{
return (errors.length() > 0);
}
TKind Token_kind_value(Token self) noexcept{
return kind;
}
int Token_line_number(Token self) noexcept{
return line;
}
int Token_column(Token self) noexcept{
return col;
}
std::shared_ptr<Decl> decl_inner(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclExported& declExported) { auto [d] = declExported; } { return d; },
[&](auto& __v) { return decl; }
}, decl);
}
mlc::String decl_name(std::shared_ptr<Decl> decl) noexcept{
return std::visit(overloaded{[&](const DeclFn& declFn) { auto [name, __1, __2, __3] = declFn; } { return name; },
[&](const DeclType& declType) { auto [name, __1] = declType; } { return name; },
[&](const DeclExtend& declExtend) { auto [type_name, __1] = declExtend; } { return type_name; },
[&](const DeclImport& declImport) { auto [__0, __1] = declImport; } { return mlc::String(""); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return decl_name(d); }
}, decl);
}
mlc::Array<mlc::String> errs_append(mlc::Array<mlc::String> dst, mlc::Array<mlc::String> src) noexcept{
auto i = 0;
while ((i < src.length())) {
dst.push_back(src[i]);
i = (i + 1);
}
return dst;
}
mlc::String dirname(mlc::String path) noexcept{
auto last_slash = (-1);
auto index = 0;
while ((index < path.length())) {
if ((path.char_at(index) == mlc::String("/"))) {
(last_slash = index);
}
index = (index + 1);
}
return ((last_slash <= 0) ? mlc::String("") : path.substring(0, last_slash));
}
mlc::String resolve_dotdot(mlc::String path) noexcept{
auto p = path;
auto i = 0;
while (((i + 4) <= p.length())) {
if ((p.substring(i, 4) == mlc::String("/../"))) {
auto j = (i - 1);
while (((j >= 0) && (p.char_at(j) != mlc::String("/")))) {
j = (j - 1);
}
auto prefix = ((j <= 0) ? mlc::String("") : p.substring(0, j));
auto suffix = p.substring((i + 4), ((p.length() - i) - 4));
p = ((prefix == mlc::String("")) ? suffix : ((prefix + mlc::String("/")) + suffix));
(i = 0);
}
i = (i + 1);
}
if (((p.length() >= 3) && (p.substring(0, 3) == mlc::String("../")))) {
(p = p.substring(3, (p.length() - 3)));
}
return p;
}
mlc::String resolve_import_path(mlc::String base_path, mlc::String import_path) noexcept{
auto base_dir = dirname(base_path);
auto rest = (((import_path.length() >= 2) && (import_path.substring(0, 2) == mlc::String("./"))) ? import_path.substring(2, (import_path.length() - 2)) : import_path);
auto with_ext = (((rest.length() >= 4) && (rest.substring((rest.length() - 4), 4) == mlc::String(".mlc"))) ? rest : (rest + mlc::String(".mlc")));
auto raw = ((base_dir == mlc::String("")) ? with_ext : ((base_dir + mlc::String("/")) + with_ext));
return resolve_dotdot(raw);
}
bool path_in_loaded(mlc::String path, mlc::Array<mlc::String> loaded) noexcept{
auto found = false;
auto i = 0;
while ((i < loaded.length())) {
if ((loaded[i] == path)) {
(found = true);
}
i = (i + 1);
}
return found;
}
LoadResult load_module_impl(mlc::String path, mlc::Array<mlc::String> loaded, Map<mlc::String, LoadResult> cache) noexcept{
auto norm_path = resolve_dotdot(path);
return (path_in_loaded(norm_path, loaded) ? LoadResult{Program{mlc::Array<auto>{}}, mlc::Array<auto>{(mlc::String("circular: ") + norm_path)}} : [&]() {
loaded.push_back(norm_path);
auto source = mlc::file::read_to_string(path);
return (((source.length() == 0) && (!mlc::file::exists(path))) ? LoadResult{Program{mlc::Array<auto>{}}, mlc::Array<auto>{(mlc::String("file not found: ") + path)}} : [&]() {
auto lex = tokenize(source);
return (lex.has_errors() ? LoadResult{Program{mlc::Array<auto>{}}, mlc::Array<auto>{(((mlc::String("lex ") + path) + mlc::String(": ")) + lex.errors[0])}} : [&]() {
auto prog = parse_program(lex.tokens);
auto all_decls = mlc::Array<std::shared_ptr<Decl>>{};
auto all_errors = mlc::Array<mlc::String>{};
auto index = 0;
while ((index < prog.decls.length())) {
std::visit(overloaded{[&](const DeclImport& declImport) { auto [import_path, __1] = declImport; } { return [&]() {
auto resolved = resolve_dotdot(resolve_import_path(path, import_path));
auto dep_result = load_module_impl(resolved, loaded, cache);
all_errors = errs_append(all_errors, dep_result.errors);
auto dep_index = 0;
return [&]() {
while ((dep_index < dep_result.prog.decls.length())) {
all_decls.push_back(dep_result.prog.decls[dep_index]);
dep_index = (dep_index + 1);
}
}();
}(); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return all_decls.push_back(d); },
[&](auto& __v) { return all_decls.push_back(prog.decls[index]); }
}, prog.decls[index]);
index = (index + 1);
}
return LoadResult{Program{all_decls}, all_errors};
}());
}());
}());
}
LoadResult load_module(mlc::String path, Map<mlc::String, LoadResult> cache) noexcept{
return load_module_impl(path, mlc::Array<auto>{}, cache);
}
MergeResult merge_program(mlc::String entry_path, Program prog) noexcept{
auto all_decls = mlc::Array<std::shared_ptr<Decl>>{};
auto all_errors = mlc::Array<mlc::String>{};
auto cache = Map.new();
auto index = 0;
while ((index < prog.decls.length())) {
std::visit(overloaded{[&](const DeclImport& declImport) { auto [path, __1] = declImport; } { return [&]() {
auto resolved = resolve_import_path(entry_path, path);
auto dep_result = load_module(resolved, cache);
all_errors = errs_append(all_errors, dep_result.errors);
auto dep_index = 0;
return [&]() {
while ((dep_index < dep_result.prog.decls.length())) {
all_decls.push_back(dep_result.prog.decls[dep_index]);
dep_index = (dep_index + 1);
}
}();
}(); },
[&](const DeclExported& declExported) { auto [d] = declExported; } { return all_decls.push_back(d); },
[&](auto& __v) { return all_decls.push_back(prog.decls[index]); }
}, prog.decls[index]);
index = (index + 1);
}
return MergeResult{Program{all_decls}, all_errors};
}
mlc::String compile_source(mlc::String src, mlc::String entry_path) noexcept{
auto lex = tokenize(src);
return (lex.has_errors() ? format_errs(mlc::String("lex"), lex.errors) : [&]() {
auto prog = parse_program(lex.tokens);
auto merged_result = merge_program(entry_path, prog);
return ((merged_result.errors.length() > 0) ? format_errs(mlc::String("import"), merged_result.errors) : [&]() {
auto chk = check(merged_result.prog);
return (chk.has_errors() ? format_errs(mlc::String("check"), chk.errors) : gen_program(merged_result.prog));
}());
}());
}
mlc::String format_errs(mlc::String label, mlc::Array<mlc::String> errors) noexcept{
auto output = mlc::String("");
auto i = 0;
while ((i < errors.length())) {
output = ((((output + label) + mlc::String(": ")) + errors[i]) + mlc::String("\n"));
i = (i + 1);
}
return output;
}
int main(int argc, char** argv) noexcept{
mlc::io::set_args(std::vector<mlc::String>(argv + 1, argv + argc));
auto cli_args = mlc::io::args();
if ((cli_args.length() == 0)) {
mlc::io::println(mlc::String("Usage: mlcc <source.mlc>"));
mlc::io::exit(1);
}
auto src = mlc::file::read_to_string(cli_args[0]);
mlc::io::print(compile_source(src, cli_args[0]));
return 0;
}
