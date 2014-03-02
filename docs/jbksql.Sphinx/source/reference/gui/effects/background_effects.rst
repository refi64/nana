.. include:: /helpers.rst

.. links
    None

background effects
==================

.. code-block:: cpp

    nana/gui/wvl.hpp

Nana defines a set of API for background effects. With background effects, we
can create a good-looking user interface.

Effect Mode
-----------

There are two kind of effect modes:

Basic Mode
    The basic mode creates a transparent background. Such as the transparent
    label widget.

Blending Mode
    The blending mode creates a semi-transparent widget. For some widgets, they
    do not support the transparent background, such as button, because they
    always draw the background by themselves, and the blending mode is used to
    make these widgets semi-transparent.

.. image:: /_static/images/effect_mode.png
    :align: center

Some of drawers may detect the current effect mode, such as lable and checkbox,
when thay are basic mode, these drawers only draw the foreground(such as text
and borders), the background is not drawn, and the transparent background will
be kept. Other drawers do not detect the current effect mode, and they always
draw the background if they are basic mode, and the transparent background will
be covered, so the basic mode does not make sense for the widgets which have
these drawers.

APIs (in namespace nana::gui::API)
----------------------------------

.. cpp:function:: void API::effects_bground(window wd, \
    const effects::bground_factory_interface&, double fade_rate)

    Sets an effect for a specified widget. The fade_rate is in the range
    [0, 1.0], and if it is zero, the effect mode is basic mode, otherwise the
    effect mode is blending mode.

.. cpp:function:: void API::effects_bground_remove(window wd)

    Removes the effect of the specified widget.

APIs for C++03
^^^^^^^^^^^^^^

.. cpp:function:: bground_mode::t API::effects_bground_mode(window wd)

    Constructor.

APIs for C++11
^^^^^^^^^^^^^^

.. cpp:function:: bground_mode API::effects_bground_mode(window wd)

    Constructor.

Effects Factory (in namespace nana::gui::effects)
-------------------------------------------------

The library defines some effects factories to create effects. The users should
not take care about the object of factories.

.. cpp:function:: effects::bground_transparent(std::size_t percent)

    Creates a transparent background effect factory. The percent is in the range
    of [0, 100], it stands for the fade rate of the background color which is
    blended with the background graphic buffer.

.. cpp:function:: effects::bground_blur(std::size_t radius)

    Creates a transparent and blur background effect factory. The radius is used
    to specify the number of pixels to be blurred.

Notes
-----

None.

Examples
--------

Background effects
^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

    #include <nana/gui/wvl.hpp>
    #include <nana/gui/widgets/button.hpp>
    #include <nana/gui/widgets/label.hpp>
    #include <nana/gui/place.hpp>

    int main()
    {
        using namespace nana::gui;

        form fm;

        button btn(fm, L"Normal Button");

        button btn0(fm, L"Basic Mode");

        //It is basic mode when the third parameter of effects_bground() is specified by zero.
        API::effects_bground(btn0, effects::bground_transparent(0), 0);

        button btn1(fm, L"Blend Mode: Transparent");
        API::effects_bground(btn1, effects::bground_transparent(0), 0.5);

        button btn2(fm, L"Blend Mode: Blur");
        API::effects_bground(btn2, effects::bground_blur(2), 0.5);

        label lb0(fm, L"Normal Label");
        lb0.foreground(0xFFFFFF);

        label lb1(fm, L"Basic Mode: Transparent");
        lb1.foreground(0xFFFFFF);
        //Equal to lb1.transparent(true);
        API::effects_bground(lb1, effects::bground_transparent(0), 0);

        label lb2(fm, L"Basic Mode: Transparent and blend with its background color");
        lb2.foreground(0xFFFFFF);
        lb2.background(0xFF0000);
        API::effects_bground(lb2, effects::bground_transparent(10), 0);

        label lb3(fm, L"Basic Mode: Blur");
        lb3.foreground(0xFFFFFF);
        API::effects_bground(lb3, effects::bground_blur(2), 0);

        place pl(fm);
        pl.div("<><weight=80% abc vertical><>");
        pl.field("abc")<<5<<btn<<5<<btn0<<5<<btn1<<5<<btn2<<5<<lb0<<5<<lb1<<5<<lb2<<5<<lb3<<5;
        pl.collocate();

        fm.show();

        nana::paint::image img(L"image.bmp");
        drawing dw(fm);
        dw.draw([&img](nana::paint::graphics & graph)
        {
            img.paste(graph, 0, 0);
        });
        dw.update();

        exec();
    }

.. image:: /_static/images/effects_bground_show.png
    :align: center

This program shows the behaviors of these effects. The button does not support
the transparent, so the basic mode does not make sense for the button.

See Also
--------

None.
