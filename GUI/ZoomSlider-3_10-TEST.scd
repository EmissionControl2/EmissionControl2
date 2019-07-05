// ZoomSlider Test

///////////////////////////////////////////////////////////////////////////////

// array of ZoomSlider's
(
var	window, sliders;
window = Window.new("< Test Area >", Rect.new(0, 1024, 405, 260), true, true);
sliders = Array.fill(10, {
	ZoomSlider.new(window, "duration", 1, 10000, 1, 1000, \exp, 1, 100, "ms ");
});
sliders.do({ arg each, i;
	each.draw(5 + (45 * i), 5, \vertical, 40, 10, 100, 40, \left);
});
window.front;
)

// array of ZoomSlider's (explicitly calls setZoomSpec)
(
var	window, sliders;
window = Window.new("< Test Area >", Rect.new(0, 1024, 405, 260), true, true);
sliders = Array.fill(10, { ZoomSlider.new(window); });
sliders.do({ arg each, i;
	each.setZoomSpec("duration", 1, 10000, 1, 1000, \exp, 1, 100, "ms ");
	each.draw(5, 5 + (25 * i), \horizontal, 20, 5, 200, 60, \right);
});
window.front;
)