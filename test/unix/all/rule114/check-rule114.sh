
OKFILE=rule114-succ-route.route
NGFILE=rule114-fail-route.route

:> $NGFILE
:> $OKFILE

c=0
while IFS= read -r line; do
    #echo "処理中: $line"
    c=$((c + 1))
    if ! echo $line | grep -qF '#' ; then
        ./farert -5 <(echo "$line") 2>/dev/null| grep -qF "規程114条"
        if [ "$?" -ne 0 ]; then
            echo $c
            echo "$line" >> $NGFILE
        else
            echo "$line" >> $OKFILE
        fi
    fi
done < "rule114.txt"
