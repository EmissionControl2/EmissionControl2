// SCNumberBox2 Test

///////////////////////////////////////////////////////////////////////////////

// SCNumberBox2 Test (defaults)
(
w = SCWindow.new("< test >", Rect.new(0, 1024, 500, 50));
a = Array.fill(10, { arg i;
	SCNumberBox2.new(w, Rect.new(5+(45*i), 5, 40, 20)); });
a.do({ arg each; each.value_(0); });
w.front;
)

///////////////////////////////////////////////////////////////////////////////

// SCNumberBox2 Test
(
w = SCWindow.new("< test >", Rect.new(0, 1024, 500, 50));
a = Array.fill(10, { arg i;
	SCNumberBox2.new(w, Rect.new(5+(45*i), 5, 40, 20)); });
a.do({ arg each;
	each.boxColor = Color.black;
	each.defaultStringColor_(Color.white);
	each.enterStringColor_(Color.green);
	each.shiftKeyScaler_(10);
	each.value_(0);
});
w.front;
)