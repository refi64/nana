.. include:: /helpers.rst

.. links
    None

nana::gui::basis
================

.. code-block:: cpp

    nana/gui/basis.hpp

.. cpp:namespace:: nana::gui

There are some predefined value or type for basic construction.


Typedefs
--------

.. cpp:type:: nana::gui::cursor

    .. code-block:: cpp

        enum class cursor

    * **hand** : displays a hand to indicate a text or an element is clickable.
    * **arrow** : the default shape.
    * **hand** : displays a hand to indicate a text or an element is clickable.
    * **arrow** : the default shape.
    * **wait**  : indicates the system currently is busy.
    * **iterm** : displays a caret to indicate the UI is inputable.
    * **size_we**
    * **size_ns**
    * **size_top_left**
    * **size_top_right**
    * **size_bottom_left**
    * **size_bottom_right**

.. cpp:type:: nana::gui::mouse

    .. code-block:: cpp

        enum class mouse

    * **any_button** : none of a mouse button is specified.
    * **left_button** : left mouse button is specified.
    * **middle_button** : middle mouse button is specified.
    * **right_button** : right mouse button is specified.

.. cpp:type:: nana::gui::z_order_action

    .. code-block:: cpp

        enum class z_order_action

    * **none**
    * **bottom** : brings a window at the bottom of z-order.
    * **top** : brings a widget at the top of the z-order.
    * **topmost** : brings a window at the top of the z-order and stays here.
    * **foreground** : brings a window to the foreground.

Notes
-----

None.

Examples
--------

None.

See Also
--------

None.
