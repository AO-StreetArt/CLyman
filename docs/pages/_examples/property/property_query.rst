..  http:example:: curl wget httpie python-requests
    :response: property_query_response.rst

    POST /v1/property/query HTTP/1.1
    Host: localhost:5885
    Content-Type: application/json

    {
      "properties":[
        {
          "name":"test"
        }
      ]
    }
