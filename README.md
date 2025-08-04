# инструкция по проверке :

## 1) Драйвер нужно скомпилировать под свою систему с помощью предоставленного Makefile-а :
для этого в консоли переходим по пути репозитория и пишем в консоль команду
```linux
make
```
 После в папке  скомпилируются файлы драйвера. Нас интересует файл mydummy_driver.ko
## 2) Загружаем скомпилированный драйвер с помощью команды :
```linux
sudo insmod mydummydriver.ko
```
 Главное не забыть указать sudo, так как модуль нужно добавлять с root правами
## 3) Убеждаемся что драйвер был загружен : 
```linux
lsmod | grep mydummydriver
```
## 4) Проверяем создался ли файл драйвера : 
```linux
lsmod | grep mydummydriver
```
## 5) Далее проверяем чтение из драйвера :
```linux
sudo cat /dev/mydummy
```
 Должно вывести Helloworld!
## 6) Выгружаем драйвер :
```linux
sudo rmmod mydummydriver
```
## 7)  Далее проверяем журнал ядра
```linux
dmesg
```
 Ищем сообщения : 
- Trying to register my_dummy_device region
- Device region registered:
- my_dummy_device file created: /dev/mydummy
- my_dummy_device opened
- my_dummy_device closed
- my_dummy_device destroyed!

# Также присутствует удобная функция очистки всех скомпилированных файлов, в том числе mydummy_driver.ko.

в консоли переходим в место, где лежит репозиторий, и вводим команду :
```linux
make clean
```
Готово! все скомпилированные файлы уничтожены
