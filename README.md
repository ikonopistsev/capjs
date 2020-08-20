![linux](https://github.com/ikonopistsev/capjs/workflows/linux/badge.svg)

# capjs

Mysql UDF библиотека генерации JSON объектов через SQL зарпросы

## jsobj 

формирование json объекта

> Название свойства формируется по имени кортежа
> !Название свойства не экранируется!
> Результат выдается блобом, в примерах используется конвертация cast as char

### Примеры

```
select cast(
    jsobj(1234 as 'value')
as char) as 'result';

# result
{"value":1234}
```

Преобразования производятся добавлением окончания .[e] где e тип необходимого преобразования. Это работает для методов jsobj и jsarr. Известные окончания удаляются при выдаче. Каждый тип имеет собственные преобразования. Всего mysql различает 5 типов, плагин работает с 4мя.

### Целые

* .m - сделно для времени, результат умножается на 1000
```
        select cast(
            jsobj(UNIX_TIMESTAMP(NOW()) as 'time.m')
        as char) as 'result';

        # result
        {"time":1536475506000}
```

* .t - конвертация миллисекунд в iso дату
```
        select cast(
            jsobj(cast(UNIX_TIMESTAMP(NOW(4)) * 1000 as unsigned) as 'time.t')
        as char) as 'result';
        # result
        {"time":"2018-09-09T06:48:15.023Z"}
```

* .u - выдача unixtime как iso даты
```
        select cast(
            jsobj(UNIX_TIMESTAMP(NOW()) as 'time.u')
        as char) as 'result';
        # result
        {"time":"2018-09-09T06:50:21.000Z"}
```

### Вещественные (!но не decimal)

* .0 - .9 - количество знаков после запятой
* .a - 10 знаков после запятой
* .b - 11 знаков после запятой
* .c - 12 знаков после запятой
* .d - 13 знаков после запятой
* .e - 14 знаков после запятой
* .f - 15 знаков после запятой
* .s - преобразование через std::to_string

### Строки и decimal

* .f - выдать вещественное число (mysql выдает тип decimal строкой)
```
        select cast(
            jsobj(123.456789999 as 'value.f')
        as char) as 'result';
        # result
        {"value":123.456789999}
```

* .s - тоже что и .f только через std::to_string
```
        select cast(
            jsobj(123.456789999 as 'value.s')
        as char) as 'result';
        # result
        {"value":123.456790}
```

* .r - отключить алгоритм экранирования строки
* .x - отключить экранирование и добавление символов \" в начале и конце строки
> предназначено для создания сложных json объектов
```
        select cast(
            jsobj("bob" as 'name',
                jsobj(123.456789999 as 'value.s') as 'salary.x')
        as char) as 'result';
        # result
        {"name":"bob","salary":{"value":123.456790}}
```

## jsarr 

формирование массива json

> преобразования аналогичны jsobj 

## jsd 

конвертация миллисекунд в iso дату

## mkkv 

сформировать key=value

```
    select cast(
        mkkv("bob" as 'name')
    as char) as 'result';
    # result
    name=bob
```

## Сборка

```
git clone --recurse-submodules https://github.com/ikonopistsev/capjs.git
run ./build.sh
```

## Установка

```
CREATE FUNCTION jsobj RETURNS STRING SONAME 'libcapjs.so';
CREATE FUNCTION jsarr RETURNS STRING SONAME 'libcapjs.so';
CREATE FUNCTION jsd RETURNS STRING SONAME 'libcapjs.so';
CREATE FUNCTION jst RETURNS INTEGER SONAME 'libcapjs.so';
CREATE FUNCTION mkkv RETURNS STRING SONAME 'libcapjs.so';
```
