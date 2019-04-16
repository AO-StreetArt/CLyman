..  http:example:: curl

    POST /v1/object/{key}/action HTTP/1.1
    Host: localhost:8768
    Content-Type: application/json

		{
			"name": "testAction",
			"description": "this is a description",
			"keyframes": [
				{
					"frame": 0,
					"transform": [1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0],
					"translation_handle":[
						{"left_type":"test","left_x":0.0,"left_y":0.0,"right_type":"","right_x":0.0,"right_y":0.0},
						{"left_type":"","left_x":0.0,"left_y":0.0,"right_type":"","right_x":0.0,"right_y":0.0},
						{"left_type":"","left_x":0.0,"left_y":0.0,"right_type":"","right_x":0.0,"right_y":0.0}
					],
					"rotation_handle":[
						{"left_type":"test","left_x":0.0,"left_y":0.0,"right_type":"","right_x":0.0,"right_y":0.0},
						{"left_type":"","left_x":0.0,"left_y":0.0,"right_type":"","right_x":0.0,"right_y":0.0},
						{"left_type":"","left_x":0.0,"left_y":0.0,"right_type":"","right_x":0.0,"right_y":0.0},
						{"left_type":"","left_x":0.0,"left_y":0.0,"right_type":"","right_x":0.0,"right_y":0.0}
					],
					"scale_handle":[
						{"left_type":"test","left_x":0.0,"left_y":0.0,"right_type":"","right_x":0.0,"right_y":0.0},
						{"left_type":"","left_x":0.0,"left_y":0.0,"right_type":"","right_x":0.0,"right_y":0.0},
						{"left_type":"","left_x":0.0,"left_y":0.0,"right_type":"","right_x":0.0,"right_y":0.0}
					]
				}
			]
		}
