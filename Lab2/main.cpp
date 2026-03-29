#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <cctype>
#include <new>

using namespace std;

// Максимальная длина ключа (256 символов + терминатор)
const int MAX_KEY_LEN = 257;

// Коды ошибок
enum ErrorCode {
    ERR_OK = 0,
    ERR_OUT_OF_MEMORY,
    ERR_FILE_OPEN,
    ERR_FILE_WRITE,
    ERR_FILE_READ,
    ERR_INVALID_FORMAT
};

// Результат операции вставки
enum InsertResult {
    INSERT_OK = 0,
    INSERT_EXIST,
    INSERT_ERROR
};

// Структура результата операции
struct Result {
    ErrorCode code;
    char message[256];

    Result() : code(ERR_OK) { message[0] = '\0'; }
    Result(ErrorCode c, const char* msg = "") : code(c) {
        strncpy(message, msg, sizeof(message) - 1);
        message[sizeof(message) - 1] = '\0';
    }

    bool isSuccess() const { return code == ERR_OK; }
};

// Узел AVL-дерева
struct Node {
    char* key;
    uint64_t value;
    int height;
    Node* left;
    Node* right;

    Node(const char* k, uint64_t v) : value(v), height(1), left(nullptr), right(nullptr) {
        key = new char[strlen(k) + 1];
        strcpy(key, k);
    }

    ~Node() {
        delete[] key;
    }
};

// Программная библиотека AVL-словаря
class Dictionary {
private:
    Node* root;

    int height(Node* N) {
        return N ? N->height : 0;
    }

    int getBalance(Node* N) {
        return N ? height(N->left) - height(N->right) : 0;
    }

    void updateHeight(Node* N) {
        if (N) N->height = 1 + max(height(N->left), height(N->right));
    }

    Node* rightRotate(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;

        x->right = y;
        y->left = T2;

        updateHeight(y);
        updateHeight(x);
        return x;
    }

    Node* leftRotate(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;

        y->left = x;
        x->right = T2;

        updateHeight(x);
        updateHeight(y);
        return y;
    }

    Node* balanceNode(Node* node) {
        updateHeight(node);
        int balance = getBalance(node);

        // Left Left Case
        if (balance > 1 && getBalance(node->left) >= 0)
            return rightRotate(node);

        // Left Right Case
        if (balance > 1 && getBalance(node->left) < 0) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        // Right Right Case
        if (balance < -1 && getBalance(node->right) <= 0)
            return leftRotate(node);

        // Right Left Case
        if (balance < -1 && getBalance(node->right) > 0) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    Node* insert(Node* node, const char* key, uint64_t value, bool& added) {
        if (!node) {
            added = true;
            return new (std::nothrow) Node(key, value);
        }

        int cmp = strcmp(key, node->key);
        if (cmp < 0)
            node->left = insert(node->left, key, value, added);
        else if (cmp > 0)
            node->right = insert(node->right, key, value, added);
        else {
            added = false; // Ключ уже существует
            return node;
        }

        return balanceNode(node);
    }

    Node* minValueNode(Node* node) {
        Node* current = node;
        while (current->left != nullptr)
            current = current->left;
        return current;
    }

    Node* remove(Node* root, const char* key, bool& removed) {
        if (!root) {
            removed = false;
            return root;
        }

        int cmp = strcmp(key, root->key);
        if (cmp < 0)
            root->left = remove(root->left, key, removed);
        else if (cmp > 0)
            root->right = remove(root->right, key, removed);
        else {
            removed = true;
            if (!root->left || !root->right) {
                Node* temp = root->left ? root->left : root->right;
                delete root;
                return temp;
            } else {
                Node* temp = minValueNode(root->right);
                delete[] root->key;
                root->key = new char[strlen(temp->key) + 1];
                strcpy(root->key, temp->key);
                root->value = temp->value;
                bool dummy; // фиктивный флаг для рекурсивного удаления
                root->right = remove(root->right, temp->key, dummy);
            }
        }

        if (!root) return root;
        return balanceNode(root);
    }

    Node* search(Node* root, const char* key) const {
        if (!root)
            return nullptr;
        int cmp = strcmp(root->key, key);
        if (cmp == 0)
            return root;
        if (cmp < 0)
            return search(root->right, key);
        return search(root->left, key);
    }

    void clear(Node* node) {
        if (!node) return;
        clear(node->left);
        clear(node->right);
        delete node;
    }

    uint64_t countNodes(Node* node) const {
        if (!node) return 0;
        return 1 + countNodes(node->left) + countNodes(node->right);
    }

    void writeNodes(Node* node, FILE* out, Result& result) const {
        if (!node) return;
        uint16_t len = strlen(node->key);
        if (fwrite(&len, sizeof(len), 1, out) != 1) {
            result = Result(ERR_FILE_WRITE, "File write error");
            return;
        }
        if (fwrite(node->key, 1, len, out) != len) {
            result = Result(ERR_FILE_WRITE, "File write error");
            return;
        }
        if (fwrite(&node->value, sizeof(node->value), 1, out) != 1) {
            result = Result(ERR_FILE_WRITE, "File write error");
            return;
        }

        writeNodes(node->left, out, result);
        if (!result.isSuccess()) return;
        writeNodes(node->right, out, result);
    }

public:
    Dictionary() : root(nullptr) {}
    ~Dictionary() { clear(); }

    void clear() {
        clear(root);
        root = nullptr;
    }

    InsertResult insert(const char* key, uint64_t value) {
        bool added = false;
        Node* newNode = insert(root, key, value, added);
        if (added) {
            // Ключ был добавлен
            if (newNode == nullptr) {
                // Не удалось выделить память
                return INSERT_ERROR;
            }
            root = newNode;
            return INSERT_OK;
        } else {
            // Ключ уже существует
            return INSERT_EXIST;
        }
    }

    bool remove(const char* key) {
        bool removed = false;
        root = remove(root, key, removed);
        return removed;
    }

    bool search(const char* key, uint64_t& val) const {
        Node* res = search(root, key);
        if (res) {
            val = res->value;
            return true;
        }
        return false;
    }

    Result save(const char* path) const {
        Result result;
        FILE* out = fopen(path, "wb");
        if (!out) {
            return Result(ERR_FILE_OPEN, "Cannot open file for writing");
        }

        uint32_t magic = 0x41564C31; // Маркер "AVL1" для валидации формата
        if (fwrite(&magic, sizeof(magic), 1, out) != 1) {
            fclose(out);
            return Result(ERR_FILE_WRITE, "File write error");
        }

        uint64_t size = countNodes(root);
        if (fwrite(&size, sizeof(size), 1, out) != 1) {
            fclose(out);
            return Result(ERR_FILE_WRITE, "File write error");
        }

        writeNodes(root, out, result);
        if (!result.isSuccess()) {
            fclose(out);
            return result;
        }

        fclose(out);
        return result;
    }

    Result load(const char* path) {
        FILE* in = fopen(path, "rb");

        // По условию: отсутствующий файл = ОК и пустое дерево
        if (!in) {
            return Result(ERR_OK);
        }

        // Проверяем размер файла
        fseek(in, 0, SEEK_END);
        long fileSize = ftell(in);
        fseek(in, 0, SEEK_SET);

        // Пустой файл = ОК и пустое дерево
        if (fileSize == 0) {
            fclose(in);
            clear();
            return Result(ERR_OK);
        }

        uint32_t magic = 0;
        if (fread(&magic, sizeof(magic), 1, in) != 1 || magic != 0x41564C31) {
            fclose(in);
            return Result(ERR_INVALID_FORMAT, "Invalid file format");
        }

        uint64_t size = 0;
        if (fread(&size, sizeof(size), 1, in) != 1) {
            fclose(in);
            return Result(ERR_INVALID_FORMAT, "Invalid file format");
        }

        Node* newRoot = nullptr;
        bool error = false;
        char errorMsg[256] = "Invalid file format";

        for (uint64_t i = 0; i < size && !error; ++i) {
            uint16_t len = 0;
            if (fread(&len, sizeof(len), 1, in) != 1 || len > 256) {
                error = true;
                break;
            }

            char* key = new (std::nothrow) char[len + 1];
            if (!key) {
                error = true;
                strcpy(errorMsg, "Out of memory");
                break;
            }

            if (fread(key, 1, len, in) != len) {
                delete[] key;
                error = true;
                break;
            }
            key[len] = '\0';

            uint64_t val = 0;
            if (fread(&val, sizeof(val), 1, in) != 1) {
                delete[] key;
                error = true;
                break;
            }

            bool added = false;
            Node* newNode = insert(newRoot, key, val, added);
            if (newNode == nullptr && added) {
                delete[] key;
                error = true;
                strcpy(errorMsg, "Out of memory");
                break;
            }
            newRoot = newNode;
            delete[] key;
        }

        if (error) {
            // Безопасное удаление временного дерева при ошибке чтения
            clear(newRoot);
            fclose(in);
            return Result(ERR_FILE_READ, errorMsg);
        }

        fclose(in);

        // Если загрузка прошла успешно, заменяем текущее дерево
        clear();
        root = newRoot;
        return Result(ERR_OK);
    }
};

// Хелпер для приведения ключей к нижнему регистру
void toLower(const char* src, char* dest) {
    int i = 0;
    while (src[i] != '\0' && i < MAX_KEY_LEN - 1) {
        dest[i] = tolower(static_cast<unsigned char>(src[i]));
        i++;
    }
    dest[i] = '\0';
}

int main() {
    Dictionary dict;
    char cmd[MAX_KEY_LEN];

    while (cin >> cmd) {
        if (strcmp(cmd, "+") == 0) {
            char word[MAX_KEY_LEN];
            uint64_t val;
            if (!(cin >> word >> val)) break;
            char lowerWord[MAX_KEY_LEN];
            toLower(word, lowerWord);
            InsertResult res = dict.insert(lowerWord, val);
            if (res == INSERT_OK) cout << "OK\n";
            else if (res == INSERT_EXIST) cout << "Exist\n";
            else cout << "ERROR: Out of memory\n";

        } else if (strcmp(cmd, "-") == 0) {
            char word[MAX_KEY_LEN];
            if (!(cin >> word)) break;
            char lowerWord[MAX_KEY_LEN];
            toLower(word, lowerWord);
            if (dict.remove(lowerWord)) cout << "OK\n";
            else cout << "NoSuchWord\n";

        } else if (strcmp(cmd, "!") == 0) {
            char action[MAX_KEY_LEN];
            char path[512];
            if (!(cin >> action >> path)) break;
            Result res;
            if (strcmp(action, "Save") == 0) {
                res = dict.save(path);
            } else if (strcmp(action, "Load") == 0) {
                res = dict.load(path);
            } else {
                cout << "ERROR: Unknown action\n";
                continue;
            }
            if (res.isSuccess()) {
                cout << "OK\n";
            } else {
                cout << "ERROR: " << res.message << "\n";
            }
        } else {
            uint64_t val;
            char lowerCmd[MAX_KEY_LEN];
            toLower(cmd, lowerCmd);
            if (dict.search(lowerCmd, val)) cout << "OK: " << val << "\n";
            else cout << "NoSuchWord\n";
        }
    }

    return 0;
}
