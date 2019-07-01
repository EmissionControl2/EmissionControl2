// ECWavetable Test

///////////////////////////////////////////////////////////////////////////////

(
s = Server.internal;
Server.default = s;
s.boot;
)

(
var	window, wavetable;
window = SCWindow.new("< Wavetable >", Rect.new(0, 1024, 612, 260), false, true);
wavetable = ECWavetable.new(window, "", Color.new255(155, 48, 255), s, 512);
wavetable.draw(5, 5, 20, 5, 40, \top, \bottom, false);
window.front;
)
