#!/bin/bash
set -e

vault kv put secret/CLYMAN_PROD_MONGO_AUTH_UN CLYMAN_PROD_MONGO_AUTH_UN=root
vault kv put secret/CLYMAN_PROD_MONGO_AUTH_PW CLYMAN_PROD_MONGO_AUTH_PW=admin
# vault kv put secret/AOSSL_CONSUL_SSL_CERT AOSSL_CONSUL_SSL_CERT=test
# vault kv put secret/AOSSL_CONSUL_ACL_TOKEN AOSSL_CONSUL_ACL_TOKEN=test
