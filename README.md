KNX to MQTT gateway (C++)
=========================

Зависимости
-----------

* libwbmqtt0 (>= 1.4.1)
* libeibclient

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

* `SrcAddr`, `DstAddr` - адреса KNX устройств в формате "n/n/n" или "n/n".
* `APCI` - 4-x битный тип сообщения[1]
* `Data` - сообщение

Пример MQTT лога:
```
$ mosquitto_pub -t '/devices/knx/controls/data/on' -m "i:0/0/1:9/7/55 4 message"
/devices/knx/controls/data/on i:0/0/1:9/7/55 4 message
/devices/knx/controls/data i:0/0/1 i:9/7/55 4 message
$ mosquitto_pub -t '/devices/knx/controls/data/on' -m "g:9/7/55 4 message"
/devices/knx/controls/data/on g:9/7/55 4 message
/devices/knx/controls/data i:0/0/0 g:9/7/55 4 message
```

[1] [KNX Protocol](http://www.knx.org/fileadmin/template/documents/downloads_support_menu/KNX_tutor_seminar_page/tutor_documentation/05_Serial%20Data%20Transmission_E0808f.pdf) pp20,28
