.. include:: /helpers.rst

.. links
.. _Note 1: #id1
.. _Note 2: #id2
.. _Note 3: #id3
.. _Note 4: #id4
.. _Note 5: #id5
.. _Note 6: #id6
.. _Note 7: #id7
.. _Note 8: #id8
.. _Note 9: #id9

Application Programming Interfaces
==================================

.. code-block:: cpp

    nana/gui/wvl.hpp

.. cpp:namespace:: nana::gui::API

Nana.GUI defines some functions for programming, these functions are all defined
in the namespace *nana::gui::API*.

Public Functions
----------------

.. cpp:function:: bool API::calc_screen_point(nana::gui::window wnd, \
    nana::point& pos)

    Converts a window coordinates of a specified point to the screen
    coordinates.

    nana::gui::window wnd
        A handle to the window whose point is used for the conversion.

    nana::point& pos
        Specifes a window coordinates of the point to be converted. And the new
        screen coordinates are copied into pos if function succeeds.

    Returns
        If the function succeeds, it returns true, otherwise it returns false.

.. cpp:function:: bool API::calc_window_point(nana::gui::window wnd, \
    nana::point& pos)

    Converts a screen coordinates of a specifed point to the window coordinates.

    nana::gui::window wnd
        A handle to the window whose coordinates are to be converted from screen
        coordinates.

    nana::point& pos
        Specifies the screen coordinates of the point to be converted. And the
        new window coordinates are copied into pos if function succeeds.

    Returns
        If the function succeeds, it returns true, otherwise it returns false.

.. cpp:function:: nana::gui::window API::capture_window(nana::gui::window wd, \
    bool enable)

    Enable or disable the window to grab the mouse input.

    nana::gui::window wd
        A handle to the window which enables or disables the grab of mouse
        input.

    bool enable
        Indicates whether to enable or disable the capture of window. If the
        parameter is true, the window captures the mouse input. If parameter is
        false, the window stops capturing the mouse input.

    Returns
        This function returns the previous captured window. If there is not a
        captured window, it returns 0.

.. cpp:function:: void API::capture_ignore_children(bool ignore)

    Enable or disable the captured window whether to redirect the mouse input
    to its children when the mouse is over its children.

    bool ignore
        indicates whether to ignore the redirection of mouse input to its
        children window when the mouse is over its children.

.. cpp:function:: void API::eat_tabstop(nana::gui::window wnd)

    Sets a window can eat the tabstop. (`Note 1`_)

    nana::gui::window wnd
        A handle to the window to be set.

.. cpp:function:: bool API::empty_window(nana::gui::window wnd)

    Determines whether a window exists.

    nana::gui::window wnd
        A handle to the window to test.

    Returns
        If the window exists it returns true, otherwise false.

.. cpp:function:: bool API::is_window_zoomed(nana::gui::window wd, \
    bool ask_for_max)

    Determines whether the specified window is maximized or minimized.

    nana::gui::window wd
        A handle to the window to be tested.

    bool ask_for_max
        Indicates whether the maximun or minimun to be tested.

    Returns
        If the window is maximized and ask_for_max is true, it returns true.
        If the window is minimized and ask_for_max is false, it returns true.

.. cpp:function:: void API::lazy_refresh()

    Tells Nana.GUI to copy the buffer of event window to screen after the event
    is processed. (`Note 2`_)

.. cpp:function:: nana::rectangle API::make_center(unsigned width, \
    unsigned height)

    Retrieves a rectangle which is in the center of screen.

    unsigned width
        Specifies the width of the rectangle.

    unsigned height
        Specifies the height of the rectangle.

    Returns
        It returns a rectange with the specifies the width and height, and the
        rectangle is position in the center of the screen.

.. cpp:function:: void API::modal_window(nana::gui::window wd)

    Blocks the routine till the specified window is closed. (`Note 3`_)

    nana::gui::window wd
        A handle to the window to be a modal window.

.. cpp:function:: nana::gui::window API::move_tabstop(nana::gui::window wnd, \
    bool next)

    Sets the focus to the window which tabstop is near by the specified window.

    nana::gui::window wnd
        A handle to the window which the focus is moved from it.

    bool next
        Move the focus to the next tabstop window or the previous tabstop
        window.

    Returns
        A window handle to the new focus window indicates success.
        0 indicates failure. If there is noly one tabstop window, 0 indicates no
        change to the focus.

.. cpp:function:: void API::refresh_window(nana::gui::window wd)

    Refreshs the window and display it immediately. (`Note 4`_)

    nana::gui::wd
        A handle to the window which to be refreshed.

.. cpp:function:: void API::refresh_window_tree(nana::gui::window wd)

    Refreshs the specified window and all it's children windows, then display it
    immediately. (`Note 4`_)

    nana::gui::wd
        A handle to the window which to be refreshed.

.. cpp:function:: nana::gui::native_window_type API::root(nana::gui::window wnd)

    Retrieves the native window of a Nana.GUI window. (`Note 5`_)

    nana::gui::window wnd
        A handle to the Nana.Gui window.

    Returns
        If the function succeeds, the return value is the native window handle
        to the Nana.GUI window. If the function fails, the return value is zero.

.. cpp:function:: void API::show_window(nana::gui::window wnd, bool show)

    Sets a window visible state.

    nana::gui::window wnd
        A handle to the window to set.

    bool show
        Visible state.

.. cpp:function:: void API::tabstop(nana::gui::window wnd)

    Sets a window that owns the tabstop. (`Note 6`_)

    nana::gui::window wnd
        A handle to the window which is to be set.

.. cpp:function:: void API::take_active(nana::gui::window wnd, \
    bool active, nana::gui::window receiver)

    Sets a window active state, if a window's active state is false, the window
    will not obtain the focus while a mouse clicks on it. (`Note 7`_)

    nana::gui::window wnd
        A handle to a window which will be set.

    bool active
        State.

    nana::gui::window receiver
        Forwards the focus to the receiver if active is false.

.. cpp:function:: bool API::track_window_size(nana::gui::window wd, \
    const nana::size& sz, bool true_fo_max)

    Sets the minimum or maximum tracking size to the specified window.
    (`Note 8`_)

    nana::gui::window wd
        A handle to the window to be set.

    const nana::size& sz
        Tracking size.

    bool true_for_max
        indicates the minimum or maximum tracking size to set. If the parameter
        is true, the maximun tracking size is set. If the parameter is false,
        the minimum tracking size is set.

    Returns
        If the function succeeds, it reutrns true. If the window is not exist,
        or minimun tracking size is greater than maximun tracking size, or
        maximum tracking size is less than minimum tracking size, it returns
        false.

.. cpp:function:: void API::update_window(nana::gui::window wnd)

    Copies the graphics buffer to the screen. (`Note 9`_)

    nana::gui::window wnd
        A handle to the window whose graphics buffer will be copied to the
        screen.

.. cpp:function:: bool API::visible(nana::gui::window wnd)

    Determines whether a window is visible.

    nana::gui::window wnd
        A handle to the window to test.

    Returns
        If the window is visible, the return value is true.
        If the window is invisible or the window handle identifies a
        non-existing window, the return value is false.

Notes
-----

Note 1
^^^^^^

If a window can eat tabstop, the focus is not to be changed when Tab key is
pressed, and a *key_char* event with tab will be generated.

Note 2
^^^^^^

This function only works for a drawer_trigger, when a drawer_trigger receives an
event, after drawing, a drawer_trigger should call lazy_refresh to tell the
Nana.GUI to refresh the window to the screen after the event process finished.

Note 3
^^^^^^

This function handles all interaction with the user while the specified window
is active, the user can not interact with other windows until the specified
window is closed. This is what makes the specified window modal.

Note 4
^^^^^^

This function only call the refresh method of its *drawer_trigger*.

Note 5
^^^^^^

The native window type is platform-dependent. Under Microsoft Windows, a
conversion can be employed between *nana::gui::native_window_type* and *HWND*
through *reinterpret_cast* operator. Under X System, a conversion can be
employed between *nana::gui::native_window_type* and Window through
*reinterpret_cast* operator.

Note 6
^^^^^^

If a window owns the tabstop, the window can get keyboard focus while pressing
the Tab key to switch the focus.

Note 7
^^^^^^

If active is false, the window will not obtain the focus anymore. If the
receiver is not NULL and active is false, the window will forward the focus to
the receiver. For example, a textbox has a scrollbar and the active of scrollbar
is false and receiver is textbox, and therefore the focus is always in the
textbox and caret is always active while mouse is dragging on or clicking on the
scrollbar, the focus does not change to the scrollbar.

Note 8
^^^^^^

The maximum tracking size is the largest window size that can be produced by
using the borders to size the window or *using window_size()*.
The minimum tracking size is the smallest window size that can be produced by
using the borders to size the window or using *window_size()*.

Note 9
^^^^^^

This function is useful when implementing a widget. The difference to
*refresh_immediately* is that *update_window* only copies the graphics buffer to
the screen.

Examples
--------

None.

See Also
--------

None.
