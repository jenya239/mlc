// Демонстрация управления памятью для массивов Aurora
// Компилировать: clang++ -std=c++11 -o memory_demo memory_lifetime_demo.cpp
// Запустить: ./memory_demo

#include <iostream>
#include <vector>

// Класс для отслеживания конструкторов/деструкторов
class TrackedInt {
public:
    int value;
    static int alive_count;

    TrackedInt(int v) : value(v) {
        alive_count++;
        std::cout << "  [+] Создан TrackedInt(" << value << "), всего живых: " << alive_count << "\n";
    }

    TrackedInt(const TrackedInt& other) : value(other.value) {
        alive_count++;
        std::cout << "  [C] Скопирован TrackedInt(" << value << "), всего живых: " << alive_count << "\n";
    }

    ~TrackedInt() {
        alive_count--;
        std::cout << "  [-] Уничтожен TrackedInt(" << value << "), осталось живых: " << alive_count << "\n";
    }
};

int TrackedInt::alive_count = 0;

void demo_basic_lifetime() {
    std::cout << "\n=== DEMO 1: Базовое время жизни ===\n";

    {
        std::cout << "Создаём vector...\n";
        std::vector<TrackedInt> arr{
            TrackedInt(1),
            TrackedInt(2),
            TrackedInt(3)
        };

        std::cout << "\nVector создан, элементы живы\n";
        std::cout << "Размер: " << arr.size() << "\n";

        std::cout << "\nВыходим из scope...\n";
    } // <- Здесь вызывается деструктор vector

    std::cout << "\nПосле выхода из scope\n";
    std::cout << "Осталось живых объектов: " << TrackedInt::alive_count << "\n";
}

void demo_push_reallocation() {
    std::cout << "\n=== DEMO 2: Push и реаллокация ===\n";

    std::vector<TrackedInt> arr;
    arr.reserve(2);  // Резервируем место для 2 элементов

    std::cout << "Capacity: " << arr.capacity() << ", Size: " << arr.size() << "\n\n";

    std::cout << "Push 1:\n";
    arr.push_back(TrackedInt(1));
    std::cout << "Capacity: " << arr.capacity() << ", Size: " << arr.size() << "\n\n";

    std::cout << "Push 2:\n";
    arr.push_back(TrackedInt(2));
    std::cout << "Capacity: " << arr.capacity() << ", Size: " << arr.size() << "\n\n";

    std::cout << "Push 3 (РЕАЛЛОКАЦИЯ!):\n";
    arr.push_back(TrackedInt(3));
    std::cout << "Capacity: " << arr.capacity() << ", Size: " << arr.size() << "\n";
    std::cout << "Заметь: старые элементы скопированы, потом уничтожены!\n\n";

    std::cout << "Выходим из scope...\n";
}

void demo_copy_vs_move() {
    std::cout << "\n=== DEMO 3: Копирование vs перемещение ===\n";

    {
        std::cout << "Создаём arr1:\n";
        std::vector<TrackedInt> arr1{TrackedInt(10), TrackedInt(20)};

        std::cout << "\nКопирование (arr2 = arr1):\n";
        std::vector<TrackedInt> arr2 = arr1;
        std::cout << "Теперь у нас ДВА массива с копиями элементов!\n";
        std::cout << "arr1.size() = " << arr1.size() << ", arr2.size() = " << arr2.size() << "\n";

        std::cout << "\nПеремещение (arr3 = std::move(arr1)):\n";
        std::vector<TrackedInt> arr3 = std::move(arr1);
        std::cout << "arr1 теперь пустой (данные переехали в arr3)\n";
        std::cout << "arr1.size() = " << arr1.size() << ", arr3.size() = " << arr3.size() << "\n";
        std::cout << "Никакого копирования элементов!\n";

        std::cout << "\nВыходим из scope...\n";
    }

    std::cout << "\nПосле выхода: " << TrackedInt::alive_count << " живых объектов\n";
}

void demo_nested_scopes() {
    std::cout << "\n=== DEMO 4: Вложенные scope ===\n";

    std::cout << "Внешний scope:\n";
    std::vector<TrackedInt> outer{TrackedInt(100)};

    {
        std::cout << "\n  Внутренний scope 1:\n";
        std::vector<TrackedInt> inner1{TrackedInt(200), TrackedInt(201)};

        {
            std::cout << "\n    Внутренний scope 2:\n";
            std::vector<TrackedInt> inner2{TrackedInt(300)};

            std::cout << "    Живых объектов: " << TrackedInt::alive_count << "\n";
            std::cout << "    Выход из scope 2...\n";
        }

        std::cout << "  После scope 2, живых: " << TrackedInt::alive_count << "\n";
        std::cout << "  Выход из scope 1...\n";
    }

    std::cout << "После scope 1, живых: " << TrackedInt::alive_count << "\n";
    std::cout << "Выход из внешнего scope...\n";
}

// Эмуляция Aurora кода
namespace Aurora {
    // fn create_array() -> i32 =
    //   let arr = [1, 2, 3, 4, 5];
    //   arr.length()
    int create_array() {
        std::cout << "\n=== AURORA: create_array() ===\n";
        std::vector<int> arr = std::vector<int>{1, 2, 3, 4, 5};
        std::cout << "Array создан на heap через std::vector\n";
        std::cout << "Размер: " << arr.size() << "\n";

        int result = arr.size();

        std::cout << "Выход из функции - деструктор vector освободит память\n";
        return result;
    }

    // fn process_arrays() -> i32 =
    //   let arr1 = [1, 2, 3];
    //   let arr2 = [4, 5, 6];
    //   for x in arr1 do x;
    //   for y in arr2 do y;
    //   arr1.length() + arr2.length()
    int process_arrays() {
        std::cout << "\n=== AURORA: process_arrays() ===\n";
        std::vector<int> arr1 = std::vector<int>{1, 2, 3};
        std::vector<int> arr2 = std::vector<int>{4, 5, 6};

        std::cout << "Два массива созданы\n";

        // for x in arr1 do x
        for (int x : arr1) {
            // x; (просто используем)
        }

        // for y in arr2 do y
        for (int y : arr2) {
            // y;
        }

        int result = arr1.size() + arr2.size();

        std::cout << "Выход из функции:\n";
        std::cout << "  1. arr2 уничтожается\n";
        std::cout << "  2. arr1 уничтожается\n";
        std::cout << "  3. Вся память освобождена\n";

        return result;
    }
}

int main() {
    std::cout << "╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║  ДЕМОНСТРАЦИЯ УПРАВЛЕНИЯ ПАМЯТЬЮ В AURORA             ║\n";
    std::cout << "║  (через std::vector и RAII)                           ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";

    demo_basic_lifetime();
    demo_push_reallocation();
    demo_copy_vs_move();
    demo_nested_scopes();

    std::cout << "\n" << "═" * 60 << "\n";
    Aurora::create_array();
    Aurora::process_arrays();

    std::cout << "\n╔════════════════════════════════════════════════════════╗\n";
    std::cout << "║  ИТОГ                                                  ║\n";
    std::cout << "╚════════════════════════════════════════════════════════╝\n";
    std::cout << "\n✅ Все объекты автоматически уничтожены\n";
    std::cout << "✅ Вся память освобождена\n";
    std::cout << "✅ Никаких утечек памяти\n";
    std::cout << "✅ Никакого ручного управления\n";
    std::cout << "✅ Детерминированное время освобождения\n";
    std::cout << "\n💡 Это и есть RAII - ресурсы освобождаются автоматически!\n\n";

    return 0;
}
