#include "utils.hpp"
#include "download.hpp"
#include "extract.hpp"
#include "SimpleIniParser.hpp"

#include <sstream>
#include <iomanip>
#include <filesystem>
#include <regex>
#include <fstream>
#include <ctime>
#include <iostream>
#include <switch.h>

namespace i18n = brls::i18n;
using namespace i18n::literals;

using namespace simpleIniParser;

namespace utils {

    std::string formatApplicationId(u64 ApplicationId)
    {
        std::stringstream strm;
        strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << ApplicationId;
        return strm.str();
    }



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

    nlohmann::json getInstalledGames() {
        NsApplicationRecord* records = new NsApplicationRecord[64000]();
        uint64_t tid;
        NsApplicationControlData controlData;
        NacpLanguageEntry* langEntry = nullptr;
        const char* desiredLanguageCode = "en";
        
        Result rc;
        int recordCount = 0;
        size_t controlSize = 0;

        nlohmann::json games = nlohmann::json::array();

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
            games.push_back({
                {"name", appName},
                {"tid", titleId}
            });
        }
        delete[] records;
        return games;
    }

    nlohmann::json searchGames(std::string gameTitle) {
        for (auto i : goodGamesName) {
            if (i.first == gameTitle) {
                gameTitle = i.second;
                break;
            }
        }
        brls::Logger::info("Game title: {}", gameTitle);

        gameTitle = replaceSpacesWithPlus(gameTitle);

        const std::string api_url = fmt::format("https://gamebanana.com/apiv11/Util/Game/NameMatch?_sName={}", gameTitle);
        brls::Logger::debug("API URL: {}", api_url);

        nlohmann::json games = net::downloadRequest(api_url);

        return games;
    }

    std::string replaceSpacesWithPlus(const std::string& str) {
        std::string resultString = str;
        std::ranges::replace(resultString, ' ', '+');
        return resultString;
    }

    nlohmann::json getMods(const int& gameID, int& page) {
        std::string api_url = fmt::format("https://gamebanana.com/apiv11/Game/{}/Subfeed?_nPage={}?_nPerpage=50&_csvModelInclusions=Mod", std::to_string(gameID), std::to_string(page));
        nlohmann::json mods = net::downloadRequest(api_url);

        if(mods.at("_aRecords").empty()) {
            api_url = fmt::format("https://gamebanana.com/apiv11/Game/{}/Subfeed?_nPage={}&_nPerpage=50&_csvModelInclusions=Mod", std::to_string(gameID),std::to_string(page-1));
            mods = net::downloadRequest(api_url);
            page -= 1;
        }

        return mods;
    }

    nlohmann::json getMods(const int& gameID, const std::string&search, int& page) {
        if (search.size() < 3) {
            brls::Application::notify("menu/notify/string_to_short"_i18n);
            return getMods(gameID, page);
        }
        std::string search_term = replaceSpacesWithPlus(search);
        std::string api_url = fmt::format("https://gamebanana.com/apiv11/Game/{}/Subfeed?_nPage={}&_nPerpage=50&_sName={}&_csvModelInclusions=Mod", std::to_string(gameID),std::to_string(page), search_term);
        nlohmann::json mods = net::downloadRequest(api_url);
        if(mods.at("_aRecords").empty() && page > 1) {
            api_url = fmt::format("https://gamebanana.com/apiv11/Game/{}/Subfeed?_nPage={}&_nPerpage=50&_sName={}&_csvModelInclusions=Mod", std::to_string(gameID),std::to_string(page-1), search_term);
            mods = net::downloadRequest(api_url);
            page -= 1;
        }
        std::cout<< api_url << std::endl;
        std::cout << mods << std::endl;
        return mods;
    }

    nlohmann::json getDownloadLinks(const std::string& ModelName, const int& idRow) {
        const std::string api_url = fmt::format("https://gamebanana.com/apiv11/{}/{}?_csvProperties=@gbprofile", ModelName, idRow);
        nlohmann::json downloadLinks = net::downloadRequest(api_url);

        return downloadLinks.at("_aFiles");
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

    std::vector<brls::Image*> getModsImages(const nlohmann::json& mod_json, const int& BigPage, size_t& sizeOfArray) {
        nlohmann::json preview_json = mod_json.at("_aPreviewMedia").at("_aImages");

        std::vector<brls::Image*> images;
        brls::Image* current_image;

        int compteur = 0;

        std::string url;

        for (auto i : preview_json) {
            std::vector<unsigned char> buffer;
            if (compteur == BigPage)
                url = fmt::format("https://images.gamebanana.com/img/ss/mods/{}", i.at("_sFile"));
            else 
                url = fmt::format("https://images.gamebanana.com/img/ss/mods/{}", i.at("_sFile100"));
            net::downloadImage(url, buffer);
            current_image = new brls::Image(buffer.data(), buffer.size());  

            if (compteur == BigPage)
                sizeOfArray = buffer.size();

            images.push_back(current_image);
            buffer.clear();  
            compteur++;         
            if (compteur > 5)
                break;
        
            svcOutputDebugString("0", 1);
        }
        return images;
    }

    void showDialogBoxInfo(const std::string& text)
    {
        brls::Dialog* dialog;
        dialog = new brls::Dialog(text);
        brls::GenericEvent::Callback callback = [dialog](brls::View* view) {
            dialog->close();
        };
        dialog->addButton("menus/common/ok"_i18n, callback);
        dialog->setCancelable(true);
        dialog->open();
    }

    int openWebBrowser(const std::string url)
    {
        Result rc = 0;
        int at = appletGetAppletType();
        if (at == AppletType_Application) {  // Running as a title
            WebCommonConfig conf;
            WebCommonReply out;
            rc = webPageCreate(&conf, url.c_str());
            if (R_FAILED(rc))
                return rc;
            webConfigSetJsExtension(&conf, true);
            webConfigSetPageCache(&conf, true);
            webConfigSetBootLoadingIcon(&conf, true);
            webConfigSetWhitelist(&conf, ".*");
            rc = webConfigShow(&conf, &out);
            if (R_FAILED(rc))
                return rc;
        }
        else {  // Running under applet
            showDialogBoxInfo("menus/utils/applet_webbrowser"_i18n);
        }
        return rc;
    }
    std::string removeHtmlTags(const std::string& input) {
        std::regex tagsRegex("<.*?>");
        return std::regex_replace(input, tagsRegex, "");
    }
    std::string getDescription(const int& mod_id) {
        const std::string api_url = fmt::format("https://gamebanana.com/apiv11/Mod/{}?_csvProperties=_sText", mod_id);
        nlohmann::json desc = net::downloadRequest(api_url);

        std::string rawText = desc.at("_sText");
        std::string cleanedText = removeHtmlTags(rawText);
        
        return cleanedText;
    }

    std::string convertTimestampToString(int timestamp) {
        std::time_t time = static_cast<std::time_t>(timestamp);
        std::tm* timeInfo = std::localtime(&time);

        std::ostringstream oss;
        oss << std::put_time(timeInfo, "%m/%d/%Y %H:%M:%S");

        return oss.str();
    }

    std::string formatFileSize(int size) {
        double fileSize = static_cast<double>(size);
        std::ostringstream oss;

        if (fileSize < 1024) {
            oss << fileSize << " octets";
        } else if (fileSize < 1048576) {
            oss << std::fixed << std::setprecision(2) << fileSize / 1024 << " Ko";
        } else if (fileSize < 1073741824) {
            oss << std::fixed << std::setprecision(2) << fileSize / 1048576 << " Mo";
        } else if (fileSize < 1099511627776) {
            oss << std::fixed << std::setprecision(2) << fileSize / 1073741824 << " Go";
        } else {
            oss << std::fixed << std::setprecision(2) << fileSize / 1099511627776 << " To";
        }

        return oss.str();
    }

    void writeOption(IniOption * option, bool withTab) {
    switch (option->type) {
        case IniOptionType::SemicolonComment:
            std::cout << ((withTab) ? "\t" : "") << "Type: Semicolon Comment, Value: \"" << option->value << "\"\n";
            break;

        case IniOptionType::HashtagComment:
            std::cout << ((withTab) ? "\t" : "") << "Type: Hashtag Comment, Value: \"" << option->value << "\"\n";
            break;

        default:
            std::cout << ((withTab) ? "\t" : "") << "Type: Option, Key: \"" << option->key << "\", Value: \"" << option->value << "\"\n";
            break;
        }
    }

    void writeSection(IniSection * section) {
        switch (section->type) {
            case IniSectionType::SemicolonComment:
                std::cout << "Type: Semicolon Comment, Value: \"" << section->value << "\"\n";
                break;

            case IniSectionType::HashtagComment:
                std::cout << "Type: Hashtag Comment, Value: \"" << section->value << "\"\n";
                break;

            case IniSectionType::HekateCaption:
                std::cout << "Type: Hekate Caption, Value: \"" << section->value << "\"\n";
                break;

            default:
                std::cout << "Type: Section, Value: \"" << section->value << "\"\n";
                break;
        }

        for (auto const& option : section->options) {
            writeOption(option, true);
        }

        std::cout << "\n";
    }

    std::string getModInstallPath() {
        std::filesystem::path path(std::string("sdmc:/config/SimpleModManager/parameters.ini"));
        if(std::filesystem::exists(path)) {
            Ini * config = Ini::parseFile(path.string());

            for(auto const& option : config->options) {
                writeOption(option, false);
            }

            if (config->options.size() > 0) {
                std::cout << "\n";
            }

            for (auto const& section : config->sections) {
                writeSection(section);
            }

            IniOption * path = config->findFirstOption("stored-mods-base-folder");

            std::cout << path->key << " = " << path->value << "\n";

            std::string pathString = path->value;

            if (!pathString.ends_with("/")) {
                pathString += "/";
            }
            return pathString;
        }
        return "/mods/";
    }

    std::vector<int> split_version(const std::string& version) {
        std::vector<int> components;
        std::string::size_type pos = 0;
        while (pos < version.size()) {
            std::string::size_type next_pos = version.find('.', pos);
            if (next_pos == std::string::npos) {
                next_pos = version.size();
            }
            std::string component_str = version.substr(pos, next_pos - pos);
            int component = std::stoi(component_str);
            components.push_back(component);
            pos = next_pos + 1;
        }
        return components;
    }

    bool is_older_version(const std::string& version1, const std::string version2) {
        std::vector<int> components1 = split_version(version1);
        std::vector<int> components2 = split_version(version2);

        // Compare the components of the versions
        for (size_t i = 0; i < components1.size(); i++) {
            if (components1[i] < components2[i]) {
                return true;
            } else if (components1[i] > components2[i]) {
                return false;
            }
        }

        // If all components are equal, the versions are the same
        return false;
    }
    
    bool cp(char *filein, char *fileout) {
        FILE *exein, *exeout;
        exein = fopen(filein, "rb");
        if (exein == NULL) {
            /* handle error */
            perror("file open for reading");
            return false;
        }
        exeout = fopen(fileout, "wb");
        if (exeout == NULL) {
            /* handle error */
            perror("file open for writing");
            return false;
        }
        size_t n, m;
        unsigned char buff[8192];
        do {
            n = fread(buff, 1, sizeof buff, exein);
            if (n) m = fwrite(buff, 1, n, exeout);
            else   m = 0;
        }
        while ((n > 0) && (n == m));
        if (m) {
            perror("copy");
            return false;
        }
        if (fclose(exeout)) {
            perror("close output file");
            return false;
        }
        if (fclose(exein)) {
            perror("close input file");
            return false;
        }
        return true;
    }

    std::string getCurrentLang() {
        nlohmann::json settings;
        std::ifstream file("sdmc:/config/SimpleModDownloader/settings.json");
        file >> settings;
        file.close();
        
        if(settings.at("language") == "auto"){
            const std::string currentLocale = i18n::getCurrentLocale();
            if (currentLocale != "fr" && currentLocale != "en-US" && currentLocale != "es" && currentLocale != "de" && currentLocale != "it" && currentLocale != "zh-CN" && currentLocale != "ja" && currentLocale != "ro" && currentLocale != "pt-BR" && currentLocale != "gr" && currentLocale != "hu" && currentLocale != "ko")
                return "en-US";
            else 
                return currentLocale;
        }
        else {
             return settings.at("language").get<std::string>();
        }
    }

    bool isLangAuto() {
        nlohmann::json settings;
        std::ifstream file("sdmc:/config/SimpleModDownloader/settings.json");
        file >> settings;
        file.close();

        if (settings.at("language") == "auto") {
            return true;
        }
        return false;
    }
}
