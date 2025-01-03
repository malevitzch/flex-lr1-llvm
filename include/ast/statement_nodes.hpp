#pragma once
#include "node.hpp"
#include "expression_nodes.hpp"

namespace ast {

  // Abstract class
  class StatementNode : public Node {
  private:
  protected:
    StatementNode() = default;
    virtual ~StatementNode() = default;
  public:
    virtual std::string get_name() override;
    virtual void codegen() override;
  };

  // This inherits from Node directly because a bundle of statements is not a statement.
  class Statements : public Node {
  private:
    std::vector<std::shared_ptr<Node>> statements;
  public:
    Statements() = default;
    virtual ~Statements() = default;
    virtual std::string get_name() override;
    virtual void codegen() override;
    void add_statement(std::shared_ptr<StatementNode> statement);
    virtual std::vector<std::shared_ptr<Node>> get_children() override;
  };

  class Block : public StatementNode {
  private:
    std::shared_ptr<Statements> statements;
  public:
    Block(std::shared_ptr<Statements> statements);
    virtual ~Block() = default;
    virtual void codegen() override;
    virtual std::string get_name() override;
    virtual std::vector<std::shared_ptr<Node>> get_children() override;

  };

  class DeclarationNode : public Node {
  private:
    std::string var_type;
    std::string var_name;
  public:
    DeclarationNode(std::string var_type, std::string var_name);
    virtual void codegen() override;
    std::string get_name() override;
    virtual std::vector<std::shared_ptr<Node>> get_children() override;
  friend class DeclarationStatement;

  };

  class ExpressionStatement : public StatementNode {
  private:
    std::shared_ptr<ExprNode> expr;
  public:
    ExpressionStatement(std::shared_ptr<ExprNode> expr);
    virtual void codegen() override;
    virtual std::string get_name() override;
    virtual std::vector<std::shared_ptr<Node>> get_children() override; 
  };

  class DeclarationStatement : public StatementNode {
  private:
    std::shared_ptr<DeclarationNode> decl;
  public:
    DeclarationStatement(std::shared_ptr<DeclarationNode> decl);
    virtual void codegen() override;
    virtual std::string get_name() override;
    virtual std::vector<std::shared_ptr<Node>> get_children() override;
  };

}
