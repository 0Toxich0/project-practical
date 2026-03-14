#include <iostream>
#include <vector>
#include <string>
#include <functional>
#include <limits>
#include <algorithm>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <sstream>
#include <climits>

using namespace std;

struct Product
{
    string name;
    int labor;
    int material;
    int profit;
};

// Функция для вывода линии заданной длины
void printLine(char c, int length)
{
    for (int i = 0; i < length; i++)
        cout << c;
    cout << endl;
}

// Функция для получения текущей даты и времени
string getCurrentDateTime()
{
    time_t now = time(0);
    tm *ltm = localtime(&now);

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

int main()
{
    // ЗАГОЛОВОК ПРОГРАММЫ
    cout << "\n";
    printLine('=', 45);
    cout << "ОПТИМАЛЬНЫЕ РЕШЕНИЯ В УСЛОВИЯХ ОГРАНИЧЕННЫХ РЕСУРСОВ\n";
    printLine('=', 45);
    cout << "\n";

    // ВВОД КОЛИЧЕСТВА ПРОДУКТОВ
    int N;
    cout << "Введите количество продуктов (1-10): ";
    cin >> N;

    vector<Product> p(N);

    // ВВОД ДАННЫХ О ПРОДУКТАХ
    cout << "\n";
    printLine('-', 30);

    for (int i = 0; i < N; i++)
    {
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

    // ВВОД РЕСУРСОВ
    int totalHours, totalMaterial;

    cout << "\n";
    printLine('-', 30);
    cout << "\nОбщее количество часов: ";
    cin >> totalHours;

    cout << "Общее количество материала (кг): ";
    cin >> totalMaterial;

    // ВЫБОР РЕЖИМА
    int mode;
    cout << "\nРЕЖИМ РАСЧЁТА:\n";
    cout << "1 - Максимизация прибыли\n";
    cout << "2 - Минимизация прибыли\n";
    cout << "Ваш выбор: ";
    cin >> mode;

    // РЕЗУЛЬТАТЫ РАСЧЁТА
    cout << "\n";
    printLine('=', 45);
    cout << "              РЕЗУЛЬТАТЫ РАСЧЁТА              \n";
    printLine('=', 45);
    cout << "\n";

    vector<int> best(N, 0);
    vector<int> current(N, 0);

    int optimalProfit;
    if (mode == 1)
    {
        optimalProfit = -1;
    }
    else
    {
        optimalProfit = numeric_limits<int>::max();
    }

    long long checks = 0;

    // ВЫЧИСЛЕНИЕ МАКСИМАЛЬНЫХ КОЛИЧЕСТВ
    vector<int> maxCount(N);
    cout << "МАКСИМАЛЬНО ВОЗМОЖНОЕ КОЛИЧЕСТВО КАЖДОГО ПРОДУКТА:\n";
    printLine('-', 45);
    for (int i = 0; i < N; i++)
    {
        int byHours = totalHours / p[i].labor;
        int byMaterial = totalMaterial / p[i].material;
        maxCount[i] = min(byHours, byMaterial);
        cout << "  " << left << setw(5) << p[i].name
             << ": " << maxCount[i] << " ед. (по труду: " << byHours
             << ", по материалам: " << byMaterial << ")\n";
    }

    // ТЕОРЕТИЧЕСКОЕ ЧИСЛО КОМБИНАЦИЙ
    long long theoreticalCombinations = 1;
    for (int i = 0; i < N; i++)
    {
        if (theoreticalCombinations > LLONG_MAX / (maxCount[i] + 1))
        {
            theoreticalCombinations = LLONG_MAX;
            break;
        }
        theoreticalCombinations *= (maxCount[i] + 1);
    }
    cout << "\nТЕОРЕТИЧЕСКОЕ ЧИСЛО КОМБИНАЦИЙ: " << theoreticalCombinations << "\n";

    // НАЧАЛО РАСЧЁТА
    cout << "\nНАЧАЛО РАСЧЁТА (Branch and Bound)...\n";

    // ЗАСЕКАЕМ ВРЕМЯ
    clock_t start = clock();

    // РЕКУРСИВНЫЙ ПЕРЕБОР
    function<void(int, int, int, int)> bruteForce = [&](int index, int hours, int material, int profit)
    {
        if (hours > totalHours || material > totalMaterial)
        {
            return;
        }

        if (index == N)
        {
            checks++;

            if (hours <= totalHours && material <= totalMaterial)
            {
                if (mode == 1)
                {
                    if (profit > optimalProfit)
                    {
                        optimalProfit = profit;
                        best = current;
                    }
                }
                else
                {
                    if (profit < optimalProfit)
                    {
                        optimalProfit = profit;
                        best = current;
                    }
                }
            }
            return;
        }

        for (int count = 0; count <= maxCount[index]; count++)
        {
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

    // ОПТИМАЛЬНОЕ РАСПРЕДЕЛЕНИЕ
    int usedHours = 0, usedMaterial = 0;
    for (int i = 0; i < N; i++)
    {
        usedHours += best[i] * p[i].labor;
        usedMaterial += best[i] * p[i].material;
    }

    cout << "\n";
    printLine('=', 45);
    cout << "      ОПТИМАЛЬНОЕ РАСПРЕДЕЛЕНИЕ РЕСУРСОВ      \n";
    printLine('=', 45);
    cout << "\n";
    cout << "Дата расчёта: " << getCurrentDateTime() << "\n";
    cout << "Режим: " << (mode == 1 ? "МАКСИМИЗАЦИЯ" : "МИНИМИЗАЦИЯ") << " прибыли\n";
    cout << "Ресурсы: " << totalHours << " часов, " << totalMaterial << " кг материалов\n\n";

    // ОПТИМАЛЬНЫЙ ПЛАН
    cout << "ОПТИМАЛЬНЫЙ ПЛАН:\n";
    printLine('-', 20);
    for (int i = 0; i < N; i++)
    {
        cout << "  " << p[i].name << ": " << best[i] << " ед.\n";
    }

    cout << "\n";
    printLine('-', 30);
    cout << "\n";
    cout << "ПРИБЫЛЬ: " << optimalProfit << "\n\n";

    // ИСПОЛЬЗОВАНИЕ РЕСУРСОВ
    cout << "ИСПОЛЬЗОВАНИЕ РЕСУРСОВ:\n";
    printLine('-', 25);
    cout << "  Часы: " << usedHours << " из " << totalHours << " ("
         << fixed << setprecision(1) << (usedHours * 100.0 / totalHours) << "%)\n";
    cout << "  Материалы: " << usedMaterial << " из " << totalMaterial << " ("
         << (usedMaterial * 100.0 / totalMaterial) << "%)\n\n";

    // СТАТИСТИКА
    cout << "СТАТИСТИКА РАСЧЁТА:\n";
    printLine('-', 20);
    cout << "  Проверено комбинаций: " << checks << "\n";
    cout << "  Время расчёта: " << fixed << setprecision(2) << calcTimeMs << " мс\n";
    cout << "  Ускорение: " << fixed << setprecision(2) << speedup << "x\n";
    cout << "  Теоретическое число комбинаций: " << theoreticalCombinations << "\n\n";

    // ДЕТАЛЬНЫЙ РАСЧЁТ - С ПРАВИЛЬНЫМИ ОТСТУПАМИ
    cout << "ДЕТАЛЬНЫЙ РАСЧЁТ:\n";
    printLine('-', 75);
    cout << "  ПРОДУКТ    ТРУД    МАТЕРИАЛ    ПРИБЫЛЬ    ЭФФЕКТИВН.    ОПТИМУМ\n";
    printLine('-', 75);

    for (int i = 0; i < N; i++)
    {
        double efficiency = (double)p[i].profit / (p[i].labor + p[i].material);

        // Форматированный вывод с правильными отступами
        cout << "  " << left;
        cout << setw(10) << p[i].name;
        cout << setw(8) << p[i].labor;
        cout << setw(11) << p[i].material;
        cout << setw(10) << p[i].profit;
        cout << setw(12) << fixed << setprecision(2) << efficiency;
        cout << best[i] << "\n";
    }

    // ЭФФЕКТИВНОСТЬ
    cout << "\nЭФФЕКТИВНОСТЬ ПРОДУКТОВ:\n";
    printLine('-', 25);
    for (int i = 0; i < N; i++)
    {
        double efficiency = (double)p[i].profit / (p[i].labor + p[i].material);
        cout << "  " << p[i].name << ": " << fixed << setprecision(2) << efficiency << "\n";
    }

    // ЗАВЕРШЕНИЕ
    cout << "\n";
    printLine('-', 30);
    cout << "\n";
    printLine('=', 45);
    cout << "                РАСЧЁТ ЗАВЕРШЁН                \n";
    printLine('=', 45);
    cout << "\n";

    return 0;
}