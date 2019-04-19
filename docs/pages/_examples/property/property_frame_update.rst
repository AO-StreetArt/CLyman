..  http:example:: curl
    :response: property_create_response.rst

    POST /v1/property/{key}/action/{name}/keyframe/{frame} HTTP/1.1
    Host: localhost:8768
    Content-Type: application/json

    {
      "frame": 1,
      "values": [
        {
          "value": 100,
          "left_type": "vector",
          "left_x": 10,
          "left_y": 5,
          "right_type": "free",
          "right_x": 4,
          "right_y": 3
        }
      ]
    }
