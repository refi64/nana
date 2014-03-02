.. include:: /helpers.rst

.. links
.. _Typedefs: #id1

nana::gui::button
=================

.. code-block:: cpp

    nana/gui/widgets/button.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: button \
    : public widget_object\<widget_tag, DrawerTrigger>

    A Button widget.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Typedefs
--------

C++03
^^^^^

.. cpp:type:: button::state::t

    .. code-block:: cpp

        struct state
        {
            enum t { normal, focused, highlight, pressed, disabled };
        };

    Defines the state of button actions.

C++11
^^^^^

.. cpp:type:: button::state

    .. code-block:: cpp

        enum class state
        {
            normal, focused, highlight, pressed, disabled
        };

    Defines the state of button actions.


Public Functions
----------------

.. cpp:function:: button::button()

    The default constructor.

.. cpp:function:: button::button(nana::gui::window wd, bool visible)

    Construct a button in specified window

.. cpp:function:: button::button(nana::gui::window wd, \
    const nana::string& text, bool visible = true)

    Ditto.

.. cpp:function:: button::button(nana::gui::window wd, \
    const nana::char_t* text, bool visible = true)

    Ditto.

.. cpp:function:: button::button(nana::gui::window wd, \
    const rectangle& r = rectangle(), bool visible = true)

    Ditto.

.. cpp:function:: void button::icon(const nana::paint::image&)

    Sets an icon image.

    const nana::paint::image&
        See :cpp:class:`image`

.. cpp:function:: void button::image(const nana::char_t* file)

    Sets a background image.

.. cpp:function:: void button::image(const nana::paint::image& imgobj)

    Sets a background image.

.. cpp:function:: void button::image_valid_area(nana::arrange arg, \
    const nana::rectangle& r)

    Sets an area of the image which is painted as background.

.. cpp:function:: void button::image_stretch(nana::arrange arg, \
    int beg, int end)

    Sets the stretch part of image. The image is stretched when the size of
    button is changed.

.. cpp:function:: void button::enable_pushed(bool eb)

    Enables or disables the pushed state of the button.

.. cpp:function:: bool button::pushed() const

    Returns the pushed state of the button.

.. cpp:function:: void button::pushed(bool ps)

    Sets the pushed state.

.. cpp:function:: void button::omitted(bool eb)

    Enables/Disables omitting displaying the caption if the text is too long.

.. cpp:function:: void button::enable_focus_color(bool eb)

    Enables/Disables showing the caption with a special color to indicate the
    button is focused.

C++03
^^^^^

.. cpp:function:: void button::image_enable(state::t, bool)

    Enable/disable a state. the button will be drawn defaultly if the state is
    disabled.

.. cpp:function:: void button::image_join(state::t to, state::t from)

    The state from joins the state to.

C++11
^^^^^

.. cpp:function:: void button::image_enable(state, bool)

    Enable/disable a state. the button will be drawn defaultly if the state is
    disabled.

.. cpp:function:: void button::image_join(state to, state from)

    The state from joins the state to.

Notes
-----

Note 1
^^^^^^

The background image of button horizontally split into 5 parts and each part
indicates one state. If the background of button is some area in the image, the
area can be specified by using image_valid_area(). For example. There is an
image that size is 520 * 70 pixels, and a button background image is just
470 * 23 pixels at position (0, 0).

.. image:: /_static/images/button_image.png
    :align: center

.. code-block:: cpp

    int main()
    {
        nana::gui::form form;
        nana::gui::button btn(form, 5, 5, 94, 23);
        btn.image(STR("button_image.bmp"));
        btn.image_valid_area(nana::arrange::horizontal, nana::rectangle(0, 0, 94 * 5, 23));
        form.show();
        nana::gui::exec();
    }

The button splits the area of image into 5 parts for each state. The order of
parts are same with the order of definition of elements of enum in type state
(refers to `Typedefs`_).

.. image:: /_static/images/button_state_transf.png
    :align: center

Note 2
^^^^^^

A image state can be disabled by calling *image_enable()* method. If a state is
disabled, the button would averagely split the area of image into parts for each
enabled state.

Examples
--------

None.

See Also
--------

None.
