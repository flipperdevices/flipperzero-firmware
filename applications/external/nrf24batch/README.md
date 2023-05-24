# nRF24-Batch
Flipper Zero application for nRF24L01 external board. Sends batch commands.

<b>Приложение для Flipper Zero, предназначено для отправки пакетных команд на удаленные устройства, использующие радио nRF24L01.</b><br><br>
Можно использовать для настройки или чтения данных с удаленного устройства. На удаленной стороне требуется поддержка, ссылка на код для микроконтроллера AVR ниже.<br>
<br>
Сначала выбирается файл настройки с описанием команд.  
Можно отредактировать адрес и номер канала.  
Затем стрелками влево или вправо выбирается нужный режим:  
- Пакетное чтение (Read Batch),  
- Чтение по одной команде (Read cmd),  
- Пакетная запись (Write Batch),  
- Пакетная отправка (Set),  
- Режим прослушивания адреса (Listen).  
<br>
Чтение по одной команде - отправка пакета, переключение на прием и отображение на экране, что получили. Длительное нажатие кнопки Ok включает режим автоматического повторного запроса через заданный интервал в секундах (настройка "ReadCmd repeat:").<br><br>
Пакетное чтение - выполнение подряд нескольких команд чтения.<br><br>
Пакетная запись - фактически отправка пакетов подряд с нужными данными. Предварительно отправляется пакет "Write start", если эта строка присутствует в файле настроек.<br><br>
Пакетная отправка - режим пульта, отправка пакетов сразу из списка без вопросов и без преварительного пакета "Write start".<br><br>
Режим прослушивания - если в файле присутствует строка "Listen: <адрес>=<поле 1>,<поле 2>,...", где "поле.." название поля. Адрес можно менять - долго нажать кнопку Ok.<br><br>
<br>
Формат пакета для отправки (payload) задается в виде размера полей структуры в байтах, например, так "Payload struct: 2,1,1", что означает структуру из 3 полей: 2 байта, 1 байт, 1 байт.<br>
Полученный в ответ пакет (такой же длины как и отправленный) состоит из одного значения (остаток пакета не используется), размерность по умолчанию 1 байт (int8), при необходимости, она задается числом после '*' после имени команды.<br><br>
Перед отправкой пакета, он заполняется сначала по шаблону по умолчанию "R default" для запроса чтения, "W default" - для записи.<br>
Поля разделены - ','. Для "W default" есть специальный маркер 'n', говорящий какое поле будет заполняться значением после '=' из команды "WBatch".<br>
Можно использовать константы по их имени, они задаются в файле в формате "имя=число", число либо десятичное или шестнадцатеричное с префиксом 0x.<br>
Затем берутся заполненные значения полей из самой команды ("R:" или "W:").<br>
Если в конце строки с командой чтения символ '#', то считанное значение будет показано в шестнадцатеричном виде.<br><br>
Пакеты чтения "RBatch:", записи "WBatch:", отправки "SBatch:" состоят из списка имен команд "R:" или "W:"("S:"), соответственно, перечисленных через ";".<br>
"S:" то же самое, что и "W:", только пакет "Write start" отправлен не будет<br><br>
Отправка пакета для записи - длительно нажать Ok в списке и подтвердить.<br>
Перед пакетом команд для записи отправляется пакет "Write start", если эта строка присутствует в файле настроек.<br><br>
Значение команды для записи можно редактировать - Ok на списке команд, стрелки - +/- и переход по цифрам, завершить - Назад, вставка цифры - Ok, удаление цифры - длительный Ok.<br>
Из списка команд в пакетах чтения и записи команды можно убирать - для этого нужно долго нажать на кнопку влево и подтвердить.<br>
Это нужно, например, чтобы убирать те команды в сохраненном пакете для чтения, для которых нет команды записи.<br>
<br>
Пример файл [CO2_mini](https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Distr/nrf24batch/CO2_mini.txt)<br>
Для устройства на Attiny44A, которое отправляет данные с датчика CO2 на контроллеры, управляющие вентиляцией или проветриватели: https://github.com/vad7/CO2-mini<br>
Управляет вентиляцией на кухне: https://github.com/vad7/Kitchen_Vent<br>
<br><br>
Структура [файла настроек](https://raw.githubusercontent.com/vad7/nRF24-Batch/main/descript.txt):  

<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/descript.png">
<br>
<br>
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-1.png">
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-2.png">
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-3.png">
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-4.png">
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-5.png">
<br>
<br>
Mini board for Flipper Zero to connect nRF24l01:<br><br>
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/NRF24_mini_3D.png">
<br>
<img src="https://raw.githubusercontent.com/vad7/nrf24scan/master/scheme2.png">
<br>
<a href="https://raw.githubusercontent.com/vad7/nrf24scan/master/Gerber_PCB_Flipper%20Zero%20nRF24%20board%20mini_v1_0.zip">Gerber</a> for mini board with 5V - 3.3V converter LM1117-3.3<br>
<a href="https://oshwlab.com/vad7/flipper-zero-nrf24-board_copy">Easyeda source</a>



TRANSLATION:
nRF24-Batch
Flipper Zero application for nRF24L01 external board. Sends batch commands.

An application for Flipper Zero designed to send batch commands to remote devices using the nRF24L01 radio.

Can be used to configure or read data from a remote device. Remote side support required, code link for AVR microcontroller below.

First, a configuration file with a description of the commands is selected.
You can edit the address and channel number.
Then use the left or right arrows to select the desired mode:

Batch reading (Read Batch),
Reading by one command (Read cmd),
Batch recording (Write Batch),
Batch send (Set),
Address listening mode (Listen).

Reading by one command - sending a packet, switching to receiving and displaying on the screen what was received. A long press of the Ok button turns on the automatic repeat request mode after a specified interval in seconds ("ReadCmd repeat:" setting).

Batch reading - execution of several read commands in a row.

Packet recording - actually sending packets in a row with the necessary data. A "Write start" packet is sent beforehand if this line is present in the settings file.

Batch sending - remote control mode, sending packets immediately from the list without questions and without the "Write start" pre-packet.

Listening mode - if the file contains the line "Listen: <address>=<field 1>,<field 2>,...", where "field.." is the name of the field.


The payload format is specified as the size of the structure fields in bytes, for example, "Payload struct: 2,1,1", which means a structure of 3 fields: 2 bytes, 1 byte, 1 byte.
The packet received in response (the same length as the one sent) consists of a single value (the rest of the packet is not used), the default dimension is 1 byte (int8), if necessary, it is given by a number after the '*' after the command name.

Before a packet is sent, it is filled first with the default pattern "R default" for a read request, "W default" for a write request.
The fields are separated - ','. For "W default" there is a special marker 'n' which tells which field will be filled with the value after the '=' from the "WBatch" command.
You can use constants by their name, they are specified in the file in the format "name=number", number or decimal or hexadecimal prefixed with 0x.
Then the filled field values ​​are taken from the command itself ("R:" or "W:").
If there is a '#' character at the end of the line with the read command, then the read value will be displayed in hexadecimal form.

Read "RBatch:", write "WBatch:", send "SBatch:" packets consist of a list of command names "R:" or "W:"("S:"), respectively, listed with ";".
"S:" is the same as "W:", only the "Write start" packet will not be sent.

Sending a packet for writing - long press Ok in the list and confirm.


The value of the command to be written can be edited - Ok on the list of commands, arrows - +/- and moving through the numbers, complete - Back, inserting a number - Ok, deleting a number - long Ok.
Commands can be removed from the list of commands in read and write packages - to do this, long press the button to the left and confirm.
This is necessary, for example, to remove those commands in the saved package for reading, for which there is no write command.

Example file [CO2_mini]( https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Distr/nrf24batch/CO2_mini.txt )
For an Attiny44A device that sends data from a CO2 sensor to controllers that control ventilation or ventilators : https://github.com/vad7/CO2-mini
Controls the ventilation in the kitchen:https://github.com/vad7/Kitchen_Vent