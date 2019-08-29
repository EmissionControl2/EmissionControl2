// DensitySlider Test
(
w = Window.new("< test >", Rect.new(0, 1024, 555, 50));
a = DensitySlider.new(w,
	Array.fill(8, {ControlSet.new;}),ControlSet.new,
	"density",
	0.1, 1000,
	0.1, 1000,
	\exp, 0.1,
	10, "% ");

a.draw(5, 5, 20, 5, 200, 40, 20, 85, 40);
w.front;
)