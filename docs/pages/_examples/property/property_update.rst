..  http:example:: curl wget httpie python-requests
    :response: property_update_response.rst

    POST /v1/property/{key} HTTP/1.1
    Host: localhost:8768
    Content-Type: application/json

    {
      "properties":[
        {
    		  "name":"anotherName",
        	"parent":"anotherParent"
    	  }
      ]
    }
