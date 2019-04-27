# Script which sends an Object Create message to CLyman,
# then injects the new key into the udp_message.json file

import sys
import requests

# Send a post request to Crazy Ivan, return the key of the scene from response
def send_http_request(url, postData, resp_path):
    r = requests.post(url, data=postData, auth=('clyman', 'clymanAdminPassword'))
    response_json = r.json()
    return response_json[resp_path][0]['key']

# Execute a Post request to create a base data object, then build a UDP message
def build_base_data_object(create_file, udp_file, udp_out_file, create_url, key_replacement_value, resp_path):
    new_object_key = ""
    udp_message = ""

    # Actually send the HTTP Request
    with open(create_file) as c_file:
        create_message = c_file.read()
        new_object_key = send_http_request(create_url, create_message, resp_path)
        print("Object Key: %s" % new_object_key)

    # Build UDP Message
    with open(udp_file, "r") as u_file:
        udp_message = u_file.read()
        udp_message = udp_message.replace(key_replacement_value, "%s" % new_object_key)

    with open(udp_out_file, "w") as w_file:
        w_file.write(udp_message)

# Create an Object and it's UDP Message
def build_object():
    print("Sending HTTP Request to create Object -- Python")
    build_base_data_object("obj_crt.json",
                           "obj_udp_message.json",
                           "obj_udp_message_upd.json",
                           "http://localhost:8768/v1/object",
                           "_OBJ_KEY_", 'objects')

# Create a Property and it's UDP Message
def build_property():
    print("Sending HTTP Request to create Property -- Python")
    build_base_data_object("prop_crt.json",
                           "prop_udp_message.json",
                           "prop_udp_message_upd.json",
                           "http://localhost:8768/v1/property",
                           "_PROP_KEY_", 'properties')

if __name__ == "__main__":
    build_object()
    build_property()
