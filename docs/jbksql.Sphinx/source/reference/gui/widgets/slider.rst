.. include:: /helpers.rst

.. links
.. _this article: http://nanaproject.wordpress.com/2011/03/20/clone_method/

nana::gui::slider
=================

.. code-block:: cpp

    nana/gui/widgets/slider.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: slider \
    : public widget_object\<widget_tag, DrawerTrigger>

    A slider widget user can drag it for tracking.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Typedefs
--------

.. cpp:type:: slider::renderer

    The interface for user-defined renderer.

.. cpp:type:: slider::provider

    The interface for user-defined provider.

.. cpp:type:: slider::seekdir

    Defines the slider seek direction.


Public Functions
----------------

.. cpp:function:: slider::slider()

    The default constructor.

.. cpp:function:: slider::slider(window wd, bool visible)

    Creates a slider.

.. cpp:function:: slider::slider(window wd, const rectangle& r = rectangle(), \
    bool visible = true)

    Ditto.

.. cpp:function:: nana::pat::cloneable<renderer>& slider::ext_renderer()

    Refers to the current renderer that slider is useing.

.. cpp:function:: void slider::ext_renderer(\
    const nana::pat::cloneable<renderer>&)

    Set a new renderer.

.. cpp:function:: void slider::ext_provider(\
    const nana::pat::cloneable<provider>&)

    Set a new provider.

.. cpp:function:: void slider::move_step(bool forward)

    Increase or decrease the value of slider.

.. cpp:function:: void slider::seek(seekdir::type)

    Define the direction that user can seek by using mouse.

.. cpp:function:: bool slider::transparent() const

    Returns true if the widget is transparent.

.. cpp:function:: void slider::transparent(bool)

    Enable/disable the transparent of widget.

.. cpp:function:: unsigned slider::value() const

    Return the value of slider.

.. cpp:function:: void slider::value(unsigned)

    Set the value of slider.

.. cpp:function:: bool slider::vertical() const

    Return true if the slider is vertical.

.. cpp:function:: void slider::vertical(bool)

    Set the slider whether is vertical or horizontal.

.. cpp:function:: unsigned slider::vmax() const

    Return the max value of slider.

.. cpp:function:: void slider::vmax(unsigned)

    Set the max value of slider.

Notes
-----

Note 1
^^^^^^

Definition of renderer:

.. code-block:: cpp

    class renderer
    {
    public:
        typedef nana::paint::graphics & graph_reference;

        struct bar_t
        {
            bool horizontal;
            nana::rectangle r; //the rectangle of bar
            unsigned border_size; //border size of bar
        };

        struct slider_t
        {
            bool horizontal;
            int pos;
            unsigned border;
            unsigned scale;
        };

        struct adorn_t
        {
            bool horizontal;
            nana::point bound;
            int fixedpos;
            unsigned block;
            unsigned vcur_scale; //pixels of vcur scale.
        };

        virtual ~renderer() = 0;
        virtual void background(nana::gui::window, graph_reference, bool isglass) = 0;
        virtual void adorn(nana::gui::window, graph_reference, const adorn_t&) = 0;
        virtual void adorn_textbox(nana::gui::window, graph_reference, const nana::string&, const nana::rectangle&) = 0;
        virtual void bar(nana::gui::window, graph_reference, const bar_t&) = 0;
        virtual void slider(nana::gui::window, graph_reference, const slider_t&) = 0;
    };

Note 2
^^^^^^

The *provider* provide a string that is used to display a tip label when the
user move the mouse in a slider.

Definition:

.. code-block:: cpp

    class provider
    {
    public:
        virtual ~provider() = 0;
        virtual nana::string adorn_trace(unsigned vmax, unsigned vadorn) const = 0;
    };

Note 3
^^^^^^

Refer to `this article`_ to understand how to write a user-definer renderer.
