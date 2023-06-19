#include <iostream>
#include <fstream>
#include <filesystem>

#include "extract.hpp"
#include "progress_event.hpp"

#include <archive.h>
#include <archive_entry.h>
#include <filesystem>
#include <borealis.hpp>


namespace extract {
    int getFileCount(const std::string& archiveFile) {
        struct archive *archive;
        struct archive_entry *entry;
        int fileCount = 0;

        archive = archive_read_new();
        archive_read_support_format_all(archive);
        archive_read_support_filter_all(archive);

        if (archive_read_open_filename(archive, archiveFile.c_str(), 10240) == ARCHIVE_OK) {
            while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
                fileCount++;
            }
            archive_read_close(archive);
        }

        archive_read_free(archive);
        return fileCount;
    }
    bool extractEntry(const std::string& archiveFile, const std::string& outputDir) {
        chdir("sdmc:/");
        struct archive* archive = archive_read_new();

        /*std::string extension = archiveFile.substr(archiveFile.find_last_of(".") + 1);
        if (extension == "zip") {
            archive_read_support_format_zip(archive);
        }
        else if (extension == "rar") {
            archive_read_support_format_rar(archive);
        }
        else if (extension == "7z") {
            archive_read_support_format_7zip(archive);
        }*/
        archive_read_support_format_all(archive);
        int result = archive_read_open_filename(archive, archiveFile.c_str(), 10240);
        if (result != ARCHIVE_OK) {
            std::cout << result << std::endl;
            std::cout << "Failed to open archive: " << archiveFile << std::endl;
            archive_read_free(archive);
            std::filesystem::remove(archiveFile);
            ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
            return false;
        }
        struct archive_entry* entry;
        ProgressEvent::instance().setTotalSteps(getFileCount(archiveFile));
        ProgressEvent::instance().setStep(0);

        while (archive_read_next_header(archive, &entry) == ARCHIVE_OK) {
            if (ProgressEvent::instance().getInterupt()) {
                archive_read_close(archive);
                archive_read_free(archive);
                std::filesystem::remove(archiveFile);
                ProgressEvent::instance().setStep(ProgressEvent::instance().getMax());
                return false;
            }
            const char* entryName = archive_entry_pathname(entry);
            std::cout << entryName << std::endl;

            // Extract the contents of the 'romfs' directory only
            if (std::string(entryName).find("romfs/") != std::string::npos || std::string(entryName).find("exefs/") != std::string::npos) {
                
                std::string outputFilePath;
                if (std::string(entryName).find("romfs/") != std::string::npos)
                    outputFilePath = outputDir + "/" + std::string(entryName).substr(std::string(entryName).find("romfs/") + 6);
                else
                    outputFilePath = outputDir + "/" + std::string(entryName).substr(std::string(entryName).find("exefs/") + 6);

                std::cout << outputFilePath << std::endl;
                std::filesystem::path outputPath(outputFilePath);
                std::filesystem::create_directories(outputPath.parent_path());

                if (archive_entry_filetype(entry) == AE_IFDIR) {
                    ProgressEvent::instance().incrementStep(1);
                    // Skip directories
                    continue;
                }


                std::ofstream outputFile(outputFilePath, std::ios::binary);
                if (!outputFile) {
                    std::cout << "Failed to create output file: " << outputFilePath << std::endl;
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

                std::cout << "Extracted file: " << outputFilePath << std::endl;
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