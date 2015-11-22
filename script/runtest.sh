#!/bin/bash

echo "Compiling"
#cd ../
#make clean
#make all
echo "Finished the compiling"

echo "Creating database"
cd script/sql
mysql -u "root" -p < "bd-test.sql"

echo "Populating database"

echo "Finished"

