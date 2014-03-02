.. include:: /helpers.rst

.. links
    none

nana::gui::date_chooser
=======================

.. code-block:: cpp

    nana/gui/widgets/date_chooser.hpp

.. cpp:namespace:: nana::gui

.. cpp:class:: date_chooser \
    : public widget_object\<widget_tag, DrawerTrigger>

    A graphical user interface widget for choosing a date.

    Inherits:
        * :cpp:class:`widget_object\<widget_tag, DrawerTrigger>`
    Concept:
        * :doc:`Widget Window </reference/gui/concepts/widget_window>`

Public Functions
----------------

.. cpp:function:: date_chooser::date_chooser()

    The default constructor

.. cpp:function:: date_chooser::date_chooser(window wd, bool visible)

    Create a date_chooser.

.. cpp:function:: date_chooser::date_chooser(window wd, \
    const nana::string& title, bool visible = true)

    Ditto.

.. cpp:function:: date_chooser::date_chooser(window wd, \
    const nana::char_t* title, bool visible = true)

    Ditto.

.. cpp:function:: date_chooser::date_chooser(window wd, \
    const rectangle& r = rectangle(), bool visible = true)

    Ditto.

.. cpp:function:: bool date_chooser::chose() const

    Return a state that indicates the user whether chose a date.

.. cpp:function:: nana::date date_chooser::read() const

    Return a date object that indicates what date the user chose.

.. cpp:function:: void date_chooser::weekstr(unsinged index, \
    const nana::string& str)

    Set the week strings which were displayed for day, index is in range of
    [0, 6]

.. cpp:function:: void date_chooser::monthstr(unsigned index, \
    const nana::string& str)

    Set the month strings which were displayed for month. index is in range of
    [0, 11]

Notes
-----

None.

Examples
--------

None.

See Also
--------

* :doc:`date </reference/basic_class/date>`
