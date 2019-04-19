..  http:example:: curl

    POST /v1/object/{key}/action/{name} HTTP/1.1
    Host: localhost:8768
    Content-Type: application/json

		{
			"name": "testAction",
			"description": "this is another description"
		}
