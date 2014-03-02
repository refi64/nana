.. include:: /helpers.rst

.. links
    none

nana::gui::radio_group
======================

.. code-block:: cpp

    nana/gui/widgets/checkbox.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: radio_group

    This is a helper class for managing checkboxs in radio mode.

    Inherits:
        * None
    Concept:
        * None

Public Functions
----------------

.. cpp:function:: radio_group::~radio_group()

    The destructor.

.. cpp:function:: void radio_group::add(nana::gui::checkbox&)

    Adds a checkbox as an option of radio choice.

.. cpp:function:: size_t radio_group::checked() const

    Retrieves the index of checkbox which is checked.

Notes
-----

None.

Examples
--------

None.

See Also
--------

* :doc:`checkbox </reference/gui/widgets/checkbox>`
