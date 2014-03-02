.. include:: /helpers.rst

.. links
    none

.. warning:: This page needs review.

nana::gui::label
================

.. code-block:: cpp

    nana/gui/widgets/label.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: label \
    : public widget_object\<widget_tag, DrawerTrigger>

    A label widget can dispaly a text string and images.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Typedefs
--------

C++03
^^^^^

.. cpp:type:: label::t

    .. code-block:: cpp

        struct command
        {
            enum t { enter, leave, click };
        };

    Defines the event type for format listener.

C++11
^^^^^

.. cpp:type:: label::command

    .. code-block:: cpp

        enum class command { enter, leave, click };

    Ditto.

Public Functions
----------------

.. cpp:function:: label::label()

    The default constructor.

.. cpp:function:: label::label(window wd, bool visible)

    Creates a label.

.. cpp:function:: label::label(window wd, const nana::string& title, \
    bool visible = true)

    Ditto.

.. cpp:function:: label::label(window wd, const nana::char_t* title, \
    bool visible = true)

    Ditto.

.. cpp:function:: label::label(window wd, const rectangle& r = rectangle(), \
    bool visible = true)

    Ditto.

.. cpp:function:: void label::format(bool enter_format_mode)

    Switchs the format mode of the widget.

.. cpp:function:: void label::transparent(bool)

    Switchs the label widget to the transparent background mode.

.. cpp:function:: bool label::transparent() const

    True if the label is transparent background mode.

.. cpp:function:: nana::size label::measure(unsigned limited) const

    Return the size of the text. If the limited is not zero, it returns the size
    that the text changes lines when a line of text beyonds the width specified
    by limited.

C++03
^^^^^

.. warning:: The following definition is not parsed by Sphinx.

.. cpp:function:: void label::add_format_listener(const nana::functor<void(\
    command::t, const nana::string&)> & )

    Adds the format listener.

.. cpp:function:: void label::text_align(nana::align::t, \
    nana::align_v::t = nana::align_v::top)

    Set the text alignment.

C++11
^^^^^

.. warning:: The following definition is not parsed by Sphinx.

.. cpp:function:: void label::add_format_listener(const std::function<void(\
    command, const nana::string&)>&)

    Ditto.

.. warning:: The following definition is not parsed by Sphinx.

.. cpp:function:: void label::add_format_listener(std::function<void(\
    command, const nana::string&)> && )

    Ditto.

.. cpp:function:: void label::text_align(nana::align, \
    nana::align_v = nana::align_v::top)

    Set the text alignment.

Notes
-----

Note 1
^^^^^^

It is not efficient when the background mode of the label is transparent.

Note 2
^^^^^^

If the caption of a label contains a character '\\n', the label will hide the
character and display the text string after '\\n' in a new line.

Note 3
^^^^^^

The format mode accepts the definition for displaying mutile-style text.
For example:

.. code-block:: cpp

    label.format(true);
    label.caption(STR("Hello, <bold=true>This is a bold text</>"));

.. image:: /_static/images/label_bold.png
    :align: center

.. code-block:: cpp

    label.caption(STR("Hello, <bold=true color=0xff0000>This is a bold red text</>"));

.. image:: /_static/images/label_bold_red.png
    :align: center

.. code-block:: cpp

    label.caption(STR("Hello <bold=true color=0xff0000 font=\"Consolas\">This is a bold red Consolas text</>"));

.. image:: /_static/images/label_bold_red_consolas.png
    :align: center

.. code-block:: cpp

    label.caption(STR("Hello, <bold=true color=0xff0000 font=\"Consolas\" url=\"http://nanapro.org\"> This is a bold red Consolas text</>"));

Arrow cursor becomes a hand when the cursor moves over the red text. Meanwhile,
it is open a web browser to open the URL by clicking the red text.
**[NOTE: the url only works under Windows]**

.. code-block:: cpp

    using namespace nana::gui;

    //void listener(label::command cmd, const nana::string& s) //C++11
    void listener(label::command::t cmd, const nana::string& s)
    {
        if(label::command::click == cmd)
        {
            msgbox mb(STR("target clicked"));
            mb<<STR("the target \"")<<s<<"\" is clicked";
            mb();
        }
    }

    int main()
    {
        form fm;
        label lab(fm, nana::rectangle(0, 0, 100, 40));
        lab.format(true);
        lab.add_format_listener(listener);
        lab.caption(STR("Click <color=0xFF target=\"target id\">here</>"));
        fm.show();
        exec();
    }

Note 4
^^^^^^

Some reserved words: **red**, **green**, **blue**, **white**, **black**.
It can simplify the format text and increase the readability.

.. code-block:: cpp

    lab.caption(STR("Click <color=0xFF target=\"target id\">here</>"));

*vs*

.. code-block:: cpp

    lab.caption(STR("Click <blue target=\"target id\">here</>"));

Note 5
^^^^^^

Image is supported for format mode.

.. code-block:: cpp

    lab.caption(STR("<image=\"file.bmp\"><bold red size=20>Nana C++ Library</>"));

As you see, the image tag has not a close-tag **</>** Defaultly, the image is
displayed with its orignal size. With specifying a size, we can get a proper
size by which the image is displayed.

.. code-block:: cpp

    label.caption(STR("<image=\"file.bmp\" size=(150,150)><bold red size=20>Nana C++ Library</>"));

.. image:: /_static/images/label_image.png
    :align: center

**size=(150,150)** means that it stretchs the image to the specified size. In
many situations, we want to display the image as it is if it is greater
than/less than a specified size. There are two reserved words can achieve it.

**max_limited**: stretchs the image to the specified size with preserving aspect
ratio when its one of edge beyonds the specified size.

**min_limited**: stretchs the image to the specified size with preserving aspect
ratio when its one of edge is less than the specified size.

.. code-block:: cpp

    label.caption(STR("<image=\"file.bmp\" size=(150,150) max_limited><bold red size=20>Nana C++ Library</>"));

.. image:: /_static/images/label_image_maxlimited.png
    :align: center

Note 6
^^^^^^

Alignments for format mode.

Defaulty, the alignment is baseline-aligned. The library provides 4 kinds
of alignment which are: **top**, **center**, **bottom** and **baseline**.
They are useful tools when display texts with different size in a line.

.. code-block:: cpp

    label.caption(STR("<size=12 top>top</><size=14 center>center<size=16 bottom>bottom</><size=30>baseline</><size=10>small font by baseline</>"));

.. image:: /_static/images/label_alignment.png
    :align: center

Examples
--------

None.

See Also
--------

None.
