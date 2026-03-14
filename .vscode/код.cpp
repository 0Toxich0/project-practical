#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <ctime>
#include <sstream>
#include <climits>  // Добавить для LLONG_MAX

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

// Функция для вывода линии заданной длины
void printLine(char c, int length) {
    for (int i = 0; i < length; i++) cout << c;
    cout << endl;
}

// Функция для получения текущей даты и времени
string getCurrentDateTime() {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    stringstream ss;
    ss << setfill('0') 
       << setw(2) << ltm->tm_mday << "."
       << setw(2) << 1 + ltm->tm_mon << "."
       << 1900 + ltm->tm_year << ", "
       << setw(2) << ltm->tm_hour << ":"
       << setw(2) << ltm->tm_min << ":"
       << setw(2) << ltm->tm_sec;
    
    return ss.str();
}

// Экспорт в TXT
void exportToTXT(const vector<Product>& products, const vector<int>& best, 
                 int totalHours, int totalMaterial, int mode, int optimalProfit,
                 long long checks, double calcTime, double speedup) {
    
    string filename = "optimization_result.txt";
    ofstream file(filename);
    
    if (!file.is_open()) {
        cout << "Ошибка создания файла " << filename << endl;
        return;
    }
    
    int usedHours = 0, usedMaterial = 0;
    for (size_t i = 0; i < products.size(); i++) {
        usedHours += best[i] * products[i].labor;
        usedMaterial += best[i] * products[i].material;
    }
    
    file << "ОПТИМАЛЬНОЕ РАСПРЕДЕЛЕНИЕ РЕСУРСОВ\n";
    file << "===================================================\n\n";
    file << "Дата расчёта: " << getCurrentDateTime() << "\n";
    file << "Режим: " << (mode == 1 ? "МАКСИМИЗАЦИЯ" : "МИНИМИЗАЦИЯ") << " прибыли\n";
    file << "Ресурсы: " << totalHours << " часов, " << totalMaterial << " кг материалов\n\n";
    file << "ОПТИМАЛЬНЫЙ ПЛАН:\n";
    file << "---\n";
    
    for (size_t i = 0; i < products.size(); i++) {
        file << products[i].name << ": " << best[i] << " ед.\n";
    }
    
    file << "\n===================================================\n\n";
    file << "ПРИБЫЛЬ: " << optimalProfit << "\n\n";
    file << "ИСПОЛЬЗОВАНИЕ РЕСУРСОВ:\n";
    file << "Часы: " << usedHours << " из " << totalHours << " ("
         << fixed << setprecision(1) << (usedHours * 100.0 / totalHours) << "%)\n";
    file << "Материалы: " << usedMaterial << " из " << totalMaterial << " ("
         << (usedMaterial * 100.0 / totalMaterial) << "%)\n\n";
    file << "СТАТИСТИКА РАСЧЁТА:\n";
    file << "Проверено комбинаций: " << checks << "\n";
    file << "Время расчёта: " << fixed << setprecision(2) << calcTime << " мс\n";
    file << "Ускорение: " << speedup << "x\n\n";
    file << "ДЕТАЛЬНЫЙ РАСЧЁТ:\n";
    file << "ПРОДУКТ    ТРУД    МАТЕРИАЛ    ПРИБЫЛЬ    ЭФФЕКТИВН.    ОПТИМУМ\n";
    
    for (size_t i = 0; i < products.size(); i++) {
        double efficiency = (double)products[i].profit / (products[i].labor + products[i].material);
        file << left << setw(10) << products[i].name
             << setw(7) << products[i].labor
             << setw(10) << products[i].material
             << setw(9) << products[i].profit
             << setw(12) << fixed << setprecision(2) << efficiency
             << best[i] << "\n";
    }
    
    file << "\nЭФФЕКТИВНОСТЬ ПРОДУКТОВ:\n";
    for (size_t i = 0; i < products.size(); i++) {
        double efficiency = (double)products[i].profit / (products[i].labor + products[i].material);
        file << products[i].name << ": " << fixed << setprecision(2) << efficiency << "\n";
    }
    
    file << "\nРАСЧЁТ ЗАВЕРШЁН\n";
    file.close();
    
    cout << "✓ Результаты экспортированы в файл: " << filename << "\n";
}

// Экспорт в JSON
void exportToJSON(const vector<Product>& products, const vector<int>& best,
                  int totalHours, int totalMaterial, int mode, int optimalProfit,
                  long long checks, double calcTime, double speedup) {
    
    string filename = "optimization_result.json";
    ofstream file(filename);
    
    if (!file.is_open()) {
        cout << "Ошибка создания файла " << filename << endl;
        return;
    }
    
    int usedHours = 0, usedMaterial = 0;
    for (size_t i = 0; i < products.size(); i++) {
        usedHours += best[i] * products[i].labor;
        usedMaterial += best[i] * products[i].material;
    }
    
    file << "{\n";
    file << "  \"title\": \"ОПТИМАЛЬНОЕ РАСПРЕДЕЛЕНИЕ РЕСУРСОВ\",\n";
    file << "  \"date\": \"" << getCurrentDateTime() << "\",\n";
    file << "  \"mode\": \"" << (mode == 1 ? "МАКСИМИЗАЦИЯ" : "МИНИМИЗАЦИЯ") << "\",\n";
    file << "  \"resources\": {\n";
    file << "    \"hours\": {\n";
    file << "      \"total\": " << totalHours << ",\n";
    file << "      \"used\": " << usedHours << ",\n";
    file << "      \"percent\": \"" << fixed << setprecision(1) << (usedHours * 100.0 / totalHours) << "\"\n";
    file << "    },\n";
    file << "    \"material\": {\n";
    file << "      \"total\": " << totalMaterial << ",\n";
    file << "      \"used\": " << usedMaterial << ",\n";
    file << "      \"percent\": \"" << (usedMaterial * 100.0 / totalMaterial) << "\"\n";
    file << "    }\n";
    file << "  },\n";
    file << "  \"optimalPlan\": [\n";
    
    for (size_t i = 0; i < products.size(); i++) {
        file << "    {\n";
        file << "      \"name\": \"" << products[i].name << "\",\n";
        file << "      \"quantity\": " << best[i] << "\n";
        file << "    }" << (i < products.size() - 1 ? "," : "") << "\n";
    }
    
    file << "  ],\n";
    file << "  \"profit\": " << optimalProfit << ",\n";
    file << "  \"statistics\": {\n";
    file << "    \"checks\": " << checks << ",\n";
    file << "    \"time\": " << fixed << setprecision(2) << calcTime << ",\n";
    file << "    \"speedup\": " << speedup << "\n";
    file << "  },\n";
    file << "  \"detailedCalculation\": [\n";
    
    for (size_t i = 0; i < products.size(); i++) {
        double efficiency = (double)products[i].profit / (products[i].labor + products[i].material);
        file << "    {\n";
        file << "      \"name\": \"" << products[i].name << "\",\n";
        file << "      \"labor\": " << products[i].labor << ",\n";
        file << "      \"material\": " << products[i].material << ",\n";
        file << "      \"profit\": " << products[i].profit << ",\n";
        file << "      \"efficiency\": " << fixed << setprecision(2) << efficiency << ",\n";
        file << "      \"optimum\": " << best[i] << "\n";
        file << "    }" << (i < products.size() - 1 ? "," : "") << "\n";
    }
    
    file << "  ]\n";
    file << "}\n";
    file.close();
    
    cout << "✓ Результаты экспортированы в файл: " << filename << "\n";
}

// Экспорт в XLSX (CSV формат для простоты, можно открыть в Excel)
void exportToXLSX(const vector<Product>& products, const vector<int>& best,
                  int totalHours, int totalMaterial, int mode, int optimalProfit,
                  long long checks, double calcTime, double speedup) {
    
    string filename = "optimization_result.csv";
    ofstream file(filename);
    
    if (!file.is_open()) {
        cout << "Ошибка создания файла " << filename << endl;
        return;
    }
    
    int usedHours = 0, usedMaterial = 0;
    for (size_t i = 0; i < products.size(); i++) {
        usedHours += best[i] * products[i].labor;
        usedMaterial += best[i] * products[i].material;
    }
    
    // UTF-8 BOM для корректного отображения кириллицы в Excel
    unsigned char bom[] = {0xEF, 0xBB, 0xBF};
    file.write((char*)bom, sizeof(bom));
    
    file << "ОПТИМАЛЬНОЕ РАСПРЕДЕЛЕНИЕ РЕСУРСОВ\n";
    file << "===================================================\n";
    file << "\n";
    file << "Дата расчёта:," << getCurrentDateTime() << "\n";
    file << "Режим:," << (mode == 1 ? "МАКСИМИЗАЦИЯ" : "МИНИМИЗАЦИЯ") << " прибыли\n";
    file << "Ресурсы:," << totalHours << " часов, " << totalMaterial << " кг материалов\n";
    file << "\n";
    file << "ОПТИМАЛЬНЫЙ ПЛАН:\n";
    file << "---\n";
    
    for (size_t i = 0; i < products.size(); i++) {
        file << products[i].name << "," << best[i] << " ед.\n";
    }
    
    file << "\n";
    file << "===================================================\n";
    file << "\n";
    file << "ПРИБЫЛЬ:," << optimalProfit << "\n";
    file << "\n";
    file << "ИСПОЛЬЗОВАНИЕ РЕСУРСОВ:\n";
    file << "Часы:," << usedHours << " из " << totalHours << "," 
         << fixed << setprecision(1) << (usedHours * 100.0 / totalHours) << "%\n";
    file << "Материалы:," << usedMaterial << " из " << totalMaterial << ","
         << (usedMaterial * 100.0 / totalMaterial) << "%\n";
    file << "\n";
    file << "СТАТИСТИКА РАСЧЁТА:\n";
    file << "Проверено комбинаций:," << checks << "\n";
    file << "Время расчёта:," << fixed << setprecision(2) << calcTime << " мс\n";
    file << "Ускорение:," << speedup << "x\n";
    file << "\n";
    file << "ДЕТАЛЬНЫЙ РАСЧЁТ:\n";
    file << "ПРОДУКТ,ТРУД,МАТЕРИАЛ,ПРИБЫЛЬ,ЭФФЕКТИВН.,ОПТИМУМ\n";
    
    for (size_t i = 0; i < products.size(); i++) {
        double efficiency = (double)products[i].profit / (products[i].labor + products[i].material);
        file << products[i].name << ","
             << products[i].labor << ","
             << products[i].material << ","
             << products[i].profit << ","
             << fixed << setprecision(2) << efficiency << ","
             << best[i] << "\n";
    }
    
    file << "\n";
    file << "ЭФФЕКТИВНОСТЬ ПРОДУКТОВ:\n";
    for (size_t i = 0; i < products.size(); i++) {
        double efficiency = (double)products[i].profit / (products[i].labor + products[i].material);
        file << products[i].name << "," << fixed << setprecision(2) << efficiency << "\n";
    }
    
    file << "\n";
    file << "РАСЧЁТ ЗАВЕРШЁН\n";
    file.close();
    
    cout << "✓ Результаты экспортированы в файл: " << filename << " (можно открыть в Excel)\n";
}

int main() {
    cout << "\n";
    printLine('=', 60);
    cout << "     ОПТИМАЛЬНЫЕ РЕШЕНИЯ В УСЛОВИЯХ ОГРАНИЧЕННЫХ РЕСУРСОВ     \n";
    printLine('=', 60);
    cout << "\n";

    int N;
    cout << "Введите количество продуктов (1-10): ";
    cin >> N;

    vector<Product> p(N);

    cout << "\n";
    printLine('-', 50);
    
    for (int i = 0; i < N; i++) {
        cout << "\nПРОДУКТ " << i + 1 << "\n";
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

    cout << "\n";
    printLine('-', 50);
    cout << "\nОбщее количество часов: ";
    cin >> totalHours;

    cout << "Общее количество материала (кг): ";
    cin >> totalMaterial;

    // Выбор режима оптимизации
    int mode;
    cout << "\nРЕЖИМ РАСЧЁТА:\n";
    cout << "1 - Максимизация прибыли\n";
    cout << "2 - Минимизация прибыли\n";
    cout << "Ваш выбор: ";
    cin >> mode;
    
    cout << "\n";
    printLine('=', 60);
    cout << "                    РЕЗУЛЬТАТЫ РАСЧЁТА                    \n";
    printLine('=', 60);
    cout << "\n";

    vector<int> best(N, 0);
    vector<int> current(N, 0);
    
    int optimalProfit;
    if (mode == 1) {
        optimalProfit = -1;
    } else {
        optimalProfit = numeric_limits<int>::max();
    }
    
    long long checks = 0;

    // Вычисление максимальных количеств
    vector<int> maxCount(N);
    cout << "МАКСИМАЛЬНО ВОЗМОЖНОЕ КОЛИЧЕСТВО КАЖДОГО ПРОДУКТА:\n";
    printLine('-', 50);
    for (int i = 0; i < N; i++) {
        int byHours = totalHours / p[i].labor;
        int byMaterial = totalMaterial / p[i].material;
        maxCount[i] = min(byHours, byMaterial);
        cout << left << setw(10) << p[i].name 
             << ": " << maxCount[i] << " ед. (по труду: " << byHours 
             << ", по материалам: " << byMaterial << ")\n";
    }

    // Теоретическое число комбинаций (с защитой от переполнения)
    long long theoreticalCombinations = 1;
    for (int i = 0; i < N; i++) {
        if (theoreticalCombinations > LLONG_MAX / (maxCount[i] + 1)) {
            theoreticalCombinations = LLONG_MAX;
            break;
        }
        theoreticalCombinations *= (maxCount[i] + 1);
    }
    cout << "\nТЕОРЕТИЧЕСКОЕ ЧИСЛО КОМБИНАЦИЙ: " << theoreticalCombinations << "\n";

    cout << "\nНАЧАЛО РАСЧЁТА (Branch and Bound)...\n";

    // Засекаем время
    clock_t start = clock();

    // Рекурсивный перебор с отсечениями
    function<void(int, int, int, int)> bruteForce = [&](int index, int hours, int material, int profit) {
        // Отсечение по ресурсам
        if (hours > totalHours || material > totalMaterial) {
            return;
        }
        
        if (index == N) {
            checks++;
            
            if (hours <= totalHours && material <= totalMaterial) {
                if (mode == 1) {
                    if (profit > optimalProfit) {
                        optimalProfit = profit;
                        best = current;
                    }
                } else {
                    if (profit < optimalProfit) {
                        optimalProfit = profit;
                        best = current;
                    }
                }
            }
            return;
        }
        
        for (int count = 0; count <= maxCount[index]; count++) {
            current[index] = count;
            bruteForce(index + 1, 
                      hours + count * p[index].labor,
                      material + count * p[index].material,
                      profit + count * p[index].profit);
        }
    };

    bruteForce(0, 0, 0, 0);

    clock_t end = clock();
    double calcTimeMs = (double)(end - start) * 1000.0 / CLOCKS_PER_SEC;
    double speedup = (double)theoreticalCombinations / checks;

    cout << "✓ Расчёт завершён\n\n";
    
    printLine('=', 60);
    cout << "                 ОПТИМАЛЬНОЕ РАСПРЕДЕЛЕНИЕ                 \n";
    printLine('=', 60);
    cout << "\n";
    
    cout << "Режим: " << (mode == 1 ? "МАКСИМИЗАЦИЯ прибыли" : "МИНИМИЗАЦИЯ прибыли") << "\n\n";

    // Вывод оптимального плана
    cout << "ОПТИМАЛЬНЫЙ ПЛАН:\n";
    printLine('-', 50);
    for (int i = 0; i < N; i++) {
        cout << left << setw(10) << p[i].name << ": " << best[i] << " ед.\n";
    }
    
    // Расчёт использованных ресурсов
    int usedHours = 0, usedMaterial = 0;
    for (int i = 0; i < N; i++) {
        usedHours += best[i] * p[i].labor;
        usedMaterial += best[i] * p[i].material;
    }
    
    cout << "\n";
    printLine('-', 50);
    cout << "ПРИБЫЛЬ: " << optimalProfit << "\n";
    printLine('-', 50);
    
    // Использование ресурсов с прогресс-баром
    cout << "\nИСПОЛЬЗОВАНИЕ РЕСУРСОВ:\n";
    printLine('-', 50);
    
    double hoursPercent = (double)usedHours / totalHours * 100;
    cout << "Часы: " << usedHours << " из " << totalHours;
    cout << " (" << fixed << setprecision(1) << hoursPercent << "%)\n";
    cout << "[";
    int barLength = 30;
    int filledLength = (int)(hoursPercent / 100 * barLength);
    for (int i = 0; i < barLength; i++) {
        if (i < filledLength) cout << "█";
        else cout << "░";
    }
    cout << "]\n";
    
    double materialPercent = (double)usedMaterial / totalMaterial * 100;
    cout << "Материалы: " << usedMaterial << " из " << totalMaterial;
    cout << " (" << fixed << setprecision(1) << materialPercent << "%)\n";
    cout << "[";
    filledLength = (int)(materialPercent / 100 * barLength);
    for (int i = 0; i < barLength; i++) {
        if (i < filledLength) cout << "█";
        else cout << "░";
    }
    cout << "]\n";
    
    cout << "\n";
    printLine('=', 60);
    cout << "                     СТАТИСТИКА РАСЧЁТА                    \n";
    printLine('=', 60);
    cout << "\n";
    
    cout << "Проверено комбинаций: " << checks << "\n";
    cout << "Время расчёта: " << fixed << setprecision(2) << calcTimeMs << " мс\n";
    cout << "Ускорение: " << fixed << setprecision(2) << speedup << "x\n";
    
    cout << "\n";
    printLine('=', 60);
    cout << "                     ДЕТАЛЬНЫЙ РАСЧЁТ                      \n";
    printLine('=', 60);
    cout << "\n";
    
    // Таблица детального расчёта
    cout << left 
         << setw(12) << "ПРОДУКТ"
         << setw(8) << "ТРУД"
         << setw(10) << "МАТЕРИАЛ"
         << setw(10) << "ПРИБЫЛЬ"
         << setw(12) << "ЭФФЕКТИВН."
         << "ОПТИМУМ\n";
    printLine('-', 62);
    
    for (int i = 0; i < N; i++) {
        double efficiency = (double)p[i].profit / (p[i].labor + p[i].material);
        
        cout << left
             << setw(12) << p[i].name
             << setw(8) << p[i].labor
             << setw(10) << p[i].material
             << setw(10) << p[i].profit
             << setw(12) << fixed << setprecision(2) << efficiency
             << best[i] << "\n";
    }
    
    cout << "\n";
    printLine('=', 60);
    cout << "                    ЭКСПОРТ РЕЗУЛЬТАТОВ                    \n";
    printLine('=', 60);
    cout << "\n";
    
    // Экспорт в файлы
    exportToTXT(p, best, totalHours, totalMaterial, mode, optimalProfit, checks, calcTimeMs, speedup);
    exportToJSON(p, best, totalHours, totalMaterial, mode, optimalProfit, checks, calcTimeMs, speedup);
    exportToXLSX(p, best, totalHours, totalMaterial, mode, optimalProfit, checks, calcTimeMs, speedup);
    
    cout << "\n";
    printLine('=', 60);
    cout << "                    РАСЧЁТ ЗАВЕРШЁН                    \n";
    printLine('=', 60);
    cout << "\n";

    return 0;
}