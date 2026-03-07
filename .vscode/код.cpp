#include <iostream>
#include <vector>
#include <string>
#include <functional>

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

    vector<int> best(N, 0);
    vector<int> current(N, 0);
    int maxProfit = 0;
    long long checks = 0;

    vector<int> maxCount(N);
    for (int i = 0; i < N; i++) {
        int byHours = totalHours / p[i].labor;
        int byMaterial = totalMaterial / p[i].material;
        maxCount[i] = min(byHours, byMaterial);
        cout << "Максимум продукта " << p[i].name << ": " << maxCount[i] << " ед.\n";
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
                if (profit > maxProfit) {
                    maxProfit = profit;
                    best = current;
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

    for (int i = 0; i < N; i++) {
        cout << p[i].name << ": " << best[i] << " ед.\n";
    }

    cout << "Максимальная прибыль: " << maxProfit << endl;
    cout << "Всего проверок комбинаций: " << checks << endl;

    return 0;
}