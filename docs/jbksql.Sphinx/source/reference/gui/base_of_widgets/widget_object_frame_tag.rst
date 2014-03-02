.. include:: /helpers.rst

.. links
    none

.. warning:: This page needs review.

widget_object<frame_tag>
========================

.. code-block:: cpp

    nana/gui/widgets/widget.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: widget_object\<frame_tag> : public widget

    A *widget_object<frame_tag>* is a base class of all the classes defined as
    a frame window.

    Inherits:
        * :cpp:class:`widget`
    Concept:
        * :doc:`Frame Window </reference/gui/concepts/frame_window>`

Public Functions
----------------

.. cpp:function:: void widget_object_ft::background(colour_t)

    Sets the color of background.

.. cpp:function:: colour_t widget_object_ft::background() const

    Returns the color of background.

.. cpp:function:: nana::string widget_object_ft::caption() const

    Returns the caption.

.. cpp:function:: void widget_object_ft::caption(const nana::string&)

    Sets the caption.

.. cpp:function:: void widget_object_ft::cursor(nana::gui::cursor)

    Sets a system predefined cursor.

.. cpp:function:: nana::gui::cursor widget_object_ft::cursor() const

    Retrieves the shape of cursor.

.. cpp:function:: void widget_object_ft::close() const

    Closes the window.

.. cpp:function:: bool widget_object_ft::create(window parent, bool visible)

    Creates the frame widget.

.. cpp:function:: bool widget_object_ft::create(window parent, \
    const nana::rectangle & = nana::rectangle(), bool visible = true)

    Creates the frame widget.

.. cpp:function:: bool widget_object_ft::empty() const

    Determines whether the manipulator is handling a window.

.. cpp:function:: bool widget_object_ft::enabled() const

    Determines whether the window is enabled for mouse and keyboard input.

.. cpp:function:: void widget_object_ft::enabled(bool)

    Sets the window whether enabled for mouse and keyboard input.

.. cpp:function:: bool widget_object_ft::focused() const

    Returns true if the widget has focused.

.. cpp:function:: void widget_object_ft::foreground(colour_t)

    Sets the color of foreground.

.. cpp:function:: colour_t widget_object_ft::foreground() const

    Returns the color of foreground.

.. cpp:function:: void widget_object_ft::hide()

    Sets the window invisible.

.. cpp:function:: event_handle widget_object_ft::make_event(Function)

    template<typename Event, typename Function>
    |br|
    Register an event callback functor.

.. warning:: The following declaration is not parsed by Sphinx.

.. cpp:function:: event_handle widget_object_ft::make_event(Class& object, \
    void (Concept::*)(const eventinfo&))

    **template<typename Event, typename Class, typename Concept>**
    |br|
    Register an event callback functor. The functor is a member function of
    Concept.

.. cpp:function:: void widget_object_ft::move(int x, int y)

    Moves the window to a new position.

.. cpp:function:: void widget_object_ft::picture(const nana::char_t* filename)

    Sets a image file as a window background.

.. cpp:function:: void widget_object_ft::show()

    Sets the window visible.

.. cpp:function:: nana::size widget_object_ft::size() const

    Returns the size of window.

.. cpp:function:: void widget_object_ft::size(unsigned width, unsigned height)

    Sets the size of window.

.. cpp:function:: void widget_object_ft::umake_event(event_handle) const

    Deletes an event callback by a handle.

Notes
-----

None.

Examples
--------

None.

See Also
--------

None.
