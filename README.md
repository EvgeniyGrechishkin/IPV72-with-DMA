# Управление индикаторами ИПВ с помощъю DMA микроконтроллера



## Описание тестовой схемы
Проект тестировался на железе, схема которого приведена в scheme.pdf.
Индикаторы соеденены последовательно по 3 штуки в строке. Всего 4 строки.
Управление линиями данных и стробом происходит напрямую от порта микроконтроллера.
Управление столбцами осуществлятся через усилители D1-D5, управляемые дишефратором D6.
D6 служит также для преобразования уровней 3.3V -> 5V.
Напряжение питание индкаторов (VCC_IND) 4V создается с помощъю диодов VD1, VD2.

## Описание программной части
DMA осуществляет отправку слов из массива.
Младший бит каждого отправляемого слова продставляет собой строб.
Далее, следуют биты данных для строк индикаторов. Количество бит данных равно количеству строк индикаторов.
Массив состоит из пар слов, которые отличаются только младшим битом (сначала слово с младшим битом, равным 1, затем 0).
Таким образом, создается спадающий фронт на выводе C индикаторов и происходит запись. Биты данных при этом не изменяются.
После окончания передачи (прервывание DMA) происходит засвет нужного столбца и запускается таймер, определяющий яркость свечения.
После этого, происходит запуск таймера, определяющего время гашения (чтобы избежать засветки). Далее, запускается новый цикл для следующего столбца.

>Проект создан в IDE Keil v5.