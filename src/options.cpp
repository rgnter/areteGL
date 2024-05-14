//
// Created by maros on 21.4.2024.
//

#include "arete/options.hpp"

void arete::opt::ParseOptions(int32_t argc, const char** argv, const OptionParserMap& parsers) {
  // Copy over arguments.
  std::vector<std::string> arguments(argc);
  for(int idx = 0; idx < argc; ++idx)
  {
    arguments.emplace_back(argv[idx]);
  }

  // Iterate over arguments and call appropriate option parsers.
  for(const auto& argument : arguments)
  {
    const auto delimiter = argument.find("=");
    // No delimiter found, skip argument.
    if (delimiter == std::string::npos)
      continue;

    // Name of the argument.
    const auto name = argument.substr(0, delimiter);
    // Value of the argument.
    const auto value = argument.substr(delimiter + 1);

    // Find option parser for the argument.
    const auto argument_parser = parsers.find(name);
    if (argument_parser == parsers.end())
    {
      continue;
    }

    // Send the argument value to parsers.
    std::stringstream value_stream(value);
    argument_parser->second(value_stream);
  }
}
