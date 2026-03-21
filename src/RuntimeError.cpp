#include "RuntimeError.hpp"

RuntimeError::RuntimeError(Token token, std::string message)
    : std::runtime_error(message), token(std::move(token))
{}
