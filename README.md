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
mosquitto_pub -t '/devices/knx/controls/data/on' -m "${DstAddr} ${ACPI} ${Data}"
```
Чтобы отправить индивидуальную телеграмму в KNX необходимо отправить:
```
mosquitto_pub -t '/devices/knx/controls/data/on' -m "${SrcAddr}/${DstAddr} ${ACPI} ${Data}"
```

Все сообщения из KNX будут доставлены в MQTT топик `/devices/knx/controls/data` в виде:
```
${SrcAddr} ${DstAddr} ${ACPI} ${Data}
```

* `SrcAddr`, `DstAddr` - адреса KNX устройств, групповые в формате "n/n/n" или "n/n", индивидуальные в формате "n.n.n". `SrcAddr` всегда является индивидуальным.
* `ACPI` - 4-x битный тип сообщения[1]
* `Data` - сообщение

Пример MQTT лога:
```
$ mosquitto_pub -t '/devices/knx/controls/data/on' -m "0.0.1/9.7.55 4 message"
/devices/knx/controls/data/on 0.0.1/9.7.55 4 message
/devices/knx/controls/data 0.0.1 9.7.55 4 message
$ mosquitto_pub -t '/devices/knx/controls/data/on' -m "9/7/55 4 message"
/devices/knx/controls/data/on 9/7/55 4 message
/devices/knx/controls/data 0.0.0 9/7/55 4 message
```

[1] [KNX Protocol](http://www.knx.org/fileadmin/template/documents/downloads_support_menu/KNX_tutor_seminar_page/tutor_documentation/05_Serial%20Data%20Transmission_E0808f.pdf) pp20,28
