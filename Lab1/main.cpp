#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>

struct Entry {
    char value[64];
    bool exists = false;
};


void countingSort(std::vector<uint16_t>& keys) {
    const int MAX_KEY = 65536;

    // Массив подсчёта
    std::vector<size_t> count(MAX_KEY, 0);

    // 1. Подсчёт вхождений
    for (uint16_t key : keys) {
        ++count[key];
    }

    for (auto i : keys) {
        keys[i+1] = keys[i+1] + keys[i]
    }
}

int main() {
    std::ifstream file("input.txt");
    if (!file) {
        std::cerr << "Ошибка открытия файла\n";
        return 1;
    }

    Entry table[65536];
    std::vector<uint16_t> keys;  // ← сохраняем ВСЕ ключи

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty())
            continue;

        size_t pos = line.find('\t');
        if (pos == std::string::npos)
            continue;

        int key = std::stoi(line.substr(0, pos));
        std::string value = line.substr(pos + 1);

        if (key < 0 || key > 65535)
            continue;

        // Сохраняем значение (64 байта)
        for (int i = 0; i < 64; ++i) {
            table[key].value[i] = (i < value.size()) ? value[i] : '\0';
        }

        table[key].exists = true;

        // ⭐ Добавляем ключ ВСЕГДА
        keys.push_back(static_cast<uint16_t>(key));
    }

    file.close();

    // 🔎 Пример использования списка ключей
    for (uint16_t key : keys) {
        std::cout << key << " -> ";

        for (int j = 0; j < 64 && table[key].value[j] != '\0'; ++j)
            std::cout << table[key].value[j];

        std::cout << "\n";
    }

    return 0;
}