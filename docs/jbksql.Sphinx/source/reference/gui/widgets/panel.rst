.. include:: /helpers.rst

.. links
    none

.. warning:: This page needs review.

nana::gui::panel<bool HasBackground>
====================================

.. code-block:: cpp

    nana/gui/widgets/panel.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: panel\<bool> \
    : public widget_object\<widget_tag, DrawerTrigger>

    A panel widget is used for placing some widgets. This widget is a class
    template with a bool template parameter, to determinte the widget and
    lite_widget by using the bool template parameter.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
        * :cpp:class:`widget_object\<lite_widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Template Parameters
-------------------

.. code-block:: cpp

    bool HasBackground

Desc.

Public Functions
----------------

.. cpp:function:: panel::panel()

    Default constructor.

.. cpp:function:: panel::panel(window wd, bool visible)

    Create a panel with a specified rectangle.

.. cpp:function:: panel::panel(window wd, const rectangle& r = rectangle(), \
    bool visible = true)

    Ditto.

.. cpp:function:: bool panel::transparent() const

    Returns true if it is transparent background.

.. cpp:function:: void panel::transparent(bool)

    Sets the background whether it is transparent.

Notes
-----

Note 1
^^^^^^

For panel there is no difference between
:doc:`widget_tag </reference/gui/base_of_widgets/widget_object_widget_tag>` and
:doc:`lite_widget_tag \
</reference/gui/base_of_widgets/widget_object_lite_widget_tag>` in use.

Examples
--------

None.

See Also
--------

None.
