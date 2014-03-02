.. include:: /helpers.rst

.. links
    none

nana::gui::tabbar
=================

.. code-block:: cpp

    nana/gui/widgets/tabbar.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: tabbar\<typename Type> \
    : public widget_object\<widget_tag, DrawerTrigger>

    A tabbar widget is analogous to dividers in a notebook or the labels in a
    file cabinet.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Template Parameters
-------------------

.. code-block:: cpp

    typename Type

The type of element data which is stored in the tabbar.

Typedefs
--------

.. cpp:type:: tabbar::value_type

    It is a alias for template parameter Type.

.. cpp:type:: tabbar::item_renderer

    The interface of an item renderer. A user-defined item renderer should be
    derived from this interface.

.. cpp:type:: tabbar::ext_event_type

    The extended event.

.. cpp:type:: tabbar::button_add

    The type identifies the add button of the tabbar's toolbox.

.. cpp:type:: tabbar::button_scroll

    The type identifies the scroll button of the tabbar's toolbox.

.. cpp:type:: tabbar::button_list

    The type identifies the list button of the tabbar's toolbox.

.. cpp:type:: tabbar::button_close

    The type identifies the close button of the tabbar's toolbox.

.. warning::

    The following declaration is not parsed by the documentation system.

.. cpp:type:: template<typename Add = null_type, typename Scroll = null_type, \
    typename List = null_type, typename Close = null_type> \
    struct button_container;

    A template class identifies the buttons of the tabbar's toolbox. Refer to
    notes for more details.

Public Functions
----------------

.. cpp:function:: tabbar::tabbar()

    The default constructor.

.. cpp:function:: tabbar::tabbar(window wd, bool visible)

    Create a tabbar.

.. cpp:function:: tabbar::tabbar(window wd, const nana::string& title, bool \
    visible = true)

    Ditto.

.. cpp:function:: tabbar::tabbar(window wd, const nana::char_t* title, bool \
    visible = true)

    Ditto.

.. cpp:function:: tabbar::tabbar(window wd, const rectangle& r = rectangle, \
    bool visible = true)

    Ditto.

.. cpp:function:: void tabbar::active(size_t i)

    Actives a tab specified by i.

.. cpp:function:: size_t tabbar::active() const

    Returns the index of the active tab.

.. cpp:function:: tabbar::value_type& tabbar::at(size_t i) const

    Returns i'th element

.. cpp:function:: void tabbar::close_fly(bool enable)

    Enables or disables the close button of toolbox to draw in each tab.

.. cpp:function:: pat::cloneable<tabbar::item_renderer> & tabbar::ext_renderer()

    Returns a reference to the cloneable item renderer.

.. cpp:function:: void tabbar::ext_renderer(\
    const pat::cloneable<tabbar::item_renderer>&)

    Sets a new user-defined item renderer

.. cpp:function:: tabbar::ext_event_type& tabbar::ext_event() const

    Retrieves the extended event object.

.. cpp:function:: size_t tabbar::length() const

    Returns the number of items.

.. cpp:function:: void tabbar::push_back(const nana::string& text)

    Append a new item.

.. cpp:function:: void tabbar::relate(size_t pos, nana::gui::window)

    Binds a window to an item specified by pos, if the item is selected, shows
    the window, otherwise, hides it.

.. cpp:function:: void tabbar::tab_bgcolor(size_t pos, nana::color_t)

    Set the tab item background color.

.. cpp:function:: void tabbar::tab_fgcolor(size_t pos, nana::color_t)

    Set the text color of a tab item.

.. cpp:function:: void tabbar::tab_image(size_t pos, const nana::paint::image& img)

    Sets the image of a tab item.

.. cpp:function:: void tabbar::text(size_t pos, const nana::string& str)

    Sets the title of the specified item. If pos is invalid, the method throws a std::out_of_range object.

.. cpp:function:: nana::string tabbar::text(size_t pos) const

    Returns a title of a specified item. If pos is invalid, the method trhows a std::out_of_range object.

.. cpp:function:: void tabbar::toolbox(const ButtonContainer& bc, bool)

    **template<typename ButtonContainer>**
    |br|
    Sets buttons of the tabbar's toolbox, refer to notes for more details.

.. cpp:function:: tabbar::value_type& tabbar::operator[](size_t pos) const

    Returns pos'th element

Notes
-----

Note 1
^^^^^^

The button_container is a member template of tabbar, it is used for identifing
the buttons of toolbox of tabbar. The buttons of toolbox are scrolling button,
list button and close button, only scrolling button is enabled by default,
enable other buttons by employing the tabbar::toolbox member function.
An example:

.. code-block:: cpp

    typedef tabbar<int> tabbar_t;
    tabbar_object.toolbox(tabbar_t::button_container<tabbar_t::button_list, tabbar_t::button_close>(), true);

Sets the list button and close button to be enabled.

.. image:: /_static/images/tabbar.png
    :align: center

Note 2
^^^^^^

The tabbar provides an event_trigger class for abstract event, such as selecting
an item or closing an item.

.. code-block:: cpp

    template<typename Tabbar>
    struct ext_event_type
    {
        typedef Tabbar tabbar;
        typedef typename tabbar::value_type value_type;

        nana::functor_group<void(tabbar&, value_type&)> add_tab;
        nana::functor_group<void(tabbar&, value_type&)> active;
        nana::functor_group<bool(tabbar&, value_type&)> remove;
    };

* The *add_tab()* is invoked when a new item is being created.
* The *select()* method is invoked when selects an item.
* The *remove()* method is invoked when closes an item and to stop closing the
  item by returning false.

Examples
--------

None.

See Also
--------

None.
