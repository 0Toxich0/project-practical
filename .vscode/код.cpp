#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <limits>
#include <algorithm>
#include <iomanip>

using namespace std;

struct Product {
    string name;
    int labor;
    int material;
    int profit;
};

// Функция для сортировки по эффективности
bool compareByEfficiency(const Product& a, const Product& b) {
    double effA = (double)a.profit / (a.labor + a.material);
    double effB = (double)b.profit / (b.labor + b.material);
    return effA > effB;
}

int main() {
    cout << "=== Оптимизация распределения ресурсов ===\n";
    cout << "Версия с улучшенным алгоритмом Branch and Bound\n\n";

    int N;
    cout << "Введите количество продуктов (1-10): ";
    cin >> N;

    vector<Product> p(N);

    for (int i = 0; i < N; i++) {
        cout << "\nПродукт " << i + 1 << ":\n";
        cout << "Название: ";
        cin >> p[i].name;

        cout << "Труд (часы на единицу): ";
        cin >> p[i].labor;

        cout << "Материал (кг на единицу): ";
        cin >> p[i].material;

        cout << "Прибыль за единицу: ";
        cin >> p[i].profit;
    }

    int totalHours, totalMaterial;

    cout << "\nОбщее количество часов: ";
    cin >> totalHours;

    cout << "Общее количество материала: ";
    cin >> totalMaterial;

    // Выбор режима оптимизации
    int mode;
    cout << "\nВыберите режим оптимизации:\n";
    cout << "1 - Максимизация прибыли\n";
    cout << "2 - Минимизация прибыли\n";
    cout << "Ваш выбор: ";
    cin >> mode;

    // Сортировка для максимизации
    if (mode == 1) {
        sort(p.begin(), p.end(), compareByEfficiency);
        cout << "\nПродукты отсортированы по эффективности (прибыль/ресурсы)\n";
    }

    vector<int> best(N, 0);
    vector<int> current(N, 0);
    
    int optimalProfit;
    if (mode == 1) {
        optimalProfit = -1; // Для максимума
    } else {
        optimalProfit = numeric_limits<int>::max(); // Для минимума
    }
    
    long long checks = 0;
    long long pruned = 0; // Количество отсечённых веток

    // Вычисление максимальных количеств
    vector<int> maxCount(N);
    cout << "\n=== Максимально возможное количество каждого продукта ===\n";
    for (int i = 0; i < N; i++) {
        int byHours = totalHours / p[i].labor;
        int byMaterial = totalMaterial / p[i].material;
        maxCount[i] = min(byHours, byMaterial);
        cout << p[i].name << ": " << maxCount[i] << " ед. (по труду: " << byHours 
             << ", по материалам: " << byMaterial << ")\n";
    }

    // Теоретическое число комбинаций
    long long theoreticalCombinations = 1;
    for (int i = 0; i < N; i++) {
        theoreticalCombinations *= (maxCount[i] + 1);
    }
    cout << "\nТеоретическое число комбинаций: " << theoreticalCombinations << endl;

    // Функция оценки максимальной прибыли для отсечения
    function<long long(int, long long)> estimateMaxProfit = [&](int startIndex, long long currentProfit) {
        long long maxPossible = currentProfit;
        for (int i = startIndex; i < N; i++) {
            maxPossible += (long long)maxCount[i] * p[i].profit;
        }
        return maxPossible;
    };

    // Рекурсивный перебор с отсечениями
    function<void(int, int, int, long long)> bruteForce = [&](int index, int hours, int material, long long profit) {
        // Отсечение по ресурсам
        if (hours > totalHours || material > totalMaterial) {
            pruned++;
            return;
        }
        
        if (index == N) {
            checks++;
            
            if (hours <= totalHours && material <= totalMaterial) {
                if (mode == 1) {
                    // Режим максимизации
                    if (profit > optimalProfit) {
                        optimalProfit = profit;
                        best = current;
                    }
                } else {
                    // Режим минимизации
                    if (profit < optimalProfit) {
                        optimalProfit = profit;
                        best = current;
                    }
                }
            }
            return;
        }
        
        // Оптимизация для максимизации
        if (mode == 1) {
            // Оценка максимально возможной прибыли
            long long maxPossibleProfit = estimateMaxProfit(index, profit);
            if (maxPossibleProfit <= optimalProfit) {
                pruned += (maxCount[index] + 1);
                return; // Отсечение всей ветки
            }
            
            // Для максимизации пробуем сначала большие значения
            for (int count = maxCount[index]; count >= 0; count--) {
                current[index] = count;
                bruteForce(index + 1, 
                          hours + count * p[index].labor,
                          material + count * p[index].material,
                          profit + (long long)count * p[index].profit);
            }
        } else {
            // Для минимизации - стандартный порядок
            for (int count = 0; count <= maxCount[index]; count++) {
                current[index] = count;
                bruteForce(index + 1,
                          hours + count * p[index].labor,
                          material + count * p[index].material,
                          profit + (long long)count * p[index].profit);
            }
        }
    };

    cout << "\n=== Начало расчёта (Branch and Bound) ===\n";
    bruteForce(0, 0, 0, 0);

    cout << "\n=== Оптимальное распределение ===\n";
    cout << "Режим: " << (mode == 1 ? "МАКСИМИЗАЦИЯ прибыли" : "МИНИМИЗАЦИЯ прибыли") << "\n\n";

    for (int i = 0; i < N; i++) {
        cout << p[i].name << ": " << best[i] << " ед.\n";
    }

    cout << "\n";
    if (mode == 1) {
        cout << "Максимальная прибыль: " << optimalProfit << endl;
    } else {
        cout << "Минимальная прибыль: " << optimalProfit << endl;
    }
    
    cout << "\n=== Статистика алгоритма ===\n";
    cout << "Всего проверок комбинаций: " << checks << endl;
    cout << "Отсечено веток: " << pruned << endl;
    cout << "Теоретическое число комбинаций: " << theoreticalCombinations << endl;
    
    double speedup = (double)theoreticalCombinations / checks;
    cout << "Ускорение: " << fixed << setprecision(2) << speedup << "x" << endl;
    cout << "Эффективность отсечений: " << fixed << setprecision(2) 
         << (double)pruned / (pruned + checks) * 100 << "%\n";

    // Дополнительная информация
    cout << "\n=== Использование ресурсов ===\n";
    int usedHours = 0, usedMaterial = 0;
    for (int i = 0; i < N; i++) {
        usedHours += best[i] * p[i].labor;
        usedMaterial += best[i] * p[i].material;
    }
    
    cout << "Использовано часов: " << usedHours << " из " << totalHours 
         << " (" << fixed << setprecision(1) << (usedHours * 100.0 / totalHours) << "%)\n";
    cout << "Использовано материалов: " << usedMaterial << " из " << totalMaterial
         << " (" << (usedMaterial * 100.0 / totalMaterial) << "%)\n";

    // Эффективность продуктов
    cout << "\n=== Эффективность продуктов в оптимальном плане ===\n";
    for (int i = 0; i < N; i++) {
        if (best[i] > 0) {
            double efficiency = (double)p[i].profit / (p[i].labor + p[i].material);
            cout << p[i].name << ": " << fixed << setprecision(2) << efficiency 
                 << " прибыли на единицу ресурсов\n";
        }
    }

    return 0;
}