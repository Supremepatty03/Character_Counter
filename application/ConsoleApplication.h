#pragma once

#include "../model/CharacterData.h"
#include "../service/FileService.h"
#include "../strategy/ICharacterCountingStrategy.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

class ConsoleApplication
{
public:
    ConsoleApplication();

    void Run();

private:
    void ShowMenu() const;
    void ShowGreeting() const;
    void EnterSourceData();
    void LoadSourceData();
    void SaveData();
    void CalculateAndShowResult();
    void ShowResult() const;

    bool HasSourceData() const;
    bool HasResult() const;

    CharacterData ReadSourceDataFromConsole() const;

    std::vector<wchar_t> ReadSymbols(int symbolsCount) const;

    std::wstring RequestFilePath() const;

private:
    FileService fileService_;
    std::unique_ptr<ICharacterCountingStrategy> countingStrategy_;
    CharacterData sourceData_;
    std::unordered_map<wchar_t, int> result_;
    bool hasSourceData_;
    bool hasResult_;
};
