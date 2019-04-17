..  http:example:: curl

    POST /v1/object/query HTTP/1.1
    Host: localhost:8768
    Content-Type: application/json

    {
      "objects":[
        {
          "name":"test",
          "assets":["TestAsset10"]
        }
      ]
    }
