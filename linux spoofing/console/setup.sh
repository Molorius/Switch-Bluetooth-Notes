#!/usr/bin/env bash

echo "Setting bluetooth name"

hciconfig hci0 name 'Nintendo Switch'
hciconfig hci0 down
hciconfig hci0 up