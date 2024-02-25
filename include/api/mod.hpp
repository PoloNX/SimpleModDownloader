#pragma once

#include <string>
#include <vector>
#include <borealis.hpp>

#include "api/game.hpp"

class File {
    public:
        File(const std::string &name, const std::string &size, const std::string &url, const std::string &checkSum, const int& date);

        std::string getName() { return name; }
        std::string getUrl() { return url; }
        std::string getCheckSum() { return checkSum; }
        int getDate() { return date; }
        std::string getSize() { return size; }

    private:
        std::string name;
        std::string size;
        std::string url;
        std::string checkSum;
        int date;
};


class Mod {
    public:
        Mod(const std::string &name, int ID, const std::vector<std::string>& imageUrls, const std::string &author);
        Mod(){}

        void downloadImages();
        void loadImages();

        std::string getName() { return name; }
        int getID() { return ID; }
        std::string getDescription() { return description; }
        std::vector<File> getFiles() { return files; }
        std::vector<brls::Image*> getImages() { return images; }
        std::string getAuthor() { return author; }

        void loadMod();
    private:
        std::string name;
        int ID = 0;
        std::string description;
        std::vector<File> files;

        std::vector<std::string> imageUrls;
        std::vector<brls::Image*> images;
        std::string author;
};  

class ModList {
    public:
        ModList(Game game);
        //ModList();
        std::vector<Mod> getMods() { return mods; }

        void nextPage();
        void previousPage();
        void updatePage();
    private:
        std::vector<Mod> mods;

        int currentPage = 1;

        Game game;
};