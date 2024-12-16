%language "c++"
%parse-param {FrogLexer &lexer}
%define parse.error verbose

%code requires {
    #include "tokens.hpp"
    #include "froglexer.hpp"
}
%code {
    #define yylex lexer.yylex
}
%union {
    Tokens::Token* token;
}
%destructor { delete $$; } <token>
%{

%}
//TODO: finish inputting tokens
%token <token> NUMBER
%token <token> IDENTIFIER
%token <token> TYPE_ID

%token ARROW

%token PLUS
%token MINUS
%token STAR
%token SLASH

%token ASSIGNMENT

%token FUNCTION
%token RETURN

%token LPAREN
%token RPAREN

%token LBRACE
%token RBRACE
%token SEMICOLON
%token COMMA
%token COLON

%left PLUS MINUS
%left STAR SLASH

%%
program: global_objs {std::cout<<"FINISHED\n";}
    ;

global_objs: global_obj
    | global_obj global_objs
    ;

global_obj: function {std::cout<<"DECLARATION\n";}
    ;

function: function_declaration block {std::cout<<"FUNC\n";}
    ;

function_declaration: FUNCTION IDENTIFIER arglist ARROW TYPE_ID {std::cout<<"DECLARED dunction\n";}
    ;

block: LBRACE statements RBRACE {std::cout<<"BLOCK\n";}
    ;

statements: /* empty */
    | statement statements {std::cout<<"combined\n";}
    ;

statement: expression SEMICOLON {std::cout<<"STATEMENT\n";}
    | declaration SEMICOLON 
    | declaration ASSIGNMENT expression SEMICOLON {}
    | RETURN expression SEMICOLON {std::cout<<"RETURNED\n";}
    ;

declaration: IDENTIFIER COLON TYPE_ID {std::cout<<"DECLARED\n";}
    ;

call_arglist: /* empty */
    | expression
    | expression COMMA call_arglist
    ;

expression: NUMBER {std::cout<<"Converted\n";}
    | IDENTIFIER
    | IDENTIFIER LPAREN call_arglist RPAREN
    | LPAREN expression RPAREN
    | expression PLUS expression {std::cout<<"Added\n";}
    | expression MINUS expression {std::cout<<"Subtracted\n";}
    | expression STAR expression {std::cout<<"Multiplied\n";}
    | expression SLASH expression {std::cout<<"Divided\n";}
    ;

args:  /*empty*/
    | declaration
    | declaration COMMA args
    ;

arglist: LPAREN args RPAREN {std::cout<<"FUNCTION ARGS\n";}
    ;
%%

void yy::parser::error(const std::string &message)
{
    std::cerr << "Error: " << message << std::endl;
}
