#pragma once
#include "node.hpp"
#include "statement_nodes.hpp"

namespace ast {

  // Maybe should be an abstract class
  class GlobjectNode : public Node {
  private:
  protected:
    GlobjectNode() = default;
  public:
    virtual void codegen() override;
    std::string get_name() override;
  };

  class ProgramNode : public Node {
  private:
    std::vector<std::shared_ptr<GlobjectNode>> globjects;
  public:
    ProgramNode();
    virtual void codegen() override;
    virtual std::string get_name() override;
    void add_obj(std::shared_ptr<GlobjectNode> globject);

    std::vector<std::shared_ptr<Node>> get_children() override;
  };

  class FunctionArgs : public Node {
  private:
    std::vector<std::shared_ptr<DeclarationNode>> args;
  public:
    FunctionArgs();
    void add_arg(std::shared_ptr<DeclarationNode> arg);
    virtual std::string get_name() override;
  };

  class FunctionDeclaration : public Node {
  private:
    //TODO: replace with type representation system
    std::string name;
    std::shared_ptr<FunctionArgs> args;
    std::string return_type;
  public:
    FunctionDeclaration(std::string name, std::shared_ptr<FunctionArgs> args, std::string return_type);
    virtual void codegen() override;
    virtual std::string get_name() override;
  };

  class FunctionGlobject : public GlobjectNode {
  private:
    std::shared_ptr<FunctionDeclaration> decl;
    std::shared_ptr<Block> body;
  protected:
  public:
    FunctionGlobject(std::shared_ptr<FunctionDeclaration> decl, std::shared_ptr<Block> body);
    virtual std::string get_name() override;
    virtual std::vector<std::shared_ptr<Node>> get_children() override;
  };
}
