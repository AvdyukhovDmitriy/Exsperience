# Что   здесь

В этом репозитории находиться код, который я написал в процессе своего основного обучения и последующего изучения языка C++

В папке `patterns/src` описаны в коде основные паттерны ООП.

В папке `serialization_transport_catalogue` находиться программа имитрующая создание базы транспортного каталога ее последующая сериализация
	и десериализация при помощи Protocol Buffers.
	
В папке `vector_and_optional` реализация контейнера Vector по примеру std::vector, а так же class Optional по примеру std::optional

# Для запуска
```sh
mkdir build
cd build
cmake ..
```
# Сборка
В папке `build` выполнить команду
```sh
cmake --build .
```
# Запуск
В папке `build` выполнить команду
```sh
./patterns 
```