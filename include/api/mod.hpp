#pragma once

#include <string>
#include <vector>
#include <borealis.hpp>

#include "api/game.hpp"

class File {
    public:
        File(const std::string &name, const std::string &path, const std::string &url, const std::string &checkSum, const std::string &date);

        std::string getName() { return name; }
        std::string getPath() { return path; }
        std::string getUrl() { return url; }
        std::string getCheckSum() { return checkSum; }
        std::string getDate() { return date; }
    private:
        std::string name;
        std::string path;
        std::string url;
        std::string checkSum;
        std::string date;
};


class Mod {
    public:
        Mod(const std::string &name, int ID, const std::vector<std::string>& imageUrls, const std::string &author);

        void downloadImages();
        void loadImages();

        std::string getName() { return name; }
        int getID() { return ID; }
        std::string getDescription() { return description; }
        std::vector<File> getFiles() { return files; }
        std::vector<brls::Image*> getImages() { return images; }
        std::string getAuthor() { return author; }
    private:
        std::string name;
        int ID;
        std::string description;
        std::vector<File> files;

        std::vector<std::string> imageUrls;
        std::vector<brls::Image*> images;
        std::string author;
};  

class ModList {
    public:
        ModList(Game& game);
        ModList();
        std::vector<Mod> getMods() { return mods; }

        void nextPage();
        void previousPage();
        void updatePage();
    private:
        std::vector<Mod> mods;

        int currentPage = 1;

        Game& game;
};