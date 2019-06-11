#!/bin/bash

blot_version=`grep 'BLOT_VERSION_NUMBER' $1 | cut -d' ' -f3`
blot_commit="notcompiledfromgit"

command -v git > /dev/null 2>&1
if [ $? -eq 0 ]; then
	blot_commit="."`git rev-parse HEAD`
fi

sed "s/_version_commit_/${blot_version}${blot_commit}/g" $1 > $2


