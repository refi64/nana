.. include:: /helpers.rst

.. links
    None

nana::gui::animation
====================

.. code-block:: cpp

    nana/gui/animation.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: animation

    The class animation provides an easy way to display an animation or create a
    animated GUI.

    Inherits:
        * None.
    Concept:
        * None.

Public Functions
----------------

.. cpp:function:: animation::animation()

    The default construction.

.. cpp:function:: void animation::push_back(const frameset&)

    Inserts frames at the end.

.. cpp:function:: void animation::looped(bool enable)

    Enables or disables the animation repeating playback.

.. cpp:function:: void animation::play()

    Plays the animation.

.. cpp:function:: void animation::pause()

    Pause the animation.

.. cpp:function:: void animation::output(nana::gui::window wd, \
    const nana::point& pos)

    Outputs the animation to a specifed window and position.

Notes
-----

None.

Examples
--------

Using nana::gui::animation
^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

    #include <nana/gui/wvl.hpp>
    #include <nana/gui/animation.hpp>

    int main()
    {
        using namespace nana::gui;

        //Build frames
        frameset fset;
        fset.push_back("a_pic0.bmp");
        fset.push_back("a_pic1.bmp");
        fset.push_back("a_pic2.bmp");

        //A widget to display animation.
        form fm;
        fm.show();

        animation ani;
        ani.push_back(fset);
        ani.output(fm, nana::point());
        ani.looped(true);
        ani.play();

        exec();
    }

See Also
--------

* :cpp:class:`frameset`
