.. include:: /helpers.rst

.. links
    None

nana::gui::dragger
==================

.. code-block:: cpp

    nana/gui/dragger.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: dragger

    The dragger is a helper class that allows the user to drag windows. If a
    user presses the mouse on the specified window and moves the mouse, the
    specified window is dragged.

    Inherits:
        * None.
    Concept:
        * None.

Public Functions
----------------

.. cpp:function:: dragger::dragger()

    The default constructor.

.. cpp:function:: dragger::~dragger()

    The destructor.

.. cpp:function:: void dragger::target(nana::gui::window wd)

    Sets the windows to be moved.

.. cpp:function:: void dragger::trigger(nana::gui::window wd)

    Sets the trigger window that the mouse drags it.

Notes
-----

None.

Examples
--------

The drag target window and trigger window can be set more than once
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. code-block:: cpp

    #include <nana/gui/wvl.hpp>
    #include <nana/gui/widgets/button.hpp>
    #include <nana/gui/dragger.hpp>

    int main()
    {
        using namespace nana::gui;
        form fm;
        button btn(fm, 10, 10, 100, 20);
        btn.caption(STR("Drag Me"));

        form & adherent = form_loader<form>()();
        adherent.show();

        dragger dg;
        dg.target(fm);
        dg.target(adherent);
        dg.trigger(btn);
        dg.trigger(adherent);

        fm.show();
        exec();
    }

This example sets the btn and the adherent to be triggers, and dragger will move
the form and adherent when the user is dragging the trigger widgets.

See Also
--------

None.
