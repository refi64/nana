.. include:: /helpers.rst

.. links
    none

.. warning:: This page needs review.

widget_object<root_tag, DrawerTrigger>
======================================

.. code-block:: cpp

    nana/gui/widgets/widget.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: widget_object\<root_tag, DrawerTrigger> : public widget

    A *widget_object* is a base class of all the classes defined as a root
    window.

    Inherits:
        * :cpp:class:`widget`
    Concept:
        * :doc:`Root Window </reference/gui/concepts/root_window>`

Public Functions
----------------

.. cpp:function:: void widget_object_rt::background(colour_t)

    Sets the color of background.

.. cpp:function:: colour_t widget_object_rt::background() const

    Returns the color of background.

.. cpp:function:: nana::string widget_object_rt::caption() const

    Returns the caption.

.. cpp:function:: void widget_object_rt::caption(const nana::string&)

    Sets the caption.

.. cpp:function:: void widget_object_rt::close() const

    Closes the window.

.. cpp:function:: void widget_object_rt::cursor(nana::gui::cursor)

    Sets a system predefined cursor.

.. cpp:function:: nana::gui::cursor widget_object_rt::cursor() const

    Retrieves the shape of cursor.

.. cpp:function:: bool widget_object_rt::empty() const

    Determines whether the manipulator is handling a window.

.. cpp:function:: bool widget_object_rt::enabled() const

    Determines whether the window is enabled for mouse and keyboard input.

.. cpp:function:: void widget_object_rt::enabled(bool)

    Sets the window whether enabled for mouse and keyboard input.

.. cpp:function:: bool widget_object_rt::focused() const

    Returns true if the widget has focus.

.. cpp:function:: void widget_object_rt::foreground(colour_t)

    Sets the color of foreground.

.. cpp:function:: colour_t widget_object_rt::foreground() const

    Returns the color of foreground.

.. cpp:function:: window widget_object_rt::handle() const

    Returns the handle of window, returns 0 if window is not created.

.. cpp:function:: void widget_object_rt::hide()

    Sets the window invisible.

.. cpp:function:: void widget_object_rt::icon(const nana::paint::image&)

    Sets the icon.

.. cpp:function:: event_handle widget_object_rt::make_event(Function)

    **template<typename Event, typename Function>**
    |br|
    Register an event callback functor.

.. warning:: The following declaration is not parsed by the documentation
    system.

.. cpp:function:: event_handle widget_object_rt::make_event(Class& object, \
    void (Concept::*)(const eventinfo&))

    **template<typename Event, typename Class, typename Concept>**
    |br|
    Register an event callback functor. The functor is a member function of
    Concept.

.. cpp:function:: void widget_object_rt::move(int x, int y)

    Moves the window to a new position.

.. cpp:function:: window widget_object_rt::owner() const

    Returns the owner of the window.

.. cpp:function:: void widget_object_rt::picture(const nana::char_t* filename)

    Sets a image file as a window background.

.. cpp:function:: void widget_object_rt::show()

    Sets the window visible.

.. cpp:function:: nana::size widget_object_rt::size() const

    Returns the size of window.

.. cpp:function:: void widget_object_rt::size(unsigned width, unsigned height)

    Sets the size of window.

.. cpp:function:: void widget_object_rt::umake_event(event_handle) const

    Deletes an event callback by a handle.


Notes
-----

None.

Examples
--------

None.

See Also
--------

* :cpp:class:`form`
