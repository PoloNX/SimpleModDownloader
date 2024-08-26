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

#include <iostream>
#include <fstream>
#include <switch.h>
#include <SimpleIniParser.hpp>

using namespace simpleIniParser;

int main(int argc, char **argv) {
    consoleInit(NULL);

    Ini * hekateIni = new Ini();

    IniSection * configSection = new IniSection(IniSectionType::Section, "config");
    configSection->options.push_back(new IniOption(IniOptionType::Option, "autoboot", "1"));
    configSection->options.push_back(new IniOption(IniOptionType::Option, "autoboot_list", "0"));
    configSection->options.push_back(new IniOption(IniOptionType::Option, "bootwait", "5"));
    configSection->options.push_back(new IniOption(IniOptionType::Option, "customlogo", "1"));
    configSection->options.push_back(new IniOption(IniOptionType::Option, "verification", "1"));
    configSection->options.push_back(new IniOption(IniOptionType::Option, "backlight", "100"));
    configSection->options.push_back(new IniOption(IniOptionType::Option, "autohosoff", "0"));
    configSection->options.push_back(new IniOption(IniOptionType::Option, "autonogc", "1"));
    hekateIni->sections.push_back(configSection);

    hekateIni->sections.push_back(new IniSection(IniSectionType::HekateCaption, "CFW"));

    IniSection * cfwSection = new IniSection(IniSectionType::Section, "CFW");
    cfwSection->options.push_back(new IniOption(IniOptionType::Option, "fss0", "atmosphere/fusee-secondary.bin"));
    cfwSection->options.push_back(new IniOption(IniOptionType::Option, "kip1patch", "nosigchk"));
    cfwSection->options.push_back(new IniOption(IniOptionType::Option, "atmosphere", "1"));
    cfwSection->options.push_back(new IniOption(IniOptionType::Option, "logopath", "bootloader/bootlogo.bmp"));
    hekateIni->sections.push_back(cfwSection);

    hekateIni->sections.push_back(new IniSection(IniSectionType::HekateCaption, "Stock"));

    IniSection * stockSection = new IniSection(IniSectionType::Section, "Stock");
    stockSection->options.push_back(new IniOption(IniOptionType::Option, "fss0", "atmosphere/fusee-secondary.bin"));
    stockSection->options.push_back(new IniOption(IniOptionType::Option, "stock", "1"));
    hekateIni->sections.push_back(stockSection);

    if (hekateIni->writeToFile("sdmc:/example.ini")) {
        std::cout << "Ini file writen to: sdmc:/example.ini\n";
    }

    delete hekateIni;

    std::cout << "Press any key to close.\n";

    while(appletMainLoop())
    {
        hidScanInput();
        if (hidKeysDown(CONTROLLER_P1_AUTO))
            break;

        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    return 0;
}
