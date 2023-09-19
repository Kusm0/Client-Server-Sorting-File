

#ifndef SERVER_GL_FILESORT_LINUX_H
#define SERVER_GL_FILESORT_LINUX_H

#ifdef __unix__

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <cstring>

struct FileInfo {
    std::string filePath;
    std::string fileType;
};

bool CompareByCreationDate(const std::string &filePath1, const std::string &filePath2) {
    struct stat st1, st2;
    if (stat(filePath1.c_str(), &st1) != 0 || stat(filePath2.c_str(), &st2) != 0) {
        return false;
    }
    return st1.st_mtime < st2.st_mtime;
}

bool CompareByFileType(const FileInfo &fileInfo1, const FileInfo &fileInfo2) {
    return fileInfo1.fileType < fileInfo2.fileType;
}

void ListFiles(const std::string &folderPath, std::vector<FileInfo> &fileList, int &fileCount) {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(folderPath.c_str())) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            std::string fileName = ent->d_name;
            if (fileName != "." && fileName != "..") {
                std::string filePath = folderPath + "/" + fileName;
                struct stat fileStat;
                if (stat(filePath.c_str(), &fileStat) == 0) {
                    if (S_ISREG(fileStat.st_mode)) {
                        std::string fileType = fileName;
                        size_t dotPos = fileType.find_last_of(".");
                        if (dotPos != std::string::npos) {
                            fileType = fileType.substr(dotPos + 1);
                        } else {
                            fileType = "Unknown";
                        }
                        fileList.push_back({filePath, fileType});
                        fileCount++;
                    } else if (S_ISDIR(fileStat.st_mode)) {
                        ListFiles(filePath, fileList, fileCount);
                    }
                }
            }
        }
        closedir(dir);
    } else {
        std::cerr << "Error: Unable to open directory " << folderPath << std::endl;
    }
}

std::string SortAndPrintFiles(const std::vector<FileInfo> &fileList, int sortingOption) {
    std::vector<FileInfo> sortedList = fileList;
    std::string resp;
    if (sortingOption == 1) {
        std::sort(sortedList.begin(), sortedList.end(), [](const FileInfo &a, const FileInfo &b) {
            return a.filePath < b.filePath;
        });
    } else if (sortingOption == 2) {
        std::sort(sortedList.begin(), sortedList.end(), [](const FileInfo &a, const FileInfo &b) {
            return CompareByCreationDate(a.filePath, b.filePath);
        });
    } else if (sortingOption == 3) {
        std::sort(sortedList.begin(), sortedList.end(), CompareByFileType);
    } else {
        resp = "Invalid sorting option.\n";
        return resp;
    }

    std::string serializedData;
    for (const FileInfo &fileInfo: sortedList) {
        resp = fileInfo.filePath + " (" + fileInfo.fileType + ")";
        serializedData += resp + "\n";
    }
    return serializedData;
}

std::string execute(int sortingOption) {
    std::string folderPath = ".";
    std::vector<FileInfo> fileList;
    int fileCount = 0;
    ListFiles(folderPath, fileList, fileCount);

    std::string sorted_files_string = SortAndPrintFiles(fileList, sortingOption);

    sorted_files_string += "Total files:" + std::to_string(fileCount) + "\n";

    return sorted_files_string;
}

#endif
#endif //SERVER_GL_FILESORT_LINUX_H
