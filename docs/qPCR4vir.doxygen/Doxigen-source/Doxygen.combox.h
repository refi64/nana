/**\class nana::gui::combox
This widget is a combo box which consists of a list box combined with a textbox. The list box should drop down when the user selects the drop-down arrow next to the control.

	#include <nana/gui/wvl.hpp>
	#include <nana/gui/widgets/combox.hpp>
	int main()
	{
		using namespace nana::gui;	   //No Minimize button, no maximize button and sizable frame.
		form fm(API::make_center(240, 100), appear::decorate<>());
		fm.caption(STR("Nana C++ Library - Example"));
		combox child(form, 20, 3, 150, 30);
		child.push_back(STR("Item 1"));
		child.push_back(STR("Item 2"));
		child.push_back(STR("Item 3"));
		child.push_back(STR("Item 4"));
		child.push_back(STR("Item 5"));
				; // This function will be called after changing the combox text by selecting 
				; // a text from the drop-down list.
		child.ext_event().selected = []{ std::cout<<"selected a new text"<<std::endl; };
		fm.show();
		exec();
	}

![](combox.png)

## n Notes
1. The class ext_event coming with the combox, is:

		struct ext_event_type
		{
		  nana::functor_group<void(nana::gui::combox&)> selected; //Selects a new item.
		};

	This class provides a method to implement that a program may respond to the combox 
	special event or state. It seems like the events registered by make_event member function.
	But the answer registered by make_event is used for general-purpose, such as mouse_down, 
	mouse_move, click and so on, while the combox´ ext_event_type is used for the own 
	providing event.

2. Refer to nana::gui::listbox for details of item_renderer.
*/