.. include:: /helpers.rst

.. links
    none

nana::gui::frame
================

.. code-block:: cpp

    nana/gui/widgets/frame.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: frame \
    : public widget_object\<frame_tag>

    A frame widget is a container for the system native windows,
    it provides an approach of displaying a control that is not written
    with Nana.GUI in a Nana.GUI window.

    Inherits:
        * :cpp:class:`widget_object\<frame_tag>`
    Concept:
        * :doc:`Frame Window </reference/gui/concepts/frame_window>`

Public Functions
----------------

.. cpp:function:: frame::frame()

    Default constructor.

.. cpp:function:: frame::frame(window wd, bool visible)

    Creates a frame.

.. cpp:function:: frame::frame(window wd, const rectangle& r = rectangle(), \
    bool visible = true)

    Ditto.

.. cpp:function:: bool frame::insert(nana::gui::native_window_type wnd)

    Inserts a platform native window.

.. cpp:function:: nana::gui::native_window_type frame::element(unsigned index)

    Returns the child window through index.

.. cpp:function:: nana::gui::native_window_type frame::container() const

    Returns the frame container native window handle.

Notes
-----

Note 1
^^^^^^

*nana::gui::native_window_type* is a type of system handle of windows.

Note 2
^^^^^^

All the children windows of a *nana::gui::frame* is topmost to Nana.GUI windows.

Examples
--------

Displaying a Windows Edit Control.
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

    nana::gui::frame frame(parent, 0, 0 200, 100);
        HWND frame_handle = reinterpret_cast<HWND>(frame.container());
        HWND edit = ::CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", L"Test",
                                      WS_CHILD | WS_VISIBLE | WS_BORDER, 0, 0, 200, 100,
                                      frame_handle, 0, ::GetModuleHandle(0), 0);
        if(edit)
            frame.insert(edit);

See Also
--------

None.
