..  http:example:: curl wget httpie python-requests
    :response: object_update_response.rst

    POST /v1/object/key HTTP/1.1
    Host: localhost:8768
    Content-Type: application/json

    {
      "msg_type": 0,
      "objects":[
        {
    		"name": "Test Object 123464",
    		"type": "Curve",
    		"subtype": "Sphere",
    		"frame": 0
    	}
      ]
    }
