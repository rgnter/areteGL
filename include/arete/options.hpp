//
// Created by maros on 21.4.2024.
//

#ifndef ARETEGL_OPTIONS_HPP
#define ARETEGL_OPTIONS_HPP

#include <string>
#include <sstream>
#include <functional>

namespace arete::opt
{

//! A parser of option value.
//!
//! @param val String stream of the option value to parse.
using OptionParser = std::function<void(std::stringstream& val)>;

//! Map of option parsers indexed by their option name.
using OptionParserMap = std::unordered_map<std::string, OptionParser>;

//! Iterates over arguments, and invokes appropriate
//! option parsers based on the argument's name.
//!
//! @param argc Argument count.
//! @param argv Arguments values.
//! @param parsers Option parsers.
void ParseOptions(int32_t argc, const char** argv, const OptionParserMap& parsers);

} // namespace opt

#endif // ARETEGL_OPTIONS_HPP
