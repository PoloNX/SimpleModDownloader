#include "api/extract.hpp"
#include "utils/progress_event.hpp"

#include <archive.h>
#include <archive_entry.h>
#include <filesystem>
#include <fstream>
#include <switch.h>
#include <borealis.hpp>

const std::string smash_tid = "01006A800016E000";

namespace extract {
    int getFileCount(const std::string& archivePath) {
        struct archive* archive;
        struct archive_entry* entry;
        int fileCount = 0;

        archive = archive_read_new();
        archive_read_support_format_all(archive);
        archive_read_support_filter_all(archive);

        if(archive_read_open_filename(archive, archivePath.c_str(), 10240) == ARCHIVE_OK) {
            while(archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
                fileCount++;
            }
            archive_read_close(archive);
        }

        archive_read_free(archive);
        return fileCount;
    }

    s64 getTotalArchiveSize(const std::string& archivePath) {
        struct archive* archive;
        struct archive_entry* entry;
        s64 totalSize = 0;

        archive = archive_read_new();
        archive_read_support_format_all(archive);
        archive_read_support_filter_all(archive);

        if(archive_read_open_filename(archive, archivePath.c_str(), 10240) == ARCHIVE_OK) {
            while(archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
                totalSize += archive_entry_size(entry);
            }
            archive_read_close(archive);
        }

        archive_read_free(archive);
        return totalSize;
    }

bool extractEntry(const std::string& archiveFile, const std::string& outputDir, const std::string& tid) {
        chdir("sdmc:/");
        struct archive* archive = archive_read_new();

        brls::Logger::debug("Extracting {} to {}", archiveFile, outputDir);

        archive_read_support_format_all(archive);
        int result = archive_read_open_filename(archive, archiveFile.c_str(), 10240);
        if (result != ARCHIVE_OK) {
            brls::Logger::error("Failed to open archive: {}", archiveFile);
            archive_read_free(archive);
            //std::filesystem::remove(archiveFile);
            ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
            return false;
        }
        struct archive_entry* entry;
        ProgressEvent::instance().setTotalSteps(getFileCount(archiveFile));
        ProgressEvent::instance().setStep(0);

        s64 freeStorage;
        if(R_SUCCEEDED(nsGetFreeSpaceSize(NcmStorageId_SdCard, &freeStorage)) && getTotalArchiveSize(archiveFile) * 1.1 > freeStorage) {
            brls::Logger::error("sd is full");
            archive_read_free(archive);
            std::filesystem::remove(archiveFile);
            ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
            brls::Application::crash("full");
            std::this_thread::sleep_for(std::chrono::microseconds(2000000));
            brls::Application::quit();
            return false;
        }

        while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
            if (ProgressEvent::instance().getInterupt()) {
                archive_read_close(archive);
                archive_read_free(archive);
                std::filesystem::remove(archiveFile);
                ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
                return false;
            }
            const char* entryName = archive_entry_pathname(entry);
            
            if ((tid != smash_tid)) {
                if (std::string(entryName).find("romfs/") != std::string::npos || std::string(entryName).find("exefs/") != std::string::npos) {
                    
                    std::string outputFilePath;


                    if (std::string(entryName).find("romfs/") != std::string::npos)
                        outputFilePath = outputDir + "/" + std::string(entryName).substr(std::string(entryName).find("romfs/"));
                    else
                        outputFilePath = outputDir + "/" + std::string(entryName).substr(std::string(entryName).find("exefs/"));

                    if (std::string(entryName).find("|") != std::string::npos)
                        outputFilePath = outputFilePath.substr(0, outputFilePath.find("|"));
                  
                    brls::Logger::debug("Extracting file: {}", outputFilePath);
                    std::filesystem::path outputPath(outputFilePath);
                    std::filesystem::create_directories(outputPath.parent_path());

                    if (archive_entry_filetype(entry) == AE_IFDIR) {
                        ProgressEvent::instance().incrementStep(1);
                        // Skip directories
                        continue;
                    }


                    std::ofstream outputFile(outputFilePath, std::ios::binary);
                    if (!outputFile) {
                        brls::Logger::error("Failed to create output file: {}", outputFilePath);
                        archive_read_free(archive);
                        std::filesystem::remove(archiveFile);
                        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
                        return false;
                    }

                    const size_t bufferSize = 100000;
                    char buffer[bufferSize];
                    ssize_t bytesRead;
                    while ((bytesRead = archive_read_data(archive, buffer, bufferSize)) > 0) {
                        outputFile.write(buffer, bytesRead);
                    }

                    outputFile.close();

                    ProgressEvent::instance().incrementStep(1);
                } else {
                    brls::Logger::debug("Skipping {}", entryName);
                }
            } else {
                // Smash bros mods
                std::string outputFilePath = fmt::format("sdmc:/ultimate/mods/{}",std::string(entryName));
                std::filesystem::path outputPath(outputFilePath);
                std::filesystem::create_directories(outputPath.parent_path());
                if (archive_entry_filetype(entry) == AE_IFDIR) {
                    // Create the directory
                    if (!std::filesystem::create_directory(outputPath)) {
                        brls::Logger::error("Failed to create directory: {}", outputFilePath);
                    }
                    ProgressEvent::instance().incrementStep(1);
                    continue;
                }

                std::ofstream outputFile(outputFilePath, std::ios::binary);
                if (!outputFile) {
                    brls::Logger::error("Failed to create output file: {}", outputFilePath);
                    archive_read_free(archive);
                    std::filesystem::remove(archiveFile);
                    ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
                    return false;
                }

                const size_t bufferSize = 100000;
                char buffer[bufferSize];
                ssize_t bytesRead;
                while ((bytesRead = archive_read_data(archive, buffer, bufferSize)) > 0) {
                    outputFile.write(buffer, bytesRead);
                }

                outputFile.close();

                brls::Logger::debug("Extracted file: {}", outputFilePath);
                ProgressEvent::instance().incrementStep(1);
            }
        }

        archive_read_close(archive);
        archive_read_free(archive);
        std::filesystem::remove(archiveFile);
        ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
        return true;
    }
}