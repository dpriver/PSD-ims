#!/bin/bash

echo "Creating database"
cd script/sql
mysql -u "psd-server" -p < "bd-test.sql"

echo "Finished"

