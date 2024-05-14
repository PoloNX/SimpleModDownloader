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
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>

#include "Ini.hpp"
#include "IniHelper.hpp"
#include "IniOption.hpp"
#include "IniStringHelper.hpp"

namespace simpleIniParser {
    Ini::~Ini() {
        magic = "";

        for (IniSection * section : sections) {
            if (section != nullptr) {
                delete section;
                section = nullptr;
            }
        }
        sections.clear();
    }

    std::string Ini::build() {
        std::string result;

        if (magic != "") {
            result += magic + "\n";
        }

        for (auto const& option : options) {
            result += option->build();
        }

        for (auto const& section : sections) {
            result += section->build();
        }

        return result;
    }

    IniOption * Ini::findFirstOption(std::string term, bool caseSensitive, IniOptionType type, IniOptionSearchField field) {
        if (!caseSensitive) {
            IniStringHelper::toupper(term);
        }

        auto it = std::find_if(
            options.begin(),
            options.end(),
            std::bind(
                IniHelper::findOption,
                std::placeholders::_1,
                term,
                caseSensitive,
                type,
                field
            )
        );

        if (it == options.end())
            return nullptr;

        return (*it);
    }


    IniOption * Ini::findOrCreateFirstOption(std::string key, std::string val, bool caseSensitive, IniOptionType type, IniOptionSearchField field) {
        auto it = findFirstOption(key, caseSensitive, type, field);
        if (it == nullptr)
        {
            it = new IniOption(type, key, val);
            options.push_back(it);
        }

        return it;
    }

    std::vector<IniOption *> Ini::findAllOptions(std::string term, bool caseSensitive, IniOptionType type, IniOptionSearchField field) {
        std::vector<IniOption *> results;

        if (!caseSensitive) {
            IniStringHelper::toupper(term);
        }

        std::copy_if(
            options.begin(),
            options.end(),
            std::back_inserter(results),
            std::bind(
                IniHelper::findOption,
                std::placeholders::_1,
                term,
                caseSensitive,
                type,
                field
            )
        );

        return results;
    }

    IniSection * Ini::findSection(std::string term, bool caseSensitive, IniSectionType type) {
        if (!caseSensitive) {
            IniStringHelper::toupper(term);
        }

        auto it = std::find_if(
            sections.begin(),
            sections.end(),
            std::bind(
                IniHelper::findSection,
                std::placeholders::_1,
                term,
                caseSensitive,
                type
            )
        );

        if (it == sections.end())
            return nullptr;

        return (*it);
    }

    IniSection * Ini::findOrCreateSection(std::string term, bool caseSensitive, IniSectionType type) {
        auto it = findSection(term, caseSensitive, type);
        if (it == nullptr)
        {
            it = new IniSection(type, term);
            sections.push_back(it);
        }

        return it;
    }

    std::vector<IniSection *> Ini::findAllSections(std::string term, bool caseSensitive, IniSectionType type) {
        std::vector<IniSection *> results;

        if (!caseSensitive) {
            IniStringHelper::toupper(term);
        }

        std::copy_if(
            sections.begin(),
            sections.end(),
            std::back_inserter(results),
            std::bind(
                IniHelper::findSection,
                std::placeholders::_1,
                term,
                caseSensitive,
                type
            )
        );

        return results;
    }

    bool Ini::writeToFile(std::string path) {
        std::ofstream file(path);
        if (!file.is_open())
            return false;

        file << build();

        file.flush();
        file.close();

        return true;
    }

    Ini * Ini::parseFile(std::string path) {
        std::ifstream file(path);
        if (!file.is_open())
            return nullptr;

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        return _parseContent(&buffer, "");
    }
    
    Ini * Ini::parseFileWithMagic(std::string path, std::string magic) {
        std::ifstream file(path);
        if (!file.is_open())
            return nullptr;

        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        std::string line;
        getline(buffer, line);
        IniStringHelper::trim(line);
        if (line != magic) {
            return nullptr;
        }
        
        return _parseContent(&buffer, magic);
    }

    Ini * Ini::parseOrCreateFile(std::string path, std::string magic) {
        auto it = Ini::parseFileWithMagic(path, magic);
        if (it == nullptr)
            it = new Ini();
        return it;
    }

    Ini * Ini::_parseContent(std::stringstream * content, std::string magic) {
        Ini * ini = new Ini();
        ini->magic = magic;
        std::string line;
        while (getline(* content, line)) {
            IniStringHelper::trim(line);

            if (line.size() == 0)
                continue;

            bool shouldParseCommentsAsSection = ini->sections.size() != 0 && ini->sections.back()->type != IniSectionType::Section;
            IniSection * section = IniSection::parse(line, shouldParseCommentsAsSection);
            if (section != nullptr) {
                ini->sections.push_back(section);
            } else {
                IniOption * option = IniOption::parse(line);

                if (option != nullptr && ini->sections.size() == 0) {
                    ini->options.push_back(option);
                } else if (option != nullptr && ini->sections.size() != 0 && ini->sections.back()->type == IniSectionType::Section) {
                    ini->sections.back()->options.push_back(option);
                }
            }
        }

        return ini;
    }
}
