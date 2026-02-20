#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdint>

struct Entry {
    uint32_t key;
    std::string key_str;
    std::string value;
};

std::vector<Entry> CountingSortEntries(std::vector<Entry> entries) {
    if (entries.empty()) return {};

    uint32_t max_val = 0;
    for (const auto& entry : entries) {
        if (entry.key > max_val) max_val = entry.key;
    }

    std::vector<uint32_t> count(max_val + 1, 0);

    for (const auto& entry : entries) {
        count[entry.key]++;
    }

    for (size_t i = 1; i < count.size(); ++i) {
        count[i] += count[i - 1];
    }

    std::vector<Entry> result(entries.size());
    for (int i = entries.size() - 1; i >= 0; --i) {
        result[count[entries[i].key] - 1] = std::move(entries[i]);
        count[entries[i].key]--;
    }

    return result;
}

int main() {
    std::vector<Entry> entries;
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            continue;
        }
        
        size_t tab_pos = line.find('\t');
        if (tab_pos == std::string::npos) {
            continue;
        }

        uint32_t key = std::stoi(line.substr(0, tab_pos));
        std::string key_str = line.substr(0, tab_pos);
        std::string value = line.substr(tab_pos + 1);

        if (value.size() > 2048) {
            value = value.substr(0, 2048);
        }

        entries.push_back({key, key_str, value});
    }

    std::vector<Entry> sorted_entries = CountingSortEntries(std::move(entries));

    for (const auto& entry : sorted_entries) {
        std::cout << entry.key_str << "\t" << entry.value << "\n";
    }

    return 0;
}