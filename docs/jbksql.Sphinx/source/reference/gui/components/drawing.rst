.. include:: /helpers.rst

.. links
.. _Typedefs: #id1

nana::gui::drawing
==================

.. code-block:: cpp

    nana/gui/wvl.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: drawing \
    : private nana::noncopyable

    The class nana::gui::drawing provides a method to draw something on the
    widget. It specifies a drawing method and everytime the widget refreshes,
    the drawing method will be employed.

    Inherits:
        * nana::noncopyable
    Concept:
        * None

Typedefs
--------

.. cpp:type:: drawing::draw_fn_t

    A function object type, wappers a function to draw.
    prototype is

    .. code-block:: cpp

        void(nana::paint::graphics&)

.. cpp:type:: drawing::diehard_t

    A handle to a drawing method

Public Functions
----------------

.. cpp:function:: drawing::drawing(nana::gui::widget& widget)

    Create a drawing object for a widget

.. cpp:function:: drawing::~drawing()

    Destruction

.. cpp:function:: void drawing::bitblt(int x, int y, unsigned width, \
    unsigned height, const nana::paint::graphics& graph, int srcx, int srcy)

    Copy a buffer from graph begin(srcx, srcy) to the position(x, y).

.. cpp:function:: void drawing::clear()

    Erases all.

.. cpp:function:: void drawing::draw(const draw_fn_t&)

    Draws things that are defined by the object of draw_fn_t.

.. cpp:function:: diehard_t drawing::draw_diehard(const draw_fn_t&)

    Draws things that are defined by the object of draw_fn_t. The drawing
    methods defined by the object of draw_fn_t are not deleted when clear()
    is called.

.. cpp:function:: void drawing::erase(diehard_t)

    Erases a diehard drawing method.

.. cpp:function:: bool drawing::empty() const

    Returns true if the drawing object is invalid.

.. cpp:function:: void drawing::line(int x, int y, int x2, int y2, \
    unsigned color)

    Draws a line from point(x, y) to point(x2, y2) with color.

.. cpp:function:: void drawing::string(int x, int y, unsigned color, \
    const nana::char_t* text)

    Draws a string of text at (x, y) with specified color.

.. cpp:function:: void drawing::rectangle(int x, int y, unsigned width, \
    unsigned height, unsigned color, bool solid)

    Draws a rectangle.

C++11
^^^^^

.. cpp:function:: void drawing::draw(draw_fn_t&&)

    Draws things that are defined by the object of draw_fn_t.

.. cpp:function:: diehard_t drawing::draw_diehard(draw_fn_t&&)

    Draws things that are defined by the object of draw_fn_t. The drawing
    methods defined by the object of draw_fn_t are not deleted when clear() is
    called.

Notes
-----

None.

Examples
--------

Draw a 10 X 10 red rectangle at the point(5, 5)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

**C++03**

.. code-block:: cpp

    #include <nana/gui/wvl.hpp>

    void draw_method(nana::paint::graphics& graph)
    {
        graph.rectangle(nana::rectangle(5, 5, 10, 10), 0xFF0000, true);
    }

    int main()
    {
        using namespace nana::gui;

        form fm;
        drawing dw(fm);
        dw.draw(draw_method);

        dw.update();
        fm.show();
        exec();
    }

**C++11**

.. code-block:: cpp

    #include <nana/gui/wvl.hpp>

    int main()
    {
        using namespace nana::gui;

        form fm;
        drawing dw(fm);
        dw.draw([](nana::paint::graphics& graph)
        {
            graph.rectangle(nana::rectangle(5, 5, 10, 10), 0xFF0000, true);
        });

        dw.update();
        fm.show();
        exec();
    }

See Also
--------

None.
