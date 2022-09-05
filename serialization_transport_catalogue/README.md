В данном разделе находяться папки с файлами необходимые для запуска программы сериализации и десериализации транспортного катталога.

В папке protobuf находиься собранный пакет protobuf достаточный для запуска программы на Windows.
Для запуска проекта под Ubutu  или Mac Вам будет необходимо установить Protobuf на Вашу систему.

Собиратся проект с Protobuf через CMake.

созадем папку build
 mkdir build
 cd build

в паке build запускаем CMake

cmake  /path/to/serialization_transport_catalogue -DCMAKE_PREFIX_PATH=/path/to/serialization_transport_catalogue/protobuf

если у вас есть собранная своя версия библиотеки Protobuf то

cmake  /path/to/serialization_transport_catalogue -DCMAKE_PREFIX_PATH=/path/to/your_protobuf

Чтобы запустить программу при помощи VS необходимо запустить файл Transport_Catalogue.sln.
После зайти в свойство проекта /Свойства конфигураци/ С/С++ / Создание кода. И установить флаг в Библиотека времени выполнения на "Многопоточная отладка(/MTd)
Далее собрать проект.
В папке Debug пояивться файл transport_catalogue.exe



В папке xamples находяться тестовые файлы. В файлах ..._make_base.json находяться данные необходимые для создания база транспортного каталога,
	 в которой оказываются остановки, маршруты и  время движения по маршрутам. В файлах ..._process_requests.json находяться запросы к базе 
	 транспортного каталога, а так же на построения карты каталога в формате XML.
	 
Запуск прогараммы осуществляется с флагами make_base и передачей 3 параметром файла ../../examples/..._make_base.json

$ ./transport_catalogue.exe make_base < ../../examples/s14_3_opentest_1_make_base.json

В этомслучае будет создана база транспортного каталога  которая будет сериализована с использованием Protobuf.

При втором запуске программы с флагом process_requests   и передачей 3 параметром файла ../../examples/..._process_requests.json

$ ./transport_catalogue.exe process_requests < ../../examples/s14_3_opentest_1_process_requests.json

Произойдет десериализация базы при помощи Protobuf. И будет выведен в консоль ответ на запрос находящийся в файле ..._process_requests.json

Если 4 параметром указать путь к файлу, то ответ на запрос запишеться в файл

$ ./transport_catalogue.exe process_requests < ../../examples/s14_3_opentest_1_process_requests.json >  ../../examples/answer.json

Так же в паке examples/ находяться файл с правильными ответами на соответствующие запросы к базам транспортного каталога
