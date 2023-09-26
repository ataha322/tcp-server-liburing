# Tcp сервер приема сообщений с использованием библиотеки liburing

0) При запуске процесс в argv[1] принимает номер прослушиваемого порта.
Открывает tcp сокет на указанном порту.
Открывает на запись текстовый файл <port_num>.txt

1) Получает на порт <port_num> сообщение от клиента (отправлять сообщение можно через telnet).
Макс. длина сообщения 128 байт.

2) сохраняет сообщение от клиента в файл, открытый в п.0

3) ожидание 3s

4) возвращает клиенту сообщение "ACCEPTED"

5) При появлении новых данных от клиента действия аналогично пп.2-4

6) При отключении по инициативе клиента сокет закрывается

## Условия:

1) Обработка должна быть конкурентной - запрос от одного клиента не блокирует запросы от других 
(2 одновременных запроса от 2 клиентов должны завершиться за ~3 секунд, а не за 6)

2) потоки использовать не нужно, нужно использовать liburing

3) для сборки использовать cmake

Результат выполнения - ссылка на репо в Gitlab\github