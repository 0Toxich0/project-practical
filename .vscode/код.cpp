#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <limits>

using namespace std;

struct Product {
    string name;
    int labor;
    int material;
    int profit;
};

int main() {
    cout << "=== Оптимизация распределения ресурсов ===\n";

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

    vector<int> best(N, 0);
    vector<int> current(N, 0);
    
    int optimalProfit;
    if (mode == 1) {
        optimalProfit = 0; // Для максимума начинаем с 0
    } else {
        optimalProfit = numeric_limits<int>::max(); // Для минимума начинаем с максимально возможного значения
    }
    
    long long checks = 0;

    vector<int> maxCount(N);
    cout << "\n=== Максимально возможное количество каждого продукта ===\n";
    for (int i = 0; i < N; i++) {
        int byHours = totalHours / p[i].labor;
        int byMaterial = totalMaterial / p[i].material;
        maxCount[i] = min(byHours, byMaterial);
        cout << p[i].name << ": " << maxCount[i] << " ед. (по труду: " << byHours 
             << ", по материалам: " << byMaterial << ")\n";
    }

    function<void(int)> bruteForce = [&](int productIndex) {
        if (productIndex == N) {
            checks++;
            
            int hours = 0;
            int material = 0;
            int profit = 0;
            
            for (int i = 0; i < N; i++) {
                hours += current[i] * p[i].labor;
                material += current[i] * p[i].material;
                profit += current[i] * p[i].profit;
            }
            
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
        
        for (int count = 0; count <= maxCount[productIndex]; count++) {
            current[productIndex] = count;
            bruteForce(productIndex + 1);
        }
    };

    bruteForce(0);

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
    cout << "Всего проверок комбинаций: " << checks << endl;

    // Дополнительная информация
    cout << "\n=== Использование ресурсов ===\n";
    int usedHours = 0, usedMaterial = 0;
    for (int i = 0; i < N; i++) {
        usedHours += best[i] * p[i].labor;
        usedMaterial += best[i] * p[i].material;
    }
    cout << "Использовано часов: " << usedHours << " из " << totalHours 
         << " (" << (usedHours * 100.0 / totalHours) << "%)\n";
    cout << "Использовано материалов: " << usedMaterial << " из " << totalMaterial
         << " (" << (usedMaterial * 100.0 / totalMaterial) << "%)\n";

    return 0;
}