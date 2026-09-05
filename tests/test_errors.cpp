#include "errors/Error.hpp"


struct MyErr1 {
  int h;
  const char* reason;

  std::string str() const {return "";}
};

struct MyErr2 {
  int j;
  const char* reason;

  std::string str() const {return std::string("Returned error with reason: ") + reason;}
};


pg::Error<MyErr1, MyErr2> errors() {
  return PG_ErrNew(MyErr2, .j = 4, .reason = "helo");
}

pg::Error<MyErr1, MyErr2> errors2() {
	auto err = errors();
	err.add_trace(__PRETTY_FUNCTION__, __FILE__, __LINE__);
  return err;
}

pg::expected<int, MyErr1, MyErr2> uexp() {
  return PG_UErrNew(MyErr2, .j = 4, .reason = "hfggg");
}

pg::expected<int, MyErr1, MyErr2> new_ret() {
	auto err = uexp();
  PG_ReturnIfUErr(err)
  return 3;
}

#include <iostream>

int main() {

  auto exp = new_ret();


}
