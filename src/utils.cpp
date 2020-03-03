//
// Created by dg on 31/10/19.
//

#include "utils.h"
#include "Dataset.h"
#include "Logger.h"
#include <regex>

#include "Node.h"

namespace utils {

    std::map<std::string, std::string> get_options_map(const std::string &args_c) {

        static const std::string endings = "\t\n\v\f\r ";
        // finds one or more spaces
        static const std::regex space_r("[ ]{1,}");
        // matches with a valid flag, e.g. "--name" or "--d"
        static const std::regex option_r("(--[a-zA-z]{1,})");
        // matches with a valid value for a flag, e.g. "pippo" or "3" or "p3r"
        static const std::regex value_r("^[a-zA-z0-9]+$");
        // Make a workable copy
        std::string args(args_c);
        // Right trim
        args.erase(0, args.find_first_not_of(endings));
        // Left trim
        args.erase(args.find_last_not_of(endings) + 1);
        // Add an ending space in order to get also the last token
        args.append(" ");
        // Find all the tokens
        std::vector<std::string> tokens;
        std::smatch match;
        while (regex_search(args, match, space_r)) {
            tokens.push_back(args.substr(0, match.position(0)));
            // suffix to find the rest of the string.
            args = match.suffix().str();
        }
        // Analyse the tokens and build the dictionary
        std::map<std::string, std::string> res;
        auto it = tokens.begin();
        while (it != tokens.end()) {
            if (std::regex_match(*it, option_r)) {
                const auto curr_option = it->substr(2);
                ++it;
                if (it != tokens.end()) {
                    // TODO: some checks about the next token (does it start with '-'?, ...)
                    res.insert(std::pair<std::string, std::string>(curr_option, *it));
                } else {
                    --it; // back of one position to match tokens.end()
                }
            }
            ++it;
        }

        return res;
    }

} // end of utils namespace