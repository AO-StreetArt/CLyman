..  http:example:: curl
    :response: property_create_response.rst

    POST /v1/property/{key}/action/{name} HTTP/1.1
    Host: localhost:8768
    Content-Type: application/json

    {
      "name": "testAction",
      "description": "this is another description"
    }
