#pragma once

#include "../model/CharacterData.h"

#include <string>
#include <unordered_map>

class FileService
{
public:
    CharacterData LoadSourceData(const std::wstring& filePath) const;

    void SaveData(
        const CharacterData& data,
        const std::unordered_map<wchar_t, int>& result,
        const std::wstring& filePath
    ) const;

    std::wstring PrepareSavePath(
        const std::wstring& initialPath
    ) const;

private:
    bool CanWriteToFile(const std::wstring& filePath) const;
    bool IsRegularFile(const std::wstring& filePath) const;
    bool AskOverwrite(const std::wstring& filePath) const;
};
