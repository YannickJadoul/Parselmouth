demo Times
demo 12
demo Select inner viewport: 0, 100, 0, 100
demo Axes: 0, 1, 0, 1
n = 100
for i to n
	demo Erase all
	;demoShow()
	demo Paint rectangle: "red", (i-1)/n, (i+0)/n, 0.4, 0.6
	;sleep (0.01)
	demoShow()
	sleep (0.01)   ; the time during which the red rectangle stays on the screenr
endfor
