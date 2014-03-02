.. include:: /helpers.rst

.. links
    None

nana::gui::frameset
===================

.. code-block:: cpp

    nana/gui/animation.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: frameset

    The class frameset holds the frames and frame builders. This class is
    reference semantics for efficiency.

    Inherits:
        * None.
    Concept:
        * None.

Typedefs
--------

.. cpp:type:: frameset::framebuiler

    A function type for a function which builds frames.

Public Functions
----------------

.. cpp:function:: frameset::frameset()

    The default construction.

.. cpp:function:: void frameset::push_back(const nana::paint::image&)

    Inserts a picture for the frame.

.. cpp:function:: void frameset::push_back(framebuilder& fb, std::size_t length)

    Inserts a framebuilder for the frame, and the length of frames that it
    generates.

C++11
^^^^^

.. cpp:function:: void frameset::push_back(nana::paint::image&&)

    Inserts a picture for the frame.

.. cpp:function:: void frameset::push_back(framebulider&& fb, \
    std::size_t length)

    Inserts a framebuilder for the frame, and the length of frames that it
    generates.

Notes
-----

None.

Examples
--------

None.

See Also
--------

* :cpp:class:`animation`
