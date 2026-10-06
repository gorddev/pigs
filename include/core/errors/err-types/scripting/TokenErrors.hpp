#pragma once

#include <string>
#include <toolkit/intdef.h>

namespace pg::script::err {

struct UnterminatedString {
  std::string script_str;
  u32 line_num;
  std::string str() const {
  	return std::string("Unterminated string: {") + script_str + "} on line " + std::to_string(line_num);
  }
};

struct InvalidLiteral {
  std::string script_str;
  u32 line_num;
  enum ErrType : char {
  	EXTRA_DECIMAL,  ///< If there is an extra decimal in an identifier
   	FLOAT_SPEC_BEFORE_DEMICAL,	///< If the floating point specification occurs before the first decimal
    MULTIPLE_TYPE_SPECIFIERS, 	///< If multiple type specifiers occur within the same literal
    CHAR_AFTER_TYPE_SPECIFIER, ///< If there is a digit after a type specifier in a numeric literal
    UNRECOGNIZED_LITERAL_CHAR, ///< If there is an unrecognized literal character in tokenizer
  } err_type;
  std::string str() const {
    return std::string(err_type_str(err_type)) +
           "\n  [\"" + script_str + "\" @ line: " + std::to_string(line_num) + "]";
  }

private:
  std::string err_type_str(ErrType t) const {
  	std::string s; //< used for easy addition
    switch (t) {
    case EXTRA_DECIMAL:
      return s + "Could not parse number because of an extra decimal place '.'";
    case FLOAT_SPEC_BEFORE_DEMICAL:
    	return s + "Floating-point specification occured before the first decimal. (e.g. 1.f is valid, but 1f is not)";
    case MULTIPLE_TYPE_SPECIFIERS:
    	return s + "Identifier has extra type specifier '" + script_str.back() +"'. Only one type specifier per numeric literal allowed.";
    case CHAR_AFTER_TYPE_SPECIFIER:
    	return s + "Character: '" + script_str.back() + "' found after the type_specifier (e.g. 'u', 'i', 'f'). This is not allowed.";
    case UNRECOGNIZED_LITERAL_CHAR:
    	return s + "Unrecognized literal character: '" + script_str.back() + "'";
    }
    return "Unknown Invalid Literal Error";
  }
};

} // namespace pg::script::err
