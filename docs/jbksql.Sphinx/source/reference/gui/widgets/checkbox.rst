.. include:: /helpers.rst

.. links
    none

nana::gui::checkbox
===================

.. code-block:: cpp

    nana/gui/widgets/checkbox.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: checkbox \
    : public widget_object\<widget_tag, DrawerTrigger>

    A checkbox widget.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Public Functions
----------------

.. cpp:function:: checkbox::checkbox()

    Default Constructor.

.. cpp:function:: checkbox::checkbox(window wd, bool visible)

    Create a checkbox widget in a specified window
    and allow to to turn on/off the visibility.

.. cpp:function:: checkbox::checkbox(window wd, const nana::string& title, \
    bool visible = true)

    Ditto.

.. cpp:function:: checkbox::checkbox(window wd, const nana::char_t* title, \
    bool visible = true)

    Ditto.

.. cpp:function:: checkbox::checkbox(window wd, \
    const rectangle& r = rectangle(), bool visible = true)

    Ditto.

.. cpp:function:: bool checkbox::checked() const

    Determines whether the checkbox is checked.

.. cpp:function:: void checkbox::check(bool)

    Check/Uncheck.

.. cpp:function:: void checkbox::radio(bool)

    Enables or disables the radio check mode.

.. cpp:function:: void checkbox::react(bool)

    Enables the reverse check while clicking on the checkbox.

.. cpp:function:: bool checkbox::transparent() const

    Returns true if the checkbox background is transparent mode.

.. cpp:function:: void checkbox::transparent(bool value)

    Switch the checkbox to the transparent background mode.

Notes
-----

Note 1
^^^^^^

It is not efficient when the background mode is transparent.

Note 2
^^^^^^

With the radio mode, users make a choice among a set of mutually exclusive,
related options. Users can choose one and only one option. There is a helper
class manages checkboxs for radio mode, see :cpp:class:`radio_group`.

Examples
--------

None.

See Also
--------

* :cpp:class:`radio_group`
