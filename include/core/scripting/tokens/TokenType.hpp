#pragma once

namespace pg::script {

	enum class TokenType : char {
		// ERROR
		MISSING,
	  // UNARY KEYWORDS
	  IF,
	  ELIF,
	  ELSE,
	  FOR,
	  WHILE,
	  RETURN,
	  AS,
	  LET,
	  USE,
	  IN,
	  EXIT,
	  // SOLO KEYWORDS
	  END,
	  DONE,
	  // OPERATIONS
	  ASSIGN,
	  ADD,
	  SUB,
	  MUL,
	  DIV,
	  AND,
	  OR,
	  NOT,
	  // LANGUAGE
	  IDENTIFIER,
	  FUNCTION,
	  DOT,
	  OPEN_PARENTHESIS,
	  CLOSE_PARENTHESIS,
	  AT,
	  NEWLINE,
	  // LITERALS
	  STRING,
	  LITERAL_FLOAT,
	  LITERAL_INT,
	  LITERAL_UINT,
	  // METADATA
	  END_OF_FILE
	};

}
