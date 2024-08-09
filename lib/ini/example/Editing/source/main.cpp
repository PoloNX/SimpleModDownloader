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

void copy_file(std::string srcPath, std::string destPath) {
    std::ifstream src(srcPath, std::ios::binary);
    std::ofstream dest(destPath, std::ios::binary);

    dest << src.rdbuf();

    src.close();
    dest.flush();
    dest.close();
}

int main(int argc, char **argv) {
    consoleInit(NULL);

    Result rc = romfsInit();
    if (R_FAILED(rc)) {
        std::cout << "Unable to initialize romfs.\n";
    }
    else {
        copy_file("romfs:/config.ini", "sdmc:/example1.ini");

        Ini * exampleIni = Ini::parseFile("sdmc:/example1.ini");
        exampleIni->sections.pop_back();
        exampleIni->sections.pop_back();

        exampleIni->findSection("config")->findFirstOption("autoboot")->value = "0";

        exampleIni->writeToFile("sdmc:/example2.ini");

        delete exampleIni;
    }
    std::cout << "Original file written to: sdmc:/example1.ini\n";
    std::cout << "Modified file written to: sdmc:/example2.ini\n\n";
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
