#!/bin/bash

cd ../profiles
rm -rf dkan
git clone --branch 7.x-1.x http://git.drupal.org/project/dkan.git
git checkout 7.x-1.3
cd dkan
rm -rf .git
rm .gitignore
sh dkan.rebuild.sh
