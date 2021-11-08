# Преобразование датапоинтов в топики

## Топики на остнове типа DataPoint

/devices/<knx_device_id>/controls/<knx_device_control_id>_<datapoint_field>

где:
* **knx_device_id** - имя группы групповых объектов (каламбур);
* **knx_device_control_id** - имя группового объекта;
* **datapoint_field** - имя поля из DataPoint типа.

Если у DataPoint типа присутствует несколько полей, то на каждое поле создаётся свой контрол. 


## Представление DataPiont типов

### 1.xxx_B1

| datapoint_field | type | range |
| ---- | ---- | ----|
| b | bool | 0, 1 |

### 2.xxx_B2

| datapoint_field | type | range |
| ---- | ---- | ----|
| c | bool | 0, 1 |
| v | bool | 0, 1 |


### 3.xxx_B1U3

| datapoint_field | type | range |
| ---- | ---- | ----|
| c | bool | 0, 1 |
| StepCode | uint |  000b ... 111b |

### 4.xxx_Character_Set

| datapoint_field | type | range |
| ---- | ---- | ----|
| Character | char |  -  |

### 5.xxx_8-Bit_Unsigned_Value

| datapoint_field | type | range |
| ---- | ---- | ----|
| UnsignedValue | uint | 0 ... 255 |

### 6.xxx_V8

| datapoint_field | type | range |
| ---- | ---- | ----|
| RelSignedValue | int | -128 ... 127 |

### 6.020_Status_with_Mode

| datapoint_field | type | range |
| ---- | ---- | ----|
| a | bool | 0,1 |
| b | bool | 0,1 |
| c | bool | 0,1 |
| d | bool | 0,1 |
| e | bool | 0,1 |
| f | enum | 001b, 010b, 100b |

### 7.xxx_2-Octet_Unsigned_Value

| datapoint_field | type | range |
| ---- | ---- | ----|
| UnsignedValue | uint | 0 ... 65 535 |

### 8.xxx_2-Octet_Signed_Value

| datapoint_field | type | range |
| ---- | ---- | ----|
| SignedValue | int | -32 768 ... 32 768 |

### 9.xxx_2-Octet_Float_Value

| datapoint_field | type | range |
| ---- | ---- | ----|
| FloatValue | float | -671 088,64 ... 670 760,96 |

### 10.001_Time

| datapoint_field | type | range |
| ---- | ---- | ----|
| Day | enum | 0 ... 7 |
| Hour | uint | 0 ... 23 |
| Minutes | uint | 0 ... 59 |
| Seconds | uint | 0 ... 59 |

### 11.001_Date

| datapoint_field | type | range |
| ---- | ---- | ----|
| Day | uint | 1 ... 31 |
| Month | uint | 1 ... 12 |
| Year | uint | 0 ... 99 |

### 12.001_4-Octet_Unsigned_Value

| datapoint_field | type | range |
| ---- | ---- | ----|
| UnsignedValue | uint | 0 ... 4 294 967 295 |

### 13.xxx_4-Octet_Signed_Value

| datapoint_field | type | range |
| ---- | ---- | ----|
| SignedValue | int | -2 147 483 648 ... 2 147 483 647 |

### X.XXX TODO

## Информация по датапоинтам
https://www.knx.org/wAssets/docs/downloads/Certification/Interworking-Datapoint-types/03_07_02-Datapoint-Types-v02.02.01-AS.pdf