#include "ConsoleApplication.h"

#include "../factory/CountingStrategyFactory.h"
#include "../utils/Utf8Converter.h"

#include <iostream>
#include <limits>
#include <stdexcept>

ConsoleApplication::ConsoleApplication()
    : countingStrategy_(CountingStrategyFactory::Create()),
      hasSourceData_(false),
      hasResult_(false)
{
}

void ConsoleApplication::Run()
{
    bool isRunning = true;

    while (isRunning)
    {
        ShowMenu();

        std::wstring command;
        std::getline(std::wcin, command);

        try
        {
            if (command == L"1")
            {
                EnterSourceData();
            }
            else if (command == L"2")
            {
                LoadSourceData();
            }
            else if (command == L"3")
            {
                SaveData();
            }
            else if (command == L"0")
            {
                isRunning = false;
            }
            else
            {
                std::wcout << L"Неизвестная команда.\n";
            }
        }
        catch (const std::exception& exception)
        {
            const std::string message = exception.what();
            const std::wstring wideMessage =
                Utf8Converter::ToWide(message);

            std::wcout
                << L"Ошибка: "
                << wideMessage
                << L'\n';
        }

        std::wcout << L'\n';
    }
}

void ConsoleApplication::ShowGreeting() const {
    std::wcout << "Лабораторная работа №1, вариант №2.\n";
    std::wcout << "Исполнители: Тарасов Артем, Александрычева Алена гр.433\n";
}
void ConsoleApplication::ShowMenu() const
{
    std::wcout
        << L"==============================\n"
        << L"  ПОДСЧЕТ ВХОЖДЕНИЙ СИМВОЛОВ\n"
        << L"==============================\n"
        << L"1. Ввести исходные данные\n"
        << L"2. Загрузить исходные данные из файла\n"
        << L"3. Сохранить данные и результат в файл\n"
        << L"0. Выход\n"
        << L"==============================\n"
        << L"Выберите пункт: ";
}

void ConsoleApplication::EnterSourceData()
{
    sourceData_ = ReadSourceDataFromConsole();

    hasSourceData_ = true;
    hasResult_ = false;
    result_.clear();

    std::wcout
        << L"\nИсходные данные успешно введены.\n";

    CalculateAndShowResult();
}

void ConsoleApplication::LoadSourceData()
{
    const std::wstring filePath = RequestFilePath();

    sourceData_ = fileService_.LoadSourceData(filePath);

    hasSourceData_ = true;
    hasResult_ = false;
    result_.clear();

    std::wcout
        << L"\nИсходные данные успешно загружены.\n";

    CalculateAndShowResult();
}

void ConsoleApplication::SaveData()
{
    if (!HasSourceData() || !HasResult())
    {
        std::wcout << L"Нет данных для сохранения.\n";
        return;
    }

    std::wcout
        << L"Введите полный путь для сохранения данных: ";

    std::wstring filePath;
    std::getline(std::wcin, filePath);

    const std::wstring preparedPath =
        fileService_.PrepareSavePath(filePath);

    fileService_.SaveData(
        sourceData_,
        result_,
        preparedPath
    );

    std::wcout
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

    std::wcout
        << L"\n==============================\n"
        << L"РЕЗУЛЬТАТ\n"
        << L"==============================\n";

    std::wcout
        << L"Исходный текст:\n"
        << sourceData_.GetText()
        << L"\n\n";

    std::wcout
        << L"Множество символов:\n";

    for (wchar_t symbol : sourceData_.GetSymbols())
    {
        std::wcout << L"'" << symbol << L"' ";
    }

    std::wcout << L"\n\n";
    std::wcout << L"Количество вхождений:\n";

    for (wchar_t symbol : sourceData_.GetSymbols())
    {
        const auto iterator = result_.find(symbol);

        if (iterator != result_.end())
        {
            std::wcout
                << L"'" << symbol << L"' -> "
                << iterator->second
                << L'\n';
        }
    }

    std::wcout
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
    std::wcout << L"Введите текст:\n";

    std::wstring text;
    std::getline(std::wcin, text);

    std::wcout
        << L"Введите количество символов множества: ";

    int symbolsCount = 0;

    if (!(std::wcin >> symbolsCount))
    {
        std::wcin.clear();
        std::wcin.ignore(
            std::numeric_limits<std::streamsize>::max(),
            L'\n'
        );

        throw std::runtime_error(
            "The number of symbols must be an integer."
        );
    }

    std::wcin.ignore(
        std::numeric_limits<std::streamsize>::max(),
        L'\n'
    );

    if (symbolsCount <= 0)
    {
        throw std::runtime_error(
            "The number of symbols must be positive."
        );
    }

    const std::vector<wchar_t> symbols =
        ReadSymbols(symbolsCount);

    return CharacterData(text, symbols);
}

std::vector<wchar_t> ConsoleApplication::ReadSymbols(
    int symbolsCount
) const
{
    std::vector<wchar_t> symbols;
    symbols.reserve(static_cast<std::size_t>(symbolsCount));

    for (int index = 0; index < symbolsCount; ++index)
    {
        std::wcout
            << L"Введите символ №"
            << index + 1
            << L": ";

        std::wstring symbolLine;
        std::getline(std::wcin, symbolLine);

        if (symbolLine.size() != 1)
        {
            throw std::runtime_error(
                "Each input line must contain exactly one character."
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

    return symbols;
}

std::wstring ConsoleApplication::RequestFilePath() const
{
    std::wcout << L"Введите полный путь к файлу: ";

    std::wstring filePath;
    std::getline(std::wcin, filePath);

    return filePath;
}
