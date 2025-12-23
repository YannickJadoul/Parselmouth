Times
12
Line width: 1.0
for side from 1 to 63
	Erase all
	Select outer viewport: 0, side, 0, side
	Draw inner box
	Save as PDF file: "kanweg" + string$ (side) + ".pdf"
endfor