KNX to MQTT gateway (C++)
=========================

Зависимости
-----------

* libwbmqtt1-3 (>= 3.2.0)
* knxd-tools (>= 0.14.51-1)
* knxd (>= 0.14.51-1)

## KNX gateway

Пересылает телеграммы из MQTT в KNX и обратно.

Чтобы отправить групповую телеграмму в KNX необходимо отправить подготовленное сообщение в MQTT топик:
```
mosquitto_pub -t '/devices/knx/controls/data/on' -m "g:${DstAddr} ${APCI} ${Data}"
```

Отправка телеграм с индивидуальным адресом получателя не поддерживается.

Все сообщения из KNX будут доставлены в MQTT топик `/devices/knx/controls/data` в виде:
```
i:${SrcAddr} [i,g]:${DstAddr} ${APCI} ${Data}
```

* `SrcAddr`, `DstAddr` - Адреса KNX устройств в формате "n/n/n" или "n/n".
* `APCI` - Тип сообщения, строка или 4х-битное число.
* `Data` - Сообщение в виде байт, разделенных пробелами. Первый байт сообщения должен иметь длину не более 6 бит.

При отправке сообщений для полей `ACPI` и `Data` допускаются форматы: `0xAA`, `0XAA`, `0b10101010`, `0B10101010`, `170`  

Поддерживаемые типы сообщений(`APCI`) при приёме телеграмм с индивидуальным и групповым адресом:
* `GroupValueRead`
* `GroupValueResponse`
* `GroupValueWrite`
* `IndividualAddrWrite`
* `IndividualAddrRequest`
* `IndividualAddrResponse`
* `AdcRead`
* `AdcResponse`
* `MemoryRead`
* `MemoryResponse`
* `MemoryWrite`
* `UserMessage`
* `MaskVersionRead`
* `MaskVersionResponse`
* `Restart`
* `Escape`
* любое 4х-битное числовое значение

Поддерживаемые типы сообщений(`APCI`) при отправке телеграмм c групповым адресом получателя:
* `GroupValueRead`
* `GroupValueResponse`
* `GroupValueWrite`

Пример MQTT лога:
```
$ mosquitto_pub -t '/devices/knx/controls/data/on' -m "g:9/7/55 GroupValueRead"

/devices/knx/controls/data/on g:9/7/55 GroupValueRead
/devices/knx/controls/data i:0/0/0 g:9/7/55 GroupValueRead 0x00

$ mosquitto_pub -t '/devices/knx/controls/data/on' -m "g:9/7/55 GroupValueWrite 0x04"

/devices/knx/controls/data/on g:9/7/55 GroupValueWrite 0x04
/devices/knx/controls/data i:1/1/22 g:9/7/55 GroupValueWrite 0x04

$ mosquitto_pub -t '/devices/knx/controls/data/on' -m "g:9/7/55 GroupValueWrite 0x00 0xAA 0xBB"

/devices/knx/controls/data/on g:9/7/55 GroupValueWrite 0x00 0xAA 0xBB
/devices/knx/controls/data i:1/1/23 g:9/7/55 GroupValueWrite 0xaa 0xbb
```

## Работа с Групповыми объектами KNX

Точкой данных в KNX называется "Групповой объект" (Group Object) или сокращенно "Объект".
Каждое устройство KNX содержит как минимум одну точку данных (Data Point).
Групповой объект представляет собой, например, реле исполнительного механизма переключателя. [[1]](https://support.knx.org/hc/en-us/articles/115003185345-Devices-objects) 

К групповому объекту привязан групповой адрес. Каждый групповой объект имеет определённый логический тип точки данных.
Каждый логический тип точки данных содержит как минимум одно поле данных.
Для каждого поля данных в MQTT создаётся контрол c соответствующим типом (switch, value, text).
Групповые адреса по своему усмотрению группируются в логические устройства MQTT. 

Все поддерживаемые типы датапоинтов описаны в:
<https://github.com/wirenboard/wb-mqtt-knx/blob/master/datapointformat.md>