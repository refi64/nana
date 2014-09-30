

    #pragma warning(disable: 4996)
    #include <nana/gui/wvl.hpp>
    #include <nana/gui/widgets/button.hpp>
    #include <nana/gui/widgets/label.hpp>
    #include <nana/gui/widgets/textbox.hpp>
    #include <nana/gui/widgets/panel.hpp>
    #include <nana/gui/detail/basic_window.hpp>
    #include <nana/gui/place.hpp>
    #include <../temp/vplace.hpp>
    using namespace nana;
     
    #include <sstream>
    #include <iostream>
    #include <functional>
    #include <unordered_map>
    #include <sstream>
    #include <numeric>
    #include <memory>
    using namespace std;
    using namespace std::placeholders;
     
     
     
    int main() {
     
            form fm(API::make_center(400, 300));
            panel<true> left(fm);
            panel<true> right(fm);
     
            vplace plc(fm);
            plc.div("<<weight=100 left><right>>");
            plc.field("left") << left;
            plc.field("right") << right;
            plc.collocate();
     
            // left area
            button btn(left);
            btn.caption(L"btn");
            vplace plc1(left);
            plc1.div("<btn>");
            plc1.field("btn") << btn;
            plc1.collocate();
     
            // right area
            button top(right);
            top.caption(L"top");
            button bottom(right);
            bottom.caption(L"bottom");
     
            vplace plc2(right);
            plc2.div("< vertical < top> |30%< bottom >>"); // it's ok for 30, but not for 30%
     
            plc2.field("top") << top;
            plc2.field("bottom") << bottom;
            plc2.collocate();
           
     
            fm.show();
            exec();
    }

