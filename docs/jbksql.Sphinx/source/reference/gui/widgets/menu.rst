.. include:: /helpers.rst

.. links
.. _See Note 4: #note-4
.. _See Note 5: #note-5

nana::gui::menu
===============

.. code-block:: cpp

    nana/gui/widgets/menu.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: menu \
    : public widget_object\<root_tag, DrawerTrigger>

    A menu widget is a list of items that specify options or group
    of options for an application.

    Inherits:
        * :cpp:class:`widget_object\<root_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Typedefs
--------

.. cpp:type:: menu::item_proxy

    It a proxy used for a parameter in a callback.

.. cpp:type:: menu::event_fn_t

    A callback function object type, the prototype is void(item_proxy&);

.. cpp:type:: menu::check_t

    .. code-block:: cpp

        enum check_t { check_none, check_option, check_highlight }

    It defines the check styles. CheckNone, CheckOption, CheckHighlight

.. cpp:type:: menu::renderer_interface

    The interface of user-defined renderer. (`See Note 5`_)

Public Fuctions
---------------

.. cpp:function:: menu::menu()

    The default constructor.

.. cpp:function:: menu::~menu()

    The destructor.

.. cpp:function:: void menu::answerer(std::size_t n, const event_fn_t&)

    Modify answerer of the specified item.

.. cpp:function:: void menu::append(const nana::string& text, \
    const event_fn_t &answerer = event_fn_t())

    Appends an item to the menu. answerer is a callback functor.

.. cpp:function:: void menu::append_splitter()

    Appends a splitter to the menu.

.. cpp:function:: void menu::clear()

    Erases all of the items.

.. cpp:function:: void menu::close()

    Closes the menu.
    It does not destroy the menu, just close the window for the menu.

.. cpp:function:: void menu::check_style(std::size_t n, check_t style)

    Sets the check style for the n'th item.

.. cpp:function:: void menu::checked(std::size_t n, bool check)

    Sets the check state of an item.

.. cpp:function:: bool menu::checked(std::size_t n) const

    Returns the check state of an item.

.. cpp:function:: menu* menu::create_sub_menu(std::size_t n)

    Creates a sub menu for the n'th item.

.. cpp:function:: void menu::enabled(std::size_t n, bool)

    Enables or disables the mouse or keyboard input for the n'th item.

.. cpp:function:: bool menu::enabled(std::size_t n) const

    Returns true if the n'th item is enabled.

.. cpp:function:: void menu::erase(std::size_t n)

    Removes n'th item.

.. cpp:function:: void menu::exit_submenu()

    Closes the current window of the sub menu.

.. cpp:function:: void menu::gaps(const nana::point &)

    Sets the gap between a menu and its sub menus. (`See Note 4`_)

.. cpp:function:: void menu::goto_next(bool forward)

    Moves the focus to the next item or previous item.

.. cpp:function:: bool menu::goto_submenu()

    Popup the sub menu of the current item if it has a sub menu.
    It returns true if it succeeds.

.. cpp:function:: void menu::image(std::size_t n, const nana::paint::image&)

    Sets a icon image for a specified menu item.

.. cpp:function:: menu & menu::item_pixels(unsigned pixels)

    Sets the height in pixel for the items.

.. cpp:function:: unsigned menu::item_pixels() const

    Returns the height in pixels for the itmes.

.. cpp:function:: bool menu::link(std::size_t n, menu& menu)

    Link a menu as a sub menu for the n'th item.

.. cpp:function:: menu* menu::link(std::size_t n)

    Retrieves a linked sub menu by the n'th index.

.. cpp:function:: menu& menu::max_pixels(unsigned px)

    Sets the max pixels of the menu width.

.. cpp:function:: unsigned menu::max_pixels() const

    Gets the max pixels of the menu width.

.. cpp:function:: void menu::popup(window owner, int x, int y, \
    bool owner_menubar)

    Popup the menu at the owner window. the last parameter specifies whether
    it is for menubar, always be false.

.. cpp:function:: void menu::renderer(const Renderer&)

    **template<typename Renderer>**
    |br|
    Sets a user-defined renderer. (`See Note 5`_)

.. cpp:function:: void menu::renderer(\
    const nana::pat::cloneable<renderer_interface>&)

    Ditto.

.. cpp:function:: const nana::pat::cloneable<renderer_interface>& \
    menu::renderer() const

    Returns a user-defined renderer. (`See Note 5`_)

.. cpp:function:: int menu::send_shortkey(nana::char_t key)

    Sents a key to the menu.

.. cpp:function:: std::size_t menu::size() const

    Return the number of items.

C++03
-----

.. warning:: The following declaration is not parsed by the documentation
    system.

.. cpp:function:: void menu::destroy_answer(const nana::functor<void()>&)

    Sets an answerer for the callback while the menu window is closing.

C++11
-----

.. warning:: The following declaration is not parsed by the documentation
    system.

.. cpp:function:: void menu::destroy_answer(const std::function<void()>&)

    Ditto.

Notes
-----

Note 1
^^^^^^

The widget sets the shortkey to the character following the first **&** in
text, for the item. e.g. "File(&F)" or "&File".

Note 2
^^^^^^

The type *item_proxy* is used for a callbacking, a programmer should not take
care about the object of *item_proxy*, it is created and destroyed by menu.
The definition is:

.. code-block:: cpp

    class item_proxy: nana::noncopyable
    {
    public:
        implementation-specified constructor
        void enabled(bool);  //Sets the enable state of the item.
        bool enabled() const; //Gets the enable state of the item.
        std::size_t index() const; //Gets the index of the item.
    private:
        //Private data members...
    };

Note 3
^^^^^^

There is a helper for automatically popuping a menu.

.. code-block:: cpp

    class menu_popuper
    {
    public:
        //C++03
        menu_popuper(menu&, mouse::t = mouse::right_button);
        menu_popuper(menu&, window owner, const point& pos, mouse::t = mouse::right_button);

        //C++11
        menu_popuper(menu&, mouse = mouse::right_button);
        menu_popuper(menu&, window owner, const point& pos, mouse = mouse::right_button);

        void operator()(const eventinfo&);
    private:
        //Implemented-Specified private members
    };

Now let's use it. There is a button, it popups the menu when it is clicked.

.. code-block:: cpp

    #include <nana/gui/wvl.hpp>
    #include <nana/gui/widgets/button.hpp>
    #include <nana/gui/widgets/menu.hpp>

    void on_menu_item(nana::gui::menu::item_proxy& ip)
    {
        std::size_t index = ip.index(); //Get the index of the clicking item.
    }

    int main()
    {
        using namespace nana::gui;
        form fm;

        //We need a menu object
        menu mobj;
        mobj.append(STR("Item 0"), &on_menu_item);
        mobj.append_splitter();
        mobj.append(STR("Item 1"), &on_menu_item);

        //Now we need a button.
        button btn(fm, nana::rectangle(nana::point(10, 10), nana::size(100, 25)));
        btn.caption(STR("Popup Menu"));

        //Popup the menu when right clicking the button.
        btn.make_event<events::click>(menu_popuper(mobj));

        //Or popuping the menu with a specified coordinate when any mouse button is clicked.
        //btn.make_event<events::click>(menu_popuper(mobj, btn, nana::point(0, 26), mouse::any_button);

        fm.show();
        exec();
    }

Note 4
^^^^^^

The gap of menu is used to specify the interval pixels between the menu and its
sub menus. It affects all sub menus and all sub menus of its sub menus.

.. image:: /_static/images/gap_of_menu.png
    :align: center

.. code-block:: cpp

    int main()
    {
        using namespace nana::gui;

        menu mobj;
        mobj.append(STR("Item 0"));
        mobj.append(STR("Item 1"));

        mobj.gaps(nana::point(3, -2)); //Remove this statement for default gaps.

        menu * sub = mobj.create_sub_menu(0);
        sub->append(STR("Item 0"));
        sub->append(STR("Item 1"));

        sub = sub->create_sub_menu(0);
        sub->append(STR("A sub's sub menu item"));

        form fm;
        fm.make_event<events::click>(menu_popuper(mobj));
        fm.show();
        exec();
    }

Note 5
^^^^^^

When a renderer is set for a menu, it affects all sub menus of the menu and all
sub menus of its all sub menus.

An example of a user-defined renderer:

.. code-block:: cpp

    //The definition of class renderer_interface

    class renderer_interface
    {
    public:
        typedef nana::paint::graphics & graph_reference;

        //C++03
        struct state
        {
            enum t{ normal, active};
        };

        //C++11
        enum class state
        {
            normal, active
        };

        struct attr
        {
            //C++03
            state::t item_state;
            //C++11
            state item_state;

            bool enabled;
            bool checked;
            int check_style;
        };

        virtual ~renderer_interface() = 0;
        virtual void background(graph_reference, window) = 0;
        virtual void item(graph_reference, const nana::rectangle&, const attr&) = 0;
        virtual void item_image(graph_reference, const nana::point&, const nana::paint::image&) = 0;
        virtual void item_text(graph_reference, const nana::point&, const nana::string&, unsigned pixles, const attr&) = 0;
        virtual void sub_arrow(graph_reference, const nana::point&, unsigned pixels, const attr&) = 0;

    };

The implementation of the user-defined renderer. In this example, the renderer
only provides the drawing of background and drawing of item, so that we have to
employ the existing renderer of a menu for other drawing methods.

.. image:: /_static/images/renderer_of_menu.png
    :align: center

.. code-block:: cpp

    using namespace nana::gui;

    class renderer: public menu::renderer_interface
    {
    public:
        //My renderer employs the existing renderer of a menu.
        renderer(const nana::pat::cloneable_interface<renderer_interface> * rd)
            : rdptr_(rd->clone())
        {}

        //The renderer is copy-constructable, therefore a deep-copy is required.
        renderer(const renderer & rd)
            : rdptr_(rd.rdptr_->clone())
        {}

        ~renderer()
        {
            rdptr_->self_delete();
        }
    private:
        void background(graph_reference graph, window wd)
        {
            graph.rectangle(0xFFFFFF, true);
            graph.rectangle(0x5DC1AC, false);

            //Makes the menu transparent, it only works under Windows with #include <windows.h>
            HWND native = reinterpret_cast(API::root(wd));
            DWORD ex_style = ::GetWindowLong(native, GWL_EXSTYLE);
            ::SetWindowLong(native, GWL_EXSTYLE, ex_style | 0x00080000 /*WS_EX_LAYERED*/);
            typedef         BOOL (WINAPI *slwa_t)(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags);
            slwa_t slwa = reinterpret_cast(::GetProcAddress(::GetModuleHandleA("User32.DLL"), "SetLayeredWindowAttributes"));
            if        (slwa)
                slwa(native, 0, 220, 0x00000002/*LWA_ALPHA*/);
        }

        void         item(graph_reference graph, const nana::rectangle& r, const attr & atr)
        {
            if            (state::active == atr.item_state)
                graph.rectangle(r, 0x9ADCCA, true);
        }

        void         item_image(graph_reference graph, const nana::point& pos, const nana::paint::image& img)
        {
                rdptr_->refer().item_image(graph, pos, img);
        }

        void         item_text(graph_reference graph, const nana::point& pos, const nana::string& text, unsigned pixels, const attr& atr)
        {
                rdptr_->refer().item_text(graph, pos, text, pixels, atr);
        }

        void         sub_arrow(graph_reference graph, const nana::point& pos, unsigned pixels, const attr & atr)
        {
                rdptr_->refer().sub_arrow(graph, pos, pixels, atr);
        }

    private:
                nana::pat::cloneable_interface<renderer_interface> * rdptr_;
    };

    int main()
    {
        menu mobj;
        mobj.append(STR("Item 0"));
        mobj.append(STR("Item 1"));
                    mobj.create_sub_menu(1)->append(STR("A sub menu item"));

        mobj.renderer(renderer(mobj.renderer()));
        mobj.item_pixels(20);

        form fm;
        fm.make_event<events::click>(menu_popuper(mobj));
        fm.show();
        exec();
    }

Examples
--------

None.

See Also
--------

* :cpp:class:`menubar`
