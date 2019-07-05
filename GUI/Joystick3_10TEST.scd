// Joystick Test
"/Users/jkilgore/Documents/education/Research/emissionControl-2019/scGUI/Joystick3_10.sc".load;
Platform.systemExtensionDir;   // Extensions available only to your user account

///////////////////////////////////////////////////////////////////////////////

(
var	window, joystick;
window = Window.new("< Joystick 1 >", Rect.new(0, 1024, 500, 500), true, true);

joystick = Joystick.new(window, "", Color.new(0.6904,0.8398,0.9147), nil, nil, true);
joystick.draw(5, 5, 20, 5, \bottom, \right, \top, true);
window.front;
)



