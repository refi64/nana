.. include:: /helpers.rst

.. links
    none

nana::gui::categorize
=====================

.. code-block:: cpp

    nana/gui/widgets/categorize.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: categorize \
    : public widget_object\<widget_tag, DrawerTrigger>

    A categorize widget is used for representing the architecture of categories
    and what category is chosen. The categorize widget can be used for
    representing a path of a directory or the order of hierarchy.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Template Parameters
-------------------

.. code-block:: cpp

    T

The value type of categorize, the type of object that stored in categorize.

Typedefs
--------

.. cpp:type:: categorize::value_type

    The type of object, T, stored in categorize.

.. cpp:type:: categorize::ext_event_type

    The methods for extra events.(See Note 1)

.. cpp:type:: categorize::renderer

    The interface for user-defined renderer.

Public Functions
----------------

.. cpp:function:: categorize::categorize()

    Default Constructor.

.. cpp:function:: categorize::categorize(window wd, bool visible)

    Create a categorize widget in a specified window
    and allow to to turn on/off the visibility.

.. cpp:function:: categorize::categorize(window wd, const nana::string& title, \
    bool visible = true)

    Ditto.

.. cpp:function:: categorize::categorize(window wd, const nana::char_t* title, \
    bool visible = true)

    Ditto.

.. cpp:function:: categorize::categorize(window wd, \
    const rectangle& r = rectangle(), bool visible = true)

    Ditto.

.. cpp:function:: ext_event_type& categorize::ext_event() const

    Refers to the object of extra events

.. cpp:function:: categorize& categorize::insert(const nana::string& name, \
    const value_type&)

    Insert a new category with a specified name and the object of value type.
    The new category would be inserted as a child in current category,
    and after inserting, the new category is replaced of the current
    category as a new current one.

.. cpp:function:: categorize& categorize::childset(const nana::string& name, \
    const value_type&)

    Inserts a child category into current category.

.. cpp:function:: categorize& categorize::childset_erase(\
    const nana::string& name)

    Erases a child category with a specified name from current category.

.. cpp:function:: categorize& categorize::splitstr(const nana::string&)

    Sets the splitter string

.. cpp:function:: nana::string categorize::splitstr() const

    Retrieves the splitter string.

.. cpp:function:: value_type& categorize::value() const

    Retrieves the reference of the current category's value type object.
    If current category is empty, it throws a exception of std::runtime_error.

Notes
-----

Note 1
^^^^^^
The definition of *ext_event_type*

.. code-block:: cpp

    struct ext_event_type
    {
        typedef Implementation-Specified categorize_type;
        typedef typename categorize_type::value_type value_type;

        nana::fn_group<void(categorize_type&, value_type&)> selected; //Selects a category
    };

Note 2
^^^^^^

Definition of *renderer*

.. code-block:: cpp

    class renderer
    {
    public:
        typedef nana::paint::graphics & graph_reference;

        enum state_t{state_normal, state_over, state_pressed};

        struct ui_element
        {
            enum t
            {
                none, //Out of the widget
                somewhere, item_root, item_name, item_arrow
            };

            static const size_t npos = static_cast<size_t>(-1);
            t what;
            size_t index;

            ui_element();
        };

        virtual ~renderer() = 0;
        virtual void background(graph_reference, window, const nana::rectangle&, const ui_element&) = 0;
        virtual void root_arrow(graph_reference, const nana::rectangle&, state_t) = 0;
        virtual void item(graph_reference, const nana::rectangle&, size_t index,
                          const nana::string& name, unsigned text_height,
                          bool has_child, state_t) = 0;
        virtual void border(graph_reference) = 0;
    };

Note 3
^^^^^^

The *caption()* methods of the categorize widget is used for retrieving and
setting the category.

Examples
--------

Categorize widget example
^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

    #include <iostream>
    #include <nana/gui/wvl.hpp>
    #include <nana/gui/widgets/categorize.hpp>

    void selected(nana::gui::categorize<int> & categ, int&)
    {
        nana::gui::msgbox mb(categ, STR("categorize"));
        mb.icon(mb.icon_information);
        mb<<STR("The value of selected is ")<<categ.value()<<STR(", caption is \"")
          <<categ.caption()<<STR("\"");    //caption() method, see Note 3.
        mb();
    }

    int main()
    {
        using namespace nana::gui;
        form fm;
        categorize<int> categ(fm, 10, 10, 200, 24);

        categ.childset(STR("Invalid Category"), 0);
        //Invalid category because of empty current category.

        categ.insert(STR("First"), 0);
        //Insert a category and now it is the current category.

        categ.insert(STR("Second"), 1);
        //Insert a category as a child of "First" category, and then
        //the "Second" is the current category.

        categ.childset(STR("Third"), 2);
        //Insert a category as a child of "Second".

        categ.childset(STR("Another Child"), 2);
        //childset() always insert categories into current category, and
        //it does not displace the current category, and therefore "Another Child"
        //is a child of "Second".

        categ.ext_event().selected = selected;
        //This may crash in VC2012-RC due to compiler bug.
        //Please refer to compatibility-issue for more details

        fm.show();
        exec();
    }

.. image:: /_static/images/categorize.png
    :align: center

See Also
--------

None.
