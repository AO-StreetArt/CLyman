# The basic CLyman Flow performs a series of basic checks on the CLyman API
# It walks a single object through a full flow, step by step, and validates
# all of the fields on the object at each step of the way.

import zmq
import logging
import sys
import json

# Basic Config
log_file = 'basicClymanFlow.log'
log_level = logging.DEBUG

# The transforms we will track throughout the flow
# These get calculated by hand, and validate the transformations applied
# Current implementation applies LHS Matrix multiplication of translation,
# then euler rotation, then scale, in that order.
# Final Result can be found using http://matrix.reshish.com/multCalculation.php
# Translation Matrix:
#   For a translation <x, y, z>, the corresponding matrix is:
#       [1.0, 0.0, 0.0, x.0,
#        0.0, 1.0, 0.0, y.0,
#        0.0, 0.0, 1.0, z.0,
#        0.0, 0.0, 0.0, 1.0]
# Rotation Matrix:
#   For a rotation of theta degrees about the vector <x, y, z>, the rotation
#   matrix can be found using https://www.andre-gaschler.com/rotationconverter/
# Scale Matrix:
#   For a scale <x, y, z>, the corresponding matrix is:
#       [x.0, 0.0, 0.0, 0.0,
#        0.0, y.0, 0.0, 0.0,
#        0.0, 0.0, z.0, 0.0,
#        0.0, 0.0, 0.0, 1.0]
test_transform = [1.0, 0.0, 0.0, 0.0,
                  0.0, 1.0, 0.0, 0.0,
                  0.0, 0.0, 1.0, 0.0,
                  0.0, 0.0, 0.0, 1.0]

updated_test_transform = [2.0, 0.0, 0.0, 2.0,
                          0.0, 1.0806046, -1.682942, -0.6023374,
                          0.0, 1.682942, 1.0806046, 2.7635466,
                          0.0, 0.0, 0.0, 1.0]

# Object data represented through each piece of the flow
test_data = {
  "key": "",
  "name": "basicTestObject",
  "type": "basicTestType",
  "subtype": "basicTestSubtype",
  "owner": "basicTestOwner",
  "scene": "basicTestScene",
  "translation": [0.0, 0.0, 0.0],
  "euler_rotation": [0.0, 1.0, 0.0, 0.0],
  "scale": [1.0, 1.0, 1.0],
  "assets": ["basicTestAsset"]
}

updated_test_data = {
  "key": "",
  "name": "Replacement",
  "type": "Mesh",
  "subtype": "Cube",
  "owner": "Alex",
  "scene": "1234",
  "translation": [1.0, 1.0, 1.0],
  "euler_rotation": [1.0, 1.0, 0.0, 0.0],
  "scale": [2.0, 2.0, 2.0],
  "assets": ["anotherAsset"]
}

# Validation Methods
def parse_response(response):
    logging.debug("Parsing Response: %s" % response)
    parsed_json = None
    try:
        parsed_json = json.loads(response)
    except Exception as e:
        try:
            parsed_json = json.loads(response[1:])
        except Exception as e:
            logging.error('Unable to parse response')
            logging.error(e)
            logging.error(get_exception())
    return parsed_json

def validate_create_response(create_response):
    logging.debug("Validating Create Response")
    global test_data
    global test_transform
    logging.debug("Validating against: %s : %s" % (test_data, test_transform))
    parsed_json = parse_response(create_response)

    if parsed_json is not None:
        assert(parsed_json["err_code"] == 100)
        assert(parsed_json["num_records"] == 1)
        assert("key" in parsed_json["objects"][0]
               and "transform" in parsed_json["objects"][0])
        for i in range(0,16):
            assert(parsed_json["objects"][0]["transform"][i] - test_transform[i] < 0.01)
        test_data["key"] = parsed_json["objects"][0]["key"]
        logging.info("Key set: %s" % test_data["key"])

def validate_get_response(get_response, validation_data, validation_transform):
    logging.debug("Validating Get Response")
    logging.debug("Validating against: %s : %s" % (validation_data, validation_transform))
    parsed_json = parse_response(get_response)

    if parsed_json is not None:
        assert(parsed_json["err_code"] == 100)
        assert(parsed_json["num_records"] == 1)
        parsed_data = parsed_json["objects"][0]
        assert(parsed_data["key"] == validation_data["key"])
        assert(parsed_data["name"] == validation_data["name"])
        assert(parsed_data["type"] == validation_data["type"])
        assert(parsed_data["subtype"] == validation_data["subtype"])
        assert(parsed_data["owner"] == validation_data["owner"])
        assert(parsed_data["scene"] == validation_data["scene"])
        assert(parsed_data["assets"][0] == validation_data["assets"][0])
        for i in range(0,16):
            logging.debug("Validating Transform element: %s" % i)
            assert(parsed_data["transform"][i] - validation_transform[i] < 0.01)

def validate_update_response(update_response):
    logging.debug("Validating Update Response")
    global updated_test_data
    global updated_test_transform
    logging.debug("Validating against: %s : %s" % (updated_test_data, updated_test_transform))
    parsed_json = parse_response(update_response)

    if parsed_json is not None:
        assert(parsed_json["err_code"] == 100)
        assert(parsed_json["num_records"] == 1)
        parsed_data = parsed_json["objects"][0]
        assert(parsed_data["name"] == updated_test_data["name"])
        assert(parsed_data["type"] == updated_test_data["type"])
        assert(parsed_data["subtype"] == updated_test_data["subtype"])
        assert(parsed_data["owner"] == updated_test_data["owner"])
        assert(parsed_data["scene"] == updated_test_data["scene"])
        for i in range(0,16):
            logging.debug("Validating Transform element: %s" % i)
            assert(parsed_data["transform"][i] - updated_test_transform[i] < 0.01)

# Execute the actual tests
def execute_main(zmq_addr="tcp://localhost:5556"):
    # Grab the global pieces of data
    global test_data
    global test_transform
    global updated_test_data
    global updated_test_transform
    global log_file
    global log_level

    logging.basicConfig(filename=log_file, level=log_level)

    # Connect to ZeroMQ
    context = zmq.Context()
    context.setsockopt(zmq.RCVTIMEO, 10000)
    context.setsockopt(zmq.LINGER, 0)
    socket = context.socket(zmq.REQ)
    socket.connect(zmq_addr)
    logging.debug("Connected to ZMQ Socket")

    # Start with a create message
    logging.info("Create Test")
    create_data = {
      "msg_type": 0,
      "objects": [test_data]
    }
    create_message = json.dumps(create_data)
    socket.send_string(create_message + "\n")
    create_response = socket.recv_string()
    validate_create_response(create_response)

    # Next, perform a get message
    logging.info("Get Test")
    get_data = {
      "msg_type": 2,
      "objects": [{"key":test_data["key"]}]
    }
    get_message = json.dumps(get_data)
    socket.send_string(get_message + "\n")
    get_response = socket.recv_string()
    logging.debug("Get Response: %s" % get_response)
    validate_get_response(get_response, test_data, test_transform)

    # Follow up with an update message
    updated_test_data["key"] = test_data["key"]
    logging.info("Update Test")
    update_data = {
      "msg_type": 1,
      "objects": [updated_test_data]
    }
    update_message = json.dumps(update_data)
    socket.send_string(update_message + "\n")
    update_response = socket.recv_string()
    validate_update_response(update_response)

    # Validate the update by issuing a get request
    socket.send_string(get_message + "\n")
    get_response = socket.recv_string()
    logging.debug("Get Response: %s" % get_response)
    # Update the test data since the asset elements are appended
    updated_test_data['assets'].insert(0, test_data['assets'][0])
    validate_get_response(get_response, updated_test_data, updated_test_transform)

if __name__ == "__main__":
    if len(sys.argv) > 1:
        execute_main(sys.argv[1])
    else:
        execute_main()
