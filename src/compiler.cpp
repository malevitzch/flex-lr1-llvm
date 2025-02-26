#include "compiler.hpp"
#include "ast/globals.hpp"
#include "tokens.hpp"
#include "parser.hpp"
#include "libgen/print.hpp"

#include <llvm/Support/raw_ostream.h>
#include <llvm/Support/raw_os_ostream.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetMachine.h>
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Instructions.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Type.h"
#include "llvm/IR/Verifier.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/TargetParser/Host.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/Host.h"
#include <iostream>
#include <fstream>

std::shared_ptr<ast::Node> ast_root;

void Compiler::prepare_llvm() {
  llvm::InitializeAllTargetInfos();
  llvm::InitializeAllTargets();
  llvm::InitializeAllTargetMCs();
  llvm::InitializeAllAsmParsers();
  llvm::InitializeAllAsmPrinters();
}

std::optional<std::string> Compiler::get_compiler_path() {
  static const std::vector<std::string> possible_compilers = {"clang", "gcc", "cc", "cl"};
  std::optional<std::string> compiler_path = std::nullopt;
  for(const std::string& compiler : possible_compilers) {
    llvm::ErrorOr<std::string> error_or_compiler_path = llvm::sys::findProgramByName(compiler);
    if(error_or_compiler_path) {
      compiler_path = error_or_compiler_path.get();
      break;
    }
  }
  return compiler_path;
}

std::optional<std::string> Compiler::generate_IR(std::istream* input_stream) {
  FrogLexer lexer(input_stream);
  Tokens::Token *yylval = new Tokens::Token();
  yy::parser p(lexer);
  int parser_return_val = p();
  delete yylval;
  if(parser_return_val != 0) {
    return "Parsing error";
  }
  dynamic_pointer_cast<ast::ProgramNode>(ast_root)->codegen();
  ast_root = nullptr;
  //FIXME: implement AST error handling
  return std::nullopt;
}

std::optional<std::string> Compiler::compile_to_IR(std::string input_filename, std::string output_filename) {
  std::ifstream input_stream(input_filename);
  return compile_to_IR(&input_stream, output_filename);
}

Compiler::Compiler(std::ostream* debug_output_stream) {
  diagnostic_stream = debug_output_stream;
}

std::optional<std::string> Compiler::compile_to_IR(std::istream* input_stream, std::string output_filename) {
  generate_IR(input_stream);
  std::error_code EC;
  llvm::raw_fd_ostream IR_out(output_filename, EC, llvm::sys::fs::OF_None);

  if(EC) {
    return "Error opening file \"" + output_filename + "\": " + EC.message();
  }

  CompilerContext::TheModule->print(IR_out, nullptr);
  return std::nullopt;
}

std::optional<std::string> Compiler::compile_to_obj(std::istream* input_stream, std::string output_filename) {
  auto TargetTriple = llvm::sys::getDefaultTargetTriple();

  prepare_llvm();

  std::string Error;
  auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

  auto CPU = "generic";
  auto Features = "";
  llvm::TargetOptions opt;
  auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, llvm::Reloc::PIC_);

  std::optional<std::string> parsing_error = generate_IR(input_stream);

  if(parsing_error) {
    return "Parsing failed due to: \"" + *parsing_error + "\""; 
  }

  //FIXME: we do not necessarily want to emit the tree_output.txt every time in the finished compiler
  //std::ofstream ast_out("tree_output.txt");
  //TODO: bring back the tree_output dfs because it was a cool thing to have

  CompilerContext::TheModule->setDataLayout(TargetMachine->createDataLayout());
  CompilerContext::TheModule->setTargetTriple(TargetTriple);
  std::error_code EC;

  llvm::raw_fd_ostream dest(output_filename, EC, llvm::sys::fs::OF_None);
  llvm::legacy::PassManager pass;

  // This is the version used by modern LLVM
  auto FileType = llvm::CGFT_ObjectFile;
  if(TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    return "Something went wrong with LLVM during compilation";
  }
  pass.run(*CompilerContext::TheModule);
  dest.flush();
  return std::nullopt;
}

std::optional<std::string> Compiler::compile_to_obj(std::string input_filename, std::string output_filename) {
  std::ifstream input_stream(input_filename);
  if(!input_stream.is_open()) {
    return "Cannot open file \"" + input_filename + "\"";
  }
  return compile_to_obj(&input_stream, output_filename);
}

std::optional<std::string> Compiler::compile_to_exec(std::istream* input_stream, std::string output_filename) {
  std::cout << "TEST" << std::endl;
  std::optional<std::string> compilation_error = compile_to_obj(input_stream, "out.o");
  if(compilation_error) {
    return "Compilation to object failed due to: \"" + *compilation_error + "\"";
  }

  std::optional<std::string> compiler_path = get_compiler_path();
  if(!compiler_path) {
    return "Can't find a C compiler";
  }
  std::vector<std::string> args = {*compiler_path, "-o", output_filename, "out.o", STDLIB_PATH};
  llvm::SmallVector<llvm::StringRef, 16> argv;
  for(auto& arg : args) {
    argv.push_back(arg);
  }
  std::string error_message;
  int result = llvm::sys::ExecuteAndWait(argv[0], argv, std::nullopt, {}, 0, 0, &error_message);
  if(result == -1) {
    return "Cannot execute the compilation command";
  }
  else if(result == -2) {
    return "The compiler used for linking crashed";
  }
  return std::nullopt;
}

std::optional<std::string> Compiler::compile_to_exec(std::vector<std::string> filenames, std::string output_filename) {
  for(std::string filename : filenames) {
    CompilerContext::reset_context();
    //FIXME: validate the stream
    std::ifstream input_stream(filename);
    //FIXME: remove extension until first dot and add generate the object filename?
    std::optional<std::string> compilation_error = compile_to_obj(&input_stream, filename + ".o");
    if(compilation_error) {
      return "Compilation of file \"" + filename + "\" to object failed due to: \"" + *compilation_error + "\"";
    }
  }
  std::optional<std::string> compiler_path = get_compiler_path();
  if(!compiler_path) {
    return "Can't find a C compiler";
  }
  std::vector<std::string> args = {*compiler_path, "-o", output_filename, STDLIB_PATH};
  for(std::string filename : filenames) {
    args.push_back(filename + ".o");
  }
  llvm::SmallVector<llvm::StringRef, 16> argv;
  for(auto& arg : args) {
    argv.push_back(arg);
  }
  std::string error_message;
  int result = llvm::sys::ExecuteAndWait(argv[0], argv, std::nullopt, {}, 0, 0, &error_message);
  if(result == -1) {
    return "Cannot execute the compilation command";
  }
  else if(result == -2) {
    return "The compiler used for linking crashed";
  }
  return std::nullopt;

}

std::optional<std::string> Compiler::compile_to_exec(std::string input_filename, std::string output_filename) {
  std::ifstream input_stream(input_filename);
    if(!input_stream.is_open()) {
      return "Cannot open file: \"" + input_filename + "\"";
  }
  return compile_to_exec(&input_stream, output_filename);
}

std::optional<std::string> Compiler::compile_stdlib() {
  auto TargetTriple = llvm::sys::getDefaultTargetTriple();

  prepare_llvm();

  std::string Error;
  auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

  auto CPU = "generic";
  auto Features = "";
  llvm::TargetOptions opt;
  auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, llvm::Reloc::PIC_);

  libgen::register_print_i32();
  //TODO: here goes the thing

  CompilerContext::TheModule->setDataLayout(TargetMachine->createDataLayout());
  CompilerContext::TheModule->setTargetTriple(TargetTriple);
  std::error_code EC;

  llvm::raw_fd_ostream dest("froglib.o", EC, llvm::sys::fs::OF_None);
  llvm::legacy::PassManager pass;

  // This is the version used by modern LLVM
  auto FileType = llvm::CGFT_ObjectFile;
  if(TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType)) {
    return "Something went wrong with LLVM during compilation";
  }
  pass.run(*CompilerContext::TheModule);
  dest.flush();
  return std::nullopt;
}

std::optional<std::string> Compiler::compile_from_args(std::vector<std::string> args) {
  //FIXME: maybe split it into smaller, iterator-based functions?
  //TODO: setting a mode should only be done once (warn the user in case of multiple options that override each other)
  std::vector<std::string>::iterator arg_it = args.begin();
  std::string output_name = "";
  std::vector<std::string> sources;
  std::string mode = "exec";
  while(arg_it != args.end()) {
    std::string arg = *arg_it;
    arg_it++;
    if(arg.empty()) {
      return "Empty argument";
    }
    if(arg[0] == '-') {
      if(arg.size() == 1) {
        return "\"-\" is not a valid option";
      }
      if(arg.size() > 1) {
        std::string option = arg.substr(1, arg.size() - 1);
        if(option == "o") {
          if(arg_it == args.end()) {
            return "The \"-o\" option requires an argument";
          }
          //TODO: validate the arg as valid output name
          arg = *arg_it;
          output_name = arg;
          arg_it++;
        }
        else if(option == "ir") {
          mode = "IR";
        }
        else if(option == "c") {
          mode = "obj";
        }
        else if(option == "genstdlib") {
          mode = "stdlib";
        }
        else {
          return "Unknown option: -\"" + option + "\"";
        }
      }
    }
    else {
      sources.push_back(arg);
      //TODO: change this to an else if, validate the filename
    }
  }
  if(mode == "stdlib") {
    CompilerContext::reset_context();
    compile_stdlib();
    return std::nullopt;
  }
  if(sources.empty()) {
    return "No source file given";
  }
  if(mode == "exec") {
    if(output_name.empty()) output_name = "exec";
    CompilerContext::reset_context();
    return compile_to_exec(sources, output_name);
  }
  if(mode == "IR") {
    if(output_name.empty()) output_name = "IR";
    CompilerContext::reset_context();
    return compile_to_IR(sources[0], output_name);
  }
  if(mode == "obj") {
    if(output_name.empty()) output_name = "obj.o";
    CompilerContext::reset_context();
    return compile_to_obj(sources[0], output_name);
  }
  return "The compiler couldn't deduce the compilation mode";
}
