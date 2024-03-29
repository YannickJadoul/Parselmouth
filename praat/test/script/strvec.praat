writeInfoLine: "string vectors..."

a$ = Calculator: ~ { "hello", "goodbye" }
assert a$ = "hello" + newline$ + "goodbye" + newline$

assert { "" } = {""}

assert { "hello", "goodbye" } = { "hello", "goodbye" }
assert ({ "hello", "goodbye" } = { "hello", "goodbye" }) = 1

assert not { "hello", "goodbye" } <> { "hello", "goodbye" }
assert ({ "hello", "goodbye" } <> { "hello", "goodbye" }) = 0

writeInfoLine: { "hello", "goodbye" }

a$# = { "hello", "goodbye" }
a$# [1] = "hallo"

assert a$# [1] = "hallo"
assert a$# [2] = "goodbye"
b$# = a$#
writeInfoLine: "<<", a$#, ">>"

writeFileLine: "kanweg.txt", "<<", a$#, ">>"
text$ = readFile$ ("kanweg.txt")
assert text$ = "<<hallo goodbye>>" + newline$

lines$# = readLinesFromFile$# ("kanweg.txt")
assert lines$# [1] = "<<hallo goodbye>>"
assert size (lines$#) = 1

assert a$# = { "hallo", "goodbye" }

writeFileLine: "kanweg.txt", "hellø", newline$, "gøødbye"
lines$# = readLinesFromFile$# ("kanweg.txt")
assert size (lines$#) = 2
assert lines$# [1] = "hellø"
assert lines$# [2] = "gøødbye"
assert lines$# = { "hellø", "gøødbye" }

stopwatch
for i to 100
	Create Table with column names: "table", 10,
	... { "speaker", "dialect", "age", "vowel", "F0_Hz", "F1_Hz", "F2_Hz" }
	Remove
endfor
appendInfoLine: stopwatch
for i to 100
	Create Table with column names: "table", 10,
	... "speaker dialect age vowel F0_Hz F1_Hz F2_Hz"
	Remove
endfor
appendInfoLine: stopwatch
appendInfoLine: "OK"
