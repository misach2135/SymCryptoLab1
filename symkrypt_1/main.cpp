#include <iostream>
#include <vector>
#include <windows.h>
#include <cwchar>
#include <fstream>
#include <cwctype>
#include <map>
#include <algorithm>

const int NUM_LETTERS = 33;

struct FreqAnalOutput
{
    int bigramCount;
    int letterCount;
    double entropy;
};

class FrequencyAnalyzer {
private:
    std::wifstream temp;
public:
    FrequencyAnalyzer(std::string fileName);


    static void processFile(std::string filePath) {
        std::locale::global(std::locale(""));
        std::vector<int> counts(NUM_LETTERS, 0);
        std::map<std::wstring, int> bigrams;
        std::wifstream file(filePath);

        if (!file.is_open()) {
            std::wcout << L"Не вдалося відкрити файл.\n";
            return;
        }

        wchar_t ch;
        wchar_t prev_ch = 0;
        std::wstring text;
        while (file.get(ch)) {
            ch = std::towlower(ch);
            if (ch >= L'а' && ch <= L'я') {
                text += ch;
            }
            else {
                text += L' ';
            }
        }

        file.close();
        auto new_end = std::unique(text.begin(), text.end());
        text.erase(new_end, text.end());

        for (size_t i = 0; i < text.size() - 1; ++i) {
            if (text[i] != L' ' && text[i + 1] != L' ') {
                bigrams[std::wstring(1, text[i]) + text[i + 1]]++;
            }
        }

        for (size_t i = 0; i < text.size() - 1; i += 2) {
            if (text[i] != L' ' && text[i + 1] != L' ') {
                bigrams[std::wstring(1, text[i]) + text[i + 1]]++;
            }
        }

        std::vector<std::pair<wchar_t, int>> sorted_counts;
        for (int i = 0; i < NUM_LETTERS; i++) {
            if (counts[i] > 0) {
                sorted_counts.push_back({ wchar_t(i + L'а'), counts[i] });
            }
        }
        std::sort(sorted_counts.begin(), sorted_counts.end(),
            [](std::pair<wchar_t, int> a, std::pair<wchar_t, int> b){
            if (a.second != b.second) {
                return a.second > b.second;
            }
            else {
                return a.first < b.first;
            }
        });

        for (const auto& pair : sorted_counts) {
            std::wcout << pair.first << L": " << pair.second << "\n";
        }

        for (const auto& pair : bigrams) {
            std::wcout << pair.first << L": " << pair.second << "\n";
        }
    }
};


int main()
{
    FrequencyAnalyzer analyzer( std::wstring("test.txt"));
    char filePath[] = ("D:\\example.txt");
    return 0;
}
