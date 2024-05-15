# Simple INI Parser

A library for the Nintendo Switch to be able to easily parse, modify, and create INI files. The main reason this library was written was due to the special needs of the hekate ini file. The hekate ini file can have the same key used multiple times within the same section, and its captions surrounded by curly braces. As these are outside the ini standard, other ini parsers would remove the duplicate keys and strip out the captions.

## Installation

I recommend adding this as a git submodule to your and then modifying your makefile to look in this directory for libs. For examples on how to do this please look at [Kosmos Updater](https://github.com/AtlasNX/Kosmos-Updater), and [ReiNX Spoofer](https://github.com/NicholeMattera/ReiNX-Spoofer).

## Usage

I've included multiple examples for editing, creating, and reading ini files. You can find them under the example folder. Remember to add `-lSimpleIniParser` to `LIBS` in your makefile.
