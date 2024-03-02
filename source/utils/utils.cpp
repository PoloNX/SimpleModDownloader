#include "utils/utils.hpp"

#include <switch.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>
#include <regex>
#include <borealis.hpp>
#include <SimpleIniParser.hpp>

using namespace simpleIniParser;

namespace utils {

    Result nacpGetLanguageEntrySpecialLanguage(NacpStruct* nacp, NacpLanguageEntry** langentry, const SetLanguage LanguageChoosen) {
        Result rc=0;
        SetLanguage Language = LanguageChoosen;
        NacpLanguageEntry *entry = NULL;
        u32 i=0;

        if (nacp==NULL || langentry==NULL)
            return MAKERESULT(Module_Libnx, LibnxError_BadInput);

        *langentry = NULL;

        entry = &nacp->lang[SetLanguage_ENUS];

        if (entry->name[0]==0 && entry->author[0]==0) {
            for(i=0; i<16; i++) {
                entry = &nacp->lang[i];
                if (entry->name[0] || entry->author[0]) break;
            }
        }

        if (entry->name[0]==0 && entry->author[0]==0)
            return rc;

        *langentry = entry;

        return rc;
    }


    std::string formatApplicationId(u64 ApplicationId)
    {
        std::stringstream strm;
        strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << ApplicationId;
        return strm.str();
    }

    std::vector<std::pair<std::string, std::string>> getInstalledGames() {
        std::vector<std::pair<std::string, std::string>> games;

        NsApplicationRecord* records = new NsApplicationRecord[64000]();
        uint64_t tid;
        NsApplicationControlData controlData;
        NacpLanguageEntry* langEntry = nullptr;
        const char* desiredLanguageCode = "en";
        
        Result rc;
        int recordCount = 0;
        size_t controlSize = 0;

        nlohmann::json json = nlohmann::json::array();

        rc = nsListApplicationRecord(records, 64000, 0, &recordCount);
        for (auto i = 0; i < recordCount; ++i) {
            tid = records[i].application_id;
            rc = nsGetApplicationControlData(NsApplicationControlSource_Storage, tid, &controlData, sizeof(controlData), &controlSize);
            if (R_FAILED(rc)) {
                continue; // Ou break je sais pas trop
            }

            rc = nacpGetLanguageEntrySpecialLanguage(&controlData.nacp, &langEntry, SetLanguage_ENUS);
            
            //rc = nsGetApplicationDesiredLanguage(&controlData.nacp, &langEntry);    
            if (R_FAILED(rc)) {
                continue; // Ou break je sais pas trop
            }

            if (!langEntry->name) {
                continue;
            }

            std::string appName = langEntry->name;
            std::string titleId = formatApplicationId(tid);
            json.push_back({
                {"name", appName},
                {"tid", titleId}
            });
        }

        delete[] records;

        for(auto i : json) {
            games.push_back(std::pair<std::string, std::string>(i.at("name").get<std::string>(), i.at("tid").get<std::string>()));
        }

        return games;
    }

    uint8_t* getIconFromTitleId(const std::string& titleId) {
        if(titleId.empty()) return nullptr;

        uint8_t* icon = nullptr;
        NsApplicationControlData controlData;
        size_t controlSize  = 0;
        uint64_t tid;

        std::istringstream buffer(titleId);
        buffer >> std::hex >> tid;

        if (R_FAILED(nsGetApplicationControlData(NsApplicationControlSource_Storage, tid, &controlData, sizeof(controlData), &controlSize))){ return nullptr; }

        icon = new uint8_t[0x20000];
        memcpy(icon, controlData.icon, 0x20000);
        return icon;
    }

    std::string removeHtmlTags(const std::string& input) {
        //Replace <br> by \n
        std::regex brRegex("<br>");
        std::string output = std::regex_replace(input, brRegex, "\n");
        std::regex nbspRegex("&nbsp;");
        output = std::regex_replace(output, nbspRegex, " ");
        std::regex tagsRegex("<.*?>");
        return std::regex_replace(output, tagsRegex, "");
    }

    bool ends_with(const std::string& str, const std::string& suffix) {
        if (suffix.length() > str.length()) {
            return false;
        }
        return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
    }

    bool starts_with(const std::string& str, const std::string& prefix) {
        if (prefix.length() > str.length()) {
            return false;
        }
        return str.compare(0, prefix.length(), prefix) == 0;
    }

    std::string getModInstallPath() {
        std::filesystem::path path(std::string("sdmc:/config/SimpleModManager/parameters.ini"));
        if(!std::filesystem::exists(path)) {
            return "mods";
        }
        Ini* config = Ini::parseFile(path.string());
        IniOption* path_mods = config->findFirstOption("stored-mods-base-folder");
        std::string pathString = path_mods->value;
        brls::Logger::debug("Mod install path: {}", pathString);
        if(ends_with(pathString, "/"))
            pathString.pop_back();
        if(starts_with(pathString, "/"))
            pathString.erase(0, 1);
        return pathString;
    }
}