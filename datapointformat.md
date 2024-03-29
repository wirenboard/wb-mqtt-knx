# Преобразование датапоинтов в топики

## Топики на остнове типа DataPoint

Топик для приёма и отправки DataPoint типов содержащих только одно поле данных:

`/devices/<knx_device_id>/controls/<knx_device_control_id>`

`/devices/<knx_device_id>/controls/<knx_device_control_id>/on`

Топик для приёма и отправки DataPoint типов содержащих несколько полей данных:

`/devices/<knx_device_id>/controls/<knx_device_control_id>_<datapoint_field>`

`/devices/<knx_device_id>/controls/<knx_device_control_id>_<datapoint_field>/on`

где:
* **knx_device_id** - имя виртуальной группы групповых объектов;
* **knx_device_control_id** - имя группового объекта;
* **datapoint_field** - имя поля из DataPoint типа.

Если у DataPoint типа присутствует несколько полей, то на каждое поле создаётся свой контрол. 

## Представление DataPoint типов

### Raw_Value

С помошью этого типа точки данных возможно принимать и оправлять массив байт по заданному групповому адресу. 

| datapoint_field | type | range |
| ---- | ---- | ----|
| - | text | - |

Пример принятых данных:   `0x00 0x01 0x03 0x50`; `0x01`.    
Пример данных для оправки: `0x00 34 0x34 0b11011`; `0x00`.

*Примечание:* В посылке всегда передаётся и принимается первый байт KNX пейлоада, как в случае короткой 
так и в случае длинной посылки.

### 1.xxx_B1

| datapoint_field | type | range |
| ---- | ---- | ----|
| b | switch | 0, 1 |

### 2.xxx_B2

| datapoint_field | type | range |
| ---- | ---- | ----|
| c | switch | 0, 1 |
| v | switch | 0, 1 |

### 3.xxx_B1U3

| datapoint_field | type   | range   |
|-----------------|--------|---------|
| c               | switch | 0, 1    |
| StepCode        | value  | 0 ... 7 |

### 4.xxx_Character_Set

| datapoint_field | type | range |
|-------------|------|---|
| Character | text | - |

### 5.xxx_8-Bit_Unsigned_Value

| datapoint_field | type | range |
| ---- | ---- | ----|
| UnsignedValue | value | 0 ... 255 |

### 6.xxx_V8

| datapoint_field | type | range |
| ---- | ---- | ----|
| RelSignedValue | value | -128 ... 127 |

### 7.xxx_2-Octet_Unsigned_Value

| datapoint_field | type | range |
| ---- | ---- | ----|
| UnsignedValue | value | 0 ... 65535 |

### 8.xxx_2-Octet_Signed_Value

| datapoint_field | type | range              |
|-----------------| ---- |--------------------|
| SignedValue     | value | -32 768 ... 32 767 |

### 9.xxx_2-Octet_Float_Value

| datapoint_field | type | range |
| ---- | ---- | ----|
| FloatValue | value | -671 088,64 ... 670 760,96 |

### 12.001_4-Octet_Unsigned_Value

| datapoint_field | type | range |
| ---- | ---- | ----|
| UnsignedValue | value | 0 ... 4 294 967 295 |

### 13.xxx_4-Octet_Signed_Value

| datapoint_field | type | range                             |
|-----------------| ---- |-----------------------------------|
| SignedValue     | value | -2 147 483 648  ... 2 147 483 647 |

### 14.xxx_4-Octet_Float_Value

| datapoint_field | type | range                         |
| ---- | ---- |-------------------------------|
| FloatValue | value | IEEE754::MIN ... IEEE754::MAX |

### 20.xxx_N8

| datapoint_field | type | range |
|-----------------| ---- | ----|
| field1          | value | 0 ... 255 |

## Информация по датапоинтам
<https://www.knx.org/wAssets/docs/downloads/Certification/Interworking-Datapoint-types/03_07_02-Datapoint-Types-v02.02.01-AS.pdf>