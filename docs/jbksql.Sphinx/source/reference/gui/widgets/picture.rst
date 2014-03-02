.. include:: /helpers.rst

.. links
    none

nana::gui::picture
==================

.. code-block:: cpp

    nana/gui/widgets/picture.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: picture \
    : public widget_object\<widget_tag, DrawerTrigger>

    A progressbar widget.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Public Functions
----------------

.. cpp:function:: picture::picture()

    The default constructor.

.. cpp:function:: picture::picture(window wd, bool visible)

    Creates a picture widget.

.. cpp:function:: picture::picture(window wd, const rectangle& r = rectangle(), \
    bool visible = true)

    Ditto.

.. cpp:function:: void picture::load(const nana::char_t* filename)

    Loads a image file.

.. cpp:function:: void picture::load(const nana::paint::image& img)

    Loads a image file.

.. cpp:function:: void picture::bgstyle(bool stretchable, nana::arrange arg, \
    int beg, int end)

    Sets the background image style, the parameter arg, beg and end will be
    ignored if stretchable is false. The parameter arg stands for stretching the
    image horizontally or vertically.The beg and end parameter specify the
    stretchy area of image.

.. cpp:function:: void picture::set_shadow_background(nana::color_t beg, \
    nana::color_t end, bool horizontal)

    Fills a gradual change color in background.

.. cpp:function:: void picture::transparent(bool)

    Enables or disables the transparent background.

.. cpp:function:: bool picture::transparent() const

    Returns true if the background is transparent.

Notes
-----

None.

Examples
--------

None.

See Also
--------

None.
