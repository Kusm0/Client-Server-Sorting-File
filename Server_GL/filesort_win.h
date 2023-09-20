

#ifndef SERVER_GL_FILESORT_WIN_H
#define SERVER_GL_FILESORT_WIN_H

#ifdef _WIN32

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <algorithm>
#include <windows.h>
#include <winsock2.h>

struct FileInfo {
    std::string filePath;
    std::string fileType;
};



bool CompareByCreationDate(const std::string& filePath1, const std::string& filePath2) {
    FILETIME fileTime1, fileTime2;
    HANDLE hFile1 = CreateFile(filePath1.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
    HANDLE hFile2 = CreateFile(filePath2.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

    if (hFile1 == INVALID_HANDLE_VALUE || hFile2 == INVALID_HANDLE_VALUE) {
        CloseHandle(hFile1);
        CloseHandle(hFile2);
        return false;
    }

    GetFileTime(hFile1, NULL, NULL, &fileTime1);
    GetFileTime(hFile2, NULL, NULL, &fileTime2);

    CloseHandle(hFile1);
    CloseHandle(hFile2);

    return CompareFileTime(&fileTime1, &fileTime2) < 0;
}

bool CompareByFileType(const FileInfo& fileInfo1, const FileInfo& fileInfo2) {
    return fileInfo1.fileType < fileInfo2.fileType;
}

void ListFiles(const std::string& folderPath, std::vector<FileInfo>& fileList, int& fileCount) {
    WIN32_FIND_DATA findFileData;
    HANDLE hFind = FindFirstFile((folderPath + "\\*").c_str(), &findFileData);

    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << "Error: Unable to open directory " << folderPath << std::endl;
        return;
    }

    do {
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (strcmp(findFileData.cFileName, ".") != 0 && strcmp(findFileData.cFileName, "..") != 0) {
                std::string subFolderPath = folderPath + "\\" + findFileData.cFileName;
                ListFiles(subFolderPath, fileList, fileCount);
            }
        } else {
            std::string filePath = folderPath + "\\" + findFileData.cFileName;
            std::string fileType = findFileData.cFileName;
            size_t dotPos = fileType.find_last_of(".");
            if (dotPos != std::string::npos) {
                fileType = fileType.substr(dotPos + 1);
            } else {
                fileType = "Unknown";
            }

            fileList.push_back({ filePath, fileType });
            fileCount++;
        }
    } while (FindNextFile(hFind, &findFileData) != 0);

    FindClose(hFind);
}

std::string SortAndPrintFiles(const std::vector<FileInfo>& fileList, int sortingOption) {
    std::vector<FileInfo> sortedList = fileList;
    std::string resp;
    if (sortingOption == 1) {
        std::sort(sortedList.begin(), sortedList.end(), [](const FileInfo& a, const FileInfo& b) {
            return a.filePath < b.filePath;
        });
    } else if (sortingOption == 2) {
        std::sort(sortedList.begin(), sortedList.end(), [](const FileInfo& a, const FileInfo& b) {
            return CompareByCreationDate(a.filePath, b.filePath);
        });
    } else if (sortingOption == 3) {
        std::sort(sortedList.begin(), sortedList.end(), CompareByFileType);
    } else {
        resp = "Invalid sorting option.\n";
        return resp;
    }

    std::string serializedData;
    for (const FileInfo& fileInfo : sortedList) {
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
#endif //SERVER_GL_FILESORT_WIN_H
