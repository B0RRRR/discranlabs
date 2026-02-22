#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <cstring>

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
        data = (Entry*)malloc(INITIAL_CAPACITY * sizeof(Entry));
        size = 0;
        capacity = INITIAL_CAPACITY;
    }

    ~EntryArray() {
        for (size_t i = 0; i < size; ++i) {
            free(data[i].key_str);
            free(data[i].value);
        }
        free(data);
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
        Entry* new_data = (Entry*)realloc(data, new_capacity * sizeof(Entry));
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

    uint32_t* count = (uint32_t*)calloc(max_val + 1, sizeof(uint32_t));

    for (size_t i = 0; i < arr->size; ++i) {
        count[arr->data[i].key]++;
    }

    uint32_t total = 0;
    for (size_t i = 0; i <= max_val; ++i) {
        uint32_t c = count[i];
        count[i] = total;
        total += c;
    }

    Entry* result = (Entry*)malloc(arr->size * sizeof(Entry));
    for (size_t i = 0; i < arr->size; ++i) {
        uint32_t key = arr->data[i].key;
        result[count[key]++] = arr->data[i];
    }

    free(arr->data);
    arr->data = result;
    free(count);
}

int main() {
    EntryArray entries;
    char line[LINE_BUFFER_SIZE];

    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (len == 0) continue;
        if (line[len - 1] == '\n') {
            line[len - 1] = '\0';
            len--;
        }
        if (len == 0) continue;

        char* tab_pos = strchr(line, '\t');
        if (!tab_pos) continue;

        *tab_pos = '\0';

        uint32_t key = atoi(line);

        const char* val = tab_pos + 1;
        size_t val_len = strlen(val);

        if (val_len > MAX_VALUE_LEN) continue;

        char* key_str = (char*)malloc(tab_pos - line + 1);
        memcpy(key_str, line, tab_pos - line);
        key_str[tab_pos - line] = '\0';

        char* value = (char*)malloc(val_len + 1);
        memcpy(value, val, val_len);
        value[val_len] = '\0';

        entries.push(key, key_str, tab_pos - line, value, val_len);
    }

    counting_sort(&entries);

    for (size_t i = 0; i < entries.size; ++i) {
        fwrite(entries.data[i].key_str, 1, entries.data[i].key_str_len, stdout);
        fputc('\t', stdout);
        fwrite(entries.data[i].value, 1, entries.data[i].value_len, stdout);
        fputc('\n', stdout);
    }

    return 0;
}
