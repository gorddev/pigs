#include <core/errors/Error.hpp>

using namespace pg;

std::string_view Err::what() {
  suppressed = true;
  return err_what;
}

std::string_view Err::display_str() {
  suppressed = true;
  return err;
}

Err &&Err::squash() {
  suppressed = true;
  return std::move(*this);
}

bool Err::is_evil() const { return error_inside && !suppressed; }

Err& Err::burst(bool panic_on_empty_err) {
  if (error_inside)
    print_and_panic();
  if (panic_on_empty_err)
    PG_Err("Attempted to burst an empty error.").burst();
  return *this;
}

Err::~Err() {
  if (is_evil()) {
    std::puts("──────────\x1B[36m\x1B[1m{Warning}\x1B[0m───────────\n"
              "\x1B[31m\x1B[1merror:\x1B[0m Unhandled "
              "\x1B[34m[pg::Err]\x1B[0m. Please call either:\n"
              "\terr.\x1B[3m\x1B[36mwhat\x1B[0m() \x1B[90m(for string "
              "output)\x1B[0m, or\n"
              "\terr.\x1B[3m\x1B[36msquash\x1B[0m() \x1B[90m(for error "
              "suppression)\x1B[0m\n"
              "To prevent the error from terminating the program.");
    print_and_panic();
  }
}

Err::Err(const Err &o)
    : err(o.err), chain_count(o.chain_count),
      suppressed(o.suppressed), // Inherit the state
      error_inside(o.error_inside) {
  o.suppressed = true;
}

Err &Err::operator=(const Err &o) {
  if (this != &o) {
    if (this->error_inside && !this->suppressed) {
      this->~Err();
    }
    err = o.err;
    chain_count = o.chain_count;
    suppressed = o.suppressed;
    error_inside = o.error_inside;
    o.suppressed = true;
  }
  return *this;
}

Err &Err::operator=(Err &&o) noexcept {
  this->err = std::move(o.err);
  this->chain_count = o.chain_count;
  this->suppressed = o.suppressed;
  this->error_inside = o.error_inside;
  o.error_inside = false;
  return *this;
}

Err::Err(Err &&o) noexcept
    : err(std::move(o.err)), chain_count(o.chain_count),
      suppressed(o.suppressed), error_inside(o.error_inside) {
  o.error_inside = false;
}
