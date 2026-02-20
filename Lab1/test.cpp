#include <iostream>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <iomanip> // для красивых отступов

using namespace std;

void analyzeCounts(const std::vector<uint16_t>& keys) {
    if (keys.empty()) return;

    // 1. Находим максимум, чтобы определить размер массива count
    uint16_t max_val = *std::max_element(keys.begin(), keys.end());
    
    // Создаем массив count (индексы от 0 до max_val)
    std::vector<size_t> count(max_val + 1, 0);

    // 2. ЭТАП: Подсчет частот (сколько раз каждое число встречается)
    for (uint16_t key : keys) {
        count[key]++;
    }

    cout << "--- Этап 1: Частотный массив ---" << endl;
    cout << "Число (индекс): ";
    for (size_t i = 0; i < count.size(); ++i) cout << setw(3) << i;
    cout << "\nВстречается раз: ";
    for (size_t val : count) cout << setw(3) << val;
    cout << "\n\n";

    // 3. ЭТАП: Префиксные суммы (накопительный итог)
    // Здесь count[i] станет равен количеству элементов <= i
    for (size_t i = 1; i < count.size(); ++i) {
        count[i] += count[i - 1];
    }

    cout << "--- Этап 2: Накопительный итог ---" << endl;
    cout << "Число (индекс): ";
    for (size_t i = 0; i < count.size(); ++i) cout << setw(3) << i;
    cout << "\nКол-во <= него: ";
    for (size_t val : count) cout << setw(3) << val;
    cout << endl;

    std::vector<size_t> result(max_val + 1, 0);
    for (size_t i = keys.size(); i >= 0; --i) {
        int elem = keys[i];
        int elems = count[elem];
        result[elems-1] = keys[i]
    }

    for (size_t val : result) cout << setw(3) << val;


}

int main() {
    // Исходный массив остается неизменным
    const std::vector<uint16_t> keys = {0, 5, 3, 0, 7, 8, 10};
    
    analyzeCounts(keys);

    // Проверка, что keys не изменился
    cout << "\nПроверка исходного массива: ";
    for (auto k : keys) cout << k << " ";
    cout << endl;

    return 0;
}