## Формат конфига для преобразования датапоинта в json пакет 

```json
[
  {
    "id": 1,
    "name": "B1",
    "encoding" : "B1",
    "field" : [
      {"name" : "b"}
    ],
    "datapoint" : [
      {
        "subid" : 1,
        "name" : "Switch",
        "field" : [
          {"name":  "switch"}
        ]
      },
      {
        "subid" : 2,
        "name" : "Bool",
        "field" : [
          {"name":  "bool"}
        ]
      }
    ]
  },
  {
    "id": 10,
    "name": "Time",
    "encoding": "N3U5r2U6r2U6",
    "field": [
      {
        "name": "Day"
      },
      {
        "name": "Hour"
      },
      {
        "name": "Minutes"
      },
      {
        "name": "Seconds"
      }
    ],
    "datapoint": [
      {
        "subid": 1,
        "name": "TimeOfDay",
        "field": [
          {
            "name": "Day"
          },
          {
            "name": "Hour"
          },
          {
            "name": "Minutes"
          },
          {
            "name": "Seconds"
          }
        ]
      }
    ]
  }
]
```



по которому драйвер сможет туда-обратно конвертировать сообщения из knx/более_лучшеая_data  в формате:

```json
{"Day":0,"Hour":22,"Minutes":1,"Seconds":2}
```