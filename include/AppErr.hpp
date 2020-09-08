#pragma once

struct AppErr : public std::exception {
    const char *info;
    AppErr(const char *message = "AppErr"): info(message) {};
    const char *what() const _NOEXCEPT { return info; }
};

