#include <iostream>
#include "node.hpp"

int main()
{
  using namespace Calculator;

  node_ptr_t nodeOne = ASTNode::formConstantNode( 3.14159265358979 / 2 );
  node_ptr_t nodeNeg = ASTNode::formNegationNode( nodeOne );
  node_ptr_t nodeFunc = ASTNode::formFunctionNode( nodeNeg, std::sin );

  std::cout << nodeFunc->calculate() << std::endl;

  node_ptr_t nodeTwo = ASTNode::formConstantNode( 5. );
  node_ptr_t nodeSum = ASTNode::formSummationNode( nodeFunc, nodeTwo );

  std::cout << nodeSum->calculate() << std::endl;

  var_map_t varMap;
  node_ptr_t nodeVar = ASTNode::formVariableNode( "simpleVariable", varMap );
  node_ptr_t nodeAssign = ASTNode::formAssignmentNode( nodeVar, nodeSum );

  std::cout << nodeAssign->calculate() << std::endl;

  for (const auto& item : varMap)
    std::cout << item.first << "\t" << item.second.m_Value << std::endl;

  node_ptr_t nodeThree = ASTNode::formConstantNode( 3 );
  node_ptr_t nodeMultiSum = ASTNode::formMultipleMultiplicationNode( nodeFunc );
  nodeMultiSum->addChildNode( nodeTwo, false );
  nodeMultiSum->addChildNode( nodeThree, true );

  std::cout << nodeMultiSum->calculate() << std::endl;

  return 0;
}