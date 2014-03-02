.. include:: /helpers.rst

.. links
    None

nana::gui::gird (deprecated)
============================

.. code-block:: cpp

    nana/gui/layout.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: gird

    The gird automatically lays out widgets in a gird.

    Inherits:
        * None.
    Concept:
        * None.

Public Functions
----------------

.. cpp:function:: gird::gird(widget& wd)

    Constructs a gird attached to a window..

.. cpp:function:: gird* gird::push(unsigned blank, unsigned scale)

    Creates a child gird.

.. cpp:function:: void gird::push(nana::gui::window wd, unsigned blank, \
    unsigned scale)

    Pushs a wd

.. cpp:function:: gird* gird::add(unsigned blank, unsigned scale)

    Create a child gird.

.. cpp:function:: void gird::add(nana::gui::window wd, unsigned blank, \
    unsigned scale)

    Pushs a wd.

.. cpp:function:: void gird::fasten(nana::gui::window)

    Fasten a window to the gird.

Notes
-----

None.

Examples
--------

A form with a button in gird layout
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

    #include<nana/gui/wvl.hpp>
    #include<nana/gui/widgets/button.hpp>
    #include<nana/gui/layout.hpp>

    int main()
    {
        using namespace nana::gui;
        form fm;
        button btn(fm);
        btn.caption(STR("Button"));

        gird gdobj(fm);


        gird * child_gird_a = gdobj.add(0, 0);

        //Create a child_gird_b, 80 pixels in width
        //10 pixels in blank between child_gird_a and child_gird_b.
        gird * child_gird_b = gdobj.add(10, 80);

        //Create a child_gird_c, 10 pixels in width
        gird * child_gird_c = gdobj.add(0, 10);

        //Lays out the button, 10 pixels blank off its top, 20 pixels in height.
        child_gird_b->push(btn, 10, 20);
        fm.show();
        exec();
    }

.. image:: /_static/images/gird.png
    :align: center

See Also
--------

None.
