.. include:: /helpers.rst

.. links
    none

nana::gui::form
===============

.. code-block:: cpp

    nana/gui/wvl.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: form \
    : public widget_object\<root_tag>

    A form is a window that provides a pop-up window.

    Inherits:
        * :cpp:class:`widget_object\<root_tag>`
    Concept:
        * :doc:`Root Window </reference/gui/concepts/root_window>`

Public Functions
----------------

.. cpp:function:: form::form()

    Creates a window with default position, default size,
    default caption, and default appearance.

.. cpp:function:: form::form(const rectangle& r, \
    const appearance& = appearance())

    Creates a form that owner is desktop.

.. cpp:function:: form::form(window, const rectangle& = rectangle(), \
    const appearance& = appearace())

    Creates a form

Notes
-----

Note 1
^^^^^^

A form is different with other window widgets, its default constructor may
create the window.

Examples
--------

None.

See Also
--------

* :doc:`nana::gui::appearance </reference/gui/components/appearance>`
