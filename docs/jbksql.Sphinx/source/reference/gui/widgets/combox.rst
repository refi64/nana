.. include:: /helpers.rst

.. links
.. _See Note 1: #note-1
.. _See Note 2: #note-2

nana::gui::combox
=================

.. code-block:: cpp

    nana/gui/widgets/combox.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: combox \
    : public widget_object\<widget_tag, DrawerTrigger>

    This widget is a combo box which consists of a list box combined with a
    textbox, the list box should drop down when the user selects thr drop-down
    arrow next to the control.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Typedefs
--------

.. cpp:type:: combox::ext_event_type

    This class provides the methods for extra event. (`See Note 1`_)

.. cpp:type:: combox::item_renderer

    A super class for user defined item renderer. (`See Note 2`_)

Public Functions
----------------

.. cpp:function:: combox::combox()

    The default construction.

.. cpp:function:: combox::combox(window wd, bool visible)

    Creates a combox.

.. cpp:function:: combox::combox(window wd, const nana::string& title, \
    bool visible = true)

    Ditto.

.. cpp:function:: combox::combox(window wd, const nana::char_t* title, \
    bool visible = true)

    Ditto.

.. cpp:function:: combox::combox(window wd, const rectangle& r = rectangle(), \
    bool visible = true)

    Ditto.

.. cpp:function:: void combox::anyobj(std::size_t index, const T&)

    **template<typename T>**
    |br|
    Binds an object with the specified item.

.. cpp:function:: T * combox::anyobj(std::size_t index) const

    **template<typename T>**
    |br|
    Retrieves the binded object with the specified item. It returns a null ptr
    if a object is not binded or the type is mismatched or the item is out of
    range.

.. cpp:function:: void combox::clear()

    Clears all items.

.. cpp:function:: bool combox::editable() const

    Returns true if the combox is editable mode.

.. cpp:function:: void combox::editable(bool)

    Set the combox editable mode.

.. cpp:function:: ext_event_type& combox::ext_event() const

    Retrieves the ext_event object.

.. cpp:function:: void combox::image(std::size_t i, const nana::paint::image&)

    Sets an image for a specified item.

.. cpp:function:: nana::paint::image combox::image(std::size_t) const

    Retrieves the image of a specified item.

.. cpp:function:: void combox::image_pixels(unsigned pixels)

    Sets the width of image area. Defaulty is 16 pixels.

.. cpp:function:: combox& combox::push_back(const nana::string& text)

    Insert a text string to the listbox.

.. cpp:function:: std::size_t combox::option() const

    Get the index of item which is selected from drop-down list last time.

.. cpp:function:: void combox::option(std::size_t index)

    Set text that specifed by index.

.. cpp:function:: nana::string combox::text(std::size_t) const

    Retrieves the text of item in drop list.

.. cpp:function:: std::size_t combox::the_number_of_options() const

    Retrieves the number of options.

.. cpp:function:: void combox::renderer(item_renderer*)

    Set user-defined item renderer object. It is a address therefore user should
    not destroy the renderer object after it set to the combox.
    Passing null_ptr cancels the user-defined renderer object.

Notes
-----

Note 1
^^^^^^

The class *ext_event* is coming with the combox, its definition is

.. code-block:: cpp

    struct ext_event_type
    {
        nana::functor_group<void(nana::gui::combox&)> selected; //Selects a new item.
    };

This class provides a method to implement that a program may respond to the
combox special event or state. It seems like the events registered by
*make_event* member function. But the answering registered by *make_event* is
used for general-purpose, such as *mouse_down*, *mouse_move*, click and so on,
the combox *ext_event_type* is used for the own providing event.

Note 2
^^^^^^

Refer to nana::gui::float_listbox for details on *item_renderer*.

Examples
--------

None.

See Also
--------

None.
