#include "ConsoleApplication.h"

#include "../factory/CountingStrategyFactory.h"
#include "../utils/Utf8Converter.h"

#include <iostream>
#include <limits>
#include <stdexcept>

using namespace std;

namespace
{
    enum class MenuCommand
    {
        Exit,
        EnterSourceData,
        LoadSourceData,
        SaveData
    };
}

ConsoleApplication::ConsoleApplication()
    : countingStrategy_(CountingStrategyFactory::Create()),
      hasSourceData_(false),
      hasResult_(false)
{
}

void ConsoleApplication::Run()
{
    bool isRunning = true;
    ShowGreeting ();
    while (isRunning)
    {
        ShowMenu();

        wstring command;
        getline(wcin, command);

        try
        {
            if (command == to_wstring(static_cast<int>(MenuCommand::EnterSourceData)))
            {
                EnterSourceData();
            }
            else if (command == to_wstring(static_cast<int>(MenuCommand::LoadSourceData)))
            {
                LoadSourceData();
            }
            else if (command == to_wstring(static_cast<int>(MenuCommand::SaveData)))
            {
                SaveData();
            }
            else if (command == to_wstring(static_cast<int>(MenuCommand::Exit)))
            {
                isRunning = false;
            }
            else
            {
                wcout << L"Неизвестная команда.\n";
            }
        }
        catch (const exception& exception)
        {
            const string message = exception.what();
            const wstring wideMessage =
                Utf8Converter::ToWide(message);

            wcout
                << L"Ошибка: "
                << wideMessage
                << L'\n';
        }

        wcout << L'\n';
    }
}

void ConsoleApplication::ShowGreeting() const {
    wcout << L"Лабораторная работа №1, вариант №2.\n";
    wcout << L"Задан текст. Задано множество из N символов. Подсчитать количество\n";
    wcout << L"вхождений в тексте каждого из символов массива.\n";
    wcout << L"Исполнители: Тарасов Артем, Александрычева Алена гр.433\n\n";
}
void ConsoleApplication::ShowMenu() const
{
    wcout
        << L"==============================\n"
        << L"  ПОДСЧЕТ ВХОЖДЕНИЙ СИМВОЛОВ\n"
        << L"==============================\n"
        << to_wstring(static_cast<int>(MenuCommand::EnterSourceData))
        << L". Ввести исходные данные\n"
        << to_wstring(static_cast<int>(MenuCommand::LoadSourceData))
        << L". Загрузить исходные данные из файла\n"
        << to_wstring(static_cast<int>(MenuCommand::SaveData))
        << L". Сохранить данные и результат в файл\n"
        << to_wstring(static_cast<int>(MenuCommand::Exit))
        << L". Выход\n"
        << L"==============================\n"
        << L"Выберите пункт: ";
}

void ConsoleApplication::EnterSourceData()
{
    sourceData_ = ReadSourceDataFromConsole();

    hasSourceData_ = true;
    hasResult_ = false;
    result_.clear();

    wcout
        << L"\nИсходные данные успешно введены.\n";

    CalculateAndShowResult();
}

void ConsoleApplication::LoadSourceData()
{
    const wstring filePath = RequestFilePath();

    sourceData_ = fileService_.LoadSourceData(filePath);

    hasSourceData_ = true;
    hasResult_ = false;
    result_.clear();

    wcout
        << L"\nИсходные данные успешно загружены.\n";

    CalculateAndShowResult();
}

void ConsoleApplication::SaveData()
{
    if (!HasSourceData() || !HasResult())
    {
        wcout << L"Нет данных для сохранения.\n";
        return;
    }

    wcout
        << L"Введите полный путь для сохранения данных: ";

    wstring filePath;
    getline(wcin, filePath);

    const wstring preparedPath =
        fileService_.PrepareSavePath(filePath);

    fileService_.SaveData(
        sourceData_,
        result_,
        preparedPath
    );

    wcout
        << L"Исходные данные и результат успешно сохранены.\n";
}

void ConsoleApplication::CalculateAndShowResult()
{
    if (!HasSourceData())
    {
        return;
    }

    result_ = countingStrategy_->Count(sourceData_);
    hasResult_ = true;

    ShowResult();
}

void ConsoleApplication::ShowResult() const
{
    if (!HasResult())
    {
        return;
    }

    wcout
        << L"\n==============================\n"
        << L"РЕЗУЛЬТАТ\n"
        << L"==============================\n";

    wcout
        << L"Исходный текст:\n"
        << sourceData_.GetText()
        << L"\n\n";

    wcout
        << L"Множество символов:\n";

    for (wchar_t symbol : sourceData_.GetSymbols())
    {
        wcout << L"'" << symbol << L"' ";
    }

    wcout << L"\n\n";
    wcout << L"Количество вхождений:\n";

    for (wchar_t symbol : sourceData_.GetSymbols())
    {
        const auto iterator = result_.find(symbol);

        if (iterator != result_.end())
        {
            wcout
                << L"'" << symbol << L"' -> "
                << iterator->second
                << L'\n';
        }
    }

    wcout
        << L"==============================\n";
}

bool ConsoleApplication::HasSourceData() const
{
    return hasSourceData_;
}

bool ConsoleApplication::HasResult() const
{
    return hasResult_;
}

CharacterData ConsoleApplication::ReadSourceDataFromConsole() const
{
    wcout << L"Введите текст:\n";

    wstring text;
    getline(wcin, text);

    wcout
        << L"Введите количество символов множества: ";

    int symbolsCount = 0;

    if (!(wcin >> symbolsCount))
    {
        wcin.clear();
        wcin.ignore(
            numeric_limits<streamsize>::max(),
            L'\n'
        );

        throw runtime_error(
            "The number of symbols must be an integer."
        );
    }

    wcin.ignore(
        numeric_limits<streamsize>::max(),
        L'\n'
    );

    if (symbolsCount <= 0)
    {
        throw runtime_error(
            "The number of symbols must be positive."
        );
    }

    const vector<wchar_t> symbols =
        ReadSymbols(symbolsCount);

    return CharacterData(text, symbols);
}

vector<wchar_t> ConsoleApplication::ReadSymbols(
    int symbolsCount
) const
{
    vector<wchar_t> symbols;
    symbols.reserve(static_cast<size_t>(symbolsCount));

    for (int index = 0; index < symbolsCount; ++index)
    {
        wcout
            << L"Введите символ №"
            << index + 1
            << L": ";

        wstring symbolLine;
        getline(wcin, symbolLine);

        if (symbolLine.size() != 1)
        {
            throw runtime_error(
                "Each input line must contain exactly one character."
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

    return symbols;
}

wstring ConsoleApplication::RequestFilePath() const
{
    wcout << L"Введите полный путь к файлу: ";

    wstring filePath;
    getline(wcin, filePath);

    return filePath;
}
