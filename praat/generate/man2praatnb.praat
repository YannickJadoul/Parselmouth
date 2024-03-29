form: "Convert from .man-file to .praatnb-file"
	text: 100, "Man version", ""
endform
manVersion$# = splitBy$# (man_version$, newline$)
writeInfo: “”
for line to size (manVersion$#)
	line$ = manVersion$# [line]
	inks$# = splitByWhitespace$# (line$)
	if size (inks$#) = 0
	elsif inks$# [1] = “MAN_BEGIN”
		date$ = inks$# [size (inks$#)]
		if not index (date$, “-”)
			year$ = mid$ (date$, 1, 4)
			month$ = mid$ (date$, 5, 2)
			day$ = mid$ (date$, 7, 2)
			line$ = replace$ (line$, date$, year$ + “-” + month$ + “-” + day$, 0)
		endif
		line$ = replace$ (line$, “ppgb”, “Paul Boersma”, 0)
		line$ = replace$ (line$, “djmw”, “David Weenink”, 0)
		line$ = “################################################################################”
		... + newline$ + replace$ (line$, “MAN_BEGIN (U"”, “"”, 0)
		line$ = replace$ (line$, “", U"”, “"” + newline$ + “© ”, 0)
		line$ = replace$ (line$, “", ”, “ ”, 0)
		line$ = replace_regex$ (line$, “\)$”, “”, 0)
	elsif inks$# [1] = “INTRO” or inks$# [1] = “NORMAL”
		line$ = newline$ + replace$ (line$, inks$# [1] + “ (U"”, “”, 0)
		line$ = replace_regex$ (line$, “ "$”, “”, 0)
		line$ = replace_regex$ (line$, “"\)$”, “”, 0)
	elsif inks$# [1] = “ENTRY”
		line$ = newline$ + replace$ (line$, inks$# [1] + “ (U"”, “”, 0)
		line$ = replace_regex$ (line$, “"\)$”, “”, 0)
		line$ += newline$ + mid$ (“======================================”, 1, length (line$) - 1)
	elsif inks$# [1] = “TERM”
		line$ = newline$ + replace$ (line$, inks$# [1] + “ (U"”, “”, 0)
		line$ = replace_regex$ (line$, “"\)$”, “”, 0)
	elsif inks$# [1] = “DEFINITION”
		line$ = “:” + tab$ + replace$ (line$, inks$# [1] + “ (U"”, “”, 0)
		line$ = replace_regex$ (line$, “ "$”, “”, 0)
		line$ = replace_regex$ (line$, “"\)$”, “”, 0)
	elsif inks$# [1] = “EQUATION”
		line$ = “~” + tab$ + replace$ (line$, inks$# [1] + “ (U"”, “”, 0)
		line$ = replace_regex$ (line$, “"\)$”, “”, 0)
	elsif inks$# [1] = “CODE”
	elsif inks$# [1] = “MAN_END”
		line$ = “”
	else
		line$ = replace$ (line$, tab$ + “"”, “”, 0)
		line$ = replace_regex$ (line$, “"\)$”, “”, 0)
		line$ = replace_regex$ (line$, “ "$”, “”, 0)
	endif
	if size (inks$#) = 0 or inks$# [1] <> “MAN_END”
		appendInfoLine: line$
	endif
endfor
