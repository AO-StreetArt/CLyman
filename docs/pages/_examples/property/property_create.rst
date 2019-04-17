..  http:example:: curl
    :response: property_create_response.rst

    POST /v1/property HTTP/1.1
    Host: localhost:8768
    Content-Type: application/json

    {
      "properties":[
        {
        	"key":"12345",
        	"name":"testName",
        	"parent":"testParent",
        	"asset_sub_id":"testAssetSubId",
        	"scene":"testScene",
        	"actions": [
    			{
    				"name": "testAction",
    				"description": "this is a description",
    				"keyframes": [
    					{
    						"frame":1,
    						"values":[
    				    		{
    				    			"value":100.0,
    				    			"left_type":"vector",
    				    			"left_x":10.0,
    				    			"left_y":5.0,
    				    			"right_type":"free",
    				    			"right_x":4.0,
    				    			"right_y":3.0

    				    		}
    				    	]
    					}
    				]
    			}
    		],
        	"values":[
        		{
        			"value":100.0,
        			"left_type":"vector",
        			"left_x":10.0,
        			"left_y":5.0,
        			"right_type":"free",
        			"right_x":4.0,
        			"right_y":3.0

        		}
        	]
        }
      ]
    }
