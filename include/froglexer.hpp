#pragma once
#include "tokens.hpp"

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

class FrogLexer : public yyFlexLexer
{
public:
    int yylex(YYSTYPE* const yylval);
    FrogLexer(std::istream* input)
        : yyFlexLexer(input) {}
};
