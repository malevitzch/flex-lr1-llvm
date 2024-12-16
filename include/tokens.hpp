#pragma once 
#include <string>
namespace Tokens {
  enum class TokType {

    TYPE_ID,
    IDENTIFIER,
    NUMBER,

    ARROW,
    PLUS,
    MINUS,
    STAR, //multiply (but I think this is better since it might mean deref later so probably worth to do this way)
    SLASH,
    ASSIGNMENT,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    COMMA,
    COLON,
    SEMICOLON,

    RETURN,
    FUNCTION,

    IF,
    ELSE,
    LOOP, //replacement of a while loop
    FOR, //there should be a for loop cause it's convenient

    UNKNOWN,

  };

  struct Token {
    TokType type;
    std::string metadata;
    Token(TokType type, std::string metadata);
    Token(TokType type);
    Token() = default;
  };

}
typedef union {
    Tokens::Token* token;
} YYSTYPE;
