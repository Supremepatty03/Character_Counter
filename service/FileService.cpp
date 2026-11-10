#include "FileService.h"

#include "../utils/Utf8Converter.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;

namespace
{
    const wstring sourceHeader = L"CHARACTER_COUNTER_DATA";
    const wstring textHeader = L"TEXT";
    const wstring symbolsHeader = L"SYMBOLS";
    const wstring resultHeader = L"RESULT";
}

CharacterData FileService::LoadSourceData(
    const wstring& filePath
) const
{
    const filesystem::path path(filePath);
    ifstream inputFile(path, ios::binary);

    if (!inputFile.is_open())
    {
        throw runtime_error(
            "Failed to open the file for reading."
        );
    }

    const string fileContent(
        (istreambuf_iterator<char>(inputFile)),
        istreambuf_iterator<char>()
    );

    const wstring content = Utf8Converter::ToWide(fileContent);
    wistringstream input(content);

    wstring line;

    if (!getline(input, line) || line != sourceHeader)
    {
        throw runtime_error(
            "Invalid source file format."
        );
    }

    if (!getline(input, line) || !line.empty())
    {
        throw runtime_error(
            "Invalid source file format."
        );
    }

    if (!getline(input, line) || line != textHeader)
    {
        throw runtime_error(
            "TEXT section was not found."
        );
    }

    wstring text;

    if (!getline(input, text))
    {
        throw runtime_error(
            "Failed to read the source text."
        );
    }

    if (!getline(input, line) || !line.empty())
    {
        throw runtime_error(
            "Invalid source file format."
        );
    }

    if (!getline(input, line) || line != symbolsHeader)
    {
        throw runtime_error(
            "SYMBOLS section was not found."
        );
    }

    int symbolsCount = 0;

    if (!(input >> symbolsCount))
    {
        throw runtime_error(
            "Failed to read the number of symbols."
        );
    }

    input.ignore(
        numeric_limits<streamsize>::max(),
        L'\n'
    );

    if (symbolsCount <= 0)
    {
        throw runtime_error(
            "The number of symbols must be positive."
        );
    }

    vector<wchar_t> symbols;
    symbols.reserve(static_cast<size_t>(symbolsCount));

    for (int index = 0; index < symbolsCount; ++index)
    {
        wstring symbolLine;

        if (!getline(input, symbolLine))
        {
            throw runtime_error(
                "Failed to read a symbol from the file."
            );
        }

        if (symbolLine.size() != 1)
        {
            throw runtime_error(
                "Each set line must contain exactly one character."
            );
        }

        for (wchar_t existingSymbol : symbols)
        {
            if (existingSymbol == symbolLine[0])
            {
                throw runtime_error(
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
    const unordered_map<wchar_t, int>& result,
    const wstring& filePath
) const
{
    const filesystem::path path(filePath);
    ofstream outputFile(path, ios::binary);

    if (!outputFile.is_open())
    {
        throw runtime_error(
            "Failed to open the file for writing."
        );
    }

    wstring content;

    content += sourceHeader + L"\n\n";

    content += textHeader + L"\n";
    content += data.GetText() + L"\n\n";

    content += symbolsHeader + L"\n";
    content += to_wstring(data.GetSymbols().size());
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
            content += to_wstring(iterator->second);
            content += L"\n";
        }
    }

    const string utf8Content =
        Utf8Converter::ToUtf8(content);

    outputFile.write(
        utf8Content.data(),
        static_cast<streamsize>(utf8Content.size())
    );

    if (!outputFile.good())
    {
        throw runtime_error(
            "Failed to write the file."
        );
    }
}

wstring FileService::PrepareSavePath(
    const wstring& initialPath
) const
{
    wstring currentPath = initialPath;

    while (true)
    {
        if (!filesystem::exists(currentPath))
        {
            if (CanWriteToFile(currentPath))
            {
                return currentPath;
            }

            wcout
                << L"Не удалось создать файл по указанному пути.\n"
                << L"Укажите новый путь: ";

            getline(wcin, currentPath);
            continue;
        }

        if (!IsRegularFile(currentPath))
        {
            wcout
                << L"Указанный путь не является обычным файлом.\n"
                << L"Укажите новый путь: ";

            getline(wcin, currentPath);
            continue;
        }

        if (!CanWriteToFile(currentPath))
        {
            wcout
                << L"Файл доступен только для чтения или недоступен для записи.\n"
                << L"Укажите новый путь: ";

            getline(wcin, currentPath);
            continue;
        }

        if (AskOverwrite(currentPath))
        {
            return currentPath;
        }

        wcout << L"Укажите новый путь: ";
        getline(wcin, currentPath);
    }
}

bool FileService::CanWriteToFile(
    const wstring& filePath
) const
{
    const filesystem::path path(filePath);
    ofstream outputFile(path, ios::app);

    if (!outputFile.is_open())
    {
        return false;
    }

    outputFile.close();
    return true;
}

bool FileService::IsRegularFile(
    const wstring& filePath
) const
{
    return filesystem::is_regular_file(filePath);
}

bool FileService::AskOverwrite(
    const wstring& filePath
) const
{
    while (true)
    {
        wcout
            << L"Файл \"" << filePath << L"\" уже существует.\n"
            << L"Перезаписать файл? (y/n): ";

        wstring answer;
        getline(wcin, answer);

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

        wcout << L"Введите y/n или д/н.\n";
    }
}
