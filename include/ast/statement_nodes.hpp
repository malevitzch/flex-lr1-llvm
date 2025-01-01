#pragma once
#include "node.hpp"
#include "expression_nodes.hpp"

namespace ast {
  class StatementNode : public Node {
  private:
  protected:
    StatementNode() = default;
    ~StatementNode() = default;
  public:
  };

  // This inherits from Node directly because a bundle of statements is not a statement.
  class Statements : public Node {
  private:
    std::vector<std::shared_ptr<StatementNode>> statements;
  public:
    virtual llvm::Value* codegen() override;
    void add_statement(std::shared_ptr<StatementNode> statement);
  };
  class Block : public StatementNode {
  private:
    Statements statements;
  public:
  };

  class ExpressionStatement : public StatementNode {
  private:
    std::shared_ptr<ExprNode> expr;
  public:
  };

  //FIXME: this needs a declaration node to even be implemented
  class DeclarationStatement : public StatementNode {
  };

}