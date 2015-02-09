#/bin/bash

# check for xdotool
xdotool --help >/dev/null 2>&1 || { echo "ERROR: cannot start tests because xdotool doesn't work"; exit 1; }


sleep 5

while read p
do
  xdotool type --delay 20 "$p
"
done <<EOF
clear
ll
echo "Some simple text" > a
ll
bin/cp a b
ll
bin/cp a b # doesn't work because it already exists
touch c # an empty file
ll
bin/cp c d # copy empty to empty
diff a b # same
diff c d # same
ll
bin/cp /usr/share/dict/linux.words wrd --test
# most efficient method:
bin/cp /usr/share/dict/linux.words e
diff wrd /usr/share/dict/linux.words
diff e /usr/share/dict/linux.words
ll
EOF

