KNX to MQTT gateway (C++)
=========================

Зависимости
-----------

* libwbmqtt0 (>= 1.4)
* knxd-tools (>= 0.11.14)
* knxd (>= 0.11.14)

Использование
-------------

Пересылает телеграммы из MQTT в KNX и обратно.

Чтобы отправить групповую телеграмму в KNX необходимо отправить подготовленное сообщение в MQTT топик:
```
mosquitto_pub -t '/devices/knx/controls/data/on' -m "g:${DstAddr} ${APCI} ${Data}"
```
Чтобы отправить индивидуальную телеграмму в KNX необходимо отправить:
```
mosquitto_pub -t '/devices/knx/controls/data/on' -m "i:${SrcAddr}:${DstAddr} ${APCI} ${Data}"
```

Все сообщения из KNX будут доставлены в MQTT топик `/devices/knx/controls/data` в виде:
```
i:${SrcAddr} [i,g]:${DstAddr} ${APCI} ${Data}
```

* `SrcAddr`, `DstAddr` - Адреса KNX устройств в формате "n/n/n" или "n/n".
* `APCI` - Тип сообщения, строка или 4х-битное число.
* `Data` - Сообщение в виде байт, разделенных пробелами. Первый байт сообщения должен иметь длину не более 6 бит.

Поддерживаемые типы сообщений(`APCI`):
* `GroupValueRead`
* `GroupValueResponse`
* `GroupValueWrite`
* `IndividualAddrWrite`
* `IndividualAddrRequest`
* `IndividualAddrResponse`
* `AdcRead`
* `AdcResponse`
* `MemoryRead`
* `MemoryRead`
* `MemoryWrite`
* `UserMessage`
* `MaskVersionRead`
* `MaskVersionResponse`
* `Restart`
* `Escape`
* любое 4х-битное числовое значение

Пример MQTT лога:
```
$ mosquitto_pub -t '/devices/knx/controls/data/on' -m "i:0/0/1:9/7/55 GroupValueWrite 0b110111 0xcf 14 0xff"

/devices/knx/controls/data/on i:0/0/1:9/7/55 GroupValueWrite 0b110111 0xcf 14 0xff
/devices/knx/controls/data i:0/0/1 i:9/7/55 GroupValueWrite 0x37 0xcf 0x0e 0xff

$ mosquitto_pub -t '/devices/knx/controls/data/on' -m "g:9/7/55 GroupValueRead"

/devices/knx/controls/data/on g:9/7/55 GroupValueRead
/devices/knx/controls/data i:0/0/0 g:9/7/55 GroupValueRead 0x00
```

[1] [KNX Protocol](http://www.knx.org/fileadmin/template/documents/downloads_support_menu/KNX_tutor_seminar_page/tutor_documentation/05_Serial%20Data%20Transmission_E0808f.pdf) pp20,28
