#include "ast/globject_nodes.hpp"

namespace ast {

  std::string GlobjectNode::get_name() {
    return "Globject Node";
  }

  void ProgramNode::codegen() {
    for(std::shared_ptr<GlobjectNode> globject : globjects)
      globject->codegen();
  }
  std::string ProgramNode::get_name() {
    return "Program Node";
  }
  void ProgramNode::add_obj(std::shared_ptr<GlobjectNode> globject) {
    globjects.push_back(globject);
  }
  ProgramNode::ProgramNode() {}
  std::vector<std::shared_ptr<Node>> ProgramNode::get_children() {
  std::vector<std::shared_ptr<Node>> children;
    for(std::shared_ptr<GlobjectNode> globject : globjects) 
      children.push_back(static_pointer_cast<Node>(globject));
    return children;
  }

  std::string FunctionArgs::get_name() {
    return "Function Args";
  }
  std::vector<std::shared_ptr<Node>> FunctionArgs::get_children() {
    std::vector<std::shared_ptr<Node>> children;
    for(std::shared_ptr<DeclarationNode> arg : args) 
      children.push_back(arg);
    return children;
  }
  void FunctionArgs::add_arg(std::shared_ptr<DeclarationNode> arg) {
    args.push_back(arg);
  }
  std::vector<std::shared_ptr<DeclarationNode>> FunctionArgs::get_args() {
    return args;
  }
  std::vector<std::shared_ptr<DeclarationNode>> FunctionArglist::get_args() {
    return args->get_args();
  }

  FunctionArglist::FunctionArglist(std::shared_ptr<FunctionArgs> args)
  : args(args) {}
  std::string FunctionArglist::get_name() {
    return "Function Arglist";
  }
  std::vector<std::shared_ptr<Node>> FunctionArglist::get_children() {
    return {args};
  }

  FunctionDeclaration::FunctionDeclaration(std::string name, std::shared_ptr<FunctionArglist> args, std::string return_type) 
  : name(name), args(args), return_type(return_type) {}
  void FunctionDeclaration::codegen() {
    std::vector<std::string> variables_to_clean_up;
    for(std::shared_ptr<DeclarationNode> arg : args->get_args()) {
      variables_to_clean_up.push_back(arg->get_varname());
    }
    //TODO: IMPLEMENT everything that actually matters
    
    for(std::string varname : variables_to_clean_up) {
      CompilerContext::NamedValues->remove_val(varname);
    }
    //TODO: Probably in reverse in the future in case of type checks? Doesn't really matter though on a second thought. Stil, better to keep this in mind
  }
  std::string FunctionDeclaration::get_name() {
    return "Function Declaration";
  }
  std::vector<std::shared_ptr<Node>> FunctionDeclaration::get_children() {
    return {args};
  }

  FunctionGlobject::FunctionGlobject(std::shared_ptr<FunctionDeclaration> decl, std::shared_ptr<Block> body) 
  : decl(decl), body(body) {}
  void FunctionGlobject::codegen() {
  }
  std::string FunctionGlobject::get_name() {
    return "Function Globject";
  }
  std::vector<std::shared_ptr<Node>> FunctionGlobject::get_children() {
    return {decl, body};
  }
}
