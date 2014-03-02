.. include:: /helpers.rst

.. links
    None

.. warning:: This page needs review.

nana::rectangle
===============

.. code-block:: cpp

    nana/basic_types.hpp

.. cpp:namespace:: nana

.. cpp:class:: rectangle

    A structure defines a rectangle with coordinates and size.

    Inherits:
        * None
    Concept:
        * None

Data Members
------------

.. cpp:member:: int rectangle::x

    The x-axis of rectangle.

.. cpp:member:: int rectangle::y

    The y-axis of rectangle.

.. cpp:member:: unsigned rectangle::width

    The width of rectangle.

.. cpp:member:: unsigned rectangle::height

    The height of rectangle.


Public Functions
----------------

.. cpp:function:: rectangle::rectangle()

    The default construction, a zero-size rectangle at (0, 0).

.. cpp:function:: rectangle::rectangle(int x, int y, unsigned width, \
    unsigned height)

    Constructs a rectangle with specified coordinates and size.

.. cpp:function:: rectangle::rectangle(const size&)

    Constructs a rectangle with specified size at coordinate(0, 0).

.. cpp:function:: rectangle::rectangle(const point& p, const size& s)

    Constructs a rectangle with specified coordinates and size.

.. cpp:function:: rectangle & rectangle::pare_off(int pixels)

    Pares the specified pixels off the rectangle. It's equal to:

    .. code-block:: cpp

        x += pixels; y + pixels; width -= (pixels << 1);
        height -= (pixels << 1);

.. cpp:function:: bool rectangle::operator==(const rectangle& r) const

    Checks whether the rectangle is equal to r.

.. cpp:function:: bool rectangle::operator!=(const rectangle& r) const

    Checks whether the rectangle is not equal to r.

Notes
-----

None.

Examples
--------

None.

See Also
--------

None.
