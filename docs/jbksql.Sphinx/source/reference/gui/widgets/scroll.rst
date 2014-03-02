.. include:: /helpers.rst

.. links
    none

nana::gui::scroll
=================

.. code-block:: cpp

    nana/gui/widgets/scroll.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: scroll\<bool> \
    : public widget_object\<widget_tag, DrawerTrigger>

    A scroll provides a way that a window can display the data object that is
    larger than the window's client area.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Template Parameters
-------------------

.. code-block:: cpp

    bool Vertical

The scrollbar's style, vertical or horizontal.

Typedefs
--------

.. cpp:type:: scroll::size_type

    A type that represents the value of a scrollbar.

Public Functions
----------------

.. cpp:function:: scroll::scroll()

    The default constructor.

.. cpp:function:: scroll::scroll(window wd, bool visible)

    Creates a scrollbar.

.. cpp:function:: scroll::scroll(window wd, const rectangle& r = rectangle(), \
    bool visible = false)

    Ditto.

.. cpp:function:: bool scroll::scrollable(bool for_less) const

    Determines whether it is scrollable. If for_less is true, it returns true
    when it can be scrolled for a less value. Otherwise, it returns true when it
    can be scrolled for a large value.

.. cpp:function:: size_type scroll::amount() const

    Returns the amount value of the scrollbar.

.. cpp:function:: void scroll::amount(size_type)

    Sets the amount value of the scrollbar.

.. cpp:function:: size_type scroll::value() const

    Returns the value of the scrollbar.

.. cpp:function:: void scroll::value(size_type)

    Sets the value of the scrollbar.

.. cpp:function:: size_type scroll::range() const

    Returns the range of the scrollbar, the range determines the length of thumb.

.. cpp:function:: void scroll::range(size_type m)

    Sets the range of the scrollbar.

.. cpp:function:: size_type scroll::step() const

    Returns the step of a scrollbar, the step stands for a variation of the value.

.. cpp:function:: void scroll::step(size_type)

    Sets the the step.

.. cpp:function:: size_type scroll::make_step(bool forward)

    Increases or decreases value by step and returns a new value of a scrollbar.

.. cpp:function:: size_type scroll::make_scroll(bool forward)

    Increases or decreases value by steps as if scrolling with mouse wheel.

Notes
-----

None.

Examples
--------

None.

See Also
--------

None.
