#pragma once

#include <iostream>
#include <string>

#include "fmt/format.h"
#include "storage/model/properties/properties.hpp"
#include "storage/model/properties/traversers/consolewriter.hpp"
#include "storage/model/properties/traversers/jsonwriter.hpp"

using std::cout;
using std::endl;

void print_props(const Properties &properties);

#ifdef NDEBUG
#define PRINT_PROPS(_)
#else
#define PRINT_PROPS(_PROPS_) print_props(_PROPS_);
#endif

void cout_properties(const Properties &properties);

void cout_property(const std::string &key, const Property &property);

// this is a nice way how to avoid multiple definition problem with
// headers because it will create a unique namespace for each compilation unit
// http://stackoverflow.com/questions/2727582/multiple-definition-in-header-file
namespace
{

template <typename... Args>
std::string format(const std::string &format_str, const Args &... args)
{
    return fmt::format(format_str, args...);
}

template <typename... Args>
std::string code_line(const std::string &format_str, const Args &... args)
{
    return "\t" + format(format_str, args...) + "\n";
}
}
