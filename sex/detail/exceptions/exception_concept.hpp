#pragma once

#include <exception>

template<class T>
concept CException = std::is_base_of_v<std::exception, std::remove_cvref_t<T>>;