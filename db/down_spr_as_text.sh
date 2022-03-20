#!/bin/bash -u

# Make database script Unix version.
# Download the google spreadsheet in tsv format and save it as *.tmp.
# This script is called by the 'make' profess
#
# [Usage] $0
#
# 2021.4.19 / sutezo
#
####################

function usage() {
  echo '[Usage]'" $0"
  exit 1
}

if [ "$#" -ne 0 ]; then
  usage
fi

# book, url, sheet def.
### for public shared ### . SPREADSHEET.copy
. SPREADSHEET

function download_gspread() {
  for sheet_name in $* ;do
    GID=$( eval echo '$'$sheet_name ) # sheet id
    CNT=5
    while :; do
      C=$(( 5 - $CNT ))
      [ $C -ne 0 ] || echo "download ${sheet_name}"
      [ $C -eq 0 ] || echo "download ${sheet_name} (retry ${C} time.)"

      curl --retry 10 --silent --show-error --connect-timeout 20 --max-time 60 \
          -L -o ${sheet_name}.tmp \
      "${UrlBase}/d/${SpredsSheetsId}/export?format=tsv&gid=${GID}"
      ST=$?
      if [ $ST -eq 0 ] ; then
          grep -q 'Internal Server Error' ${sheet_name}.tmp
          if [ $? -eq 0 ]; then
              echo "Internal Server Error ${sheet_name}.tmp"
              exit -1
          else
              grep -qi '<html' ${sheet_name}.tmp
              [ $? -eq 0 ] || break
              sleep 3
              echo "busy retry..."
          fi
      else
          echo "Err rc=$ST"
      fi
      CNT=$(( $CNT - 1 ))
      [ $CNT != 0 ] || break
      sleep 4
    done
    if [ $CNT == 0 ]; then
       echo '**** Error retry out abortd!!'
       exit -1
    fi
  #  sleep 5
  done
}

# jrdb2014 = [ "lines(2014)", "clinfar(2014)", "rule69", "rule70_new", "rule86", "fare2" ]
# jrdb2015 = [ "lines2015", "clinfar (2015)", "rule69", "rule70_new", "rule86", "fare2" ]
# jrdb2017 = [ "lines2017", "clinfar (2015)", "rule69", "rule70_new", "rule86", "fare2" ]
# jrdb2019 = [ "lines2019", "clinfar (2019)", "rule69", "rule70_new", "rule86", "fare2", "BRT" ]
# dbfiles = [ jrdb2014, jrdb2015, jrdb2017, jrdb2019 ]
#dbfilename = [ "jrdb2014.txt", "jrdb2015.txt", "jrdb2017.txt", "jrdb2019.txt" ]
#
function mktext() {
    TARGET=$1
    shift

    cp /dev/null ${TARGET}

    for fn in $*
    do
        cat ${fn} >> ${TARGET}
        echo "" >> ${TARGET}
    done
}

download_gspread fare2 rule86 rule70_new rule69 clinfar2014 lines2014 clinfar2015 clinfar2019 lines2015 lines2017 lines2019 brt
mktext jrdb2014.txt lines2014.tmp clinfar2014.tmp rule69.tmp rule70_new.tmp rule86.tmp fare2.tmp
mktext jrdb2015.txt lines2015.tmp clinfar2015.tmp rule69.tmp rule70_new.tmp rule86.tmp fare2.tmp
mktext jrdb2017.txt lines2017.tmp clinfar2015.tmp rule69.tmp rule70_new.tmp rule86.tmp fare2.tmp
mktext jrdb2021.txt lines2019.tmp clinfar2019.tmp rule69.tmp rule70_new.tmp rule86.tmp fare2.tmp brt.tmp
exit 0
