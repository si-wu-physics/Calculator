#include "parser.hpp"

namespace Calculator
{
  Parser::Parser( Scanner& scanner,
                  var_map_t& varMap,
                  const func_map_t& funcMap ) :
    m_Scanner( scanner ),
    m_Root(),
    m_Status( Status::OK ),
    m_VarMap( varMap ),
    m_FuncMap( funcMap )
  { }

  Parser::~Parser( void )
  { }

  Parser::Status Parser::parse( void )
  {
    m_Root = expression();
    if ( !m_Scanner.isDone() )
      m_Status = Status::ERROR;

    return m_Status;
  }

  double Parser::calculate( void ) const
  {
    if (!m_Root)
      throw("No valid AST formed!");
    return m_Root->calculate();
  }

  void Parser::print( void ) const
  {
    if (!m_Root)
      throw("No valid AST formed!");
    return m_Root->print( 0 );
  }

  node_ptr_t Parser::expression( void )
  {
    node_ptr_t pNode = term();
    Scanner::Token token = m_Scanner.token();

    if ( token == Scanner::Token::PLUS || token == Scanner::Token::MINUS )
    {
      node_ptr_t pSumNode = ASTNode::formMultipleSummationNode( pNode );

      do
      {
        m_Scanner.accept();
        node_ptr_t pNewNode = term();
        pSumNode->addChildNode( pNewNode, token == Scanner::Token::PLUS );
        token = m_Scanner.token();
      } while ( token == Scanner::Token::PLUS || token == Scanner::Token::MINUS );
      pNode = pSumNode;
    }
    else if ( token == Scanner::Token::ASSIGN )
    {
      m_Scanner.accept();
      node_ptr_t pRightNode = expression();

      if ( pNode->isLValue() )
      {
        pNode = ASTNode::formAssignmentNode( pNode, pRightNode );
      }
      else
      {
        m_Status = Status::ERROR;
        pNode = expression();
      }
    }

    return pNode;
  }

  node_ptr_t Parser::term( void )
  {
    node_ptr_t pNode = factor();
    Scanner::Token token = m_Scanner.token();

    if ( token == Scanner::Token::MULTIPLY || token == Scanner::Token::DIVIDE )
    {
      node_ptr_t pProductNode = ASTNode::formMultipleMultiplicationNode( pNode );

      do
      {
        m_Scanner.accept();
        node_ptr_t pNewNode = factor();
        pProductNode->addChildNode( pNewNode, token == Scanner::Token::MULTIPLY );
        token = m_Scanner.token();
      } while ( token == Scanner::Token::MULTIPLY || token == Scanner::Token::DIVIDE );
      pNode = pProductNode;
    }

    return pNode;
  }

  node_ptr_t Parser::factor( void )
  {
    node_ptr_t pNode = simpleFactor();
    Scanner::Token token = m_Scanner.token();

    if ( token == Scanner::Token::POWER )
    {
      m_Scanner.accept();
      node_ptr_t pNewNode = factor();
      pNode = ASTNode::formPowerNode( pNode, pNewNode );
    }

    return pNode;
  }

  node_ptr_t Parser::simpleFactor( void )
  {
    node_ptr_t pNode;
    Scanner::Token token = m_Scanner.token();

    if ( token == Scanner::Token::LEFT_PAREN )
    {
      m_Scanner.accept();
      pNode = expression();
      if ( m_Scanner.token() == Scanner::Token::RIGHT_PAREN )
        m_Scanner.accept();
      else
        m_Status = Status::ERROR;
    }
    else if ( token == Scanner::Token::NUMBER )
    {
      pNode = ASTNode::formConstantNode( m_Scanner.value() );
      m_Scanner.accept();
    }
    else if ( token == Scanner::Token::IDENTIFIER )
    {
      std::string id = m_Scanner.getID();
      m_Scanner.accept();

      if ( m_Scanner.token() == Scanner::Token::LEFT_PAREN )
      {
        if (m_FuncMap.count(id))
        {
          real_function_t pFunc = m_FuncMap.at(id);

          m_Scanner.accept();
          pNode = expression();
          if ( m_Scanner.token() == Scanner::Token::RIGHT_PAREN )
            m_Scanner.accept();
          else
            m_Status = Status::ERROR;

          pNode = ASTNode::formFunctionNode( pNode, pFunc, id );
        }
        else
        {
          throw("Unknow function encountered!");
        }
      }
      else
      {
        pNode = ASTNode::formVariableNode( id, m_VarMap );
      }
    }
    else if ( token == Scanner::Token::MINUS )
    {
      m_Scanner.accept();
      pNode = ASTNode::formNegationNode( factor() );
    }
    else if ( token == Scanner::Token::PLUS )
    {
      m_Scanner.accept();
      pNode = ASTNode::formIdentityNode( factor() );
    }
    else
    {
      m_Scanner.accept();
      m_Status = Status::ERROR;
    }

    return pNode;
  }
}