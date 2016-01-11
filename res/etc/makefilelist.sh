for SUBDIR in $(find . -name 'Makefile' | xargs dirname)
do
	if [ -f "$SUBDIR/CMakeLists.txt" ]
	then
		(cd $SUBDIR && meld CMakeLists.txt <(egrep -o '([[:graph:]]+\.o *)+' Makefile | sed -E 's/([[:graph:]]+)\.o/ls \1.c* | tr "\n" " " ;/ge' | sed '1!s/^/            /g' | sed 's/ $//' | sed '1s/^/add_sources(/' | sed '$s/$/)/'))
	fi
done
