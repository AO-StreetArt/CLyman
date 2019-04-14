# Script which sends an Object Create message to CLyman,
# then injects the new key into the udp_message.json file

import sys
import requests

# Send a post request to Crazy Ivan, return the key of the scene from response
def send_http_request(url, postData):
    r = requests.post(url, data=postData, auth=('clyman', 'clymanAdminPassword'))
    response_json = r.json()
    return response_json['objects'][0]['key']

def execute_main():
    create_file = "obj_crt.json"
    udp_file = "udp_message.json"
    udp_out_file = "udp_message_upd.json"
    create_url = "http://localhost:8768/v1/object"
    new_object_key = ""
    udp_message = ""

    print("Sending HTTP Request -- Python")

    # Actually send the HTTP Request
    with open(create_file) as c_file:
        create_message = c_file.read()
        new_object_key = send_http_request(create_url, create_message)
        print("Object Key: %s" % new_object_key)

    # Build UDP Message
    with open(udp_file, "r") as u_file:
        udp_message = u_file.read()
        udp_message = udp_message.replace("_OBJ_KEY_", "%s" % new_object_key)

    with open(udp_out_file, "w") as w_file:
        w_file.write(udp_message)

if __name__ == "__main__":
    execute_main()
