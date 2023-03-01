# nRF24-Batch
Flipper Zero application for nRF24L01 external board. Sends batch commands. (IN DEVELOP...)

<b>Приложение для Flipper Zero, предназначено для отправки пакетных команд на удаленные устройства, использующие радио nRF24L01.</b><br><br>
Можно использовать для настройки или чтения данных с удаленного устройства. На удаленной стороне для команды чтения требуется поддержка.<br>
Сначала выбирается файл настройки с описанием команд.  
Затем стрелками влево или вправо выбирается нужный режим - Пакетное чтение (Read Batch), Чтение по одной команде (Read cmd), Пакетная запись (Write Batch).  
<br><br>
Есть два вида команд: запрос-ответ и запись.<br>
Запрос-ответ - отправка пакета, переключение на прием и отображение на экране, что получили.<br>
Запись - фактически отправка пакетов подряд с нужными данными.<br>
<br><br>
Формат пакета для отправки (payload) задается в виде размера полей структуры в байтах, например, так "Payload struct: 2,1,1", что означает структуру из 3 полей: 2 байта, 1 байт, 1 байт.<br>
Полученный в ответ пакет состоит из одного значения, размерность по умолчанию 1 байт (int8), при необходимости, задается числом после '*' после имени команды.<br><br>
Перед отправкой пакета, он заполняется сначала шаблону по умолчанию "R default" для запроса чтения, "W default" - для записи.<br>
Можно использовать константы по их имени, они задаются в формате "имя=число", число либо десятичное или шестнадцатеричное с префиксом 0x.<br>
Затем берутся заполненные значения полей из самой команды ("R:" или "W:").<br>
Если в конце строки с командой чтения символ '#', считанное значение будет показано в шестнадцатеричном виде.<br><br>
Пакет состоит из списка имен команд, перечисленных через ";".<br><br>
Отправка пакета для записи - длительно нажать Ok в списке и подтвердить.<br>
Перед пакетом команд для записи отправляется пакет 'Write start', если эта строка присутствует в файле настроек.<br><br>
Значение команды для записи можно редактировать - Ok на списке команд, стрелки - +/- и переход по цифрам, завершить - Назад, вставка цифры - Ok, удаление цифры - длительный Ok.<br>
<br>
Пример файл [CO2_mini](https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Distr/nrf24batch/CO2_mini.txt)<br>
Для устройства на Attiny44A, которое отправляет данные с датчика CO2 на контроллеры, управляющие вентиляцией или проветриватели: https://github.com/vad7/CO2-mini
<br><br>
Структура [файла настроек](https://raw.githubusercontent.com/vad7/nRF24-Batch/main/descript.txt):  

<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/descript.png">
<br>
<br>
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-1.png">
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-2.png">
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-3.png">
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-4.png">
<br>
<br>
Mini board for Flipper Zero to connect nRF24l01:  
  
<img src="https://raw.githubusercontent.com/vad7/nrf24scan/master/scheme2.png">
<br>
<a href="https://raw.githubusercontent.com/vad7/nrf24scan/master/Gerber_PCB_Flipper%20Zero%20nRF24%20board%20mini_v1_0.zip">Gerber</a> for mini board with 5V - 3.3V converter LM1117-3.3<br>
<a href="https://oshwlab.com/vad7/flipper-zero-nrf24-board_copy">Easyeda source</a>
