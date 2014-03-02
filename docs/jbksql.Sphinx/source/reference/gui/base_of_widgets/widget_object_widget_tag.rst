.. include:: /helpers.rst

.. links
    none

.. warning:: This page needs review.

widget_object<widget_tag, DrawerTrigger>
========================================

.. code-block:: cpp

    nana/gui/widgets/widget.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: widget_object\<widget_tag, DrawerTrigger> : public widget

    A **widget_object<widget_tag, DrawerTrigger>** is a base class of all the
    classes defined as a widget window.

    Inherits:
        * :cpp:class:`widget`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Public Functions
----------------

.. cpp:function:: void widget_object_wt::background(colour_t)

    Sets the color of background.

.. cpp:function:: colour_t widget_object_wt::background() const

    Returns the color of background.

.. cpp:function:: nana::string widget_object_wt::caption() const

    Returns the caption.

.. cpp:function:: void widget_object_wt::caption(const nana::string&)

    Sets the caption.

.. cpp:function:: void widget_object_wt::close() const

    Closes the window.

.. cpp:function:: bool widget_object_wt::create(window wd, bool visible)

    Creates no-size a widget.

.. cpp:function:: bool widget_object_wt::create(window wd, \
    const nana::rectangle& r = rectangle(), bool visible = true)

    Creates a specified size widget.

.. cpp:function:: void widget_object_wt::cursor(nana::gui::cursor)

    Sets a system predefined cursor.

.. cpp:function:: nana::gui::cursor widget_object_wt::cursor() const

    Retrieves the shape of cursor.

.. cpp:function:: bool widget_object_wt::empty() const

    Determines whether the manipulator is handling a window.

.. cpp:function:: bool widget_object_wt::enabled() const

    Determines whether the window is enabled for mouse and keyboard input.

.. cpp:function:: void widget_object_wt::enabled(bool)

    Sets the window whether enabled for mouse and keyboard input.

.. cpp:function:: bool widget_object_wt::focused() const

    Returns true if the widget has focused.

.. cpp:function:: void widget_object_wt::foreground(colour_t)

    Sets the color of foreground.

.. cpp:function:: colour_t widget_object_wt::foreground() const

    Returns the color of foreground.

.. cpp:function:: void widget_object_wt::hide()

    Sets the window invisible.

.. cpp:function:: event_handle widget_object_wt::make_event(Function)

    **template<typename Event, typename Function>**
    |br|
    Register an event callback functor.

.. warning:: The following declaration is not parsed by the documentation
    system.

.. cpp:function:: event_handle widget_object_wt::make_event(Class& object, \
    void (Concept::*)(const eventinfo&))

    **template<typename Event, typename Class, typename Concept>**
    |br|
    Register an event callback functor. The functor is a member function of
    Concept.

.. cpp:function:: void widget_object_wt::move(int x, int y)

    Moves the window to a new position.

.. cpp:function:: void widget_object_wt::show()

    Sets the window visible.

.. cpp:function:: nana::size widget_object_wt::size() const

    Returns the size of window.

.. cpp:function:: void widget_object_wt::size(unsigned width, unsigned height)

    Sets the size of window.

.. cpp:function:: void widget_object_wt::umake_event(event_handle) const

    Deletes an event callback by a handle.

Notes
-----

None.

Examples
--------

None.

See Also
--------

* :cpp:class:`button`
