.. include:: /helpers.rst

.. links
    none

nana::gui::toolbar
==================

.. code-block:: cpp

    nana/gui/widgets/toolbar.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: toolbar \
    : public widget_object\<widget_tag, DrawerTrigger>

    A toolbar is a control bar that contains the buttons for controlling.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Typedefs
--------

.. cpp:type:: toolbar::size_type

    A type that counts the number of elements.

.. cpp:type:: toolbar::ext_event_type

    The extended event.


Public Functions
----------------

.. cpp:function:: toolbar::toolbar()

    Default constructor.

.. cpp:function:: toolbar::toolbar(window, bool visible)

    The constructor inits the toolbar in the specified window.

.. cpp:function:: toolbar::toolbar(window, const rectangle& r = rectangle(), \
    bool visible = true)

    Ditto

.. cpp:function:: void toolbar::append()

    Adds a separator.

.. cpp:function:: void toolbar::append(const nana::string& text, \
    const nana::paint::image& img)

    Adds a control button .

.. cpp:function:: void toolbar::append(const nana::string& text)

    Adds a control button.

.. cpp:function:: bool toolbar::enable(size_type n)

    Gets the enabled state of control button.

.. cpp:function:: void toolbar::enable(size_type n, bool eb)

    Sets the enabled state of control button.

.. cpp:function:: ext_event_type& toolbar::ext_event() const

    Retrieves the extended event object.

.. cpp:function:: void toolbar::scale(unsigned)

    Sets the scale of control button.

Notes
-----

Note 1
^^^^^^

Defintion of the extended event.

.. code-block:: cpp

    struct ext_event_type
    {
        nana::functor_group<void(nana::gui::toolbar&, size_t)> selected; //
        nana::functor_group<void(nana::gui::toolbar&, size_t)> enter;
        nana::functor_group<void(nana::gui::toolbar&, size_t)> leave;
    };

* **select:** a mouse click on a control button.
* **enter:** the mouse enters a control button.
* **leave:** the mouse leaves a control button.

Examples
--------

None.

See Also
--------

None.
