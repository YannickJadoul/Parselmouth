writeInfoLine: "Testing runSubprocess..."
@test: empty$# (0)
@test: { "hello" }
@test: { “hello” }
@test: { """hello""" }
@test: { "a""hello""b" }
@test: { "a""""hello""""b" }
@test: { “he"llo"a” }
@test: { “he " llo"a” }
@test: { “he " llo” }
@test: { "" }
@test: { "", "" }
@test: { "hello", "" }
@test: { "", "hello" }
@test: { "hello world", "" }
@test: { "", "hello world" }
@test: { "hello world" }
@test: { "hello, world" }
@test: { "hello", "world" }
@test: { "hello goodbye", "world" }
@test: { "hello/goodbye", "world" }
@test: { "hello@goodbye", "world" }
@test: { "hello", "goodbye world" }
@test: { "hello all", "goodbye world" }
@test: { “hello goodbye”, "world" }
@test: { “hello" goodbye”, "world" }
@test: { “hello " goodbye”, "world" }
@test: { “hello "goodbye”, "world" }
@test: { “hello "goodbye"”, "world" }
@test: { “hello "goodbye" all”, "world 1a" }
@test: { “hello "goodbye"all”, "world 1b" }
@test: { “hello"goodbye" all”, "world 1c" }
@test: { “hello"goodbye"all”, "world 1c" }
@test: { “hello ""goodbye""”, "world 2" }
@test: { “hello ""goodbye"" all”, "world 3" }
@test: { “hello ""goodbye""all”, "world 4" }
@test: { “hello """goodbye""" all”, "world" }
@test: { “hello ""goodbye""allp”, "world" }
@test: { “hello ""a""""goodbye""""b""”, "world 5" }
@test: { “hello "a"""goodbye"""b"”, "world 6" }
@test: { “hello "a"""goodbye""b"”, "world 7" }
@test: { "hello goodbye", "wor ld&world" }
@test: { "hello goodbye", "&world" }
@test: { "hello&", "goodbye world" }
@test: { "hello, goodbye, world!" }
if not windows
	@test: { "*.exe" }   ; there are executables in this folder, so the asterisk will expand on Windows
	@test: { "*.praat" }   ; there is a Praat script in this folder, so the asterisk will expand on Windows
endif
@test: { "*.praathello" }
@test: { "*.praat ; ls -al"}   ; to show that this is safe
@test: { "*.praat ; ls -al", "hello" }   ; to show that this is safe
@test: { "*.praathello" + newline$ + "ls -al"}   ; to show that this is safe
@test: { "*.praathello" + newline$ + "ls -al", "hello"}   ; to show that this is safe
@test: { "\""" }

#
# From https://learn.microsoft.com/en-us/cpp/c-language/parsing-c-command-line-arguments?view=msvc-170
#
@test: { “a b c”, “d”, “e” }
@test: { “ab"c”, “\”, “d” }
@test: { “a\\\b”, “de fg”, “h” }
@test: { “a\"b”, “c”, “d” }
@test: { “a\\b c”, “d”, “e” }
@test: { “a b" c d” }

@test: { “back\slash\” }
@test: { "\" }
@test: { "\\" }
@test: { "\\\" }
@test: { "\\\\" }
@test: { "\\\\\" }
@test: { "\\\\\\" }

procedure test: .args$#
	executable$ =
	... if macintosh then
		... if praat_arm64 then "spit_mac_arm64" else "spit_mac_intel64" fi
	... else if windows then
		... if praat_arm64 then "spit_win_arm64.exe" else "spit_win_intel64.exe" fi
	... else if unix then
		... if praat_arm64 then "spit_linux_arm64"
		... else if praat_s390x then "spit_linux_s390x"
		... else if praat_armv7 then "spit_linux_armv7"
		... else "spit_linux_intel64"
		... fi fi fi
	... else "spit_unknown" fi fi fi
	;executable$ = "a.exe"
	folderPath$ = if macintosh then "./" else "" fi
	appendInfoLine ()
	@testje: folderPath$ + executable$, .args$#
	executable$ = replace$ (executable$, "_", " ", 0)
	@testje: folderPath$ + executable$, .args$#
endproc
procedure testje: .spittingApp$, .args$#
	appendInfoLine: .spittingApp$
	.narg = size (.args$#)
	for .iarg to .narg
		appendInfoLine: "in[", .iarg, "] = <<", .args$# [.iarg], ">>"
	endfor
	if .narg = 0
		.output$ = runSubprocess$: .spittingApp$
	elif .narg = 1
		.output$ = runSubprocess$: .spittingApp$, .args$# [1]
	elif .narg = 2
		.output$ = runSubprocess$: .spittingApp$, .args$# [1], .args$# [2]
	elif .narg = 3
		.output$ = runSubprocess$: .spittingApp$, .args$# [1], .args$# [2], .args$# [3]
	elif .narg = 4
		.output$ = runSubprocess$: .spittingApp$, .args$# [1], .args$# [2], .args$# [3], .args$# [4]
	elif .narg = 5
		.output$ = runSubprocess$: .spittingApp$, .args$# [1], .args$# [2], .args$# [3], .args$# [4],
		... .args$# [5]
	elif .narg = 6
		.output$ = runSubprocess$: .spittingApp$, .args$# [1], .args$# [2], .args$# [3], .args$# [4]
		... .args$# [5], .args$# [6]
	elif .narg = 7
		.output$ = runSubprocess$: .spittingApp$, .args$# [1], .args$# [2], .args$# [3], .args$# [4]
		... .args$# [5], .args$# [6], .args$# [7]
	elif .narg = 8
		.output$ = runSubprocess$: .spittingApp$, .args$# [1], .args$# [2], .args$# [3], .args$# [4]
		... .args$# [5], .args$# [6], .args$# [7], .args$# [8]
	elif .narg = 9
		.output$ = runSubprocess$: .spittingApp$, .args$# [1], .args$# [2], .args$# [3], .args$# [4]
		... .args$# [5], .args$# [6], .args$# [7], .args$# [8], .args$# [9]
	endif
	assert index (.output$, "arg [0] = <<" + .spittingApp$ + ">>")   ; <<'.output$'>>
	for .iarg to .narg
		if not index (.output$, "arg [" + string$ (.iarg) + "] = <<" + .args$# [.iarg] + ">>")
			for i to length (.output$)
				kar$ = mid$ (.output$, i)
				appendInfoLine: i, " ", kar$, " ", unicode (kar$)
			endfor
			for i to length (.args$# [.iarg])
				kar$ = mid$ (.args$# [.iarg], i)
				appendInfoLine: i, " ", kar$, " ", unicode (kar$)
			endfor
		endif
		assert index (.output$, "arg [" + string$ (.iarg) + "] = <<" + .args$# [.iarg] + ">>")   ; <<'.output$'>>
	endfor
	assert not index (.output$, "arg [" + string$ (.narg + 1) + "]")   ; <<'.output$'>>
endproc
appendInfoLine: newline$, "OK"
