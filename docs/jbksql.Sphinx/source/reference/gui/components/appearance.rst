.. include:: /helpers.rst

.. links
    None

nana::gui::appearance
=====================

.. code-block:: cpp

    nana/gui/basis.hpp

.. cpp:namespace:: nana::gui

This is a structure type defined to specify the appearance of a form.

.. code-block:: cpp

    struct appearance
    {
        struct decoration_desc
        {
            bool empty;
            bool minimize;
            bool maximize;
            bool sizable;

            decoration_desc();
            decoration_desc(bool minimize,
                            bool maximize,
                            bool sizable);
        }decoration;

        bool taskbar;
        bool floating;

        appearance();
        appearance(bool no_decoration,
                    bool taskbar,
                    bool floating);
        appearance(bool min,
                    bool max,
                    bool size,
                    bool show_taskbar,
                    bool floating);
    };

If a form would to be defined without min/max button and sizable border, then

.. code-block:: cpp

    nana::gui::form form(x, y, width, height, nana::gui::appearance(false, false, false, true, false));

This piece of code may be confused because of the 5th parameter of constructor
from :cpp:class:`nana::gui::form <form>`. So the library provides a helper class
for making it easy.

.. code-block:: cpp

    struct appear
    {
        struct minimize{};
        struct maximize{};
        struct sizable{};
        struct taskbar{};
        struct floating{};

        template<typename Param0 = null_type,
                 typename Param1 = null_type,
                 typename Param2 = null_type,
                 typename Param3 = null_type,
                 typename Param4 = null_type>
        struct decorate
        {
            operator appearance() const;
        };

        template<typename Param0 = null_type,
                 typename Param1 = null_type>
        struct bald
        {
            operator appearance() const;
        };

        template<bool HasDecoration,
                 typename Param0 = null_type,
                 typename Param1 = null_type>
        struct optional
        {
            operator appearance() const;
        };
    };

*nana:gui::appear::decorate* is used for creating an appearance of a window
decoration. the window decoration in Windows is called non-client area, such as
title bar.

So, we can create a form without min/max button and sizable border like this:

.. code-block:: cpp

    using nana::gui::appear;
    nana::gui::form form(x, y, width, height, appear::decorate<appear::taskbar>());

.. image:: /_static/images/appearance_01.png
    :align: center

If a window needs a minimize button, it should be:

.. code-block:: cpp

    appear::decorate<appear::minimize, appear::taskbar>()

* The *nana::gui::appear::bald* is used for creating a window without
  decoration.

* The *nana::gui::appear::optional* is used for creating a window with
  decoration depending on the first non-type template parameter.

* The parameter of class template decorate can be *minimize*, *maximize*,
  *sizable*, *taskbar*, *floating*.

* The parameter of class template bald can be *taskbar*, *floating*.

* The parameter of class template optional can be *true*, *false*, *taskbar*,
  *floating*.

Notes
-----

None.

Examples
--------

None.

See Also
--------

* :cpp:class:`form`
