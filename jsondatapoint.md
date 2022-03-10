## Формат конфига для преобразования датапоинта в json пакет 

```json
[
  {
    "id": 10,
    "name": "DPT_TimeOfDay",
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
        "name": "DPT_TimeOfDaySpecial",
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