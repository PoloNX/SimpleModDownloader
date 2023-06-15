#include "utils.hpp"
#include <switch.h>
#include <iomanip>

namespace utils {
    std::string formatApplicationId(u64 ApplicationId){
        std::stringstream strm;
        strm << std::uppercase << std::setfill('0') << std::setw(16) << std::hex << ApplicationId;
        return strm.str();
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
    
    nlohmann::json getInstalledGames() {
        NsApplicationRecord* records = new NsApplicationRecord[64000]();
        uint64_t tid;
        NsApplicationControlData controlData;
        NacpLanguageEntry* langEntry = nullptr;
        
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
            rc = nacpGetLanguageEntry(&controlData.nacp, &langEntry);
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
}