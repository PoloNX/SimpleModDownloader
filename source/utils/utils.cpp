#include "utils/utils.hpp"

#include <switch.h>
#include <nlohmann/json.hpp>
#include <sstream>
#include <iomanip>

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
}