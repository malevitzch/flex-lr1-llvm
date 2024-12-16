%{
#include "tokens.hpp"
#include "parser.hpp"
#include "froglexer.hpp"

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using namespace Tokens;

const std::unordered_map<std::string, std::pair<Tokens::TokType, int> > keywords = {
  //{"if", Tokens::TokType::IF},
  //{"loop", Tokens::TokType::LOOP},
  {"return", Tokens::TokType::RETURN},
  {"function", {Tokens::TokType::FUNCTION, yy::parser::token::FUNCTION}},
};

std::unordered_set<std::string> types = {
  "int",
  "float",
};
#undef  YY_DECL
#define YY_DECL int FrogLexer::yylex(YYSTYPE *const yylval)
%}

%option c++
%option noyywrap
%option yyclass="FrogLexer"

%x DECL
%%

[a-zA-Z_]+[a-zA-Z_0-9]* {
  if(keywords.contains(yytext)) {
    auto&[type, yy_type] = keywords.at(yytext);
    std::cout << "Keyword : [" << yytext << "]\n";
    yylval->token = Tokens::Token(type, yytext);
    return yy_type; 
  }
  else if(types.contains(yytext)) {
    std::cout << "TYPE_ID(" << yytext << ")\n";
    yylval->token = Tokens::Token(Tokens::TokType::TYPE_ID, yytext);
    return yy::parser::token::TYPE_ID;
  }
  else {
    std::cout << "IDENTIFIER(" << yytext << ")\n";
    yylval->token = Tokens::Token(Tokens::TokType::IDENTIFIER, yytext);
    return yy::parser::token::IDENTIFIER;
  }
}
[0-9]+ {
  std::cout << "NUMBER(" << yytext << ")\n";
  yylval->token = Tokens::Token(Tokens::TokType::NUMBER, yytext);
  return yy::parser::token::NUMBER;
}
"->" {
  std::cout << "ARROW\n";
  yylval->token = Tokens::Token(Tokens::TokType::ARROW, yytext);
  return yy::parser::token::ARROW;
}
"+" { 
  std::cout << "PLUS\n";
  yylval->token = Tokens::Token(Tokens::TokType::PLUS, yytext);
  return yy::parser::token::PLUS;
}
"-" {
  std::cout << "MINUS\n"; 
  yylval->token = Tokens::Token(Tokens::TokType::MINUS, yytext);
  return yy::parser::token::MINUS;
}
"*" {
  std::cout << "STAR\n";
  yylval->token = Tokens::Token(Tokens::TokType::STAR, yytext);
  return yy::parser::token::STAR;
}
"/" {
  std::cout << "SLASH\n";
  yylval->token = Tokens::Token(Tokens::TokType::SLASH, yytext);
  return yy::parser::token::SLASH;
}
"=" {
  std::cout << "ASSIGNMENT\n";
  yylval->token = Tokens::Token(Tokens::TokType::ASSIGNMENT, yytext);
  return yy::parser::token::ASSIGNMENT;
}
"(" {
  std::cout << "LPAREN\n"; 
  yylval->token = Tokens::Token(Tokens::TokType::LPAREN, yytext);
  return yy::parser::token::LPAREN;
}
")" {
  std::cout << "RPAREN\n"; 
  yylval->token = Tokens::Token(Tokens::TokType::RPAREN, yytext);
  return yy::parser::token::RPAREN;
}
"{" {
  std::cout << "LBRACE\n";
  yylval->token = Tokens::Token(Tokens::TokType::LBRACE, yytext);
  return yy::parser::token::LBRACE;
}
"}" {
  std::cout << "RBRACE\n"; 
  yylval->token = Tokens::Token(Tokens::TokType::RBRACE, yytext);
  return yy::parser::token::RBRACE;
}
"," {
  std::cout << "COMMA\n";
  yylval->token = Tokens::Token(Tokens::TokType::COMMA, yytext);
  return yy::parser::token::COMMA;
}
":" {
  std::cout << "COLON\n";
  yylval->token = Tokens::Token(Tokens::TokType::COLON, yytext);
  return yy::parser::token::COLON;
}
";" {
  std::cout << "SEMICOLON\n";
  yylval->token = Tokens::Token(Tokens::TokType::SEMICOLON, yytext);
  return yy::parser::token::SEMICOLON;
}
[ \t\n]+ {
  /* Ignore whitespace */ 
}

.   {
  std::cout << "UNKNOWN TOKEN: " << yytext << "\n"; 
}

%%

