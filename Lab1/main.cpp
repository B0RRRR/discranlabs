#include <cstdint>
#include <cstring>
#include <iostream>

using namespace std;

const int INITIAL_CAPACITY = 1024;
const int MAX_VALUE_LEN = 2048;
const int LINE_BUFFER_SIZE = 4096;

struct Entry {
    uint32_t key;
    char* key_str;
    uint16_t key_str_len;
    char* value;
    uint16_t value_len;
};

class EntryArray {
public:
    Entry* data;
    size_t size;
    size_t capacity;

    EntryArray() {
        data = new Entry[INITIAL_CAPACITY];
        size = 0;
        capacity = INITIAL_CAPACITY;
    }

    ~EntryArray() {
        for (size_t i = 0; i < size; ++i) {
            delete[] data[i].key_str;
            delete[] data[i].value;
        }
        delete[] data;
    }

    void push(uint32_t key, char* key_str, uint16_t key_str_len,
              char* value, uint16_t value_len) {
        if (size >= capacity) {
            reserve(capacity * 2);
        }
        Entry entry;
        entry.key = key;
        entry.key_str = key_str;
        entry.key_str_len = key_str_len;
        entry.value = value;
        entry.value_len = value_len;
        data[size++] = entry;
    }

private:
    void reserve(size_t new_capacity) {
        if (new_capacity <= capacity) return;
        Entry* new_data = new Entry[new_capacity];
        for (size_t i = 0; i < size; ++i) {
            new_data[i] = data[i];
        }
        delete[] data;
        data = new_data;
        capacity = new_capacity;
    }
};

void counting_sort(EntryArray* arr) {
    if (arr->size == 0) return;

    uint32_t max_val = 0;
    for (size_t i = 0; i < arr->size; ++i) {
        if (arr->data[i].key > max_val) {
            max_val = arr->data[i].key;
        }
    }

    uint32_t* count = new uint32_t[max_val + 1]();

    for (size_t i = 0; i < arr->size; ++i) {
        count[arr->data[i].key]++;
    }

    uint32_t total = 0;
    for (size_t i = 0; i <= max_val; ++i) {
        total += count[i];
        count[i] = total;
    }

    Entry* result = new Entry[arr->size];
    for (ssize_t i = arr->size - 1; i >= 0; --i) {
        uint32_t key = arr->data[i].key;
        result[--count[key]] = arr->data[i];
    }

    delete[] arr->data;
    arr->data = result;
    delete[] count;
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    EntryArray entries;
    char line[LINE_BUFFER_SIZE];

    while (cin.getline(line, sizeof(line))) {
        size_t len = strlen(line);
        if (len == 0) continue;

        char* tab_pos = strchr(line, '\t');
        if (!tab_pos) continue;

        *tab_pos = '\0';

        uint32_t key = atoi(line);

        const char* val = tab_pos + 1;
        size_t val_len = strlen(val);

        if (val_len > MAX_VALUE_LEN) continue;

        char* key_str = new char[tab_pos - line + 1];
        memcpy(key_str, line, tab_pos - line);
        key_str[tab_pos - line] = '\0';

        char* value = new char[val_len + 1];
        memcpy(value, val, val_len);
        value[val_len] = '\0';

        entries.push(key, key_str, tab_pos - line, value, val_len);
    }

    counting_sort(&entries);

    for (size_t i = 0; i < entries.size; ++i) {
        cout.write(entries.data[i].key_str, entries.data[i].key_str_len);
        cout.put('\t');
        cout.write(entries.data[i].value, entries.data[i].value_len);
        cout.put('\n');
    }

    return 0;
}
