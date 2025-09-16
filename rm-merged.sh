#!/bin/bash

for b in `git branch --merged | grep -v '^\*'`
do
  b=$(echo $b | tr -d ' ')
  ( [ "main" != "`echo $b|cut -c1-4`" ] && \
    [ "develop" != "`echo $b|cut -c1-7`" ] ) \
    || continue
   read -p "remove $b ?(Y/n)" q
   [ "$q" != "Y" ] || git branch -d $b
done
