..  http:example:: curl wget httpie python-requests
    :response: object_query_response.rst

    POST /v1/query/object HTTP/1.1
    Host: localhost:5885
    Content-Type: application/json

    {
      "objects":[
        {
          "name":"test",
          "assets":["TestAsset10"]
        }
      ]
    }
