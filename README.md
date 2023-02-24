# nRF24-Batch
Flipper Zero application for nRF24L01 external board. Send batch commands. (TESTING...)

<b>Приложение для Flipper Zero, предназначено для отправки пакетных команд на удаленные устройства, использующие nRF24L01.</b><br>
Можно использовать для настройки или чтения данных с удаленного устройства. На удаленной стороне для команды чтения требуется поддержка.<br>
Два вида команд: запрос-ответ и запись.<br>
Запрос-ответ - отправка пакета, переключение на прием и отображение на экране, что получили.<br>
Запись - фактически отправка пакетов подряд с нужными данными.<br>
<br>
Описание команд в текстовом файле.<br>
<br>
Формат пакета (payload) в виде размера полей структуры в байтах задается, например, так "Payload struct: 2,1,1",<br>
что означает структуру из 3 полей: 2 байта, 1 байт, 1 байт.<br>
Перед отправкой пакета, он заполняется сначала шаблону по умолчанию: "R default" - запрос чтения, "W default" - запись.<br>
Можно использовать константы, они указываются в формате "имя=число", число либо десятичное или шестнадцатеричное с префиксом 0x.<br>
Затем берутся заполненные значения из самой команды.<br>
Пакет состоит из списка команд, перечисленных через ";".<br>
<br>
Пример файл [CO2_mini](https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Distr/nrf24batch/CO2_mini.txt)<br>
<br>
<br>
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-1.png">
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-2.png">
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-3.png">
<img src="https://raw.githubusercontent.com/vad7/nRF24-Batch/main/Pics/Screenshot-4.png">
