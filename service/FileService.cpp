#include "FileService.h"

#include "../utils/Utf8Converter.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

namespace
{
    const std::wstring sourceHeader = L"CHARACTER_COUNTER_DATA";
    const std::wstring textHeader = L"TEXT";
    const std::wstring symbolsHeader = L"SYMBOLS";
    const std::wstring resultHeader = L"RESULT";
}

CharacterData FileService::LoadSourceData(
    const std::wstring& filePath
) const
{
    const std::filesystem::path path(filePath);
    std::ifstream inputFile(path, std::ios::binary);

    if (!inputFile.is_open())
    {
        throw std::runtime_error(
            "Failed to open the file for reading."
        );
    }

    const std::string fileContent(
        (std::istreambuf_iterator<char>(inputFile)),
        std::istreambuf_iterator<char>()
    );

    const std::wstring content = Utf8Converter::ToWide(fileContent);
    std::wistringstream input(content);

    std::wstring line;

    if (!std::getline(input, line) || line != sourceHeader)
    {
        throw std::runtime_error(
            "Invalid source file format."
        );
    }

    if (!std::getline(input, line) || !line.empty())
    {
        throw std::runtime_error(
            "Invalid source file format."
        );
    }

    if (!std::getline(input, line) || line != textHeader)
    {
        throw std::runtime_error(
            "TEXT section was not found."
        );
    }

    std::wstring text;

    if (!std::getline(input, text))
    {
        throw std::runtime_error(
            "Failed to read the source text."
        );
    }

    if (!std::getline(input, line) || !line.empty())
    {
        throw std::runtime_error(
            "Invalid source file format."
        );
    }

    if (!std::getline(input, line) || line != symbolsHeader)
    {
        throw std::runtime_error(
            "SYMBOLS section was not found."
        );
    }

    int symbolsCount = 0;

    if (!(input >> symbolsCount))
    {
        throw std::runtime_error(
            "Failed to read the number of symbols."
        );
    }

    input.ignore(
        std::numeric_limits<std::streamsize>::max(),
        L'\n'
    );

    if (symbolsCount <= 0)
    {
        throw std::runtime_error(
            "The number of symbols must be positive."
        );
    }

    std::vector<wchar_t> symbols;
    symbols.reserve(static_cast<std::size_t>(symbolsCount));

    for (int index = 0; index < symbolsCount; ++index)
    {
        std::wstring symbolLine;

        if (!std::getline(input, symbolLine))
        {
            throw std::runtime_error(
                "Failed to read a symbol from the file."
            );
        }

        if (symbolLine.size() != 1)
        {
            throw std::runtime_error(
                "Each set line must contain exactly one character."
            );
        }

        for (wchar_t existingSymbol : symbols)
        {
            if (existingSymbol == symbolLine[0])
            {
                throw std::runtime_error(
                    "The set must not contain duplicate characters."
                );
            }
        }

        symbols.push_back(symbolLine[0]);
    }

    return CharacterData(text, symbols);
}

void FileService::SaveData(
    const CharacterData& data,
    const std::unordered_map<wchar_t, int>& result,
    const std::wstring& filePath
) const
{
    const std::filesystem::path path(filePath);
    std::ofstream outputFile(path, std::ios::binary);

    if (!outputFile.is_open())
    {
        throw std::runtime_error(
            "Failed to open the file for writing."
        );
    }

    std::wstring content;

    content += sourceHeader + L"\n\n";

    content += textHeader + L"\n";
    content += data.GetText() + L"\n\n";

    content += symbolsHeader + L"\n";
    content += std::to_wstring(data.GetSymbols().size());
    content += L"\n";

    for (wchar_t symbol : data.GetSymbols())
    {
        content += symbol;
        content += L"\n";
    }

    content += L"\n";
    content += resultHeader + L"\n";

    for (wchar_t symbol : data.GetSymbols())
    {
        const auto iterator = result.find(symbol);

        if (iterator != result.end())
        {
            content += symbol;
            content += L" ";
            content += std::to_wstring(iterator->second);
            content += L"\n";
        }
    }

    const std::string utf8Content =
        Utf8Converter::ToUtf8(content);

    outputFile.write(
        utf8Content.data(),
        static_cast<std::streamsize>(utf8Content.size())
    );

    if (!outputFile.good())
    {
        throw std::runtime_error(
            "Failed to write the file."
        );
    }
}

std::wstring FileService::PrepareSavePath(
    const std::wstring& initialPath
) const
{
    std::wstring currentPath = initialPath;

    while (true)
    {
        if (!std::filesystem::exists(currentPath))
        {
            if (CanWriteToFile(currentPath))
            {
                return currentPath;
            }

            std::wcout
                << L"Не удалось создать файл по указанному пути.\n"
                << L"Укажите новый путь: ";

            std::getline(std::wcin, currentPath);
            continue;
        }

        if (!IsRegularFile(currentPath))
        {
            std::wcout
                << L"Указанный путь не является обычным файлом.\n"
                << L"Укажите новый путь: ";

            std::getline(std::wcin, currentPath);
            continue;
        }

        if (!CanWriteToFile(currentPath))
        {
            std::wcout
                << L"Файл доступен только для чтения или недоступен для записи.\n"
                << L"Укажите новый путь: ";

            std::getline(std::wcin, currentPath);
            continue;
        }

        if (AskOverwrite(currentPath))
        {
            return currentPath;
        }

        std::wcout << L"Укажите новый путь: ";
        std::getline(std::wcin, currentPath);
    }
}

bool FileService::CanWriteToFile(
    const std::wstring& filePath
) const
{
    const std::filesystem::path path(filePath);
    std::ofstream outputFile(path, std::ios::app);

    if (!outputFile.is_open())
    {
        return false;
    }

    outputFile.close();
    return true;
}

bool FileService::IsRegularFile(
    const std::wstring& filePath
) const
{
    return std::filesystem::is_regular_file(filePath);
}

bool FileService::AskOverwrite(
    const std::wstring& filePath
) const
{
    while (true)
    {
        std::wcout
            << L"Файл \"" << filePath << L"\" уже существует.\n"
            << L"Перезаписать файл? (y/n): ";

        std::wstring answer;
        std::getline(std::wcin, answer);

        if (answer == L"y" || answer == L"Y" ||
            answer == L"д" || answer == L"Д")
        {
            return true;
        }

        if (answer == L"n" || answer == L"N" ||
            answer == L"н" || answer == L"Н")
        {
            return false;
        }

        std::wcout << L"Введите y/n или д/н.\n";
    }
}
