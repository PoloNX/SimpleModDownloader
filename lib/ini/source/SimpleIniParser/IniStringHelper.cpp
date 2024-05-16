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

#include <algorithm> 
#include <cctype>
#include <locale>

#include "IniStringHelper.hpp"

namespace simpleIniParser {
    void IniStringHelper::toupper(std::string &s) {
        for_each(s.begin(), s.end(), [](char & c){
            c = ::toupper(c);
        });
    }

    std::string IniStringHelper::toupper_copy(std::string s) {
        toupper(s);
        return s;
    }

    // Start of Source from https://stackoverflow.com/questions/216823
    void IniStringHelper::ltrim(std::string &s) {
        s.erase(s.begin(), find_if(s.begin(), s.end(), [](int ch) {
            return !isspace(ch);
        }));
    }

    void IniStringHelper::rtrim(std::string &s) {
        s.erase(find_if(s.rbegin(), s.rend(), [](int ch) {
            return !isspace(ch);
        }).base(), s.end());
    }

    void IniStringHelper::trim(std::string &s) {
        ltrim(s);
        rtrim(s);
    }

    std::string IniStringHelper::ltrim_copy(std::string s) {
        ltrim(s);
        return s;
    }

    std::string IniStringHelper::rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

    std::string IniStringHelper::trim_copy(std::string s) {
        trim(s);
        return s;
    }
    // End
}
