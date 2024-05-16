/*
 * Simple INI Parser
 * Copyright (c) 2021 Nichole Mattera
 * 
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above 
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "IniOption.hpp"
#include "IniStringHelper.hpp"

namespace simpleIniParser {
    IniOption::IniOption(IniOptionType t, std::string k, std::string v) {
        type = t;
        key = k;
        value = v;
    }

    std::string IniOption::build() {
        switch (type) {
            case IniOptionType::SemicolonComment:
                return "; " + value + "\n";

            case IniOptionType::HashtagComment:
                return "# " + value + "\n";

            default:
                return key + "=" + value + "\n";
        }
    }

    IniOption * IniOption::parse(std::string line) {
        if (line.at(0) == ';') {
            return new IniOption(IniOptionType::SemicolonComment, "", IniStringHelper::trim_copy(line.substr(1, line.size() - 1)));
        } else if (line.at(0) == '#') {
            return new IniOption(IniOptionType::HashtagComment, "", IniStringHelper::trim_copy(line.substr(1, line.size() - 1)));
        } else {
            size_t pos = line.find('=');
            if (pos != std::string::npos && pos > 0) {
                return new IniOption(IniOptionType::Option, IniStringHelper::rtrim_copy(line.substr(0, pos)), IniStringHelper::ltrim_copy(line.substr(pos + 1)));
            } else {
                return nullptr;
            }
        }
    }
}
