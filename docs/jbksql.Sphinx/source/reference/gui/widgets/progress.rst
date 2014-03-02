.. include:: /helpers.rst

.. links
    none

nana::gui::progress
===================

.. code-block:: cpp

    nana/gui/widgets/progress.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: progress \
    : public widget_object\<widget_tag, DrawerTrigger>

    A progressbar widget.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Public Functions
----------------

.. cpp:function:: progress::progress()

    The default constructor.

.. cpp:function:: progress::progress(window wd, bool visible)

    Creates a progress indicator

.. cpp:function:: progress::progress(window wd, \
    const rectangle& r = rectangle(), bool visible = true)

    Ditto.

.. cpp:function:: unsigned progress::value() const

    Returns the value of progress indicator

.. cpp:function:: void progress::value(unsigned val)

    Set the value of progress indicator

.. cpp:function:: void progress::inc()

    Increases the value by itself

.. cpp:function:: unsigned progress::amount() const

    Returns the amount value of progress indicator

.. cpp:function:: unsigned progress::amount(unsigned value)

    Set the amount value of progress indicator

.. cpp:function:: void progress::unknown(bool)

    Enables/disables the unknown progress mode.

.. cpp:function:: bool progress::unknown() const

    Returns true if the progress is unknown mode.

Notes
-----

Note 1
^^^^^^

The progressbar has two modes, unknown and known. the known mode means the
progressbar has a specific amount value. The progressbar in the unknown mode
ignores the amount value and it always scroll the progress block when its
value has changed.

Examples
--------

None.

See Also
--------

None.
